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
#include "NiCameraTumbleCommand.h"
#include "NiOrbitCamera.h"

NiCommandID NiCameraTumbleCommand::ms_kID = 0;
  
//---------------------------------------------------------------------------
NiCameraTumbleCommand::NiCameraTumbleCommand(NiPoint3 kRotationAnglesInDegrees,
    NiPoint3 kCenterPoint, bool bAbsolute)
{
    m_kRotationAngles = kRotationAnglesInDegrees * (NI_PI / 180.0f);
    m_kCenterPoint = kCenterPoint;
    m_bAbsolute = bAbsolute;
}
//---------------------------------------------------------------------------
bool NiCameraTumbleCommand::Apply(NiSceneViewer* pkViewer)
{
    NiViewerCamera* pkCameraViewer = pkViewer->GetCurrentCameraOrientation();
    NiCamera* pkCamera = pkViewer->GetCamera(pkViewer->GetCurrentCameraID(), 
        true);
    NiOrbitCamera* pkOrbitCam = NULL;
    if (! NiIsKindOf(NiOrbitCamera, pkCameraViewer))
    {
        if (pkCamera->GetParent() != pkViewer->GetMasterScene() &&
           pkCamera->GetParent()->GetParent() != pkViewer->GetMasterScene())
            return false;
        NiDelete pkCameraViewer;
        pkViewer->SetCurrentCameraOrientation(NiNew NiOrbitCamera(pkCamera));
        pkOrbitCam = (NiOrbitCamera*) pkViewer->GetCurrentCameraOrientation();
        NIASSERT(pkOrbitCam != NULL);
        pkOrbitCam->SetOrbitPoint(m_kCenterPoint + 
            pkViewer->GetScene()->GetWorldBound().GetCenter());
    }

    if (pkOrbitCam == NULL)
        pkOrbitCam = (NiOrbitCamera*) pkViewer->GetCurrentCameraOrientation();

    if (!m_bAbsolute)
    {
        pkOrbitCam->IncrementPitch(m_kRotationAngles.x);
        pkOrbitCam->IncrementYaw(m_kRotationAngles.y);
        pkOrbitCam->IncrementRoll(m_kRotationAngles.z);
    }
    else
    {
        pkOrbitCam->SetPitch(m_kRotationAngles.x);
        pkOrbitCam->SetYaw(m_kRotationAngles.y);
        pkOrbitCam->SetRoll(m_kRotationAngles.z);
    }
 
    pkOrbitCam->UpdateCamera();
    return true;

}
//---------------------------------------------------------------------------
NiCommandID NiCameraTumbleCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiCameraTumbleCommand::~NiCameraTumbleCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiCameraTumbleCommand::Create(NiSceneCommandInfo* pkInfo)
{
    if (pkInfo == NULL)
    {
        return NiNew NiCameraTumbleCommand(NiPoint3::ZERO, NiPoint3::ZERO, 
            false);
    }

    if (pkInfo->m_kParamList.GetSize() != 7)
    {
        NiOutputDebugString("Invalid Number of Arguments for CameraTumble\n");
        return NULL;
    }
    float fScale = pkInfo->m_fSystemDependentScaleFactor;
    NiPoint3 kRotationAnglesInDegrees(pkInfo->m_kParamList.GetAt(0)*fScale,
        pkInfo->m_kParamList.GetAt(1)*fScale,
        pkInfo->m_kParamList.GetAt(2)*fScale);
    NiPoint3 kCenterPoint(pkInfo->m_kParamList.GetAt(3),
        pkInfo->m_kParamList.GetAt(4), 
        pkInfo->m_kParamList.GetAt(5));
    bool bAbsolute = ToBoolean(pkInfo->m_kParamList.GetAt(6));

    return NiNew NiCameraTumbleCommand(kRotationAnglesInDegrees, kCenterPoint,
        bAbsolute);
}
//---------------------------------------------------------------------------
NiSceneCommand::NiSceneCommandApplyType NiCameraTumbleCommand::GetApplyType()
{
    return APPLY_ORBIT_CAM;
}
//---------------------------------------------------------------------------
bool NiCameraTumbleCommand::CanChangeCameraStates()
{
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiCameraTumbleCommand::GetParameterCount()
{
    return 7;
}
//---------------------------------------------------------------------------
char* NiCameraTumbleCommand::GetParameterName(unsigned int uiWhichParam) 
{
    char* pcString = NULL;
    switch(uiWhichParam)
    {
        case 0:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Roll (Degrees)");
            break;
        case 1:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Yaw (Degrees)");
            break;
        case 2:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Pitch (Degrees)");
            break;
        case 3:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "X Offset");
            break;
        case 4:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Y Offset");
            break;
        case 5:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Z Offset");
            break;
        case 6:
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
    NiCameraTumbleCommand::GetDefaultCommandParamaterizations()
{
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();

    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Orbit Up");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Orbit Down");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Orbit Right");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::CONTINUOUS_PRESSED;
    pkCommandInfo->m_iPriority = 1;
    pkCommandInfo->SetName("Orbit Left");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    return pkArray;
}
//---------------------------------------------------------------------------
void NiCameraTumbleCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("CAMERA_TUMBLE",
        NiCameraTumbleCommand::Create);
}
//---------------------------------------------------------------------------
