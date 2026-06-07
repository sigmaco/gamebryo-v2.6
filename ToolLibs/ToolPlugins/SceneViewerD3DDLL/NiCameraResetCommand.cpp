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
#include "NiCameraResetCommand.h"


NiCommandID NiCameraResetCommand::ms_kID = 0;
//---------------------------------------------------------------------------
NiCameraResetCommand::NiCameraResetCommand()
{
}
//---------------------------------------------------------------------------
bool NiCameraResetCommand::Apply(NiSceneViewer* pkViewer)
{
    unsigned int uiCurrentCameraID = pkViewer->GetCurrentCameraID();
    NiCamera* pkCamera = pkViewer->GetCamera(uiCurrentCameraID, false);
    NiPoint3 kLocation = 
        pkViewer->GetCameraInitialLocation(uiCurrentCameraID);
    NiMatrix3 kRotation = 
        pkViewer->GetCameraInitialRotation(uiCurrentCameraID);
    pkCamera->SetRotate(kRotation);
    pkCamera->SetTranslate(kLocation);
    pkViewer->GetCurrentCameraOrientation()->Reset();
    
    pkViewer->ResetAnimations();
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiCameraResetCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiCameraResetCommand::~NiCameraResetCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiCameraResetCommand::Create(NiSceneCommandInfo*)
{
    return NiNew NiCameraResetCommand();
}
//---------------------------------------------------------------------------
NiSceneCommand::NiSceneCommandApplyType NiCameraResetCommand::GetApplyType()
{
    return APPLY_ANY;
}
//---------------------------------------------------------------------------
bool NiCameraResetCommand::CanChangeCameraStates()
{
    return true;
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiCameraResetCommand::GetDefaultCommandParamaterizations()
{
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Reset Camera");
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiCameraResetCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("CAMERA_RESET",
        NiCameraResetCommand::Create);
}
//---------------------------------------------------------------------------
