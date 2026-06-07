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

// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MZoomInteractionMode.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

#pragma warning(push)
#pragma warning(disable: 4756)

//---------------------------------------------------------------------------
MZoomInteractionMode::MZoomInteractionMode() :
    m_fLastTime(-FLT_MAX), m_bDragging(false)
{
}
//---------------------------------------------------------------------------
void MZoomInteractionMode::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);
}
//---------------------------------------------------------------------------
String* MZoomInteractionMode::get_Name()
{
    return "GamebryoZoom";
}
//---------------------------------------------------------------------------
void MZoomInteractionMode::SetInteractionMode(Object*, 
    EventArgs*)
{
    MVerifyValidInstance;

    if (InteractionModeService->ActiveMode->GetType()->IsSubclassOf( 
        __typeof(MExitableInteractionMode)))
    {
        MExitableInteractionMode* pmViewMode;
        pmViewMode = static_cast<MExitableInteractionMode*>(
            InteractionModeService->ActiveMode);
        m_pmPreviousIM = pmViewMode->GetPreviousIM();
    }
    else
    {
        m_pmPreviousIM = InteractionModeService->ActiveMode;
    }
    InteractionModeService->ActiveMode = this;
}
//---------------------------------------------------------------------------
void MZoomInteractionMode::ValidateInteractionMode(Object*,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
    pmState->Enabled = MFramework::Instance->ViewportManager->ActiveViewport
        ->CameraEntity->Writable;
}
//---------------------------------------------------------------------------
void MZoomInteractionMode::MouseDown(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if (eType == MouseButtonType::LeftButton)
    {
        m_bDragging = true;
        m_bLeftDown = true;
        m_iLastY = iY;
        m_fLastTime = -FLT_MAX;
        MEntity* pmCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->CameraEntity;
        if (pmCamera != NULL)
        {
            NiEntityInterface* pkCamera;
            pkCamera = pmCamera->GetNiEntityInterface();
            pkCamera->GetPropertyData(*m_pkTranslationName, 
                *m_pkStartPosition);
            float fOrthoWidth;
            pkCamera->GetPropertyData(*m_pkOrthoWidthName, fOrthoWidth);
            m_fStartOrthoWidth = fOrthoWidth;
        }
    }
    else
    {
        if ((m_bDragging) && (eType == MouseButtonType::RightButton))
        {
            // if the user right-click cancels, return the cam's state
            MEntity* pmCamera = MFramework::Instance->ViewportManager->
                ActiveViewport->CameraEntity;
            if (pmCamera != NULL)
            {
                NiEntityInterface* pkCamera;
                pkCamera = pmCamera->GetNiEntityInterface();
                pkCamera->SetPropertyData(*m_pkTranslationName, 
                    *m_pkStartPosition);
                pkCamera->SetPropertyData(*m_pkOrthoWidthName, 
                    m_fStartOrthoWidth);
            }
            m_bDragging = false;
            m_bLeftDown = false;
            m_bRightDown = false;
            m_bMiddleDown = false;
        }
        else if (eType == MouseButtonType::RightButton)
        {
            // right-clicking will exit pan mode
            InteractionModeService->ActiveMode = m_pmPreviousIM;
            m_bDragging = false;
            m_bLeftDown = false;
            m_bRightDown = false;
            m_bMiddleDown = false;
        }
        else if (eType == MouseButtonType::MiddleButton)
        {
            MEntity* pmCamera = MFramework::Instance->ViewportManager->
                ActiveViewport->CameraEntity;
            if (pmCamera != NULL)
            {
                NiEntityInterface* pkCamera;
                pkCamera = pmCamera->GetNiEntityInterface();
                pkCamera->GetPropertyData(*m_pkRotationName,
                    *m_pkStartRotation);
                float fOrthoWidth;
                pkCamera->GetPropertyData(*m_pkOrthoWidthName, 
                    fOrthoWidth);
                m_fStartOrthoWidth = fOrthoWidth;
            }
            m_bDragging = true;
            // if another button was clicked, just let the super
            // class deal with it - probably viewport movement
            __super::MouseDown(eType, iX, iY);
        }
    }
}
//---------------------------------------------------------------------------
void MZoomInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if (eType == MouseButtonType::LeftButton)
    {
        MEntity* pmCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->CameraEntity;
        if (pmCamera != NULL)
        {
            NiEntityInterface* pkCamera;
            NiPoint3 kTranslation;
            NiPoint3 kDeltaTranslation;
            pkCamera = pmCamera->GetNiEntityInterface();
            pkCamera->GetPropertyData(*m_pkTranslationName, kTranslation);

            kDeltaTranslation = *m_pkStartPosition - kTranslation;
            if ((kDeltaTranslation.x > 0.01f) || (kDeltaTranslation.x < -0.01f)
                || (kDeltaTranslation.y > 0.01f) || 
                (kDeltaTranslation.y < -0.01f) || (kDeltaTranslation.z > 0.01f)
                || (kDeltaTranslation.z < -0.01f))
            {
                CommitZoom();
            }
        }
        m_bDragging = false;
        m_bLeftDown = false;
    }
    else
    {
        // make sure that super class handles its mouse up
        __super::MouseUp(eType, iX, iY);
    }
}
//---------------------------------------------------------------------------
void MZoomInteractionMode::MouseMove(int iX, int iY)
{
    MVerifyValidInstance;

    if ((m_bDragging) && (m_bLeftDown))
    {
        float fCurrentTime = MFramework::Instance->TimeManager->ContinuousTime;
        if (m_fLastTime == -FLT_MAX)
        {
            m_fLastTime = fCurrentTime;
        }

        if (m_fLastTime != fCurrentTime)
        {
            int iDY = m_iLastY - iY;
            m_iLastY = iY;
            __super::MouseWheel(iDY);
            m_fLastTime = fCurrentTime;
        }
    }
    else
    {
        __super::MouseMove(iX, iY);
    }
}
#pragma warning(pop)
//---------------------------------------------------------------------------
