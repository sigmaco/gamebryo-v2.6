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

#pragma once

#include "IService.h"
#include "SettingChangedEventArgs.h"


namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    /// <summary>
    /// Summary description for IOptionsService.
    /// </summary>
    public __gc __interface IOptionsService : public IService
    {
        void AddOption(String* strName, SettingsCategory eCategory,
            String* strSetting);
        void SetHelpDescription(String* strOptionName, String* strHelpText);
        String* GetHelpDescription(String* strOptionName);

        void SetTypeConverter(String* strOptionName, 
            TypeConverter* pmTypeConverter);
        TypeConverter* GetTypeConverter(String* strOptionName);

        void SetTypeEditor(String* strOptionName,
            Object* pmEditor);
        Object* GetTypeEditor(String* strOptionName);

        String* GetOptions(SettingsCategory eCategory)[];
        String* GetOptions(String* strOptionCategory, 
            SettingsCategory eCategory)[];
        Object* GetSettingsObject(String* optionName);
        String* GetSettingsName(String* strOptionName);
        SettingsCategory GetSettingsCategory(String* strOptionName);
    };
}}}}}