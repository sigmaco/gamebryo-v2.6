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

// NifGlobals.h

#ifndef NIFGLOBALS_H
#define NIFGLOBALS_H

enum
{
    NIF_DESTROYSCENE = 0x36,
    NIF_CREATESCENE,
    NIF_ADDNIF,
    NIF_REMOVENIF,
    NIF_UPDATEDEFAULTNODE,
    NIF_SCENECHANGED,
    NIF_CAMERALISTCHANGED,
    NIF_BACKGROUNDCOLORCHANGED,
    NIF_USERPREFERENCESCHANGED,
    NIF_CURRENTCAMERACHANGED,
    NIF_TIMECHANGED,
    NIF_TIMINGINFOCHANGED,
    NIF_BROADCASTUPDATE,
    NIF_UPDATECAMERASMENU,
    NIF_SELECTEDOBJECTCHANGED,
    NIF_CAMERAMODECHANGED,
    NIF_CAMERAFOCUSREQUESTED
};

void MainThreadUpdateAllViews(LPARAM params);
extern const int g_iParamNotFound;
extern const float g_fParamNotFound;
extern const CString g_strParamNotFound;
extern const CString g_strBoundShapePath;
extern const CString g_strBoundShapeRootName;

extern bool g_bNeedPromptOnDiscard;
extern unsigned int g_uiMaxFrameRate;
extern float g_fMinFrameTime;

extern bool g_bD3D10;

#define PROPERTY_PANEL_TIMER_ID 1
#define PROPERTY_PANEL_TIMER_INCREMENT 333
#define STATISTICS_TIMER_ID 2
#define STATISTICS_TIMER_INCREMENT 1000

#endif  // #ifndef NIFGLOBALS_H
