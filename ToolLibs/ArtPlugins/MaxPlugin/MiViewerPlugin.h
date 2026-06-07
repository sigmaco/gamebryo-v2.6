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

#ifndef MIVIEWERPLUGIN_H
#define MIVIEWERPLUGIN_H


#include "windows.h"
#include "NiMAX.h"
#include "MiResource.h"
#include "NiMAXOptions.h"


// defined & set in dll.cpp
extern HINSTANCE hInstance;

#define ROLLUP1_OPEN  (1<<0)
#define ROLLUP2_OPEN  (1<<1)
#define ROLLUP3_OPEN  (1<<2)
#define ROLLUP4_OPEN  (1<<3)
#define ROLLUP5_OPEN  (1<<4)
#define ROLLUP6_OPEN  (1<<5)
#define ROLLUP7_OPEN  (1<<6)
#define ROLLUP8_OPEN  (1<<7)
#define ROLLUP9_OPEN  (1<<8)
#define ROLLUP10_OPEN (1<<9)
#define ROLLUP11_OPEN (1<<10)
#define DEFAULT_ROLLUP ROLLUP1_OPEN | ROLLUP11_OPEN

// Parameter block indices
enum ParamBlockIndices {
    PB_RENDERTYPE,
    PB_WINDOWED,
    PB_WINDOW_X,   
    PB_WINDOW_Y,   
    PB_SOFTWARE,    // no longer used
    PB_KYBDBINDING,// no longer used
    PB_JOYBINDING,  // no longer used
    PB_MOUSEBINDING,// no longer used
    PB_TUMBLE_MODE,// no longer used
    PB_KYBD_TR_SPEED,// no longer used
    PB_KYBD_ROT_SPEED,// no longer used
    PB_JOY_TR_SPEED, // no longer used
    PB_JOY_ROT_SPEED,// no longer used
    PB_MOUSE_TR_SPEED, // no longer used
    PB_MOUSE_ROT_SPEED,// no longer used
    PB_REPORT_FRAME_RATE,// no longer used
    PB_IKSOLVER,
    PB_IKSOLVEMODE,
    PB_CS_AS_KEYFRAME,
    PB_UITYPE,
    //PB_UINAME,
    PB_TOOL_PORT,
    PB_TOOL_PRIORITY,
    PB_TOOL_TIMEOUT,
    PB_FIELD_MODE,// no longer used
    PB_PS2NTSC,// no longer used
    PB_PS2Z24,// no longer used
    PB_STRIPIFY_FOR_VIEWER
};

class CSceneViewerDll;
//---------------------------------------------------------------------------
class MiViewerPlugin : public UtilityObj, public IParamArray
{
public:

    MiViewerPlugin();
    ~MiViewerPlugin();

    void BeginEditParams(Interface *pIntf,IUtil *pIUtil);
    void EndEditParams(Interface *pIntf,IUtil *pIUtil);
    inline void DeleteThis() {/*we don't NiDelete this b/c 
                       we use a shared copy always*/}

    // --- Inherited virtual methods of IParamArray ---
    BOOL SetValue(int i, TimeValue t, int v);
    BOOL GetValue(int i, TimeValue t, int &v, Interval &iValid);
    BOOL SetValue(int i, TimeValue t, float v);
    BOOL GetValue(int i, TimeValue t, float &v, Interval &iValid);

public:
    // the indices of each enum here correspond to integers passed into the
    // DoGamebryoView MaxScript function
    enum ViewerType
    {
        MAX_USE_OPTIONS_VIEWER = -1,
        MAX_DX9_VIEWER = 0,
        MAX_SCENEIMMERSE_VIEWER,
        MAX_ANIMATION_VIEWER,
        MAX_CUSTOM_VIEWER,
        MAX_XENON_VIEWER,
        MAX_PS3_VIEWER,
        MAX_WII_VIEWER,
        MAX_PHYSX_VIEWER,
        MAX_D3D10_VIEWER,
        MAX_VIEWER_TYPES
    };

    static IParamMap *m_pAboutPMap;
    static IParamMap *m_pRendererPMap;
    static IParamMap *m_pInteractionPMap;
    static IParamMap *m_pViewerLoaderPMap;
    static IParamMap *m_pQuickSavePMap;

    static void CleanUp();
    static bool ms_bViewButtonVisible;
    static void EnableView();
    static void DisableView();
    static void View(ViewerType eViewerType = MAX_USE_OPTIONS_VIEWER,
        bool bViewSelected = false);

private:

    int RunTime(HINSTANCE hInstance, 
                   Interface *pIntf, 
                   int iRenderer,
                   bool bIsWindowed,
                   int iXRes,
                   int iYRes,
                   bool bIsSoftware,
                   bool bTumbleMode,
                   bool bReportFrameRate,
                   bool bCSAsKeyframe);

    int               m_iFlags;
    Interface        *m_pIntf;

};

//---------------------------------------------------------------------------
#define MI_UTIL_CLASS_ID    Class_ID(0x27a079dd, 0)
//---------------------------------------------------------------------------
extern ClassDesc* GetMAXViewerClassDesc();
//---------------------------------------------------------------------------

#endif
