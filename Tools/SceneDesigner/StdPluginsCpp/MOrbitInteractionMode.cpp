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

#include "MOrbitInteractionMode.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MOrbitInteractionMode::MOrbitInteractionMode()
{
    m_bDragging = false;
}
//---------------------------------------------------------------------------
void MOrbitInteractionMode::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);
}
//---------------------------------------------------------------------------
String* MOrbitInteractionMode::get_Name()
{
    return "GamebryoOrbit";
}
//---------------------------------------------------------------------------
void MOrbitInteractionMode::SetInteractionMode(Object*, 
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
void MOrbitInteractionMode::ValidateInteractionMode(Object*,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
    pmState->Enabled = MFramework::Instance->ViewportManager->ActiveViewport
        ->CameraEntity->Writable;
}
//---------------------------------------------------------------------------
void MOrbitInteractionMode::MouseDown(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if (eType == MouseButtonType::LeftButton)
    {
        m_bDragging = true;
        m_bLeftDown = true;
        m_iLastX = iX;
        m_iLastY = iY;

        UserCameraSwitchHelper();
        MEntity* pmCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->CameraEntity;
        if (pmCamera != NULL)
        {
            NiEntityInterface* pkCamera;
            pkCamera = pmCamera->GetNiEntityInterface();
            pkCamera->GetPropertyData(*m_pkTranslationName, 
                *m_pkStartPosition);
            pkCamera->GetPropertyData(*m_pkRotationName,
                *m_pkStartRotation);
        }

        NiCamera* pkCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->GetNiCamera();
        if (pkCamera->GetViewFrustum().m_bOrtho)
        {
            AdjustOrthoDistance();
        }

        SetupOrbit(iX, iY);
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
                pkCamera->SetPropertyData(*m_pkRotationName,
                    *m_pkStartRotation);
            }
            m_bDragging = false;
            m_bMiddleDown = false;
            m_bLeftDown = false;
            m_bRightDown = false;
            return;
        }
        else if (eType == MouseButtonType::RightButton)
        {
            // right-clicking will exit pan mode
            InteractionModeService->ActiveMode = m_pmPreviousIM;
            m_bDragging = false;
            m_bMiddleDown = false;
            m_bLeftDown = false;
            m_bRightDown = false;
            return;
        }
        else if (eType == MouseButtonType::MiddleButton)
        {
            // allow panning to be right-click cancellable.
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
        }
        // if another button was clicked, just let the super
        // class deal with it - probably viewport movement
        __super::MouseDown(eType, iX, iY);
    }
}
//---------------------------------------------------------------------------
void MOrbitInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if (eType == MouseButtonType::LeftButton)
    {
        m_bDragging = false;
        m_bLeftDown = false;

        // check to see if we have moved enough to warrant a command
        bool bTranslationChanged = false;
        bool bRotationChanged = false;
        NiPoint3 kTranslation;
        NiPoint3 kDeltaTranslation;
        NiMatrix3 kRotation;
        NiEntityInterface* pkCamera = NULL;
        float fEulerX;
        float fEulerY;
        float fEulerZ;
        float fDeltaEulerX;
        float fDeltaEulerY;
        float fDeltaEulerZ;

        MEntity* pmCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->CameraEntity;
        if (pmCamera != NULL)
        {
            pkCamera = pmCamera->GetNiEntityInterface();
            pkCamera->GetPropertyData(*m_pkTranslationName, kTranslation);
            pkCamera->GetPropertyData(*m_pkRotationName, kRotation);

            kDeltaTranslation = *m_pkStartPosition - kTranslation;
            if ((kDeltaTranslation.x > 0.01f) || 
                (kDeltaTranslation.x < -0.01f) ||
                (kDeltaTranslation.y > 0.01f) ||
                (kDeltaTranslation.y < -0.01f) ||
                (kDeltaTranslation.z > 0.01f) ||
                (kDeltaTranslation.z < -0.01f))
            {
                bTranslationChanged = true;
            }

            m_pkStartRotation->ToEulerAnglesXYZ(fEulerX, fEulerY, fEulerZ);
            kRotation.ToEulerAnglesXYZ(
                fDeltaEulerX, fDeltaEulerY, fDeltaEulerZ);
            fDeltaEulerX = fDeltaEulerX - fEulerX;
            fDeltaEulerY = fDeltaEulerY - fEulerY;
            fDeltaEulerZ = fDeltaEulerZ - fEulerZ;
            if ((fDeltaEulerX > 0.01f) || (fDeltaEulerX < -0.01f) ||
                (fDeltaEulerY > 0.01f) || (fDeltaEulerY < -0.01f) ||
                (fDeltaEulerZ > 0.01f) || (fDeltaEulerZ < -0.01f))
            {
                bRotationChanged = true;
            }
        }

        if (bTranslationChanged || bRotationChanged)
        {
            // we have completed a mouse orbit, register the translation and
            // rotation with the camera entity
            CommandService->BeginUndoFrame("Viewport: Orbit view");
            // first set the original values on the interface so the undo
            // can record the original and changed state
            pkCamera->SetPropertyData(*m_pkRotationName, *m_pkStartRotation);
            pkCamera->SetPropertyData(*m_pkTranslationName, 
                *m_pkStartPosition);

            if (pmCamera->Writable)
            {
                // set the entity properties so an undo-able command is issued
                pmCamera->SetPropertyData(*m_pkTranslationName, new 
                    MPoint3(kTranslation), true);
                pmCamera->SetPropertyData(*m_pkRotationName, 
                    new MMatrix3(kRotation), true);
            }
            else
            {
                MessageService->AddMessage(MessageChannelType::Errors,
                    String::Format("The \"{0}\" camera is not writable "
                    "and cannot be modified.", pmCamera->Name));
            }

            CommandService->EndUndoFrame(m_bViewUndoable);
        }
    }
    else
    {
        // make sure that super class handles its mouseup
        __super::MouseUp(eType, iX, iY);
    }
}
//---------------------------------------------------------------------------
void MOrbitInteractionMode::MouseMove(int iX, int iY)
{
    MVerifyValidInstance;

    NiCamera* pkCamera;
    MEntity* pmCamera;
    int iDX;
    int iDY;

    pkCamera = MFramework::Instance->ViewportManager->ActiveViewport
        ->GetNiCamera();
    pmCamera = MFramework::Instance->ViewportManager->ActiveViewport->
        CameraEntity;
    m_bOrthographic = pkCamera->GetViewFrustum().m_bOrtho;
    iDX = iX - m_iLastX;
    iDY = iY - m_iLastY;

    if ((m_bDragging) && (m_bLeftDown))
    {
        NiPoint3 kNewLocation;
        NiMatrix3 kNewRotation;
        NiViewMath::Orbit((float)iDX * m_fMouseLookScalar, (float)iDY * 
            m_fMouseLookScalar, pkCamera->GetTranslate(), 
            pkCamera->GetRotate(), *m_pkOrbitCenter, *m_pkUpAxis, 
            kNewLocation, kNewRotation);
        if (pmCamera != NULL)
        {
            NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
            pkEntity->SetPropertyData(*m_pkTranslationName, kNewLocation);
            pkEntity->SetPropertyData(*m_pkRotationName, kNewRotation);
        }
    }
    
    __super::MouseMove(iX, iY);
}
//---------------------------------------------------------------------------
