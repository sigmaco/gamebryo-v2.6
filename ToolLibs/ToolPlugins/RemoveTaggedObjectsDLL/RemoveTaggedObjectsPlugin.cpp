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

#include "stdafx.h"
#include "RemoveTaggedObjectsPlugin.h"
#include <NiParticle.h>

#define SAMPLE_FRAMES_PER_SECOND 30.0f

//NiAllocatorDeclareStatics(unsigned int, 2048)

// RTTI implementation macro.
NiImplementRTTI(RemoveTaggedObjectsPlugin, NiPlugin);

#define REMOVE_OBJECT_STRING "NiRemoveObject"

//---------------------------------------------------------------------------
RemoveTaggedObjectsPlugin::RemoveTaggedObjectsPlugin() : NiPlugin(
    "Remove Tagged Objects Plug-in",
    "2.0",
    "Removes objects from the scene graph",
    "Removes objects that are tagged with \"NiRemoveObject\" from the scene "
    "graph.\nWill not remove skinned mesh, bones, or objects that are "
    "referenced by other objects in the scene graph.\nIf an NiNode is tagged "
    "to be removed, it and all of its children that can be removed will be.")
{
}
//---------------------------------------------------------------------------
NiPluginInfo* RemoveTaggedObjectsPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("RemoveTaggedObjectsPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool RemoveTaggedObjectsPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool RemoveTaggedObjectsPlugin::DoManagementDialog(
    NiPluginInfo*, NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
bool RemoveTaggedObjectsPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "RemoveTaggedObjectsPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr RemoveTaggedObjectsPlugin::Execute(
    const NiPluginInfo*)
{
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        GetSharedData(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
    {
        NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
        NIASSERT(pkDataList);
        NiExporterOptionsSharedData* pkExporterSharedData = 
            (NiExporterOptionsSharedData*) 
            pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
        bool bWriteResultsToLog = false;
        NiXMLLogger* pkLogger = NULL;

        if (pkExporterSharedData)
        {
            bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
            pkLogger = pkExporterSharedData->GetXMLLogger();
        }

        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("RemoveTaggedObjectsError", "The Remove "
                "Tagged Objects Process Plug-in could not find the "
                "NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The Remove Tagged Objects Process Plug-in could "
                "not find the NiSceneGraphSharedData!", 
                "Scene Graph Shared Data Missing");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    for (NiUInt32 ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        RemoveTaggedObjects(pkSGSharedData->GetRootAt(ui));
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
void RemoveTaggedObjectsPlugin::RemoveTaggedObjects(NiAVObject* pkObject)
{
    NiTPointerMap<NiAVObject*, bool> kMap;
    NiOptimize::BuildHashTable(pkObject, kMap);
    NiTPointerList<NiAVObject*> kList;
    BuildRemoveTaggedObjectsList(pkObject, kList, kMap);
    if (NiIsKindOf(NiNode, pkObject))
        RemoveTaggedObjects((NiNode*) pkObject, kList);

}
//---------------------------------------------------------------------------
bool RemoveTaggedObjectsPlugin::RemoveTaggedObjects(NiNode* pkSceneRoot, 
    NiTPointerList<NiAVObject*>& kList)
{
    while (!kList.IsEmpty())
    {
        NiAVObject* pkObj = kList.RemoveHead();
        if (pkObj)
        {
            NiNode* pkNode = NiDynamicCast(NiNode, pkObj);     
            NiNode* pkParentNode = pkObj->GetParent();
        
            if (pkNode != NULL && pkNode->GetChildCount() > 0)
            {
                for (NiUInt32 ui = 0; ui < pkNode->GetArrayCount(); ui++)
                {
                    NiAVObjectPtr spChild = pkNode->GetAt(ui);
                    if (pkParentNode && spChild)
                    {
                        if (pkSceneRoot)
                            BakeKeys(pkSceneRoot, pkNode, pkParentNode,
                            spChild);
                        pkNode->DetachChild(spChild);
                        pkParentNode->AttachChild(spChild);
                    }
                }
            }
            
            if (pkParentNode)
            {
                pkParentNode->DetachChild(pkObj);
            }
        
        }
    }

    return true;
}

//---------------------------------------------------------------------------
void RemoveTaggedObjectsPlugin::BakeKeys(NiNode* pkSceneRoot, 
    NiNode* pkCurrentParent, NiNode* pkNewParent, NiAVObject* pkChild)
{
    if (pkSceneRoot && pkCurrentParent && pkNewParent && pkChild)
    {
        float fBegin = NI_INFINITY;
        float fEnd = -NI_INFINITY;

        NiTransformController* pkCurrentParentCtrl = NULL; 
        NiTransformController* pkNewParentCtrl = NULL;
        NiTransformController* pkChildCtrl = NULL;

        pkCurrentParentCtrl = NiGetController(NiTransformController, 
            pkCurrentParent);

        pkNewParentCtrl = NiGetController(NiTransformController, 
            pkNewParent);

        pkChildCtrl = NiGetController(NiTransformController, 
            pkChild);

        if (pkCurrentParentCtrl)
        {
            float fTempBegin = pkCurrentParentCtrl->GetBeginKeyTime();
            float fTempEnd = pkCurrentParentCtrl->GetEndKeyTime();
            if (fBegin > fTempBegin)
                fBegin = fTempBegin;
            if (fEnd < fTempEnd)
                fEnd = fTempEnd;
        }

        if (pkChildCtrl)
        {
            float fTempBegin = pkChildCtrl->GetBeginKeyTime();
            float fTempEnd = pkChildCtrl->GetEndKeyTime();
            if (fBegin > fTempBegin)
                fBegin = fTempBegin;
            if (fEnd < fTempEnd)
                fEnd = fTempEnd;
        }

        float fTime = fBegin;
        float fIncTime = 1.0f / SAMPLE_FRAMES_PER_SECOND;
        float fNumKeys = (fEnd - fBegin)/fIncTime;

        if (fBegin == NI_INFINITY && fEnd == -NI_INFINITY || fBegin == fEnd)
        {
            NiTransform kChildTransform = pkChild->GetWorldTransform();
            NiTransform kNewParentTransform =
                pkNewParent->GetWorldTransform();
            NiTransform kInvNewParentTransform;
            kNewParentTransform.Invert(kInvNewParentTransform);

            NiTransform kResultTransform =
                kInvNewParentTransform * kChildTransform;

            pkChild->SetTranslate(kResultTransform.m_Translate);
            pkChild->SetRotate(kResultTransform.m_Rotate);
            
        }
        else
        {
            NiUInt32 uiNumKeys = (NiUInt32)(fNumKeys + 1.5f);
            NiLinPosKey* pkPosKeys = NiNew NiLinPosKey[uiNumKeys];
            NiLinRotKey* pkRotKeys = NiNew NiLinRotKey[uiNumKeys];

            NiTransform kChildTransform = pkChild->GetWorldTransform();
            NiTransform kNewParentTransform =
                pkNewParent->GetWorldTransform();
            NiTransform kInvNewParentTransform;

            NiQuaternion kQuat;

            for (NiUInt32 ui = 0; ui < uiNumKeys; ui++)
            {
                pkSceneRoot->Update(fTime);
                NiMesh::CompleteSceneModifiers(pkSceneRoot);
                kChildTransform = pkChild->GetWorldTransform();
                kNewParentTransform = pkNewParent->GetWorldTransform();
                kNewParentTransform.Invert(kInvNewParentTransform);

                NiTransform kResultTransform = kInvNewParentTransform * 
                    kChildTransform;

                // Bake Pos
                pkPosKeys[ui].SetTime(fTime);
                pkPosKeys[ui].SetPos(kResultTransform.m_Translate);
                
                
                // Bake Rots
                kQuat.FromRotation(kResultTransform.m_Rotate);
                pkRotKeys[ui].SetTime(fTime);
                pkRotKeys[ui].SetQuaternion(kQuat);

                fTime += fIncTime;
            }

            if (pkChildCtrl == NULL)
            {
                pkChildCtrl = NiNew NiTransformController;
                pkChildCtrl->SetTarget(pkChild);
               
            }

            if (pkCurrentParentCtrl)
            {
                pkChildCtrl->SetCycleType(
                    pkCurrentParentCtrl->GetCycleType());
            }

            NiTransformInterpolator* pkInterp = NiNew NiTransformInterpolator;
            pkInterp->ReplacePosData(pkPosKeys, uiNumKeys, 
                NiAnimationKey::LINKEY);
            pkInterp->ReplaceRotData(pkRotKeys, uiNumKeys, 
                NiAnimationKey::LINKEY);
            pkChildCtrl->SetInterpolator(pkInterp);
            pkChildCtrl->ResetTimeExtrema();

        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void RemoveTaggedObjectsPlugin::BuildRemoveTaggedObjectsList(
    NiAVObject* pkObject, NiTPointerList<NiAVObject*>& kList,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    if (pkObject == NULL)
        return;

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode =  (NiNode*) pkObject;
        for (NiUInt32 ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            BuildRemoveTaggedObjectsList(pkNode->GetAt(ui), kList, kMap);
        }
    }

    
    bool bKeep;
    if (kMap.GetAt(pkObject, bKeep))
        return;

    bool bRemoveObject = false;
    NiAVObject* pkParent = pkObject;
    while (pkParent)
    {
        for (NiUInt32 ui = 0; ui < pkParent->GetExtraDataSize(); ui++)
        {
            NiStringExtraData* pkStringExtraData = NiDynamicCast(
                NiStringExtraData, pkParent->GetExtraDataAt((unsigned short)ui));
            if (pkStringExtraData)
            {
                const char* pcString = pkStringExtraData->GetValue();
                if (pcString && strstr(pcString, REMOVE_OBJECT_STRING))
                {
                    bRemoveObject = true;
                    break;
                }
            }
        }

        if (bRemoveObject)
        {
            break;
        }

        pkParent = pkParent->GetParent();
    }

    if (bRemoveObject)
        kList.AddTail(pkObject);

}
//---------------------------------------------------------------------------
