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
#include "NiCameraCutToCommand.h"

#if defined(WIN32)
#include <windows.h>
#endif

NiCommandID NiCameraCutToCommand::ms_kID = 0;
NiWindowRef NiCameraCutToCommand::m_kWnd = 0;
int NiCameraCutToCommand::m_iMsgID = 0;
//---------------------------------------------------------------------------
NiCameraCutToCommand::NiCameraCutToCommand(int iCameraToJumpTo)
{
    m_iCameraIDToCutTo = iCameraToJumpTo;
}
//---------------------------------------------------------------------------
bool NiCameraCutToCommand::Apply(NiSceneViewer* pkViewer)
{
    int iOldId = pkViewer->GetCurrentCameraID();
    if (m_iCameraIDToCutTo >= 0 && 
        m_iCameraIDToCutTo < (int)pkViewer->GetCameraCount()) 
    {
        pkViewer->SelectCamera(m_iCameraIDToCutTo);
        
#if defined (WIN32)
        if (m_kWnd != NULL)
            SendMessage(m_kWnd, m_iMsgID, m_iCameraIDToCutTo, iOldId);
#endif
    }
    else if (((int)pkViewer->GetCameraCount() + m_iCameraIDToCutTo < 
            (int)pkViewer->GetCameraCount()) && 
            ((int)pkViewer->GetCameraCount() + m_iCameraIDToCutTo >= 0))
    {
        m_iCameraIDToCutTo = (int)pkViewer->GetCameraCount() + 
            m_iCameraIDToCutTo;
        pkViewer->SelectCamera(m_iCameraIDToCutTo);
        
#if defined (WIN32)
        if (m_kWnd != NULL)
            SendMessage(m_kWnd, m_iMsgID, m_iCameraIDToCutTo, iOldId);
#endif
    }
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiCameraCutToCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiCameraCutToCommand::~NiCameraCutToCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiCameraCutToCommand::Create(NiSceneCommandInfo* pkInfo)
{
    if (pkInfo == NULL)
    {
        return NiNew NiCameraCutToCommand(0);
    }
    int i = 1;

    if (pkInfo->m_kParamList.GetSize() > 0)
        i = (int) pkInfo->m_kParamList.GetAt(0);
    return NiNew NiCameraCutToCommand(i);
}
//---------------------------------------------------------------------------
unsigned int NiCameraCutToCommand::GetParameterCount()
{
    return 1;
}
//---------------------------------------------------------------------------
char* NiCameraCutToCommand::GetParameterName(unsigned int uiWhichParam)
{
    if (uiWhichParam == 0)
    {
        char* pcString = NiAlloc(char, 256);
        NiStrcpy(pcString, 256, "Camera ID To Cut To");
        return pcString;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiCameraCutToCommand::GetDefaultCommandParamaterizations()
{
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();

    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->m_kParamList.Add(0.0f);
    pkCommandInfo->SetName("Cut To First Camera");
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiCameraCutToCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("CAMERA_CUT_TO",
        NiCameraCutToCommand::Create);

}
//---------------------------------------------------------------------------
void NiCameraCutToCommand::RegisterListener(NiWindowRef kWnd, int iMsgID)
{
    m_kWnd = kWnd;
    m_iMsgID = iMsgID;
}
//---------------------------------------------------------------------------
