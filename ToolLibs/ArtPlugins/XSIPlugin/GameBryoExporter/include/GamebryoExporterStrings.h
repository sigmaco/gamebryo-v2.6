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

#ifndef XSI_GAMEBRYO_EXPORT_PLUGIN_STRINGS_H
#define XSI_GAMEBRYO_EXPORT_PLUGIN_STRINGS_H

#include <NiVersion.h>

// The extra level of macro indirection is needed to correctly
// expand the strings.
#define NI_WIDE_STR2(A) L#A
#define NI_WIDE_STR(B) NI_WIDE_STR2(B)

//---------------------------------------------------------------------------
// Version Constants.
//---------------------------------------------------------------------------

#define XSI_GAMEBRYO_EXPORT_PLUGIN_NAME                 L"Gamebryo Export Plugin"
#define XSI_GAMEBRYO_EXPORT_PLUGIN_ASCII_NAME            "Gamebryo Export Plugin"
#define XSI_GAMEBRYO_EXPORT_PLUGIN_VERSION_MAJOR        1
#define XSI_GAMEBRYO_EXPORT_PLUGIN_VERSION_MINOR        0
#define XSI_GAMEBRYO_EXPORT_PLUGIN_VERSION_STRING       L"1.1.0"
#define XSI_GAMEBRYO_EXPORT_GAMEBRYO_VERSION_STRING     \
    L"Gamebryo " NI_WIDE_STR(GAMEBRYO_MAJOR_VERSION) L"." NI_WIDE_STR(GAMEBRYO_MINOR_VERSION)
#define XSI_GAMEBRYO_EXPORT_CROSSWALK_VERSION_STRING    L"Crosswalk 3.0"
#define XSI_GAMEBRYO_EXPORT_PLUGIN_API_VERSIONS_STRING  \
    XSI_GAMEBRYO_EXPORT_PLUGIN_NAME L" " XSI_GAMEBRYO_EXPORT_PLUGIN_VERSION_STRING L", " \
    XSI_GAMEBRYO_EXPORT_CROSSWALK_VERSION_STRING L", " XSI_GAMEBRYO_EXPORT_GAMEBRYO_VERSION_STRING
#define XSI_GAMEBRYO_EXPORT_PLUGIN_DICTIONARY_NAME      L"XSIGAMEBRYO"

//---------------------------------------------------------------------------
// Gamebryo export property constants.
//---------------------------------------------------------------------------

#define XSI_GAMEBRYO_EDIT_SCRIPT_LABEL                  L"Options..."

#define XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_LABEL    L"Processing Script"
#define XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_DESC     \
    L"Gamebryo plugin pipeline data processing script."
#define XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_EDIT     L"EditProcessingScript"

#define XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_LABEL     L"Exporting Script"
#define XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_DESC      L"Gamebryo plugin pipeline export script."
#define XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_EDIT      L"EditExportingScript"

#define XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_LABEL       L"Viewing Script"
#define XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_DESC        \
    L"Gamebryo plugin pipeline viewing script."
#define XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_EDIT        L"EditViewingScript"

#define XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_LABEL     L"Export Selection Only"
#define XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_DESC      \
    L"Export only the currently selected objects in the scene."

#define XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_LABEL       L"Export Cameras"
#define XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_DESC        L"Export the cameras found in the scene."

#define XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_LABEL        L"Export Lights"
#define XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_DESC         L"Export the lights found in the scene."

#define XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_LABEL      L"Export Textures"
#define XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_DESC       L"Export the textures found in the scene."

#define XSI_GAMEBRYO_EXPORT_SHAPE_ANIMS_SETTING_LABEL   L"Export Shape Animations"
#define XSI_GAMEBRYO_EXPORT_SHAPE_ANIMS_SETTING_DESC    \
    L"Export the shape animations found in the scene."

#define XSI_GAMEBRYO_EXPORT_SKINNINGS_SETTING_LABEL     L"Export Skinnings"
#define XSI_GAMEBRYO_EXPORT_SKINNINGS_SETTING_DESC      \
    L"Export the skinnings and the bones found in the scene."

#define XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_LABEL  L"Launch Asset Viewer"
#define XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_DESC   \
    L"Launch the Gamebryo asset viewer with the exported file."

