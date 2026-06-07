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

#include "MPanInteractionMode.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MPanInteractionMode::MPanInteractionMode()
{
    m_bDragging = false;
}
//---------------------------------------------------------------------------
void MPanInteractionMode::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);
}
//---------------------------------------------------------------------------
String* MPanInteractionMode::get_Name()
{
    return "GamebryoPan";
}
//---------------------------------------------------------------------------
void MPanInteractionMode::SetInteractionMode(Object*, 
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
void MPanInteractionMode::ValidateInteractionMode(Object*,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
    pmState->Enabled = MFramework::Instance->ViewportManager->ActiveViewport
        ->CameraEntity->Writable;
}
//---------------------------------------------------------------------------
void MPanInteractionMode::MouseDown(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if (eType == MouseButtonType::LeftButton)
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
        }
        __super::MouseDown(MouseButtonType::MiddleButton, iX, iY);
    }
    else
    {
        if ((m_bDragging) && (eType == MouseButtonType::RightButton))
        {
            // if the user right-click cancels, return the cam to previous pos
            MEntity* pmCamera = MFramework::Instance->ViewportManager->
                ActiveViewport->CameraEntity;
            if (pmCamera != NULL)
            {
                NiEntityInterface* pkCamera;
                pkCamera = pmCamera->GetNiEntityInterface();
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
            }
            m_bDragging = true;
            // if another button was clicked, just let the super
            // class deal with it - probably viewport movement
            __super::MouseDown(eType, iX, iY);
        }
    }
}
//---------------------------------------------------------------------------
void MPanInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if (eType == MouseButtonType::LeftButton)
    {
        m_bDragging = false;
        m_bLeftDown = false;
        __super::MouseUp(MouseButtonType::MiddleButton, iX, iY);
    }
    else
    {
        // make sure that super class handles its mouseup
        __super::MouseUp(eType, iX, iY);
    }
}
//---------------------------------------------------------------------------
