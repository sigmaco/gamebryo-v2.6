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
#include "NiCameraZoomExtentsCommand.h"
#include "NiMain.h"

NiCommandID NiCameraZoomExtentsCommand::ms_kID = 0;
//---------------------------------------------------------------------------
NiCameraZoomExtentsCommand::NiCameraZoomExtentsCommand()
{
}
//---------------------------------------------------------------------------
bool NiCameraZoomExtentsCommand::Apply(NiSceneViewer* pkViewer)
{
    unsigned int uiCurrentCameraID = pkViewer->GetCurrentCameraID();
    NiCamera* pkCamera = pkViewer->GetCamera(uiCurrentCameraID, false);
    
    NiNode* pkScene = pkViewer->GetScene();
    float fDiameter = 2.0f*pkScene->GetWorldBound().GetRadius();
    NiPoint3 kOriginLocation = pkScene->GetWorldBound().GetCenter();
    NiPoint3 kCamLocation = pkCamera->GetWorldTranslate();

    NiPoint3 kLocation = (kOriginLocation - kCamLocation) + 
        pkCamera->GetTranslate();
    NiPoint3 kViewVec = (pkCamera->GetRotate()*NiPoint3::UNIT_X)*-1.0f;

    kViewVec.Unitize();
    kLocation = kLocation + kViewVec*fDiameter;
    NiMatrix3 kRotation = pkCamera->GetRotate();

    pkCamera->SetRotate(kRotation);
    pkCamera->SetTranslate(kLocation);
    pkViewer->GetCurrentCameraOrientation()->Reset();
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiCameraZoomExtentsCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiCameraZoomExtentsCommand::~NiCameraZoomExtentsCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiCameraZoomExtentsCommand::Create(NiSceneCommandInfo*)
{
    return NiNew NiCameraZoomExtentsCommand();
}
//---------------------------------------------------------------------------
NiSceneCommand::NiSceneCommandApplyType 
    NiCameraZoomExtentsCommand::GetApplyType()
{
    return APPLY_ANY;
}
//---------------------------------------------------------------------------
bool NiCameraZoomExtentsCommand::CanChangeCameraStates()
{
    return true;
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiCameraZoomExtentsCommand::GetDefaultCommandParamaterizations()
{
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Zoom Extents");
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiCameraZoomExtentsCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("CAMERA_ZOOM_EXTENTS",
        NiCameraZoomExtentsCommand::Create);
}
//---------------------------------------------------------------------------
