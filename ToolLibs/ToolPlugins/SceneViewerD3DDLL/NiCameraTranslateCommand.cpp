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
#include "NiCameraTranslateCommand.h"
#include "NiSceneMouseCommandInfo.h"
#include "NiSceneMouse.h"

NiCommandID NiCameraTranslateCommand::ms_kID = 0;
                  
//---------------------------------------------------------------------------
NiCameraTranslateCommand::NiCameraTranslateCommand(NiPoint3 kTranslateVector, 
    bool bAbsolute)
{
    m_kTranslateVector = kTranslateVector;
    m_bAbsolute = bAbsolute;
}
//---------------------------------------------------------------------------
bool NiCameraTranslateCommand::Apply(NiSceneViewer* pkViewer)
{

    unsigned int uiCameraID = pkViewer->GetCurrentCameraID();
    pkViewer->GetCamera(uiCameraID, true);
 
    NiViewerCamera* pkViewerCam = pkViewer->GetCurrentCameraOrientation();
    if (m_bAbsolute)
        pkViewerCam->SetTranslate(m_kTranslateVector);
    else
    {
        float fWorldSizeScale = pkViewer->GetWorldRadius()/100.0f;
        m_kTranslateVector = m_kTranslateVector*fWorldSizeScale;
        pkViewerCam->IncrementTranslate(m_kTranslateVector);
    }
    pkViewerCam->UpdateCamera();

    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiCameraTranslateCommand::GetClassID()
{
     return ms_kID;
}
//---------------------------------------------------------------------------
NiCameraTranslateCommand::~NiCameraTranslateCommand()
{

}
//---------------------------------------------------------------------------
bool NiCameraTranslateCommand::HasAnimationController(NiAVObject* pkObject)
{
    if (pkObject == NULL)
        return false;
    else if (NiGetController(NiTransformController, pkObject) != NULL )
        return true;
    else
        return HasAnimationController(pkObject->GetParent());
}
//---------------------------------------------------------------------------
NiPoint3 NiCameraTranslateCommand::GetXAxis(NiCamera* pkCamera)
{
    return pkCamera->GetRotate()*NiPoint3::UNIT_X;
}
//---------------------------------------------------------------------------
NiPoint3 NiCameraTranslateCommand::GetYAxis(NiCamera* pkCamera)
{
    return pkCamera->GetRotate()*NiPoint3::UNIT_Y;
}
//---------------------------------------------------------------------------
NiPoint3 NiCameraTranslateCommand::GetZAxis(NiCamera* pkCamera)
{
    return pkCamera->GetRotate()*NiPoint3::UNIT_Z;
}
//---------------------------------------------------------------------------
NiSceneCommand* NiCameraTranslateCommand::Create(NiSceneCommandInfo* pkInfo)
{
    if (pkInfo == NULL)
    {
        return NiNew NiCameraTranslateCommand(NiPoint3::ZERO, false);
    }
    if (pkInfo->m_kParamList.GetSize() != 4)
    {
        NiOutputDebugString("Invalid Number of Arguments for "
            "CameraTranslate\n");
        return NULL;
    }
    float fScale = pkInfo->m_fSystemDependentScaleFactor;
    // Parameter 0: Forward/Backward maps to Y in our internal coordinate 
    //   system
    // Parameter 1: Left/Right maps to X in our internal coordinate system
    // Parameter 2: Up/Down maps to Z in our internal coordinate system

    NiPoint3 kTranslateVector(pkInfo->m_kParamList.GetAt(0)*fScale,
        pkInfo->m_kParamList.GetAt(1)*fScale,
        pkInfo->m_kParamList.GetAt(2)*fScale);
    bool bAbsolute = ToBoolean(pkInfo->m_kParamList.GetAt(3));

    return NiNew NiCameraTranslateCommand(kTranslateVector, bAbsolute);
}
//---------------------------------------------------------------------------
unsigned int NiCameraTranslateCommand::GetParameterCount()
{
    return 4;
}
//---------------------------------------------------------------------------
char* NiCameraTranslateCommand::GetParameterName(unsigned int uiWhichParam) 
{
    char* pcString = NULL;
    switch(uiWhichParam)
    {
        case 0:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Forward/Back");
            break;
        case 1:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Up/Down");
            break;
        case 2:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Left/Right");
            break;
        case 3:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Use Absolute Mode");
            break;
        default:
            break;
    }
    return pcString;
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiCameraTranslateCommand::GetDefaultCommandParamaterizations()
{
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();

    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Move Forward");
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Move Backward");
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Move Down");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Move Up");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Move Right");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Move Left");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    NiSceneMouseCommandInfo* pkMouseCommandInfo = 
        NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 0;
    pkMouseCommandInfo->SetName("Mouse-Translate");
    pkMouseCommandInfo->m_kParamList.Add(1.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(1.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iDxAffectedParameterIndex = 0;
    pkMouseCommandInfo->m_iDyAffectedParameterIndex = 2;
    pkArray->Add(pkMouseCommandInfo);

    pkMouseCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 0;
    pkMouseCommandInfo->SetName("Mouse Move(L/R)");
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(1.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iDxAffectedParameterIndex = 2;
    pkArray->Add(pkMouseCommandInfo);

    pkMouseCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 0;
    pkMouseCommandInfo->SetName("Mouse Move(F/B)");
    pkMouseCommandInfo->m_kParamList.Add(1.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iDyAffectedParameterIndex = 0;
    pkArray->Add(pkMouseCommandInfo);

    pkMouseCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 0;
    pkMouseCommandInfo->SetName("Mouse Move(U/D)");
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(1.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iDyAffectedParameterIndex = 1;
    pkArray->Add(pkMouseCommandInfo);

    pkMouseCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 0;
    pkMouseCommandInfo->SetName("Wheel Move(U/D)");
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.01f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iWheelDeltaAffectedParameterIndex = 1;
    pkMouseCommandInfo->m_lModifiers = NiSceneMouse::MMOD_WHEEL;
    pkArray->Add(pkMouseCommandInfo);

    pkMouseCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 0;
    pkMouseCommandInfo->SetName("Wheel Move(L/R)");
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.01f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iWheelDeltaAffectedParameterIndex = 2;
    pkMouseCommandInfo->m_lModifiers = NiSceneMouse::MMOD_WHEEL;
    pkArray->Add(pkMouseCommandInfo);

    
    return pkArray;
}
//---------------------------------------------------------------------------
void NiCameraTranslateCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("CAMERA_TRANSLATE",
        NiCameraTranslateCommand::Create);
}
//---------------------------------------------------------------------------
