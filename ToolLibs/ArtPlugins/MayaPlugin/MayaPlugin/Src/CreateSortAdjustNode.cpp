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

#include "MayaPluginPCH.h"
#include "CreateSortAdjustNode.h"


void CreateSortAdjustNode::ProcessScene(NiNodePtr spScene)
{
    RecursiveReplaceNodes(spScene);
}

void CreateSortAdjustNode::SortAdjustReplaceNode(NiNodePtr spNode, 
        NiSortAdjustNode::SortingMode eType)
{
    NiSortAdjustNode* pNewNode = NiNew NiSortAdjustNode;
    if (pNewNode == NULL)
        return;

    pNewNode->SetSortingMode(eType);
    pNewNode->SetName(spNode->GetName());

    for (NiUInt32 uiChildIndex = 0; uiChildIndex < spNode->GetArrayCount(); 
        uiChildIndex++)
    {
        NiAVObjectPtr spChild = spNode->DetachChildAt(uiChildIndex);
        if (spChild)
        {
            pNewNode->AttachChild(spChild);
        }
    }

    NiPropertyList* pPropList = &spNode->GetPropertyList();
    NiTListIterator kIter = pPropList->GetHeadPos();
    while (kIter)
    {
        NiPropertyPtr spProp = pPropList->GetNext(kIter);
        if (spProp)
        {
            pNewNode->AttachProperty(spProp);
        }
    }

    pNewNode->SetTranslate(spNode->GetTranslate());
    pNewNode->SetRotate(spNode->GetRotate());
    pNewNode->SetScale(spNode->GetScale());

    // Move Controllers to control the new SortAdjustNode
    NiTimeControllerPtr spControl;

    while (spControl = spNode->GetControllers())
    {
        spControl->SetTarget(pNewNode);
    }

    //Propagate extra data to new NiSortAdjustNode
    for (NiUInt32 uiExtraDataIndex = 0; 
        uiExtraDataIndex < spNode->GetExtraDataSize(); 
        uiExtraDataIndex++)
    {
        NiExtraData* pkExtraData = spNode->GetExtraDataAt(
            (unsigned short)uiExtraDataIndex);
        if (pkExtraData)
        {
            pNewNode->AddExtraData(pkExtraData);
        }
    }

    NiNode* pkParent = spNode->GetParent();
    if (pkParent)
    {
        // Not sure how to handle root node right now, but this will work
        // for any non-root. Scan the parent's child array and remove at
        // index. Replace with the new sort adjust node.
        for (NiUInt32 uiParentIndex = 0; 
            uiParentIndex < pkParent->GetArrayCount(); uiParentIndex++)
        {
            if (pkParent->GetAt(uiParentIndex) == spNode)
            {
                pkParent->DetachChildAt(uiParentIndex);
                pkParent->SetAt(uiParentIndex, pNewNode);
            }
        }
    }
    return;
}
//-------------------------------------------------------------------------    
void CreateSortAdjustNode::RecursiveReplaceNodes(NiAVObject* pkRoot)
{
    // Traverse the scene graph finding nodes with appropriate strings
    // and replace those nodes with NiSortAdjustNode objects.
    if (NiIsKindOf(NiNode, pkRoot))
    {
        NiNode* pkNode = (NiNode*)pkRoot;

        // Scan for string extra data and replace if necessary.
        // Traverse children.
        NiUInt32 uiNumChildren = pkNode->GetArrayCount();
        for (NiUInt32 uiChild = 0; uiChild < uiNumChildren; ++uiChild)
        {
            NiAVObject* pkObject = pkNode->GetAt(uiChild);
            if (pkObject)
            {
                RecursiveReplaceNodes(pkObject);
            }
        }

        NiUInt32 uiDataIndex = 0;
        NiSortAdjustNode::SortingMode eType = NiSortAdjustNode::SORTING_MAX;
        bool bSuccess = ScanExtraData(pkNode, eType, uiDataIndex);
        if (bSuccess)
        {
            NiNodePtr spNode = pkNode;
            SortAdjustReplaceNode(spNode, eType);
        }
    }
    return;
}
//--------------------------------------------------------------------------
bool CreateSortAdjustNode::ScanExtraData(NiNode* pkNode, 
    NiSortAdjustNode::SortingMode& eType, NiUInt32& uiDataIndex)
{
    for (NiUInt32 uiExtraDataIndex = 0; 
        uiExtraDataIndex < pkNode->GetExtraDataSize(); 
        uiExtraDataIndex++)
    {
        NiExtraData* pkExtraData = pkNode->GetExtraDataAt(
            (unsigned short)uiExtraDataIndex);
        if (NiIsKindOf(NiStringExtraData, pkExtraData) && pkExtraData)
        {
            NiStringExtraData* pkSED = (NiStringExtraData*)pkExtraData;
            const char* pcName = pkSED->GetName();
            const char* pcValue = pkSED->GetValue();

            NiUInt32 uiStrlen = (NiUInt32)strnlen_s(pcValue, 255);
            if (uiStrlen > 254)
            {
                continue;
            }

            if (_stricmp(pcName, "NiSortAdjustNode") == 0)
            {
                if (_stricmp(pcValue, "SORTING_OFF") == 0)
                {
                    eType = NiSortAdjustNode::SORTING_OFF;
                    uiDataIndex = uiExtraDataIndex;
                    return true;
                }
                else if (_stricmp(pcValue, "SORTING_INHERIT") == 0)
                {
                    eType = NiSortAdjustNode::SORTING_INHERIT;
                    uiDataIndex = uiExtraDataIndex;
                    return true;
                }
                else
                {
                    const char* pcNodeName = pkNode->GetName();
                    DtExt_Err("Error:: Unsupported sorting option for sort"
                        " adjust node %s", pcNodeName);
                    return false;
                }
            }
        }
    }
    return false;
}
//--------------------------------------------------------------------------

