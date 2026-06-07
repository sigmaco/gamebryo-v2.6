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

#include "NiOptimize.h"
#include <NiSwitchNode.h>
#include <NiMesh.h>
#include <NiMeshUtilities.h>
#include <NiToolDataStream.h>
#include <NiDataStreamPrimitiveLock.h>
#include <NiDataStreamLock.h>
#include <NiAlphaProperty.h>
#include <NiLookAtInterpolator.h>
#include <NiVisController.h>
#include <NiStringExtraData.h>
#include <NiTransformController.h>
#include <NiBoneLODController.h>
#include <NiBoolInterpolator.h>
#include <NiTransformInterpolator.h>
#include <NiStepBoolKey.h>
#include <NiSkinningMeshModifier.h>
#include <NiTSimpleArray.h>
#include <NiTStridedRandomAccessIterator.h>
#include <NiToolPipelineCloneHelper.h>
#include <NiDataStreamElementLock.h>
#include <NiMorphWeightsController.h>
#include <NiPSParticleSystem.h>
#include <NiSkinningLODController.h>
#include <NiRoom.h>

typedef NiTPrimitiveSet<NiTriBasedGeom*>  NiTriBasedGeomSet;

//---------------------------------------------------------------------------
// Optimization functions.
//---------------------------------------------------------------------------
bool NiOptimize::IsPropertyUnique(NiProperty* pkProp, 
    NiTPointerList<NiProperty*>& pkPropList, NiProperty* &pkDupProp)
{
    // Given a property and a list of properties. Check to see if the 
    // given property is a duplicate/Equal to any in the list. Return
    // if a duplicate was found and set the duplicate property. This function
    // assumes the list contains unique properties so it will only return
    // the first duplicate found.

    // Returns: true/false if a duplicate/equal property was found.

    NiTListIterator kIter;

    kIter = pkPropList.GetHeadPos();

    // Loop through the list looking for duplicate properties
    while(kIter != NULL)
    {
        if(pkPropList.Get(kIter)->IsEqual(pkProp))
        {
            pkDupProp = pkPropList.Get(kIter);
            return false;
        }

        kIter = pkPropList.GetNextPos(kIter);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiOptimize::RecursiveRemoveDupProperties(NiAVObject* pkObject,
    NiTPropertyPointerListArray& apaUniqueProps)
{
    // This function will remove duplicate properties from a scene graph.
    // It will check each property of this object against the lists of unique
    // properties. The unique properties consists of a TArray where there is
    // one element for each type of property. And each TArray element
    // consists of a list of unique properties of that type. Each duplicate
    // property will be replaced

    // Check the uniqueness of each property
    NiTListIterator kPos = pkObject->GetPropertyList().GetHeadPos();
    while(kPos)
    {
        NiTListIterator kPosDelete = kPos;
        NiProperty* pkProp = pkObject->GetPropertyList().GetNext(kPos);

        if (pkProp)
        {
            // Get the unique property list for this type of property
            NiTPointerList<NiProperty*>* pkUniqueList = 
                apaUniqueProps.GetAt(pkProp->Type());
            NiProperty* pkDupProp;

            if (IsPropertyUnique(pkProp, *pkUniqueList, pkDupProp))
            {
                pkUniqueList->AddTail(pkProp);
            }
            else
            {
                // call RemovePos instead of DetachProperty to avoid
                // a search
                pkObject->GetPropertyList().RemovePos(kPosDelete);
                pkObject->AttachProperty(pkDupProp);
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                RecursiveRemoveDupProperties(pkChild, apaUniqueProps);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveDupProperties(NiAVObject* pkObject)
{
    // This function will remove all of the duplicate properties from the 
    // scene graph. We will need to create a structure of record keeping
    // lists to hold unique properties.

    // Create the record keeping lists
    NiTPropertyPointerListArray apaUniqueProps(
        NiProperty::MAX_TYPES);

    int iLoop;
    for(iLoop = 0; iLoop < NiProperty::MAX_TYPES; iLoop++)
    {
        apaUniqueProps.SetAt(iLoop, NiNew NiTPointerList<NiProperty*>);
    }


    RecursiveRemoveDupProperties(pkObject, apaUniqueProps);

    // Delete our record keeping lists
    for(iLoop = 0; iLoop < NiProperty::MAX_TYPES; iLoop++)
    {
        NiDelete apaUniqueProps.GetAt(iLoop);
    }

}
//---------------------------------------------------------------------------
void NiOptimize::CollectChildlessNodes(NiNode* pkNode,
    NiNodeArray& apkNodes, const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Detects the presence of childless nodes that can be removed by
    // RemoveChildlessNodes and stores a pointer to each in apkNodes.

    bool bTemp;
    if (pkNode->GetChildCount() == 0 && !CheckForExtraDataTags(pkNode) &&
        !kMap.GetAt(pkNode, bTemp))
    {
        apkNodes.Add(pkNode);
    }

    // Recurse over children.
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                CollectChildlessNodes((NiNode*) pkChild, apkNodes, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::RemoveChildlessNodes(NiNode* pkNode,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Removes all childless nodes. A node will not be removed if it is
    // singled out by BuildHashTable or has an "sgoKeep" extra data tag.

    // Returns: true, parent should delete the object.
    //          false, parent should keep the object.

    // If pkNode is an NiSwitchNode, save the active index.
    int iIndex = -1;
    if (NiIsKindOf(NiSwitchNode, pkNode))
    {
        iIndex = ((NiSwitchNode*) pkNode)->GetIndex();
    }

    // Recurse over children.
    NiTObjectArray<NiAVObjectPtr> aspSaveChildren(50);
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            bool bRemove;
            if (NiIsKindOf(NiNode, pkChild))
            {
                // Recurse on NiNode child.
                bRemove = RemoveChildlessNodes((NiNode*) pkChild, kMap);
            }
            else
            {
                // Child is some other kind of object; don't remove.
                bRemove = false;
            }

            if (bRemove)
            {
                NiNode* pkChildNode = NiDynamicCast(NiNode, pkChild);
                if (pkChildNode)
                {
                    // Save the children of the node.
                    for (unsigned int uj = 0;
                        uj < pkChildNode->GetArrayCount(); uj++)
                    {
                        aspSaveChildren.Add(pkChildNode->DetachChildAt(uj));
                    }
                }
                pkNode->DetachChildAt(ui);
            }
            else
            {
                aspSaveChildren.Add(pkNode->DetachChildAt(ui));
            }
        }
    }

    // Re-attach the saved children to the node.
    NIASSERT(pkNode->GetChildCount() == 0);
    pkNode->RemoveAllChildren();
    for (ui = 0; ui < aspSaveChildren.GetSize(); ui++)
        pkNode->AttachChild(aspSaveChildren.RemoveAt(ui));

    // If pkNode is an NiSwitchNode, restore the saved index.
    if ((iIndex > -1) && (pkNode->GetChildCount() != 0))
    {
        ((NiSwitchNode*) pkNode)->SetIndex(iIndex);
    }

    // Return true if node is childless and can be removed.
    bool bTemp;
    if (pkNode->GetChildCount() == 0 && !CheckForExtraDataTags(pkNode) &&
        !kMap.GetAt(pkNode, bTemp))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiOptimize::CollectMultiMtlNodes(NiNode* pkNode,
    NiNodeArray& apkNodes, const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Detects the presence of multi-material nodes that can be removed by
    // RemoveMultiMtlNodes and stores a pointer to each in apkNodes.

    const char* pcName = pkNode->GetName();
    bool bTemp;
    if (pcName && strcmp(pcName, "__NDL_MultiMtl_Node") == 0 &&
        !CheckForExtraDataTags(pkNode) && !kMap.GetAt(pkNode, bTemp) &&
        !pkNode->GetControllers())
    {
        apkNodes.Add(pkNode);
    }

    // Recurse over children.
    for (unsigned int ui=0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                CollectMultiMtlNodes((NiNode*) pkChild, apkNodes, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::RemoveMultiMtlNodes(NiNode* pkNode,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Recursively removes all multi-material nodes. A node will not be
    // removed if it is singled out by BuildHashTable, has an "sgoKeep" extra
    // data tag, or has any controllers.

    // Returns: true, pkNode should be deleted.
    //          false, pkNode should not be deleted.

    // If pkNode is an NiSwitchNode, save the active index.
    int iIndex = -1;
    if (NiIsKindOf(NiSwitchNode, pkNode))
    {
        iIndex = ((NiSwitchNode*) pkNode)->GetIndex();
    }

    // Recurse over children.
    NiTObjectArray<NiAVObjectPtr> aspSaveChildren(50);
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            bool bRemove;
            if (NiIsKindOf(NiNode, pkChild))
            {
                // Recurse on NiNode child.
                bRemove = RemoveMultiMtlNodes((NiNode*) pkChild, kMap);
            }
            else
            {
                // Child is some other kind of object; don't remove.
                bRemove = false;
            }

            if (bRemove)
            {
                NiNode* pkChildNode = NiDynamicCast(NiNode, pkChild);
                if (pkChildNode)
                {
                    // Propagate the AppCulled flag to children.
                    bool bAppCulled = pkChildNode->GetAppCulled();

                    // Save the children of the node.
                    for (unsigned int uj = 0;
                        uj < pkChildNode->GetArrayCount(); uj++)
                    {
                        NiAVObjectPtr spGrandchild =
                            pkChildNode->DetachChildAt(uj);
                        if (bAppCulled || spGrandchild->GetAppCulled())
                        {
                            spGrandchild->SetAppCulled(true);
                        }
                        aspSaveChildren.Add(spGrandchild);
                    }
                }
                pkNode->DetachChildAt(ui);
            }
            else
            {
                aspSaveChildren.Add(pkNode->DetachChildAt(ui));
            }
        }
    }

    // Re-attach the saved children to the node.
    NIASSERT(pkNode->GetChildCount() == 0);
    pkNode->RemoveAllChildren();
    for (ui = 0; ui < aspSaveChildren.GetSize(); ui++)
        pkNode->AttachChild(aspSaveChildren.RemoveAt(ui));

    // If pkNode is an NiSwitchNode, restore the saved index.
    if ((iIndex > -1) && (pkNode->GetChildCount() != 0))
    {
        ((NiSwitchNode*) pkNode)->SetIndex(iIndex);
    }

    // Return true if pkNode is a multi-material node that can be removed.
    const char* pcName = pkNode->GetName();
    bool bTemp;
    if (pcName && strcmp(pcName, "__NDL_MultiMtl_Node") == 0 &&
        !CheckForExtraDataTags(pkNode) && !kMap.GetAt(pkNode, bTemp) &&
        !pkNode->GetControllers())
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiOptimize::CollectSingleChildNodes(NiNode* pkNode,
    NiNodeArray& apkNodes, const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Detects the presence of single-child nodes that can be removed by
    // RemoveSingleChildNodes and stores a pointer to each in apkNodes.

    bool bChildCtlrFound = false;
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild && pkChild->GetControllers())
        {
            bChildCtlrFound = true;
            break;
        }
    }

    bool bTemp;
    if (!bChildCtlrFound && !pkNode->GetControllers() &&
        NiIsExactKindOf(NiNode, pkNode) && pkNode->GetChildCount() == 1 &&
        !CheckForExtraDataTags(pkNode) && kMap.GetAt(pkNode, bTemp))
    {
        apkNodes.Add(pkNode);
    }

    // Recurse over children.
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                CollectSingleChildNodes((NiNode*) pkChild, apkNodes, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::RemoveSingleChildNodes(NiNode* pkNode,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Recursively removes all single-child nodes in the scene of exact type
    // NiNode, propagating their properties, transforms, extra data, and
    // names to the single child. A node will not be removed if it is
    // singled out by BuildHashTable, has an "sgoKeep" extra data tag, has
    // any controllers, or whose child has any controllers.

    // Returns: true, parent should delete the object.
    //          false, parent should keep the object.

    // If pkNode is an NiSwitchNode, save the active index.
    int iIndex = -1;

    if (NiIsKindOf(NiRoom, pkNode))
    {
        // Optimization will not be applied to portal rooms which would 
        // require fixing up fixture pointers.
        return false;
    }
    else if (NiIsKindOf(NiSwitchNode, pkNode))
    {
        iIndex = ((NiSwitchNode*) pkNode)->GetIndex();
    }

    // Recurse over children.
    NiTObjectArray<NiAVObjectPtr> aspSaveChildren(50);
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            bool bRemove;
            if (NiIsKindOf(NiNode, pkChild))
            {
                // Recurse on NiNode child.
                bRemove = RemoveSingleChildNodes((NiNode*) pkChild, kMap);
            }
            else
            {
                // Child is some other kind of object; don't remove.
                bRemove = false;
            }

            if (bRemove)
            {
                NiNode* pkChildNode = NiDynamicCast(NiNode, pkChild);
                if (pkChildNode)
                {
                    // Propagate properties, transforms, extra data, and name
                    // to the child and remove the node.
                    for (unsigned int uj = 0;
                        uj < pkChildNode->GetArrayCount(); uj++)
                    {
                        NiAVObject* pkGrandchild = pkChildNode->GetAt(uj);
                        if (pkGrandchild)
                        {
                            PropagateProperties(pkChildNode, pkGrandchild);
                            PropagateTransforms(pkChildNode, pkGrandchild);

                            for (unsigned int uk=0;
                                uk < pkChildNode->GetExtraDataSize(); uk++)
                            {
                                NiExtraData* pkExtra =
                                    pkChildNode->GetExtraDataAt(
                                    (unsigned short)uk);
                                pkGrandchild->AddExtraData(pkExtra);
                            }

                            if (!CheckForExtraDataTags(pkGrandchild))
                            {
                                pkGrandchild->SetName(pkChildNode->GetName());
                            }
                            if (pkChildNode->GetAppCulled() ||
                                pkGrandchild->GetAppCulled())
                            {
                                pkGrandchild->SetAppCulled(true);
                            }
                        }
                        aspSaveChildren.Add(pkChildNode->DetachChildAt(uj));
                    }
                }
                pkNode->DetachChildAt(ui);
            }
            else
            {
                aspSaveChildren.Add(pkNode->DetachChildAt(ui));
            }
        }
    }

    // Re-attach the saved children to the node.
    NIASSERT(pkNode->GetChildCount() == 0);
    pkNode->RemoveAllChildren();
    for (ui = 0; ui < aspSaveChildren.GetSize(); ui++)
    {
        pkNode->AttachChild(aspSaveChildren.RemoveAt(ui));
    }

    // If pkNode is an NiSwitchNode, restore the saved index.
    if ((iIndex > -1) && (pkNode->GetChildCount() != 0))
    {
        ((NiSwitchNode*) pkNode)->SetIndex(iIndex);
    }

    bool bChildCtlrFound = false;
    bool bChildExtraDataFound = false;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (pkChild->GetControllers())
            {
                bChildCtlrFound = true;
            }

            for (unsigned int uj = 0; uj < pkChild->GetExtraDataSize(); uj++)
            {
                NiExtraData* pkExtra = pkChild->GetExtraDataAt(
                    (unsigned short)uj);
                if (pkNode->GetExtraData(pkExtra->GetName()))
                {
                    bChildExtraDataFound = true;
                }
            }

            if (bChildCtlrFound && bChildExtraDataFound)
            {
                break;
            }
        }

    }
    bool bTemp;
    if (!bChildCtlrFound && !bChildExtraDataFound && !pkNode->GetControllers()
        && NiIsExactKindOf(NiNode, pkNode) && pkNode->GetChildCount() == 1 &&
        !CheckForExtraDataTags(pkNode) && !kMap.GetAt(pkNode, bTemp))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiOptimize::MergeSiblingNodes(NiNode* pkNode,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
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

    // Recurse through children.
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild && NiIsKindOf(NiNode, pkChild))
        {
            MergeSiblingNodes((NiNode*) pkChild, kMap);
        }
    }

    // Do not merge if this node is not of exact type NiNode.
    if (!NiIsExactKindOf(NiNode, pkNode))
    {
        return;
    }

    // Merge sibling nodes.
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild1 = pkNode->GetAt(ui);
        if (pkChild1 && NiIsExactKindOf(NiNode, pkChild1))
        {
            NiNode* pkChildNode1 = (NiNode*) pkChild1;
            NiTimeController* pkController = pkChildNode1->GetControllers();
            bool bChildCtlrFound = false;
            unsigned int uiChild;
            for (uiChild = 0; uiChild < pkChildNode1->GetArrayCount();
                uiChild++)
            {
                NiAVObject* pkChild = pkChildNode1->GetAt(uiChild);
                if (pkChild && pkChild->GetControllers())
                {
                    bChildCtlrFound = true;
                    break;
                }
            }
            bool bTemp;
            if (bChildCtlrFound || pkController ||
                pkChildNode1->GetExtraDataSize() ||
                CheckForExtraDataTags(pkChildNode1) ||
                kMap.GetAt(pkChildNode1, bTemp))
            {
                continue;
            }

            for (unsigned int uj = ui + 1; uj < pkNode->GetArrayCount(); uj++)
            {
                NiAVObject* pkChild2 = pkNode->GetAt(uj);
                if (pkChild2 && NiIsExactKindOf(NiNode, pkChild2))
                {
                    NiNode* pkChildNode2 = (NiNode*) pkChild2;
                    pkController = pkChildNode2->GetControllers();
                    bChildCtlrFound = false;
                    for (uiChild = 0; uiChild < pkChildNode2->GetArrayCount();
                        uiChild++)
                    {
                        NiAVObject* pkChild = pkChildNode2->GetAt(uiChild);
                        if (pkChild && pkChild->GetControllers())
                        {
                            bChildCtlrFound = true;
                            break;
                        }
                    }
                    bool bTemp;
                    if (!bChildCtlrFound && !pkController &&
                        !pkChildNode2->GetExtraDataSize() &&
                        !CheckForExtraDataTags(pkChildNode2) &&
                        !kMap.GetAt(pkChildNode2, bTemp) &&
                        CompareProperties(pkChildNode1, pkChildNode2) &&
                        pkChild1->GetAppCulled() == pkChild2->GetAppCulled())

                    {
                        // Propagate transforms from pkChildNode1 to its
                        // children and reset the transforms on the node.
                        unsigned int uiChild;
                        for (uiChild = 0;
                            uiChild < pkChildNode1->GetArrayCount();
                            uiChild++)
                        {
                            NiAVObject* pkChild =
                                pkChildNode1->GetAt(uiChild);
                            if (pkChild)
                            {
                                PropagateTransforms(pkChildNode1, pkChild);
                            }
                        }
                        pkChildNode1->SetTranslate(NiPoint3::ZERO);
                        pkChildNode1->SetRotate(NiMatrix3::IDENTITY);
                        pkChildNode1->SetScale(1.0f);

                        // Propagate transforms from pkChildNode2 to its
                        // children and move those children under
                        // pkChildNode1.
                        for (uiChild = 0;
                            uiChild < pkChildNode2->GetArrayCount();
                            uiChild++)
                        {
                            NiAVObject* pkChild =
                                pkChildNode2->GetAt(uiChild);
                            if (pkChild)
                            {
                                PropagateTransforms(pkChildNode2, pkChild);
                                pkChildNode1->AttachChild(pkChild);
                                pkChildNode2->DetachChild(pkChild);
                            }
                        }
                        pkNode->DetachChild(pkChild2);
                    }
                }
            }
        }
    }
    pkNode->CompactChildArray();
}
//---------------------------------------------------------------------------
void NiOptimize::OptimizeMeshes(NiNode* pkNode, const float cfSizeModifier, 
    const float cfNormalModifier, const float cfPositionModifier, 
    const NiUInt32 uiVertexCacheSize)
{
    // Recurses through the scene graph, removing degenerate triangles and
    // collapsing vertices of all mesh objects, deleting them if all
    // their triangles are degenerate.

    // Recurse over children.
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild && 
            !NiOptimize::GetNoOptimizeMeshExtraData(pkChild))
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                // Continue recursing.
                OptimizeMeshes((NiNode*) pkChild, cfSizeModifier, 
                    cfNormalModifier, cfPositionModifier, uiVertexCacheSize);
            }
            else if (NiIsKindOf(NiMesh, pkChild) &&
                !NiIsKindOf(NiPSParticleSystem, pkChild))
            {
                NiMorphWeightsController* pkMorpher = 
                    NiGetController(NiMorphWeightsController, pkChild);

                if (pkMorpher == NULL)
                {
                    // Optimize the mesh.
                    bool bRemove = OptimizeMesh((NiMesh*) pkChild, 
                        cfSizeModifier, cfNormalModifier, cfPositionModifier, 
                        uiVertexCacheSize);
                    if (bRemove)
                        pkNode->DetachChild(pkChild);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::MergeSiblingMeshes(NiNode* pkNode, bool bNoMergeOnAlpha,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{    
    if (bNoMergeOnAlpha)
    {
        // Check if alpha blending is enabled for the node and whether or
        // not sorters are allowed. If so, then recursive optimization
        // should not be performed. This prevents a merge of two siblings
        // with disjoint mesh but the same alpha property. Such a merge
        // causes a problem when an alpha sorter is used (the bounding
        // sphere center for the alpha blended objects may cause incorrect
        // drawing order).
        NiAlphaProperty* pkAlphaProp = (NiAlphaProperty*)
            pkNode->GetProperty(NiProperty::ALPHA);

        if (pkAlphaProp && pkAlphaProp->GetAlphaBlending() &&
            !pkAlphaProp->GetNoSorter())
        {
            return;
        }
    }

    // Recurse over children.
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild && NiIsKindOf(NiNode, pkChild))
        {  
            MergeSiblingMeshes((NiNode*)pkChild, bNoMergeOnAlpha,
                kMap);
        }
    }

    // Do not merge if this node is not of exact type NiNode.
    if (!NiIsExactKindOf(NiNode, pkNode))
        return;

    // Collect sibling meshes that can be merged.
    NiTPrimitiveSet<NiMesh*> kMergeArray;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (!pkChild || !NiIsKindOf(NiMesh, pkChild))
            continue;

        NiMesh* pkMesh = (NiMesh*)pkChild;
        if (bNoMergeOnAlpha)
        {
            NiAlphaProperty* pkAlphaProp = 
                (NiAlphaProperty*)pkMesh->GetProperty(NiProperty::ALPHA);

            if (pkAlphaProp && pkAlphaProp->GetAlphaBlending() &&
                !pkAlphaProp->GetNoSorter())
            {
                continue;
            }
        }

        NiTimeController* pkCtlr = pkMesh->GetControllers();
        bool bTemp;
        if (pkCtlr || CheckForExtraDataTags(pkMesh) ||
            kMap.GetAt(pkMesh, bTemp))
        {
            continue;
        }

        // Don't Merge Skinned Objects
        if (NiGetModifier(NiSkinningMeshModifier, pkMesh))
        {
            continue;
        }

        // Don't merge particle systems.
        if (NiIsKindOf(NiPSParticleSystem, pkMesh))
        {
            continue;
        }

        // At this point we can flag the mesh as a candidate for merging.
        kMergeArray.Add(pkMesh);
    }

    NiMeshPtrArray kMeshesToMerge;
    for (ui = 0; ui < kMergeArray.GetSize(); ui++)
    {
        NiMesh* pkMeshA = kMergeArray.GetAt(ui);
        if (!pkMeshA)
            continue;

        kMeshesToMerge.RemoveAll();
        unsigned int uj = ui + 1;
        while (uj < kMergeArray.GetSize())
        {
            NiMesh* pkMeshB = kMergeArray.GetAt(uj);
            if (!pkMeshB)
            {
                uj++;
                continue;
            }

            if (CanMergeMesh(pkMeshA, pkMeshB))
            {
                // Add the mesh to the list of meshes to merge with mesh A.
                kMeshesToMerge.Add(pkMeshB);
                pkNode->DetachChild(pkMeshB);
                kMergeArray.OrderedRemoveAt(uj);
            }
            else
            {
                uj++;
            }
        }

        // Merge the list of meshes.        
        MergeMeshes(pkMeshA, kMeshesToMerge);
    }

    pkNode->CompactChildArray();
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveExtraUVSets(NiAVObject* pkObject)
{
    // Removes all UV sets in mesh that are not used by any
    // NiTexturingProperty attached to the current or a higher node.


    // First build a map of texture properties to all the meshes that use it.
    // Note, this method will also prune meshes with no texture property and
    // automatically remove their uv sets.
    NiOptimize::TextureMeshMap kTextureMeshMap;
    BuildTexturePropertyMeshMap(pkObject, kTextureMeshMap);

    NiTPrimitiveArray<NiMesh*>* pkMeshes;
    NiTexturingProperty* pkTexProp;
    NiTMapIterator kIter = kTextureMeshMap.GetFirstPos();

    // For each texturing property, we need to look through all the meshes with
    // that property and remove/reshuffle the data streams that do not
    // reference a map on the texturing property.
    while (kIter)
    {
        kTextureMeshMap.GetNext(kIter, pkTexProp, pkMeshes);

        // Check the maps in the texture property to see if they all 
        // reference a stream. Remove streams that are not referenced.        

        unsigned int uiNumMeshes = pkMeshes->GetSize();
        for (unsigned int ui = 0; ui < uiNumMeshes; ui++)
        {
            NiMesh* pkMesh = pkMeshes->GetAt(ui);

            NIASSERT(pkMesh->IsValid());

            unsigned int uiCurrStreamRef = 0; 

            while (uiCurrStreamRef < pkMesh->GetStreamRefCount())
            {
                NiDataStreamRef* pkStreamRef = 
                    pkMesh->GetStreamRefAt(uiCurrStreamRef);

                NIASSERT(pkStreamRef->GetElementDescCount() != 0);

                // If the data stream is packed, we aren't going to re-pack it.
                if (pkStreamRef->GetElementDescCount() > 1 || 
                    pkStreamRef->GetSemanticNameAt(0) != 
                    NiCommonSemantics::TEXCOORD())
                {
                    uiCurrStreamRef++;
                }
                else 
                {
                    // We have a valid texture stream. Now see if a map 
                    // references it.
                    unsigned int uiTexCoordSet = 
                        pkStreamRef->GetSemanticIndexAt(0);

                    const NiTexturingProperty::NiMapArray& kMapArray = 
                        pkTexProp->GetMaps();

                    unsigned int uiNumMaps = kMapArray.GetSize();
                    bool bFound = false;
                    for (unsigned int uiCurrMap = 0; uiCurrMap < uiNumMaps;
                        uiCurrMap++)
                    {
                        const NiTexturingProperty::Map* pkMap = 
                            kMapArray.GetAt(uiCurrMap);

                        if (!pkMap)
                            continue;

                        if (pkMap->GetTextureIndex() == uiTexCoordSet) 
                        {
                            bFound = true;
                            uiCurrStreamRef++;
                            break;
                        }
                    }

                    //Maybe it is using a shader map
                    if(!bFound)
                    {

                        unsigned int uiNumMaps = 
                            pkTexProp->GetShaderArrayCount();

                        for (unsigned int uiCurrMap = 0; uiCurrMap < uiNumMaps;
                            uiCurrMap++)
                        {
                            const NiTexturingProperty::Map* pkMap = 
                                pkTexProp->GetShaderMap(uiCurrMap);

                            if (!pkMap)
                                continue;

                            unsigned int uiTextureIndex = 
                                pkMap->GetTextureIndex();
                            if (uiTextureIndex == uiTexCoordSet) 
                            {
                                bFound = true;
                                uiCurrStreamRef++;
                                break;
                            }
                        }
                    }

                    // If we didn't find a corresponding map, remove the 
                    // stream.
                    if (!bFound)
                    {
                        pkStreamRef->SetDataStream(NULL);
                        pkMesh->RemoveStreamRef(pkStreamRef);
                    }
                }
            }

            pkMesh->ResetModifiers();
            NIASSERT(pkMesh->IsValid());
        }       
    }

    // Clean up our bookkeeping data.
    kIter = kTextureMeshMap.GetFirstPos();
    while (kIter)
    {
        kTextureMeshMap.GetNext(kIter, pkTexProp, pkMeshes);
        pkMeshes->RemoveAll();
        NiDelete pkMeshes;
    }

    kTextureMeshMap.RemoveAll();
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveUnnecessaryNormals(NiAVObject* pkObject)
{
    // Recursively removes normals that are not needed. Will not remove
    // normals from scenes without lights.

    if (NiIsKindOf(NiMesh, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*)pkObject;

        if (CanRemoveNormals(pkMesh))
        {            
            NIASSERT(pkMesh->IsValid());

            // Remove all the data streams that are used in lighting
            // calculations.
            NiDataStreamRef* pkRef = NULL;
            pkRef = pkMesh->FindStreamRef(NiCommonSemantics::NORMAL(), 0);
            if (pkRef && 
                !(pkRef->GetAccessMask() & NiDataStream::ACCESS_CPU_READ))
            {
                pkMesh->RemoveStreamRef(pkRef);
                pkRef = pkMesh->FindStreamRef(
                    NiCommonSemantics::NORMAL_BP(), 0);
                if (pkRef)
                {
                    pkMesh->RemoveStreamRef(pkRef);
                }
            }

            pkRef = pkMesh->FindStreamRef(NiCommonSemantics::NORMAL_BP(), 0);
            if (pkRef && 
                !(pkRef->GetAccessMask() & NiDataStream::ACCESS_CPU_READ))
            {
                pkMesh->RemoveStreamRef(pkRef);
            }

            pkRef = pkMesh->FindStreamRef(NiCommonSemantics::BINORMAL(), 0);
            if (pkRef && 
                !(pkRef->GetAccessMask() & NiDataStream::ACCESS_CPU_READ))
            {
                pkMesh->RemoveStreamRef(pkRef);
                pkRef = pkMesh->FindStreamRef(
                    NiCommonSemantics::BINORMAL_BP(), 0);
                if (pkRef)
                {
                    pkMesh->RemoveStreamRef(pkRef);
                }
            }

            pkRef = pkMesh->FindStreamRef(NiCommonSemantics::BINORMAL_BP(), 0);
            if (pkRef && 
                !(pkRef->GetAccessMask() & NiDataStream::ACCESS_CPU_READ))
            {
                pkMesh->RemoveStreamRef(pkRef);
            }

            pkRef = pkMesh->FindStreamRef(NiCommonSemantics::TANGENT(), 0);
            if (pkRef && 
                !(pkRef->GetAccessMask() & NiDataStream::ACCESS_CPU_READ))
            {
                pkMesh->RemoveStreamRef(pkRef);
                pkRef = pkMesh->FindStreamRef(
                    NiCommonSemantics::TANGENT_BP(), 0);
                if (pkRef)
                {
                    pkMesh->RemoveStreamRef(pkRef);
                }
            }

            pkRef = pkMesh->FindStreamRef(NiCommonSemantics::TANGENT_BP(), 0);
            if (pkRef && 
                !(pkRef->GetAccessMask() & NiDataStream::ACCESS_CPU_READ))
            {
                pkMesh->RemoveStreamRef(pkRef);
            }

            NIASSERT(pkMesh->IsValid());
        }
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        // Recurse over children.
        NiNode* pkNode = (NiNode*) pkObject;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                RemoveUnnecessaryNormals(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::CollectUnnecessaryVisControllers(NiAVObject* pkObject,
    NiVisControllerArray& apkCtlrs)
{
    // Detects the presence of any visibility controllers in the scene whose
    // keys all have the same visibility flag. Adds a pointer to each
    // offending controller to apkCtlrs.

    // Returns: true, if spurious visibility controllers are present.
    //          false, otherwise.

    NiVisController* pkBoolCtlr = NiGetController(NiVisController, pkObject);
    if (pkBoolCtlr)
    {
        unsigned int uiNumBoolKeys;

        NiInterpolator* pkInterp = pkBoolCtlr->GetInterpolator();
        if (NiIsKindOf(NiBoolInterpolator, pkInterp))
        {
            NiBoolData* pkBoolData = ((NiBoolInterpolator*)pkInterp)->
                GetBoolData();

            bool bSame = true;

            // If there is no data then the Iterator has been collapsed
            if (pkBoolData)
            {
                NiBoolKey::KeyType eType;
                unsigned char ucSize;
                NiBoolKey* pkBoolKeys = pkBoolData->GetAnim(uiNumBoolKeys,
                    eType, ucSize);

                NIASSERT(eType == NiAnimationKey::STEPKEY);

                // There are only Stepped Bool Keys
                NiStepBoolKey* pkStepKeys = (NiStepBoolKey*)pkBoolKeys;
                bool bValue = pkStepKeys->GetBool();

                // Make sure each value is the same as the first.
                for (unsigned int ui = 1; bSame && ui < uiNumBoolKeys; ui++)
                {
                    if (pkStepKeys[ui].GetBool() != bValue)
                        bSame = false;
                }
            }

            // Remove the controller if all are the same
            if (bSame)
            {
                apkCtlrs.Add(pkBoolCtlr);
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                CollectUnnecessaryVisControllers(pkChild, apkCtlrs);
            }
        }
    }

    return (apkCtlrs.GetEffectiveSize() > 0);
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveUnnecessaryVisControllers(NiAVObject* pkObject)
{
    // Removes all visibility controllers whose keys all have the same
    // visibility data. If that data is not the same as the AppCulled flag of
    // pkObject, pkObject's AppCulled flag is updated to reflect the
    // visibility data.

    NiVisControllerArray apkCtlrs(100);
    CollectUnnecessaryVisControllers(pkObject, apkCtlrs);
    apkCtlrs.Compact();
    apkCtlrs.UpdateSize();

    for (unsigned int ui = 0; ui < apkCtlrs.GetSize(); ui++)
    {
        NiVisController* pkBoolCtlr = apkCtlrs.GetAt(ui);

        // Update the controller to set the value
        pkBoolCtlr->Update(0.0);

        // Remove the Controller
        NiObjectNET* pkTarget = pkBoolCtlr->GetTarget();
        pkTarget->RemoveController(pkBoolCtlr);
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveNames(NiAVObject* pkObject)
{
    // Removes the name string from all objects and their material
    // properties.

    NiProperty* pkMat = pkObject->GetProperty(NiProperty::MATERIAL);

    if (pkMat)
    {
        pkMat->SetName(NULL);
    }

    pkObject->SetName(NULL);

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RemoveNames(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::NameUnnamedObjects(NiAVObject* pkObject)
{
    // Adds unique names to all objects in the scene that have none.

    static unsigned int uiNameCounter = 0;

    if (!pkObject->GetName())
    {
        char acName[NI_MAX_PATH];
        NiSprintf(acName, NI_MAX_PATH, "SgoUnnamedObject %u", 
            uiNameCounter++);
        pkObject->SetName(acName);
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                NameUnnamedObjects(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveBadTexturingProps(NiAVObject* pkObject)
{
    // Inspects NiTexturingProperties on a mesh and removes any maps with
    // invalid image data or maps with which the mesh contains no texture
    // coordinates. Note, does not handle shader maps.

    // Remove map if bad, but deep copy first the texturing property. Remove
    // from the mesh, add deep copied map.

    if (NiIsKindOf(NiMesh, pkObject) &&
        !NiIsKindOf(NiPSParticleSystem, pkObject))
    {

        NiTexturingProperty* pkTexProp = FindTexturingProperty(pkObject);
        NiMesh* pkMesh = (NiMesh*)pkObject;
        if (pkTexProp)
        {            
            NiTexturingProperty::NiMapArray& kMaps = 
                const_cast<NiTexturingProperty::NiMapArray&>(
                pkTexProp->GetMaps());

            NiTexturingPropertyPtr spTexPropCopy = NULL;

            unsigned int uiNumMaps = kMaps.GetSize();            
            for (unsigned int ui = 0; ui < uiNumMaps; ui++)
            {
                NiTexturingProperty::Map* pkMap = kMaps.GetAt(ui);
                if (pkMap && (!pkMap->GetTexture() || 
                    !HasTexCoordSet(pkMesh, pkMap->GetTextureIndex())))
                {
                    if (!spTexPropCopy)
                    {
                        // Deep copy the texturing property since it may be
                        // shared and remove the map on the deep copy.
                        spTexPropCopy = NiSmartPointerCast(NiTexturingProperty,
                            pkTexProp->CreateDeepCopy());
                    }

                    const NiTexturingProperty::NiMapArray& kMapArray = 
                        spTexPropCopy->GetMaps();
                    NiTexturingProperty::Map* pkCopyMap = kMapArray.GetAt(ui);
                    NiDelete pkCopyMap;
                    spTexPropCopy->SetMap(ui, NULL);

                }
            }

            if (spTexPropCopy)
            {
                // Now replace the texturing property on the mesh with the
                // corrected property.
                pkMesh->DetachProperty(pkTexProp);
                pkMesh->AttachProperty(spTexPropCopy);
            }
        }
    }
    else
    {
        // Recurse over children.
        NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
        if (pkNode)
        {
            for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
            {
                NiAVObject* pkChild = pkNode->GetAt(ui);
                if (pkChild)
                {
                    RemoveBadTexturingProps(pkChild);
                }
            }
        }
    }    
}
//---------------------------------------------------------------------------
bool NiOptimize::HasTexCoordSet(NiMesh* pkMesh, unsigned int uiTexChannel)
{
    NiUInt32 uiStreamRefCount = pkMesh->GetStreamRefCount();
    for (NiUInt32 ui = 0; ui < uiStreamRefCount; ui++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->GetStreamRefAt(ui);

        NiUInt32 uiElemRefCount = pkStreamRef->GetElementDescCount();
        for (NiUInt32 uj = 0; uj < uiElemRefCount; uj++)
        {
            if (pkStreamRef->GetSemanticNameAt(uj) == 
                NiCommonSemantics::TEXCOORD() &&
                pkStreamRef->GetSemanticIndexAt(uj) == uiTexChannel)
            {
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveUnnecessaryExtraData(NiAVObject* pkObject,
    bool bRemoveKeepTags)
{
    // Removes extra data strings that are empty or that contain the strings
    // "zMode" or "billboard". If bRemoveKeepTags is true, extra data strings
    // that contain the string "sgoKeep" are also removed.

    NiTPrimitiveArray<unsigned int> apkRemoveList(4, 4);

    for (unsigned int i=0; i < pkObject->GetExtraDataSize(); i++)
    {
        NiExtraData* pkExtraData = pkObject->GetExtraDataAt((unsigned short)i);

        NiStringExtraData* pkStrExData =
            NiDynamicCast(NiStringExtraData, pkExtraData);
        if (pkStrExData)
        {
            const char* pcString = pkStrExData->GetValue();
            if (pcString)
            {
                if (strcmp(pcString, "") == 0 ||
                    strcmp(pcString, "zMode") == 0 ||
                    strcmp(pcString, "billboard") == 0 ||
                    strstr(pcString, "NiBoneLOD#") != NULL ||
                    (bRemoveKeepTags && (strcmp(pcString, "sgoKeep") == 0 ||
                    strcmp(pcString, "NiOptimizeKeep") == 0)))
                {
                    apkRemoveList.Add(i);
                }
            }
            else
            {
                apkRemoveList.Add(i);
            }
        }
    }

    // Remove from end of list, to preserve validity of index for remaining
    //    entries.
    for (int j = ((int)(apkRemoveList.GetSize()) - 1); j >= 0; j--)
    {
        pkObject->RemoveExtraDataAt((unsigned short)apkRemoveList.GetAt(j));
    }

    // Remove bones per partition integer extra data.
    pkObject->RemoveExtraData(NI_BONES_PER_PARTITION_KEY);

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                RemoveUnnecessaryExtraData(pkChild, bRemoveKeepTags);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveUnnecessaryLookAtControllers(NiAVObject* pkObject)
{
    // Removes NiLookAtControllers in the scene if there are no keyframe
    // controllers or path controllers on either the target or the LookAt
    // of the controller or above them in the scene graph.

    NiTransformController* pkTransCtlr = NiGetController(NiTransformController,
        pkObject);
    if (pkTransCtlr)
    {
        NiInterpolator* pkInterp = pkTransCtlr->GetInterpolator(0);
        NiLookAtInterpolator* pkLookAtInterp = NiDynamicCast(
            NiLookAtInterpolator, pkInterp);
        if (pkLookAtInterp)
        {
            NiAVObject* pkLookAt = pkLookAtInterp->GetLookAt();
            if (pkLookAt &&
                !CheckForTransformControllers(pkLookAt, true) &&
                !pkLookAtInterp->GetTranslateInterpolator() &&
                !pkLookAtInterp->GetScaleInterpolator())
            {
                pkObject->RemoveController(pkTransCtlr);
            }
        }
    }


    // Recurse on children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RemoveUnnecessaryLookAtControllers(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveHiddenBoneMesh(NiAVObject* pkObject,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Removes any NiRenderObject object that is a child of a bone and whose
    // AppCulled flag is true.

    bool bTemp;
    if (kMap.GetAt(pkObject, bTemp))
    {
        // If pkObject is of type NiRenderObject and is hidden, remove it from
        // the scene graph.
        if (NiIsKindOf(NiRenderObject, pkObject) && pkObject->GetAppCulled())
        {
            NiNode* pkParent = pkObject->GetParent();
            if (pkParent)
            {
                pkParent->DetachChild(pkObject);
                return;
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        // Recurse over children.
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RemoveHiddenBoneMesh(pkChild, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceAnimationKeys(NiAVObject* pkObject, float fTolerance)
{
    // Reduces redundant animation keys for all NiTransformControllers in the
    // scene graph rooted at pkObject. fTolerance specifies a ratio of
    // how far off keys are allowed to be to still be considered redundant.

    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        NiTransformController* pkKFCtlr = NiDynamicCast(NiTransformController,
            pkCtlr);
        if (pkKFCtlr)
        {
            ReduceAnimationKeys(pkKFCtlr, fTolerance);
        }

        pkCtlr = pkCtlr->GetNext();
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                ReduceAnimationKeys(pkChild, fTolerance);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceAnimationKeys(NiTransformController* pkCtlr,
    float fTolerance)
{
    // Reduces redundant animation keys for the specified
    // NiTransformController fTolerance specifies a ratio of how far off keys
    // are allowed to be to still be considered redundant.

    NIASSERT(pkCtlr);

    // Obtain the target object which is needed to ensure
    // transforms (out of Maya) if after completely reducing
    // keys 
    NiAVObject* pkObject = NiDynamicCast(NiAVObject, pkCtlr->GetTarget());
    NIASSERT(pkObject);

    NiInterpolator* pkInterp = pkCtlr->GetInterpolator();
    if (NiIsKindOf(NiTransformInterpolator, pkInterp))
    {
        NiTransformInterpolator* pkTransformInterp = 
            (NiTransformInterpolator*) pkInterp;

        NiTransformData* pkData = pkTransformInterp->GetTransformData();
        if (pkData)
        {
            // Reduce rotation keys.
            bool bKeysReduced = false;
            unsigned int uiNumRotKeys;
            NiRotKey::KeyType eRotKeyType;
            unsigned char ucRotSize;
            NiRotKey* pkRotKeys = pkData->GetRotAnim(uiNumRotKeys, 
                eRotKeyType, ucRotSize);
            unsigned int uiNumNewRotKeys = 0;
            NiRotKey* pkNewRotKeys = NULL;
            if (uiNumRotKeys > 1 || eRotKeyType == NiRotKey::EULERKEY)
            {
                switch (eRotKeyType)
                {
                case NiRotKey::LINKEY:
                    ReduceLinRotKeys(uiNumRotKeys, pkRotKeys, 
                        uiNumNewRotKeys, pkNewRotKeys, 
                        fTolerance, pkObject);
                    bKeysReduced = true;
                    break;
                case NiRotKey::BEZKEY:
                    ReduceBezRotKeys(uiNumRotKeys, pkRotKeys, 
                        uiNumNewRotKeys, pkNewRotKeys, 
                        fTolerance, pkObject);
                    bKeysReduced = true;
                    break;
                case NiRotKey::TCBKEY:
                    // TCB keys are not reduced, since redundant 
                    // TCB keys only appear when placed there 
                    // by the artist.
                    break;
                case NiRotKey::EULERKEY:
                    // Note - pkObject not passed in.
                    ReduceEulerRotKeys(uiNumRotKeys, pkRotKeys, 
                        fTolerance);

                    // EulerRotKeys replace data within themselves.  
                    // pkNewRotKeys will always be NULL since it is 
                    // not set within ReduceEulerRotKeys. Setting this 
                    // flag to false is redundant, but it insures that 
                    // the key data is not replaced with NULL.
                    bKeysReduced = false;
                    break;
                default:
                    break;
                }
            }
            if (bKeysReduced)
            {
                // Replace the animation data through the interpolator so
                // it can update it internal variables
                pkTransformInterp->ReplaceRotData(pkNewRotKeys, 
                    uiNumNewRotKeys, eRotKeyType);
            }

            // Reduce position keys.
            bKeysReduced = false;
            unsigned int uiNumPosKeys;
            NiPosKey::KeyType ePosKeyType;
            unsigned char ucPosSize;
            NiPosKey* pkPosKeys = pkData->GetPosAnim(uiNumPosKeys,
                ePosKeyType, ucPosSize);
            unsigned int uiNumNewPosKeys = 0;
            NiPosKey* pkNewPosKeys = NULL;

            if (uiNumPosKeys > 1)
            {
                switch (ePosKeyType)
                {
                case NiPosKey::LINKEY:
                    ReduceLinPosKeys(uiNumPosKeys, pkPosKeys, 
                        uiNumNewPosKeys, pkNewPosKeys, fTolerance, 
                        pkObject);
                    bKeysReduced = true;
                    break;
                case NiPosKey::BEZKEY:
                    ReduceBezPosKeys(uiNumPosKeys, pkPosKeys, 
                        uiNumNewPosKeys, pkNewPosKeys, fTolerance, 
                        pkObject);
                    bKeysReduced = true;
                    break;
                case NiPosKey::TCBKEY:
                    // TCB keys are not reduced, since redundant 
                    // TCB keys only appear when placed there 
                    // by the artist.
                    break;
                default:
                    break;
                }
            }
            if (bKeysReduced)
            {
                // Replace the animation data through the interpolator so
                // it can update it internal variables
                pkTransformInterp->ReplacePosData(pkNewPosKeys, 
                    uiNumNewPosKeys, ePosKeyType);
            }

            // Reduce scale keys.
            bKeysReduced = false;
            unsigned int uiNumScaleKeys;
            NiFloatKey::KeyType eScaleKeyType;
            unsigned char ucScaleSize;
            NiFloatKey* pkScaleKeys = pkData->GetScaleAnim(uiNumScaleKeys,
                eScaleKeyType, ucScaleSize);
            unsigned int uiNumNewScaleKeys = 0;
            NiFloatKey* pkNewScaleKeys = NULL;

            if (uiNumScaleKeys > 1)
            {
                switch (eScaleKeyType)
                {
                    // NOTE - pkObject not passed in - removal of 2 keys
                    // assumes that pkObject has correct scale outside of
                    // animations
                case NiFloatKey::LINKEY:
                    ReduceLinFloatKeys(uiNumScaleKeys, pkScaleKeys,
                        uiNumNewScaleKeys, pkNewScaleKeys, fTolerance);
                    bKeysReduced = true;
                    break;
                case NiFloatKey::BEZKEY:
                    ReduceBezFloatKeys(uiNumScaleKeys, pkScaleKeys,
                        uiNumNewScaleKeys, pkNewScaleKeys, fTolerance);
                    bKeysReduced = true;
                    break;
                case NiFloatKey::TCBKEY:
                    // TCB keys are not reduced, since redundant TCB 
                    // keys only appear when placed there by the artist.
                    break;
                default:
                    break;
                }
            }
            if (bKeysReduced)
            {
                // Replace the animation data through the interpolator so
                // it can update it internal variables
                pkTransformInterp->ReplaceScaleData(pkNewScaleKeys, 
                    uiNumNewScaleKeys, eScaleKeyType);
            }
        }

        pkTransformInterp->Collapse();
        pkCtlr->ResetTimeExtrema();
    }
}
//---------------------------------------------------------------------------
void NiOptimize::SortByTexturingProp(NiNode* pkNode,
    NiTexturingProperty*& pkLoProp, NiTexturingProperty*& pkHiProp)
{
    // Recursively sorts all the children of each node by the address of the
    // NiTexturingProperty present in their property states. Children with no
    // NiTexturingProperty in their property state attached are sorted first.

    if (!NiIsExactKindOf(NiNode, pkNode) || pkNode->GetChildCount() == 0)
    {
        pkLoProp = NULL;
        pkHiProp = NULL;
        return;
    }

    NiTPointerList<TexSortObj*> kSortList;

    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            TexSortObj* pkNewSortObj = NiNew TexSortObj;
            pkNewSortObj->m_pkObject = pkChild;
            pkNewSortObj->m_pkLoProp = pkNewSortObj->m_pkHiProp = NULL;

            if (NiIsKindOf(NiNode, pkChild))
            {
                SortByTexturingProp((NiNode*) pkChild,
                    pkNewSortObj->m_pkLoProp, pkNewSortObj->m_pkHiProp);
            }
            else if (NiIsKindOf(NiRenderObject, pkChild))
            {
                NiPropertyState* pkPropState = ((NiRenderObject*) pkChild)
                    ->GetPropertyState();
                NiTexturingProperty* pkTexProp = NULL;
                if (pkPropState)
                {
                    pkTexProp = pkPropState->GetTexturing();
                }

                if (pkTexProp)
                {
                    // Check for an active map.
                    bool bMapFound = false;
                    for (unsigned int uiIndex = 0;
                        uiIndex < pkTexProp->GetMaps().GetSize(); uiIndex++)
                    {
                        if (pkTexProp->GetMaps().GetAt(uiIndex))
                        {
                            bMapFound = true;
                            break;
                        }
                    }

                    if (bMapFound)
                    {
                        pkNewSortObj->m_pkLoProp = pkNewSortObj->m_pkHiProp =
                            pkTexProp;
                    }
                }
            }

            NiTListIterator pos = kSortList.GetHeadPos();
            if (!pos)
            {
                kSortList.AddHead(pkNewSortObj);
            }
            else
            {
                bool bInsertAfter = false;

                while (pos)
                {
                    TexSortObj* pkCurSortObj = kSortList.Get(pos);
                    if (pkNewSortObj->m_pkLoProp < pkCurSortObj->m_pkLoProp)
                    {
                        NiTListIterator prevpos = kSortList.GetPrevPos(pos);
                        TexSortObj* pkPrevSortObj = NULL;
                        if (prevpos)
                        {
                            pkPrevSortObj = kSortList.Get(prevpos);
                        }

                        if (!pkPrevSortObj || pkPrevSortObj->m_pkHiProp !=
                            pkCurSortObj->m_pkHiProp)
                        {
                            break;
                        }
                    }
                    else if (pkCurSortObj->m_pkHiProp ==
                        pkNewSortObj->m_pkLoProp)
                    {
                        bInsertAfter = true;
                        break;
                    }
                    pos = kSortList.GetNextPos(pos);
                }

                if (!pos)
                {
                    kSortList.AddTail(pkNewSortObj);
                }
                else
                {
                    if (bInsertAfter)
                    {
                        kSortList.InsertAfter(pos, pkNewSortObj);
                    }
                    else
                    {
                        kSortList.InsertBefore(pos, pkNewSortObj);
                    }
                }
            }
        }
    }

    pkLoProp = kSortList.GetHead()->m_pkLoProp;
    pkHiProp = kSortList.GetTail()->m_pkHiProp;

    NiTObjectArray<NiAVObjectPtr> aspNewChildren(pkNode->GetChildCount());

    NiTListIterator pos = kSortList.GetHeadPos();
    while (pos)
    {
        TexSortObj* pkSortObj = kSortList.GetNext(pos);
        aspNewChildren.Add(pkSortObj->m_pkObject);
        NiDelete pkSortObj;
    }

    pkNode->RemoveAllChildren();
    for (ui = 0; ui < aspNewChildren.GetSize(); ui++)
    {
        pkNode->AttachChild(aspNewChildren.GetAt(ui));
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::CollectImproperlySizedTextures(NiAVObject* pkObject,
    NiTPointerMap<NiTexture*,bool>& kBadTex)
{
    // Detects if there are any images whose dimensions are not powers of two.
    // A pointer to each improperly sized texture is stored in kBadTex.

    // Returns: true, if any improperly sized textures are found.
    //          false, otherwise.

    NiTexturingProperty* pkProp = (NiTexturingProperty*)
        pkObject->GetProperty(NiProperty::TEXTURING);
    if (pkProp)
    {
        NiTexturingProperty::Map* pkMap;

        pkMap = pkProp->GetBaseMap();
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (pkTexture && !CheckTextureSize(pkTexture))
                kBadTex.SetAt(pkTexture, true);
        }
        pkMap = pkProp->GetDarkMap();
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (pkTexture && !CheckTextureSize(pkTexture))
                kBadTex.SetAt(pkTexture, true);
        }
        pkMap = pkProp->GetDetailMap();
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (pkTexture && !CheckTextureSize(pkTexture))
                kBadTex.SetAt(pkTexture, true);
        }
        pkMap = pkProp->GetGlossMap();
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (pkTexture && !CheckTextureSize(pkTexture))
                kBadTex.SetAt(pkTexture, true);
        }
        pkMap = pkProp->GetGlowMap();
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (pkTexture && !CheckTextureSize(pkTexture))
                kBadTex.SetAt(pkTexture, true);
        }
        for (unsigned int ui = 0; ui < pkProp->GetDecalArrayCount(); ui++)
        {
            pkMap = pkProp->GetDecalMap(ui);
            if (pkMap)
            {
                NiTexture* pkTexture = pkMap->GetTexture();
                if (pkTexture && !CheckTextureSize(pkTexture))
                    kBadTex.SetAt(pkTexture, true);
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                CollectImproperlySizedTextures(pkChild, kBadTex);
        }
    }

    return (kBadTex.GetCount() > 0);
}
//---------------------------------------------------------------------------
void NiOptimize::CleanUpAnimationKeys(NiAVObject* pkObject)
{
    // cleanup object controllers
    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        NiInterpController* pkInterpCtlr = NiDynamicCast(NiInterpController,
            pkCtlr);
        if (pkInterpCtlr && 
            pkInterpCtlr->GetBeginKeyTime() != pkInterpCtlr->GetEndKeyTime())
        {
            pkInterpCtlr->GuaranteeTimeRange(pkInterpCtlr->GetBeginKeyTime(),
                pkInterpCtlr->GetEndKeyTime());
        }
        pkCtlr = pkCtlr->GetNext();
    }

    // cleanup property controllers
    NiTListIterator kPos = pkObject->GetPropertyList().GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProperty = pkObject->GetPropertyList().GetNext(kPos);
        if (pkProperty)
        {
            pkCtlr = pkObject->GetControllers();
            while (pkCtlr)
            {
                NiInterpController* pkInterpCtlr = NiDynamicCast(
                    NiInterpController, pkCtlr);
                if (pkInterpCtlr && pkInterpCtlr->GetBeginKeyTime() !=
                    pkInterpCtlr->GetEndKeyTime())
                {
                    pkInterpCtlr->GuaranteeTimeRange(
                        pkInterpCtlr->GetBeginKeyTime(),
                        pkInterpCtlr->GetEndKeyTime());
                }
                pkCtlr = pkCtlr->GetNext();
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                CleanUpAnimationKeys(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::RemoveDegenerateTriangles(NiMesh* pkMesh)
{
    NIASSERT(pkMesh);
    NIASSERT(pkMesh->GetPrimitiveType() == 
        NiPrimitiveType::PRIMITIVE_TRIANGLES);
    NIASSERT(pkMesh->IsValid());

    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMesh));

    NiDataStreamRef* pkIndexStreamRef = 
        const_cast<NiDataStreamRef*>(
        pkMesh->GetFirstStreamRef(NiDataStream::USAGE_VERTEX_INDEX));

    // Don't remove the mesh just because it is non-indexed
    if (!pkIndexStreamRef)
        return false;

    NiDataStreamElementLock kVertexLock(pkMesh, NiCommonSemantics::POSITION(),
        0, NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_READ);

    NIASSERT(kVertexLock.IsLocked());
    NiTStridedRandomAccessIterator<NiPoint3> kVertexItor = 
        kVertexLock.begin<NiPoint3>();

    NIASSERT(pkIndexStreamRef->GetElementDescCount() == 1);    

    NiToolDataStream* pkIndexStream = NiVerifyStaticCast(NiToolDataStream, 
        pkIndexStreamRef->GetDataStream());

    NiUInt32* puiIndices = (NiUInt32*)pkIndexStream->LockRegion(0, 
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);

    NiUInt32* puiOld = puiIndices;
    NiUInt32* puiNew = puiIndices;

    NiUInt32 uiTriangleCount = pkMesh->GetTotalPrimitiveCount();
    NiUInt32 uiNewTriangleCount = 0;

    for (NiUInt32 ui = 0; ui < uiTriangleCount; ui++)
    {
        // if the triangle doesn't have repeated verts, keep it
        NiUInt32 uiTri = 3 * ui;
        NiUInt32 uiI0 = puiIndices[uiTri];
        NiUInt32 uiI1 = puiIndices[uiTri + 1];
        NiUInt32 uiI2 = puiIndices[uiTri + 2];

        const NiPoint3& kP0 = kVertexItor[uiI0];
        const NiPoint3& kP1 = kVertexItor[uiI1];
        const NiPoint3& kP2 = kVertexItor[uiI2];

        if (NiOptimize::NonDegenerateTriangle(uiI0, uiI1, uiI2, kP0, kP1, kP2))
        {
            // If it is not 32 bit both new and old will be NULL thus will 
            // always fail this test and continue.
            if (puiNew != puiOld)
            {
                puiNew[0] = puiOld[0];
                puiNew[1] = puiOld[1];
                puiNew[2] = puiOld[2];
            }

            uiNewTriangleCount++;
            puiNew += 3;
        }

        puiOld += 3;
    }

    // Replace the old indices with the new ones.
    if (uiNewTriangleCount != 0 && uiNewTriangleCount != uiTriangleCount)
    {        
        NiUInt32 uiNewIndexCount = 3 * uiNewTriangleCount;

        NiUInt32* puiNewTris = NiAlloc(NiUInt32, uiNewIndexCount);
        NIASSERT(puiNewTris);

        NiMemcpy(puiNewTris, puiIndices, 
            uiNewIndexCount * sizeof(NiUInt32));            
        pkIndexStream->Unlock(NiDataStream::LOCK_TOOL_READ | 
            NiDataStream::LOCK_TOOL_WRITE);

        // Create a new stream and replace the old stream.
        NiDataStream* pkNewStream = 
            NiDataStream::CreateSingleElementDataStream(
            NiDataStreamElement::F_UINT32_1,
            uiNewIndexCount, 
            pkIndexStream->GetAccessMask(), 
            NiDataStream::USAGE_VERTEX_INDEX, 
            puiNewTris,
            true);

        pkIndexStreamRef->SetDataStream(pkNewStream);
        NiFree(puiNewTris);
        pkMesh->ResetModifiers();
    }
    else
    {
        pkIndexStream->Unlock(NiDataStream::LOCK_TOOL_READ | 
            NiDataStream::LOCK_TOOL_WRITE);
    }

    NIASSERT(pkMesh->IsValid());
    return uiNewTriangleCount == 0;
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveDegenerateVertices(NiMesh* pkMesh,
    unsigned int* puiNewToOld)
{
    // this function will remove any vertices that are exact duplicates of any
    // others in the mesh. It determines if verts are duplicates by checking 
    // the value of the data for every element that is marked as vertex usage
    // It makes several assumptions
    //  * incoming meshes only have 1 region
    //  * All the meshes data streams have only 1 element at index 0
    //  * All data is in either 32-bit floats or 32-bit ints
    NIASSERT(pkMesh);
    NIASSERT(pkMesh->GetSubmeshCount() == 1);
    NIASSERT(pkMesh->IsValid());

    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMesh));

    // first get arrays/iterators for the index
    NiDataStreamElementLock kIndexLock(pkMesh, NiCommonSemantics::INDEX(),
        0, NiDataStreamElement::F_UNKNOWN, NiDataStream::LOCK_TOOL_READ);
    NIASSERT(kIndexLock.IsLocked());

    NiUInt32 uiVertCount = pkMesh->GetVertexCount();
    NiUInt32 uiListLength = kIndexLock.count();

    // First Quicksort the vertices.  The important feature is that indices of
    // vertices that are exactly equal occur in consecutive locations in
    // the sorted index array.  This allows for a linear pass over the
    // indices to identify and remove redundant vertices.

    NiUInt32* puiIndex0 = NiAlloc(unsigned int, uiVertCount);
    NiUInt32* puiIndex1 = NiAlloc(unsigned int, uiVertCount);

    // vertex[puiIndex0[i]] is the ith smallest and puiIndex1[i] tells 
    // where the ith vertex ended up in the sorted array. Therefore 
    // puiIndex0[puiIndex1[i]] == i

    // kLockSet and kDataSet are parallel arrays of element information
    NiTObjectSet<NiDataStreamPtr> kLockSet;
    NiTObjectSet<ElementData> kDataSet;
    if (!BuildElementDataSet(pkMesh, kLockSet, kDataSet))
        return;

    for (NiUInt32 ui = 0; ui < uiVertCount; ui++)
    {
        puiIndex0[ui] = ui;
    }

    IndexQSort(puiIndex0, 0, uiVertCount - 1, kDataSet);

    for (NiUInt32 ui = 0; ui < uiVertCount; ui++)
    {
        puiIndex1[puiIndex0[ui]] = ui;
    }

    // puiIndex0[i] is now the index of the i-th largest vert
    // puiIndex1[i] is now the size rank at each vert index

    // Compress index array by removing indices for redundant vertices.
    bool bVertsRemoved = false;
    for (NiUInt32 uiV = 1; uiV < uiVertCount; uiV++)
    {
        if (VertexCompare(puiIndex0[uiV], puiIndex0[uiV - 1], kDataSet) == 0)
        {
            puiIndex0[uiV] = puiIndex0[uiV - 1];
            bVertsRemoved = true;
        }
    }

    // puiIndex0[i] is now the index of the i-th largest vert w/ no duplicate
    // verts

    // Remap the connectivity list such that every element that was a 
    // duplicate vert index now uses the common index for matching verts
    // puiNewIndices should match the data in puiIndices everywhere except
    // where puiIndices referenced a degenerate
    NiUInt32* puiNewIndices = NiAlloc(NiUInt32, uiListLength);
    NiTStridedRandomAccessIterator<NiUInt32> puiIndices = 
        kIndexLock.begin<NiUInt32>();

    for (NiUInt32 ui = 0; ui < uiListLength; ui++)
    {
        puiNewIndices[ui] = puiIndex0[puiIndex1[puiIndices[ui]]];
    }

    NiFree(puiIndex0);
    NiFree(puiIndex1);

    // Map old vertex indices to NiNew ones. The number uiMaxUnsignedInt is
    // considered to be an invalid index.
    NiUInt32* puiOldToNew = NiAlloc(NiUInt32, uiVertCount);
    memset(puiOldToNew, UINT_MAX, uiVertCount * sizeof(puiOldToNew[0]));

    NiUInt32 uiNewVertices = 0;
    for (NiUInt32 ui = 0; ui < uiListLength; ui++)
    {
        unsigned int uiOldIndex = puiNewIndices[ui];
        if (puiOldToNew[uiOldIndex] == UINT_MAX)
        {
            puiOldToNew[uiOldIndex] = uiNewVertices++;
        }

        puiNewIndices[ui] = puiOldToNew[uiOldIndex];
    }

    // puiOldToNew now contains where each vertex will be placed by RebuildMesh
    // puiNewIndices now indexes into the new tightly packed vertex list

    // unlock all of the locked data streams
    for (NiUInt32 ui = 0; ui < kLockSet.GetSize(); ui++)
    {
        NiDataStreamPtr spDS = kLockSet.GetAt(ui);
        spDS->Unlock(NiDataStream::LOCK_TOOL_READ);
    }
    kLockSet.RemoveAll();
    kDataSet.RemoveAll();

    // if any verts were removed, re-allocate data arrays to remove duplicates
    if (bVertsRemoved)
        RebuildMesh(pkMesh, puiNewIndices, puiOldToNew, uiNewVertices);

    // If necessary, compute mapping of new vertex indices to old ones
    if (puiNewToOld)
    {
        for (NiUInt32 uiV = 0; uiV < uiVertCount; uiV++)
        {
            if (puiOldToNew[uiV] != UINT_MAX)
            {
                puiNewToOld[puiOldToNew[uiV]] = uiV;
            }
        }
    }

    NiFree(puiOldToNew);
    NiFree(puiNewIndices);
    NIASSERT(pkMesh->IsValid());
}
//---------------------------------------------------------------------------
bool NiOptimize::RemoveLowInfluenceBones(NiMesh* pkMesh, 
    float fMinInfluence)
{
    NIASSERT(pkMesh->IsValid());
    // This function will find any vertices that have bones influencing them
    // less than the given threshold. Once found on a vert, the bones are 
    // removed from that vert's bone list. Next, bones that are no longer 
    // referenced by any vert are removed and the index, weight, and bone
    // data streams are rebuilt. This function makes the following assumptions:
    //  * the skin data has not yet been partitioned
    //  * the bone blend weight data stream is at index 0
    //  * the weight data is 4 weights per vert in float32 format

    // First, perform a query to see if the mesh is skinned

    NiSkinningMeshModifier* pkSkin = NiGetModifier(NiSkinningMeshModifier, 
        pkMesh);
    if (!pkSkin)
        return false;

    NiUInt32 uiNumBones = pkSkin->GetBoneCount();
    if (uiNumBones == 0)
        return false;

    NiUInt32 uiVertCount = pkMesh->GetVertexCount();

    NiDataStreamRef* pkBlendStreamRef;
    NiDataStreamElement kBlendElement;
    // make sure the mesh has blend weights in the expected format
    if (!pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::BLENDWEIGHT(), 0, NiDataStreamElement::F_FLOAT32_4,
        pkBlendStreamRef, kBlendElement))
    {
        return false;
    }

    // we must iterate over all of the bone weights, lock for read & write
    NiDataStreamElementLock kWeightLock = NiDataStreamElementLock(pkMesh, 
        NiCommonSemantics::BLENDWEIGHT(), 0, NiDataStreamElement::F_FLOAT32_4, 
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    // if the lock is invalid, that semantic doesn't exist at that index, or 
    // it is in the wrong format
    if (!kWeightLock.IsLocked())
        return false;
    // NiTSimpleArray helps us iterate over each component cleanly
    NiTStridedRandomAccessIterator<NiTSimpleArray<float, 4> > kWeights = 
        kWeightLock.begin<NiTSimpleArray<float, 4> >();

    // we must also have access to the index data, lock for read & write
    NiDataStreamElementLock kBoneLock = NiDataStreamElementLock(pkMesh,
        NiCommonSemantics::BLENDINDICES(), 0, NiDataStreamElement::F_INT16_4,
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    // if the lock is invalid, that semantic doesn't exist at that index, or 
    // it is in the wrong format
    if (!kBoneLock.IsLocked())
        return false;
    // NiTSimpleArray helps us iterate over each component cleanly
    NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16, 4> > kBones = 
        kBoneLock.begin<NiTSimpleArray<NiInt16, 4> >();

    // We will keep track of the reference count for each bone so that we
    // can tell which ones are safe to remove
    NiUInt32* auiVertCount = NiAlloc(NiUInt32, uiNumBones);
    memset(auiVertCount, 0, uiNumBones * sizeof(NiUInt32));

    // We expect this to be 4
    NiUInt32 uiBonesPerVert = kBlendElement.GetComponentCount();
    for (NiUInt32 uiVert = 0; uiVert < uiVertCount; uiVert++)
    {
        // for each vert, check it's weights
        float fWeightSum = 0.0f;
        for (NiUInt32 uiWeight = 0; uiWeight < uiBonesPerVert; uiWeight++)
        {
            // for each weight, check if it is below threshold
            float fWeight = kWeights[uiVert][uiWeight];
            if (fWeight < fMinInfluence)
            {
                // we have found a bone influence that should be removed
                // to remove a bone, set index and weight to zero
                kWeights[uiVert][uiWeight] = 0.0f;
                kBones[uiVert][uiWeight] = 0;
            }
            else
            {
                // sum influential bones for use with normalization
                fWeightSum += fWeight;
                // record that this bone influences this vert
                auiVertCount[kBones[uiVert][uiWeight]]++;
            }
        }

        // after we have removed bones, we should re-normalize
        // we normalize whether bones are removed or not just in case incoming
        // data wasn't already normalized
        // if all bones were removed, just move on
        if (fWeightSum == 0.0f)
            continue;
        for (NiUInt32 uiWeight = 0; uiWeight < uiBonesPerVert; uiWeight++)
        {
            kWeights[uiVert][uiWeight] /= fWeightSum;
        }
    }

    // now we need to re-map the bone indices to be a compact list
    // iterate over each bone and if used, add to the compact list and mapping
    NiUInt32 uiNewIndex = 0;
    NiAVObject** apkOldBoneList = pkSkin->GetBones();
    NiTransform* akOldTransformList = pkSkin->GetSkinToBoneTransforms();
    NiBound* akOldBoundList = pkSkin->GetBoneBounds();

    NiAVObject** apkTempBoneList = NiAlloc(NiAVObject*, uiNumBones);
    NiTransform* akTempTransformList = NiAlloc(NiTransform, uiNumBones);
    NiBound* akTempBoundList = NiAlloc(NiBound, uiNumBones);

    NiUInt32* auiOldToNew = NiAlloc(NiUInt32, uiNumBones);
    for (NiUInt32 uiOldIndex = 0; uiOldIndex < uiNumBones; uiOldIndex++)
    {
        if (auiVertCount[uiOldIndex] > 0)
        {
            apkTempBoneList[uiNewIndex] = apkOldBoneList[uiOldIndex];
            akTempTransformList[uiNewIndex] = akOldTransformList[uiOldIndex];

            if (akOldBoundList)
                akTempBoundList[uiNewIndex] = akOldBoundList[uiOldIndex];

            auiOldToNew[uiOldIndex] = uiNewIndex;
            uiNewIndex++;
        }
        else
        {
            auiOldToNew[uiOldIndex] = UINT_MAX;
        }
    }

    // uiNewIndex is now equivalent to the new bone count
    const NiUInt32 uiNewBoneCount = uiNewIndex;

    // no need to continue if no bones were removed
    if (uiNewBoneCount != uiNumBones)
    {
        // Copy the reordered bone lists to the mesh modifier after resizing it
        pkSkin->Resize(uiNewBoneCount);

        NiMemcpy(pkSkin->GetBones(), apkTempBoneList,
            sizeof(NiAVObject*) * uiNewBoneCount);

        NiMemcpy(pkSkin->GetSkinToBoneTransforms(), akTempTransformList,
            sizeof(NiTransform) * uiNewBoneCount);

        if (akOldBoundList)
        {
            NiMemcpy(pkSkin->GetBoneBounds(), akTempBoundList,
                sizeof(NiBound) * uiNewBoneCount);
        }

        // change the vert bone indices to point to the new indices
        for (NiUInt32 uiVert = 0; uiVert < uiVertCount; uiVert++)
        {
            for (NiUInt32 uiBone = 0; uiBone < uiBonesPerVert; uiBone++)
            {
                if (auiOldToNew[kBones[uiVert][uiBone]] != UINT_MAX)
                {
                    kBones[uiVert][uiBone] = 
                        (NiInt16)auiOldToNew[kBones[uiVert][uiBone]];
                }
            }
        }

        // create a new bone remapping array
        NiDataStreamRef* pkOldBoneRemapRef = 
            pkMesh->FindStreamRef(NiCommonSemantics::BONE_PALETTE());
        if (pkOldBoneRemapRef)
        {
            NiDataStreamPtr spNewBoneRemap = 
                NiDataStream::CreateSingleElementDataStream(
                NiDataStreamElement::F_UINT16_1, uiNewBoneCount, 
                pkOldBoneRemapRef->GetDataStream()->GetAccessMask(),
                NiDataStream::USAGE_USER, NULL, true, true);

            NiDataStreamLock kNewRemapLock(
                spNewBoneRemap, 0, NiDataStream::LOCK_TOOL_WRITE);

            NiTStridedRandomAccessIterator<NiUInt16> pusNewRemap = 
                kNewRemapLock.begin<NiUInt16>();

            // change the bone index remapping array
            for (NiUInt32 ui = 0; ui < uiNewBoneCount; ui++)
            {
                pusNewRemap[ui] = (NiUInt16)ui;
            }
            kNewRemapLock.Unlock();

            pkOldBoneRemapRef->SetDataStream(spNewBoneRemap);

            pkMesh->ResetModifiers();
        }
    }

    NiFree(akTempBoundList);
    NiFree(akTempTransformList);
    NiFree(apkTempBoneList);
    NiFree(auiVertCount);
    NiFree(auiOldToNew);

    NIASSERT(pkMesh->IsValid());


    return true;
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveAppCulledObjects(NiAVObject* pkObject,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RemoveAppCulledObjects(pkChild, kMap);
            }
        }
    }

    bool bWillBeAppCulled = false;
    for (NiAVObject* pkCurrent = pkObject; pkCurrent != NULL; 
        pkCurrent = pkCurrent->GetParent())
    {
        if (pkCurrent->GetAppCulled())
            bWillBeAppCulled = true;
    }

    NiNode* pkParent = pkObject->GetParent();
    if (bWillBeAppCulled && pkParent)
    {
        bool bTemp;
        if (!kMap.GetAt(pkObject, bTemp) &&
            !NiIsKindOf(NiSwitchNode, pkParent) &&
            !CheckForExtraDataTags(pkObject) &&
            (!pkNode || pkNode->GetChildCount() == 0))
        {
            // Remove this object.
            pkParent->DetachChild(pkObject);
        }
    }
}
//---------------------------------------------------------------------------
// Utility typedefs for skinning LOD...
typedef NiTPointerMap<NiNode*, NiNode*> NodeMap;
typedef NiTPrimitiveArray<NodeMap*> NodeMapArray;
//---------------------------------------------------------------------------
static bool RemapIndices(NiMesh* pkMesh, const NodeMap* pkNodeMap)
{
    NIASSERT(pkMesh);
    NiSkinningMeshModifier* pkSkinMod = NiGetModifier(
        NiSkinningMeshModifier, pkMesh);
    NIASSERT(pkSkinMod);

    NiDataStreamRef* pkOldIndexStreamRef = pkMesh->FindStreamRef(
        NiCommonSemantics::BLENDINDICES(), 0,
        NiDataStreamElement::F_INT16_4);
    if (!pkOldIndexStreamRef)
    {
        NILOG("WARNING: Could not find blend indices on mesh for "
            "skinning LOD.\n");
        return false;
    }
    NiDataStream* pkOldIndexStream = pkOldIndexStreamRef->GetDataStream();
    if (!pkOldIndexStream || pkOldIndexStream->GetElementDescCount() != 1)
    {
        NILOG("WARNING: Can't remap indices for skinning LOD on streams "
            "with multiple elements.\n");
        return false;
    }

    // This function doesn't handle bone palettes.  It assumes that there
    // is a single blend indices stream (with a single element).
    if (pkMesh->FindStreamRef(NiCommonSemantics::BONE_PALETTE()))
    {
        NILOG("WARNING: Can't remap indices for skinning LOD on meshes "
            "with bone palettes.\n");
        return false;
    }

    NiDataStreamRef* pkOldBlendWeightRef = pkMesh->FindStreamRef(
        NiCommonSemantics::BLENDWEIGHT(), 0,
        NiDataStreamElement::F_FLOAT32_4);
    if (!pkOldBlendWeightRef)
    {
        NILOG("WARNING: Could not find blend weights on mesh for "
            "skinning LOD.\n");
        return false;
    }

    // Mapping of new index -> old index
    NiTPrimitiveSet<unsigned int> kNewToOldIndices;
    // Mapping of old index -> new index
    NiTPrimitiveArray<unsigned int> kIndices;

    // Determine the (potentially smaller) set of bones to use for this
    // mesh
    {
        kIndices.SetSize(pkSkinMod->GetBoneCount());

        for (unsigned int i = 0; i < pkSkinMod->GetBoneCount(); i++)
        {
            NiAVObject* pkOrigAV = pkSkinMod->GetBones()[i];
            NiNode* pkOrig = NiDynamicCast(NiNode, pkOrigAV);
            NIASSERT(pkOrig);
            if (!pkOrig)
                return false;

            // All bones that are used in the modifier should be mapped.
            NiNode* pkMapped;
            if (!pkNodeMap->GetAt(pkOrig, pkMapped))
            {
                // If a bone is not in the map, it means that it did not
                // meet the weight tolerance or was not used in the index
                // stream.  Just map this bone to zero.  It will get cleaned
                // up in the skin optimization step.
                pkMapped = (NiNode*)pkSkinMod->GetBones()[0];
            }

            // find pkMapped in pkOrigAV
            bool bFound = false;
            unsigned int uiIndex = 0;
            NiNode* pkFinal = pkMapped;
            while (pkFinal)
            {
                for (unsigned int j = 0; j < pkSkinMod->GetBoneCount(); j++)
                {
                    if (pkSkinMod->GetBones()[j] == pkFinal)
                    {
                        uiIndex = j;
                        bFound = true;
                        break;
                    }
                }

                if (bFound)
                    break;

                // If the mesh has a bone affecting it, but not its parent,
                // then it may be the case that under certain LOD levels that
                // its parent will not be found.  Because we do not have its
                // skin-to-bone transform at this point, we cannot use this
                // unknown bone.  We will walk up the tree until we find its
                // parent.  If we don't find it, then we'll just use the
                // original bone.
                pkFinal = pkFinal->GetParent();
            }

            // If none of the parents are mapped, then use the original bone.
            if (!pkFinal)
            {
                pkFinal = pkOrig;
                uiIndex = i;
            }

            kIndices.SetAt(i, kNewToOldIndices.AddUnique(uiIndex));

            // If we ended up using another bone, issue a warning.
            if (pkFinal != pkMapped)
            {
                char acBuffer[1024];
                NiSprintf(acBuffer, sizeof(acBuffer), "WARNING: the bone (%s) "
                    "wasn't used in the original mesh (%s), so couldn't be "
                    "used for bone LOD.  Using bone (%s) instead.\n",
                    (const char*)pkMapped->GetName(),
                    (const char*)pkMesh->GetName(),
                    (const char*)pkFinal->GetName());
                NILOG(acBuffer);
            }
        }
    }

    // remap index and blend weight streams
    {
        // Rather than just reading the data and writing back into the same
        // stream, create a new blend index and weight stream in case other
        // stream refs are sharing this data stream.

        // Create a blend index stream
        NiDataStreamRef kNewIndexStreamRef = 
            pkOldIndexStreamRef->CreateFullCopy();
        NiDataStream* pkNewIndexStream = kNewIndexStreamRef.GetDataStream();
        NiInt16* psDest = (NiInt16*)pkNewIndexStream->Lock(
            NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);
        NiInt16* psSource = (NiInt16*)pkOldIndexStream->Lock(
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

        // Create a blend weight stream 
        NiDataStream* pkOldBlendWeightStream = 
            pkOldBlendWeightRef->GetDataStream();   
        NiDataStreamRef kNewBlendWeightRef = 
            pkOldBlendWeightRef->CreateFullCopy();
        NiDataStream* pkNewBlendWeightStream =
            kNewBlendWeightRef.GetDataStream();
        float* psBWDest = (float*)pkNewBlendWeightStream->Lock(
            NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);
        float* psBWSource = (float*)pkOldBlendWeightStream->Lock(
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

        // Remap indices and weights
        unsigned int uiNumIndices = pkOldIndexStream->GetTotalCount() * 4;
        for (unsigned int i = 0; i < uiNumIndices; i++)
        {
            short sSource = psSource[i];
            bool bValid = (sSource >=0 && 
                (unsigned int)sSource < pkSkinMod->GetBoneCount());
            NIASSERT(bValid);
            psDest[i] = (NiInt16)(bValid ? kIndices.GetAt(sSource) : 0);

            psBWDest[i] = psBWSource[i];

            // Within this group of four indices/weights, check the current
            // index against all other indices in the set for duplicates.
            for (unsigned int j = i % 4; j >= 1; j--)
            {
                if (psDest[i] == psDest[i-j])
                {
                    // If we find a duplicate bone, merge the weights.
                    psBWDest[i-j] += psBWDest[i];
                    psBWDest[i] = 0;
                    break;
                }
            }
        }

        pkOldIndexStream->Unlock(
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        pkNewIndexStream->Unlock(
            NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);
        pkMesh->RemoveStreamRef(pkOldIndexStreamRef);
        pkMesh->AddStreamRef(&kNewIndexStreamRef);

        pkOldBlendWeightStream->Unlock(
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        pkNewBlendWeightStream->Unlock(
            NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);
        pkMesh->RemoveStreamRef(pkOldBlendWeightRef);
        pkMesh->AddStreamRef(&kNewBlendWeightRef);
    }

    // Remap skinning mesh modifier
    NIVERIFY(NiMeshTools::RemapSkinningMeshModifier(pkSkinMod,
        kNewToOldIndices.GetSize(), kNewToOldIndices.GetBase()));

    return true;
}
//---------------------------------------------------------------------------
static const char* ParseBoneLODSkinExtraData(NiExtraData* pkExtra,
    unsigned int& uiMinLOD, unsigned int& uiMaxLOD)
{
    // String extra data is of the form "NiBoneLOD#Skin#n#m#" where n and
    // m are the min and max LODs for this skin, respectively.

    if (!pkExtra)
        return NULL;

    NiStringExtraData* pkStringExtra = NiDynamicCast(NiStringExtraData,
        pkExtra);
    if (!pkStringExtra)
        return NULL;

    const char* pcString = pkStringExtra->GetValue();
    if (!pcString)
        return NULL;

    if (!strstr(pcString, "NiBoneLOD#Skin#"))
        return NULL;

    size_t stLen = strlen(pcString) + 1;
    char* pcPtr = NiStackAlloc(char, stLen);
    NiStrcpy(pcPtr, stLen, pcString);
    char* pcContext;

    char* pcLOD = NiStrtok(pcPtr, "#", &pcContext);
    if (!pcLOD || (pcLOD && strcmp(pcLOD, "NiBoneLOD") != 0))
    {
        NiStackFree(pcPtr);
        return NULL;
    }

    pcLOD = NiStrtok(NULL, "#", &pcContext);
    if (!pcLOD || (pcLOD && strcmp(pcLOD, "Skin") != 0))
    {
        NiStackFree(pcPtr);
        return NULL;
    }

    pcLOD = NiStrtok(NULL, "#", &pcContext);
    if (!pcLOD)
    {
        NiStackFree(pcPtr);
        return NULL;
    }
    uiMinLOD = (unsigned int) atoi(pcLOD);

    pcLOD = NiStrtok(NULL, "#", &pcContext);
    if (!pcLOD)
    {
        NiStackFree(pcPtr);
        return NULL;
    }

    uiMaxLOD = (unsigned int) atoi(pcLOD);

    NiStackFree(pcPtr);
    return pcString;
}
//---------------------------------------------------------------------------
static void AddBoneLODSkin(NiMesh* pkMesh, NiSkinningLODController* pkCont,
    NodeMapArray& kMapArray, NiNode* pkBoneRoot)
{
    if (!pkMesh)
        return;

    NiSkinningMeshModifier* pkSkinMod = NiGetModifier(
        NiSkinningMeshModifier, pkMesh);

    if (!pkSkinMod)
        return;

    unsigned int uiMinLOD = 0;
    unsigned int uiMaxLOD = 0;

    const char* pcExtraData = NULL;

    // parse extra data off of parent nodes rescursively to determine if this
    // mesh has been set up with skin LOD levels
    {
        NiNode* pkTest = pkMesh->GetParent();

        while (!pcExtraData && pkTest && pkTest != pkBoneRoot)
        {
            for (unsigned int i = 0; i < pkTest->GetExtraDataSize(); i++)
            {
                pcExtraData = ParseBoneLODSkinExtraData(
                    pkTest->GetExtraDataAt((unsigned short)i), 
                    uiMinLOD, uiMaxLOD);
                if (pcExtraData)
                    break;
            }

            pkTest = pkTest->GetParent();
        }

        if (!pcExtraData)
            return;
    }

    // Determine if all of the bones of this mesh are under the bone root
    // of pkCont.
    {
        bool bHasBonesUnder = false;
        bool bHasBonesNotUnder = false;

        NiObjectNET* pkTarget = pkCont->GetTarget();
        NiNode* pkBoneRoot = 
            NiDynamicCast(NiNode, pkTarget);
        NI_UNUSED_ARG(pkBoneRoot);

        NIASSERT(pkBoneRoot);

        const NiDataStreamRef* pkIndexRef = pkMesh->FindStreamRef(
            NiCommonSemantics::BLENDINDICES(), 0,
            NiDataStreamElement::F_INT16_4);
        NIASSERT(pkIndexRef);
        if (!pkIndexRef)
            return;

        const NiDataStreamRef* pkWeightRef = pkMesh->FindStreamRef(
            NiCommonSemantics::BLENDWEIGHT(), 0,
            NiDataStreamElement::F_FLOAT32_4);
        NIASSERT(pkWeightRef);
        if (!pkWeightRef)
            return;

        short* psIndices = (short*)
            ((NiDataStream*)pkIndexRef->GetDataStream())->Lock(
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        float* pfWeights = 
            (float*)((NiDataStream*)pkWeightRef->GetDataStream())->Lock(
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

        unsigned int uiNumVerts = pkIndexRef->GetCount(0);

        unsigned int uiBonesFound = 0;

        for (unsigned int i = 0; i < pkSkinMod->GetBoneCount(); i++)
        {
            const float fTolerance = 0.001f;

            // Is this bone used and does it have a non-zero weight?
            bool bUsed = false;
            for (unsigned int uiV = 0; uiV < uiNumVerts * 4; uiV++)
            {
                if (psIndices[uiV] == (int)i)
                {
                    if (pfWeights[uiV] > fTolerance)
                    {
                        uiBonesFound++;
                        bUsed = true;
                        break;
                    }
                }
            }

            if (!bUsed)
            {
                continue;
            }

            NiAVObject* pkBoneAV = pkSkinMod->GetBones()[i];
            NiNode* pkBone = NiDynamicCast(NiNode, pkBoneAV);
            NIASSERT(pkBone);
            NiNode* pkDummy;
            bool bUnder = kMapArray.GetAt(0)->GetAt(pkBone, pkDummy);

            bHasBonesUnder |= bUnder;
            bHasBonesNotUnder |= !bUnder;

        }

        ((NiDataStream*)pkWeightRef->GetDataStream())->Unlock(
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        ((NiDataStream*)pkIndexRef->GetDataStream())->Unlock(
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

        if (!bHasBonesUnder)
            return;

        if (bHasBonesNotUnder)
        {
            NILOG("WARNING: Mesh has bones that are not under the skinning "
                "LOD bone root.  Skipping mesh.\n");
            return;
        }
    }

    // At this point, we know all the bones are under the bone root and the
    // mesh is valid.

    // Add extra levels of LOD to the skin controller if this mesh specifies
    // LOD levels that haven't been seen before.
    {
        unsigned int uiOldNumLODs = pkCont->GetNumberOfBoneLODs();
        unsigned int uiNumBones = pkCont->GetNumBonesUsed(uiOldNumLODs - 1);
        for (unsigned int i = uiOldNumLODs; i <= uiMaxLOD; i++)
        {
            pkCont->AddBoneLOD(i, uiNumBones);
        }
    }

    // Generate copies of meshes for the different LOD levels
    unsigned int uiLODLevels = uiMaxLOD - uiMinLOD + 1;
    NiMesh** apkLODs = NiStackAlloc(NiMesh*, uiLODLevels);
    {
        apkLODs[0] = pkMesh;
        char acBuffer[NI_MAX_PATH];
        for (unsigned int i = 1; i < uiLODLevels; i++)
        {
            // Due to the fact that breaking up submeshes for skinning
            // creates different meshes, we will create deep copies of all
            // of the data streams.  The way submeshes are created implies
            // that different levels of LOD cannot share vertex data, 
            // even though they only differ by blend index prior to 
            // breaking it up into submeshes.

            NiMesh* pkLODMesh = (NiMesh*)pkMesh->Clone();
            pkMesh->GetParent()->AttachChild(pkLODMesh);

            for (unsigned int j = 0; j < pkLODMesh->GetStreamRefCount(); j++)
            {
                NiDataStreamRef* pkRef = pkLODMesh->GetStreamRefAt(j);
                NiDataStream* pkStream = pkRef->GetDataStream();
                NiDataStreamPtr spNewStream = 
                    (NiDataStream*)((NiObject*)pkStream->CreateDeepCopy());
                pkRef->SetDataStream(spNewStream);
            }

            apkLODs[i] = pkLODMesh;

            // Name other LOD levels appropriately.  Don't rename the first
            // mesh (LOD 0) in case an application is using it by name.
            NiSprintf(acBuffer, sizeof(acBuffer), 
                "%s:LOD %d", pkMesh->GetName(), i);
            pkLODMesh->SetName(acBuffer);
        }
    }

    // Remap all of the indices for each LOD level.
    NiFixedString kExtraData(pcExtraData);
    for (unsigned int i = uiMinLOD; i <= uiMaxLOD; i++)
    {
        NiMesh* pkLODMesh = apkLODs[i - uiMinLOD];

        // The bone map array holds mappings for all the levels of LOD found
        // from the bones.  It's possible that meshes are specified outside
        // of that range (i.e. a mesh that is for LOD level 3, but all bones
        // are for LOD level 2 and lower.)
        unsigned int uiBoneLOD = kMapArray.GetSize() - 1;
        uiBoneLOD = uiBoneLOD < i ? uiBoneLOD : i;

        if (RemapIndices(pkLODMesh, kMapArray.GetAt(uiBoneLOD)))
        {
            pkCont->AddSkin(i, pkLODMesh);

            // Propagate the extra data to the meshes so that they get
            // skipped by further optimization.
            pkLODMesh->AddExtraData(NiNew NiStringExtraData(kExtraData));

        }
    }

    NiStackFree(apkLODs);
}
//---------------------------------------------------------------------------
static void FindBoneLODSkins(NiNode* pkNode, NodeMapArray& kMapArray, 
    NiTPrimitiveArray<NiMesh*>& kMeshes)
{
    for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
    {
        NiAVObject* pkChild = pkNode->GetAt(i);
        if (!pkChild)
            continue;

        if (NiIsKindOf(NiMesh, pkChild))
        {
            NiMesh* pkMesh = (NiMesh*)pkChild;

            // Verify that all meshes in the bone hierarchy are on 
            // bones that are on at the lowest level of detail.  If not,
            // then those meshes may not get updated, if UpdateSelected is
            // used to update the scene.
            const unsigned int uiMaxBoneLOD = kMapArray.GetSize() - 1;
            NiNode* pkMap;
            if (kMapArray.GetAt(uiMaxBoneLOD)->GetAt(pkNode, pkMap) &&
                pkMap != pkNode)
            {
                NILOG("WARNING: Skinning LOD bone hierarchy contains other "
                    "meshes on bones that are not at the lowest level of "
                    "detail.\n");

                NiNode* pkParent = pkNode;
                while (pkParent)
                {
                    // If the parent is not mapped, then we've walked up
                    // the tree past the bone root node.
                    if (!kMapArray.GetAt(0)->GetAt(pkParent, pkMap))
                        break;

                    // Walk through all levels of detail and map parent
                    // onto itself.
                    for (unsigned int uiL = 0; uiL < kMapArray.GetSize(); 
                        uiL++)
                    {
                        kMapArray.GetAt(uiL)->SetAt(pkParent, pkParent);
                    }
                    pkParent = pkParent->GetParent();
                }
            }

            kMeshes.Add(pkMesh);
        }
        else if (NiIsKindOf(NiNode, pkChild))
        {
            FindBoneLODSkins((NiNode*)pkChild, kMapArray, kMeshes);
        }
    }
}
//---------------------------------------------------------------------------
static bool ParseBoneLODBoneExtraData(NiExtraData* pkExtra,
    unsigned int& uiLOD)
{
    if (!pkExtra)
        return false;

    NiStringExtraData* pkStringExtra = NiDynamicCast(NiStringExtraData,
        pkExtra);

    if (!pkStringExtra)
        return false;

    const char* pcString = pkStringExtra->GetValue();
    if (!pcString)
        return false;

    if (!strstr(pcString, "NiBoneLOD#Bone#"))
        return false;

    size_t stLen = strlen(pcString) + 1;
    char* pcPtr = NiStackAlloc(char, stLen);
    NiStrcpy(pcPtr, stLen, pcString);

    // Get bone LOD from string extra data.
    char* pcContext;
    char* pcLOD = NiStrtok(pcPtr, "#", &pcContext);
    if (!pcLOD || strcmp(pcLOD, "NiBoneLOD") != 0)
    {
        NiStackFree(pcPtr);
        return false;
    }

    pcLOD = NiStrtok(NULL, "#", &pcContext);
    if (!pcLOD || strcmp(pcLOD, "Bone") != 0)
    {
        NiStackFree(pcPtr);
        return false;
    }

    pcLOD = NiStrtok(NULL, "#", &pcContext);
    if (!pcLOD)
    {
        NiStackFree(pcPtr);
        return false;
    }

    uiLOD = (unsigned int)atoi(pcLOD);

    NiStackFree(pcPtr);
    return true;
}
//---------------------------------------------------------------------------
static void FindBoneLODBones(NiNode* pkBone,
    NiTPointerMap<NiNode*, unsigned int>& kBoneToLOD)
{
    if (!pkBone)
        return;

    for (unsigned int i = 0; i < pkBone->GetExtraDataSize(); i++)
    {
        unsigned int uiBoneLOD;
        if (ParseBoneLODBoneExtraData(
            pkBone->GetExtraDataAt((unsigned short)i), uiBoneLOD))
        {
            kBoneToLOD.SetAt(pkBone, uiBoneLOD);
            break;
        }
    }

    for (unsigned int j = 0; j < pkBone->GetArrayCount(); j++)
    {
        NiNode* pkChild = NiDynamicCast(NiNode, pkBone->GetAt(j));
        if (pkChild)
            FindBoneLODBones(pkChild, kBoneToLOD);
    }
}
//---------------------------------------------------------------------------
static void FillMapArray(NodeMapArray& kMapArray, NiNode* pkNode, 
    NiTPointerMap<NiNode*, unsigned int>& kBoneToLOD,
    unsigned int uiParentLOD)
{
    for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
    {
        NiNode* pkChild = NiDynamicCast(NiNode, pkNode->GetAt(i));
        if (!pkChild)
            continue;

        unsigned int uiChildLOD;
        if (!kBoneToLOD.GetAt(pkChild, uiChildLOD))
        {
            uiChildLOD = uiParentLOD;
            kBoneToLOD.SetAt(pkChild, uiChildLOD);
        }
        else
        {
            // This should have been previously checked, but we will
            // assert just to be sure.
            NIASSERT(uiChildLOD <= uiParentLOD);
        }

        unsigned int uiNumLODs = kMapArray.GetSize();
        NIASSERT(uiChildLOD < uiNumLODs);
        unsigned int uiL;

        // At higher levels of detail, this bone maps to itself.
        for (uiL = 0; uiL <= uiChildLOD; uiL++)
        {
            kMapArray.GetAt(uiL)->SetAt(pkChild, pkChild);
        }

        // At lower levels of detail, find the bone that this should map to
        for (uiL = uiChildLOD + 1; uiL < uiNumLODs; uiL++)
        {
            NiNode* pkParent = pkChild->GetParent();
            bool bFound = false;
            while (pkParent)
            {
                unsigned int uiLOD;
                NIVERIFY(kBoneToLOD.GetAt(pkParent, uiLOD));
                if (uiLOD >= uiL)
                {
                    kMapArray.GetAt(uiL)->SetAt(pkChild, pkParent);
                    bFound = true;
                    break;
                }

                pkParent = pkParent->GetParent();
            }
            NIASSERT(bFound);
        }

        FillMapArray(kMapArray, pkChild, kBoneToLOD, uiChildLOD);
    }
}
//---------------------------------------------------------------------------
static void ProcessBoneRootNode(NiNode* pkScene, NiNode* pkBoneRoot)
{
    // Make sure our inputs are valid.
    if (!pkScene || !pkBoneRoot)
        return;

    // Find bones
    NiTPointerMap<NiNode*, unsigned int> kBoneToLOD;
    FindBoneLODBones(pkBoneRoot, kBoneToLOD);

    // Find max LOD
    NiNode* pkBone = NULL;
    unsigned int uiLOD = 0;
    unsigned int uiMaxLOD = 0;

    NiTMapIterator kIter;

    for (kIter = kBoneToLOD.GetFirstPos(); kIter != NULL;
        kBoneToLOD.GetNext(kIter, pkBone, uiLOD))
    {
        if (uiLOD > uiMaxLOD)
            uiMaxLOD = uiLOD;
    }

    for (kIter = kBoneToLOD.GetFirstPos(); kIter != NULL;
        kBoneToLOD.GetNext(kIter, pkBone, uiLOD))
    {
        if (!pkBone)
            continue;

        // Bones that are at the lowest level of detail never need to fall
        // back on a parent.
        if (uiLOD == uiMaxLOD)
            continue;

        NiNode* pkParent = pkBone->GetParent();

        // verify that only higher LOD bones are above each bone
        bool bFound = false;
        while (pkParent)
        {
            unsigned int uiParentLOD;
            if (kBoneToLOD.GetAt(pkParent, uiParentLOD))
            {
                if (uiParentLOD < uiLOD)
                {
                    NILOG("WARNING: Skinning LOD bone hierarchy specifies "
                        "parent bones at a lower level of detail than its "
                        "children.\n");
                    return;
                }
                else if (uiParentLOD > uiLOD)
                {
                    bFound = true;
                    break;
                }
            }
            pkParent = pkParent->GetParent();
        }

        // No lower level of detail found for this bone.
        // Promote this bone to the lowest level of detail, because
        // it has no other bone to fall back on.
        if (!bFound)
        {
            kBoneToLOD.SetAt(pkBone, uiMaxLOD);
        }
    }

    unsigned int uiNumBones = kBoneToLOD.GetCount();
    if (!uiNumBones)
        return;

    NiNode** apkBones = NiStackAlloc(NiNode*, uiNumBones);

    NiSkinningLODController* pkCont = NiNew NiSkinningLODController();
    pkCont->SetTarget(pkBoneRoot);
    pkCont->SetMaxBoneLODs(uiMaxLOD);

    // Sort bones by LOD
    {
        unsigned int uiBones = 0;
        for (unsigned int uiL = uiMaxLOD; uiL < uiMaxLOD + 1; uiL--)
        {
            for (NiTMapIterator kIter = kBoneToLOD.GetFirstPos(); 
                kIter != NULL; kBoneToLOD.GetNext(kIter, pkBone, uiLOD))
            {
                if (uiLOD == uiL)
                {
                    apkBones[uiBones++] = pkBone;
                }
            }

            // Record number of bones at each level
            pkCont->AddBoneLOD(uiL, uiBones);
        }

        // At this point, the bones that are stored are only the ones that
        // have been specified in the art package as having a particular
        // bone LOD.  This is the set of bones that will have selective
        // update turned on and off on them.  It is a separate set of bones
        // from those which are truly affecting skinning.
        //
        // We will mark bones not in this array (but under the bone root) 
        // with their implied bone LOD (based on their position in the bone 
        // hierarchy) and enforce the condition that the "used" set of bones 
        // is a subset of all marked bones.
        pkCont->SetBones(uiNumBones, apkBones);
        NIASSERT(uiBones == uiNumBones);
    }

    // For each lod level, generate a mapping of bones to
    // replacement bones at a lower LOD.
    NodeMapArray kMapArray;
    {
        kMapArray.SetSize(uiMaxLOD+1);
        for (unsigned int uiL = 0; uiL <= uiMaxLOD; uiL++)
        {
            kMapArray.Add(NiNew NodeMap());
            kMapArray.GetAt(uiL)->SetAt(pkBoneRoot, pkBoneRoot);
        }
        FillMapArray(kMapArray, pkBoneRoot, kBoneToLOD, uiMaxLOD);
    }

    // Find all meshes and replace indices with the correct values.
    {
        // Find skins first so that newly created meshes are not found.
        NiTPrimitiveArray<NiMesh*> kMeshes;
        FindBoneLODSkins(pkScene, kMapArray, kMeshes);

        for (unsigned int i = 0; i < kMeshes.GetSize(); i++)
        {
            AddBoneLODSkin(kMeshes.GetAt(i), pkCont, kMapArray, pkBoneRoot);
        }
    }

    // clean up
    {
        for (unsigned int uiL = 0; uiL < kMapArray.GetSize(); uiL++)
        {
            NiDelete kMapArray.GetAt(uiL);
        }
    }

    pkCont->InitializeNewController();

    NiStackFree(apkBones);
}
//---------------------------------------------------------------------------
static void FindBoneRootNodes(NiNode* pkNode,
    NiTPrimitiveArray<NiNode*>& kBoneRoots)
{
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetExtraDataSize(); ui++)
    {
        NiExtraData* pkExtra = pkNode->GetExtraDataAt((unsigned short)ui);
        NiStringExtraData* pkStringExtra = NiDynamicCast(NiStringExtraData,
            pkExtra);
        if (pkStringExtra)
        {
            const char* pcString = pkStringExtra->GetValue();
            if (pcString && strstr(pcString, "NiBoneLOD#BoneRoot#"))
            {
                kBoneRoots.Add(pkNode);

                // Do not recurse on children of a bone root node.
                return;
            }
        }
    }

    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiNode* pkChild = NiDynamicCast(NiNode, pkNode->GetAt(ui));
        if (pkChild)
        {
            FindBoneRootNodes(pkChild, kBoneRoots);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::CreateSkinningLODControllers(NiNode* pkScene)
{
    NiTPrimitiveArray<NiNode*> kBoneRoots;
    FindBoneRootNodes(pkScene, kBoneRoots);

    for (unsigned int i = 0; i < kBoneRoots.GetSize(); i++)
    {
        NiNode* pkRoot = kBoneRoots.GetAt(i);
        if (pkRoot)
        {
            ProcessBoneRootNode(pkScene, pkRoot);
        }
    }
}
//---------------------------------------------------------------------------
