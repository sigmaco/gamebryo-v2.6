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

#include "NiScaleDeviceDeltaCommand.h"
#include "NiSceneMouseCommandInfo.h"
#include "NiSceneKeyboardCommandInfo.h"

NiCommandID NiScaleDeviceDeltaCommand::ms_kID = 0;

//---------------------------------------------------------------------------
NiScaleDeviceDeltaCommand::NiScaleDeviceDeltaCommand()
{
}
//---------------------------------------------------------------------------
NiSceneCommand* NiScaleDeviceDeltaCommand::Create(NiSceneCommandInfo*)
{
    return NiNew NiScaleDeviceDeltaCommand();
}
//---------------------------------------------------------------------------
bool NiScaleDeviceDeltaCommand::Apply(NiSceneViewer*)
{
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiScaleDeviceDeltaCommand::GetClassID() 
{
    return ms_kID;
}

//---------------------------------------------------------------------------
unsigned int NiScaleDeviceDeltaCommand::GetParameterCount()
{
    return 1;
}
//---------------------------------------------------------------------------
char* NiScaleDeviceDeltaCommand::GetParameterName(unsigned int uiWhichParam) 
{
    char* pcString = NULL;
    switch(uiWhichParam)
    {
        case 0:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Device Scale Factor");
            break;
        default:
            break;
    }
    return pcString;       
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiScaleDeviceDeltaCommand::GetDefaultCommandParamaterizations()
{
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    
    NiSceneCommandInfo* pkCommandInfo = NULL;
    
    pkCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Increase Mouse Input Rate");
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneMouseCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Decrease Mouse Input Rate");
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkArray->Add(pkCommandInfo);

    pkCommandInfo = NiNew NiSceneKeyboardCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Increase Keyboard Input Rate");
    pkCommandInfo->m_kParamList.Add(1.0f);
    pkArray->Add(pkCommandInfo);
    
    pkCommandInfo = NiNew NiSceneKeyboardCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Decrease Keyboard Input Rate");
    pkCommandInfo->m_kParamList.Add(-1.0f);
    pkArray->Add(pkCommandInfo);

    return pkArray;
}
//---------------------------------------------------------------------------
void NiScaleDeviceDeltaCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("SCALE_DEVICE_DELTA",
        NiScaleDeviceDeltaCommand::Create);
}
//---------------------------------------------------------------------------
