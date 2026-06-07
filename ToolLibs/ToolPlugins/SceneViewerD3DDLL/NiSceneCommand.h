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

#ifndef NISCENECOMMAND_H
#define NISCENECOMMAND_H

#include "NiSceneCommandRegistry.h"
#include "NiSceneCommandInfo.h"
#include <NiTArray.h>

// Interface for creating command events that drive interaction with
// the SceneViewer
class NiSceneViewer;

class NiSceneCommand  : public NiMemObject
{
public:
    enum NiSceneCommandApplyType
    {
        APPLY_ANY,
        APPLY_ORBIT_CAM,
        APPLY_ROTATE_CAM,
        APPLY_TYPE_COUNT
    };
    virtual ~NiSceneCommand();

    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    virtual bool Apply(NiSceneViewer* pkViewer) = 0;
    virtual NiSceneCommandApplyType GetApplyType();
    virtual bool CanChangeCameraStates();

    static void Register();

    NiCommandID GetClassID();
    unsigned int GetParameterCount();
    virtual char* GetParameterName(unsigned int uiWhichParam);
    virtual char** GetAllParameterNames(); 
    virtual NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();

protected:
    static bool ToBoolean(float fValue);
};

#endif