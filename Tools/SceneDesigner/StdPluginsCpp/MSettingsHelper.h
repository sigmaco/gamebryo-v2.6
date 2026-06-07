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

using namespace System::Drawing;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MSettingsHelper : public MDisposable
    {
    public:
        static void RegisterStandardSettings();

        // this is how external classes will get at standard settigns
        __value enum StandardSetting
        {
            UP_AXIS = 0,
            HIGHLIGHT_COLOR,
            GIZMO_SCALE,
            ORBIT_DISTANCE,
            DOLLY_SCALAR,
            PAN_SCALAR,
            MOUSE_LOOK_SCALAR,
            VIEW_UNDOABLE,
            SELECTION_UNDOABLE,
            TRANSLATION_SNAP,
            TRANSLATION_SNAP_ENABLED,
            TRANSLATION_PRECISION,
            TRANSLATION_PRECISION_ENABLED,
            SNAP_TO_SURFACE_ENABLED,
            ALIGN_TO_SURFACE_ENABLED,
            ROTATION_SNAP,
            ROTATION_SNAP_ENABLED,
            ALWAYS_USE_LINEAR_ROTATION,
            SCALE_SNAP,
            SCALE_SNAP_ENABLED,
            SCALE_PRECISION,
            SCALE_PRECISION_ENABLED,
            SETTING_COUNT
        };

        static String* GetSettingName(StandardSetting eSetting);
        static String* GetSettingDescription(StandardSetting eSetting);
        static SettingsCategory GetSettingCategory(StandardSetting eSetting);
        static bool IsArgStandardSetting(SettingChangedEventArgs* pmEventArgs, 
            StandardSetting eSetting);

        static void GetStandardSetting(StandardSetting eSetting, 
            float __gc& fValue, SettingChangedHandler* pmHandler);
        static void GetStandardSetting(StandardSetting eSetting, 
            bool __gc& bValue, SettingChangedHandler* pmHandler);
        static void GetStandardSetting(StandardSetting eSetting, 
            NiPoint3& kValue, SettingChangedHandler* pmHandler);
        static void GetStandardSetting(StandardSetting eSetting, 
            NiColor& kValue, SettingChangedHandler* pmHandler);

        static void RegisterSetting(String* strName, String* strDescription, 
            String* strOptionCategory, float __gc& fValue, 
            SettingsCategory eCategory, bool bOption, 
            SettingChangedHandler* pmHandler);
        static void RegisterSetting(String* strName, String* strDescription, 
            String* strOptionCategory, bool __gc& bValue, 
            SettingsCategory eCategory, bool bOption, 
            SettingChangedHandler* pmHandler);
        static void RegisterSetting(String* strName, String* strDescription, 
            String* strOptionCategory, NiPoint3& kValue, 
            SettingsCategory eCategory, bool bOption, 
            SettingChangedHandler* pmHandler);
        static void RegisterSetting(String* strName, String* strDescription, 
            String* strOptionCategory, NiColor& kValue, 
            SettingsCategory eCategory, bool bOption, 
            SettingChangedHandler* pmHandler);

    protected:
        __property static ISettingsService* get_SettingsService();
        static ISettingsService* ms_pmSettingsService;

        __property static IOptionsService* get_OptionsService();
        static IOptionsService* ms_pmOptionsService;

        static String* ms_astrSettingName[];
        static String* ms_astrSettingDescription[];
        static String* ms_astrSettingOption[];
        static SettingsCategory ms_aeSettingCategory[];

        // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

        // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MSettingsHelper* get_Instance();
    private:
        static MSettingsHelper* ms_pmThis = NULL;
        MSettingsHelper();
    };
}}}}
