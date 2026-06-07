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

#include "MGridManager.h"
#include "MSettingsHelper.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

//---------------------------------------------------------------------------
MGridManager::MGridManager() : m_fSnapSpacing(1.0f), m_iSnapsPerLine(10), 
    m_iExtent(20), m_iMajorLines(10), m_bDisplayGrid(true),
    m_pspGridComponents(NULL), m_uiNumGridComponents(0)
{
    __hook(&MEventManager::ViewportCameraChanged,
        MFramework::Instance->EventManager,
        &MGridManager::OnViewportCameraChanged);

    m_pkColor = NiNew NiColor(0.3f, 0.3f, 0.3f);
    m_pkMajorColor = NiNew NiColor(0.0f, 0.0f, 0.0f);
}
//---------------------------------------------------------------------------
void MGridManager::Do_Dispose(bool bDisposing)
{
    NiDelete[] m_pspGridComponents;
    NiDelete m_pkColor;
    NiDelete m_pkMajorColor;

    if (bDisposing)
    {
        __unhook(&MEventManager::ViewportCameraChanged,
            MFramework::Instance->EventManager,
            &MGridManager::OnViewportCameraChanged);
    }
}
//---------------------------------------------------------------------------
void MGridManager::RegisterSettings()
{
    // register the event handler for changed settings
    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MGridManager::OnSettingChanged);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::TRANSLATION_SNAP,
        m_fSnapSpacing, pmHandler);

    // first register the setting with the service if it doesn't already exist
    SettingsService->RegisterSettingsObject("Snaps per Grid Line", 
        __box(m_iSnapsPerLine), SettingsCategory::PerScene);
    SettingsService->RegisterSettingsObject("Grid Color",
        __box(Color::FromArgb(255, MUtility::FloatToRGB(m_pkColor->r), 
        MUtility::FloatToRGB(m_pkColor->g), MUtility::FloatToRGB(m_pkColor->b)
        )), SettingsCategory::PerUser);
    SettingsService->RegisterSettingsObject("Major Lines Color",
        __box(Color::FromArgb(255, MUtility::FloatToRGB(m_pkMajorColor->r), 
        MUtility::FloatToRGB(m_pkMajorColor->g), 
        MUtility::FloatToRGB(m_pkMajorColor->b))), SettingsCategory::PerUser);
    SettingsService->RegisterSettingsObject("Grid Extent", __box(m_iExtent),
        SettingsCategory::PerUser);
    SettingsService->RegisterSettingsObject("Lines per Major Line", 
        __box(m_iMajorLines), SettingsCategory::PerUser);
    SettingsService->RegisterSettingsObject("Display Grid",
        __box(m_bDisplayGrid), SettingsCategory::PerScene);

    // set the event handler for when that setting changes
    SettingsService->SetChangedSettingHandler("Snaps per Grid Line", 
        SettingsCategory::PerScene, pmHandler);
    SettingsService->SetChangedSettingHandler("Grid Color",
        SettingsCategory::PerUser, pmHandler);
    SettingsService->SetChangedSettingHandler("Major Lines Color",
        SettingsCategory::PerUser, pmHandler);
    SettingsService->SetChangedSettingHandler("Grid Extent",
        SettingsCategory::PerUser, pmHandler);
    SettingsService->SetChangedSettingHandler("Lines per Major Line",
        SettingsCategory::PerUser, pmHandler);
    SettingsService->SetChangedSettingHandler("Display Grid",
        SettingsCategory::PerScene, pmHandler);

    // create an option in the settings dialog for the settings
    OptionsService->AddOption("Grid.Snaps per Grid Line", 
        SettingsCategory::PerScene, "Snaps per Grid Line");
    OptionsService->AddOption("Grid.Grid Color", SettingsCategory::PerUser,
        "Grid Color");
    OptionsService->AddOption("Grid.Major Lines Color", 
        SettingsCategory::PerUser, "Major Lines Color");
    OptionsService->AddOption("Grid.Grid Extent", SettingsCategory::PerUser,
        "Grid Extent");
    OptionsService->AddOption("Grid.Lines per Major Line", 
        SettingsCategory::PerUser, "Lines per Major Line");
    OptionsService->AddOption("Grid.Display Grid", SettingsCategory::PerScene,
        "Display Grid");

    // add descriptions to each option
    OptionsService->SetHelpDescription("Grid.Snaps per Grid Line", 
        "How many snap increments between each grid line.");
    OptionsService->SetHelpDescription("Grid.Grid Color", 
        "The color of the grid lines.");
    OptionsService->SetHelpDescription("Grid.Major Lines Color", 
        "The color of the major grid lines.");
    OptionsService->SetHelpDescription("Grid.Grid Extent", 
        "How many grid lines from the center to the end of the grid.");
    OptionsService->SetHelpDescription("Grid.Lines per Major Line", 
        "How many grid lines are between each major grid line. The minimum "
        "value for this option is 2.");
    OptionsService->SetHelpDescription("Grid.Display Grid", "Whether or not "
        "the grid should be displayed in the viewports. This setting is "
        "saved per-scene.");

    // get the value of the setting in case it existed before and update grid
    Object* pmObj;
    pmObj = SettingsService->GetSettingsObject("Snaps per Grid Line", 
        SettingsCategory::PerScene);
    __box int* piVal = dynamic_cast<__box int*>(pmObj);
    if (piVal != NULL)
    {
        m_iSnapsPerLine = *piVal;
    }
    __box Color* pmValue;
    pmObj = SettingsService->GetSettingsObject("Grid Color", 
        SettingsCategory::PerUser);
    pmValue = dynamic_cast<__box Color*>(pmObj);
    if (pmValue != NULL)
    {
        m_pkColor->r = MUtility::RGBToFloat((*pmValue).R);
        m_pkColor->g = MUtility::RGBToFloat((*pmValue).G);
        m_pkColor->b = MUtility::RGBToFloat((*pmValue).B);
    }
    pmObj = SettingsService->GetSettingsObject("Major Lines Color",
        SettingsCategory::PerUser);
    pmValue = dynamic_cast<__box Color*>(pmObj);
    if (pmValue != NULL)
    {
        m_pkMajorColor->r = MUtility::RGBToFloat((*pmValue).R);
        m_pkMajorColor->g = MUtility::RGBToFloat((*pmValue).G);
        m_pkMajorColor->b = MUtility::RGBToFloat((*pmValue).B);
    }
    pmObj = SettingsService->GetSettingsObject("Grid Extent", 
        SettingsCategory::PerUser);
    piVal = dynamic_cast<__box int*>(pmObj);
    if (piVal != NULL)
    {
        m_iExtent = *piVal;
    }
    pmObj = SettingsService->GetSettingsObject("Lines per Major Line", 
        SettingsCategory::PerUser);
    piVal = dynamic_cast<__box int*>(pmObj);
    if (piVal != NULL)
    {
        m_iMajorLines = *piVal;
    }
    pmObj = SettingsService->GetSettingsObject("Display Grid",
        SettingsCategory::PerScene);
    __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
    if (pbVal != NULL)
    {
        m_bDisplayGrid = *pbVal;
    }

    // use the gotten settings to set the true grid spacing
    for (unsigned int ui = 0; ui < m_uiNumGridComponents; ui++)
    {
        m_pspGridComponents[ui]->SetGridSpacing(m_fSnapSpacing *
            m_iSnapsPerLine);
        m_pspGridComponents[ui]->SetGridColor(*m_pkColor);
        m_pspGridComponents[ui]->SetMajorColor(*m_pkMajorColor);
        m_pspGridComponents[ui]->SetGridExtent(m_iExtent);
        m_pspGridComponents[ui]->SetMajorLineSpacing(m_iMajorLines);
        m_pspGridComponents[ui]->SetDisplayGrid(m_bDisplayGrid);
    }
}
//---------------------------------------------------------------------------
void MGridManager::OnSettingChanged(Object*,
    SettingChangedEventArgs* pmEventArgs)
{
    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    // if a setting we care about was changed, update the Grid
    bool bSpacingChanged = false;

    if ((strSetting->Equals("Snaps per Grid Line")) && 
        (eCategory == SettingsCategory::PerScene))
    {
        bSpacingChanged = true;
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            "Snaps per Grid Line", eCategory);
        __box int* piVal = dynamic_cast<__box int*>(pmObj);
        if (piVal != NULL)
        {
            m_iSnapsPerLine = *piVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs,
        MSettingsHelper::TRANSLATION_SNAP))
    {
        bSpacingChanged = true;
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fSnapSpacing = *pfVal;
        }
    }
    else if ((strSetting->Equals("Grid Color")) && 
        (eCategory == SettingsCategory::PerUser))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box Color* pmValue;
        pmValue = dynamic_cast<__box Color*>(pmObj);
        if (pmValue != NULL)
        {
            m_pkColor->r = MUtility::RGBToFloat((*pmValue).R);
            m_pkColor->g = MUtility::RGBToFloat((*pmValue).G);
            m_pkColor->b = MUtility::RGBToFloat((*pmValue).B);
            for (unsigned int ui = 0; ui < m_uiNumGridComponents; ui++)
            {
                m_pspGridComponents[ui]->SetGridColor(*m_pkColor);
            }
        }
    }
    else if ((strSetting->Equals("Major Lines Color")) && 
        (eCategory == SettingsCategory::PerUser))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box Color* pmValue;
        pmValue = dynamic_cast<__box Color*>(pmObj);
        if (pmValue != NULL)
        {
            m_pkMajorColor->r = MUtility::RGBToFloat((*pmValue).R);
            m_pkMajorColor->g = MUtility::RGBToFloat((*pmValue).G);
            m_pkMajorColor->b = MUtility::RGBToFloat((*pmValue).B);
            for (unsigned int ui = 0; ui < m_uiNumGridComponents; ui++)
            {
                m_pspGridComponents[ui]->SetMajorColor(*m_pkMajorColor);
            }
        }
    }
    if ((strSetting->Equals("Grid Extent")) && 
        (eCategory == SettingsCategory::PerUser))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box int* piVal = dynamic_cast<__box int*>(pmObj);
        if (piVal != NULL)
        {
            m_iExtent = *piVal;
            for (unsigned int ui = 0; ui < m_uiNumGridComponents; ui++)
            {
                m_pspGridComponents[ui]->SetGridExtent(m_iExtent);
            }
        }
    }
    if ((strSetting->Equals("Lines per Major Line")) && 
        (eCategory == SettingsCategory::PerUser))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box int* piVal = dynamic_cast<__box int*>(pmObj);
        if (piVal != NULL)
        {
            m_iMajorLines = *piVal;
            // The minumum value for this setting is 2, so clamp it here.
            if (m_iMajorLines < 2)
            {
                int iMajorLines = 2;
                SettingsService->SetSettingsObject("Lines per Major Line",
                    __box(iMajorLines), SettingsCategory::PerUser);
                return;
            }
            for (unsigned int ui = 0; ui < m_uiNumGridComponents; ui++)
            {
                m_pspGridComponents[ui]->SetMajorLineSpacing(m_iMajorLines);
            }
        }
    }
    if (strSetting->Equals("Display Grid") &&
        eCategory == SettingsCategory::PerScene)
    {
        Object* pmObj = SettingsService->GetSettingsObject(strSetting,
            eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bDisplayGrid = *pbVal;
            for (unsigned int ui = 0; ui < m_uiNumGridComponents; ui++)
            {
                m_pspGridComponents[ui]->SetDisplayGrid(m_bDisplayGrid);
            }
        }
    }

    if (bSpacingChanged)
    {
        for (unsigned int ui = 0; ui < m_uiNumGridComponents; ui++)
        {
            m_pspGridComponents[ui]->SetGridSpacing(m_fSnapSpacing *
                m_iSnapsPerLine);
        }
    }
}
//---------------------------------------------------------------------------
void MGridManager::OnViewportCameraChanged(MViewport* pmViewport,
    MEntity* pmCamera)
{
    MViewportManager* pmViewportManager = MFramework::Instance
        ->ViewportManager;
    unsigned int uiViewportIndex;
    for (uiViewportIndex = 0;
        uiViewportIndex < pmViewportManager->ViewportCount; uiViewportIndex++)
    {
        if (pmViewport == pmViewportManager->GetViewport(uiViewportIndex))
        {
            break;
        }
    }
    MAssert(uiViewportIndex != pmViewportManager->ViewportCount, "Viewport "
        "not in viewport manager!");
    
    // if camera is one of MCameraManager's default ortho cams, make dynamic
    MCameraManager* pmCameraManager = MFramework::Instance->CameraManager;
    if (pmCameraManager->IsAxisAlignedStandardCamera(pmViewport, pmCamera))
    {
        m_pspGridComponents[uiViewportIndex]->SetStatic(false);
    }
    else
    {
        m_pspGridComponents[uiViewportIndex]->SetStatic(true);
    }
    m_pspGridComponents[uiViewportIndex]->RebuildInvalidGrid();
}
//---------------------------------------------------------------------------
void MGridManager::CreateGridEntity()
{
    NiFixedString kGridComponentName = "Grid";

    NiUniqueID kTemplateID;
    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiEntityInterface* pkEntity = NiNew NiGeneralEntity(kGridComponentName,
        kTemplateID, 1);

    NiGridComponent* pkGridComponent = NiNew NiGridComponent();
    NIVERIFY(pkEntity->AddComponent(pkGridComponent));

    MEntity* pmGrid = MFramework::Instance->EntityFactory->Get(pkEntity);

    MViewportManager* pmViewportManager = MFramework::Instance
        ->ViewportManager;
    unsigned int uiViewportCount = pmViewportManager->ViewportCount;
    m_pspGridComponents = NiNew NiGridComponentPtr[uiViewportCount];
    m_uiNumGridComponents = uiViewportCount;
    for (unsigned int ui = 0; ui < uiViewportCount; ui++)
    {
        MEntity* pmClone;
        if (ui == 0)
        {
            pmClone = pmGrid;
        }
        else
        {
            MEntity* amEntities[] = pmGrid->Clone(pmGrid->Name, false);
            NIASSERT(amEntities->Length == 1);
            pmClone = amEntities[0];
        }

        MViewport* pmViewport = pmViewportManager->GetViewport(ui);
        pmViewport->ToolScene->AddEntity(pmClone, false);

        NiEntityComponentInterface* pkComponent = pmClone
            ->GetNiEntityInterface()->GetComponentByTemplateID(
            pkGridComponent->GetTemplateID());
        MAssert(pkComponent != NULL, "Grid component not found on clone!");

        m_pspGridComponents[ui] = (NiGridComponent*) pkComponent;
    }
}
//---------------------------------------------------------------------------
ISettingsService* MGridManager::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found.");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MGridManager::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not found.");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
