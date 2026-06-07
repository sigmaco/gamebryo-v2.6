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

#ifndef NIADJUSTFRUSTUMCOMMAND_H
#define NIADJUSTFRUSTUMCOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   CameraAdjustFrustum
// Command ClassID: 16
// Command Purpose: To scale the current timescale so that animations 
//  `               decrease or increase in speed
// Command String Syntax: ADJUST_FRUSTUM(fNearAdjust, fFarAdjust,
//                                       bool bAbsolute) SINGLE

class NiCameraAdjustFrustumCommand : public NiSceneCommand
{
public:
    NiCameraAdjustFrustumCommand(float fNearAdjust, float fFarAdjust, 
        bool bAbsolute);
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiCameraAdjustFrustumCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    unsigned int GetParameterCount();
    char* GetParameterName(unsigned int uiWhichParam) ;
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
    static void Register();

protected:
    bool m_bAbsolute;
    float m_fNearAdjust;
    float m_fFarAdjust;
    static NiCommandID ms_kID;
};

#endif