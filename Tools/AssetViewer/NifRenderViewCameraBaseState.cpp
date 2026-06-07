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


#include "StdAfx.h"
#include "NifRenderViewCameraBaseState.h"
#include "NifRenderView.h"
#include "NifDoc.h"
#include "NiSmoothCameraController.h"
#include "NifUserPreferences.h"



//---------------------------------------------------------------------------
float CNifRenderViewCameraBaseState::ms_fWheelZoomSpeed = 0.1f;
float CNifRenderViewCameraBaseState::ms_fZoomSpeed = 10.0f;
float CNifRenderViewCameraBaseState::ms_fRotateSpeed = 7.0f;

float CNifRenderViewCameraBaseState::ms_fCameraSmoothMoveTime = 0.3f;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
CNifRenderViewCameraBaseState::CNifRenderViewCameraBaseState(
    CNifRenderView* pkView) : CNifRenderViewBufferedUIState(pkView)
{
    m_bTrackingMouse = false;
    m_fFocusRadius = 0.0f;
    m_pkCameraInfo = NULL;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::Initialize()
{ 
    m_kDeltaTrans = NiPoint3::ZERO;
    m_kFocusPoint = NiPoint3::ZERO;
    m_fFocusRadius = 0.0f;
    m_kDeltaRot = NiPoint3::ZERO;
    m_fDeltaZoom = 0.0f;
    m_spTransformController = NULL;
    m_kBaseRot.MakeIdentity();

    CNifRenderViewBufferedUIState::Initialize();

    CNifRenderView* pkView = m_pkView;

    CNifDoc* pkDoc = pkView->GetDocument();
    NIASSERT(pkDoc);
    pkDoc->Lock();

    NiCameraInfo* kInfo = 
        pkDoc->GetCameraInfo(pkView->GetCurrentCameraIndices());
    if (kInfo && !kInfo->m_bIsAnimated && kInfo->m_spCam != NULL)
    {
        m_pkCameraInfo = kInfo;
    }
    else
    {
        m_bActive = false;
    }

    NiNode* pkScene = pkDoc->GetSceneGraph();
    if (pkScene)
    {
        m_fFocusRadius = pkScene->GetWorldBound().GetRadius();
        // Double radius because we need to see scene not to touch it
        m_fFocusRadius *= 2.0f;
    }
    else
    {
        m_bActive = false;
    }

    pkDoc->UnLock();

    ComputeFocus();
    UpdateBases();
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::ApplyTranslation(
    NiPoint3& kCameraPos,
    NiMatrix3&,
    NiPoint3& kFocusPos,
    const NiPoint3& kTrans) const
{
    kCameraPos += kTrans;
    kFocusPos += kTrans;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::ApplyZoom(
    NiPoint3& kCameraPos,
    NiMatrix3&,
    NiPoint3& kFocusPos,
    const float fZoom) const
{
    NiPoint3 kViewDir = kFocusPos - kCameraPos;
    float fDist = kViewDir.Length();
    if (fDist > 0.0f)
    {
        kViewDir /= fDist;
        float fNewDist = NiMax(1.0f, fDist + fZoom);
        kCameraPos = kFocusPos - kViewDir * fNewDist;
    }
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::ApplyRotation(
    NiPoint3& kCameraPos,
    NiMatrix3& kCameraRot,
    NiPoint3& kFocusPos,
    const NiPoint3& kRotation) const
{
    // Local translation from focus point (in world space)
    NiPoint3 kFocusLocalPos = kFocusPos - kCameraPos;

    // Horizontal Rotation
    NiMatrix3 kHRotate;
    kHRotate.MakeZRotation(kRotation.z);

    // Vertical Rotation
    NiPoint3 kRightVec;
    kCameraRot.GetCol(2, kRightVec);

    NiMatrix3 kVRotate;
    kVRotate.MakeRotation(kRotation.y, kRightVec);

    // Transforming our pos and rot
    kFocusLocalPos = kHRotate * kVRotate * kFocusLocalPos;
    kCameraRot = kHRotate * kVRotate * kCameraRot;

    // Restoring camera position
    kCameraPos = kFocusPos - kFocusLocalPos;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::Update()
{
    // If camera is in interpolation mode we need to skip user's motion
    if (UpdateTransformController() == true)
        return;

    if (m_pkCameraInfo && m_bActive && m_pkCameraInfo->m_bIsDefault)
    {
        NiMatrix3 kRotate = m_kBaseRot;
        NiPoint3 kTrans = m_kBasePos;
        NiPoint3 kFocusPoint = m_kBaseFocus;

        ApplyTranslation(
            kTrans,
            kRotate,
            kFocusPoint,
            m_kDeltaTrans);

        ApplyZoom(
            kTrans,
            kRotate,
            kFocusPoint,
            m_fDeltaZoom);

        ApplyRotation(
            kTrans,
            kRotate,
            kFocusPoint,
            m_kDeltaRot);

        m_kFocusPoint = kFocusPoint;
        m_pkCameraInfo->m_spCam->SetTranslate(kTrans);
        m_pkCameraInfo->m_spCam->SetRotate(kRotate);
        m_pkCameraInfo->m_spCam->Update(0.0f);

        NiMesh::CompleteSceneModifiers(m_pkCameraInfo->m_spCam);
    }

    // This routine will call UpdateDevices() which can change variables.
    // So we should always call it AFTER we do all our processing
    // to make all new in the next update
    CNifRenderViewBufferedUIState::Update();
}

//---------------------------------------------------------------------------
bool CNifRenderViewCameraBaseState::UpdateTransformController()
{
    if (m_spTransformController)
    {
        m_spTransformController->Update(0.033f);
        if (m_spTransformController->IsDone())
        {
            m_spTransformController = NULL;

            // Recomputing base
            UpdateBases();
        }

        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::OnUpdate(UINT Msg)
{ 
    switch (Msg)
    {
        case NIF_DESTROYSCENE:
            m_bActive = false;
        case NIF_CURRENTCAMERACHANGED:
        case NIF_USERPREFERENCESCHANGED:
        case NIF_CREATESCENE:
        case NIF_ADDNIF:
        case NIF_REMOVENIF:
        case NIF_SCENECHANGED:
            m_bReInitialize = true;
            break;
        default:
            break;
    }
}

//---------------------------------------------------------------------------
bool CNifRenderViewCameraBaseState::CanExecuteCommand(char*)
{ 
    return true;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::UpdateFocus()
{
    CNifDoc* pkDoc = m_pkView->GetDocument();
    NIASSERT(pkDoc);

    // Set FocusPoint to selection center and FocusRadius to selection radius.
    // If failed -- stay at current value.
    if (pkDoc->GetSelectionParams(m_kFocusPoint, m_fFocusRadius))
    {
        // Double radius because we need to see an object not to touch it
        m_fFocusRadius *= 2.0f;

        // Directing camera to FocusPoint
        if (m_pkCameraInfo)
        {
            // Creating controller
            m_spTransformController = NiNew CNiSmoothCameraController(
                m_pkCameraInfo->m_spCam,
                m_kFocusPoint,
                m_fFocusRadius,
                ms_fCameraSmoothMoveTime);
        }
    }
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::ComputeFocus()
{
    CNifDoc* pkDoc = m_pkView->GetDocument();
    NIASSERT(pkDoc);

    // Set FocusPoint to selection center and FocusRadius to selection radius.
    // If failed -- stay at current value.
    if (pkDoc->GetSelectionParams(m_kFocusPoint, m_fFocusRadius))
    {
        // Double radius because we need to see an object not to touch it
        m_fFocusRadius *= 2.0f;
    }

    if (m_pkCameraInfo)
    {
        NiPoint3 kCameraDir = m_pkCameraInfo->m_spCam->GetWorldDirection();
        NiPoint3 kCameraPos = m_pkCameraInfo->m_spCam->GetWorldLocation();

        float fDist = (kCameraPos - m_kFocusPoint).Length();

        // Using desired focus to compute current
        m_kFocusPoint =  kCameraPos + kCameraDir * fDist;
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::UpdateBases()
{
    if (m_pkCameraInfo)
    {
        NiPoint3 kCameraDir = m_pkCameraInfo->m_spCam->GetWorldDirection();

        // Transformation deltas and bases
        m_fDeltaZoom = 0.0f;
        m_kDeltaTrans = NiPoint3::ZERO;
        m_kDeltaRot =  NiPoint3::ZERO;
        
        m_kBasePos = m_pkCameraInfo->m_spCam->GetTranslate();
        m_kBaseFocus = m_kFocusPoint;

        // Restoring rotation base with strict Up vector
        NiPoint3 kUpVec = NiPoint3::UNIT_Z;
        NiPoint3 kRightVec = kCameraDir.Cross(kUpVec);
        kRightVec.Unitize();
        NiPoint3 kDirVec = kUpVec.Cross(kRightVec);

        m_kBaseRot.SetCol(0, kDirVec);
        m_kBaseRot.SetCol(1, kUpVec);
        m_kBaseRot.SetCol(2, kRightVec);

        // Current rotate delta from base
        float fDot = kCameraDir.Dot(kDirVec);
        float fVAngle = NiACos(fDot);
        m_kDeltaRot = NiPoint3(
            0.0f,
            kCameraDir.z < kDirVec.z ? fVAngle : -fVAngle,
            0.0f);

        // Fixing base pos: it should be on the plane
        float fFocusDist = (m_kBaseFocus - m_kBasePos).Length();
        m_kBasePos = m_kBaseFocus  - kDirVec * fFocusDist;
    }
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::HandlePan(
    const CPoint& kDelta,
    const NiPoint2& kScreenBounds,
    float fPanCoef)
{
    NiPoint3 kTransVector = NiPoint3::ZERO;

    if (kScreenBounds.y > 0)
    {
        kTransVector += 
            NiPoint3::UNIT_Y *
            ((float)kDelta.y) /
            kScreenBounds.y *
            m_fFocusRadius *
            fPanCoef;
    }

    if (kScreenBounds.x > 0)
    {
        kTransVector += 
            NiPoint3::UNIT_Z *
            (-(float)kDelta.x) /
            kScreenBounds.x *
            m_fFocusRadius *
            fPanCoef;
    }

    // Using trans in world space
    const NiMatrix3& kCurRot = m_pkCameraInfo->m_spCam->GetRotate();
    kTransVector = kCurRot * kTransVector;

    m_kDeltaTrans += kTransVector;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::HandleZoom(
    const CPoint& kDelta,
    const NiPoint2& kScreenBounds)
{
    // Using only Y
    float fDeltaLength = 0.0f;
    if (kScreenBounds.y > 0)
        fDeltaLength = ((float)kDelta.y) / kScreenBounds.y;

    float fZoomInc = fDeltaLength * ms_fZoomSpeed * m_fFocusRadius;
    m_fDeltaZoom += fZoomInc;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::HandleWheelZoom(float fWheelDelta)
{
    float fZoomInc = -fWheelDelta * ms_fWheelZoomSpeed * m_fFocusRadius;
    m_fDeltaZoom += fZoomInc;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::HandleRotate(
    const CPoint& kDelta,
    const NiPoint2& kScreenBounds)
{
    float fXRot = 0.0f;
    if (kScreenBounds.x > 0)
        fXRot = ms_fRotateSpeed * ((float)kDelta.x) / kScreenBounds.x;
    float fYRot = 0.0f;
    if (kScreenBounds.y > 0)
        fYRot = ms_fRotateSpeed * ((float)kDelta.y) / kScreenBounds.y;

    m_kDeltaRot += NiPoint3(0.0f, fYRot, fXRot);
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::StartMouseTracking(
    CPoint kCurrMousePoint)
{
    if (!m_bTrackingMouse)
    {
        m_pkView->SetCapture();
        m_bTrackingMouse = true;
        m_kLastPoint = kCurrMousePoint;
        m_kBeginPoint = kCurrMousePoint;
    }
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::StopMouseTracking(
    CPoint kCurrMousePoint)
{
    if (m_bTrackingMouse)
    {
        ::ReleaseCapture();
        m_bTrackingMouse = false;
        m_kLastPoint = kCurrMousePoint;
    }
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::UpdateMouseTracking(
    CPoint kCurrMousePoint)
{
    if (m_bTrackingMouse)
    {        
        m_kLastPoint = kCurrMousePoint;
    }
}

//---------------------------------------------------------------------------
bool CNifRenderViewCameraBaseState::IsAxisConstraintAllowed(UIAxisConstraint)
{
    return false;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraBaseState::PerformPick(CPoint point)
{
    CNifRenderView* pkView = m_pkView;
    if (!pkView)
        return;

    CNifDoc* pkDoc = pkView->GetDocument();
    NIASSERT(pkDoc);

    pkDoc->Lock();

    pkView->ScreenToClient(&point);

    NiCamera* pkCamera = pkDoc->GetCameraInfo(
        pkView->GetCurrentCameraIndices())->m_spCam;

    NiPoint3 kOrigin, kDir;
    pkCamera->WindowPointToRay(point.x, point.y, kOrigin, kDir);

    NiPick kPick;
    kPick.SetTarget(pkDoc->GetSceneGraph());
    kPick.SetQueryType(NiPick::QUERY_ALL);
    kPick.SetSortType(NiPick::SORT_ON);
    kPick.SetPickObjectPolicy(NiNew SimpleSkinPickObjectPolicy());
    kPick.SetIntersectType(NiPick::INTERSECT_COLLISION_DATA);
    kPick.SetFallbackType(NiPick::FALLBACK_TRIANGLE);

    bool bHighlightedObject = false;
    bool bHit = kPick.PickObjects(kOrigin, kDir);
    if (bHit)
    {
        NiPick::Results& kPickResults = kPick.GetResults();

        for (unsigned int ui = 0; ui < kPickResults.GetSize(); ui++)
        {
            NiPick::Record* pkPickRecord = kPickResults.GetAt(ui);
            if (pkPickRecord)
            {
                NiAVObject* pkObject = pkPickRecord->GetAVObject();
                NiNode* pkParent = pkObject->GetParent();
                if (pkParent && pkParent->GetName() &&
                    g_strBoundShapeRootName != (const char*)
                    pkParent->GetName())
                {
                    pkDoc->HighlightObject(pkObject);
                    bHighlightedObject = true;
                    break;
                }
            }
        }
    }

    if (!bHighlightedObject)
    {
        pkDoc->HighlightObject(NULL);
    }

    pkDoc->UnLock();
}