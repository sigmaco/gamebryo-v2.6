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

#ifndef NISCALETIMECOMMAND_H
#define NISCALETIMECOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   ScaleTime
// Command ClassID: 8
// Command Purpose: To scale the current timescale so that animations 
//  `               decrease or increase in speed
// Command String Syntax: SCALE_TIME(Scalefactor) SINGLE

class NiScaleTimeCommand : public NiSceneCommand
{
public:
    NiScaleTimeCommand(float fScale);
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiScaleTimeCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    static void Register();

    unsigned int GetParameterCount();
    char* GetParameterName(unsigned int uiWhichParam) ;
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();

protected:
    float m_fScale;
    static NiCommandID ms_kID;
};

#endif