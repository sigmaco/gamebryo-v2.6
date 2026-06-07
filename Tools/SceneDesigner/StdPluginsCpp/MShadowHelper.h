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

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MShadowHelper
    {
    public:
        static void RegisterSettings();

        static void ShadowSettingsChangedHandler(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

    private:
        static void SetShadowClickGenerator();

        static String* ms_strShadowClickGeneratorSettingName = "Shadow Click "
            "Generator";

        static String* ms_strShadowOptionCategory = "Shadows.";
        static String* ms_strShadowClickGeneratorOptionName = String::Concat(
            ms_strShadowOptionCategory, ms_strShadowClickGeneratorSettingName);
        static String* ms_strShadowClickGeneratorOptionDescription =
            "The shadow click generator to use when rendering shadow maps. "
            "This specifies the shadow map generation method.";

        static String* ms_strDefaultShadowClickGenerator =
            "NiDefaultShadowClickGenerator";
    };
}}}}
