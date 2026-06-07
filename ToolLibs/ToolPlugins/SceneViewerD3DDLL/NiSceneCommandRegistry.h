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

#ifndef NISCENECOMMANDREGISTRY_H
#define NISCENECOMMANDREGISTRY_H

#include "NiTArray.h"

class NiSceneCommand;
class NiSceneCommandInfo;

typedef unsigned int NiCommandID;
typedef NiSceneCommand* (*CommandCreationFunction) (NiSceneCommandInfo*); 
//typedef void (*CreateFunction)(NiStream&,NiObject*&);

#define NI_NOCOMMAND 0

class NiSceneCommandRegistry : public NiMemObject
{ 
public:
    static NiCommandID RegisterCommand(char* pcCommandName, 
        CommandCreationFunction pfnFunc);

    static NiSceneCommand* CreateCommand(NiCommandID kId, 
        NiSceneCommandInfo* pkInfo);
    static char* GetCommandName(NiCommandID kId);
    static NiCommandID GetStartCommandID();
    static NiCommandID GetEndCommandID();
    static NiCommandID GetCommandID(char* pcName);
    static bool IsInitialized();

    static void Init();
    static void Shutdown();
protected:
    static NiTPrimitiveArray<char*>* ms_pacCommandIDStrings;
    static NiTPrimitiveArray<CommandCreationFunction>* ms_pafnCreateFunctions;

};
#endif