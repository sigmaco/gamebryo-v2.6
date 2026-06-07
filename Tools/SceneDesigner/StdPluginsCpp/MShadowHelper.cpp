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

#include "MShadowHelper.h"
#include "MShadowClickGeneratorConverter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
void MShadowHelper::RegisterSettings()
{
    // Register shadow click generator setting.
    ISettingsService* pmSettingsService = MGetService(ISettingsService);
    MAssert(pmSettingsService != NULL, "Settings service not found!");

    pmSettingsService->RegisterSettingsObject(
        ms_strShadowClickGeneratorSettingName,
        ms_strDefaultShadowClickGenerator,
        SettingsCategory::PerUser);
    pmSettingsService->SetChangedSettingHandler(
        ms_strShadowClickGeneratorSettingName, SettingsCategory::PerUser,
        new SettingChangedHandler(NULL,
        &MShadowHelper::ShadowSettingsChangedHandler));

    // Register shadow click generator option.
    IOptionsService* pmOptionsService = MGetService(IOptionsService);
    MAssert(pmOptionsService != NULL, "Options service not found!");

    pmOptionsService->AddOption(ms_strShadowClickGeneratorOptionName,
        SettingsCategory::PerUser, ms_strShadowClickGeneratorSettingName);
    pmOptionsService->SetHelpDescription(ms_strShadowClickGeneratorOptionName, 
        ms_strShadowClickGeneratorOptionDescription);
    pmOptionsService->SetTypeConverter(ms_strShadowClickGeneratorOptionName,
        new MShadowClickGeneratorConverter());

    // Set the active shadow click generator on the manager.
    SetShadowClickGenerator();
}
//---------------------------------------------------------------------------
void MShadowHelper::ShadowSettingsChangedHandler(Object*,
    SettingChangedEventArgs* pmEventArgs)
{
    if (pmEventArgs->Name == ms_strShadowClickGeneratorSettingName &&
        pmEventArgs->Category == SettingsCategory::PerUser)
    {
        // Set the active shadow click generator on the manager.
        SetShadowClickGenerator();
    }
}
//---------------------------------------------------------------------------
void MShadowHelper::SetShadowClickGenerator()
{
    if (NiShadowManager::GetShadowManager())
    {
        ISettingsService* pmSettingsService = MGetService(ISettingsService);
        MAssert(pmSettingsService != NULL, "Settings service not found!");
        String* strValue = dynamic_cast<String*>(
            pmSettingsService->GetSettingsObject(
            ms_strShadowClickGeneratorSettingName, SettingsCategory::PerUser));
        MAssert(strValue != NULL, "Settings value is empty!");

        const char* pcValue = MStringToCharPointer(strValue);
        NiShadowManager::SetActiveShadowClickGenerator(pcValue);
        MFreeCharPointer(pcValue);
    }
}
//---------------------------------------------------------------------------
