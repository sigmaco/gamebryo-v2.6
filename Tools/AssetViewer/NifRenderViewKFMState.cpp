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

// NifRenderViewKFMState.cpp

#include "stdafx.h"
#include "SceneViewer.h"
#include "NifRenderViewKFMState.h"
#include "NifDoc.h"
#include "NifUserPreferences.h"
#include "ChooseKFMDlg.h"
#include "KeystrokeBindingsDlg.h"
#include "NifTimeManager.h"

//---------------------------------------------------------------------------
bool CNifRenderViewKFMState::CanExecuteCommand(char* pcCommandID)
{
    CString strCommandID = pcCommandID;
    if (strCommandID == "CameraNext" ||
        strCommandID == "CameraPrevious" ||
        strCommandID == "CameraReset" ||
        strCommandID == "CameraZoomExtents")
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiNode* CNifRenderViewKFMState::GetKFMRoot(CNifDoc* pkDocument)
{
    NiNode* pkRoot = NULL;

    pkDocument->Lock();

    NiTSet<NiNode*> kKFMRoots;
    for (unsigned int ui = 0; ui < pkDocument->GetNumberOfRoots(); ui++)
    {
        NiNode* pkRoot = pkDocument->GetRoot(ui);
        if (pkDocument->GetKFMInfo(pkRoot))
        {
            kKFMRoots.Add(pkRoot);
        }
    }

    pkDocument->UnLock();

    assert(kKFMRoots.GetSize() > 0);
    if (kKFMRoots.GetSize() == 1)
    {
        pkRoot = kKFMRoots.GetAt(0);
    }
    else
    {
        CChooseKFMDlg dlg;
        dlg.m_pkKFMRoots = &kKFMRoots;
        if (dlg.DoModal() == IDOK)
        {
            pkRoot = dlg.m_pkRoot;
        }
    }

    return pkRoot;
}
//---------------------------------------------------------------------------
CNifRenderViewKFMState::CNifRenderViewKFMState(CNifRenderView* pkView,
    NiNode* pkRoot, bool bOrbitCamera) : CNifRenderViewUIState(pkView)
{
    m_bInitialized = false;
    m_bDialogActive = false;
    m_pkCurManager = NULL;
    m_pkCurEventMap = NULL;
    m_bLButtonDown = false;
    m_bRButtonDown = false;
    m_pkModel = NULL;
    m_pkAccumRoot = NULL;
    m_pkCamera = NULL;
    m_spCamZRot = NULL;
    m_spCamXRot = NULL;
    m_spCameraNode = NULL;

    m_bOrbitCamera = bOrbitCamera;

    if (pkRoot)
    {
        m_pkView->GetDocument()->Lock();

        // Get KFM info.
        CNifDoc::KFMInfo* pkInfo =
            m_pkView->GetDocument()->GetKFMInfo(pkRoot);
        assert(pkInfo);
        m_pkCurManager = pkInfo->m_pkManager;
        m_pkCurEventMap = pkInfo->m_pkEventMap;

        // Ensure that event map exists.
        if (!m_pkCurEventMap)
        {
            ::MessageBox(NULL, "No keystroke mapping file was found for "
                "this KFM file.\nThis MAP file must be present in order "
                "to control the\ncharacter.", "No MAP File", MB_OK |
                MB_ICONERROR);
            m_pkCurManager = NULL;
            m_pkView->GetDocument()->UnLock();
            return;
        }

        // Ensure that idle animation exists.
        unsigned int uiIdleEC;
        bool bResult = m_pkCurEventMap->GetEventCode(uiIdleEC,
            EventMap::INPUT_CODE_NONE, EventMap::INPUT_CODE_NONE);
        if (!bResult)
        {
            ::MessageBox(NULL, "No idle animation was found.\n\nExactly one "
                "animation must have no keystroke or modifier bound\nto it in "
                "order to control the character. Make the necessary\nchanges "
                "in the Gamebryo Animation Tool and try again.", "No Idle "
                "Animation", MB_OK | MB_ICONERROR);
            m_pkCurManager = NULL;
            m_pkCurEventMap = NULL;
            m_pkView->GetDocument()->UnLock();
            return;
        }

        // Initialize keystroke and modifier list.
        m_kModifierList.AddTail(EventMap::INPUT_CODE_NONE);
        m_kKeystrokeList.AddTail(EventMap::INPUT_CODE_NONE);

        // Create camera and place in scene.
        m_pkCamera = new NiCamera;
        m_pkCamera->SetName("KFM Camera");
        m_spCamZRot = new NiNode;
        m_spCamZRot->SetName("KFM ZRot Turret Node");
        m_spCamZRot->SetSelectiveUpdate(true);
        m_spCamZRot->SetSelectiveUpdateTransforms(true);
        m_spCamZRot->SetSelectiveUpdatePropertyControllers(true);
        m_spCamZRot->SetSelectiveUpdateRigid(false);
        m_spCamXRot = new NiNode;
        m_spCamXRot->SetName("KFM XRot Turret Node");
        m_spCamXRot->SetSelectiveUpdate(true);
        m_spCamXRot->SetSelectiveUpdateTransforms(true);
        m_spCamXRot->SetSelectiveUpdatePropertyControllers(true);
        m_spCamXRot->SetSelectiveUpdateRigid(false);
        m_spCameraNode = new NiNode;
        m_spCameraNode->SetName("KFM Camera Node");
        m_spCameraNode->SetSelectiveUpdate(true);
        m_spCameraNode->SetSelectiveUpdateTransforms(true);
        m_spCameraNode->SetSelectiveUpdatePropertyControllers(true);
        m_spCameraNode->SetSelectiveUpdateRigid(false);
        m_spCamZRot->AttachChild(m_spCamXRot);
        m_spCamXRot->AttachChild(m_spCameraNode);

        m_pkModel = m_pkCurManager->GetNIFRoot();
        m_pkAccumRoot = NiDynamicCast(NiNode,
            m_pkCurManager->GetAccumRoot());
        assert(m_pkAccumRoot);
        m_pkAccumRoot->AttachChild(m_spCamZRot);

        CalcCameraFrustum();
        CalcInitialCameraTransform();

        // Add camera to camera list and set as current camera.
        m_kCameraIndex = m_pkView->GetDocument()->GetCameraList()
            ->AddCamera(0, m_pkCamera);
        assert(m_kCameraIndex.uiCameraIndex != (unsigned int) -1 &&
            m_kCameraIndex.uiRootIndex != (unsigned int) -1);
        m_pkCamera = m_pkView->GetDocument()->GetCameraList()
            ->GetCamera(m_kCameraIndex);
        m_kPrevIndex = m_pkView->GetCurrentCameraIndices();
        m_pkView->SetCurrentCameraIndices(m_kCameraIndex);

        m_spCamZRot->Update(0.0f);
        SetOrbitCamera(m_bOrbitCamera);

        // Get previous UI Mode.
        m_ePrevUIMode = m_pkView->GetCurrentUIMode();

        // Turn animation looping off and start time running.
        m_pkView->GetDocument()->GetTimeManager()->SetTimeMode(
            CNifTimeManager::CONTINUOUS);
        m_pkView->GetDocument()->GetTimeManager()->Enable();

        m_bInitialized = true;

        MainThreadUpdateAllViews(MAKELPARAM(NIF_UPDATECAMERASMENU,0));
        m_pkView->GetDocument()->UnLock();
    }
}
//---------------------------------------------------------------------------
CNifRenderViewKFMState::~CNifRenderViewKFMState()
{
    if (m_bInitialized)
    {
        // Activate idle sequence.
        m_pkCurManager->Reset();

        // Clear keystroke and modifier lists.
        m_kModifierList.RemoveAll();
        m_kKeystrokeList.RemoveAll();

        // Delete camera and nodes.
        m_pkView->GetDocument()->Lock();
        m_pkView->SetCurrentCameraIndices(m_kPrevIndex);
        m_pkView->GetDocument()->GetCameraList()->RemoveCamera(0, m_pkCamera);
        MainThreadUpdateAllViews(MAKELPARAM(NIF_UPDATECAMERASMENU,0));
        m_pkView->GetDocument()->UnLock();
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::SetOrbitCamera(bool bOrbitCamera)
{
    m_bOrbitCamera = bOrbitCamera;
    if (m_bOrbitCamera)
    {
        // Clear camera transforms.
        m_pkCamera->SetTranslate(NiPoint3::ZERO);
        m_pkCamera->SetRotate(NiMatrix3::IDENTITY);

        // Attach camera to camera node.
        m_spCameraNode->AttachChild(m_pkCamera);
    }
    else
    {
        // Propagate world transforms to camera.
        m_pkCamera->SetTranslate(m_spCameraNode->GetWorldTranslate());
        m_pkCamera->SetRotate(m_spCameraNode->GetWorldRotate());

        // Detach camera from camera node.
        m_spCameraNode->DetachChild(m_pkCamera);
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::DisplayKeystrokesDialog()
{
    CKeystrokeBindingsDlg dlg;
    dlg.m_pkEventMap = m_pkCurEventMap;
    dlg.m_pkKFM = m_pkCurManager->GetKFMTool();
    m_bDialogActive = true;
    dlg.DoModal();
    m_bDialogActive = false;
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::CalcCameraFrustum()
{
    float fSceneRadius = m_pkModel->GetWorldBound().GetRadius();

    float fRight, fTop;

    fRight = fTop = 1.0f / NiSqrt(3.0f);

    RECT rect;
    m_pkView->GetClientRect(&rect);
    if (rect.right > rect.bottom)
        fRight *= (float) rect.right / (float) rect.bottom;
    else
        fTop *= (float) rect.bottom / (float) rect.right;


    NiCamera::Frustum fr;
    fr.m_fLeft = -fRight;
    fr.m_fRight = fRight;
    fr.m_fTop = fTop;
    fr.m_fBottom = -fTop;
    fr.m_fNear = 0.01f * fSceneRadius;
    fr.m_fFar = 200.0f * fSceneRadius; 
 
    m_pkCamera->SetViewFrustum(fr);
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::CalcInitialCameraTransform()
{
    // Get up axis.
    CNifUserPreferences::Lock();
    NiPoint3 kUpAxis = CNifUserPreferences::AccessUserPreferences()
        ->GetUpAxis();
    CNifUserPreferences::UnLock();

    float fSceneRadius = m_pkModel->GetWorldBound().GetRadius();

    NiPoint3 kCameraNodeTrans;
    NiMatrix3 kCameraNodeRot;
    if (kUpAxis == NiPoint3::UNIT_Y)
    {
        // Rotate and position the camera so that we are looking down the
        // negative world z-axis and y is up.
        kCameraNodeTrans = NiPoint3(0.0f, 0.0f, 2.0f * fSceneRadius);

        kCameraNodeRot = NiMatrix3(
            NiPoint3(0.0f, 0.0f, -1.0f),
            NiPoint3(0.0f, 1.0f, 0.0f),
            NiPoint3(1.0f, 0.0f, 0.0f));
    }
    else    // if (kUpAxis == NiPoint3::UNIT_Z)
    {
        // Rotate and position the camera so that we are looking down the
        // world y-axis and z is up.
        kCameraNodeTrans = NiPoint3(0.0f, -2.0f * fSceneRadius, 0.0f);

        kCameraNodeRot = NiMatrix3(
            NiPoint3(0.0f, 1.0f, 0.0f),
            NiPoint3(0.0f, 0.0f, 1.0f),
            NiPoint3(1.0f, 0.0f, 0.0f));
    }
    m_spCameraNode->SetRotate(kCameraNodeRot);
    m_spCameraNode->SetTranslate(kCameraNodeTrans);

    NiPoint3 kSceneCenter = m_pkModel->GetWorldBound().GetCenter();
    NiPoint3 kCharacterCenter = m_pkAccumRoot->GetWorldTranslate();
    NiPoint3 kCamZNodeTrans = kSceneCenter - kCharacterCenter;
    m_spCamZRot->SetTranslate(kCamZNodeTrans);
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::Update()
{
    if (!m_bInitialized)
    {
        return;
    }

    float fCurTime = m_pkView->GetDocument()->GetTimeManager()
        ->GetCurrentTime();

    // Get event code from keystroke and modifier.
    unsigned int uiDesEC;
    bool bResult = m_pkCurEventMap->GetEventCode(uiDesEC,
        m_kKeystrokeList.GetTail(), m_kModifierList.GetTail());
    if (bResult && m_pkCurManager->GetTargetAnimation() != uiDesEC)
    {
        m_pkCurManager->SetTargetAnimation(uiDesEC);
    }

    m_pkCurManager->Update(fCurTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (!m_bInitialized)
    {
        return;
    }

    POSITION pos = m_kModifierList.Find(nChar);
    if (pos)
    {
        m_kModifierList.RemoveAt(pos);
    }
    else
    {
        pos = m_kKeystrokeList.Find(nChar);
        if (pos)
        {
            m_kKeystrokeList.RemoveAt(pos);
        }
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (!m_bInitialized)
    {
        return;
    }

    // Don't process if not the first WM_KEYDOWN event.
    if (!(nFlags & 0x4000))
    {
        if (m_pkCurEventMap->IsModifier(nChar))
        {
            m_kModifierList.AddTail(nChar);
        }
        else
        {
            m_kKeystrokeList.AddTail(nChar);
        }
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::OnUpdate(UINT Msg)
{
    switch (Msg)
    {
        case NIF_DESTROYSCENE:
            m_bInitialized = false;
            break;
        default:
            break;
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_bInitialized)
    {
        return;
    }

    m_bLButtonDown = true;
    m_pkView->ClientToScreen(&point);
    m_ptClickPoint = point;

    m_pkView->SetCapture();
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (!m_bInitialized)
    {
        return;
    }

    m_bLButtonDown = false;

    ::ReleaseCapture();
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_bInitialized)
    {
        return;
    }

    if (m_bLButtonDown)
    {
        m_pkView->ClientToScreen(&point);

        // Get screen metrics for scaling.
        CClientDC dc(m_pkView);
        int iHorzRes = dc.GetDeviceCaps(HORZRES);
        int iVertRes = dc.GetDeviceCaps(VERTRES);

        // Get scene radius.
        m_pkView->GetDocument()->Lock();
        float fSceneRadius = m_pkModel->GetWorldBound().GetRadius();
        m_pkView->GetDocument()->UnLock();

        // Get up axis.
        CNifUserPreferences::Lock();
        NiPoint3 kUpAxis = CNifUserPreferences::AccessUserPreferences()
            ->GetUpAxis();
        CNifUserPreferences::UnLock();

        if (m_bOrbitCamera)
        {
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
            {
                // Get vertical distance moved.
                float fVert = (float) (m_ptClickPoint.y - point.y);

                // Find translation amount.
                float fTrans = (fVert / iVertRes) * 5.0f * fSceneRadius;
                NiPoint3 kTrans = m_spCameraNode->GetTranslate();
                NiPoint3 kOffset;
                if (kUpAxis == NiPoint3::UNIT_Y)
                {
                    kOffset = NiPoint3(0.0f, 0.0f, -fTrans);
                    kTrans = kTrans + NiPoint3(0.0f, 0.0f, -fTrans);
                }
                else    // if (kUpAxis == NiPoint3::UNIT_Z)
                {
                    kOffset = NiPoint3(0.0f, fTrans, 0.0f);
                    kTrans = kTrans + NiPoint3(0.0f, fTrans, 0.0f);
                }
                kTrans += kOffset;
                m_spCameraNode->SetTranslate(kTrans);
            }
            else
            {
                // Make up rotation.
                float fHorz = (float) (point.x - m_ptClickPoint.x);
                float fUpRot = (fHorz / iHorzRes) * 2.0f * NI_PI;
                NiMatrix3 kUpRot;
                if (kUpAxis == NiPoint3::UNIT_Y)
                {
                    kUpRot.MakeYRotation(fUpRot);
                }
                else    // if (kUpAxis == NiPoint3::UNIT_Z)
                {
                    kUpRot.MakeZRotation(fUpRot);
                }

                // Make X rotation.
                float fVert = (float) (point.y - m_ptClickPoint.y);
                float fXRot = (fVert / iVertRes) * 2.0f * NI_PI;
                NiMatrix3 kXRot;
                kXRot.MakeXRotation(fXRot);
            
                // Apply rotations.
                if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
                {
                    NiMatrix3 kCamZRot = m_spCamZRot->GetRotate();
                    kCamZRot = kUpRot * kCamZRot;
                    m_spCamZRot->SetRotate(kCamZRot);
                }
                else if (GetAsyncKeyState(VK_MENU) & 0x8000)
                {
                    NiMatrix3 kCamXRot = m_spCamXRot->GetRotate();
                    kCamXRot = kXRot * kCamXRot;
                    m_spCamXRot->SetRotate(kCamXRot);
                }
                else
                {
                    NiMatrix3 kCamZRot = m_spCamZRot->GetRotate();
                    kCamZRot = kUpRot * kCamZRot;
                    m_spCamZRot->SetRotate(kCamZRot);

                    NiMatrix3 kCamXRot = m_spCamXRot->GetRotate();
                    kCamXRot = kXRot * kCamXRot;
                    m_spCamXRot->SetRotate(kCamXRot);
                }
            }
        }
        else
        {
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
            {
                // Get vert translation.
                float fVert = (float) (m_ptClickPoint.y - point.y);
                float fVertTrans = (fVert / iVertRes) * 5.0f *
                    fSceneRadius;

                NiPoint3 kTrans = m_pkCamera->GetTranslate();
                kTrans = kTrans + fVertTrans *
                    (m_pkCamera->GetRotate() * NiPoint3::UNIT_X);
                m_pkCamera->SetTranslate(kTrans);
            }
            else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
            {
                // Make up rotation.
                float fHorz = (float) (point.x - m_ptClickPoint.x);
                float fUpRot = (fHorz / iHorzRes) * 2.0f * NI_PI;
                NiMatrix3 kUpRot;
                if (kUpAxis == NiPoint3::UNIT_Y)
                {
                    kUpRot.MakeYRotation(fUpRot);
                }
                else    // if (kUpAxis == NiPoint3::UNIT_Z)
                {
                    kUpRot.MakeZRotation(fUpRot);
                }

                NiMatrix3 kRot = m_pkCamera->GetRotate();
                kRot = kUpRot * kRot;
                m_pkCamera->SetRotate(kRot);
            }
            else if (GetAsyncKeyState(VK_MENU) & 0x8000)
            {
                // Make right rotation.
                float fVert = (float) (point.y - m_ptClickPoint.y);
                float fRightRot = (fVert / iVertRes) * 2.0f * NI_PI;

                NiMatrix3 kRightRot;
                kRightRot.MakeRotation(fRightRot,
                    m_pkCamera->GetRotate() * NiPoint3::UNIT_Z);

                NiMatrix3 kRot = m_pkCamera->GetRotate();
                kRot = kRightRot * kRot;
                m_pkCamera->SetRotate(kRot);
            }
            else
            {
                // Get horz translation.
                float fHorz = (float) (m_ptClickPoint.x - point.x);
                float fHorzTrans = (fHorz / iHorzRes) * 5.0f *
                    fSceneRadius;

                // Get vert translation.
                float fVert = (float) (point.y - m_ptClickPoint.y);
                float fVertTrans = (fVert / iVertRes) * 5.0f *
                    fSceneRadius;

                NiPoint3 kTrans = m_pkCamera->GetTranslate();
                kTrans = kTrans +
                    fVertTrans *
                        (m_pkCamera->GetRotate() * NiPoint3::UNIT_Y) +
                    fHorzTrans *
                        (m_pkCamera->GetRotate() * NiPoint3::UNIT_Z);
                m_pkCamera->SetTranslate(kTrans);
            }
        }

        m_ptClickPoint = point;
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewKFMState::OnKillFocus(CWnd* pNewWnd)
{
    if (!m_bDialogActive)
    {
        // Leave KFM interaction mode.
        m_pkView->SetCurrentUIMode(m_ePrevUIMode);
    }
}
//---------------------------------------------------------------------------
