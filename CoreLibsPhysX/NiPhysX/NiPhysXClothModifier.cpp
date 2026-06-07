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

// Precompiled Header
#include "NiPhysXPCH.h"

#include "NiPhysXClothModifier.h"
#include "NiPhysXScene.h"
#include "NiPHysXSyncArgs.h"
#include "NiPhysXTypes.h"
#include "NiNode.h"

#include <NiPoint2.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', 
                               // possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', 
                               // signed/unsigned mismatch
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

//---------------------------------------------------------------------------
NiImplementRTTI(NiPhysXClothModifier, NiPhysXMeshModifier);
//---------------------------------------------------------------------------

NiUInt16 NiPhysXClothModifier::ms_uiInvalidVertexCount = 0;

//---------------------------------------------------------------------------
NiPhysXClothModifier::NiPhysXClothModifier() : NiPhysXMeshModifier(),
    m_kSplitParentMap(17), m_pkCloth(0), m_fMeshGrowFactor(0.0),
    m_puiVertexMap(0), m_pkScene(0), m_pkParentActor(0),
    m_bUpdatedPositionData(false), m_uiPreviousVertexCount(0), 
    m_uiOriginalVertexCount(ms_uiInvalidVertexCount),
    m_uiNBTSet(0), m_bForceCached(false)
{
    m_kClothData.verticesPosBegin = 0;
    m_kClothData.numVerticesPtr = 0;
    m_kClothData.indicesBegin = 0;
    m_kClothData.numIndicesPtr = 0;
    m_kClothData.parentIndicesBegin = 0;
    m_kClothData.numParentIndicesPtr = 0;
    m_kClothData.verticesNormalBegin = 0;

    m_kWorldXform.MakeIdentity();
    m_kXform.MakeIdentity();
    
    m_uFlags = ACTIVE_MASK;
}
//---------------------------------------------------------------------------
NiPhysXClothModifier::~NiPhysXClothModifier()
{
    DeallocateClothData();
}
//---------------------------------------------------------------------------
bool NiPhysXClothModifier::IsRequirementMet(NiMesh* pkMesh,
    const NiFixedString& kSemantic, bool bMustExist,
    NiUInt8 uiReadMask, NiUInt8 uiWriteMask,
    const char* acWriteDesc, const char* acCondition,
    NiDataStreamElement::Format kFormat) const
{
    NiDataStreamRef* pkDSRef = 0;

    pkDSRef = pkMesh->FindStreamRef(kSemantic);
    if (!pkDSRef)
    {
        if (bMustExist)
        {
            char acMsg[256];
            NiSprintf(acMsg, 256,
                "NiPhysXClothModifier: The %s data stream must exist%s.\n",
                (const char*)kSemantic, acCondition);
            NILOG(acMsg);
            return false;
        }
        else
        {
            // Doesn't exist, but doesn't need to, so return.
            return true;
        }
    }
    
    NiUInt8 uiAccessMask = pkDSRef->GetAccessMask();
    if (!((uiAccessMask & uiReadMask) && (uiAccessMask & uiWriteMask)))
    {
        char acMsg[256];
        NiSprintf(acMsg, 256,
            "NiPhysXClothModifier: The %s data stream must "
            "be CPU readable%s data%s.\n",
            (const char*)kSemantic, acWriteDesc, acCondition);
        NILOG(acMsg);
        return false;
    }

    if (kFormat != NiDataStreamElement::F_UNKNOWN)
    {
        NiDataStreamElement kMeshFormat = pkDSRef->GetElementDescAt(0);
        if (kMeshFormat.GetFormat() != kFormat)
        {
            NILOG("NiPhysXClothModifier: Incorrect format on semantic %s.\n",
                (const char*)kSemantic);
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXClothModifier::AreRequirementsMet(NiMesh* pkMesh, 
    NiSystemDesc::RendererID) const
{
    if (!IsRequirementMet(pkMesh, NiCommonSemantics::POSITION(), true,
        NiDataStream::ACCESS_CPU_READ,
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE |
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        " and writable", "", NiDataStreamElement::F_FLOAT32_3))
    {
        return false;
    }

    if (m_fMeshGrowFactor > 1.0f || GetTearableHint())
    {
        // Mesh grow > 1.0 indicates tearing, and we need to be able to
        // copy vertex data for torn vertices and update index data.
        NiUInt32 uiStreamCount = pkMesh->GetStreamRefCount();
        for (NiUInt32 uiStreamIndex = 0; uiStreamIndex < uiStreamCount; 
            uiStreamIndex++)
        {
            NiDataStreamRef* pkDSRef = pkMesh->GetStreamRefAt(uiStreamIndex);
            NiUInt8 uiAccessMask = pkDSRef->GetAccessMask();

            if (pkDSRef->GetUsage() == NiDataStream::USAGE_VERTEX)
            {
                if (!((uiAccessMask & NiDataStream::ACCESS_CPU_READ) &&
                    (uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_MUTABLE)))
                {
                    NILOG("NiPhysXClothModifier: "
                        "Streams with usage USAGE_VERTEX must be CPU READABLE "
                        "and CPU WRITE MUTABLE when vertices can tear.\n");
                    return false;
                }
            }
        }

        if (!IsRequirementMet(pkMesh, NiCommonSemantics::INDEX(), true,
            NiDataStream::ACCESS_CPU_READ,
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            " and mutable", " when vertices can tear"))
        {
            return false;
        }
    }
    
    if (GetUpdateNormals())
    {
        if (!IsRequirementMet(pkMesh, NiCommonSemantics::INDEX(), true,
            NiDataStream::ACCESS_CPU_READ, 0xFF,
            "", " when normals are to be recomputed"))
        {
            return false;
        }

        if (!IsRequirementMet(pkMesh, NiCommonSemantics::NORMAL(), true,
            NiDataStream::ACCESS_CPU_READ,
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE |
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
            " and writable", " when normals are to be recomputed"))
        {
            return false;
        }

        if (!IsRequirementMet(pkMesh, NiCommonSemantics::BINORMAL(), false,
            NiDataStream::ACCESS_CPU_READ,
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE |
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
            " and writable", " when normals are to be recomputed"))
        {
            return false;
        }

        if (!IsRequirementMet(pkMesh, NiCommonSemantics::TANGENT(), false,
            NiDataStream::ACCESS_CPU_READ,
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE |
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
            " and writable", " when normals are to be recomputed"))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::RetrieveRequirements(
    NiMeshRequirements& kRequirements) const
{
    // Two sets of requirements, one with NBT the other just N. We use hints
    // to decide what to do. We can't set requirements for the tearing case
    // because there is no way to set the requirement on all USAGE_VERTEX
    // and USAGE_INDEX streams.

    NiUInt32 uiReqIndex;

    NiUInt32 uiSet1Index = kRequirements.CreateNewRequirementSet();
    NiUInt32 uiStreamID = 1;

    NiUInt32 uiSet2Index = (NiUInt32)~0;
    NI_UNUSED_ARG(uiSet2Index);
    NiMeshRequirements::SemanticRequirement* pkIndexReq = 0;
    NiMeshRequirements::SemanticRequirement* pkPosnReq = 0;

    // Required access depends on tearable.
    NiUInt8 uiTearableAccess = GetTearableHint() ?
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE :
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE;

    uiReqIndex = kRequirements.AddRequirement(uiSet1Index,
        NiCommonSemantics::POSITION(), 0,
        NiMeshRequirements::CAN_SPLIT,
        uiStreamID++, NiDataStream::USAGE_VERTEX, uiTearableAccess,
        NiDataStreamElement::F_FLOAT32_3);
    pkPosnReq = kRequirements.GetRequirement(uiSet1Index, uiReqIndex);

    // Only need index write access if tearing
    if (GetTearableHint())
    {
        uiReqIndex = kRequirements.AddRequirement(uiSet1Index,
            NiCommonSemantics::INDEX(), 0,
            NiMeshRequirements::CAN_SPLIT,
            uiStreamID++, NiDataStream::USAGE_VERTEX_INDEX, uiTearableAccess,
            NiDataStreamElement::F_UINT16_1);
        pkIndexReq = kRequirements.GetRequirement(uiSet1Index, uiReqIndex);
    }
    
    if (GetUpdateNormals())
    {
        // Create an additional set:
        // - Set 1: Index, normal, binormal/tangent.
        // - Set 2: Index, normal
        NiUInt32 uiSet2Index2 = kRequirements.CreateNewRequirementSet();

        kRequirements.AddRequirement(uiSet2Index2, pkPosnReq);

        // Indices
        if (!pkIndexReq)
        {
            uiReqIndex = kRequirements.AddRequirement(uiSet1Index,
                NiCommonSemantics::INDEX(), 0, NiMeshRequirements::CAN_SPLIT,
                uiStreamID++, NiDataStream::USAGE_VERTEX_INDEX, NiDataStream::
                ACCESS_CPU_READ, NiDataStreamElement::F_UNKNOWN);
            pkIndexReq = kRequirements.GetRequirement(uiSet1Index, uiReqIndex);
            kRequirements.AddRequirement(uiSet2Index2, pkIndexReq);
        }
        else
        {
            kRequirements.AddRequirement(uiSet2Index, pkIndexReq);
        }

        NiMeshRequirements::SemanticRequirement* pkReq = 0;

        // Normal
        uiReqIndex = kRequirements.AddRequirement(uiSet1Index,
            NiCommonSemantics::NORMAL(), 0, NiMeshRequirements::CAN_SPLIT,
            uiStreamID++, NiDataStream::USAGE_VERTEX, uiTearableAccess, 
            NiDataStreamElement::F_UNKNOWN);
        pkReq = kRequirements.GetRequirement(uiSet1Index, uiReqIndex);
        kRequirements.AddRequirement(uiSet2Index, pkIndexReq);

        // Binormal
        kRequirements.AddRequirement(uiSet1Index,
            NiCommonSemantics::BINORMAL(), 0, NiMeshRequirements::CAN_SPLIT,
            uiStreamID++, NiDataStream::USAGE_VERTEX, uiTearableAccess, 
            NiDataStreamElement::F_UNKNOWN);

        // Tangent
        kRequirements.AddRequirement(uiSet1Index,
            NiCommonSemantics::TANGENT(), 0, NiMeshRequirements::CAN_SPLIT,
            uiStreamID++, NiDataStream::USAGE_VERTEX, uiTearableAccess, 
            NiDataStreamElement::F_UNKNOWN);
    }

}
//---------------------------------------------------------------------------
bool NiPhysXClothModifier::Attach(NiMesh* pkMesh)
{
    if (!m_pkCloth)
        return false;

    // Only support one submesh this to make tractable the handling of 
    // tearing cloth.
    NIASSERT(pkMesh->GetSubmeshCount() == 1);
    
    // We will only support PRIMITIVE_TRIANGLES, because we cannot 
    // stripify on the fly as mesh connectivity changes.
    NIASSERT(pkMesh->GetPrimitiveType() == 
        NiPrimitiveType::PRIMITIVE_TRIANGLES);

    // Do not set any sync points until we are associated with a PhysX scene.
    
    // Retrieve the datastreamref with the semmantic POSITION.
    NiDataStreamRef* pkOutputPositionsDSRef = pkMesh->FindStreamRef(
        NiCommonSemantics::POSITION());
    if (!pkOutputPositionsDSRef)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXClothModifier::Detach(NiMesh* pkMesh)
{
    NI_UNUSED_ARG(pkMesh);
    NIASSERT(pkMesh);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXClothModifier::SubmitTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs,
    NiSPWorkflowManager* pkWFManager)
{
    NI_UNUSED_ARG(pkWFManager);
    m_bUpdatedPositionData = false;
    if (!m_pkCloth || !m_pkScene)
    {
        return false;
    }
    
    bool bAsyncSim = m_pkScene->GetAsynchronousSimulation();

    if (pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_PHYSICS_COMPLETED)
    {
        NiPhysXSyncArgs* pkSyncArgs = (NiPhysXSyncArgs*)pkArgs;
        m_bForceCached = pkSyncArgs->m_bForce;
        if (!m_bForceCached && (!GetActive() || bAsyncSim))
        {
            return false;
        }
    }
    else
    {
        NIASSERT(pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_VISIBLE);
        if (m_bForceCached)
        {             
            // Turn off the forced cache, in case we get called again without
            // the workflow completing.
            m_bForceCached = false;
            
            return false;
        }
        
        if (!GetActive())
            return false;
    }

    if (m_pkCloth->isSleeping())
    {
        if (GetSleeping())
        {
            // Code path if the cloth is asleep and we have updated already
            if (GetDoSleepXforms())
            {
                NiAVObject* pkParent = pkMesh->GetParent();
                if (pkParent)
                {
                    // Figure out where we want the node to be in the world.
                    NiTransform kTarget =
                        m_pkScene->GetSceneXform() * m_kWorldXform;
                    
                    // Get the parent's inverse
                    NiTransform kParentInv;
                    pkParent->GetWorldTransform().Invert(kParentInv);
                    
                    // Get the offset transform to make the new pose match
                    // the old one
                    NiTransform kOffset = kTarget * kParentInv;
                        
                    pkMesh->SetRotate(kOffset.m_Rotate);
                    pkMesh->SetTranslate(kOffset.m_Translate);
                    pkMesh->SetScale(kOffset.m_fScale);
                }
                else
                {
                    // We need the global transform such that the node
                    // ends up at m_kWorldXform in PhysX space
                    NiTransform kOffset =
                        m_pkScene->GetSceneXform() * m_kWorldXform;
                    pkMesh->SetRotate(kOffset.m_Rotate);
                    pkMesh->SetTranslate(kOffset.m_Translate);
                    pkMesh->SetScale(kOffset.m_fScale);
                }
            }
            return false;
        }
        else
        {
            // First sleeping frame
            if (GetDoSleepXforms())
            {
                // While the cloth is sleeping, is it presumably static in
                // the PhysX coordinate system. Hence, we need to keep the
                // target Gamebryo node in a fixed location in the PhysX
                // coordinate system. So record that location.
                NiTransform kInvRootTransform;
                m_pkScene->GetSceneXform().Invert(kInvRootTransform);
                const NiTransform kSrcXform = pkMesh->GetWorldTransform();
                m_kWorldXform = kInvRootTransform * kSrcXform;
            }

            SetSleeping(true);
        }
    }
    else
    {
        SetSleeping(false);
    }

    // 1. Compute the transformation from PhysX global coordinates,
    //    which is the reference system for the NxMeshData object, to
    //    Gamebryo scene graph local coordinates. If the actor parent is
    //    present, we obtain its PhysX pose and use the parent transform
    //    to compute the relative pose between PhysX global coordinates
    //    and each target mesh. If the actor parent is not set, we use the
    //    target’s world transformation in combination with the PhysX root
    //    transform passed as an argument to UpdateFromActors. This code is
    //    very similar to the existing rigid-body code.
    m_kXform.MakeIdentity();
    if (m_pkParentActor)
    {
        // X(Geom<-Cloth) = X(Geom<-Cloth)
        // = X(Geom<-Parent) X(PhysXParent<-Cloth)
        // = X(Parent<-Geom)^-1 X(PhysXScene<-Parent)^-1
        
        // Get the parent's transform, inverted
        NxMat34 kPose = m_pkParentActor->getGlobalPose();
        NiTransform kParent;
        NiPhysXTypes::NxMat34ToNiTransform(kPose, kParent.m_Rotate, 
            kParent.m_Translate);
        kParent.m_fScale = 1.0f / m_pkScene->GetScaleFactor();

        // Get the target's local transform
        NiTransform kTargetLocal;
        kTargetLocal.m_Rotate = pkMesh->GetRotate();
        kTargetLocal.m_Translate = pkMesh->GetTranslate();
        kTargetLocal.m_fScale = pkMesh->GetScale();

        // We have all we need
        (kParent * kTargetLocal).Invert(m_kXform);
    }
    else
    {
        // X(Geom<-PhysX) = X(Geom<-World) X(World<-PhysX)
        //                = X(World<-Geom)^-1 X(World<-PhysX)
        
        // Get the target's world transform: X(Geom<-World)
        NiTransform kTargetWorld;
        kTargetWorld.m_Rotate = pkMesh->GetWorldRotate();
        kTargetWorld.m_Translate = pkMesh->GetWorldTranslate();
        kTargetWorld.m_fScale = pkMesh->GetWorldScale();
        NiTransform kTargetInv;
        kTargetWorld.Invert(kTargetInv);
        
        // Get a transform to take PhysX to world
        NiTransform kPhysXWorld = m_pkScene->GetSceneXform();
        kPhysXWorld.m_fScale *= m_pkScene->GetScaleFactor();

        m_kXform = kTargetInv * kPhysXWorld;
    }

    // 2. Process any tears. This means updating the vertex counts, copying
    //    texture coordinates, colors and updating triangle index information.
    //    We make a major assumption here, enforced by the code that
    //    reallocates the Gamebryo mesh data to match the PhysX data:
    //    - There is a 1-1 correspondence between triangles, including
    //      vertex order.
    NiUInt32 uiNumNxVerts = m_kClothData.numVerticesPtr[0];
    if (uiNumNxVerts > m_uiPreviousVertexCount)
    {
        if (m_uiPreviousVertexCount == pkMesh->GetVertexCount())
        {
            // 1-1 mapping between GB vertices and Px vertices
            ProcessBasicTearing(pkMesh);
        }
        else
        {
            // Many-1 mapping between GB vertices and Px vertices
            ProcessGeneralTearing(pkMesh);
        }
    }
    else if (uiNumNxVerts < m_uiPreviousVertexCount)
    {
        // Typically, this means we have not taken a simulation step
        // to get any mesh data.
        return false;
    }

    m_uiPreviousVertexCount = (NiUInt16)uiNumNxVerts;

    // 3. Transform all of the vertices from the NxMeshData into the
    //    NiGeometryData. 
    NiUInt16* pusVertexMap = m_puiVertexMap;
    NxVec3* pkPositionsInput = (NxVec3*)m_kClothData.verticesPosBegin;

    // Retrieve the datastreamref with the semmantic POSITION.
    NiDataStreamRef* pkOutputPositionsDSRef = pkMesh->FindStreamRef(
        NiCommonSemantics::POSITION());
    if (!pkOutputPositionsDSRef)
        return false;

    NIASSERT(pkOutputPositionsDSRef->GetDataStream());
    NiPoint3* pkPositionsOutput = 
        (NiPoint3*)pkOutputPositionsDSRef->GetDataStream()->Lock(
        NiDataStream::LOCK_WRITE);

    // Transform the vertices and write the result to the output stream.
    NiUInt16 uiVertCount = (NiUInt16)pkMesh->GetVertexCount();
    for (NiUInt16 us = 0; us < uiVertCount; us++)
    {
        NxVec3& kNxPos = pkPositionsInput[pusVertexMap[us]];
        NiPoint3 kV(kNxPos.x, kNxPos.y, kNxPos.z);

        pkPositionsOutput[us] = (m_kXform.m_Rotate * 
            (m_kXform.m_fScale * kV)) + m_kXform.m_Translate;
    }
    pkOutputPositionsDSRef->GetDataStream()->Unlock(NiDataStream::LOCK_WRITE);

    // Indicate that the position data has been updated so complete tasks 
    // can determine if it needs to update NBT data.
    m_bUpdatedPositionData = true;

    // 4. If necessary, compute normals and NBT frames for the vertices.
    //    This is done in the Complete-method.

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXClothModifier::CompleteTasks(NiMesh* pkMesh, NiSyncArgs*)
{
    if (m_bUpdatedPositionData)
    {
        if (GetUpdateNormals())
        {
            NiDataStreamRef* pkNormalsDSRef = pkMesh->FindStreamRef(
                NiCommonSemantics::NORMAL());

            if (pkNormalsDSRef)
            {
                NiDataStreamRef* pkBinormalsDSRef = pkMesh->FindStreamRef(
                    NiCommonSemantics::BINORMAL());
                NiDataStreamRef* pkTangentsDSRef = pkMesh->FindStreamRef(
                    NiCommonSemantics::TANGENT());
                
                if (pkBinormalsDSRef && pkTangentsDSRef)
                {
                    NiMeshUtilities::RecalculateNBTs_NDL(pkMesh, m_uiNBTSet,
                        NiCommonSemantics::POSITION(), 0,
                        NiCommonSemantics::NORMAL(), 0,
                        NiCommonSemantics::BINORMAL(),
                        NiCommonSemantics::TANGENT());
                }
                else
                {
                    NiMeshUtilities::RecalculateNormals(pkMesh,
                        NiCommonSemantics::POSITION(), 0,
                        NiCommonSemantics::NORMAL(), 0);
                }
            }
        }

        // 5. Update the bounds.
        pkMesh->RecomputeBounds();
        
        m_bForceCached = false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::ProcessBasicTearing(NiMesh* pkMesh)
{
    NiUInt32 uiNumNxVerts = m_kClothData.numVerticesPtr[0];
    NxU16* pusParentIndex = (NxU16*)m_kClothData.parentIndicesBegin;

    NiDataStreamRef* pkStreamRef = 0;
    NiDataStream* pkStream = 0;

    // Iterate over all the streams in the mesh, if the usage of the stream 
    // is USAGE_VERTEX, then copy into the new elements from their respective
    // parent.
    for (NiUInt32 ui = 0; ui < pkMesh->GetStreamRefCount(); ui++)
    {
        pkStreamRef = pkMesh->GetStreamRefAt(ui);
        if (pkStreamRef && 
            pkStreamRef->GetUsage() == NiDataStream::USAGE_VERTEX)
        {
            pkStream = pkStreamRef->GetDataStream();
            
            // The stride of the NiDataStreamElementSet.
            NiUInt32 uiStride = pkStream->GetStride();

            // Aquire a lock to the NiDataStream.
            unsigned char* pucBuffer = (unsigned char*)pkStream->Lock(
                NiDataStream::LOCK_READ | NiDataStream::LOCK_WRITE);

            NIASSERT(pucBuffer && "Failed to lock the stream");
            
            // Copy (uiNumNxVerts - m_uiPreviusVertexCount) elements.
            for (NiUInt32 uiElement = m_uiPreviousVertexCount; 
                uiElement < uiNumNxVerts; uiElement++)
            {
                NxU16 usParentIndex = pusParentIndex[uiElement];
                
                // Memcpy from the parent elment to the new element.
                NiMemcpy(pucBuffer + uiStride * uiElement, 
                    pucBuffer + uiStride * usParentIndex, uiStride);
            }

            // Unlock the NiDataStream.
            pkStream->Unlock(NiDataStream::LOCK_READ 
                | NiDataStream::LOCK_WRITE);

            // Resize the current region.
            pkStream->GetRegion(0).SetRange(uiNumNxVerts);
        }
    }

    // Add to end of vertex map
    for (NiUInt32 ui = m_uiPreviousVertexCount; ui < uiNumNxVerts; ui++)
    {
        m_puiVertexMap[ui] = (NiUInt16)ui;
    }

    // Update triangles
    NiDataStreamElementLock kIndexLock = NiDataStreamElementLock(pkMesh, 
        NiCommonSemantics::INDEX(), 0, NiDataStreamElement::F_UNKNOWN, 
        NiDataStream::LOCK_WRITE | NiDataStream::LOCK_READ);

    NIASSERT(kIndexLock.IsLocked());

    NiTStridedRandomAccessIterator<NiUInt16> kIndexIterator = 
        kIndexLock.begin<NiUInt16>();

    NiUInt32 uiNumIndices = pkMesh->GetPrimitiveCount(0) * 3;
    NiUInt16* puiNxIndices =  (NiUInt16*)m_kClothData.indicesBegin;
    NIASSERT(uiNumIndices == m_kClothData.numIndicesPtr[0]);
    for (NiUInt32 ui = 0; ui < uiNumIndices; ui++)
    {
        if (puiNxIndices[ui] >= m_uiPreviousVertexCount)
        {
            m_kSplitParentMap.SetAt(puiNxIndices[ui], kIndexIterator[ui]);
            kIndexIterator[ui] = puiNxIndices[ui];
        }
    }

    kIndexLock.Unlock();
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::ProcessGeneralTearing(NiMesh* pkMesh)
{
    // In this instance, many Gamebryo vertices may map to the same PhysX
    // vertex, and many triangles may share the same Gamebryo vertex. The
    // most obvious case is a cube with multiple copies of each corner vertex
    // so that different normals can be specified. Other cases include
    // texture seams.
    
    // As a result, the processing is very different from the simple case.
    // For every reference to a newly created PhysX vertex, we need to decide
    // what to do with the corresponding Gamebryo vertex. Here, we simply
    // duplicate the old vertex data and create a new Gamebryo vertex to
    // match the new PhysX vertex. This may create extra vertices - the
    // required vertex may already exist in Gamebryo. However, we have no
    // easy way to know this, short of identifying all the vertices that
    // may need duplication, then making a second pass to see if they actually
    // need it. Instead we trade memory for speed.

    NiUInt32 uiNumNiVerts = pkMesh->GetVertexCount();

    // Iterate over all triangles, building up information about how torn
    // vertices are used.
    NiUInt32 uiNumIndices = pkMesh->GetPrimitiveCount(0) * 3;
    NIASSERT(uiNumIndices == m_kClothData.numIndicesPtr[0]);

    NiUInt16* puiNxIndices = (NiUInt16*)m_kClothData.indicesBegin;

    NiDataStreamElementLock kIndexLock = NiDataStreamElementLock(pkMesh, 
        NiCommonSemantics::INDEX(), 0, NiDataStreamElement::F_UNKNOWN, 
        NiDataStream::LOCK_WRITE);

    NIASSERT(kIndexLock.IsLocked());

    NiTStridedRandomAccessIterator<NiUInt16> kNiIndexIterator = 
        kIndexLock.begin<NiUInt16>();
    
    // Bad to use maps at runtime, but there is no better solution.
    // This code should be rarely executed (how often does topology really
    // change, and how many vertices are really affected?)

    // Map of vertices that have been replicated. The key is a combination
    // of the previous Gamebryo index for the vertex and the new PhysX
    // index. The value is the new Gamebryo index for the replicated
    // vertex. This handles a single PhysX old vertex being split into
    // more than two new vertices.
    NiTMap<NiUInt32, NiUInt16> kReplicated(7);
    for (NiUInt32 ui = 0; ui < uiNumIndices; ui++)
    {
        NiUInt32 uiNxIndex = puiNxIndices[ui];
        if (uiNxIndex >= m_uiPreviousVertexCount)
        {
            // Have a vertex that refers to a new vertex. Get the
            // Gamebryo vertex index.
            NiUInt16 uiGBIndex = kNiIndexIterator[ui];
            
            // Check if this vertex is already replicated
            NiUInt32 uiMapKey = (uiNxIndex << 16) | uiGBIndex;
            NiUInt16 uiNewGBIndex = 0;
            if (kReplicated.GetAt(uiMapKey, uiNewGBIndex))
            {
                // We have already duplicated this Gamebryo vertex for
                // the corresponding PhysX vertex. So go ahead and simply
                // change the vertex ID in the array and we are done.
                NIASSERT(uiNxIndex == m_puiVertexMap[uiNewGBIndex]);
                kNiIndexIterator[ui] = uiNewGBIndex;
                continue; // Breaks us out to the next triangle index
            }

            // Here if the vertex has not already been replicated.
            // At this point we must replicate. Sometimes we will replicate
            // when we don't have to (Gamebryo already has the vertex split
            // or some other reason, such as a texture seam) but we need
            // reliable parent indices from PhysX to do it, but the parent
            // is always the very original vertex, not the one from which
            // a new vertex was split.

            // If you hit this assert, there isn't enough space
            // for the replicated vertex, so you need to allocate
            // a bigger growth factor.
            NIASSERT(uiNumNiVerts < 
                kIndexLock.GetDataStream()->GetTotalCount() - 1);
            uiNewGBIndex = (NiUInt16)uiNumNiVerts++;
            
            // Copy all data over and set maps and indexes
            CopyVertexData(uiGBIndex, uiNewGBIndex, pkMesh);
            m_puiVertexMap[uiNewGBIndex] = (NiUInt16)uiNxIndex;
            kNiIndexIterator[ui] = uiNewGBIndex;
            kReplicated.SetAt(uiMapKey, uiNewGBIndex);
            m_kSplitParentMap.SetAt(uiNewGBIndex, uiGBIndex);
        }
    }

    for (NiUInt32 ui = 0; ui < pkMesh->GetStreamRefCount(); ui++)
    {
        NiDataStreamRef* pkStreamRef = pkMesh->GetStreamRefAt(ui);

        if (pkStreamRef && 
            pkStreamRef->GetUsage() == NiDataStream::USAGE_VERTEX)
        {
            pkStreamRef->GetRegionForSubmesh(0).SetRange(uiNumNiVerts);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::CopyVertexData(NiUInt32 uiSrc, 
    NiUInt32 uiDest, NiMesh* pkMesh)
{
    NiDataStreamRef* pkStreamRef = 0;
    NiDataStream* pkStream = 0;

    // Iterate over all the streams in the mesh, if the usage of the stream 
    // is USAGE_VERTEX, then copy from uiSrc to uiDest.
    for (NiUInt32 ui = 0; ui < pkMesh->GetStreamRefCount(); ui++)
    {
        pkStreamRef = pkMesh->GetStreamRefAt(ui);
        if (pkStreamRef && 
            pkStreamRef->GetUsage() == NiDataStream::USAGE_VERTEX)
        {
            pkStream = pkStreamRef->GetDataStream();
            
            // The stride of the NiDataStreamElementSet.
            NiUInt32 uiStride = pkStream->GetStride();

            // Aquire a lock to the NiDataStream.
            unsigned char* pucBuffer = (unsigned char*)pkStream->Lock(
                NiDataStream::LOCK_READ | NiDataStream::LOCK_WRITE);

            NIASSERT(pucBuffer && "Failed to lock the stream");
            
            // Copy from uiSrc to uiDest.
            NiMemcpy(pucBuffer + uiStride * uiDest, 
                pucBuffer + uiStride * uiSrc, uiStride);

            // Unlock the NiDataStream.
            pkStream->Unlock(NiDataStream::LOCK_READ | 
                NiDataStream::LOCK_WRITE);
        }
    }

    NiDataStreamElementLock kNormalShareIndexLock(pkMesh, 
        NiCommonSemantics::NORMALSHAREINDEX(), 0,
        NiDataStreamElement::F_UNKNOWN, NiDataStream::LOCK_WRITE);

    if (kNormalShareIndexLock.IsLocked())
    {
        NiTStridedRandomAccessIterator<NiUInt32> kIndexIterator = 
            kNormalShareIndexLock.begin<NiUInt32>();
        kIndexIterator[uiDest] = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions done rarely, at setup or destruction.
//---------------------------------------------------------------------------
void NiPhysXClothModifier::SceneChanged(NiMesh* pkMesh, NiPhysXScene* pkScene)
{
    // Remove old sync points
    m_kSubmitPoints.RemoveAll();
    m_kCompletePoints.RemoveAll();

    m_pkScene = pkScene;

    // If there is no scene, we are done
    if (!pkScene)
    {
        pkMesh->DetachModifier(this);
        return;
    }

    // Pre-size arrays
    m_kSubmitPoints.Realloc(1);
    m_kCompletePoints.Realloc(1);

    // Set the submit and complete syncpoints depending on if the
    // simulation is run in parallel with rendering or not.
    if (pkScene->GetAsynchronousSimulation())
    {
        // Tasks are submitted at NiSyncArgs::SYNC_PHYSICS_COMPLETED and 
        // completed at NiSyncArgs::SYNC_PHYSICS_SIMULATE if simulation 
        // is asynchronous.
        AddSubmitSyncPoint(NiSyncArgs::SYNC_PHYSICS_COMPLETED);
        AddCompleteSyncPoint(NiSyncArgs::SYNC_PHYSICS_SIMULATE);
    }
    else
    {
        // Tasks are submitted at NiSyncArgs::SYNC_VISIBILE and completed at
        // NiSyncArgs::SYNC_RENDER if simulation is synchronous. We also add
        // a submit point at SYNC_PHYSICS_COMPLETED which we use to cache data.
        AddSubmitSyncPoint(NiSyncArgs::SYNC_PHYSICS_COMPLETED);
        AddSubmitSyncPoint(NiSyncArgs::SYNC_VISIBLE);
        AddCompleteSyncPoint(NiSyncArgs::SYNC_RENDER);
    }    
    
    // The modifier might not have been attached due to a lack of cloth
    // data or the lack of a PhysX scene. Give it another chance to attach
    // now.
    pkMesh->AttachModifier(this);
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::SetCloth(NxCloth* pkCloth, 
    const NiUInt16* puiVertexMap, NiMesh* pkMesh)
{
    RestoreTriangleIndices(pkMesh);

    // We must detach the modifier because it holds data streams that
    // reference the mesh data we are about to release.
    pkMesh->DetachModifier(this);

    DeallocateClothData();

    m_pkCloth = pkCloth;
    
    if (!m_pkCloth)
    {
        pkMesh->DetachModifier(this);
        return;
    }

    // Set default growth factor, if needed
    if (m_fMeshGrowFactor == 0.0f)
    {
        // Find out if the cloth is tearable
        if (pkCloth->getFlags() & NX_CLF_TEARABLE)
            m_fMeshGrowFactor = 1.5f;
        else
            m_fMeshGrowFactor = 1.0f;
    }

    AllocateClothData(puiVertexMap, pkMesh);
    
    m_pkCloth->setMeshData(m_kClothData);
    
    SetSleeping(false);
    
    // We detached the modifier, now re-attach.
    pkMesh->AttachModifier(this);
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::RestoreTriangleIndices(NiMesh* pkMesh)
{
    if (!pkMesh)
        return;
        
    NiUInt32 uiNumIndices = pkMesh->GetPrimitiveCount(0) * 3;
    
    NiDataStreamElementLock kIndexLock = NiDataStreamElementLock(pkMesh, 
        NiCommonSemantics::INDEX(), 0, NiDataStreamElement::F_UNKNOWN, 
        NiDataStream::LOCK_WRITE);

    NIASSERT(kIndexLock.IsLocked());

    NiTStridedRandomAccessIterator<NiUInt16> kIndexIterator = 
        kIndexLock.begin<NiUInt16>();

    NIASSERT(kIndexLock.IsLocked());

    for (NiUInt32 ui = 0; ui < uiNumIndices; ui++)
    {
        NiUInt16 uiChild = kIndexIterator[ui];
        NiUInt16 uiParent;
        while (m_kSplitParentMap.GetAt(uiChild, uiParent))
        {
            uiChild = uiParent;
        }
        kIndexIterator[ui] = uiChild;
    }

    kIndexLock.Unlock();
    
    if (m_uiOriginalVertexCount != ms_uiInvalidVertexCount)
    {
        // Reset the vertex count on all the vertex streams.
        for (NiUInt32 ui = 0; ui < pkMesh->GetStreamRefCount(); ui++)
        {
            NiDataStreamRef* pkStreamRef = pkMesh->GetStreamRefAt(ui);

            if (pkStreamRef && 
                pkStreamRef->GetUsage() == NiDataStream::USAGE_VERTEX)
            {
                pkStreamRef->GetRegionForSubmesh(0).
                    SetRange(m_uiOriginalVertexCount);
            }
        }
    }
    
    // Set the original vertex count to be invalid
    m_uiOriginalVertexCount = ms_uiInvalidVertexCount;
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::AllocateClothData(const NiUInt16* puiVertexMap,
    NiMesh* pkMesh)
{
    DeallocateClothData();

    // There is the size of the PhysX mesh, without redundant vertices, and
    // the size of the Gamebryo mesh, with redudant vertices (possibly).
    
    // Deal with the PhysX side of things
    NxClothMesh* pkNxMesh = m_pkCloth->getClothMesh();
    NxClothMeshDesc kNxMeshDesc;
    pkNxMesh->saveToDesc(kNxMeshDesc);
    NiUInt32 uiNumNxVerts = (NiUInt32)
        (kNxMeshDesc.numVertices * m_fMeshGrowFactor);
    NiUInt32 uiNumNxTris = kNxMeshDesc.numTriangles;
    
    m_kClothData.maxVertices = uiNumNxVerts;
    m_kClothData.verticesPosByteStride = sizeof(NxVec3);
    m_kClothData.verticesPosBegin = NiAlloc(NxVec3, uiNumNxVerts);
    m_kClothData.numVerticesPtr = NiAlloc(NxU32, 1);
    m_kClothData.numVerticesPtr[0] = 0;

    m_kClothData.verticesNormalByteStride = 0;
    m_kClothData.verticesNormalBegin = 0;
 
    m_kClothData.maxIndices = uiNumNxTris * 3;
    m_kClothData.indicesByteStride = sizeof(NxU16);
    m_kClothData.indicesBegin = NiAlloc(NxU16, m_kClothData.maxIndices);
    m_kClothData.numIndicesPtr = NiAlloc(NxU32, 1);
    m_kClothData.numIndicesPtr[0] = 0;

    m_kClothData.maxParentIndices = uiNumNxVerts;
    m_kClothData.parentIndicesByteStride = sizeof(NxU16);
    m_kClothData.parentIndicesBegin = NiAlloc(NxU16, uiNumNxVerts);
    m_kClothData.numParentIndicesPtr = NiAlloc(NxU32, 1);
 
    m_kClothData.flags = NX_MDF_16_BIT_INDICES;
 
    m_uiOriginalVertexCount = (NiUInt16)pkMesh->GetVertexCount();

    m_uiPreviousVertexCount = (NiUInt16)kNxMeshDesc.numVertices;
        
    // Deal with the Gamebryo side
    
    NiUInt32 uiNumNiVerts = (NiUInt32)
        (m_uiOriginalVertexCount * m_fMeshGrowFactor);
    // Set up the vertex map
    NIASSERT(puiVertexMap);
    m_puiVertexMap = NiAlloc(NiUInt16, uiNumNiVerts);
    for (NiUInt32 ui = 0; ui < m_uiOriginalVertexCount; ui++)
    {
        m_puiVertexMap[ui] = puiVertexMap[ui];
    }

    // Only need to reallocate the datastreams if the grow factor is bigger 
    // than 1.0f;
    if (m_fMeshGrowFactor > 1.0f)
    {
        // Iterate over all of the datastreams in the mesh, make a new 
        // datastream with the new bigger size. Copy the original data 
        // into the new one.
        NiUInt32 uiStreamCount = pkMesh->GetStreamRefCount();
        NiDataStreamRef* pkStreamRef = 0;
        for (NiUInt32 ui = 0; ui < uiStreamCount; ui++)
        {
            pkStreamRef = pkMesh->GetStreamRefAt(ui);
            if (pkStreamRef &&
                pkStreamRef->GetUsage() == NiDataStream::USAGE_VERTEX)
            {
                NiDataStreamPtr pkOldStream = pkStreamRef->GetDataStream();

                NIASSERT(pkOldStream->GetRegionCount() == 1);
                
                if (uiNumNiVerts == pkOldStream->GetTotalCount())
                    continue;
                
                // Create an exact copy of the elementset in the old 
                // datastream.
                NiDataStreamElementSet kElementSet;
                for (NiUInt32 uiElementIndex = 0; 
                    uiElementIndex < pkOldStream->GetElementDescCount(); 
                    uiElementIndex++)
                {
                    kElementSet.AddElement(pkOldStream->GetElementDescAt(
                        uiElementIndex).GetFormat());
                }

                NiDataStreamPtr pkNewStream = NiDataStream::CreateDataStream(
                    kElementSet, uiNumNiVerts, pkOldStream->GetAccessMask(),
                    pkOldStream->GetUsage());

                // Copy the regions from the old stream to the new one.
                pkNewStream->AddRegion(pkOldStream->GetRegion(0));

                // Aquire the actual data buffers.
                const void* pOldStreamBuff = 
                    pkOldStream->Lock(NiDataStream::LOCK_READ);
                NIASSERT(pOldStreamBuff);

                void* pNewStreamBuff = pkNewStream->Lock(
                    NiDataStream::LOCK_WRITE);
                NIASSERT(pNewStreamBuff);

                // Copy the content from the old buffer into the new buffer.
                NiMemcpy(pNewStreamBuff, pOldStreamBuff, 
                    pkOldStream->GetSize());

                // Unlock the datastreams.
                pkOldStream->Unlock(NiDataStream::LOCK_READ);
                pkNewStream->Unlock(NiDataStream::LOCK_WRITE);

                // Destroy the old datastream by assigning the new 
                // datastream to the NiDataStreamRef.
                pkStreamRef->SetDataStream(pkNewStream);
            } // if (pkStreamRef)
        } // for (NiUInt32 ui = 0; ui < uiStreamCount; ui++)
        
        pkMesh->ResetModifiers();
    }
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::DeallocateClothData()
{
    NiFree(m_kClothData.verticesPosBegin);
    NiFree(m_kClothData.numVerticesPtr);
    NiFree(m_kClothData.indicesBegin);
    NiFree(m_kClothData.numIndicesPtr);
    NiFree(m_kClothData.parentIndicesBegin);
    NiFree(m_kClothData.numParentIndicesPtr);
    NiFree(m_puiVertexMap);
    
    m_kClothData.verticesPosBegin = 0;
    m_kClothData.numVerticesPtr = 0;
    m_kClothData.indicesBegin = 0;
    m_kClothData.numIndicesPtr = 0;
    m_kClothData.parentIndicesBegin = 0;
    m_kClothData.numParentIndicesPtr = 0;
    m_puiVertexMap = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXClothModifier);
//---------------------------------------------------------------------------
void NiPhysXClothModifier::CopyMembers(NiPhysXClothModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXMeshModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_pkCloth = 0;

    pkDest->m_puiVertexMap = 0;

    pkDest->m_pkParentActor = 0;

    pkDest->m_fMeshGrowFactor = m_fMeshGrowFactor;
    pkDest->m_uiOriginalVertexCount = m_uiOriginalVertexCount;
    pkDest->m_uiNBTSet = m_uiNBTSet;

    pkDest->m_kWorldXform.MakeIdentity();
    pkDest->m_uiPreviousVertexCount = 0;

    pkDest->m_uFlags = m_uFlags;
    pkDest->SetSleeping(false);
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiMeshModifier::ProcessClone(kCloning);
}
//---------------------------------------------------------------------------
bool NiPhysXClothModifier::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXClothModifier);
//---------------------------------------------------------------------------
void NiPhysXClothModifier::LoadBinary(NiStream& kStream)
{
    NiPhysXMeshModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiOriginalVertexCount);
    NiStreamLoadBinary(kStream, m_fMeshGrowFactor);
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_uFlags);
    }
    else
    {
        NiBool bBool;
        NiStreamLoadBinary(kStream, bBool);
        SetDoSleepXforms(bBool ? true : false);
    }
    
    NiStreamLoadBinary(kStream, m_uiNBTSet);
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::LinkObject(NiStream& kStream)
{
    NiPhysXMeshModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXClothModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXMeshModifier::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::SaveBinary(NiStream& kStream)
{
    NiPhysXMeshModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiOriginalVertexCount);
    NiStreamSaveBinary(kStream, m_fMeshGrowFactor);

    NiStreamSaveBinary(kStream, m_uFlags);
    
    NiStreamSaveBinary(kStream, m_uiNBTSet);
}
//---------------------------------------------------------------------------
bool NiPhysXClothModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPhysXMeshModifier::IsEqual(pkObject))
        return false;

    if (!NiIsKindOf(NiPhysXClothModifier, pkObject))
        return false;

    NiPhysXClothModifier* pkThatObject = (NiPhysXClothModifier*) pkObject;

    if (m_fMeshGrowFactor != pkThatObject->m_fMeshGrowFactor ||
        m_uFlags != pkThatObject->m_uFlags ||
        m_uiNBTSet != pkThatObject->m_uiNBTSet)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXClothModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPhysXMeshModifier::GetViewerStrings(pkStrings);
    pkStrings->Add(NiGetViewerString(NiPhysXClothModifier::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
