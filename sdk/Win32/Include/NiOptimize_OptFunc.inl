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

#include <NiNode.h>
#include <NiMeshTools.h>

//---------------------------------------------------------------------------
// Optimization functions.
//---------------------------------------------------------------------------
inline bool NiOptimize::CollectChildlessNodes(NiNode* pkNode,
    NiNodeArray& apkNodes)
{
    // Detects the presence of childless nodes that can be removed by
    // RemoveChildlessNodes and stores a pointer to each in apkNodes.

    // Returns: true, if childless nodes exist that can be removed.
    //          false, otherwise.

    NiTPointerMap<NiAVObject*, bool> kMap;
    BuildHashTable(pkNode, kMap);
    CollectChildlessNodes(pkNode, apkNodes, kMap);

    return (apkNodes.GetEffectiveSize() > 0);
}
//---------------------------------------------------------------------------
inline void NiOptimize::RemoveChildlessNodes(NiNode* pkNode)
{
    // Removes all childless nodes. A node will not be removed if it is
    // singled out by BuildHashTable or has an "sgoKeep" extra data tag.

    NiTPointerMap<NiAVObject*, bool> kMap;
    BuildHashTable(pkNode, kMap);
    RemoveChildlessNodes(pkNode, kMap);
}
//---------------------------------------------------------------------------
inline bool NiOptimize::CollectMultiMtlNodes(NiNode* pkNode,
    NiNodeArray& apkNodes)
{
    // Detects the presence of multi-material nodes that can be removed by
    // RemoveMultiMtlNodes and stores a pointer to each in apkNodes.

    // Returns: true, if multi-material nodes exist that can be removed.
    //          false, otherwise.

    NiTPointerMap<NiAVObject*, bool> kMap;
    BuildHashTable(pkNode, kMap);
    CollectMultiMtlNodes(pkNode, apkNodes, kMap);

    return (apkNodes.GetEffectiveSize() > 0);
}
//---------------------------------------------------------------------------
inline void NiOptimize::RemoveMultiMtlNodes(NiNode* pkNode)
{
    // Recursively removes all multi-material nodes. A node will not be
    // removed if it is singled out by BuildHashTable, has an "sgoKeep" extra
    // data tag, or has any controllers.

    NiTPointerMap<NiAVObject*, bool> kMap;
    BuildHashTable(pkNode, kMap);
    RemoveMultiMtlNodes(pkNode, kMap);
}
//---------------------------------------------------------------------------
inline bool NiOptimize::CollectSingleChildNodes(NiNode* pkNode,
    NiNodeArray& apkNodes)
{
    // Detects the presence of single-child nodes that can be removed by
    // RemoveSingleChildNodes and stores a pointer to each in apkNodes.

    // Returns: true, if single-child nodes that can be removed exist.
    //          false, otherwise.

    NiTPointerMap<NiAVObject*, bool> kMap;
    BuildHashTable(pkNode, kMap);
    CollectSingleChildNodes(pkNode, apkNodes, kMap);

    return (apkNodes.GetEffectiveSize() > 0);
}
//---------------------------------------------------------------------------
inline void NiOptimize::RemoveSingleChildNodes(NiNode* pkNode)
{
    // Recursively removes all single-child nodes in the scene of exact type
    // NiNode, propagating their properties, transforms, extra data, and
    // names to the single child. A node will not be removed if it is
    // singled out by BuildHashTable, has an "sgoKeep" extra data tag, has
    // any controllers, or whose child has any controllers.

    NiTPointerMap<NiAVObject*, bool> kMap;
    BuildHashTable(pkNode, kMap);
    RemoveSingleChildNodes(pkNode, kMap);
}
//---------------------------------------------------------------------------
inline void NiOptimize::MergeSiblingNodes(NiNode* pkNode)
{
    // Recursively merges sibling nodes. A node will not be merged if it is
    // singled out by BuildHashTable, has an "sgoKeep" extra data tag, has
    // any controllers, has an ABV, has any extra data, or whose children
    // have any controllers.
    // Two nodes will be merged if the following holds:
    // The nodes are
    //     sibling nodes,
    //     of exact type NiNode,
    // and they have
    //     a parent of exact type NiNode,
    //     the same properties,
    //     the same AppCulled flag.
    // If two nodes can be merged and they have different transforms on them,
    // the transforms for both nodes are pushed down to their children before
    // they are merged.

    NiTPointerMap<NiAVObject*, bool> kMap;
    BuildHashTable(pkNode, kMap);
    MergeSiblingNodes(pkNode, kMap);
}
//---------------------------------------------------------------------------
inline bool NiOptimize::OptimizeMesh(NiMesh* pkMesh, 
    const float cfSizeModifier, const float cfNormalModifier, 
    const float cfPositionModifier, const NiUInt32 uiVertexCacheSize)
{
    if (!GetNoOptimizeMeshExtraData(pkMesh))
    {
        NiMeshTools::ViewIndependentOptimization(pkMesh, cfSizeModifier,
            cfNormalModifier, cfPositionModifier);

        NiMeshTools::VertexCacheOptimization(pkMesh, uiVertexCacheSize);
    }

    return false;
}
//---------------------------------------------------------------------------
inline void NiOptimize::RemoveHiddenBoneMesh(NiAVObject* pkObject)
{
    // Removes any NiRenderObject object that is a child of a bone and whose
    // AppCulled flag is true.

    NiTPointerMap<NiAVObject*, bool> kMap;
    CollectBoneChildren(pkObject, kMap);
    RemoveHiddenBoneMesh(pkObject, kMap);
}
//---------------------------------------------------------------------------
inline void NiOptimize::MergeSiblingMeshes(NiNode* pkNode,
    bool bNoMergeOnAlpha)
{
    NiTPointerMap<NiAVObject*, bool> kMap;
    BuildHashTable(pkNode, kMap);
    MergeSiblingMeshes(pkNode, bNoMergeOnAlpha, kMap);
}
//---------------------------------------------------------------------------
inline void NiOptimize::SortByTexturingProp(NiNode* pkNode)
{
    // Recursively sorts all the children of each node by the address of the
    // NiTexturingProperty present in their property states. Children with no
    // NiTexturingProperty in their property state attached are sorted first.

    pkNode->UpdateProperties();

    NiTexturingProperty* pkLoProp;
    NiTexturingProperty* pkHiProp;
    SortByTexturingProp(pkNode, pkLoProp, pkHiProp);
}
//---------------------------------------------------------------------------
inline void NiOptimize::RemoveAppCulledObjects(NiAVObject* pkObject)
{
    NiTPointerMap<NiAVObject*, bool> kMap;
    BuildHashTable(pkObject, kMap);
    AddBoneLODSkinsToHashTable(pkObject, kMap);
    RemoveAppCulledObjects(pkObject, kMap);
}
//---------------------------------------------------------------------------
