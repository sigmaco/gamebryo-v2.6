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
#include "NiCameraAdjustFrustumCommand.h"


NiCommandID NiCameraAdjustFrustumCommand::ms_kID = 0;
                    
//---------------------------------------------------------------------------
NiCameraAdjustFrustumCommand::NiCameraAdjustFrustumCommand(float fNearAdjust,
    float fFarAdjust, bool bAbsolute)
{
    m_bAbsolute = bAbsolute;
    m_fFarAdjust = fFarAdjust;
    m_fNearAdjust = fNearAdjust;
}
//---------------------------------------------------------------------------
bool NiCameraAdjustFrustumCommand::Apply(NiSceneViewer* pkViewer)
{
    NiCamera* pkCamera = pkViewer->GetCamera(pkViewer->GetCurrentCameraID());
    NiFrustum kFrustum = pkCamera->GetViewFrustum();

//    float fScaleFactor = pkViewer->GetScene()->GetWorldBound().GetRadius()*
//        2.0f;
//    m_fFarAdjust = (m_fFarAdjust / 100.0f)*fScaleFactor;
//    m_fNearAdjust = (m_fNearAdjust / 100.0f)*fScaleFactor;

    if (m_bAbsolute)
    {
        kFrustum.m_fFar = m_fFarAdjust;
        kFrustum.m_fNear = m_fNearAdjust;
    }
    else
    {
        kFrustum.m_fFar += m_fFarAdjust;
        kFrustum.m_fNear += m_fNearAdjust;
    }

    if (kFrustum.m_fFar < 0.0)
        kFrustum.m_fFar = 0.0f;

    if (kFrustum.m_fNear < 0.0)
        kFrustum.m_fNear = 0.0f;

    if (kFrustum.m_fNear >= kFrustum.m_fFar)
        return false;

    pkCamera->SetViewFrustum(kFrustum);

    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiCameraAdjustFrustumCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiCameraAdjustFrustumCommand::~NiCameraAdjustFrustumCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiCameraAdjustFrustumCommand::Create(
    NiSceneCommandInfo* pkInfo)
{
    if (pkInfo == NULL)
    {
        return NiNew NiCameraAdjustFrustumCommand(0.0f, 0.0f, false);
    }
    else if (pkInfo->m_kParamList.GetSize() == 3)
    {
        return NiNew NiCameraAdjustFrustumCommand(
            pkInfo->m_kParamList.GetAt(0),
            pkInfo->m_kParamList.GetAt(1),
            ToBoolean(pkInfo->m_kParamList.GetAt(2)));
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
unsigned int NiCameraAdjustFrustumCommand::GetParameterCount()
{
    return 3;
}
//---------------------------------------------------------------------------
char* NiCameraAdjustFrustumCommand::GetParameterName(
    unsigned int uiWhichParam) 
{
    char* pcString = NULL;
    switch(uiWhichParam)
    {
        case 0:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Near Plane Adjust");
            break;
        case 1:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Far Plane Adjust");
            break;
        case 2:
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
    NiCameraAdjustFrustumCommand::GetDefaultCommandParamaterizations()
{
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Increase Far Plane");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Decrease Far Plane");
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Increase Near Plane");
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Decrease Near Plane");
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkArray->Add(pkCommandInfo);

    return pkArray;
}
//---------------------------------------------------------------------------
void NiCameraAdjustFrustumCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("ADJUST_FRUSTUM",
        NiCameraAdjustFrustumCommand::Create);
}
//---------------------------------------------------------------------------
