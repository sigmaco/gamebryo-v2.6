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

#include "MSettingsHelper.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
void MSettingsHelper::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MSettingsHelper();
    }
}
//---------------------------------------------------------------------------
void MSettingsHelper::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MSettingsHelper::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MSettingsHelper* MSettingsHelper::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MSettingsHelper::MSettingsHelper()
{
    // initialized standard settings arrays here
    ms_astrSettingName = new String*[SETTING_COUNT];
    ms_astrSettingDescription = new String*[SETTING_COUNT];
    ms_astrSettingOption  = new String*[SETTING_COUNT];
    ms_aeSettingCategory = new SettingsCategory[SETTING_COUNT];

    ms_astrSettingName[UP_AXIS] = "Up Axis";
    ms_aeSettingCategory[UP_AXIS] = SettingsCategory::PerUser;
    ms_astrSettingDescription[UP_AXIS] = "The axis that should be used when "
        "aligning the cameras and the grid.";
    ms_astrSettingOption[UP_AXIS] = "View Settings.Up Axis";

    ms_astrSettingName[HIGHLIGHT_COLOR] = "Gizmo Highlight Color";
    ms_aeSettingCategory[HIGHLIGHT_COLOR] = SettingsCategory::PerUser;
    ms_astrSettingDescription[HIGHLIGHT_COLOR] = "The color of the "
        "highlighted transformation axis in the gizmo.";
    ms_astrSettingOption[HIGHLIGHT_COLOR] = "Interaction Modes.Gizmo "
        "Highlight Color";

    ms_astrSettingName[GIZMO_SCALE] = "Gizmo Scale";
    ms_aeSettingCategory[GIZMO_SCALE] = SettingsCategory::PerUser;
    ms_astrSettingDescription[GIZMO_SCALE] = "The size of the transformation "
        "gizmos.";
    ms_astrSettingOption[GIZMO_SCALE] = "Interaction Modes.Gizmo Scale";

    ms_astrSettingName[ORBIT_DISTANCE] = "Orbit Distance";
    ms_aeSettingCategory[ORBIT_DISTANCE] = SettingsCategory::PerUser;
    ms_astrSettingDescription[ORBIT_DISTANCE] = "The default distance in "
        "front of the camera to orbit around when it is not facing an "
        "entity.";
    ms_astrSettingOption[ORBIT_DISTANCE] = "View Settings.Orbit Distance";

    ms_astrSettingName[DOLLY_SCALAR] = "Dolly Scalar";
    ms_aeSettingCategory[DOLLY_SCALAR] = SettingsCategory::PerUser;
    ms_astrSettingDescription[DOLLY_SCALAR] = "A multiplier for the speed "
        "the camera moves while dollying.";
    ms_astrSettingOption[DOLLY_SCALAR] = "View Settings.Dolly Scalar";

    ms_astrSettingName[PAN_SCALAR] = "Pan Scalar";
    ms_aeSettingCategory[PAN_SCALAR] = SettingsCategory::PerUser;
    ms_astrSettingDescription[PAN_SCALAR] = "The speed that the camera pans "
        "when the mouse is not over an entity.";
    ms_astrSettingOption[PAN_SCALAR] = "View Settings.Pan Scalar";

    ms_astrSettingName[MOUSE_LOOK_SCALAR] = "Mouse Look Scalar";
    ms_aeSettingCategory[MOUSE_LOOK_SCALAR] = SettingsCategory::PerUser;
    ms_astrSettingDescription[MOUSE_LOOK_SCALAR] = "The rate at which the "
        "camera rotates during mouse look and orbit.";
    ms_astrSettingOption[MOUSE_LOOK_SCALAR] = "View Settings.Mouse Look "
        "Scalar";

    ms_astrSettingName[VIEW_UNDOABLE] = "View Commands Undoable";
    ms_aeSettingCategory[VIEW_UNDOABLE] = SettingsCategory::PerUser;
    ms_astrSettingDescription[VIEW_UNDOABLE] = "Indicates whether or not "
        "camera navigation commands get added to the undo stack.";
    ms_astrSettingOption[VIEW_UNDOABLE] = 
        "Undo Settings.View Commands Undoable";

    ms_astrSettingName[SELECTION_UNDOABLE] = "Selection Commands Undoable";
    ms_aeSettingCategory[SELECTION_UNDOABLE] = SettingsCategory::PerUser;
    ms_astrSettingDescription[SELECTION_UNDOABLE] = "Indicates whether or "
        "not entity selection changes get added to the undo stack.";
    ms_astrSettingOption[SELECTION_UNDOABLE] = 
        "Undo Settings.Selection Commands Undoable";

    ms_astrSettingName[TRANSLATION_SNAP] = "Translation Snap";
    ms_aeSettingCategory[TRANSLATION_SNAP] = SettingsCategory::PerScene;
    ms_astrSettingDescription[TRANSLATION_SNAP] = "The distance between each "
        "translation increment when snap is enabled.";
    ms_astrSettingOption[TRANSLATION_SNAP] = 
        "Snap & Precision.Translation & Creation.Translation Snap";

    ms_astrSettingName[TRANSLATION_SNAP_ENABLED] = "Translation Snap Enabled";
    ms_aeSettingCategory[TRANSLATION_SNAP_ENABLED] =
        SettingsCategory::PerScene;
    ms_astrSettingDescription[TRANSLATION_SNAP_ENABLED] = 
        "Toggles translation snap.";
    ms_astrSettingOption[TRANSLATION_SNAP_ENABLED] = 
        "Snap & Precision.Translation & Creation.Translation Snap Enabled";

    ms_astrSettingName[TRANSLATION_PRECISION] = "Translation Precision";
    ms_aeSettingCategory[TRANSLATION_PRECISION] = SettingsCategory::PerScene;
    ms_astrSettingDescription[TRANSLATION_PRECISION] = "The final "
        "translation will round off to this increment when tranlation "
        "precision is enabled.";
    ms_astrSettingOption[TRANSLATION_PRECISION] = 
        "Snap & Precision.Translation & Creation.Translation Precision";

    ms_astrSettingName[TRANSLATION_PRECISION_ENABLED] = 
        "Translation Precision Enabled";
    ms_aeSettingCategory[TRANSLATION_PRECISION_ENABLED] = 
        SettingsCategory::PerScene;
    ms_astrSettingDescription[TRANSLATION_PRECISION_ENABLED] = 
        "Toggles translation precision.";
    ms_astrSettingOption[TRANSLATION_PRECISION_ENABLED] = "Snap & "
        "Precision.Translation & Creation.Translation Precision Enabled";

    ms_astrSettingName[SNAP_TO_SURFACE_ENABLED] = 
        "Snap To Surface Enabled";
    ms_aeSettingCategory[SNAP_TO_SURFACE_ENABLED] = 
        SettingsCategory::PerScene;
    ms_astrSettingDescription[SNAP_TO_SURFACE_ENABLED] = 
        "Toggles snap to surface.";
    ms_astrSettingOption[SNAP_TO_SURFACE_ENABLED] = 
        "Snap & Precision.Translation & Creation.Snap To Surface Enabled";

    ms_astrSettingName[ALIGN_TO_SURFACE_ENABLED] = 
        "Align To Surface Enabled";
    ms_aeSettingCategory[ALIGN_TO_SURFACE_ENABLED] = 
        SettingsCategory::PerScene;
    ms_astrSettingDescription[ALIGN_TO_SURFACE_ENABLED] = 
        "Toggles align to surface.";
    ms_astrSettingOption[ALIGN_TO_SURFACE_ENABLED] = 
        "Snap & Precision.Translation & Creation.Align To Surface Enabled";

    ms_astrSettingName[ROTATION_SNAP] = "Rotation Snap";
    ms_aeSettingCategory[ROTATION_SNAP] = SettingsCategory::PerScene;
    ms_astrSettingDescription[ROTATION_SNAP] = "The number of degrees "
        "between each rotation increment when snap is enabled.";
    ms_astrSettingOption[ROTATION_SNAP] = 
        "Snap & Precision.Rotation.Rotation Snap";

    ms_astrSettingName[ROTATION_SNAP_ENABLED] = "Rotation Snap Enabled";
    ms_aeSettingCategory[ROTATION_SNAP_ENABLED] = SettingsCategory::PerScene;
    ms_astrSettingDescription[ROTATION_SNAP_ENABLED] = 
        "Toggles rotation snap.";
    ms_astrSettingOption[ROTATION_SNAP_ENABLED] = 
        "Snap & Precision.Rotation.Rotation Snap Enabled";

    ms_astrSettingName[ALWAYS_USE_LINEAR_ROTATION] = 
        "Always Use Linear Rotation";
    ms_aeSettingCategory[ALWAYS_USE_LINEAR_ROTATION] = 
        SettingsCategory::PerUser;
    ms_astrSettingDescription[ALWAYS_USE_LINEAR_ROTATION] = "If this option "
        "is enabled, rotation is performed by moving the cursor in a "
        "straight line relative to the gizmo instead of in a circle.";
    ms_astrSettingOption[ALWAYS_USE_LINEAR_ROTATION] = 
        "Interaction Modes.Always Use Linear Rotation";

    ms_astrSettingName[SCALE_SNAP] = "Scale Snap";
    ms_aeSettingCategory[SCALE_SNAP] = SettingsCategory::PerScene;
    ms_astrSettingDescription[SCALE_SNAP] = "The distance between each "
        "scale increment when snap is enabled.";
    ms_astrSettingOption[SCALE_SNAP] = 
        "Snap & Precision.Scale.Scale Snap";

    ms_astrSettingName[SCALE_SNAP_ENABLED] = "Scale Snap Enabled";
    ms_aeSettingCategory[SCALE_SNAP_ENABLED] = SettingsCategory::PerScene;
    ms_astrSettingDescription[SCALE_SNAP_ENABLED] = 
        "Toggles scale snap.";
    ms_astrSettingOption[SCALE_SNAP_ENABLED] = 
        "Snap & Precision.Scale.Scale Snap Enabled";

    ms_astrSettingName[SCALE_PRECISION] = "Scale Precision";
    ms_aeSettingCategory[SCALE_PRECISION] = SettingsCategory::PerScene;
    ms_astrSettingDescription[SCALE_PRECISION] = "The final scale "
        "will round off to this increment when scale precision is enabled.";
    ms_astrSettingOption[SCALE_PRECISION] = 
        "Snap & Precision.Scale.Scale Precision";

    ms_astrSettingName[SCALE_PRECISION_ENABLED] = 
        "Scale Precision Enabled";
    ms_aeSettingCategory[SCALE_PRECISION_ENABLED] = 
        SettingsCategory::PerScene;
    ms_astrSettingDescription[SCALE_PRECISION_ENABLED] = 
        "Toggles scale precision.";
    ms_astrSettingOption[SCALE_PRECISION_ENABLED] = 
        "Snap & Precision.Scale.Scale Precision Enabled";
}
//---------------------------------------------------------------------------
void MSettingsHelper::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
void MSettingsHelper::RegisterStandardSettings()
{
    float fValue;
    bool bValue;
    NiPoint3 kPoint;
    NiColor kColor;

    // uncomment these lines to expose up axis as a settable option
    // this may expose bugs as alternate up axes are not officially supported
    //kPoint = NiPoint3::UNIT_Z;
    //GetStandardSetting(UP_AXIS, kPoint, NULL);
    kColor = NiColor(1.0f, 1.0f, 0.0f);
    GetStandardSetting(HIGHLIGHT_COLOR, kColor, NULL);
    fValue = 1.0f;
    GetStandardSetting(GIZMO_SCALE, fValue, NULL);
    fValue = 100.0f;
    GetStandardSetting(ORBIT_DISTANCE, fValue, NULL);
    fValue = 0.1f;
    GetStandardSetting(DOLLY_SCALAR, fValue, NULL);
    fValue = 0.5f;
    GetStandardSetting(PAN_SCALAR, fValue, NULL);
    fValue = 0.001f;
    GetStandardSetting(MOUSE_LOOK_SCALAR, fValue, NULL);
    bValue = false;
    GetStandardSetting(VIEW_UNDOABLE, bValue, NULL);
    bValue = true;
    GetStandardSetting(SELECTION_UNDOABLE, bValue, NULL);
    fValue = 1.0f;
    GetStandardSetting(TRANSLATION_SNAP, fValue, NULL);
    bValue = false;
    GetStandardSetting(TRANSLATION_SNAP_ENABLED, bValue, NULL);
    fValue = 0.1f;
    GetStandardSetting(TRANSLATION_PRECISION, fValue, NULL);
    bValue = false;
    GetStandardSetting(TRANSLATION_PRECISION_ENABLED, bValue, NULL);
    bValue = false;
    GetStandardSetting(SNAP_TO_SURFACE_ENABLED, bValue, NULL);
    bValue = false;
    GetStandardSetting(ALIGN_TO_SURFACE_ENABLED, bValue, NULL);
    fValue = 5.0f;
    GetStandardSetting(ROTATION_SNAP, fValue, NULL);
    bValue = false;
    GetStandardSetting(ROTATION_SNAP_ENABLED, bValue, NULL);
    bValue = false;
    GetStandardSetting(ALWAYS_USE_LINEAR_ROTATION, bValue, NULL);
    fValue = 0.1f;
    GetStandardSetting(SCALE_SNAP, fValue, NULL);
    bValue = false;
    GetStandardSetting(SCALE_SNAP_ENABLED, bValue, NULL);
    fValue = 0.1f;
    GetStandardSetting(SCALE_PRECISION, fValue, NULL);
    bValue = false;
    GetStandardSetting(SCALE_PRECISION_ENABLED, bValue, NULL);
}
//---------------------------------------------------------------------------
String* MSettingsHelper::GetSettingName(StandardSetting eSetting)
{
    return ms_astrSettingName[eSetting];
}
//---------------------------------------------------------------------------
String* MSettingsHelper::GetSettingDescription(StandardSetting eSetting)
{
    return ms_astrSettingDescription[eSetting];
}
//---------------------------------------------------------------------------
SettingsCategory MSettingsHelper::GetSettingCategory(StandardSetting eSetting)
{
    return ms_aeSettingCategory[eSetting];
}
//---------------------------------------------------------------------------
bool MSettingsHelper::IsArgStandardSetting(
    SettingChangedEventArgs* pmEventArgs, StandardSetting eSetting)
{
    String* strName = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    return ((strName->Equals(GetSettingName(eSetting))) && (eCategory == 
        GetSettingCategory(eSetting)));
}
//---------------------------------------------------------------------------
void MSettingsHelper::GetStandardSetting(StandardSetting eSetting, 
    float __gc& fValue, SettingChangedHandler* pmHandler)
{
    RegisterSetting(ms_astrSettingName[eSetting], 
        ms_astrSettingDescription[eSetting], ms_astrSettingOption[eSetting],
        fValue, ms_aeSettingCategory[eSetting], true, pmHandler);
}
//---------------------------------------------------------------------------
void MSettingsHelper::GetStandardSetting(StandardSetting eSetting, 
    bool __gc& bValue, SettingChangedHandler* pmHandler)
{
    RegisterSetting(ms_astrSettingName[eSetting], 
        ms_astrSettingDescription[eSetting], ms_astrSettingOption[eSetting],
        bValue, ms_aeSettingCategory[eSetting], true, pmHandler);
}
//---------------------------------------------------------------------------
void MSettingsHelper::GetStandardSetting(StandardSetting eSetting, 
    NiPoint3& kValue, SettingChangedHandler* pmHandler)
{
    RegisterSetting(ms_astrSettingName[eSetting], 
        ms_astrSettingDescription[eSetting], ms_astrSettingOption[eSetting],
        kValue, ms_aeSettingCategory[eSetting], true, pmHandler);
}
//---------------------------------------------------------------------------
void MSettingsHelper::GetStandardSetting(StandardSetting eSetting, 
    NiColor& kValue, SettingChangedHandler* pmHandler)
{
    RegisterSetting(ms_astrSettingName[eSetting], 
        ms_astrSettingDescription[eSetting], ms_astrSettingOption[eSetting],
        kValue, ms_aeSettingCategory[eSetting], true, pmHandler);
}
//---------------------------------------------------------------------------
void MSettingsHelper::RegisterSetting(String* strName, String* strDescription, 
    String* strOptionCategory, float __gc& fValue, SettingsCategory eCategory, 
    bool bOption, SettingChangedHandler* pmHandler)
{
    // first register the setting with the service if it doesn't already exist
    SettingsService->RegisterSettingsObject(strName, __box(fValue), eCategory);
    // set the event handler for when that setting changes
    if (pmHandler != NULL)
    {
        SettingsService->SetChangedSettingHandler(strName, eCategory, 
            pmHandler);
    }
    // get the value of the setting in case it existed before and cache it
    Object* pmObj;
    pmObj = SettingsService->GetSettingsObject(strName, eCategory);
    __box float* pfVal = dynamic_cast<__box float*>(pmObj);
    if (pfVal != NULL)
    {
        fValue = *pfVal;

        if ((bOption) && (strOptionCategory != NULL))
        {
            OptionsService->AddOption(strOptionCategory, eCategory, strName); 
        }
        if ((bOption) && (strDescription != NULL))
        {
            OptionsService->SetHelpDescription(strOptionCategory, 
                strDescription);
        }
    }
}
//---------------------------------------------------------------------------
void MSettingsHelper::RegisterSetting(String* strName, String* strDescription, 
    String* strOptionCategory, bool __gc& bValue, SettingsCategory eCategory, 
    bool bOption, SettingChangedHandler* pmHandler)
{
    // first register the setting with the service if it doesn't already exist
    SettingsService->RegisterSettingsObject(strName, __box(bValue), eCategory);
    // set the event handler for when that setting changes
    if (pmHandler != NULL)
    {
        SettingsService->SetChangedSettingHandler(strName, eCategory, 
            pmHandler);
    }
    // get the value of the setting in case it existed before and cache it
    Object* pmObj;
    pmObj = SettingsService->GetSettingsObject(strName, eCategory);
    __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
    if (pbVal != NULL)
    {
        bValue = *pbVal;

        if ((bOption) && (strOptionCategory != NULL))
        {
            OptionsService->AddOption(strOptionCategory, eCategory, strName); 
        }
        if ((bOption) && (strDescription != NULL))
        {
            OptionsService->SetHelpDescription(strOptionCategory, 
                strDescription);
        }
    }
}
//---------------------------------------------------------------------------
void MSettingsHelper::RegisterSetting(String* strName, String* strDescription, 
    String* strOptionCategory, NiPoint3& kValue, SettingsCategory eCategory, 
    bool bOption, SettingChangedHandler* pmHandler)
{
    // first register the setting in case it doesn't already exist
    SettingsService->RegisterSettingsObject(strName, new MPoint3(kValue), 
        eCategory);
    // set the event handler for when that setting changes
    if (pmHandler != NULL)
    {
        SettingsService->SetChangedSettingHandler(strName, eCategory, 
            pmHandler);
    }
    // get the value of the setting in case it existed before and cache it
    Object* pmObj;
    MPoint3* pmValue;
    pmObj = SettingsService->GetSettingsObject(strName, eCategory);
    pmValue = dynamic_cast<MPoint3*>(pmObj);
    if (pmValue != NULL)
    {
        pmValue->ToNiPoint3(kValue);

        if ((bOption) && (strOptionCategory != NULL))
        {
            OptionsService->AddOption(strOptionCategory, eCategory, strName); 
        }
        if ((bOption) && (strDescription != NULL))
        {
            OptionsService->SetHelpDescription(strOptionCategory, 
                strDescription);
        }
    }
}
//---------------------------------------------------------------------------
void MSettingsHelper::RegisterSetting(String* strName, String* strDescription, 
    String* strOptionCategory, NiColor& kValue, SettingsCategory eCategory, 
    bool bOption, SettingChangedHandler* pmHandler)
{
    // first register the setting in case it doesn't already exist
    SettingsService->RegisterSettingsObject(strName, __box(Color::FromArgb(255,
        MUtility::FloatToRGB(kValue.r), MUtility::FloatToRGB(kValue.g), 
        MUtility::FloatToRGB(kValue.b))), eCategory);
    // set the event handler for when that setting changes
    if (pmHandler != NULL)
    {
        SettingsService->SetChangedSettingHandler(strName, eCategory, 
            pmHandler);
    }
    // get the value of the setting in case it existed before and cache it
    Object* pmObj;
    __box Color* pmValue;
    pmObj = SettingsService->GetSettingsObject(strName, eCategory);
    pmValue = dynamic_cast<__box Color*>(pmObj);
    if (pmValue != NULL)
    {
        kValue.r = MUtility::RGBToFloat((*pmValue).R);
        kValue.g = MUtility::RGBToFloat((*pmValue).G);
        kValue.b = MUtility::RGBToFloat((*pmValue).B);

        if ((bOption) && (strOptionCategory != NULL))
        {
            OptionsService->AddOption(strOptionCategory, eCategory, strName); 
        }
        if ((bOption) && (strDescription != NULL))
        {
            OptionsService->SetHelpDescription(strOptionCategory, 
                strDescription);
        }
    }
}
//---------------------------------------------------------------------------
ISettingsService* MSettingsHelper::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found.");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MSettingsHelper::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not found.");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
