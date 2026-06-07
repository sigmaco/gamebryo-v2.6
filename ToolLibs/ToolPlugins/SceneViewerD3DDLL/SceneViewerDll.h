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
#ifndef SCENEVIEWERDLL_H
#define SCENEVIEWERDLL_H

#include "StdAfx.h"
#include "NiScene.h"
#include "NiUIMap.h"
#include "NiWin32SceneViewerWindow.h"
#include "NiTreeCtrl.h"

class SceneViewerInfo : public NiRefObject
{
public:
    SceneViewerInfo();
    ~SceneViewerInfo();

    HINSTANCE m_hI;
    HWND m_hWnd;
    int m_iWinMode;
    bool m_bWindowActive;
    NiScenePtr m_spScene;
    NiUIMap* m_pkUIMap;
    NiPoint2 m_kDimensions;
    NiPoint2 m_kLocation;
    NiNodePtr m_spCameraNode;
    bool m_bStencilBuffer;
    bool m_bFullScreen;
    bool m_bSoftware;
    bool m_bD3D10;
    NiColor m_kBackgroundColor;
    char* m_pcUIMapFilename;
    char* m_pcDefaultFilePath;
    bool m_bAppIsDead;
    NiBinaryStream* m_pkSceneStream;
    NiSceneViewerWindow* m_pkSceneViewerWindow;
    char* m_pcWindowTitle;
    unsigned int m_uiFrameRate;
    bool m_bLoopAnimations;
    float m_fAnimationLoopStartTime;
    float m_fAnimationLoopEndTime;
};

NiSmartPointer(SceneViewerInfo);
 
// This class is exported from the SceneViewerDll.dll
class CSceneViewerDll : public NiRefObject //: SceneViewerDll
{
public:
    CSceneViewerDll();
    ~CSceneViewerDll();
    
    void Run(SceneViewerInfo* pkViewerInfo);
    void RunThreaded(SceneViewerInfo* pkViewerInfo);
    bool IsRunning();
    void Shutdown();

    char* RunConfigurableUIMapEditor(char* pcExecPath, char* pcParentPath, 
        char* pcStartFile);

    void RunSceneGraphTreeViewer(NiInstanceRef kInstanceRef,
        NiWindowRef kWindowRef, NiNode* pkScene,
        char* pcTitle = "SceneGraph Tree View", int iXPos = CW_USEDEFAULT, 
        int iYPos = CW_USEDEFAULT, int iWidth = 640, int iHeight = 480);

    bool RunColorSelector(NiWindowRef kWindowRef, NiColor& kColor);

protected:
    HANDLE hThread;
    SceneViewerInfoPtr m_spWindow;
    NiTreeCtrl* m_pkTreeControl;

    // Cg Shader Plugin
    HMODULE m_hNiCgShaderLib;
};

NiSmartPointer(CSceneViewerDll);
#endif
