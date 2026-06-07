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

#ifndef NIFVIEWER_H
#define NIFVIEWER_H

#include <NiApplication.h>
#include <NiTSet.h>

#if defined(_WII)
#include <NiCursor.h>
#include <NiCursorRenderClick.h>
#endif

#include "NiMenu.h"

class NiTextureBrowser;
class NiStats;

class NifViewer : public NiApplication
{
public:
    NifViewer();
    virtual bool Initialize();
    virtual void Terminate();
    virtual void OnIdle();

    virtual void UpdateInput();
    virtual void ProcessInput();

protected:
    struct Camera : public NiMemObject
    {
        NiCameraPtr m_spCamera;
        NiMatrix3 m_kInitialRot;
        NiPoint3 m_kInitialPos;
    };

    // scene creation/destruction
    bool CreateSceneFromNif(const char *pcFileName);
    void CloseScene();
    bool SceneIsOpen() const;
    void Traverse(NiAVObject* pkObject, bool& bNeedAlphaSort);
    bool CreateCursor();

    // frame rendering system
    bool CreateFrame();
    void UpdateFrame();

    // camera
    void CreateDefaultCamera();
    void CalcCameraFrustum(NiCamera *pkCamera) const;
    void CalcInitialCameraTransform(NiCamera *pkCamera) const;
    bool UpdateTurret();
    void SelectCamera(unsigned int uiCamera);
    void AddCamera(NiCamera *pkCamera);
    NiCamera *GetCamera(unsigned int i) const;
    
    // response to menu commands
    void OpenFile();
#if !defined(_XENON)
    void RequestScreenCapture();
    void ScreenCapture();
#endif  //#if !defined(_XENON)
    void ViewReset();
    void ToggleWireframe(NiMenu::Entry* pkEntry);

    void ResetAnimationTime(bool bUpdate);
    void ToggleTumbleMode(NiMenu::Entry* pkEntry);
    void ToggleInvertGamepad(NiMenu::Entry* pkEntry);
    void AdjustRotationSpeed(bool bFaster);
    void AdjustTranslationSpeed(bool bFaster);
    void AdjustAnimationSpeed(float fAnimationSpeed);
    void AdjustLockUpAxis(unsigned int iValue);

    static bool UpdateGUI_Callback(NiRenderClick* pkClick, void* pvThis);
    static bool UpdateStats_Callback(NiRenderClick* pkClick, void* pvThis);
    static bool UpdateTextureBrowser_Callback(
        NiRenderClick* pkClick, void* pvThis);

    void SwitchCamera(int iIncrement);
    
    // misc
    void AttachGamePad();

    //  Menu
    void CreateMenu();
    void CreateMenuFile();
    void CreateMenuView();
    void CreateMenuAnimation();
    void CreateMenuGamepad();

    static void MenuExecuteCallback(NiMenu::Entry* pkEntry, void* pvData);
    void OnMenuCommand(NiMenu::Entry* pkEntry);

    void DestroyMenu();

    enum MenuCommand
    {
        // File
        LKSAVE,
        SCREEN_SHOT,
        EXIT,

        // Edit
        LKEDIT,

        // View
        WIRE,
        VRAM,
        SMART_MIP,
        VIEW_RESET,
        STATS,
        VSYNC,

        // View->Sorter
        SORTER_NONE,
        SORTER_ALPHA,

        // Animation
        FREEZE,
        ANIMATION_TIME_RESET,
        LOOP,
        ANIMATE_FASTER,
        ANIMATE_SLOWER,
        ANIMATE_SPEED_RESET,

        // Gamepad
        TUMBLE,
        INVERT,
        ROTATE_FASTER,
        ROTATE_SLOWER,
        TRANSLATE_FASTER,
        TRANSLATE_SLOWER,
        LOCK_UP_AXIS_NONE,
        LOCK_UP_AXIS_X,
        LOCK_UP_AXIS_Y,
        LOCK_UP_AXIS_Z,

        Z24,
        Z16,

        COLOR32,
        COLOR16,
        DITHER,

        // SELECT_CAMERA must be the last enumerated command since it is used
        // to create a variable number of entries in a submenu of the View
        // menu.
        SELECT_CAMERA,

        NUM_COMMANDS
    };

    void CreateStats();
    void CreateTextureBrowser();
    void DestroyTextureBrowser();
    void ActivateTextureBrowser();
    void DeactivateTextureBrowser();

    //  Create the camera list
    void CreateCameraList(NiAVObject* pkObject);

    bool m_bMaxImmerseMode;
    bool m_bSaveNifEnable;
    
    // data
    NiObjectPtr* m_pspNifObjects;
    unsigned int m_uiNifObjCnt;
    
    NiNodePtr m_spCenterNode;
    NiTObjectSet<Camera> m_kCameras;
    NiTObjectArray<NiLightPtr> m_kLights;
    NiWireframePropertyPtr m_spWireframe;
    NiDitherPropertyPtr m_spDither;
    NiAlphaAccumulatorPtr m_spAlphaAccumulator;
    NiTurret m_kTurret;
    unsigned int m_uiActiveCamera;
    float m_fRotSpeed, m_fTrnSpeed;
    int m_iRotSpeed, m_iTrnSpeed;
    float m_fAnimationSpeed;
    float m_fLoopTime;
    bool m_bLoop;
    bool m_bTumble;
    bool m_bInvertGamepad;
    int  m_uiLockUpAxis;
    bool m_bFreeze;
    bool m_bCompress;
    unsigned int m_uiFramesTilCapture;
        
#if defined(_WII)
    // Wii Remote pointer
    NiCursorPtr m_spCursor;
    NiFixedString m_kCursorRenderStepName;
    NiFixedString m_kCursorRenderClickName;
    NiCursorRenderClickPtr m_spCursorRenderClick;
    NiTexturePtr m_spNavPointer;
#endif

    // allow setting of near and far plane via the command line
    float m_fAppNear, m_fAppFar;
    
    // file to view (from command line)
    char m_acFileName[NI_MAX_PATH];

    // list of UI screen texture clicks
    NiTPointerList<NiViewRenderClick*> m_kUITextureClicks;

    NiTexturePtr m_spFontTexture;
    unsigned int m_uiFontHeight;
    unsigned int m_uiFontWidth;
    unsigned int m_uiFontColumns;

    NiMenu* m_pkMenu;
    NiTextureBrowser* m_pkTextureBrowser;

    NiMenu::Entry* m_pkCameraEntry;
    NiMenu::Entry* m_pkSorterNoneEntry;
    NiMenu::Entry* m_pkSorterAlphaEntry;
    NiMenu::Entry* m_pkAnimateSpeedEntry;
    NiMenu::Entry* m_pkRotateSpeedEntry;
    NiMenu::Entry* m_pkTranslateSpeedEntry;
    NiMenu::Entry* m_pkLockUpAxisEntry;

    NiStats* m_pkStats;
};

#endif // NIFVIEWER_H
