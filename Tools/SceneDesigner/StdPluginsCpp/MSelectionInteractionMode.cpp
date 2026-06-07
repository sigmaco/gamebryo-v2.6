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

#include "MSelectionInteractionMode.h"
#include "NiVirtualBoolBugWrapper.h"
#include "MSettingsHelper.h"

using namespace System::Collections::Generic;
using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

#define ENSURE_GIZMO_LOADED() if (!m_pkGizmo) \
    { \
        if (LoadGizmo() == false) \
        { \
            NIASSERT(!"MSelectionInteractionMode could not load " \
                "..\\..\\Data\\select.nif"); \
        } \
    }

//---------------------------------------------------------------------------
MSelectionInteractionMode::MSelectionInteractionMode() : m_pkGizmo(NULL)
{
    m_pkPickRecord = NiNew NiPick::Record(NULL);
    m_pkPickRecord->SetDistance(NI_INFINITY);
    m_pkPickRecord->SetIntersection(NiPoint3::ZERO);
    m_pkPickRecord->SetNormal(NiPoint3::ZERO);
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::RegisterSettings()
{
    // register the settings of all the classes we inherit from 
    __super::RegisterSettings();

    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MSelectionInteractionMode::OnSettingChanged);

    // convert this settings from a distance to a ratio
    float fScale = STANDARD_DISTANCE / m_fDefaultDistance;
    MSettingsHelper::GetStandardSetting(MSettingsHelper::GIZMO_SCALE, fScale,
        pmHandler);
    m_fDefaultDistance = STANDARD_DISTANCE / fScale;
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::OnSettingChanged(Object*,
    SettingChangedEventArgs* pmEventArgs)
{
    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    // if a setting we care about was changed, re-cache it
    if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::GIZMO_SCALE))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fDefaultDistance = STANDARD_DISTANCE / *pfVal;
        }
    }
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::SetInteractionMode(Object*, 
    EventArgs*)
{
    MVerifyValidInstance;

    InteractionModeService->ActiveMode = this;
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::ValidateInteractionMode(Object*,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::SetGizmoScale(NiCamera* pkCamera)
{
    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;

    // figure out the gizmo's location
    NiPoint3 kCenter;
    SelectionService->SelectionCenter->ToNiPoint3(kCenter);
    m_pkGizmo->SetTranslate(kCenter);
    // figure out how large it needs to be
    if (pkCamera->GetViewFrustum().m_bOrtho)
    {
        NiFrustum pkFrustum = pkCamera->GetViewFrustum();
        m_pkGizmo->SetScale((pkFrustum.m_fRight * 2.0f) / 
            m_fDefaultDistance);
    }
    else
    {
        float fCamDistance = (kCenter - 
            pkCamera->GetWorldTranslate()).Length();
        if ((fCamDistance / m_fDefaultDistance) > 0.0f)
        {
            m_pkGizmo->SetScale((fCamDistance / m_fDefaultDistance) * 
                pkCamera->GetViewFrustum().m_fRight * 2.0f);
        }
        else
        {
            m_pkGizmo->SetScale(1.0f);
        }
    }
    m_pkGizmo->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_pkGizmo);
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::Do_Dispose(bool bDisposing)
{
    NiDelete m_pkPickRecord;
    MDisposeRefObject(m_pkGizmo);
    MViewInteractionMode::Do_Dispose(bDisposing);
}
//---------------------------------------------------------------------------
String* MSelectionInteractionMode::get_Name()
{
    return "GamebryoSelection";
}
//---------------------------------------------------------------------------
bool MSelectionInteractionMode::Initialize()
{
    MVerifyValidInstance;

    m_fDefaultDistance = STANDARD_DISTANCE;

    // Loading of NIF file is deferred
    m_pkGizmo = NULL;

    return true;
}
//---------------------------------------------------------------------------
bool MSelectionInteractionMode::LoadGizmo()
{
    String* pmPath = String::Concat(MFramework::Instance->AppStartupPath,
        "..\\..\\Data\\select.nif");
    NiStream kStream;
    const char* pcPath = MStringToCharPointer(pmPath);
    int iSuccess = NiVirtualBoolBugWrapper::NiStream_Load(kStream, pcPath);
    MFreeCharPointer(pcPath);

    if (iSuccess != 0)
    {
        m_pkGizmo = (NiNode*)kStream.GetObjectAt(0);
        MInitRefObject(m_pkGizmo);
        m_pkGizmo->UpdateProperties();
        m_pkGizmo->UpdateEffects();
        m_pkGizmo->Update(0.0f);
        NiMesh::CompleteSceneModifiers(m_pkGizmo);
    }
    else
    {
        MessageService->AddMessage(MessageChannelType::Errors, 
            "Failed to load scale gizmo file 'select.nif'");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::RenderGizmo(
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;

    if ((SelectionService->NumSelectedEntities > 0))
    {
        NiEntityRenderingContext* pkContext = 
            pmRenderingContext->GetRenderingContext();
        NiCamera* pkCam = pkContext->m_pkCamera;

        //clear the z-buffer
        pkContext->m_pkRenderer->ClearBuffer(NULL, NiRenderer::CLEAR_ZBUFFER);

        SetGizmoScale(pkCam);
        NiDrawScene(pkCam, m_pkGizmo, *pkContext->m_pkCullingProcess);
    }
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::MouseDown(MouseButtonType eType, int iX,
    int iY)
{
    MVerifyValidInstance;

    MViewInteractionMode::MouseDown(eType, iX, iY);

    if (eType == MouseButtonType::LeftButton)
    {
        NiPoint3 kOrigin, kDir;
        NiViewMath::MouseToRay((float) iX, (float) iY, 
            MFramework::Instance->ViewportManager->ActiveViewport->Width,
            MFramework::Instance->ViewportManager->ActiveViewport->Height,
            MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera(), kOrigin, kDir);

        MEntity* pmPickedEntity;
        float fPickDistance = NI_INFINITY;
        if (MFramework::Instance->PickUtility->PerformPick(
            MFramework::Instance->Scene, kOrigin, kDir, false))
        {
            const NiPick* pkPick =
                MFramework::Instance->PickUtility->GetNiPick();
            const NiPick::Results& kPickResults = pkPick->GetResults();
            NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
            if (pkPickRecord)
            {
                if (pkPickRecord->GetDistance() < fPickDistance)
                {
                    m_pkPickRecord->SetDistance(pkPickRecord->GetDistance());
                    m_pkPickRecord->SetIntersection(
                        pkPickRecord->GetIntersection());
                    m_pkPickRecord->SetNormal(pkPickRecord->GetNormal());
                    fPickDistance = pkPickRecord->GetDistance();
                    NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
                    pmPickedEntity = MFramework::Instance->PickUtility
                        ->GetEntityFromPickedObject(pkPickedObject);
                }
            }
        }

        MFramework::Instance->ProxyManager->UpdateProxyScales(
            MFramework::Instance->ViewportManager->ActiveViewport);

        if (MFramework::Instance->PickUtility->PerformPick(
            MFramework::Instance->ProxyManager->ProxyScene, kOrigin, kDir,
            false))
        {
            const NiPick* pkPick =
                MFramework::Instance->PickUtility->GetNiPick();
            const NiPick::Results& kPickResults = pkPick->GetResults();
            NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
            if (pkPickRecord)
            {
                if (pkPickRecord->GetDistance() < fPickDistance)
                {
                    m_pkPickRecord->SetDistance(pkPickRecord->GetDistance());
                    m_pkPickRecord->SetIntersection(
                        pkPickRecord->GetIntersection());
                    m_pkPickRecord->SetNormal(pkPickRecord->GetNormal());
                    fPickDistance = pkPickRecord->GetDistance();
                    NiAVObject* pkPickedProxy = pkPickRecord->GetAVObject();
                    MEntity* pmPickedProxy = MFramework::Instance->PickUtility
                        ->GetEntityFromPickedObject(pkPickedProxy);
                    if (pmPickedProxy != NULL &&
                        pmPickedProxy->HasProperty("Source Entity"))
                    {
                        MEntity* pmEntity = dynamic_cast<MEntity*>(
                            pmPickedProxy->GetPropertyData("Source Entity"));
                        if (pmEntity != NULL)
                        {
                            if (MFramework::Instance->Scene->IsEntityInScene(
                                pmEntity))
                            {
                                pmPickedEntity = pmEntity;
                            }
                        }
                    }
                }
            }
        }

        if (pmPickedEntity != NULL)
        {
            MPrefabManager* pm = MFramework::Instance->PrefabManager;
            while (pmPickedEntity->PrefabRoot != NULL && 
                   !pm->PickPrefabEntities)
            {
                pmPickedEntity = pmPickedEntity->PrefabRoot;
            }
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
            {
                // first, check if the picked entity is already selected
                bool bAddSelection = true;
                List<MEntity*>* entitiesToRemove = new List<MEntity*>();
                MEntity* amSelection[] = 
                    SelectionService->GetSelectedEntities();
                for (int i = 0; i < amSelection->Count; i++)
                {
                    if (amSelection[i] == pmPickedEntity)
                    {
                        bAddSelection = false;
                        break;
                    }
                    // we also want to see if we are a prefab and our
                    // children need to be removed
                    if (amSelection[i]->PrefabRoot == pmPickedEntity)
                    {
                        entitiesToRemove->Add(amSelection[i]);
                    }
                }
                if (bAddSelection)
                {
                    SelectionService->AddEntityToSelection(pmPickedEntity);
                    List<MEntity*>::Enumerator en = 
                        entitiesToRemove->GetEnumerator();
                    while (en.MoveNext())
                    {
                        SelectionService->
                            RemoveEntityFromSelection(en.Current);
                    }
                }
            }
            else if (GetAsyncKeyState(VK_MENU) & 0x8000)
            {
                // first check that the entity is in the selection
                bool bRemove = false;
                MEntity* amSelection[] = 
                    SelectionService->GetSelectedEntities();
                for (int i = 0; i < amSelection->Count; i++)
                {
                    if (amSelection[i] == pmPickedEntity)
                    {
                        bRemove = true;
                        break;
                    }
                }
                if (bRemove)
                {
                    SelectionService->RemoveEntityFromSelection(
                        pmPickedEntity);
                }
            }
            else
            {
                // check that the selection doesn't match the entity
                if ((SelectionService->NumSelectedEntities != 1) || 
                    (SelectionService->SelectedEntity != pmPickedEntity))
                {
                    SelectionService->SelectedEntity = pmPickedEntity;
                }
            }
        }
        else
        {
            SelectionService->SelectedEntity = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::SetPickPrefabEntities(Object*, EventArgs*)
{
    MPrefabManager* pm = MFramework::Instance->PrefabManager;
    pm->PickPrefabEntities = !pm->PickPrefabEntities;
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::ValidatePickPrefabEntities(Object*,
    UIState* pmState)
{
    MPrefabManager* pm = MFramework::Instance->PrefabManager;
    pmState->Checked = pm->PickPrefabEntities;
}
//---------------------------------------------------------------------------
