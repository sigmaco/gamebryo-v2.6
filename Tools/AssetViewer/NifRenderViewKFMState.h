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

// NifRenderViewKFMState.h

#ifndef NIFRENDERVIEWKFMSTATE_H
#define NIFRENDERVIEWKFMSTATE_H

#include "NifRenderViewUIState.h"
#include "NifRenderView.h"

class CNifDoc;

class CNifRenderViewKFMState : public CNifRenderViewUIState  
{
public:
    static NiNode* GetKFMRoot(CNifDoc* pkDocument);

    CNifRenderViewKFMState(CNifRenderView* pkView, NiNode* pkRoot,
        bool bOrbitCamera);
    virtual ~CNifRenderViewKFMState();

    bool GetInitialized() const {return m_bInitialized;}
    void SetOrbitCamera(bool bOrbitCamera);
    void DisplayKeystrokesDialog();

    virtual bool CanExecuteCommand(char* pcCommandID);
    virtual void Update();
    virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual void OnUpdate(UINT Msg);
    virtual void OnLButtonDown(UINT nFlags, CPoint point);
    virtual void OnLButtonUp(UINT nFlags, CPoint point);
    virtual void OnMouseMove(UINT nFlags, CPoint point);
    virtual void OnKillFocus(CWnd* pNewWnd);

protected:
    void CalcCameraFrustum();
    void CalcInitialCameraTransform();

    bool m_bInitialized;
    bool m_bDialogActive;

    float m_fTransEndTime;
    CList<unsigned int, unsigned int> m_kModifierList;
    CList<unsigned int, unsigned int> m_kKeystrokeList;
    NiActorManager* m_pkCurManager;
    EventMap* m_pkCurEventMap;

    bool m_bLButtonDown;
    bool m_bRButtonDown;
    CPoint m_ptClickPoint;

    NiAVObject* m_pkModel;
    NiNode* m_pkAccumRoot;
    NiCamera* m_pkCamera;
    NiNodePtr m_spCamZRot;
    NiNodePtr m_spCamXRot;
    NiNodePtr m_spCameraNode;
    bool m_bOrbitCamera;
    NiCameraInfoIndex m_kPrevIndex;
    NiCameraInfoIndex m_kCameraIndex;

    CNifRenderView::PersistentUIMode m_ePrevUIMode;
};

#endif  // #ifndef NIFRENDERVIEWKFMSTATE_H
