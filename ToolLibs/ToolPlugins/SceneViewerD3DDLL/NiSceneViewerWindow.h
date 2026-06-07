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

#ifndef NISCENEVIEWERWINDOW_H
#define NISCENEVIEWERWINDOW_H

#include "NiSceneChangeInfo.h"
#include "NiSceneEventHandler.h"

class NiSceneViewerWindow : public NiRefObject
{
public:
    NiSceneViewerWindow();
    virtual ~NiSceneViewerWindow();
    bool Initialize(NiScenePtr spScene,
        NiSceneChangeInfo* pkChangeInfo = NULL);
    bool Initialize(const char* pcFilename, 
        NiSceneChangeInfo* pkChangeInfo = NULL);
    bool ReinitializeCameras();

    void CloseScene();
    inline bool Terminate();
    virtual void OnIdle() = 0;
    inline bool SetUI(NiUIMap* ui);
    inline NiUIMap* GetUI();
    
    // Get input devices
    inline NiSceneKeyboard* GetKeyboard() const;
    inline NiSceneMouse* GetMouse() const;
    
    // window painting
    virtual bool OnWindowDraw(NiContextRef pkDC) = 0;
    virtual bool OnWindowErase(NiContextRef pkDC) = 0;

    // window maintenance
    virtual bool OnWindowCreate(NiCreateStructRef pkCreateStructRef) = 0;
    virtual bool OnWindowResize(int iWidth, int iHeight, 
        unsigned int uiSizeType, NiWindowRef pkWnd) = 0;
    virtual bool OnWindowDestroy(NiWindowRef pkWnd, bool bOption) = 0;
    virtual bool OnWindowMove(int iXPos, int iYPos, NiWindowRef pkWnd) = 0;

    // Events

    // command processing
    inline bool OnCommand(int iParam1, int iParam2, NiWindowRef pkCtl);

    // input/system processing (returns true to continue, false to exit)
    virtual bool Process(long* plRetVal) = 0;

    // default processing
    virtual bool OnDefault(NiEventRef pkEventRecord) = 0;

    static void SetAcceleratorReference(NiAcceleratorRef pkAccel);
    static NiAcceleratorRef GetAcceleratorReference();
    static void SetInstanceReference(NiInstanceRef pkInstance);
    static NiInstanceRef GetInstanceReference();
    
    void PreMainLoop();
    long MainLoopTick();
    void PostMainLoop();

    // window size
    inline void SetWidth(unsigned int uiWidth);
    inline unsigned int GetWidth() const;
    inline void SetHeight(unsigned int uiHeight);
    inline unsigned int GetHeight() const;

    inline void SetParentWidth(unsigned int uiWidth);
    inline unsigned int GetParentWidth() const;
    inline void SetParentHeight(unsigned int uiHeight);
    inline unsigned int GetParentHeight() const;

    inline void SetWindowReference(NiWindowRef pkWnd);
    inline NiWindowRef GetWindowReference();
    inline void SetRenderWindowReference(NiWindowRef pkWnd);
    inline NiWindowRef GetRenderWindowReference();
    
    inline void SetFullscreen(bool bOn);
    inline bool GetFullscreen() const;
    inline void SetHardware(bool bOn);
    inline bool GetHardware() const;
    inline void SetStencil(bool bOn);
    inline bool GetStencil() const;
    inline void SetMultitexture(bool bOn);
    inline bool GetMultitexture() const;
    inline void SetBackgroundColor(NiColor kColor);
    inline NiColor GetBackgroundColor();

    inline void SetD3D10(bool bD3D10);
    inline bool GetD3D10() const;

    void SetAnimationLooping(bool bLoop);
    bool GetAnimationLooping();

    void SetAnimationLoopEndTime(float fEndTime);
    float GetAnimationLoopEndTime();
    void SetAnimationLoopStartTime(float fStartTime);
    float GetAnimationLoopStartTime();

    void SetFrameRateLimit(unsigned int uiCeiling);
    unsigned int GetFrameRateLimit();

    void SetDefaultCamera(NiNodePtr spCamNode);
    
    NiSceneViewer* GetSceneViewer();

    void CalcCameraFrustum(NiCamera* pkCamera) const;
    void CalcInitialCameraTransform(NiCamera* pkCamera) const;

protected:

    bool CreateScene(NiScenePtr spScene);
    bool CreateCamera(NiScenePtr spScene);
    bool SceneHasLights(NiNode* pkScene);

    virtual bool CreateRenderer()=0;
    float m_fElapsedTime;
    float m_fStartTime;
    float m_fLastTime;

    NiCameraPtr m_spCamera;
    NiLightPtr m_spDefaultLight;
    NiNodePtr m_spDefaultCamNode;
    NiNode* m_spScene;
    NiRendererPtr m_spRenderer;
    NiScenePtr m_spTheSceneDatabase;

    NiSceneViewer* m_pkSceneViewer;
    NiSceneEventHandler* m_pkSceneEventHandler;
    NiSceneChangeInfo* m_pkSceneChangeInfo;
    bool m_bFullscreen;
    bool m_bHardware;
    bool m_bStencil;
    bool m_bMultitexture;
    bool m_bOwnsShadowManager;

    bool m_bD3D10;

    unsigned int m_uiBitDepth;
    NiColor m_kBackground;
    static NiAcceleratorRef ms_pkAccel;
    static NiInstanceRef ms_pkInstance;

    unsigned int m_uiWidth;
    unsigned int m_uiParentWidth;
    unsigned int m_uiHeight;
    unsigned int m_uiParentHeight;

    NiWindowRef m_pkWnd;
    NiWindowRef m_pkRenderWnd;
    NiSceneKeyboard* m_pkKeyboard;
    NiSceneMouse* m_pkMouse;

    bool m_bIsAlive;
};

#include "NiSceneViewerWindow.inl"
#endif //NISCENEVIEWERWINDOW_H
