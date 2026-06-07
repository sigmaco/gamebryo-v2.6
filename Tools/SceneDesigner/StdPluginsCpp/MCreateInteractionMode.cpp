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

#include "MCreateInteractionMode.h"
#include "NiVirtualBoolBugWrapper.h"
#include "NiTransformationComponent.h"
#include "MSettingsHelper.h"

using namespace System::Collections::Generic;
using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MCreateInteractionMode::MCreateInteractionMode() : m_pkPlaneNormal(NULL)
{
    m_pkPlaneNormal = NiNew NiPoint3(0.0f, 0.0f, 1.0f);
    m_pmCursor = Cursors::Default;
    m_pmTmpCursor = NULL;
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::RegisterSettings()
{
    // register the settings of all the classes we inherit from 
    __super::RegisterSettings();

    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MCreateInteractionMode::OnSettingChanged);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::TRANSLATION_SNAP,
        m_fSnapSpacing, pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_SNAP_ENABLED, m_bSnapEnabled, pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_PRECISION, m_fPrecision, pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED, m_bPrecisionEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::SNAP_TO_SURFACE_ENABLED, m_bSnapToPickEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::ALIGN_TO_SURFACE_ENABLED, m_bRotateToPickEnabled, 
        pmHandler);
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::OnSettingChanged(Object*,
    SettingChangedEventArgs* pmEventArgs)
{
    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    // if a setting we care about was changed, re-cache it
    if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_SNAP))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fSnapSpacing = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_SNAP_ENABLED))
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
        MSettingsHelper::TRANSLATION_PRECISION))
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
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED))
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
        MSettingsHelper::SNAP_TO_SURFACE_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bSnapToPickEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::ALIGN_TO_SURFACE_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bRotateToPickEnabled = *pbVal;
        }
    }
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::Do_Dispose(bool bDisposing)
{
    NiDelete m_pkPlaneNormal;
    __super::Do_Dispose(bDisposing);
}
//---------------------------------------------------------------------------
String* MCreateInteractionMode::get_Name()
{
    return "GamebryoCreate";
}
//---------------------------------------------------------------------------
Cursor* MCreateInteractionMode::get_MouseCursor()
{
    return m_pmCursor;
}
//---------------------------------------------------------------------------
bool MCreateInteractionMode::Initialize()
{
    MVerifyValidInstance;

    m_bAlreadyDragging = false;
    m_bSnapEnabled = true;
    m_fSnapSpacing = 1.0f;
    m_fPrecision = 0.1f;
    m_bPrecisionEnabled = true;
    m_bSnapToPickEnabled = false;
    m_bRotateToPickEnabled = false;
    m_usAlignFacingAxis = 2;
    m_usAlignUpAxis = 1;

    String* strFilename = String::Concat(MFramework::Instance->AppStartupPath,
        "Data\\create.cur");
    m_pmCursor = new System::Windows::Forms::Cursor(strFilename);
    m_pmTmpCursor = m_pmCursor;

    return true;
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::SetInteractionMode(Object*, 
    EventArgs*)
{
    MVerifyValidInstance;

    if (MFramework::Instance->LayerManager->ActiveLayer == NULL)
    {
        MessageService->AddMessage(MessageChannelType::Errors, new Emergent::
            Gamebryo::SceneDesigner::PluginAPI::Message("Cannot set Create "
            "Interaction Mode.", "There is no active layer specified to which "
            "to add new entities.", NULL));
        return;
    }

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
void MCreateInteractionMode::ValidateInteractionMode(Object*,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
    pmState->Enabled = (MFramework::Instance->LayerManager->ActiveLayer !=
        NULL);
    if (pmState->Checked && !pmState->Enabled)
    {
        pmState->Checked = false;
        InteractionModeService->ActiveMode = m_pmPreviousIM;
    }
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::CreateHelper(const float fX, const float fY)
{
    MVerifyValidInstance;

    // this method is called when the user is dragging - we must translate
    // the entity until it is in place.
    NiPoint3 kOrigin, kDir;

    NiViewMath::MouseToRay(fX, fY,
        MFramework::Instance->ViewportManager->ActiveViewport->Width,
        MFramework::Instance->ViewportManager->ActiveViewport->Height,
        MFramework::Instance->ViewportManager->ActiveViewport->GetNiCamera(),
        kOrigin, kDir);
    NiPoint3 kTranslation;
    NiPoint3 kDelta;
    NiEntityInterface* pkEntity = 
        m_pmNewEntity[0]->GetNiEntityInterface();
    pkEntity->GetPropertyData(*m_pkTranslationName, kTranslation);
    kDelta = NiViewMath::TranslateOnPlane(kTranslation, *m_pkPlaneNormal, 
        kOrigin, kDir);
    kTranslation += kDelta;
    if (m_bSnapEnabled)
    {
        kTranslation /= m_fSnapSpacing;
        kTranslation.x = NiFloor(kTranslation.x + 0.5f);
        kTranslation.y = NiFloor(kTranslation.y + 0.5f);
        kTranslation.z = NiFloor(kTranslation.z + 0.5f);
        kTranslation *= m_fSnapSpacing;
    }
    if (m_bSnapToPickEnabled)
    {
        // perform a pick, and use the delta between that and the starting pt
        if (MFramework::Instance->PickUtility->PerformPick(
            MFramework::Instance->Scene, kOrigin, kDir, true))
        {
            const NiPick* pkPick;
            pkPick = MFramework::Instance->PickUtility->GetNiPick();
            const NiPick::Results& kPickResults = pkPick->GetResults();
            NiPick::Record* pkPickRecord;
            pkPickRecord = kPickResults.GetAt(0);
            if (pkPickRecord)
            {
                kTranslation = pkPickRecord->GetIntersection();
                if (m_bRotateToPickEnabled)
                {
                    NiPoint3 kNormal;
                    NiPoint3 kUpAxis;
                    NiMatrix3 kRotation;

                    kNormal = pkPickRecord->GetNormal();
                    kUpAxis = *m_pkUpAxis;
                    // first check if model's up and facing axis are parallel
                    if ((m_usAlignUpAxis - m_usAlignFacingAxis) % 3 == 0)
                    {
                        m_usAlignUpAxis += 1;
                    }
                    if ((kNormal.Dot(kUpAxis) > NiViewMath::PARALLEL_THRESHOLD)
                        || (kNormal.Dot(kUpAxis) < 
                        -NiViewMath::PARALLEL_THRESHOLD))
                    {
                        if ((kNormal.Dot(NiPoint3::UNIT_Z) < 
                            NiViewMath::PARALLEL_THRESHOLD) &&
                            (kNormal.Dot(NiPoint3::UNIT_Z) > 
                            -NiViewMath::PARALLEL_THRESHOLD))
                        {
                            kUpAxis = NiPoint3::UNIT_Z;
                        }
                        else if ((kNormal.Dot(NiPoint3::UNIT_Y) < 
                            NiViewMath::PARALLEL_THRESHOLD) &&
                            (kNormal.Dot(NiPoint3::UNIT_Y) >
                            -NiViewMath::PARALLEL_THRESHOLD))
                        {
                            kUpAxis = NiPoint3::UNIT_Y;
                        }
                        else
                        {
                            kUpAxis = NiPoint3::UNIT_X;
                        }
                    }
                    while (m_usAlignFacingAxis > 5)
                        m_usAlignFacingAxis -= 6;
                    while (m_usAlignUpAxis > 5)
                        m_usAlignUpAxis -= 6;
                    if (m_usAlignFacingAxis > 2)
                    {
                        m_usAlignFacingAxis -= 3;
                        kNormal = -kNormal;
                    }
                    if (m_usAlignUpAxis > 2)
                    {
                        m_usAlignUpAxis -= 3;
                        kUpAxis = -kUpAxis;
                    }

                    // calculate rotation matrix
                    NiPoint3 kFrameX;
                    NiPoint3 kFrameY;
                    NiPoint3 kFrameZ;
                    if (m_usAlignFacingAxis == 0)
                    {
                        if (m_usAlignUpAxis == 1)
                        {
                            // we want X axis to face N and Y to face up
                            kFrameX = kNormal;
                            kFrameZ = kFrameX.Cross(kUpAxis);
                            kFrameY = kFrameZ.Cross(kFrameX);
                        }
                        else
                        {
                            // we want X axis to face N and Z to face up
                            kFrameX = kNormal;
                            kFrameY = kUpAxis.Cross(kFrameX);
                            kFrameZ = kFrameX.Cross(kFrameY);
                        }
                    }
                    else if (m_usAlignFacingAxis == 1)
                    {
                        if (m_usAlignUpAxis == 0)
                        {
                            // we want Y axis to face N and X to face up
                            kFrameY = kNormal;
                            kFrameZ = kUpAxis.Cross(kFrameY);
                            kFrameX = kFrameY.Cross(kFrameZ);
                        }
                        else
                        {
                            // we want Y axis to face N and Z to face up
                            kFrameY = kNormal;
                            kFrameX = kFrameY.Cross(kUpAxis);
                            kFrameZ = kFrameX.Cross(kFrameY);
                        }
                    }
                    else
                    {
                        if (m_usAlignUpAxis == 0)
                        {
                            // we want Z axis to face N and X to face up
                            kFrameZ = kNormal;
                            kFrameY = kFrameZ.Cross(kUpAxis);
                            kFrameX = kFrameY.Cross(kFrameZ);
                        }
                        else
                        {
                            // we want Z axis to face N and Y to face up
                            kFrameZ = kNormal;
                            kFrameX = kUpAxis.Cross(kFrameZ);
                            kFrameY = kFrameZ.Cross(kFrameX);
                        }
                    }
                    kRotation.SetCol(0, kFrameX);
                    kRotation.SetCol(1, kFrameY);
                    kRotation.SetCol(2, kFrameZ);
                    kRotation.Reorthogonalize();

                    pkEntity->SetPropertyData(*m_pkRotationName, kRotation);
                }
            }
        }
        else
        {
            pkEntity->SetPropertyData(*m_pkRotationName, NiMatrix3::IDENTITY);
        }
    }
    if (m_bPrecisionEnabled)
    {
        kTranslation /= m_fPrecision;
        kTranslation.x = NiFloor(kTranslation.x + 0.5f);
        kTranslation.y = NiFloor(kTranslation.y + 0.5f);
        kTranslation.z = NiFloor(kTranslation.z + 0.5f);
        kTranslation *= m_fPrecision;
    }
    pkEntity->SetPropertyData(*m_pkTranslationName, kTranslation);
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::GetBestPlane(const NiPoint3* pkLook)
{
    float fCosine;

    *m_pkPlaneNormal = *m_pkUpAxis;
    fCosine = pkLook->Dot(*m_pkPlaneNormal);

    if ((fCosine <= NiViewMath::INV_PARALLEL_THRESHOLD) &&
        (fCosine >= -NiViewMath::INV_PARALLEL_THRESHOLD))
    {
        // the default up-axis is no good - find another one
        // we want the axis who's normal is closest to the look vector
        float fXBias;
        float fYBias;
        float fZBias;
        fXBias = NiAbs(pkLook->Dot(NiPoint3::UNIT_X));
        fYBias = NiAbs(pkLook->Dot(NiPoint3::UNIT_Y));
        fZBias = NiAbs(pkLook->Dot(NiPoint3::UNIT_Z));

        if ((fXBias >= fYBias) && (fXBias >= fZBias))
        {
            *m_pkPlaneNormal = NiPoint3::UNIT_X;
        }
        else if (fYBias >= fZBias)
        {
            *m_pkPlaneNormal = NiPoint3::UNIT_Y;
        }
        else
        {
            *m_pkPlaneNormal = NiPoint3::UNIT_Y;
        }
    }
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::Update(float fTime)
{
    MVerifyValidInstance;

    __super::Update(fTime);

    if ((m_bAlreadyDragging) && (m_pmNewEntity != NULL))
    {
        for (int i = 0; i != m_pmNewEntity->Count; ++i)
        {
            m_pmNewEntity[i]->Update(
                MFramework::Instance->TimeManager->CurrentTime,
                MFramework::Instance->ExternalAssetManager);
        }

        if (m_pmNewProxy != NULL)
        {
            for (int i = 0; i != m_pmNewProxy->Count; ++i)
            {
                if (m_pmNewProxy[i] != NULL)
                {
                    m_pmNewProxy[i]->Update(fTime,
                        MFramework::Instance->ExternalAssetManager);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::RenderGizmo(
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    // we need to get the scene root of the active entity template and
    // display a ghost image of it if we are dragging.
    if ((m_bAlreadyDragging) && (m_pmNewEntity != NULL))
    {
        GhostRenderingMode->Begin(pmRenderingContext);

        for (int i = 0; i != m_pmNewEntity->Count; ++i)
        {
            GhostRenderingMode->Render(m_pmNewEntity[i], pmRenderingContext);
        }

        if(m_pmNewProxy != NULL)
        {
            for (int i = 0; i != m_pmNewProxy->Count; ++i)
            {
                if (m_pmNewProxy[i] != NULL)
                {
                    GhostRenderingMode->Render(m_pmNewProxy[i],
                        pmRenderingContext);
                }
            }
        }

        GhostRenderingMode->End(pmRenderingContext);
    }
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::MouseDown(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if (eType == MouseButtonType::LeftButton)
    {
        // make sure we are placing it on the ideal plane relative to the view
        NiPoint3 kLook;
        NiCamera* pkCamera;
        pkCamera = MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera();
        (pkCamera->GetRotate()).GetCol(0, kLook);
        GetBestPlane(&kLook);
        // create a cloned object
        MScene* pmScene = MFramework::Instance->Scene;
        
        MPalette* pmPalette = MFramework::Instance->PaletteManager->
            ActivePalette;
        MPrefab* pmPrefab = 
            MFramework::Instance->PrefabManager->ActivePrefab;
        if (pmPalette != NULL)
        {
            MEntity* pmTemplate = pmPalette->ActiveEntity;
            if (pmTemplate == NULL)
                return;

            bool bDoNotAdd = false;
            MFramework::Instance->EventManager->RaiseRequestAddCloneToScene(
                pmScene, pmTemplate, &bDoNotAdd);
            if (bDoNotAdd)
            {
                return;
            }

            // Process is starting - so show waitcursor
            m_pmCursor = Cursors::WaitCursor;

            String* strCloneName = pmScene->GetUniqueEntityName(
                String::Concat(
                    EntityPathService->GetSimpleName(pmTemplate->Name),
                    " 01"));
            m_pmNewEntity = pmTemplate->Clone(strCloneName, true);
            NIASSERT(m_pmNewEntity->Length == 1);
            m_pmNewEntity[0]->Update(MFramework::Instance->TimeManager->
                CurrentTime, MFramework::Instance->ExternalAssetManager);

            MFramework::Instance->LightManager->AddEntityToDefaultLights(
                m_pmNewEntity[0]);

            Guid oldID = pmTemplate->TemplateID;
            Guid newID = m_pmNewEntity[0]->TemplateID;
            MAssert(oldID == newID);
            m_pmNewProxy = new MEntity*[1]();
            m_pmNewProxy[0] = MFramework::Instance->ProxyManager->CreateProxy(
                m_pmNewEntity[0]);
        }
        else if (pmPrefab != NULL)
        {
            if (MFramework::Instance->LayerManager->
                GetLayerByName(pmPrefab->SourceFilename) != NULL)
            {
                MessageService->AddMessage(MessageChannelType::Errors, 
                    new Emergent::Gamebryo::SceneDesigner::PluginAPI::
                    Message("Cannot instantiate prefab because a layer by "
                    "that name already exists.",
                    pmPrefab->Filename, NULL));

                return;
            }

            List<String*>* pmLayerConflicts = new List<String*>();
            MEntity* amPrefabEntities[] = pmPrefab->Scene->GetEntities();
            for (int i = 0; i < amPrefabEntities->Length; ++i)
            {
                MEntity* pmPrefabEntity = amPrefabEntities[i];
                if (pmPrefabEntity->IsPrefabRoot)
                {
                    String* strPrefabPath = dynamic_cast<String*>(
                        pmPrefabEntity->GetPropertyData(
                        NiPrefabComponent::PROP_PREFAB_PATH(), 0));
                    MAssert(strPrefabPath != NULL, "Invalid Prefab Path!");
                    if (MFramework::Instance->LayerManager->GetLayerByName(
                        strPrefabPath) != NULL)
                    {
                        if (!pmLayerConflicts->Contains(strPrefabPath))
                        {
                            pmLayerConflicts->Add(strPrefabPath);
                        }
                    }
                }
            }
            if (pmLayerConflicts->Count > 0)
            {
                String* strMessage = "Cannot instantiate prefab because it "
                    "contains instances of the following layers:\n\n";
                List<String*>::Enumerator mEnum =
                    pmLayerConflicts->GetEnumerator();
                while (mEnum.MoveNext())
                {
                    strMessage = String::Concat(strMessage, mEnum.Current,
                        new String("\n"));
                }

                MessageService->AddMessage(MessageChannelType::Errors, 
                    new Emergent::Gamebryo::SceneDesigner::PluginAPI::
                    Message(strMessage, pmPrefab->Filename, NULL));
                return;
            }

            // it's a prefab
            m_pmNewEntity = pmPrefab->Instantiate();
            m_pmNewProxy = new MEntity*[m_pmNewEntity->Length]();
            MProxyManager* pmProxyManager = 
                MFramework::Instance->ProxyManager;
            for (int i = 0; i != m_pmNewEntity->Count; ++i)
            {
                m_pmNewProxy[i] = pmProxyManager->
                    CreateProxy(m_pmNewEntity[i]);
            }
            MFramework::Instance->LightManager->
                AddEntitiesToDefaultLights(m_pmNewEntity);
        }
        else
        {
            // it's not valid
            return;
        }
        m_bAlreadyDragging = true;

        // place the entity based off of the mouse location
        MouseMove(iX, iY);
    }
    else if (eType == MouseButtonType::RightButton)
    {
        m_pmCursor = m_pmTmpCursor; // Ensure that wait cursor is removed

        if (m_bAlreadyDragging)
        {
            // if the user right-click cancels, stop displaying the mesh

            if (m_pmNewEntity != NULL)
            {
                for (int i = 0; i != m_pmNewEntity->Count; ++i)
                {
                    MFramework::Instance->LightManager->
                        RemoveEntityFromDefaultLights(m_pmNewEntity[i]);
                    MFramework::Instance->EntityFactory->Remove(
                        m_pmNewEntity[i]->GetNiEntityInterface());
                }
                m_pmNewEntity = NULL;
            }
            
            if (m_pmNewProxy != NULL)
            {
                for (int i = 0; i != m_pmNewProxy->Count; ++i)
                {
                    if (m_pmNewProxy[i] != NULL)
                    {
                        MFramework::Instance->EntityFactory->Remove(
                            m_pmNewProxy[i]->GetNiEntityInterface());
                    }
                }
                m_pmNewProxy = NULL;
            }

            m_bAlreadyDragging = false;
        }
        else
        {
            // right click exit
            InteractionModeService->ActiveMode = m_pmPreviousIM;
            m_bAlreadyDragging = false;
            m_bLeftDown = false;
            m_bRightDown = false;
            m_bMiddleDown = false;
        }
    }
    else
    {
        // if the middle mouse button was clicked, just let the super
        // class deal with it - probably viewport movement
        __super::MouseDown(eType, iX, iY);
    }
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if ((eType == MouseButtonType::LeftButton) && (m_bAlreadyDragging))
    {
        // stop transforming object
        m_bAlreadyDragging = false;

        // Destroy temporary proxy geometry. New proxy geometry will be added
        // when entity is added to the scene.
        if (m_pmNewProxy != NULL)
        {
            for (int i = 0; i != m_pmNewProxy->Count; ++i)
            {
                if (m_pmNewProxy[i] != NULL)
                {
                    MFramework::Instance->EntityFactory->Remove(
                        m_pmNewProxy[i]->GetNiEntityInterface());
                }
            }
            m_pmNewProxy = NULL;
        }

        String* msg = String::Format("Add \"{0}\" prefab to \"{1}\" scene",
            m_pmNewEntity[0]->Name, MFramework::Instance->Scene->Name);
        CommandService->BeginUndoFrame(msg);
        NIASSERT(m_pmNewEntity);
        for (int i = 0; i != m_pmNewEntity->Count; ++i)
        {
            MFramework::Instance->LightManager->RemoveEntityFromDefaultLights(
                m_pmNewEntity[i]);
            // add our temporary entity to the main scene
            MFramework::Instance->Scene->AddEntity(m_pmNewEntity[i], true);
        }
        SelectionService->SelectedEntity = m_pmNewEntity[0];
        CommandService->EndUndoFrame(true);

        m_pmCursor = m_pmTmpCursor;
    }

    // make sure that super class handles its mouseup
    __super::MouseUp(eType, iX, iY);
}
//---------------------------------------------------------------------------
void MCreateInteractionMode::MouseMove(int iX, int iY)
{
    MVerifyValidInstance;

    if (m_bAlreadyDragging)
    {
        //transform
        CreateHelper((float)iX, (float)iY);
    }
    else
    {
        // allow camera movement
        __super::MouseMove(iX, iY);
    }
}
//---------------------------------------------------------------------------
