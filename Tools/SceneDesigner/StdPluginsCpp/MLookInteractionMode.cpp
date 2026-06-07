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

#include "MLookInteractionMode.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MLookInteractionMode::MLookInteractionMode()
{
    m_bDragging = false;
}
//---------------------------------------------------------------------------
void MLookInteractionMode::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);
}
//---------------------------------------------------------------------------
String* MLookInteractionMode::get_Name()
{
    return "GamebryoLook";
}
//---------------------------------------------------------------------------
void MLookInteractionMode::SetInteractionMode(Object*, 
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
void MLookInteractionMode::ValidateInteractionMode(Object*,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
    pmState->Enabled = MFramework::Instance->ViewportManager->ActiveViewport
        ->CameraEntity->Writable;
}
//---------------------------------------------------------------------------
void MLookInteractionMode::MouseDown(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if (eType == MouseButtonType::LeftButton)
    {
        NiCamera* pkCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->GetNiCamera();
        if (!pkCamera->GetViewFrustum().m_bOrtho)
        {
            m_bDragging = true;
            m_bLeftDown = true;
            MEntity* pmCamera = MFramework::Instance->ViewportManager->
                ActiveViewport->CameraEntity;
            if (pmCamera != NULL)
            {
                NiEntityInterface* pkCamera;
                pkCamera = pmCamera->GetNiEntityInterface();
                pkCamera->GetPropertyData(*m_pkRotationName, 
                    *m_pkStartRotation);
                pkCamera->GetPropertyData(*m_pkTranslationName,
                    *m_pkStartPosition);
            }
            __super::MouseDown(MouseButtonType::RightButton, iX, iY);
        }
    }
    else
    {
        if ((m_bDragging) && (eType == MouseButtonType::RightButton))
        {
            // if the user right-click cancels, return reset cam state
            MEntity* pmCamera = MFramework::Instance->ViewportManager->
                ActiveViewport->CameraEntity;
            if (pmCamera != NULL)
            {
                NiEntityInterface* pkCamera;
                pkCamera = pmCamera->GetNiEntityInterface();
                pkCamera->SetPropertyData(*m_pkRotationName, 
                    *m_pkStartRotation);
                pkCamera->SetPropertyData(*m_pkTranslationName,
                    *m_pkStartPosition);
            }
            m_bDragging = false;
            m_bLeftDown = false;
            m_bRightDown = false;
            m_bMiddleDown = false;
        }
        else if (eType == MouseButtonType::RightButton)
        {
            // right-clicking will exit pan mode
            m_bDragging = false;
            m_bLeftDown = false;
            m_bRightDown = false;
            m_bMiddleDown = false;
            InteractionModeService->ActiveMode = m_pmPreviousIM;
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
                pkCamera->GetPropertyData(*m_pkTranslationName,
                    *m_pkStartPosition);
            }
            m_bDragging = true;
            // if another button was clicked, just let the super
            // class deal with it - probably viewport movement
            __super::MouseDown(eType, iX, iY);
        }
    }
}
//---------------------------------------------------------------------------
void MLookInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if (eType == MouseButtonType::LeftButton)
    {
        m_bDragging = false;
        m_bLeftDown = false;
        __super::MouseUp(MouseButtonType::RightButton, iX, iY);
    }
    else
    {
        m_bDragging = false;
        // make sure that super class handles its mouseup
        __super::MouseUp(eType, iX, iY);
    }
}
//---------------------------------------------------------------------------
