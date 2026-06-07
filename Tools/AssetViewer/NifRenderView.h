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

// NifRenderView.h

#ifndef NIFRENDERVIEW_H
#define NIFRENDERVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ID_CAMERA_BEGIN  50000
#define ID_CAMERA_END    50500

#include "NiDebugGeometryClick.h"
#include "NifCameraList.h"
#include "NifRenderViewUIState.h"
#include <Ni2DString.h>
#include <Ni2DStringRenderClick.h>
#include <NiFont.h>
#include <NiMeshCullingProcess.h>
#include <NiMesh2DRenderView.h>

class CNifDoc;
class CNifRenderViewUIManager;

class CNifRenderView : public CView
{
protected: // create from serialization only
    CNifRenderView();
    DECLARE_DYNCREATE(CNifRenderView)

// Attributes
public:
    CNifDoc* GetDocument();

    CNifRenderViewUIManager* GetUIManager();


// Operations
public:
    enum PersistentUIMode 
    {   
        CAMERA_ROTATE = 0, 
        CAMERA_TRANSLATE, 
        CAMERA_TUMBLE,
        CAMERA_ZOOM,
        OBJECT_SELECTION,
        COLLISION_LAB_TRANSLATE,
        CAMERA_LOCKED,
        NUM_UIMODES
    };
    void SetCurrentUIMode(PersistentUIMode eMode, bool bForceUpdate = false);
    PersistentUIMode GetCurrentUIMode() const;

    void SetCurrentAxisMode(CNifRenderViewUIState::UIAxisConstraint eMode);
    CNifRenderViewUIState::UIAxisConstraint GetCurrentAxisMode();

    void ReCreateRenderer();
    
    void CreateRenderer ();
    void DestroyRenderer ();
    void CreateFrame ();
    void ClearClickSwap (NiCamera* pkCamera = NULL);

    void OnIdle();

    bool ReInitializeScene();
    NiPoint2 GetScreenBounds();

    NiCameraInfoIndex GetCurrentCameraIndices();
    void SetCurrentCameraIndices(NiCameraInfoIndex kIndex);
    bool IsCurrentCameraDefault();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNifRenderView)
    public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual BOOL DestroyWindow();
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, 
        AFX_CMDHANDLERINFO* pHandlerInfo);
    protected:
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CNifRenderView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    NiRenderer* GetRenderer();
protected:
    PersistentUIMode m_eCurrentUIMode;
    
    CMenu* m_pkCameraMenu;
    HMENU m_hMenu;
    unsigned int m_uiMaxCameras;
    unsigned int m_uiCurrentCamMenuIdx;
    void BuildCameraMenu();
    void AttachCameraMenu();
    void EmptyCameraMenu();
    int GetMenuIndexForCamera(NiCameraInfoIndex kIndex);

    static UINT ThreadFunc (LPVOID pParam);
    void UpdateScreenConsole();
    void CreateScreenConsole();

    HANDLE m_hThread;
    HWND m_hTopLevelHwnd;
    bool m_bContinue;
    bool m_bSceneChanged;
    bool m_bNeedRecreateRenderer;
    bool m_bOverdraw;
    bool m_bWireframe;
    bool m_bOrbitCamera;
    NiRendererPtr m_spRenderer;
    bool m_bRendererCreationFailed;
    bool m_bNewDocument;

    // frame rendering objects
    NiRenderFramePtr m_spFrame;
    NiCullingProcessPtr m_spCuller;
    Ni3DRenderViewPtr m_spSceneView;
    NiViewRenderClickPtr m_spSceneRenderClick;
    NiAlphaSortProcessorPtr m_spAlphaProcessor;
    NiMesh2DRenderViewPtr m_spElementView;
    Ni2DStringRenderClickPtr m_spLabelRenderClick;
    NiDebugGeometryClickPtr m_spDebugGeometryClick;
    NiDefaultClickRenderStepPtr m_spRenderStep;
    NiFixedString m_kShadowRenderStepName;
    NiDefaultClickRenderStepPtr m_spShadowRenderStep;
    void Precache(NiAVObject* pkObject);

    static bool ShadowRenderStepPre(NiRenderStep* pkCurrentStep, 
        void* pvCallbackData);

    NiVisibleArray m_kVisible;
    NiMeshCullingProcess m_kCuller;

    NiPoint2 m_kScreenBounds;
    NiCameraPtr m_spCamera;
    NiCameraInfoIndex m_kCameraIndex;
    Ni2DStringPtr m_spCameraLabel;
    NiFontPtr m_spFont;

    bool m_bBackgroundColorChanged;
    CCriticalSection m_kRendererCS;
    CNifRenderViewUIManager* m_pkUIManager;

    unsigned int m_uiSleepTime;

    float m_fLastTime;

    HMODULE m_hNiCgShaderLib;

// Generated message map functions
protected:
    //{{AFX_MSG(CNifRenderView)
    afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSysKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
    afx_msg void OnSysKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
    afx_msg void OnDestroy();
    afx_msg void OnCameraMenuSelection(UINT nID);
    afx_msg void OnCameraFocus();
    afx_msg void OnCameraNext();
    afx_msg void OnCameraPrevious();
    afx_msg void OnCameraRotate();
    afx_msg void OnCameraTranslate();
    afx_msg void OnCameraTumble();
    afx_msg void OnCollisionLabTranslate();
    afx_msg void OnZoomExtentsAll();
    afx_msg void OnCameraReset();
    afx_msg void OnUpdateCameraRotate(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCameraTranslate(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCameraTumble(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCollisionLabTranslate(CCmdUI* pCmdUI);
    afx_msg void OnOverdraw();
    afx_msg void OnUpdateOverdraw(CCmdUI* pCmdUI);
    afx_msg void OnWireframe();
    afx_msg void OnUpdateWireframe(CCmdUI* pCmdUI);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnCameraZoom();
    afx_msg void OnUpdateCameraZoom(CCmdUI* pCmdUI);
    afx_msg void OnXaxisConstraint();
    afx_msg void OnUpdateXaxisConstraint(CCmdUI* pCmdUI);
    afx_msg void OnXzaxisConstraint();
    afx_msg void OnUpdateXzaxisConstraint(CCmdUI* pCmdUI);
    afx_msg void OnYaxisConstraint();
    afx_msg void OnUpdateYaxisConstraint(CCmdUI* pCmdUI);
    afx_msg void OnZaxisConstraint();
    afx_msg void OnUpdateZaxisConstraint(CCmdUI* pCmdUI);
    afx_msg void OnXyaxisConstraint();
    afx_msg void OnUpdateXyaxisConstraint(CCmdUI* pCmdUI);
    afx_msg void OnYzaxisConstraint();
    afx_msg void OnUpdateYzaxisConstraint(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCameraNext(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCameraPrevious(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCameraReset(CCmdUI* pCmdUI);
    afx_msg void OnUpdateZoomExtentsAll(CCmdUI* pCmdUI);
    afx_msg void OnObjectSelection();
    afx_msg void OnUpdateObjectSelection(CCmdUI* pCmdUI);
    afx_msg void OnLoddownButton();
    afx_msg void OnUpdateLodoverrideButton(CCmdUI* pCmdUI);
    afx_msg void OnLodoverrideButton();
    afx_msg void OnLodupButton();
    afx_msg void OnUpdateLoddownButton(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLodupButton(CCmdUI* pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnReloadShaders();
};

#ifndef _DEBUG  // debug version in NifRenderView.cpp
inline CNifDoc* CNifRenderView::GetDocument()
   { return (CNifDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIFRENDERVIEW_H
