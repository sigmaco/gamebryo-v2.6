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
#include "NiScaleTimeCommand.h"
#include "NiMain.h"

NiCommandID NiScaleTimeCommand::ms_kID = 0;
//---------------------------------------------------------------------------
NiScaleTimeCommand::NiScaleTimeCommand(float)
{
    m_fScale = 1.0f;
}
//---------------------------------------------------------------------------
bool NiScaleTimeCommand::Apply(NiSceneViewer* pkViewer)
{
    pkViewer->SetAnimationSpeed(pkViewer->GetAnimationSpeed() + m_fScale);
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiScaleTimeCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiScaleTimeCommand::~NiScaleTimeCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiScaleTimeCommand::Create(NiSceneCommandInfo* pkInfo)
{
    if (pkInfo == NULL)
        return NiNew NiScaleTimeCommand(0.0f);
    if (pkInfo->m_kParamList.GetSize() == 1)
        return NiNew NiScaleTimeCommand(pkInfo->m_kParamList.GetAt(0));
    else
        return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiScaleTimeCommand::GetParameterCount()
{
    return 1;
}
//---------------------------------------------------------------------------
char* NiScaleTimeCommand::GetParameterName(unsigned int uiWhichParam) 
{
    char* pcString = NULL;
    switch(uiWhichParam)
    {
        case 0:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Time Scale Factor");
            break;
        default:
            break;
    }
    return pcString;       
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiScaleTimeCommand::GetDefaultCommandParamaterizations()
{
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Unit Increase Time");
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Unit Decrease Time");
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkArray->Add(pkCommandInfo);
    
    return pkArray;
}
//---------------------------------------------------------------------------
void NiScaleTimeCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("SCALE_TIME",
        NiScaleTimeCommand::Create);
}
//---------------------------------------------------------------------------

