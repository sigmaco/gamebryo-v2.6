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

#ifndef PHYSXNIFVIEWER_H
#define PHYSXNIFVIEWER_H

#include <NiPhysXManager.h>

#include <NiApplication.h>
#include <NiTSet.h>
#include <NiPhysX.h>
#include <NiPhysXFluid.h>
#include <NiPhysXParticle.h>
#include <NiSystemCursor.h>
#include "NiMenu.h"
#include "GroundPlane.h"

#if defined(_WII)
#include <NiCursor.h>
#include <NiCursorRenderClick.h>
#endif

class NiStats;

class PhysXNifViewer : public NiApplication
{
public:
    PhysXNifViewer();
    virtual bool Initialize();
    virtual void Terminate();
    virtual void OnIdle();
    virtual void UpdateInput();

protected:
    struct Camera : public NiMemObject
    {
        NiCameraPtr m_spCamera;
        NiMatrix3 m_kInitialRot;
        NiPoint3 m_kInitialPos;
    };

    void ProcessInputWithTime(float fDt);

    // scene creation/destruction
    bool CreateSceneFromNif(const char *pcFileName);
    bool CreatePhysX(NiTObjectArray<NiPhysXPropPtr>& kProps);
    void CloseScene();
    bool SceneIsOpen() const;
    
    // Functions that walk through the scene graph or props
   // void Precache(NiAVObject* pkObject);
    void Traverse(NiAVObject* pkObject, bool& bNeedAlphaSort);
    void TraverseProps(NiTObjectArray<NiPhysXPropPtr>& kProps,
        bool& bHaveFluids, bool& bNeedHardware);
    void PageMeshes();

    // camera
    void CreateDefaultCamera();
    void CalcCameraFrustum(NiCamera *pkCamera) const;
    void CalcInitialCameraTransform(NiCamera *pkCamera) const;
    void LockGravity(NiAVObject *pkObjectHoldingXform);
    void GravityFromCamera(NiCamera *pkCamera);
    bool UpdateTurret(const float fDt);
    void SelectCamera(unsigned int uiCamera);
    void AddCamera(NiCamera *pkCamera);
    NiCamera *GetCamera(unsigned int i) const;
    
    // response to menu commands
    void OpenFile();
    void ViewReset();
    void ToggleWireframe(NiMenu::Entry* pkEntry);

    void ToggleFrozen();
    void ToggleDisableKeyframe();
    void AdjustStepLength(float fStepLength);
    void ResetAnimationTime(NiFixedString& kStateName);
    void ToggleInvertGamepad(NiMenu::Entry* pkEntry);
    void AdjustRotationSpeed(bool bFaster);
    void AdjustTranslationSpeed(bool bFaster);
    void AdjustAnimationSpeed(float fAnimationSpeed);
    void ToggleGravityMode(NiMenu::Entry* pkEntry);
    void ToggleDebugRender(NiMenu::Entry* pkEntry);
    void ToggleClothDebug(bool bState);
    void AdjustDebugScale(float fRatio);
    void ToggleGroundPlane(NiMenu::Entry* pkEntry);
    void AdjustGroundHeight(const int iSign);
    void AdjustForceExponent(int iNewVal);

    void SwitchCamera(int iIncrement);
    
    // PhysX
    void ResetPhysXSDKParams(const NxParameter kParam = NX_PARAMS_NUM_VALUES);
   
    // Frame rendering
    bool CreateFrame();
    void UpdateFrame();
    NiTPointerList<NiViewRenderClick*> m_kUITextureClicks;
    NiViewRenderClickPtr m_spCursorRenderClick;

    static bool UpdateGUI_Callback(NiRenderClick* pkClick, void* pvThis);
    static bool UpdateStats_Callback(NiRenderClick* pkClick, void* pvThis);

    // Turn off key-frame animation on object driven by PhysX
    void DisableKeyframes(bool bDisable);
    
    // Ground planes
    void AddGroundPlane();
    void RemoveGroundPlane();
    
    // Input devices
    void AttachKeyboard();
    void AttachGamePad();
    
    // Picking
    bool CreateCursor();
    
    // Forces
    bool SelectForceActor(NiPoint3& kOrigin, NiPoint3& kDir);
    void ApplyForce(int iPtrX, int iPtrY);
    
    //  Menu
    void CreateMenuFont();
    void CreateMenu();
    void CreateMenuFile();
    void CreateMenuView();
    void CreateMenuAnimation();
    void CreateMenuGamepad();
    void CreateMenuSDK();
    void CreateMenuGround();

    // Reset state names
    void AddStateName(NiFixedString& kName);

    // FPS counter
    void CreateStats();

    static void MenuExecuteCallback(NiMenu::Entry* pkEntry, void* pvData);
    void OnMenuCommand(NiMenu::Entry* pkEntry);

    void DestroyMenu();

    enum MenuCommand
    {
        // File
        EXIT,

        // View
        WIRE,
        VIEW_RESET,
        STATS,

        // View->Sorter
        SORTER_NONE,
        SORTER_ALPHA,

        // Animation
        FREEZE,
        STEP,
        STEP_LARGER,
        STEP_SMALLER,
        STEP_RESET,
        DISABLE_KEYS,
        ANIMATE_FASTER,
        ANIMATE_SLOWER,
        ANIMATE_SPEED_RESET,

