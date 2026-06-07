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

#ifndef NISCALEDEVICEDELTACOMMAND_H
#define NISCALEDEVICEDELTACOMMAND_H

#include "NiSceneCommand.h"

class NiScaleDeviceDeltaCommand : public NiSceneCommand
{
public:
    NiScaleDeviceDeltaCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    bool Apply(NiSceneViewer* pkViewer);

    static void Register();
    NiCommandID GetClassID();

    unsigned int GetParameterCount();
    char* GetParameterName(unsigned int uiWhichParam) ;
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();

protected:
    static NiCommandID ms_kID;
};

#endif