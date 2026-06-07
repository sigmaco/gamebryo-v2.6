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

// NifRenderViewCameraTranslationState.h

#include "stdafx.h"
#include "NifRenderViewCollisionLabTranslationState.h"
#include "NifRenderView.h"
#include "NifDoc.h"

//---------------------------------------------------------------------------
CNifRenderViewCollisionLabTranslationState::
    CNifRenderViewCollisionLabTranslationState(
    CNifRenderView* pkView) : CNifRenderViewBufferedUIState(pkView)
{
    m_pkCameraInfo = NULL;
    m_bTrackingMouse = false;
    m_fSceneScale = 0.0f;
}
//---------------------------------------------------------------------------
CNifRenderViewCollisionLabTranslationState::
    ~CNifRenderViewCollisionLabTranslationState()
{
    m_pkCameraInfo = NULL;
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::Update()
{
    const float fTolPercent = 0.001f;

    CNifDoc* pkDoc = m_pkView->GetDocument();
    pkDoc->Lock();   

    m_kDeltaTranslate = NiPoint3::ZERO;
    CNifRenderViewBufferedUIState::Update();

    NiCollisionLabInfo* pkLabInfo = &pkDoc->m_kCollisionLabInfo;
    pkLabInfo->SetUpdate(true); // Let's always update

    if (pkDoc->m_pkCollisionLabGrp && pkLabInfo)
    {
        pkLabInfo->HideWireFrames();
        pkLabInfo->ClearIntersectData();
        pkDoc->m_pkCollisionLabGrp->FindCollisions(m_fDeltaTime);       
        pkLabInfo->CreateCollisionTriangles(true); 
    }

    if(m_pkCameraInfo && m_bActive)
    {     
        NiNode* pkCollider = 
            (NiNode*)pkLabInfo->FindObjectByName(STR_COLLISION_COLLIDER);

        if (pkCollider == NULL)
        {
            pkDoc->UnLock();
            return;
        }

        NiCollisionLabInfo* pkLabInfo = &pkDoc->m_kCollisionLabInfo;

        if (pkLabInfo->GetDynamicSim())
        {
            NiMatrix3 kRot = pkCollider->GetRotate();
            NiPoint3 kVelocity = pkLabInfo->GetDirection();
            NiPoint3 kTranslate =  pkCollider->GetTranslate();
            NiPoint3 kDelta = kRot * kVelocity; 

            if (pkLabInfo->GetCollisionState())
            {
                pkLabInfo->SetDynamicSim(false);
                pkLabInfo->SetRemainingDistance(0.0f);
              
                float fIntersectionTime = pkLabInfo->GetIntersectionTime();

                // ensure time isn't too small
                if (fIntersectionTime > TIME_TOLERANCE)
                {
                    float fRadius = pkLabInfo->GetSmallestObjRadius();
                    float fPercentOfDist = fRadius * fTolPercent;

                    kDelta = kDelta * fIntersectionTime;
                    float fDeltaLength = kDelta.Length();

                    // check to ensure that the length we must travel
                    // is greater than our smallest unit of update
                    if (fDeltaLength > fPercentOfDist)
                    {
                        // only travel as far as to our distance percentage
                        float fEpsilonMult = 
                            (fDeltaLength - fPercentOfDist) / fDeltaLength;

                        NiPoint3 kDeltaFraction = kDelta;
                        kDeltaFraction *= fEpsilonMult;

                        // if kDelta is very small, then we take precaution
                        // because we have been dealing with small numbers
                        // and precision could be suspect.
                        kTranslate += kDeltaFraction;
                        pkCollider->SetTranslate(kTranslate);
                    }
                }
                      
                // Set the new collision velocity.
                NiCollisionData::SetEnableVelocity(false);
                NiCollisionTraversals::ClearWorldVelocities(pkCollider, 
                    NiPoint3(0.0f,0.0f,0.0f));
                NiCollisionTraversals::UpdateWorldData(pkCollider);

                pkLabInfo->CreateCollisionIndicator();
            }
            else
            {               
                // factor in time unit....
                pkLabInfo->SetRemainingDistance(
                    pkLabInfo->GetRemainingDistance() - 
                    (NiPoint3::VectorLength(
                    pkLabInfo->GetDirection()) * m_fDeltaTime));

                kDelta = kDelta * m_fDeltaTime;

                float fRemainingDist = pkLabInfo->GetRemainingDistance();
                float fDeltaLength = kDelta.Length();

                float fRadius = pkLabInfo->GetSmallestObjRadius();
                float fPercentOfDist = fRadius * fTolPercent;

                // Ensure that we aren't in a situation where we are
                // just moving into a possible "just" contact situation.
                if (fRemainingDist > fDeltaLength &&
                   (fRemainingDist - fDeltaLength > fPercentOfDist))
                {
                    // if kDelta is very small, then we take precaution
                    // because we have been dealing with small numbers
                    // and precision could be suspect.
                    kTranslate += kDelta;
                    pkCollider->SetTranslate(kTranslate);
                }
           
                // Set the new collision velocity.
                NiCollisionData* pkCD = NiGetCollisionData(pkCollider);

                NIASSERT(pkCD);

                if (pkCD)
                    pkCD->SetLocalVelocity(kVelocity);

                if (pkLabInfo->GetRemainingDistance() < 0.0f)
                {
                    NiCollisionData::SetEnableVelocity(false);
                    pkLabInfo->SetDynamicSim(false);

                    NiCollisionTraversals::ClearWorldVelocities(pkCollider, 
                        NiPoint3(0.0f,0.0f,0.0f));
                    NiCollisionTraversals::UpdateWorldData(pkCollider);
                }
            }
        }
        else
        {
            NiMatrix3 kRot = pkCollider->GetRotate();
            NiPoint3 kTranslate =  pkCollider->GetTranslate();
            kTranslate += kRot * m_kDeltaTranslate;
            pkCollider->SetTranslate(kTranslate);
        }
    }
    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::Initialize()
{ 
    CNifDoc* pkDoc = m_pkView->GetDocument();
    pkDoc->Lock();

    CNifRenderViewBufferedUIState::Initialize();

    NiCameraInfo* pkInfo = 
        pkDoc->GetCameraInfo(m_pkView->GetCurrentCameraIndices());

    if(pkInfo && !pkInfo->m_bIsAnimated && pkInfo->m_spCam != NULL)
    {
        m_pkCameraInfo = pkInfo;
        m_bActive = m_bActive && true;
    }
    else
        m_bActive = false;

    NiNode* pkScene = pkDoc->GetSceneGraph();

    if(pkScene)
       m_fSceneScale = pkScene->GetWorldBound().GetRadius();
    else
       m_bActive = false;

    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::OnUpdate(UINT Msg)
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
bool CNifRenderViewCollisionLabTranslationState::CanExecuteCommand(
    char* pcCommandID)
{ 
    return true;
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::ResetCollisionState()
{
    CNifDoc* pkDoc = m_pkView->GetDocument();
    pkDoc->Lock();

    NiCollisionLabInfo* pkLabInfo = &pkDoc->m_kCollisionLabInfo;

    pkLabInfo->ResetCollisionIndicator(false);
    pkLabInfo->ResetCollisionIndicator(true);

    // Clear... remove the graphical intersection points
    NiCollisionData::SetEnableVelocity(false);
    pkLabInfo->SetDynamicSim(false);

    NiNode* pkCollidee = 
        (NiNode*)pkLabInfo->FindObjectByName(STR_COLLISION_COLLIDEE);

    NiNode* pkCollider = 
        (NiNode*)pkLabInfo->FindObjectByName(STR_COLLISION_COLLIDER);

    if (pkCollider)
        NiCollisionTraversals::ClearWorldVelocities(pkCollider, 
        NiPoint3(0.0f,0.0f,0.0f));


    NiCollisionTraversals::UpdateWorldData(pkCollider);

    pkLabInfo->SetCollisionState(false);
    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::UpdateDevices()
{ 
    CNifRenderViewBufferedUIState::UpdateDevices();

    if (!m_pkKeyboard)
        return;

    if (!m_pkCameraInfo || !m_pkCameraInfo->m_spCam)
        return;

    CNifDoc* pkDoc = m_pkView->GetDocument();
    pkDoc->Lock();

    if (pkDoc->m_pkCollisionLabGrp == NULL)
    {
        pkDoc->UnLock();
        return;
    }

    bool bTestKey = m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_T);
    bool bClearKey = m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_C);
    bool bDynamicKey = m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_SPACE);

    bool bRightAxisKey = m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_X);
    bool bUpAxisKey = m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_SHIFT);
    bool bForwardAxisKey = m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_Z);

    static float fCurrentTime = 0.0f;
    static float fLastTime = 0.0f;
    static float fSimTime = 0.0f;

    fCurrentTime = NiGetCurrentTimeInSec();

    fSimTime += (fCurrentTime - fLastTime);

    if (bTestKey)
    {
        if (pkDoc->m_pkCollisionLabGrp->TestCollisions(m_fDeltaTime))
        {
            ::MessageBox(NULL,
                "Report indicates COLLISION.", 
                "Test Collision State", MB_OK |
                MB_ICONEXCLAMATION);
        }
        else
        {
            ::MessageBox(NULL,
                "Report indicates NO Collision.", 
                "Test Collision State", MB_OK |
                MB_ICONEXCLAMATION);
        }

    }

    if (bClearKey)
    {
        ResetCollisionState();
        fSimTime = 0.0f;
    }
    else if (bDynamicKey && fSimTime > 2.0)        
    {
        fLastTime = fCurrentTime;

        NiCollisionLabInfo* pkLabInfo = &pkDoc->m_kCollisionLabInfo;

        if (pkLabInfo->GetDynamicSim()==false)
        {
            pkLabInfo->ResetCollisionIndicator(true);

            // Clear and send...
            // [1] Remove intersections graphics
            // [2] Calc. direction from collider to collidee...
            // [3] Calc. distance to travel
            NiNode* pkCollidee =
                (NiNode*)pkLabInfo->FindObjectByName(STR_COLLISION_COLLIDEE);

            NiNode* pkCollider =
                (NiNode*)pkLabInfo->FindObjectByName(STR_COLLISION_COLLIDER);

            if (pkCollidee && pkCollider)
            {
                NiCollisionData::SetEnableVelocity(true);
                pkLabInfo->SetDynamicSim(true);

                NiPoint3 kDir = 
                    pkCollidee->GetTranslate() - pkCollider->GetTranslate();
            
                // Traverse a distance of the length between the objects plus
                // their radius.
                pkLabInfo->SetRemainingDistance( 
                    NiPoint3::VectorLength(kDir) + 
                    (pkCollider->GetWorldBound().GetRadius() +
                     pkCollidee->GetWorldBound().GetRadius()));

                NiPoint3::UnitizeVector(kDir);
                      
                pkLabInfo->SetDirection((kDir * pkLabInfo->GetVelocity()));

                pkLabInfo->SetCollisionState(false);

                NiCollisionData* pkCD = NiGetCollisionData(pkCollider);
                pkCD->SetLocalVelocity(pkLabInfo->GetDirection());

                NiCollisionTraversals::UpdateWorldData(pkCollider);
            }
        }
        else
        {
            // Stop the simulation... because the key was pressed again
            // during simulation.

            fLastTime = fCurrentTime;

            pkLabInfo->ResetCollisionIndicator(true);
            // simulation in process...
            NiCollisionData::SetEnableVelocity(false);
            pkLabInfo->SetDynamicSim(false);
            pkLabInfo->SetRemainingDistance( 0.00f );
            pkLabInfo->SetDirection( NiPoint3(0.0f,0.0f,0.0f) );
            pkLabInfo->SetCollisionState(false);

            NiNode* pkCollider =
                (NiNode*)pkLabInfo->FindObjectByName(STR_COLLISION_COLLIDER);
            if (pkCollider)
                NiCollisionTraversals::ClearWorldVelocities(pkCollider, 
                    NiPoint3(0.0f,0.0f,0.0f));

            ResetCollisionState();
        }

        fSimTime = 0.0f;
    }
    else if(bRightAxisKey && bUpAxisKey)
    {
        m_eCurrentAxisConstraint = RIGHT_UP_AXIS;
    }
    else if(bUpAxisKey && bForwardAxisKey)
    {
        m_eCurrentAxisConstraint = FORWARD_UP_AXIS;
    }
    else if(bRightAxisKey && bForwardAxisKey)
    {
        m_eCurrentAxisConstraint = FORWARD_RIGHT_AXIS;
    }
    else if(bRightAxisKey)
    {
        m_eCurrentAxisConstraint = RIGHT_AXIS;
    }
    else if(bUpAxisKey)
    {
        m_eCurrentAxisConstraint = UP_AXIS;
    }
    else if (bForwardAxisKey)
    {
        m_eCurrentAxisConstraint = FORWARD_AXIS;
    }
    else
    {
        m_eCurrentAxisConstraint = m_eLastSetAxisConstraint;
    }
 
    if(m_bTrackingMouse)
    {
        const NiPoint3 kRight =
            -m_pkCameraInfo->m_spCam->GetWorldRightVector();
        
        const NiPoint3 kForward = 
            m_pkCameraInfo->m_spCam->GetWorldDirection();
        
        const NiPoint3 kUp =
            -m_pkCameraInfo->m_spCam->GetWorldUpVector();

        CPoint kDelta = m_kLastPoint - m_kBeginPoint;
        kDelta = -kDelta;
        NiPoint2 kBounds = m_pkView->GetScreenBounds();

        // We're using MAX's coordinate system here, not NI
        //       Y                     Y
        // NI:   |__ Z         MAX:    |__ X
        //      /                     /
        //    -X                     Z
        //
        if(m_eCurrentAxisConstraint == FORWARD_AXIS)
        {
            m_kDeltaTranslate += kForward * ((float)kDelta.y) / 
                kBounds.y * m_fSceneScale * 2.0f;
        }
        else if(m_eCurrentAxisConstraint == UP_AXIS)
        {
            kDelta.y = -kDelta.y;
            m_kDeltaTranslate += kUp * ((float)kDelta.y) /  
                kBounds.y * m_fSceneScale * 2.0f;
        }
        else if(m_eCurrentAxisConstraint == RIGHT_AXIS)
        {
            m_kDeltaTranslate += kRight * ((float)kDelta.x) / 
                kBounds.x * m_fSceneScale * 2.0f;
        }
        else if(m_eCurrentAxisConstraint == RIGHT_UP_AXIS)
        {
            kDelta.y = -kDelta.y;
            m_kDeltaTranslate += kRight * ((float)kDelta.x) / 
                kBounds.x  * m_fSceneScale * 2.0f;
            m_kDeltaTranslate += kUp * ((float)kDelta.y) / 
                kBounds.y * m_fSceneScale * 2.0f;
        }
        else if(m_eCurrentAxisConstraint == FORWARD_UP_AXIS)
        {
            kDelta.y = -kDelta.y;
            m_kDeltaTranslate += kForward * ((float)kDelta.x) / 
                kBounds.x  * m_fSceneScale * 2.0f;
            m_kDeltaTranslate += kUp * ((float)kDelta.y) / 
                kBounds.y * m_fSceneScale * 2.0f;
        }
        else if(m_eCurrentAxisConstraint == FORWARD_RIGHT_AXIS)
        {
            kDelta.y = -kDelta.y;
            m_kDeltaTranslate += kRight * ((float)kDelta.x) / 
                kBounds.x  * m_fSceneScale * 2.0f;
            m_kDeltaTranslate += kForward * ((float)kDelta.y) / 
                kBounds.y * m_fSceneScale * 2.0f;
        }
        m_kBeginPoint = m_kLastPoint;
    }

    if(m_fWheelDelta != 0.0f)
    {
        const NiPoint3 kForward = 
            m_pkCameraInfo->m_spCam->GetWorldDirection();

        m_kDeltaTranslate += kForward * m_fWheelDelta * 0.1f *
            m_fSceneScale;
    }
    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::OnForwardKey(
    NiDevice::DeviceState eState)
{
   const NiPoint3 kForward = m_pkCameraInfo->m_spCam->GetWorldUpVector();
 
    m_kDeltaTranslate += kForward * m_kTranslateSpeed.z * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::OnBackwardKey(
    NiDevice::DeviceState eState)
{
    const NiPoint3 kForward = m_pkCameraInfo->m_spCam->GetWorldUpVector();
 
    m_kDeltaTranslate -= kForward * m_kTranslateSpeed.z * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::OnLeftKey(
    NiDevice::DeviceState eState)
{
    const NiPoint3 kRight = m_pkCameraInfo->m_spCam->GetWorldRightVector();

    m_kDeltaTranslate -= kRight * m_kTranslateSpeed.x * 
        m_fSceneScale * m_fDeltaTime;   
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::OnRightKey(
    NiDevice::DeviceState eState)
{
    const NiPoint3 kRight = m_pkCameraInfo->m_spCam->GetWorldRightVector();

    m_kDeltaTranslate += kRight * m_kTranslateSpeed.x * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::OnUpKey(
    NiDevice::DeviceState eState)
{
    const NiPoint3 kUp = m_pkCameraInfo->m_spCam->GetWorldDirection();

    m_kDeltaTranslate += kUp * m_kTranslateSpeed.y * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::OnDownKey(
    NiDevice::DeviceState eState)
{
    const NiPoint3 kUp = m_pkCameraInfo->m_spCam->GetWorldDirection();

    m_kDeltaTranslate -= kUp * m_kTranslateSpeed.y * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::OnMouseMove(UINT nFlags,
    CPoint point)
{
    if(m_bTrackingMouse)
    {        
        m_kLastPoint = point;
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::OnLButtonUp(UINT nFlags,
    CPoint point)
{
    ::ReleaseCapture();
    this->m_bTrackingMouse = false;
    m_kLastPoint = point;
}
//---------------------------------------------------------------------------
void CNifRenderViewCollisionLabTranslationState::OnLButtonDown(UINT nFlags,
    CPoint point)
{
    m_pkView->SetCapture();
    m_bTrackingMouse = true;
    m_kLastPoint = point;
    m_kBeginPoint = point;
}
//---------------------------------------------------------------------------
bool 
CNifRenderViewCollisionLabTranslationState::
IsAxisConstraintAllowed(UIAxisConstraint eMode)
{
    return true;
}
//---------------------------------------------------------------------------
