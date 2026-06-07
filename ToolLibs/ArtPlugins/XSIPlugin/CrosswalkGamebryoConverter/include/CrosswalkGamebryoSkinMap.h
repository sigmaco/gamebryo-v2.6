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

#ifndef XSI_GAMEBRYO_CONVERTER_SKIN_MAP_H
#define XSI_GAMEBRYO_CONVERTER_SKIN_MAP_H

#include "SLTypes.h"

#include "NiMainLibType.h"
#include "NiString.h"

#include <map>
#include <set>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLModel;

class NIMAIN_ENTRY NiAVObject;
class NIMAIN_ENTRY NiMesh;
class NIMAIN_ENTRY NiSkinningMeshModifier;
template <class T> class NiPointer;
typedef NiPointer<NiAVObject> NiAVObjectPtr;

namespace epg
{
    class Context;

    //---------------------------------------------------------------------------
    /// Bone weight.
    //---------------------------------------------------------------------------
    struct BoneWeight
    {
        BoneWeight();
        BoneWeight(CSLModel& in_Bone, float in_Weight);
        bool operator<(const BoneWeight& rhs) const;

        float     weight;
        CSLModel* bone;
    };

    //---------------------------------------------------------------------------
    /// Container for the bones associated with each vertex.
    //---------------------------------------------------------------------------
    struct BonesDescriptor
    {
        /// The bones with their weight.
        typedef std::set<BoneWeight> BoneWeights;

        // Per-vertex bones list, with their weigh.
        typedef std::map<int,BoneWeights> VertexBones;

        /// Adds a bone affecting a vertex.
        void AddVertexBone(int in_VertexIndex, CSLModel& in_Bone, float in_BoneWeight);

        /// Retrieves the list of bones for the given vertex.
        const BoneWeights* FindVertexBones(int in_VertexIndex) const;

        /// Retrieves all bones.
        const VertexBones& GetVertexBones() const;

    private:
        VertexBones         m_VertexBones;
    };

    //---------------------------------------------------------------------------
    /// Skin descriptor for Gamebryo.
    //---------------------------------------------------------------------------
    struct SkinDescriptor
    {
        /// Create an empty skin descriptor.
        SkinDescriptor();

        /// Maximum number of bones per vertex (Gamebryo limitation).
        enum { MAX_BONES_PER_VERTEX = 4 };

        /// The final bone indexes used in the stream.
        typedef std::map<CSLModel*,int> BoneIndices;

        /// Assigns the bone indices used in the following functions.
        /// The root bone parent or skin is added as bone if some vertex have all-zero weights.
        /// @return the highest bone index used, -1 if no bone.
        int AssignBoneIndices(Context& io_Ctx, CSLModel& in_Skin, const BonesDescriptor& in_Desc);

        /// Retrieve the index used in Gamebryo streams for the given bone.
        int GetBoneIndex(CSLModel& in_Bone) const;

        /// Retrieve all Gamebryo bone indices.
        const BoneIndices& GetBoneIndices() const;

        /// Find the bone that is the highest in the bone hierarchy.
        CSLModel* FindRootBoneParent() const;

        /// Gamebryo skin mesh modifier representing this skin.
        NiSkinningMeshModifier* m_pSkinModifier;

    private:
        BoneIndices             m_BoneIndices;
        int                     m_HighestBoneIndex;
        mutable CSLModel*       m_RootBoneParent;
    };

    //---------------------------------------------------------------------------
    /// Skin map.
    ///
    /// Remembers the mapping between XSI model being skinned and Gamebryo skin.
    //---------------------------------------------------------------------------

    class SkinMap
    {
    public:
        /// List of Gamebryo skins indexed by the mesh being skinned.
        typedef std::map<NiMesh*,SkinDescriptor> Skins;

        // List of bones weights indexed by the model being skinned.
        typedef std::map<CSLModel*,BonesDescriptor> SkinVertexBones;

        /// Create an empty skin map.
        SkinMap();

        /// Empty the skin map.
        void Clear();

        /** Add a new XSI model being skinned.
            @param CSLModel& the XSI model.
            @return the skin's vertices' bones container to be filled.
        */
        BonesDescriptor& AddBones(CSLModel& in_SkinnedModel);

        /** Lookup the skin vertex bones corresponding to an XSI model being skinned.
            @param CSLModel& the XSI model we know.
            @return the skin's vertices' bones, if any. May return NULL.
        */
        BonesDescriptor* FindBones(CSLModel& in_Model);

        /** Add a new mesh being skinned.
            @param in_Mesh the mesh being skinned.
            @return the skin descriptor to be filled.
        */
        SkinDescriptor& AddSkin(NiMesh& in_Mesh);

        /** Lookup the skin descriptor corresponding to a mesh being skinned.
            @param in_Mesh the mesh being skinned.
            @return the skin descriptor, if any. May return NULL.
        */
        SkinDescriptor* FindSkin(NiMesh& in_Mesh);

        /** Retrieve the list of Gamebryo skins.
            @return the list of skins.
        */
        Skins& GetSkins();

    private:
        // Prevent copies.
        SkinMap(const SkinMap &);
        void operator=(const SkinMap &);

        SkinVertexBones     m_Bones;
        Skins               m_Skins;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_SKIN_MAP_H */
