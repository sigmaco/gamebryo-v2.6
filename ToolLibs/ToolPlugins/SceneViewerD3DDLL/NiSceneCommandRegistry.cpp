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
#include "NiSceneCommandRegistry.h"
#include <NiSystem.h>
#include "NiSceneCommand.h"

NiTPrimitiveArray<char*>* NiSceneCommandRegistry::ms_pacCommandIDStrings = 
    NULL;
NiTPrimitiveArray<CommandCreationFunction>* 
    NiSceneCommandRegistry::ms_pafnCreateFunctions = NULL;

//---------------------------------------------------------------------------
NiCommandID NiSceneCommandRegistry::RegisterCommand(char* pcCommandName, 
    CommandCreationFunction pfnFunc)
{
    if (pcCommandName == NULL || pfnFunc == NULL)
        return NI_NOCOMMAND;

    if (ms_pacCommandIDStrings->GetSize() == 0)
    {
        ms_pacCommandIDStrings->Add("NO_COMMAND");
        ms_pafnCreateFunctions->Add(&NiSceneCommand::Create);
    }

    for (unsigned int ui = 0; ui < ms_pacCommandIDStrings->GetSize(); ui++)
    {
        if (NiStricmp(ms_pacCommandIDStrings->GetAt(ui), pcCommandName) == 0)
        {
            NIASSERT(pfnFunc == ms_pafnCreateFunctions->GetAt(ui));
            return (NiCommandID) ui;
        }
    }
    NiCommandID kReturnID = ms_pacCommandIDStrings->GetSize();
    ms_pacCommandIDStrings->Add(pcCommandName);
    ms_pafnCreateFunctions->Add(pfnFunc);
    return kReturnID;
}
//---------------------------------------------------------------------------
NiSceneCommand* NiSceneCommandRegistry::CreateCommand(NiCommandID kId,
    NiSceneCommandInfo* pkInfo)
{  
    CommandCreationFunction pkFunction = 
        (CommandCreationFunction)ms_pafnCreateFunctions->GetAt(kId);
    if (pkFunction != NULL)
    {
        return pkFunction(pkInfo);
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
char* NiSceneCommandRegistry::GetCommandName(NiCommandID kId)
{
    if (kId >= ms_pacCommandIDStrings->GetSize())
        return NULL;

    char* pcName = ms_pacCommandIDStrings->GetAt(kId);
    return pcName;
}
//---------------------------------------------------------------------------
NiCommandID NiSceneCommandRegistry::GetCommandID(char* pcName)
{
    if (pcName == NULL)
        return NI_NOCOMMAND;

    for (unsigned int ui = 0; ui < ms_pacCommandIDStrings->GetSize(); ui++)
    {
        if (NiStricmp(ms_pacCommandIDStrings->GetAt(ui), pcName) == 0)
        {
            return (NiCommandID) ui;
        }
    }

    return NI_NOCOMMAND;
}
//---------------------------------------------------------------------------
bool NiSceneCommandRegistry::IsInitialized()
{
    if (!ms_pacCommandIDStrings || 
        !ms_pafnCreateFunctions)
    {
        return false;
    }
    else
    {
        return true;
    }
}
//---------------------------------------------------------------------------
NiCommandID NiSceneCommandRegistry::GetStartCommandID()
{
    if (IsInitialized())
        return (NiCommandID)1;
    else
        return NI_NOCOMMAND;
}

//---------------------------------------------------------------------------
NiCommandID NiSceneCommandRegistry::GetEndCommandID()
{
    if (IsInitialized())
        return ms_pacCommandIDStrings->GetSize();
    else
        return NI_NOCOMMAND;
}
//---------------------------------------------------------------------------
void NiSceneCommandRegistry::Init()
{
    ms_pacCommandIDStrings = NiNew NiTPrimitiveArray<char*>();
    ms_pafnCreateFunctions = NiNew 
        NiTPrimitiveArray<CommandCreationFunction>;

}
//---------------------------------------------------------------------------
void NiSceneCommandRegistry::Shutdown()
{
    NiDelete ms_pacCommandIDStrings;
    NiDelete ms_pafnCreateFunctions;
}
//---------------------------------------------------------------------------
