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
#include "NiCameraRotateCommand.h"

#include "NiSceneMouseCommandInfo.h"
#include "NiSceneMouse.h"

NiCommandID NiCameraRotateCommand::ms_kID = 0;
//---------------------------------------------------------------------------
NiCameraRotateCommand::NiCameraRotateCommand(
    NiPoint3 kRotationAnglesInDegrees, bool bAbsolute)
{
    m_kRotationAngles = kRotationAnglesInDegrees * (NI_PI / 180.0f);
    m_bAbsolute = bAbsolute;
}
//---------------------------------------------------------------------------
bool NiCameraRotateCommand::Apply(NiSceneViewer* pkViewer)
{
    NiViewerCamera* pkViewerCam = pkViewer->GetCurrentCameraOrientation();
    NiCamera* pkCamera = pkViewer->GetCamera(pkViewer->GetCurrentCameraID(), 
        true);

    if (pkViewerCam->GetApplyType() != GetApplyType())
    {
        pkViewer->SetCurrentCameraOrientation(NiNew NiViewerCamera(pkCamera));
        pkViewerCam = pkViewer->GetCurrentCameraOrientation();
        NIASSERT(pkViewerCam != NULL);
    }
    
    if (!m_bAbsolute)
    {
        pkViewerCam->IncrementPitch(m_kRotationAngles.x);
        pkViewerCam->IncrementYaw(m_kRotationAngles.y);
        pkViewerCam->IncrementRoll(m_kRotationAngles.z);
    }
    else
    {
        pkViewerCam->SetPitch(m_kRotationAngles.x);
        pkViewerCam->SetYaw(m_kRotationAngles.y);
        pkViewerCam->SetRoll(m_kRotationAngles.z);
    }
 
    pkViewerCam->UpdateCamera();
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiCameraRotateCommand::GetClassID()
{
     return ms_kID;
}
//---------------------------------------------------------------------------
NiCameraRotateCommand::~NiCameraRotateCommand()
{
    
}
//---------------------------------------------------------------------------
NiSceneCommand* NiCameraRotateCommand::Create(NiSceneCommandInfo* pkInfo)
{
    if (pkInfo == NULL)
    {
        NiPoint3 kRotationAnglesInDegrees(0.0f, 0.0f, 0.0f);
        return NiNew NiCameraRotateCommand(kRotationAnglesInDegrees, false);
    }
    if (pkInfo->m_kParamList.GetSize() != 4)
    {
        NiOutputDebugString("Invalid Number of Arguments for CameraRotate\n");
        return NULL;
    }

    float fScale = pkInfo->m_fSystemDependentScaleFactor;
    NiPoint3 kRotationAnglesInDegrees(pkInfo->m_kParamList.GetAt(0) * fScale,
                                      pkInfo->m_kParamList.GetAt(1) * fScale,
                                      pkInfo->m_kParamList.GetAt(2) * fScale);
    bool bAbsolute = ToBoolean(pkInfo->m_kParamList.GetAt(3));

    return NiNew NiCameraRotateCommand(kRotationAnglesInDegrees, bAbsolute);
}
//---------------------------------------------------------------------------
NiSceneCommand::NiSceneCommandApplyType NiCameraRotateCommand::GetApplyType()
{
    return APPLY_ROTATE_CAM;
}
//---------------------------------------------------------------------------
bool NiCameraRotateCommand::CanChangeCameraStates()
{
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiCameraRotateCommand::GetParameterCount()
{
    return 4;
}
//---------------------------------------------------------------------------
char* NiCameraRotateCommand::GetParameterName(unsigned int uiWhichParam) 
{
    char* pcString = NULL;
    switch(uiWhichParam)
    {
        case 0:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Look Up\\Down(Degrees)");
            break;
        case 1:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Turn Right\\Left(Degrees)");
            break;
        case 2:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Roll (Degrees)");
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
    NiCameraRotateCommand::GetDefaultCommandParamaterizations()
{
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();

    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Look Up");
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Look Down");
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Roll Right");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Roll Left");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Turn Right");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Turn Left");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    NiSceneMouseCommandInfo* pkMouseCommandInfo = 
        NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 0;
    pkMouseCommandInfo->SetName("Mouse-look");
    pkMouseCommandInfo->m_kParamList.Add(0.50f);
    pkMouseCommandInfo->m_kParamList.Add(0.50f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iDyAffectedParameterIndex = 0;
    pkMouseCommandInfo->m_iDxAffectedParameterIndex = 1;
    pkArray->Add(pkMouseCommandInfo);

    pkMouseCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 0;
    pkMouseCommandInfo->SetName("Inverted Mouse-look");
    pkMouseCommandInfo->m_kParamList.Add(-0.50f);
    pkMouseCommandInfo->m_kParamList.Add(0.50f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iDyAffectedParameterIndex = 0;
    pkMouseCommandInfo->m_iDxAffectedParameterIndex = 1;
    pkArray->Add(pkMouseCommandInfo);

    pkMouseCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 0;
    pkMouseCommandInfo->SetName("Mouse-Look(Up/Down)");
    pkMouseCommandInfo->m_kParamList.Add(0.5f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iDyAffectedParameterIndex = 0;
    pkArray->Add(pkMouseCommandInfo);

    pkMouseCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 0;
    pkMouseCommandInfo->SetName("Mouse-Turn(Left/Right)");
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.50f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iDxAffectedParameterIndex = 1;
    pkArray->Add(pkMouseCommandInfo);

    pkMouseCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 1;
    pkMouseCommandInfo->SetName("Wheel-Look(Up/Down)");
    pkMouseCommandInfo->m_kParamList.Add(0.01f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_kParamList.Add(0.0f);
    pkMouseCommandInfo->m_iWheelDeltaAffectedParameterIndex = 0;
    pkMouseCommandInfo->m_lModifiers = NiSceneMouse::MMOD_WHEEL;
    pkArray->Add(pkMouseCommandInfo);

    pkMouseCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkMouseCommandInfo->m_uiClassID = GetClassID();
    pkMouseCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkMouseCommandInfo->m_iPriority = 1;
    pkMouseCommandInfo->SetName("Wheel-Turn(L/R)");
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
    pkMouseCommandInfo->m_iPriority = 1;
    pkMouseCommandInfo->SetName("Wheel-Roll");
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
void NiCameraRotateCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("CAMERA_ROTATE",
        NiCameraRotateCommand::Create);

}
//---------------------------------------------------------------------------
