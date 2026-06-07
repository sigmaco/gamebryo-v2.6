// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoSkinMap.h"
#include "CrosswalkGamebryoMeshHelpers.h"
#include "CrosswalkGamebryoNodeMap.h"
#include "CrosswalkGamebryoSettings.h"
#include "CrosswalkGamebryoAnimMap.h"

#include "Model.h"
#include "XSITriangleList.h"
#include "XSITransform.h"

#include "NiMesh.h"
#include "NiDataStream.h"
#include "NiCommonSemantics.h"
#include "NiSkinningMeshModifier.h"
#include "NiNode.h"
#include "NiOptimize.h"

namespace
{
    //---------------------------------------------------------------------------
    // Create the bone index stream.
    //---------------------------------------------------------------------------
    bool CreateBoneStreams(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSITriangleList& in_Triangles,
        const epg::BonesDescriptor& in_Bones,
        const epg::SkinDescriptor& in_Skin,
        int in_HighBoneIndex,
        NiMesh& io_Mesh)
    {
        static const int MAX_NIUINT8 = (1 << 8) - 1;
        static const int MAX_NIINT16 = (1 << 15) - 1;

        std::vector<float> boneWeights;
        std::vector<NiUInt8> smallBoneIndices;
        std::vector<NiInt16> largeBoneIndices;

        if (in_HighBoneIndex > MAX_NIINT16)
        {
            return io_Context.Logf(
                epg::LOG_ERROR,
                "Too many bones (%d) used to skin \"%s\", only %d bones supported.",
                in_HighBoneIndex,
                in_Model.Name().GetText(),
                MAX_NIINT16);
        }

        const int triangleCount = in_Triangles.GetTriangleCount();
        static const int VERTICES_PER_TRIANGLE = 3;
        static const int BONES_PER_VERTEX = epg::SkinDescriptor::MAX_BONES_PER_VERTEX;
        static const int WEIGHTS_PER_VERTEX = 4;

        static const bool canUseNiUInt8 = false;

        boneWeights.resize(triangleCount * VERTICES_PER_TRIANGLE * WEIGHTS_PER_VERTEX, 0.0f);
        if (canUseNiUInt8)
            smallBoneIndices.resize(triangleCount * VERTICES_PER_TRIANGLE * BONES_PER_VERTEX, 0);
        else
            largeBoneIndices.resize(triangleCount * VERTICES_PER_TRIANGLE * BONES_PER_VERTEX, 0);

        // Note: if the XSI shape is in  mode XSI_INDEXED instead of ORDERED,
        //       this code might be wrong. It's hard to say: if the shape is
        //       XSI_INDEXED, are the vertex-index in the XSI envelope final
        //       vertex indices or are they referencing into the XSI_INDEXED
        //       attribute and thus must be further de-indexed? We have never
        //       seen such a file, so we can't say.
        epg::CSLIntArray* pIndicesArray = in_Triangles.GetVertexIndices();
        if (!pIndicesArray)
            return true;
        SI_Int* pIndices = pIndicesArray->ArrayPtr();
        if (!pIndices)
            return true;

        int badBoneCount = 0;

        for (int it = 0; it < triangleCount; ++it)
        {
            const int triangleBasedOffset = it * VERTICES_PER_TRIANGLE;
            for (int iv = 0; iv < VERTICES_PER_TRIANGLE; ++iv)
            {
                const int vertexBasedOffset = triangleBasedOffset + iv;
                NIASSERT(vertexBasedOffset < pIndicesArray->GetUsed());

                const int vertexIndex = pIndices[vertexBasedOffset];

                // Accumulate the number of bones and their total weight
                // to be able to correct degenerate cases.
                float total = 0.0f;
                int vertexBoneIndex = 0;

                // Add the vertex bones and bone-weights to their respective stream.
                typedef epg::BonesDescriptor::BoneWeights BoneWeights;
                if (const BoneWeights* weights = in_Bones.FindVertexBones(vertexIndex))
                {
                    typedef BoneWeights::const_iterator iter;
                    for (iter pos = weights->begin();
                        vertexBoneIndex < WEIGHTS_PER_VERTEX && pos != weights->end();
                        ++pos, ++vertexBoneIndex)
                    {
                        const int boneValueIndex = vertexBasedOffset * WEIGHTS_PER_VERTEX
                                                 + vertexBoneIndex;
                        assert((size_t)boneValueIndex < boneWeights.size());

                        total += pos->weight;
                        boneWeights[boneValueIndex] = pos->weight;

                        if (canUseNiUInt8)
                        {
                            NIASSERT((size_t)boneValueIndex < smallBoneIndices.size());
                            smallBoneIndices[boneValueIndex] = in_Skin.GetBoneIndex(*pos->bone);
                        }
                        else
                        {
                            NIASSERT((size_t)boneValueIndex < largeBoneIndices.size());
                            largeBoneIndices[boneValueIndex] = in_Skin.GetBoneIndex(*pos->bone);
                        }
                    }
                }

                // Verify degenerate case: no bone or no weight.
                if (0 == vertexBoneIndex || 0.0f == total)
                {
                    // Not a single bone... Gamebryo doesn't seem to mind after all.
                    // Don't try to normalize with zero as divisor.
                    ++badBoneCount;
                }
                else
                {
                    // Note: the value of 0.8 (80%) is pretty arbitrary...
                    static const float MINIMUM_VALID_TOTAL_WEIGHT = 0.8f;
                    if (vertexBoneIndex == WEIGHTS_PER_VERTEX
                    &&  total < MINIMUM_VALID_TOTAL_WEIGHT)
                    {
                        ++badBoneCount;

                        io_Context.Logf(
                            epg::LOG_WARNING,
                            "The 4 heaviest bones used to skin vertex #%d"
                            " of \"%s\" add up to %d%% which is less than"
                            " %d%% of the total weight, skinning may look incorrect.",
                            vertexIndex+1,
                            in_Model.Name().GetText(),
                            (int) (total * 100.0f + 0.5f),
                            (int) (MINIMUM_VALID_TOTAL_WEIGHT * 100.0f));
                    }

                    // Re-normalize the weights to add-up to 1.0 if needed.
                    for (int ib = 0; ib < vertexBoneIndex; ++ib)
                    {
                        const int boneValueIndex = vertexBasedOffset * WEIGHTS_PER_VERTEX + ib;
                        boneWeights[boneValueIndex] /= total;
                    }
                }
            }
        }

        if (badBoneCount > 0)
        {
            io_Context.Logf(
                epg::LOG_WARNING,
                "Found %d vertices with potentially bad bone assignment in \"%s\".",
                badBoneCount,
                in_Model.Name().GetText());
        }

        io_Mesh.AddStream(
            NiCommonSemantics::BLENDWEIGHT(),
            0,
            NiDataStreamElement::F_FLOAT32_4,
            boneWeights.size() / WEIGHTS_PER_VERTEX,
            NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX,
            &(boneWeights[0]));

        if (canUseNiUInt8)
            io_Mesh.AddStream(
                NiCommonSemantics::BLENDINDICES(),
                0,
                NiDataStreamElement::F_UINT8_4,
                smallBoneIndices.size() / BONES_PER_VERTEX,
                NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
                NiDataStream::USAGE_VERTEX,
                &(smallBoneIndices[0]));
        else
            io_Mesh.AddStream(
                NiCommonSemantics::BLENDINDICES(),
                0,
                NiDataStreamElement::F_INT16_4,
                largeBoneIndices.size() / BONES_PER_VERTEX,
                NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
                NiDataStream::USAGE_VERTEX,
                &(largeBoneIndices[0]));

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert the mesh skinnable streams into skin streams.
    //---------------------------------------------------------------------------
    bool ConvertSkinStreams(
        epg::Context& io_Context,
        CSLModel& in_Model,
        NiMesh& io_Mesh)
    {
        const NiFixedString skinnableSemantics[] =
        {
            NiCommonSemantics::POSITION(),
            NiCommonSemantics::NORMAL(),
            NiCommonSemantics::BINORMAL(),
            NiCommonSemantics::TANGENT()
        };

        const NiFixedString bindPoseSemantics[] =
        {
            NiCommonSemantics::POSITION_BP(),
            NiCommonSemantics::NORMAL_BP(),
            NiCommonSemantics::BINORMAL_BP(),
            NiCommonSemantics::TANGENT_BP()
        };

        for (int i = 0; i < sizeof(skinnableSemantics)/sizeof(skinnableSemantics[0]); ++i)
        {
            NiDataStreamRef* pStreamRef = io_Mesh.FindStreamRef(skinnableSemantics[i]);
            if (!pStreamRef)
                continue;
            NiDataStream* pStream = pStreamRef->GetDataStream();
            if (!pStream)
                continue;

            NiDataStream* pSkinStream = pStream->CreateFullCopyEx(
                NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
                NiDataStream::USAGE_VERTEX,
                true, true);
            if (!pSkinStream)
            {
                return io_Context.Logf(
                    epg::LOG_ERROR,
                    "Cannot create skin stream for \"%s\".",
                    in_Model.Name().GetText());
            }
            io_Mesh.AddStreamRef(pSkinStream, bindPoseSemantics[i]);

            if (!SetStreamFlags(
                io_Context,
                in_Model,
                io_Mesh,
                skinnableSemantics[i],
                (NiDataStream::Access)
                (NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_MUTABLE),
                NiDataStream::USAGE_VERTEX))
                return false;
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Compute the world transform of the base pose.
    //---------------------------------------------------------------------------
    NiTransform& ComputeBasePoseWorldTransform(
        const epg::NodeMap& in_NodeMap,
        const NiAVObject& in_Object,
        NiTransform& out_WorldTransform)
    {
        out_WorldTransform.MakeIdentity();

        for (const NiAVObject* pNode = &in_Object; pNode; pNode = pNode->GetParent())
        {
            NiAVObject* pBasePose = in_NodeMap.FindBasePose(*pNode);
            if (pBasePose)
            {
                // Whenever we find that a node in the scene hierarchy has a base pose
                // (AKA bind pose in Gamebryo parlance), we simply use the base pose
                // transform directly and stop climbing up to the scene root (world-space)
                // since the one given in Crosswalk is already in world-space.
                // (Maybe, just maybe, they'd like to actually *document* this fact?)
                while (pBasePose)
                {
                    out_WorldTransform = pBasePose->GetLocalTransform() * out_WorldTransform;
                    pBasePose = pBasePose->GetParent();
                }

                break;
            }

            out_WorldTransform = pNode->GetLocalTransform() * out_WorldTransform;
        }

        return out_WorldTransform;
    }

    //---------------------------------------------------------------------------
    // Update the modifiers. For some reason, skin animation doesn't work without this.
    //---------------------------------------------------------------------------
    void CompleteSceneModifiers(epg::NodeMap& in_NodeMap)
    {
        epg::NodeMap::RootObjects& roots = in_NodeMap.GetRoots();
        typedef epg::NodeMap::RootObjects::iterator iter;
        for (iter pos = roots.begin(); pos != roots.end(); ++pos)
        {
            NiAVObject* pObj = *pos;
            if (!pObj)
                continue;
            NiMesh::CompleteSceneModifiers(pObj);
        }
    }

    //---------------------------------------------------------------------------
    // Find and set the root bone parent and its associated root-parent-to-skin transform.
    //---------------------------------------------------------------------------
    bool SetRootBoneParent(
        epg::Context& io_Context,
        const epg::SkinDescriptor& in_Skin,
        const epg::NodeMap& in_NodeMap,
        NiMesh& in_Mesh,
        NiSkinningMeshModifier& io_SkinMod)
    {
        CSLModel* pRootBoneParent = in_Skin.FindRootBoneParent();
        NiAVObject* pParent = pRootBoneParent ? in_NodeMap.FindObject(*pRootBoneParent) : 0;
        // Make sure the root bone parent is also a parent of the skin otherwise
        // the skin-to-bone and bone-to-skin will be incorrect.
        while (pParent)
        {
            NiNode *pMeshRoot = in_Mesh.GetParent();
            while (pMeshRoot && pMeshRoot != pParent)
                pMeshRoot = pMeshRoot->GetParent();
            if (pMeshRoot == pParent)
                break;

            pParent = pParent->GetParent();
        }

        if (!pParent)
        {
            return io_Context.Logf(
                epg::LOG_ERROR,
                "Cannot find the root bone parent in skin \"%s\".",
                (const char *)in_Mesh.GetName());
        }

        io_SkinMod.SetRootBoneParent(pParent);
        // The parent-to-skin is calculated as follow.
        //
        // We have:
        //    - skin-to-world
        //    - parent-to-world
        //
        // We need:
        //    - parent-to-skin
        //
        // We do:
        //    - world-to-skin = inverse(skin-to-world)
        //    - parent-to-skin = world-to-skin x parent-to-world
        NiTransform worldTransform;
        NiTransform worldToSkin;
        ComputeBasePoseWorldTransform(in_NodeMap, in_Mesh, worldTransform).Invert(worldToSkin);
        io_SkinMod.SetRootBoneParentToSkinTransform(
            worldToSkin * ComputeBasePoseWorldTransform(in_NodeMap, *pParent, worldTransform));

        return true;
    }

    //---------------------------------------------------------------------------
    // Calculate and set the proper skin-to-bone transform for the given bone.
    //---------------------------------------------------------------------------
    bool SetSkinToBoneTransform(
        epg::Context& io_Context,
        const epg::NodeMap& in_NodeMap,
        CSLModel& in_XSIBone,
        const NiMesh& in_Mesh,
        NiTransform& io_SkinToBoneTransform)
    {
        // The skin-to-bone is calculated as follow.
        //
        // We have:
        //    - skin-to-world
        //    - bone-to-world
        //
        // We need:
        //    - skin-to-bone
        //
        // We do:
        //    - world-to-bone = inverse(bone-to-world)
        //    - skin-to-bone  = world-to-bone x skin-to-world
        NiNode * pBone = NiDynamicCast(NiNode,in_NodeMap.FindObject(in_XSIBone));
        if (!pBone)
        {
            return io_Context.Logf(
                epg::LOG_ERROR,
                "Cannot find bone transform root \"%s\" in skin \"%s\".",
                in_XSIBone.Name().GetText(),
                (const char *)in_Mesh.GetName());
        }

        NiTransform worldTrans;
        NiTransform worldToBone;
        ComputeBasePoseWorldTransform(in_NodeMap, *pBone, worldTrans).Invert(worldToBone);
        io_SkinToBoneTransform = worldToBone
                               * ComputeBasePoseWorldTransform(in_NodeMap, in_Mesh, worldTrans);

        return true;
    }

    //---------------------------------------------------------------------------
    // Fill skinning modifier data for one skin.
    //---------------------------------------------------------------------------
    bool FillSkinningModifier(
        epg::Context& io_Context,
        NiMesh& in_Mesh,
        const epg::SkinDescriptor& in_Skin)
    {
        // Note: the skinning modifier and skin should never be null
        //       unless the skin convesion failed. Normally, when it
        //       fails the conversion should end but we prefer to check
        //       rather than crash.
        NiSkinningMeshModifier* pSkinMod = in_Skin.m_pSkinModifier;
        if (!pSkinMod)
            return true;

        epg::NodeMap& nodeMap = io_Context.GetNodeMap();

        if (!SetRootBoneParent(io_Context, in_Skin, nodeMap, in_Mesh, *pSkinMod))
            return false;

        NiAVObject** pBoneArray = pSkinMod->GetBones();
        NiTransform* transformArray = pSkinMod->GetSkinToBoneTransforms();

        typedef epg::SkinDescriptor::BoneIndices BoneIndices;
        typedef BoneIndices::const_iterator iter;
        const BoneIndices& indices = in_Skin.GetBoneIndices();
        for (iter pos = indices.begin(); pos != indices.end(); ++pos)
        {
            CSLModel* pXSIBone = pos->first;
            NiAVObject* pGBBone = nodeMap.FindObject(*pXSIBone);
            if (!pGBBone)
            {
                return io_Context.Logf(
                    epg::LOG_ERROR,
                    "Cannot find bone \"%s\" in skin \"%s\".",
                    pXSIBone->Name().GetText(),
                    (const char *)in_Mesh.GetName());
            }

            const int boneIndex = pos->second;
            NIASSERT((size_t)boneIndex < indices.size());
            pBoneArray[boneIndex] = pGBBone;
            if (!SetSkinToBoneTransform(
                io_Context,
                nodeMap,
                *pXSIBone,
                in_Mesh,
                transformArray[boneIndex]))
            {
                return false;
            }
        }

        if (!NiOptimize::CalculateBoneBounds(&in_Mesh))
        {
            return io_Context.Logf(
                epg::LOG_ERROR,
                "Bounding box calculation failed for skin \"%s\". Object might get clipped.",
                (const char *)in_Mesh.GetName());
        }

        return true;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Fill skinning modifier data for all skins:
    //     - the bone array,
    //     - the skin to bone transform array,
    //     - the root bone parent.
    //     - the root bone parent to skin transform.
    //---------------------------------------------------------------------------
    bool FillSkinningModifiers(Context& io_Context)
    {
        if (!io_Context.GetSettings().GetExportSkinnings())
            return true;

        // Now we can fill the skinning modifiers.
        typedef SkinMap::Skins Skins;
        typedef Skins::iterator iter;
        Skins& skins = io_Context.GetSkinMap().GetSkins();
        Phaser phase(io_Context, FILL_SKINS_PHASE, skins.size());
        for (iter pos = skins.begin(); pos != skins.end(); ++pos)
        {
            phase.Progress();

            NiMesh* pMesh = pos->first;
            if (!pMesh)
                continue;

            const SkinDescriptor& skin = pos->second;
            if (!FillSkinningModifier(io_Context, *pMesh, skin))
                return false;
        }

        CompleteSceneModifiers(io_Context.GetNodeMap());

        return true;
    }

    //---------------------------------------------------------------------------
    // Conversion of skin and bones.
    //---------------------------------------------------------------------------
    bool ConvertSkin(
        Context& io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_SkinShape,
        CSLXSITriangleList& in_Triangles,
        NiMesh& io_Mesh)
    {
        if (!io_Context.GetSettings().GetExportSkinnings())
            return true;

        BonesDescriptor* pBones = io_Context.GetSkinMap().FindBones(in_Model);
        if (!pBones)
            return true;

        SkinDescriptor& kSkin = io_Context.GetSkinMap().AddSkin(io_Mesh);
        const int highBoneIndex = kSkin.AssignBoneIndices(io_Context, in_Model, *pBones);
        if (highBoneIndex < 0)
            return true;

        if (!CreateBoneStreams(
            io_Context,
            in_Model,
            in_Triangles,
            *pBones,
            kSkin,
            highBoneIndex,
            io_Mesh))
        {
            return false;
        }

        if (!ConvertSkinStreams(io_Context, in_Model, io_Mesh))
            return false;

        kSkin.m_pSkinModifier = NiNew NiSkinningMeshModifier(highBoneIndex+1);
        kSkin.m_pSkinModifier->SetSoftwareSkinned(true);
        io_Mesh.AddModifier(kSkin.m_pSkinModifier);

        // Set the animation update flags.
        io_Mesh.SetSelectiveUpdate(true);
        io_Mesh.SetSelectiveUpdateTransforms(true);
        io_Mesh.SetSelectiveUpdateRigid(false);

        io_Context.GetAnimationMap().AddAnimatedObject(io_Mesh);

        return true;
    }

    //---------------------------------------------------------------------------
}
