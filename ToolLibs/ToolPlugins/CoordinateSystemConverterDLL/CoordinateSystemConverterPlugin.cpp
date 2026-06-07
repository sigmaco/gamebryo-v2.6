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
#include "CoordinateSystemConverterPlugin.h"
#include "CoordinateSystemConverterDialog.h"
#include "CoordinateSystemConverterDLLDefines.h"
#include <NiTransformController.h>
#include <NiMesh.h>
#include <NiSkinningMeshModifier.h>

NiImplementRTTI(CoordinateSystemConverterPlugin, NiPlugin);
CoordinateSystemConverterPlugin* 
    CoordinateSystemConverterPlugin::ms_pkThis = NULL;

//---------------------------------------------------------------------------
CoordinateSystemConverterPlugin::CoordinateSystemConverterPlugin() :
    NiPlugin("Coordinate System Converter", "1.0", 
        "Converts the coordinate system between Maya and Max",
        "Max = Z axis is up. Maya = Y axis is up."),

    m_pkProgressBar(NULL)
{
}
//---------------------------------------------------------------------------
bool CoordinateSystemConverterPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "CoordinateSystemConverterPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr CoordinateSystemConverterPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    ms_pkThis = this;
    // Get the Scene graph from the shared data
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    NiUInt32 uiRootCount = pkSGSharedData->GetRootCount();

    m_pkProgressBar = NiNew NiProgressDialog("Modifing the Coordinate System");
    m_pkProgressBar->Create();
    m_pkProgressBar->SetPosition(0);
    m_pkProgressBar->SetLineOne("Modifing the Coordinate System...");
    m_pkProgressBar->SetRangeSpan(uiRootCount);

    // Get which type of rotation to apply
    NiString strConverter = pkInfo->GetValue(COORDINATE_SYSTEM);

    // Set the matrix for rotation
    NiMatrix3 kMatrix;
    if (strConverter == MAX_TO_MAYA)
    {
        // To convert from max to maya we need to make the Y 
        // axis up.
        kMatrix.MakeXRotation(NI_HALF_PI);
    }
    else // if MAYA_TO_MAX this is the default case
    {
        // To convert from maya to max we need to make the Z 
        // axis up.
        kMatrix.MakeXRotation(-NI_HALF_PI);
    }

    for (NiUInt32 ui = 0; ui < uiRootCount; ui++)
    {
        m_pkProgressBar->StepIt();

        NiNode* pkScene = pkSGSharedData->GetRootAt(ui);

        NiNode* pkNewNode = NiNew NiNode;
        pkNewNode->SetTranslate(kMatrix * pkNewNode->GetTranslate());
        pkNewNode->SetRotate(kMatrix * pkNewNode->GetRotate());

        NiUInt32 uiChildArrayCount = pkScene->GetArrayCount();

        for (NiUInt32 uiChild = 0; uiChild < uiChildArrayCount; uiChild++)
        {
            // Get children
            NiAVObject* pkChild = pkScene->GetAt(uiChild);
            if (pkChild)
            {
                pkNewNode->AttachChild(pkChild);
            }
        }

        // Reset the root bone parent
        NiTPointerList<NiAVObject*> kMeshObjects;
        pkNewNode->GetObjectsByType(&NiMesh::ms_RTTI, kMeshObjects);
        NiTListIterator kIter = kMeshObjects.GetHeadPos();
        while (kIter)
        {
            NiMesh* pkMesh = (NiMesh*)kMeshObjects.GetNext(kIter);
            NiSkinningMeshModifier* pkMod = (NiSkinningMeshModifier*)
                pkMesh->GetModifierByType(&NiSkinningMeshModifier::ms_RTTI);
            if (pkMod)
            {
                if (pkMod->GetRootBoneParent() == pkScene)
                {
                    pkMod->SetRootBoneParent(pkNewNode);
                }
            }
        }

        pkScene->AttachChild(pkNewNode);
        pkScene->Update(0.0f);
    }

    m_pkProgressBar->Destroy();
    NiDelete m_pkProgressBar;
    m_pkProgressBar = NULL;

    ms_pkThis = NULL;

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* CoordinateSystemConverterPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("CoordinateSystemConverterPlugin");
    pkPluginInfo->SetType("PROCESS");
    pkPluginInfo->SetValue(COORDINATE_SYSTEM, MAYA_TO_MAX);
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool CoordinateSystemConverterPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool CoordinateSystemConverterPlugin::DoManagementDialog(
        NiPluginInfo* pkInitialInfo, NiWindowRef hWndParent)
{
    CoordinateSystemConverterDialog kDialog(m_hModule, hWndParent, 
        pkInitialInfo);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
        break;
    case IDCANCEL:
        return false;
        break;
    default:
        NIASSERT(false);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
