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

#include "MScaleInteractionMode.h"
#include "NiVirtualBoolBugWrapper.h"
#include "MSettingsHelper.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

#define ENSURE_GIZMO_LOADED() if (!m_pkGizmo) \
    { \
        if (LoadGizmo() == false) \
        { \
            NIASSERT(!"MScaleInteractionMode could not load " \
                "..\\..\\Data\\scale.nif"); \
        } \
    }

//---------------------------------------------------------------------------
MScaleInteractionMode::MScaleInteractionMode() : 
    m_pkInitialScales(NULL), m_pkInitialScalesUnique(NULL),
    m_pkInitialTranslations(NULL), m_pkInitialTranslationsUnique(NULL),
    m_pmPreviousSelection(NULL), m_pmCloneArray(NULL), m_pkCloneCenter(NULL)
{
    m_pkInitialScales = NiNew NiTPrimitiveSet<float>(32);
    m_pkInitialScalesUnique = NiNew NiTPrimitiveSet<bool>(32);
    m_pkInitialTranslations = NiNew NiTObjectSet<NiPoint3>(32);
    m_pkInitialTranslationsUnique = NiNew NiTPrimitiveSet<bool>(32);
    m_pmPreviousSelection = new ArrayList(1);
    m_pmCloneArray = new CloneArray();
    m_pkCloneCenter = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::RegisterSettings()
{
    // register the settings of all the classes we inherit from 
    __super::RegisterSettings();

    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MScaleInteractionMode::OnSettingChanged);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::SCALE_SNAP, 
        m_fScaleSnapIncrement, pmHandler);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::SCALE_SNAP_ENABLED,
        m_bSnapEnabled, pmHandler);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::SCALE_PRECISION,
        m_fPrecision, pmHandler);

    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::SCALE_PRECISION_ENABLED, m_bPrecisionEnabled, 
        pmHandler);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::TRANSLATION_PRECISION,
        m_fTranslationPrecision, pmHandler);

    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED, 
        m_bTranslationPrecisionEnabled, pmHandler);
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::OnSettingChanged(Object*,
    SettingChangedEventArgs* pmEventArgs)
{
    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SCALE_SNAP))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fScaleSnapIncrement = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SCALE_SNAP_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bSnapEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SCALE_PRECISION))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fPrecision = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SCALE_PRECISION_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bPrecisionEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_PRECISION))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fTranslationPrecision = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bTranslationPrecisionEnabled = *pbVal;
        }
    }
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);

    NiDelete m_pkInitialTranslations;
    NiDelete m_pkInitialTranslationsUnique;
    NiDelete m_pkInitialScales;
    NiDelete m_pkInitialScalesUnique;
    NiDelete m_pkPick;

    NiDelete m_pkCloneCenter;
}
//---------------------------------------------------------------------------
String* MScaleInteractionMode::get_Name()
{
    return "GamebryoScale";
}
//---------------------------------------------------------------------------
bool MScaleInteractionMode::Initialize()
{
    MVerifyValidInstance;

    m_bAlreadyScaling = false;
    m_fDefaultDistance = STANDARD_DISTANCE;
    m_fScaleSnapIncrement = 0.1f;
    m_bSnapEnabled = true;
    m_fInitialGizmoScale = 1.0f;
    m_fPrecision = 0.05f;
    m_bPrecisionEnabled = true;

    m_pkPick = NiNew NiPick();

    return true;
}
//---------------------------------------------------------------------------
bool MScaleInteractionMode::LoadGizmo()
{
    String* pmPath = String::Concat(MFramework::Instance->AppStartupPath,
        "..\\..\\Data\\scale.nif");
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
            "Failed to load scale gizmo file 'scale.nif'");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::SetInteractionMode(Object*, 
    EventArgs*)
{
    MVerifyValidInstance;

    InteractionModeService->ActiveMode = this;
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::ValidateInteractionMode(Object*,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
}
//---------------------------------------------------------------------------
bool MScaleInteractionMode::CanTransform()
{
    MEntity* amEntities[] = SelectionService->GetSelectedEntities();
    for (int i = 0; i < amEntities->Count; ++i)
    {
        if (!amEntities[i]->Writable)
        {
            return false;
        }
    }
    for (int i = 0; i < amEntities->Count; i++)
    {
        if (amEntities[i] != NULL &&
            amEntities[i]->HasProperty(*m_pkTranslationName) &&
            amEntities[i]->HasProperty(*m_pkScaleName))
        {
            if (!amEntities[i]->IsPropertyReadOnly(*m_pkTranslationName) &&
                !amEntities[i]->IsPropertyReadOnly(*m_pkScaleName))
            {
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::ScaleHelper(const float fX, const float fY)
{
    MVerifyValidInstance;

    float fCurrentDistance;
    float fScaleMultiplier;

    fCurrentDistance = GetDistance(fX, fY);
    fScaleMultiplier = fCurrentDistance / m_fStartDistance;
    if (m_bSnapEnabled)
    {
        // round this number to the nearest m_fScaleSnapIncrement
        fScaleMultiplier /= m_fScaleSnapIncrement;
        fScaleMultiplier = NiFloor(fScaleMultiplier + 0.5f);
        fScaleMultiplier *= m_fScaleSnapIncrement;
    }

    NiPoint3 kCenter;
    if (m_bCloning)
    {
        kCenter = *m_pkCloneCenter;
    }
    else
    {
        SelectionService->SelectionCenter->ToNiPoint3(kCenter);
    }

    if (m_bCloning)
    {
        // if cloning, just scale the cloned entities
        unsigned int uiIndex = 0;
        CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
        while (en.MoveNext())
        {
            NIASSERT(en.Current->Length > 0);
            MEntity* pmEntity = en.Current[0];
            if (pmEntity == NULL ||
                !pmEntity->HasProperty(*m_pkTranslationName) ||
                !pmEntity->HasProperty(*m_pkScaleName))
            {
                continue;
            }

            NiEntityInterface* pkEntity = pmEntity->GetNiEntityInterface();
            float fCurrentScale;
            fCurrentScale = m_pkInitialScales->GetAt(uiIndex) *
                fScaleMultiplier;
            if (m_bPrecisionEnabled)
            {
                // if precision is turned on, we must round the scale such 
                // that it makes the object's final scale snap.
                fCurrentScale /= m_fPrecision;
                fCurrentScale = NiFloor(fCurrentScale + 0.5f);
                fCurrentScale *= m_fPrecision;
            }
            pkEntity->SetPropertyData(*m_pkScaleName, fCurrentScale);

            NiPoint3 kEntityTranslation = m_pkInitialTranslations->GetAt(
                uiIndex);
            NiPoint3 kDeltaTranslation = kEntityTranslation - kCenter;
            kDeltaTranslation *= fScaleMultiplier;
            NiPoint3 kFinalTranslation = kCenter + kDeltaTranslation;
            if (m_bTranslationPrecisionEnabled)
            {
                // if precision is turned on, we must round translation to 
                // the nearest m_fTranslationPrecision;
                kFinalTranslation /= m_fTranslationPrecision;
                kFinalTranslation.x = NiFloor(kFinalTranslation.x + 0.5f);
                kFinalTranslation.y = NiFloor(kFinalTranslation.y + 0.5f);
                kFinalTranslation.z = NiFloor(kFinalTranslation.z + 0.5f);
                kFinalTranslation *= m_fTranslationPrecision;
            }
            pkEntity->SetPropertyData(*m_pkTranslationName, kFinalTranslation);
            uiIndex++;
        }
    }
    else
    {
        MEntity* amEntities[] = SelectionService->GetSelectedEntities();
        unsigned int uiIndex = 0;
        for (int i = 0; i < amEntities->Count; i++)
        {
            if (amEntities[i] != NULL &&
                amEntities[i]->HasProperty(*m_pkTranslationName) &&
                amEntities[i]->HasProperty(*m_pkScaleName))
            {
                NiEntityInterface* pkEntity = amEntities[i]->
                    GetNiEntityInterface();
                float fCurrentScale;
                fCurrentScale = m_pkInitialScales->GetAt(uiIndex) *
                    fScaleMultiplier;
                if (m_bPrecisionEnabled)
                {
                    // if precision is turned on, we must round the scale such 
                    // that it makes the object's final scale snap.
                    fCurrentScale /= m_fPrecision;
                    fCurrentScale = NiFloor(fCurrentScale + 0.5f);
                    fCurrentScale *= m_fPrecision;
                }
                pkEntity->SetPropertyData(*m_pkScaleName, fCurrentScale);

                NiPoint3 kEntityTranslation = m_pkInitialTranslations->GetAt(
                    uiIndex);
                NiPoint3 kDeltaTranslation = kEntityTranslation - kCenter;
                kDeltaTranslation *= fScaleMultiplier;
                NiPoint3 kFinalTranslation;
                kFinalTranslation = kCenter + kDeltaTranslation;
                if (m_bTranslationPrecisionEnabled)
                {
                    // if precision is turned on, we must round translation to
                    // the nearest m_fTranslationPrecision;
                    kFinalTranslation /= m_fTranslationPrecision;
                    kFinalTranslation.x = NiFloor(kFinalTranslation.x + 0.5f);
                    kFinalTranslation.y = NiFloor(kFinalTranslation.y + 0.5f);
                    kFinalTranslation.z = NiFloor(kFinalTranslation.z + 0.5f);
                    kFinalTranslation *= m_fTranslationPrecision;
                }
                pkEntity->SetPropertyData(*m_pkTranslationName,
                    kFinalTranslation);
                uiIndex++;
            }
        }
    }
    // scale the Gizmo as well
    m_fCurrentScale = m_fInitialGizmoScale * fScaleMultiplier;
}
//---------------------------------------------------------------------------
float MScaleInteractionMode::GetDistance(const float fX,
    const float fY)
{
    NiPoint3 kCenter;
    float fDX, fDY;

    if (m_bCloning)
    {
        kCenter = *m_pkCloneCenter;
    }
    else
    {
        SelectionService->SelectionCenter->ToNiPoint3(kCenter);
    }
    MFramework::Instance->ViewportManager->ActiveViewport->GetNiCamera()->
        WorldPtToScreenPt(kCenter, fDX, fDY);

    int iScreenX = (int) (MFramework::Instance->Renderer->Width * fDX);
    int iScreenY = (int) (MFramework::Instance->Renderer->Height *
        (1.0f - fDY));
    int iViewportX, iViewportY;
    MFramework::Instance->ViewportManager
        ->ScreenCoordinatesToViewportCoordinates(iScreenX, iScreenY,
        iViewportX, iViewportY);
    fDX = (float) iViewportX - fX;
    fDY = (float) iViewportY - fY;
    return (NiSqrt(fDX * fDX + fDY * fDY));
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::SetGizmoScale(NiCamera* pkCamera)
{
    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;

    // figure out the gizmo's location
    NiPoint3 kCenter;
    if (m_bCloning)
    {
        kCenter = *m_pkCloneCenter;
    }
    else
    {
        SelectionService->SelectionCenter->ToNiPoint3(kCenter);
    }
    m_pkGizmo->SetTranslate(kCenter);
    // figure out how large it needs to be
    if (m_bAlreadyScaling)
    {
        m_pkGizmo->SetScale(m_fCurrentScale);
    }
    else
    {
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
    }
    m_pkGizmo->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_pkGizmo);
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::Update(float fTime)
{
    MVerifyValidInstance;

    __super::Update(fTime);

    if (m_bCloning)
    {
        CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
        while (en.MoveNext())
        {
            EntityArray amEntities = en.Current;
            for (int i = 0; i < amEntities->Length; ++i)
            {
                MEntity* pmEntity = amEntities[i];
                pmEntity->Update(
                    MFramework::Instance->TimeManager->CurrentTime, 
                    MFramework::Instance->ExternalAssetManager);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::RenderGizmo(
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;

    if (((CanTransform()) || (m_bCloning)))
    {
        NiEntityRenderingContext* pkContext = 
            pmRenderingContext->GetRenderingContext();
        NiCamera* pkCam = pkContext->m_pkCamera;

        if (m_bCloning)
        {
            // if we are cloning, draw a ghost of the new objects
            GhostRenderingMode->Begin(pmRenderingContext);
            CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
            while (en.MoveNext())
            {
                EntityArray amEntities = en.Current;
                for (int i = 0; i < amEntities->Length; ++i)
                {
                    MEntity* pmEntity = amEntities[i];
                    GhostRenderingMode->Render(pmEntity, pmRenderingContext);
                }
            }
            GhostRenderingMode->End(pmRenderingContext);
        }

        //clear the z-buffer
        pkContext->m_pkRenderer->ClearBuffer(NULL, NiRenderer::CLEAR_ZBUFFER);

        SetGizmoScale(pkCam);
        NiDrawScene(pkCam, m_pkGizmo, *pkContext->m_pkCullingProcess);
    }
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::MouseDown(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    ENSURE_GIZMO_LOADED();
    if (!m_pkGizmo)
        return;

    if (eType == MouseButtonType::LeftButton)
    {
        NiPoint3 kOrigin, kDir;
        NiViewMath::MouseToRay((float) iX, (float) iY, 
            MFramework::Instance->ViewportManager->ActiveViewport->Width,
            MFramework::Instance->ViewportManager->ActiveViewport->Height,
            MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera(), kOrigin, kDir);

        SetGizmoScale(MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera());
        m_pkPick->SetTarget(m_pkGizmo);
        if ((!CanTransform()) || 
            (!m_pkPick->PickObjects(kOrigin, kDir, false)))
        {
            // pass the call off to selection mode, it handles if we are or
            // aren't clicking on an object
            __super::MouseDown(eType, iX, iY);
        }

        if (CanTransform())
        {
            // set gizmo scale appropriately
            NiCamera* pkCam = MFramework::Instance->ViewportManager->
                ActiveViewport->GetNiCamera();
            SetGizmoScale(pkCam);

            m_fStartDistance = GetDistance((float)iX, (float)iY);
            // record the initial scales
            MEntity* amEntities[] = SelectionService->GetSelectedEntities();
            for (int i = 0; i < amEntities->Count; i++)
            {
                if (amEntities[i] != NULL &&
                    amEntities[i]->HasProperty(*m_pkTranslationName) &&
                    amEntities[i]->HasProperty(*m_pkScaleName))
                {
                    float fCurrentScale = 0.0f;
                    NiEntityInterface* pkEntity = amEntities[i]->
                        GetNiEntityInterface();
                    pkEntity->GetPropertyData(*m_pkScaleName, fCurrentScale);
                    m_pkInitialScales->Add(fCurrentScale);

                    bool bUnique;
                    NIVERIFY(pkEntity->IsPropertyUnique(*m_pkScaleName,
                        bUnique));
                    m_pkInitialScalesUnique->Add(bUnique);
                    
                    NiPoint3 kCurrentTranslation(NiPoint3::ZERO);
                    pkEntity->GetPropertyData(*m_pkTranslationName,
                        kCurrentTranslation);
                    m_pkInitialTranslations->Add(kCurrentTranslation);

                    NIVERIFY(pkEntity->IsPropertyUnique(*m_pkTranslationName,
                        bUnique));
                    m_pkInitialTranslationsUnique->Add(bUnique);
                }
            }

            if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
            {
                MFramework* FW = MFramework::Instance;

                // if the user is holding shift, we need to clone the 
                // selection

                // First, narrow down the selection to only those entities
                // that can be added to the scene.

                List<MEntity*>* pmSelectedEntities = new List<MEntity*>(
                    SelectionService->NumSelectedEntities);
                MEntity* amSelectedEntities[] = SelectionService->
                    GetSelectedEntities();
                for (int i = 0; i < amSelectedEntities->Count; i++)
                {
                    bool bDoNotAdd = false;
                    FW->EventManager->RaiseRequestAddCloneToScene(
                        FW->Scene, amSelectedEntities[i], &bDoNotAdd);
                    if (!bDoNotAdd)
                    {
                        pmSelectedEntities->Add(amSelectedEntities[i]);
                    }
                }

                // Iterate over surviving entities, cloning each.
                List<MEntity*>::Enumerator mEnum =
                    pmSelectedEntities->GetEnumerator();
                while (mEnum.MoveNext())
                {
                    m_pmPreviousSelection->Add(mEnum.Current);
                    EntityArray amClones = mEnum.Current->Clone(
                        mEnum.Current->Name, false);
                    m_pmCloneArray->Add(amClones);
                    for (int i = 0; i < amClones->Length; ++i)
                    {
                        amClones[i]->Update(
                            FW->TimeManager->CurrentTime, 
                            FW->ExternalAssetManager);
                    }

                    // Add the default lights to the clones so we can see them.
                    FW->LightManager->AddEntitiesToDefaultLights(amClones);
                }
                SelectionService->SelectionCenter->ToNiPoint3(
                    *m_pkCloneCenter);
                SelectionService->ClearSelectedEntities();
                m_bCloning = true;
            }
            else
            {
                m_pmPreviousSelection->Clear();
                m_pmCloneArray->Clear();
                m_bCloning = false;
            }
            m_fInitialGizmoScale = m_pkGizmo->GetScale();
            m_fCurrentScale = m_fInitialGizmoScale;
            m_bAlreadyScaling = true;
        }
    }
    else
    {
        if ((m_bAlreadyScaling) && (eType == MouseButtonType::RightButton))
        {
            // if the user right-click cancels, reset scale to the original
            if (m_bCloning)
            {
                SelectionService->AddEntitiesToSelection(static_cast<
                    MEntity*[]>(m_pmPreviousSelection->ToArray(__typeof(
                    MEntity))));
                // detach the clones from scene lights
                CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
                while (en.MoveNext())
                {
                    EntityArray amEntities = en.Current;
                    MFramework::Instance->LightManager->
                        RemoveEntitiesFromDefaultLights(amEntities);
                    for (int i = 0; i < amEntities->Length; i++)
                    {
                        MFramework::Instance->EntityFactory->Remove(
                            amEntities[i]->GetNiEntityInterface());
                    }
                }
                m_pmPreviousSelection->Clear();
                m_pmCloneArray->Clear();
                MFramework::Instance->Scene->UpdateEffects();
                m_bCloning = false;
            }
            else
            {
                MEntity* amEntities[] = SelectionService->
                    GetSelectedEntities();
                unsigned int uiIndex = 0;
                for (int i = 0; i < amEntities->Count; i++)
                {
                    if (amEntities[i] != NULL &&
                        amEntities[i]->HasProperty(*m_pkTranslationName) &&
                        amEntities[i]->HasProperty(*m_pkScaleName))
                    {
                        NiEntityInterface* pkEntity = amEntities[i]->
                            GetNiEntityInterface();
                        if (!amEntities[i]->IsPropertyReadOnly(*m_pkScaleName))
                        {
                            pkEntity->SetPropertyData(*m_pkScaleName, 
                                m_pkInitialScales->GetAt(uiIndex));

                            if (!m_pkInitialScalesUnique->GetAt(uiIndex))
                            {
                                pkEntity->ResetProperty(*m_pkScaleName);
                            }
                        }
                        if (!amEntities[i]->IsPropertyReadOnly(
                            *m_pkTranslationName))
                        {
                            pkEntity->SetPropertyData(*m_pkTranslationName,
                                m_pkInitialTranslations->GetAt(uiIndex));

                            if (!m_pkInitialTranslationsUnique->GetAt(uiIndex))
                            {
                                pkEntity->ResetProperty(*m_pkTranslationName);
                            }
                        }
                        uiIndex++;
                    }
                }
            }
            m_pkInitialScales->RemoveAll();
            m_pkInitialScalesUnique->RemoveAll();
            m_pkInitialTranslations->RemoveAll();
            m_pkInitialTranslationsUnique->RemoveAll();
            m_bAlreadyScaling = false;
        }
        else
        {
            // if a button other than the left was clicked, just let the super
            // class deal with it - probably viewport movement
            __super::MouseDown(eType, iX, iY);
        }
    }
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if ((eType == MouseButtonType::LeftButton) && (m_bAlreadyScaling))
    {
        // stop transforming object
        m_bAlreadyScaling = false;

        // commit transform to properties if we have moved a certain distance
        float fDistanceDelta;
        fDistanceDelta = m_fStartDistance - GetDistance((float)iX, (float)iY);

        if (m_bCloning)
        {
            CommandService->BeginUndoFrame("Placement: Clone and scale "
                "selection");
            MScene* pmScene = MFramework::Instance->Scene;
            CloneArray::Enumerator en = m_pmCloneArray->GetEnumerator();
            while (en.MoveNext())
            {
                EntityArray amEntities = en.Current;
                // we must assign each clone a unique name before adding
                amEntities[0]->Name = 
                    pmScene->GetUniqueEntityName(amEntities[0]->Name);

                // detach the default lights and let the scene handle adding 
                // appropriate ones
                MFramework::Instance->LightManager->
                    RemoveEntitiesFromDefaultLights(amEntities);
                for (int i = 0; i < amEntities->Length; i++)
                {
                    MEntity* pmClone = amEntities[i];

                    // If the entity is a light, we must prepare its affected
                    // entities before adding.
                    if (MLightManager::EntityIsLight(pmClone))
                    {
                        PrepareClonedLight(pmClone);
                    }

                    // Add entity.
                    pmScene->AddEntity(pmClone, true);
                }
                SelectionService->AddEntityToSelection(amEntities[0]);
            }
            CommandService->EndUndoFrame(true);
            m_pmPreviousSelection->Clear();
            m_pmCloneArray->Clear();
            m_bCloning = false;
        }
        else if ((fDistanceDelta < -0.01f) || (fDistanceDelta > 0.01f))
        {
            CommandService->BeginUndoFrame("Placement: Scale selected "
                "entities");
            MEntity* amEntities[] = SelectionService->GetSelectedEntities();
            unsigned int uiIndex = 0;
            for (int i = 0; i < amEntities->Count; i++)
            {
                if (amEntities[i] != NULL &&
                    amEntities[i]->HasProperty(*m_pkTranslationName) &&
                    amEntities[i]->HasProperty(*m_pkScaleName))
                {
                    // in order to set up the undo command correctly, the
                    // property state must be at the original location before
                    // we commit the change to the entity, so we set the scale
                    // so the original before setting the new one
                    NiEntityInterface* pkEntity = amEntities[i]->
                        GetNiEntityInterface();

                    if (!amEntities[i]->IsPropertyReadOnly(*m_pkScaleName))
                    {
                        float fScale;
                        pkEntity->GetPropertyData(*m_pkScaleName, fScale);

                        pkEntity->SetPropertyData(*m_pkScaleName, 
                            m_pkInitialScales->GetAt(uiIndex));

                        if (!m_pkInitialScalesUnique->GetAt(uiIndex))
                        {
                            pkEntity->ResetProperty(*m_pkScaleName);
                        }

                        amEntities[i]->SetPropertyData(*m_pkScaleName, 
                            __box(fScale), true);
                    }

                    if (!amEntities[i]->IsPropertyReadOnly(
                        *m_pkTranslationName))
                    {
                        NiPoint3 kTranslation;
                        pkEntity->GetPropertyData(*m_pkTranslationName,
                            kTranslation);

                        pkEntity->SetPropertyData(*m_pkTranslationName,
                            m_pkInitialTranslations->GetAt(uiIndex));

                        if (!m_pkInitialTranslationsUnique->GetAt(uiIndex))
                        {
                            pkEntity->ResetProperty(*m_pkTranslationName);
                        }

                        amEntities[i]->SetPropertyData(*m_pkTranslationName,
                            new MPoint3(kTranslation), true);
                    }

                    uiIndex++;
                }
            }
            CommandService->EndUndoFrame(true);

        }
        // make sure that our gizmo is updated
        MouseMove(iX, iY);
        m_pkInitialScales->RemoveAll();
        m_pkInitialScalesUnique->RemoveAll();
        m_pkInitialTranslations->RemoveAll();
        m_pkInitialTranslationsUnique->RemoveAll();
    }

    // make sure that super class handles its mouseup
    __super::MouseUp(eType, iX, iY);
}
//---------------------------------------------------------------------------
void MScaleInteractionMode::MouseMove(int iX, int iY)
{
    MVerifyValidInstance;

    if (m_bAlreadyScaling)
    {
        //transform
        ScaleHelper((float)iX, (float)iY);
    }
    else
    {
        // allow camera movement
        __super::MouseMove(iX, iY);
    }
}
//---------------------------------------------------------------------------