#define XSI_GAMEBRYO_IGNORE_ALL_ERRORS_SETTING_LABEL    L"Ignore All Errors"
#define XSI_GAMEBRYO_IGNORE_ALL_ERRORS_SETTING_DESC     \
    L"Ignore all errors while exporting to the Gamebryo format."

#define XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_LABEL       L"Embed Textures"
#define XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_DESC        \
    L"Embed the texture within the exported file."

#define XSI_GAMEBRYO_TEXTURE_FORMAT_SETTING_LABEL       L"Texture Format"
#define XSI_GAMEBRYO_TEXTURE_FORMAT_SETTING_DESC        \
    L"The image file format used for textures."

#define XSI_GAMEBRYO_TEXTURE_COMPRESSION_SETTING_LABEL  L"Texture Compression"
#define XSI_GAMEBRYO_TEXTURE_COMPRESSION_SETTING_DESC   \
    L"The type of compression used on the texture."

#define XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_LABEL        L"Keyframe Sampling Rate"
#define XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_DESC         \
    L"The rate at which keyframes are sampled, in frames per second."

#define XSI_GAMEBRYO_LOG_FILENAME_SETTING_LABEL         L"Log Filename"
#define XSI_GAMEBRYO_LOG_FILENAME_SETTING_DESC          \
    L"The name of the log file containing the description of the results of the export."

#define XSI_GAMEBRYO_OUT_FILENAME_SETTING_LABEL         L"Filename"
#define XSI_GAMEBRYO_OUT_FILENAME_SETTING_DESC          L"The name of the output file."

#define XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_LABEL      L"Target Platform"
#define XSI_GAMEBRYO_MESH_PROFILE_SETTING_LABEL      L"Mesh Profile"
#define XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_DESC       \
    L"The target platform for platform-specfic optimizations."

#define XSI_GAMEBRYO_MESH_PROFILE_SETTING_DESC       \
    L"The mesh profile to use during export."

#define XSI_GAMEBRYO_GENERIC_PLATFORM_VALUE_LABEL       L"Generic"
#define XSI_GAMEBRYO_DIRECTX_9_PLATFORM_VALUE_LABEL     L"DirectX 9"
#define XSI_GAMEBRYO_DIRECTX_10_PLATFORM_VALUE_LABEL    L"DirectX 10 (D3D10)"
#define XSI_GAMEBRYO_XBOX_360_PLATFORM_VALUE_LABEL      L"Xbox 360"
#define XSI_GAMEBRYO_PS3_PLATFORM_VALUE_LABEL           L"Playstation 3 (PS3)"
#define XSI_GAMEBRYO_WII_PLATFORM_VALUE_LABEL           L"Wii"

#define XSI_GAMEBRYO_NIF_FILE_TYPES                     \
    L"Gamebryo File (*.nif)|*.nif|All Files (*.*)|*.*||"
#define XSI_GAMEBRYO_SCRIPT_FILE_TYPES                  \
    L"Gamebryo Plugin Script (*.script)|*.script|All Files (*.*)|*.*||"
#define XSI_GAMEBRYO_LOG_FILE_TYPES                     \
    L"XML Log File (*.xml)|*.xml|All Files (*.*)|*.*||"

//---------------------------------------------------------------------------
// Gamebryo z-buffer property constants.
//---------------------------------------------------------------------------

#define XSI_GAMEBRYO_ZBUFFER_TEST_PARAM_LABEL           L"Enable Testing"
#define XSI_GAMEBRYO_ZBUFFER_TEST_PARAM_DESC            L"Enable Z-buffer testing."

#define XSI_GAMEBRYO_ZBUFFER_WRITE_PARAM_LABEL          L"Enable Writing"
#define XSI_GAMEBRYO_ZBUFFER_WRITE_PARAM_DESC           L"Enable Z-buffer writing."

#define XSI_GAMEBRYO_ZBUFFER_FUNCTION_PARAM_LABEL       L"Test Function"
#define XSI_GAMEBRYO_ZBUFFER_FUNCTION_PARAM_DESC        \
    L"Choose which Z-buffer test to use "               \
    L"(Always, Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual, Never)."

