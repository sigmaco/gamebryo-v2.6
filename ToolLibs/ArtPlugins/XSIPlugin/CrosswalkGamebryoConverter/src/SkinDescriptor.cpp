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

#include "CrosswalkGamebryoSkinMap.h"
#include "CrosswalkGamebryoContext.h"

#include "Model.h"

#include <set>
#include <algorithm>

namespace
{
    //---------------------------------------------------------------------------
    // Container for the set of parents of a model.
    //---------------------------------------------------------------------------
    typedef std::set<CSLModel*> ParentSet;

    //---------------------------------------------------------------------------
    // Retrieves all the parent model of a given bone.
    //---------------------------------------------------------------------------
    void GetAllBoneParents(CSLModel* pBone, ParentSet& out_Parents)
    {
        out_Parents.clear();
        if (!pBone)
            return;

        for (CSLModel* pParent = pBone->Parent(); pParent; pParent = pParent->Parent())
            out_Parents.insert(pParent);
    }

    //---------------------------------------------------------------------------
    // Intersect two set of parents and return the result in the first set.
    //---------------------------------------------------------------------------
    void InplaceIntersection(ParentSet& io_Parents, const ParentSet& in_BoneParents)
    {
        typedef ParentSet::iterator iter;
        for (iter pos = io_Parents.begin(); pos != io_Parents.end();)
        {
            iter checkMe = pos++;
            if (!in_BoneParents.count(*checkMe))
                io_Parents.erase(checkMe);
        }
    }

    //---------------------------------------------------------------------------
    // Keep common parents.
    //---------------------------------------------------------------------------
    void KeepCommonParents(
        const epg::SkinDescriptor::BoneIndices& in_BoneIndices,
        ParentSet& io_Parents)
    {
        typedef epg::SkinDescriptor::BoneIndices::const_iterator iter;
        for (iter pos = in_BoneIndices.begin(); pos != in_BoneIndices.end(); ++pos)
        {
            ParentSet boneParents;
            GetAllBoneParents(pos->first, boneParents);
            InplaceIntersection(io_Parents, boneParents);
        }
    }

