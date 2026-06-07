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

#include "stdafx.h"
#include "MUIPhysicsForceState.h"
#include "MCamera.h"
#include "MFramework.h"
#include "MPhysics.h"

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIPhysicsForceState::MUIPhysicsForceState()
{
    m_bProcessMouseMove = false;
    m_eForceConstraint = PARALLEL_SCREEN;
}
//---------------------------------------------------------------------------
MUIState::UIType MUIPhysicsForceState::GetUIType()
{
    return MUIState::PHYSICSFORCE;
}
//---------------------------------------------------------------------------
String* MUIPhysicsForceState::GetName()
{
    return "Physics Force";
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::MouseEnter()
{
    MUIState::MouseEnter();
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::MouseLeave()
{
    MUIState::MouseLeave();
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::MouseHover()
{
    MUIState::MouseHover();
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::DoubleClick()
{
    MUIState::DoubleClick();
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::MouseMove(int x, int y)
{
    MUIState::MouseMove(x,y);
    if (m_bProcessMouseMove)
    {
        POINT point;
        point.x = x;
        point.y = y;

        GetPositionInScreenSpace(point);

        MRenderer* pkRenderer = MFramework::Instance->Renderer;
        MCamera* pkCamera = pkRenderer->ActiveCamera;

        // Compute the magnitude. This will be scaled by other factors when
        // the force is stored.
        float fDx = (float)(point.x - m_ptClickPoint.x);
        float fDy = (float)(m_ptClickPoint.y - point.y);
        float fMagnitude = NiSqrt(fDx * fDx + fDy * fDy);
        
        if (fDx == 0.0f && fDy == 0.0f)
            return;
        
        // Figure the direction
        NiPoint3 kForce;
        if (m_eForceConstraint == PARALLEL_SCREEN)
        {
            NiPoint3 kUp = pkCamera->GetCamera()->GetWorldUpVector();
            NiPoint3 kRight = pkCamera->GetCamera()->GetWorldRightVector();
            kForce = kRight * fDx + kUp * fDy;
            NiPoint3::UnitizeVector(kForce);
            kForce *= fMagnitude;
        }
        else
        {
            NiPoint3 kDir = pkCamera->GetCamera()->GetWorldDirection();
            kForce = kDir * fDy;
        }
        
        MPhysics* pkPhysics = MFramework::Instance->Physics;
        pkPhysics->AddForce(MFramework::Instance->Clock->CurrentTime, kForce);
    }
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::MouseUp(MouseButtonType eType, int x, int y)
{
    MUIState::MouseUp(eType, x, y);
    if ((eType == LMB && m_eForceConstraint == PARALLEL_SCREEN) ||
        (eType == MMB && m_eForceConstraint == PERPENDICULAR_SCREEN))
    {
        MPhysics* pkPhysics = MFramework::Instance->Physics;
        pkPhysics->FinishForce();
        m_bProcessMouseMove = false;
        ReleaseCapture();
    }
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::MouseDown(MouseButtonType eType, int x, int y)
{
    MUIState::MouseDown(eType, x, y);

    if (eType == LMB || eType == MMB)
    {
        POINT point;
        point.x = x;
        point.y = y;

        GetPositionInScreenSpace(point);
        m_ptClickPoint.x = point.x;
        m_ptClickPoint.y = point.y;

        // Clear existing forces
        MPhysics* pkPhysics = MFramework::Instance->Physics;

        // Need the camera
        MCamera* pkMCamera = MFramework::Instance->Renderer->ActiveCamera;
        NIASSERT(pkMCamera != 0);
        
        // x,y is in client form space, supposely, so this should do the job
        NiPoint3 kOrigin;
        NiPoint3 kDir;
        if (pkMCamera->WindowPointToRay(x, y, kOrigin, kDir))
        {
            if (pkPhysics->InitiateForces(kOrigin, kDir))
            {
                m_bProcessMouseMove = true;
                if (eType == LMB)
                    m_eForceConstraint = PARALLEL_SCREEN;
                else
                    m_eForceConstraint = PERPENDICULAR_SCREEN;
                
                MRenderer* pkRenderer = MFramework::Instance->Renderer;
                SetCapture(pkRenderer->GetRenderWindowHandle()); 
            }
        }
    }
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::MouseWheel(int iDelta)
{
    WheelZoom(iDelta);
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::Update()
{
    MUIState::Update();
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::RefreshData()
{
    MUIState::RefreshData();
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::DeleteContents()
{
    MUIState::DeleteContents();
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::Activate(bool bFlushPrevious)
{
    MUIState::Activate(bFlushPrevious);
}
//---------------------------------------------------------------------------
void MUIPhysicsForceState::Deactivate()
{
    MUIState::Deactivate();
}
//---------------------------------------------------------------------------