#define XSI_GAMEBRYO_ZBUFFER_LESS_VALUE_LABEL           L"Less"
#define XSI_GAMEBRYO_ZBUFFER_LESS_EQUAL_VALUE_LABEL     L"Less or Equal"
#define XSI_GAMEBRYO_ZBUFFER_GREATER_VALUE_LABEL        L"Greater"
#define XSI_GAMEBRYO_ZBUFFER_GREATER_EQUAL_VALUE_LABEL  L"Greater or Equal"
#define XSI_GAMEBRYO_ZBUFFER_EQUAL_VALUE_LABEL          L"Equal"
#define XSI_GAMEBRYO_ZBUFFER_NOT_EQUAL_VALUE_LABEL      L"Not Equal"
#define XSI_GAMEBRYO_ZBUFFER_ALWAYS_VALUE_LABEL         L"Always"
#define XSI_GAMEBRYO_ZBUFFER_NEVER_VALUE_LABEL          L"Never"

//---------------------------------------------------------------------------
// Gamebryo level-of-details property constants.
//---------------------------------------------------------------------------

#define XSI_GAMEBRYO_LOD_GROUP_NAME_PARAM_LABEL         L"Group Name"
#define XSI_GAMEBRYO_LOD_GROUP_NAME_PARAM_DESC          \
    L"The level-of-details group to which this object belongs."

#define XSI_GAMEBRYO_LOD_DISTANCE_PARAM_LABEL           L"Camera Distance"
#define XSI_GAMEBRYO_LOD_DISTANCE_PARAM_DESC            \
    L"The distance between the object and the camera where this level-of-details begins."

//---------------------------------------------------------------------------
// Light inclusive lit objects constants.
//---------------------------------------------------------------------------

#define XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_LABEL               L"Group Name"
#define XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_DESC                \
    L"List of light group this object belongs to."

#define XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_LABEL      L"Ignored Shadow Casters"
#define XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_DESC       \
    L"The list of light groups not casting shadow on this mesh."

#define XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_LABEL   L"Ignored Shadow Receivers"
#define XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_DESC    \
    L"The list of light groups ignored by this mesh for shadows."

#define XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_LABEL           L"Shadow Technique"
#define XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_DESC            \
    L"The shadow generation technique used by the light."

#define XSI_GAMEBRYO_NONE_SHADOW_TECHNIQUE_LABEL            L"None"
#define XSI_GAMEBRYO_DEFAULT_SHADOW_TECHNIQUE_LABEL         L"Standard Technique"
#define XSI_GAMEBRYO_VSM_SHADOW_TECHNIQUE_LABEL             L"Variance Shadow Mapping Technique"
#define XSI_GAMEBRYO_PCF_SHADOW_TECHNIQUE_LABEL             \
    L"Percentage Closer Filtering Technique"

//---------------------------------------------------------------------------
// Crosswalk Property Constants.
// Note: these names have been taken from the dotXSI plugin.
// Note: we only forced these, but changing others might affect export.
//---------------------------------------------------------------------------

#define XSI_CROSSWALK_EXPORT_FILENAME_SETTING_NAME          L"Filename"
#define XSI_CROSSWALK_EXPORT_FORMAT_SETTING_NAME            L"Format"
#define XSI_CROSSWALK_EXPORT_TRIANGULATE_SETTING_NAME       L"Triangulate"
#define XSI_CROSSWALK_EXPORT_PLOT_ANIMATION_SETTING_NAME    L"PlotAnimation"
#define XSI_CROSSWALK_EXPORT_EXPORT_NORMALS_SETTING_NAME    L"ExportXSINormals"
#define XSI_CROSSWALK_EXPORT_INTERPOLATION_SETTING_NAME     L"PlotInterpolation"
#define XSI_CROSSWALK_EXPORT_SELECTION_SETTING_NAME         L"ExportSelectionOnly"
#define XSI_CROSSWALK_EXPORT_RELATIVE_PATH_SETTING_NAME     L"PathRelative"

//---------------------------------------------------------------------------
#endif /* XSI_GAMEBRYO_EXPORT_PLUGIN_STRINGS_H */
