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
#include "SceneDesignerFrameworkPCH.h"

#include "MViewportManager.h"
#include "MCameraManager.h"
#include "MEventManager.h"
#include "MRenderer.h"
#include "MFramework.h"
#include "ServiceProvider.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MViewportManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MViewportManager();
    }
}
//---------------------------------------------------------------------------
void MViewportManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MViewportManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MViewportManager* MViewportManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MViewportManager::MViewportManager() : m_iExclusiveIndex(-1),
    m_iActiveIndex(0), m_fXViewportCenter(0.5f), m_fYViewportCenter(0.5f),
    m_bViewUndoable(true)
{
    m_amViewports = new MViewport*[4];
    m_amViewports[0] = new MViewport();
    m_amViewports[1] = new MViewport();
    m_amViewports[2] = new MViewport();
    m_amViewports[3] = new MViewport();

    for (int i = 0; i < m_amViewports->Length; i++)
    {
        ResetViewportValues(m_amViewports[i], false);
    }

    __hook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
        &MViewportManager::OnNewSceneLoaded);
    __hook(&MEventManager::CameraRemovedFromManager, MEventManager::Instance,
        &MViewportManager::OnCameraRemovedFromManager);
    __hook(&MEventManager::ViewportCameraChanged, MEventManager::Instance,
        &MViewportManager::OnViewportCameraChanged);
    __hook(&MEventManager::EntityPropertyChanged, MEventManager::Instance,
        &MViewportManager::OnEntityPropertyChanged);
}
//---------------------------------------------------------------------------
void MViewportManager::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        for (int i = 0; i < m_amViewports->Length; i++)
        {
            m_amViewports[i]->Dispose();
        }

        __unhook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
            &MViewportManager::OnNewSceneLoaded);

        __unhook(&MEventManager::CameraRemovedFromManager,
            MEventManager::Instance,
            &MViewportManager::OnCameraRemovedFromManager);

        //__unhook(&MEventManager::ViewportCameraChanged,
        // MEventManager::Instance,&MViewportManager::OnViewportCameraChanged);

        MEventManager::Instance->remove_ViewportCameraChanged(
            new MEventManager::__Delegate_ViewportCameraChanged(
            this, &MViewportManager::OnViewportCameraChanged));
    }
}
//---------------------------------------------------------------------------
void MViewportManager::Startup()
{
    MVerifyValidInstance;

    RegisterViewUndoableSetting();
    ResetViewportCameras();
}
//---------------------------------------------------------------------------
void MViewportManager::OnResize()
{
    for (int i = 0; i < m_amViewports->Length; i++)
    {
        MViewport* pmViewport = m_amViewports[i];
        OnViewportCameraChanged(pmViewport, pmViewport->CameraEntity);
    }
}
//---------------------------------------------------------------------------
void MViewportManager::RegisterViewUndoableSetting()
{
    MVerifyValidInstance;

    SettingsService->RegisterSettingsObject(ms_strViewUndoableSettingName,
        __box(m_bViewUndoable), SettingsCategory::PerUser);
    SettingsService->SetChangedSettingHandler(ms_strViewUndoableSettingName,
        SettingsCategory::PerUser, new SettingChangedHandler(this,
        &MViewportManager::OnViewUndoableSettingChanged));
    OnViewUndoableSettingChanged(NULL, NULL);
}
//---------------------------------------------------------------------------
void MViewportManager::OnViewUndoableSettingChanged(Object*,
    SettingChangedEventArgs*)
{
    MVerifyValidInstance;

    __box bool* pbViewUndoable = dynamic_cast<__box bool*>(
        SettingsService->GetSettingsObject(ms_strViewUndoableSettingName,
        SettingsCategory::PerUser));
    if (pbViewUndoable != NULL)
    {
        m_bViewUndoable = *pbViewUndoable;
    }
}
//---------------------------------------------------------------------------
void MViewportManager::ClearExclusiveViewport()
{
    MVerifyValidInstance;

    if (m_iExclusiveIndex != -1)
    {
        ResetViewportValues(m_amViewports[m_iExclusiveIndex], true);
    }
    m_iExclusiveIndex = -1;
}
//---------------------------------------------------------------------------
void MViewportManager::SetExclusiveViewport(unsigned int uiIndex)
{
    MVerifyValidInstance;

    MAssert((int) uiIndex < m_amViewports->Length, "Index out of range!");

    if (m_iExclusiveIndex != (int)uiIndex)
    {
        int iOldExclusiveIndex = m_iExclusiveIndex;
        m_iExclusiveIndex = uiIndex;

        MViewport* pmOldViewport = (iOldExclusiveIndex == -1 ? NULL :
            m_amViewports[iOldExclusiveIndex]);
        MViewport* pmNewViewport = (m_iExclusiveIndex == -1 ? NULL :
            m_amViewports[m_iExclusiveIndex]);

        if (pmOldViewport)
        {
            ResetViewportValues(pmOldViewport, true);
        }
        if (pmNewViewport)
        {
            MEntity* pmCamera = pmNewViewport->CameraEntity; 
            if (MCameraManager::Instance->IsStandardCamera(pmNewViewport, 
                pmCamera))
            {
                pmNewViewport->SetViewportValues(0.0f, 1.0f,
                    1.0f, 0.0f);
            }
            else
            {
                float fLeft = 0.0f;
                float fRight = 1.0f;
                float fTop = 1.0f;
                float fBottom = 0.0f; 

                AdjustLetterboxedViewportValues(pmNewViewport, fLeft, fRight, 
                    fTop, fBottom);

                 
                pmNewViewport->SetViewportValues(fLeft, fRight, fTop, fBottom);
            }
            
        }

        MEventManager::Instance->RaiseExclusiveViewportChanged(
            pmOldViewport, pmNewViewport);

        int iOldActiveIndex = m_iActiveIndex;
        m_iActiveIndex = uiIndex;

        MEventManager::Instance->RaiseActiveViewportChanged(
            m_amViewports[iOldActiveIndex], m_amViewports[m_iActiveIndex]);
    }
}
//---------------------------------------------------------------------------
MViewport* MViewportManager::get_ExclusiveViewport()
{
    MVerifyValidInstance;

    if (m_iExclusiveIndex == -1)
    {
        return NULL;
    }

    return m_amViewports[m_iExclusiveIndex];
}
//---------------------------------------------------------------------------
void MViewportManager::SetActiveViewport(unsigned int uiIndex)
{
    MVerifyValidInstance;

    MAssert((int) uiIndex < m_amViewports->Length, "Index out of range!");

    if ((int)uiIndex != m_iActiveIndex)
    {
        if (m_iExclusiveIndex == m_iActiveIndex)
        {
            SetExclusiveViewport(uiIndex);
        }
        else
        {
            ClearExclusiveViewport();

            int iOldActiveIndex = m_iActiveIndex;
            m_iActiveIndex = uiIndex;

            MEventManager::Instance->RaiseActiveViewportChanged(
                m_amViewports[iOldActiveIndex], m_amViewports[m_iActiveIndex]);
        }
    }
}
//---------------------------------------------------------------------------
MViewport* MViewportManager::get_ActiveViewport()
{
    MVerifyValidInstance;

    return m_amViewports[m_iActiveIndex];
}
//---------------------------------------------------------------------------
unsigned int MViewportManager::get_ViewportCount()
{
    MVerifyValidInstance;

    return m_amViewports->Length;
}
//---------------------------------------------------------------------------
MViewport* MViewportManager::GetViewport(unsigned int uiIndex)
{
    MVerifyValidInstance;

    MAssert((int) uiIndex < m_amViewports->Length, "Index out of range.");
    return m_amViewports[uiIndex];
}
//---------------------------------------------------------------------------
void MViewportManager::AssignDefaultCamerasAndRenderingModes()
{
    MVerifyValidInstance;

    IRenderingMode* pmStandardMode = RenderingModeService
        ->GetRenderingModeByName("Standard");
    MAssert(pmStandardMode != NULL, "Standard rendering mode not found!");

    IRenderingMode* pmWireframeMode = RenderingModeService
        ->GetRenderingModeByName("Wireframe (Unshaded)");
    MAssert(pmWireframeMode != NULL, "Wireframe rendering mode not found!");

    m_amViewports[0]->SetCamera(
        MCameraManager::Instance->GetStandardCameraEntity(m_amViewports[0],
        MCameraManager::StandardCamera::NegativeZ), m_bViewUndoable);
    m_amViewports[0]->RenderingMode = pmWireframeMode;
    m_amViewports[1]->SetCamera(
        MCameraManager::Instance->GetStandardCameraEntity(m_amViewports[1],
        MCameraManager::StandardCamera::PositiveY), m_bViewUndoable);
    m_amViewports[1]->RenderingMode = pmWireframeMode;
    m_amViewports[2]->SetCamera(
        MCameraManager::Instance->GetStandardCameraEntity(m_amViewports[2],
        MCameraManager::StandardCamera::PositiveX), m_bViewUndoable);
    m_amViewports[2]->RenderingMode = pmWireframeMode;
    m_amViewports[3]->SetCamera(
        MCameraManager::Instance->GetStandardCameraEntity(m_amViewports[3],
        MCameraManager::StandardCamera::Perspective), m_bViewUndoable);
    m_amViewports[3]->RenderingMode = pmStandardMode;
}
//---------------------------------------------------------------------------
void MViewportManager::ResetViewportCameras()
{
    MVerifyValidInstance;

    CommandService->BeginUndoFrame("Reset viewports to default cameras");
    AssignDefaultCamerasAndRenderingModes();
    ClearExclusiveViewport();
    SetActiveViewport(3);
    CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
void MViewportManager::OnResetViewportCameras(Object*,
    EventArgs*)
{
    MVerifyValidInstance;

    ResetViewportCameras();
}
//---------------------------------------------------------------------------
void MViewportManager::Update(float fTime)
{
    MVerifyValidInstance;

    if (m_iExclusiveIndex != -1)
    {
        m_amViewports[m_iExclusiveIndex]->Update(fTime);
    }
    else
    {
        for (int i = 0; i < m_amViewports->Length; i++)
        {
            m_amViewports[i]->Update(fTime);
        }
    }
}
//---------------------------------------------------------------------------
void MViewportManager::PreRender()
{
    MVerifyValidInstance;

    // allow each viewport to set its clipping planes
    if (m_iExclusiveIndex != -1)
    {
        m_amViewports[m_iExclusiveIndex]->UpdateClippingPlanes();
    }
    else
    {
        for (int i = 0; i < m_amViewports->Length; i++)
        {
            m_amViewports[i]->UpdateClippingPlanes();
        }
    }
}
//---------------------------------------------------------------------------
void MViewportManager::RenderUnselectedEntities()
{
    MVerifyValidInstance;

    if (m_iExclusiveIndex != -1)
    {
        m_amViewports[m_iExclusiveIndex]->RenderUnselectedEntities();
    }
    else
    {
        for (int i = 0; i < m_amViewports->Length; i++)
        {
            m_amViewports[i]->RenderUnselectedEntities();
        }
    }
}
//---------------------------------------------------------------------------
void MViewportManager::RenderSelectedEntities()
{
    MVerifyValidInstance;

    if (m_iExclusiveIndex != -1)
    {
        m_amViewports[m_iExclusiveIndex]->RenderSelectedEntities();
    }
    else
    {
        for (int i = 0; i < m_amViewports->Length; i++)
        {
            m_amViewports[i]->RenderSelectedEntities();
        }
    }
}
//---------------------------------------------------------------------------
void MViewportManager::RenderGizmo()
{
    MVerifyValidInstance;

    if (m_iExclusiveIndex != -1)
    {
        m_amViewports[m_iExclusiveIndex]->RenderGizmo();
    }
    else
    {
        for (int i = 0; i < m_amViewports->Length; i++)
        {
            m_amViewports[i]->RenderGizmo();
        }
    }
}
//---------------------------------------------------------------------------
void MViewportManager::RenderScreenElements()
{
    MVerifyValidInstance;

    if (m_iExclusiveIndex != -1)
    {
        m_amViewports[m_iExclusiveIndex]->RenderScreenElements(true);
    }
    else
    {
        for (int i = 0; i < m_amViewports->Length; i++)
        {
            m_amViewports[i]->RenderScreenElements(i == m_iActiveIndex);
        }
    }
}
//---------------------------------------------------------------------------
void MViewportManager::ActivateViewport(int iScreenX, int iScreenY)
{
    MVerifyValidInstance;

    // If we are in exclusive viewport mode, don't change viewport activation.
    if (m_iExclusiveIndex != -1)
    {
        return;
    }

    float fXRatio = (float) iScreenX / MRenderer::Instance->Width;
    float fYRatio = 1.0f - (float) iScreenY / MRenderer::Instance->Height;

    int iViewportIndex = -1;
    if (fXRatio < m_fXViewportCenter)
    {
        if (fYRatio < m_fYViewportCenter)
        {
            iViewportIndex = 2;
        }
        else if (fYRatio > m_fYViewportCenter)
        {
            iViewportIndex = 0;
        }
    }
    else if (fXRatio > m_fXViewportCenter)
    {
        if (fYRatio < m_fYViewportCenter)
        {
            iViewportIndex = 3;
        }
        else if (fYRatio > m_fYViewportCenter)
        {
            iViewportIndex = 1;
        }
    }

    if (iViewportIndex != -1)
    {
        SetActiveViewport((unsigned int) iViewportIndex);
    }
}
//---------------------------------------------------------------------------
bool MViewportManager::IsInsideActiveViewport(int iScreenX, int iScreenY)
{
    int iViewportX;
    int iViewportY;
    if (ScreenCoordinatesToViewportCoordinates(iScreenX, iScreenY,
        iViewportX, iViewportY))
    {
        return (iViewportX >= 0) &&
            (iViewportY >= 0) &&
            (iViewportX < ActiveViewport->Width) &&
            (iViewportY < ActiveViewport->Height);

    }
    return true;
}
//---------------------------------------------------------------------------
bool MViewportManager::ScreenCoordinatesToViewportCoordinates(int iScreenX,
    int iScreenY, [Out] Int32& iViewportX, [Out] Int32& iViewportY)
{
    MVerifyValidInstance;

    float fXRatio = (float) iScreenX / MRenderer::Instance->Width;
    float fYRatio = 1.0f - (float) iScreenY / MRenderer::Instance->Height;

    iViewportX = (int) ((fXRatio - ActiveViewport->Left) /
        (ActiveViewport->Right - ActiveViewport->Left) *
        ActiveViewport->Width);
    iViewportY = (int) (ActiveViewport->Height -
        (fYRatio - ActiveViewport->Bottom) /
        (ActiveViewport->Top - ActiveViewport->Bottom) *
        ActiveViewport->Height);

    return true;
}
//---------------------------------------------------------------------------
void MViewportManager::AdjustLetterboxedViewportValues(MViewport* pmViewport,
    float& fLeft, float& fRight, float& fTop, float& fBottom)
{
    MEntity* pmCamera = pmViewport->CameraEntity; 

    MAssert(pmCamera->HasProperty(m_strAspectPropertyName), "Camera does "
        "not have aspect ratio property!");

    float fBackBufferAspect = ((float)MRenderer::Instance->Width) / 
        ((float)MRenderer::Instance->Height);

    float fCameraAspect = (*dynamic_cast<__box Single*>
        (pmCamera->GetPropertyData(m_strAspectPropertyName)));

    float fAdjustedAspect = fCameraAspect / fBackBufferAspect;
    if (fAdjustedAspect > 1.0f)
    {
        float fVerticalCenter = (fTop + fBottom)/2.0f;
        float fVerticalSpan = (1.0f/fAdjustedAspect)/2*fabs(fTop-fBottom);

        fTop = fVerticalCenter+fVerticalSpan;
        fBottom = fVerticalCenter-fVerticalSpan;
        
        //pmNewViewport->SetViewportValues(fLeft, fRight,
        //    0.5f+fVerticalSpan, 0.5f-fVerticalSpan);
    }
    else
    {
        float fHorizontalCenter = (fLeft + fRight)/2.0f;
        float fHorizontalSpan = (fAdjustedAspect)/2*fabs(fLeft-fRight);

        fLeft = fHorizontalCenter-fHorizontalSpan;
        fRight = fHorizontalCenter+fHorizontalSpan;

        //pmNewViewport->SetViewportValues(
        //    0.5f-fHorizontalSpan, 0.5f+fHorizontalSpan,
        //    1.0f, 0.0f);
    }
    
}
//---------------------------------------------------------------------------
void MViewportManager::ResetViewportValues(MViewport* pmViewport, 
    bool bAdjustAspect)
{
    MVerifyValidInstance;
    float fLeft = 0.0f;
    float fRight = 0.0f;
    float fTop = 0.0f;
    float fBottom = 0.0f;

    if (pmViewport == m_amViewports[0])
    {
        fLeft = 0;
        fRight = m_fXViewportCenter - ms_fBorderOffset;
        fTop = 1.0f;
        fBottom = m_fYViewportCenter + ms_fBorderOffset;
    }
    else if (pmViewport == m_amViewports[1])
    {
        fLeft = m_fXViewportCenter + ms_fBorderOffset;
        fRight = 1.0f;
        fTop = 1.0f;
        fBottom = m_fYViewportCenter + ms_fBorderOffset;
    }
    else if (pmViewport == m_amViewports[2])
    {
        fLeft = 0.0f;
        fRight = m_fXViewportCenter - ms_fBorderOffset;
        fTop = m_fYViewportCenter - ms_fBorderOffset;
        fBottom = 0.0f;
    }
    else if (pmViewport == m_amViewports[3])
    {
        fLeft = m_fXViewportCenter + ms_fBorderOffset;
        fRight = 1.0f;
        fTop = m_fYViewportCenter - ms_fBorderOffset;
        fBottom = 0.0f;
    }
    if (bAdjustAspect && !MCameraManager::Instance->IsStandardCamera(
        pmViewport, pmViewport->CameraEntity))
    {
        AdjustLetterboxedViewportValues(pmViewport, fLeft, fRight, fTop,
            fBottom);
    }
    pmViewport->SetViewportValues(fLeft, fRight, fTop, fBottom);
}
//---------------------------------------------------------------------------
void MViewportManager::OnNewSceneLoaded(MScene* pmScene)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene == pmScene)
    {
        ResetViewportCameras();
    }
}
//---------------------------------------------------------------------------
void MViewportManager::OnCameraRemovedFromManager(MEntity* pmCamera)
{
    MVerifyValidInstance;

    for (int i = 0; i < m_amViewports->Length; i++)
    {
        MViewport* pmViewport = m_amViewports[i];
        if (pmViewport->CameraEntity == pmCamera)
        {
            MEntity* pmPerspectiveCamera = MCameraManager::Instance
                ->GetStandardCameraEntity(pmViewport,
                MCameraManager::StandardCamera::Perspective);
            MAssert(pmPerspectiveCamera != NULL, "Perspective camera not "
                "found in viewport!");

            pmViewport->SetCamera(pmPerspectiveCamera, false);
        }
    }
}
//---------------------------------------------------------------------------
void MViewportManager::OnViewportCameraChanged(MViewport* pmViewport,
    MEntity*)
{
    
    if (ExclusiveViewport == NULL)
    {
        ResetViewportValues(pmViewport, true);
    }
    else
    {
        if (ExclusiveViewport == pmViewport)
        {
            //Reset the exclusive viewport
            float fLeft = 0.0f;
            float fRight = 1.0f;
            float fTop = 1.0f;
            float fBottom = 0.0f; 

            AdjustLetterboxedViewportValues(pmViewport, fLeft, fRight, 
                fTop, fBottom);
                
            pmViewport->SetViewportValues(fLeft, fRight, fTop, fBottom);
        }
    }
}
//---------------------------------------------------------------------------
void MViewportManager::OnEntityPropertyChanged(MEntity* pmEntity, 
    String* strPropertyName, unsigned int, bool)
{
    //This function will be called frquently, and do nothing 99.9% of the time,
    //so make it quick to bail early
    if (strPropertyName->Equals(m_strAspectPropertyName))
    {
        for (int i = 0; i < m_amViewports->Length; i++)
        {
            MViewport* pmViewport = m_amViewports[i];
            if (pmViewport->CameraEntity == pmEntity)
            {
                OnViewportCameraChanged(pmViewport, pmEntity);
            }
        }

    }
}
//---------------------------------------------------------------------------
void MViewportManager::ToggleViewports(Object*, EventArgs*)
{
    MVerifyValidInstance;

    if (m_iExclusiveIndex == m_iActiveIndex)
    {
        ClearExclusiveViewport();
    }
    else
    {
        SetExclusiveViewport(m_iActiveIndex);
    }
}
//---------------------------------------------------------------------------
ISettingsService* MViewportManager::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found!");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
ICommandService* MViewportManager::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found.");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
IRenderingModeService* MViewportManager::get_RenderingModeService()
{
    if (ms_pmRenderingModeService == NULL)
    {
        ms_pmRenderingModeService = MGetService(IRenderingModeService);
        MAssert(ms_pmRenderingModeService != NULL, "Rendering mode service "
            "not found.");
    }
    return ms_pmRenderingModeService;
}
//---------------------------------------------------------------------------
