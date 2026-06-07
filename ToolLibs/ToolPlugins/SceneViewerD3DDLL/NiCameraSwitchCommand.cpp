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
#include "NiCameraSwitchCommand.h"
#include "NiMain.h"

#if defined (WIN32)
#include <windows.h>
#endif

NiCommandID NiCameraSwitchCommand::ms_kID = 0;
NiWindowRef NiCameraSwitchCommand::m_kWnd = 0;
int NiCameraSwitchCommand::m_iMsgID = 0;
//---------------------------------------------------------------------------
NiCameraSwitchCommand::NiCameraSwitchCommand(int iJumpFactor)
{
    m_iJumpFactor = iJumpFactor;
}
//---------------------------------------------------------------------------
bool NiCameraSwitchCommand::Apply(NiSceneViewer* pkViewer)
{
    int uiCurrentCameraID = (int) pkViewer->GetCurrentCameraID();
    int iOldID = uiCurrentCameraID;
    uiCurrentCameraID += m_iJumpFactor;
    if (uiCurrentCameraID < 0)
        pkViewer->SelectCamera(pkViewer->GetCameraCount() -1 );
    else if (uiCurrentCameraID >= (int)pkViewer->GetCameraCount())
        pkViewer->SelectCamera(0);
    else
        pkViewer->SelectCamera(uiCurrentCameraID);

#if defined (WIN32)
    if (m_kWnd != NULL)
        SendMessage(m_kWnd, m_iMsgID, pkViewer->GetCurrentCameraID(), iOldID);
#endif
    pkViewer->ResetAnimations();
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiCameraSwitchCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiCameraSwitchCommand::~NiCameraSwitchCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiCameraSwitchCommand::Create(NiSceneCommandInfo* pkInfo)
{
    if (pkInfo == NULL)
    {
        return NiNew NiCameraSwitchCommand(0);
    }
    int i = 1;

    if (pkInfo->m_kParamList.GetSize() > 0)
        i = (int) pkInfo->m_kParamList.GetAt(0);
    return NiNew NiCameraSwitchCommand(i);
}
//---------------------------------------------------------------------------
unsigned int NiCameraSwitchCommand::GetParameterCount()
{
    return 1;
}
//---------------------------------------------------------------------------
char* NiCameraSwitchCommand::GetParameterName(unsigned int uiWhichParam)
{
    if (uiWhichParam == 0)
    {
        char* pcString = NiAlloc(char, 256);
        NiStrcpy(pcString, 256, "Camera Increment");
        return pcString;
    }
    else
        return NULL;

}

//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiCameraSwitchCommand::GetDefaultCommandParamaterizations()
{
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();

    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkCommandInfo->SetName("Next Camera");
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkCommandInfo->SetName("Previous Camera");
    pkArray->Add(pkCommandInfo);

    return pkArray;
}
//---------------------------------------------------------------------------
void NiCameraSwitchCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("CAMERA_SWITCH",
        NiCameraSwitchCommand::Create);

}
//---------------------------------------------------------------------------
void NiCameraSwitchCommand::RegisterListener(NiWindowRef kWnd, int iMsgID)
{
    m_kWnd = kWnd;
    m_iMsgID = iMsgID;
}
//---------------------------------------------------------------------------
