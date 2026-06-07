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
#include "NiAnimationFreezeCommand.h"

NiCommandID NiAnimationFreezeCommand::ms_kID = 0;
                  
//---------------------------------------------------------------------------
NiAnimationFreezeCommand::NiAnimationFreezeCommand()
{
}
//---------------------------------------------------------------------------
bool NiAnimationFreezeCommand::Apply(NiSceneViewer* pkViewer)
{
    pkViewer->SetAnimationMode(!pkViewer->GetAnimationMode());
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiAnimationFreezeCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiAnimationFreezeCommand::~NiAnimationFreezeCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiAnimationFreezeCommand::Create(NiSceneCommandInfo*)
{
    return NiNew NiAnimationFreezeCommand();
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiAnimationFreezeCommand::GetDefaultCommandParamaterizations()
{
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Toggle Animation Pause");
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiAnimationFreezeCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("ANIMATION_FREEZE", 
        NiAnimationFreezeCommand::Create);

}
//---------------------------------------------------------------------------