    //---------------------------------------------------------------------------
    // Remove all the node that are parent to another node.
    // The last node left is the lowest parent of all the bones.
    //---------------------------------------------------------------------------
    void KeepLowestParent(ParentSet& io_Parents)
    {
        typedef ParentSet::iterator iter;
        for (iter pos = io_Parents.begin(); pos != io_Parents.end(); ++pos)
            for (CSLModel* pParent = (*pos)->Parent(); pParent; pParent = pParent->Parent())
                if (io_Parents.count(pParent))
                    io_Parents.erase(pParent);
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Create an empty bone weight.
    //---------------------------------------------------------------------------
    BoneWeight::BoneWeight()
        : bone(0)
        , weight(0.0f)
    {
    }

    //---------------------------------------------------------------------------
    // Create an initialized bone weight.
    //---------------------------------------------------------------------------
    BoneWeight::BoneWeight(CSLModel& in_Bone, float in_Weight)
        : bone(&in_Bone)
        , weight(in_Weight)
    {
    }

    //---------------------------------------------------------------------------
    // Compare bone weights.
    //---------------------------------------------------------------------------
    bool BoneWeight::operator<(const BoneWeight& rhs) const
    {
        if (weight > rhs.weight)
            return true;
        if (weight < rhs.weight)
            return false;
        if ((uintptr_t)bone < (uintptr_t)&rhs.bone)
            return true;
        return false;
    }

    //---------------------------------------------------------------------------
    // Adds a bone affecting a vertex.
    //---------------------------------------------------------------------------
    void BonesDescriptor::AddVertexBone(
        int in_VertexIndex,
        CSLModel& in_Bone,
        float in_BoneWeight)
    {
        if (in_BoneWeight > 0.0f)
        {
            BoneWeights& bones = m_VertexBones[in_VertexIndex];
            bones.insert(BoneWeight(in_Bone, in_BoneWeight));
        }
    }

    //---------------------------------------------------------------------------
    // Retrieves the list of bones for the given vertex.
    //---------------------------------------------------------------------------
    const BonesDescriptor::BoneWeights* BonesDescriptor::FindVertexBones(int in_VertexIndex) const
    {
        typedef VertexBones::const_iterator iter;
        const iter pos = m_VertexBones.find(in_VertexIndex);
        if (pos != m_VertexBones.end())
            return &pos->second;
        else
            return 0;
    }

    //---------------------------------------------------------------------------
    /// Retrieves all bones.
    //---------------------------------------------------------------------------
    const BonesDescriptor::VertexBones& BonesDescriptor::GetVertexBones() const
    {
        return m_VertexBones;
    }

    //---------------------------------------------------------------------------
    // Skin descriptor.
    //---------------------------------------------------------------------------
    SkinDescriptor::SkinDescriptor()
        : m_pSkinModifier(0)
        , m_HighestBoneIndex(-1)
        , m_RootBoneParent(0)
    {
    }

    //---------------------------------------------------------------------------
    // Fills the bone indices used in the following functions.
    //---------------------------------------------------------------------------
    int SkinDescriptor::AssignBoneIndices(
        Context& io_Context,
        CSLModel& in_Skin,
        const BonesDescriptor& in_Bones)
    {
        // Avoid doing the same work twice.
        if (m_HighestBoneIndex < 0)
        {
            typedef BonesDescriptor::VertexBones VertexBones;
            const VertexBones& bones = in_Bones.GetVertexBones();

            typedef VertexBones::const_iterator iter;
            for (iter pos = bones.begin(); pos != bones.end(); ++pos)
            {
                const int vertexIndex = pos->first;
                float totalWeight = 0.0f;

                // Fill bone indices for the yet-unseen bones of that vertex.
                typedef BonesDescriptor::BoneWeights BoneWeights;
                const BoneWeights& bones = pos->second;
                typedef BoneWeights::const_iterator iter;
                for (iter pos = bones.begin(); pos != bones.end(); ++pos)
                {
                    CSLModel *pBone = pos->bone;
                    if (m_BoneIndices.count(pBone) == 0)
                    {
                        m_BoneIndices[pBone] = ++m_HighestBoneIndex;
                    }
                    totalWeight += pos->weight;
                }

                // Make sure that all skin vertices that have no bone affecting them
                // have the skin as the bone because gamebryo doesn't support skinned
                // vertices with all weight set to zero.
                static const float MINIMUM_VALID_TOTAL_WEIGHT = 0.001f;
                if (totalWeight < MINIMUM_VALID_TOTAL_WEIGHT)
                {
                    io_Context.Logf(
                        LOG_WARNING,
                        "Vertex #%d in skin \"%s\" has less than %d%% total weight,"
                        " adding skin as bone to compensate.",
                        vertexIndex+1,
                        in_Skin.Name().GetText(),
                        (int) (MINIMUM_VALID_TOTAL_WEIGHT * 100.0f));
                }
            }
        }

        return m_HighestBoneIndex;
    }

    //---------------------------------------------------------------------------
    // Retrieve the index used in Gamebryo streams for the given bone.
    //---------------------------------------------------------------------------
    int SkinDescriptor::GetBoneIndex(CSLModel& in_Bone) const
    {
        typedef BoneIndices::const_iterator iter;
        const iter pos = m_BoneIndices.find(&in_Bone);
        if (pos != m_BoneIndices.end())
            return pos->second;
        else
            return -1;
    }

    //---------------------------------------------------------------------------
    // Retrieve all Gamebryo bone indices.
    //---------------------------------------------------------------------------
    const SkinDescriptor::BoneIndices& SkinDescriptor::GetBoneIndices() const
    {
        return m_BoneIndices;
    }

    //---------------------------------------------------------------------------
    // Find the bone that is the highest in the bone hierarchy.
    //---------------------------------------------------------------------------
    CSLModel* SkinDescriptor::FindRootBoneParent() const
    {
        if (m_BoneIndices.size() == 0)
            return 0;

        if (0 == m_RootBoneParent)
        {
            // First, we create the set of potential parent from an arbitrary bone
            // by accumulating all node up to the root of the tree from that bone.
            ParentSet potentialParents;
            GetAllBoneParents(m_BoneIndices.begin()->first, potentialParents);

            // Now, for each bone, we remove from the potential parent all the node
            // that are *not* a parent of the bone. This will leave us with the set
            // of nodes that are parent to all the bones.
            KeepCommonParents(m_BoneIndices, potentialParents);

            // Finally, we remove all the node that are parent to another node.
            // The last node left is the lowest parent of all the bones.
            KeepLowestParent(potentialParents);

            if (potentialParents.size() > 0)
            {
                m_RootBoneParent = *potentialParents.begin();
            }
        }

        return m_RootBoneParent;
    }
}
