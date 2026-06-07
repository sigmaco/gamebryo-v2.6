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
#include "NiSceneCommand.h"

//---------------------------------------------------------------------------
NiSceneCommand::~NiSceneCommand()
{
}
//---------------------------------------------------------------------------
NiSceneCommand* NiSceneCommand::Create(NiSceneCommandInfo*)
{
    return NULL;
}
//---------------------------------------------------------------------------
NiSceneCommand::NiSceneCommandApplyType NiSceneCommand::GetApplyType()
{
    return APPLY_ANY;
}
//---------------------------------------------------------------------------
bool NiSceneCommand::CanChangeCameraStates() 
{
    return false;
}
//---------------------------------------------------------------------------
NiCommandID NiSceneCommand::GetClassID() 
{
    return NI_NOCOMMAND;
}
//---------------------------------------------------------------------------
unsigned int NiSceneCommand::GetParameterCount() 
{
    return 0;
}
//---------------------------------------------------------------------------
char* NiSceneCommand::GetParameterName(unsigned int) 
{
    return NULL;
}
//---------------------------------------------------------------------------
char** NiSceneCommand::GetAllParameterNames()
{
    unsigned int uiParamCount = GetParameterCount();

    if (uiParamCount == 0)
        return NULL;

    char** ppcStrings = NiAlloc(char*, uiParamCount);
    for (unsigned int ui = 0; ui < uiParamCount; ui++)
         ppcStrings[ui] = GetParameterName(ui);

    return ppcStrings;
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiSceneCommand::GetDefaultCommandParamaterizations()
{
    return NULL;
}
//---------------------------------------------------------------------------
bool NiSceneCommand::ToBoolean(float fValue)
{
    if (fValue != 0.0f)
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
void NiSceneCommand::Register()
{
    NiSceneCommandRegistry::RegisterCommand("NO_COMMAND",
        NiSceneCommand::Create);
}
//---------------------------------------------------------------------------