        // Controls
        INVERT,
        ROTATE_FASTER,
        ROTATE_SLOWER,
        TRANSLATE_FASTER,
        TRANSLATE_SLOWER,
        GRAVITY,
        
        // SDK
        SDK_RESET,
        FORCE_EXPONENT_BIGGER,
        FORCE_EXPONENT_SMALLER,
        FORCE_EXPONENT_RESET,
        DEBUG_ON,
        DEBUG_SCALE_BIGGER,
        DEBUG_SCALE_SMALLER,
        DEBUG_SCALE_RESET,

        // Ground plane
        GROUND_PLANE_ON,
        GROUND_PLANE_HIGHER,
        GROUND_PLANE_LOWER,

        // SELECT_CAMERA must be explicitly enumerated because it is used
        // to create a variable number of entries in a submenu of the View
        // menu. Max of 1000 cameras.
        SELECT_CAMERA = 1000,
        
        // PHYSICS_RESET must be explicitly enumerated because it is used
        // to create a variable number of entries in a submenu of the
        // Animation menu.
        PHYSICS_RESET = 2000
    };

    //  Create the camera list
    void CreateCameraList(NiAVObject* pkObject);

    // Min frame step and other constants
    static const float MAX_FRAME_STEP;
    static const float INIT_ROT_SPEED;
    static const float INIT_TRN_SPEED;
    static const float FORCE_SPRING_CONST;

    // Physics members
    NiPhysXManager* m_pkPhysManager;
    NiPhysXScenePtr m_spPhysScene;
    GroundPlanePtr m_spGroundPlane;
    
    // SDK parameters
    NiPhysXSDKDescPtr m_spSDKDefaults;
    bool m_bDebugGeom;
    float m_fDebugScale;
    bool m_bDoingGround;
    float m_fGroundHeight;
    int m_iForceExponent;
    
    // data
    NiObjectPtr* m_pspNifObjects;
    unsigned int m_uiNifObjCnt;
    
    NiNodePtr m_spBoundNode;
    NiTObjectSet<Camera> m_kCameras;
    NiTObjectArray<NiLightPtr> m_kLights;
    
    NiWireframePropertyPtr m_spWireframe;
    NiDitherPropertyPtr m_spDither;
    NiAlphaAccumulatorPtr m_spAlphaAccumulator;
    
    NiTurret m_kTurret;
    unsigned int m_uiActiveCamera;
    int m_iRotSpeed, m_iTrnSpeed;
    float m_fAnimationSpeed;
    bool m_bInvertGamepad;
    bool m_bLockGravity;
    bool m_bPreferKeyboard;
    bool m_bGravityProvided;
    
    // Timing
    float m_fSimulationTime;
    bool m_bFreeze;
    bool m_bSingleStep;
    float m_fStepLength;
    float m_fStepLengthRem;
    bool m_bDisableKeyframes;

#if defined(_WII)
    // Wii Remote navigation cursor
    NiCursorPtr m_spNavCursor;
    NiCursorRenderClickPtr m_spNavCursorRenderClick;
    NiTexturePtr m_spNavPointer;
#endif

    // allow setting of near and far plane via the command line
    float m_fAppNear, m_fAppFar;
    
    // file to view (from command line)
    char m_acFileName[NI_MAX_PATH];

    // The set of names available for reset
    NiTObjectArray<NiFixedString> m_kStateNames;

    //
    // Things for the picking mechanism
    //
    NiCursor* m_pkCursor;
    bool m_bSelecting;
    
    //
    // Things for force application
    //
    bool m_bApplyingForce;
    NxActor* m_pkForceActor;
    NxCloth* m_pkForceCloth;
    NxVec3 m_kForcePt;
    float m_fForceMass;
    unsigned int m_uiForceVertex;
    unsigned int m_uiMouseX;
    unsigned int m_uiMouseY;
    unsigned int m_uiOriginX;
    unsigned int m_uiOriginY;

    //
    // Things for the menu system
    //
    NiTexturePtr m_spFontTexture;
    unsigned int m_uiFontHeight;
    unsigned int m_uiFontWidth;
    unsigned int m_uiFontColumns;

    NiMenu* m_pkMenu;

    NiMenu::Entry* m_pkCameraEntry;
    NiMenu::Entry* m_pkSorterNoneEntry;
    NiMenu::Entry* m_pkSorterAlphaEntry;
    NiMenu::Entry* m_pkFreezeEntry;
    NiMenu::Entry* m_pkStepLengthEntry;
    NiMenu::Entry* m_pkDisableKeyEntry;
    NiMenu::Entry* m_pkAnimateSpeedEntry;
    NiMenu::Entry* m_pkRotateSpeedEntry;
    NiMenu::Entry* m_pkTranslateSpeedEntry;
    NiMenu::Entry* m_pkDebugScaleEntry;
    NiMenu::Entry* m_pkForceExponentEntry;
    NiMenu::Entry* m_pkGroundHeightEntry;
    NiMenu::Entry* m_pkPhysicsResetEntry;

    NiStats* m_pkStats;
    
    // Scene parameters
    NiPoint3 m_kGravity;
    bool m_bDebugOutput;
    
    // PhysX Visual Remote Debugger
    bool m_bUsingVRD;
};

#endif // PHYSXNIFVIEWER_H
