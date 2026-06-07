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

#ifndef XSI_GAMEBRYO_CONVERTER_HELPERS_H
#define XSI_GAMEBRYO_CONVERTER_HELPERS_H

#include "SLTypes.h"

#include <vector>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLModel;
class XSIEXPORT CSLTemplate;
class XSIEXPORT CSLCustomPSet;
class XSIEXPORT CSLVariantParameter;
class XSIEXPORT CSLBaseMaterial;
class XSIEXPORT CSLConnectionPoint;
class XSIEXPORT CSLShaderConnectionPoint;
class XSIEXPORT CSLXSIMaterial;
class XSIEXPORT CSLXSIShader;
class XSIEXPORT CSLFCurve;
class XSIEXPORT CSLActionFCurve;
class XSIEXPORT CSLAction;
class XSIEXPORT CdotXSIParams;
class XSIEXPORT CSLStaticValue;

#if defined(NIMAIN_IMPORT) || defined(NIMAIN_EXPORT)
#include "NiMainLibType.h"
class NIMAIN_ENTRY NiColor;
class NIMAIN_ENTRY NiAVObject;
class NIMAIN_ENTRY NiFixedString;
#else
class NiColor;
class NiFixedString;
class NiAVObject;
#endif

namespace epg
{
    class ConverterSettings;
    class Context;

    //---------------------------------------------------------------------------
    // XSI helper functions.
    //---------------------------------------------------------------------------

    //@{
    /// Retrieve the XSI type names.
    const char * GetModelTypeName(CSLModel& in_Model);
    const char * GetTemplateTypeName(CSLTemplate& in_Template);
    const char * GetMaterialTypeName(CSLBaseMaterial& in_Material);
    const char * GetFCurveTypeName(CSLFCurve& in_FCurve);
    //@}

    /** Check if the model has the visibility flag on.
        @param CSLModel& the model to check.
        @return true if the model is visible.
    */
    bool IsVisible(CSLModel& in_Model);
    /** Check if the model is of a type that can be seen.
        This is used to avoid making invisible a model that is not visible anyway.
        @param CSLModel& the model to check.
        @return true if the model has the capacity of being visible.
    */
    bool CanBeVisible(CSLModel& in_Model);

    /** Extract converter setting from a given model.
        @param CSLModel& the model where to load the settings from.
        @param ConverterSettings& the settings to load into.
        @param const char* the name of the property to load from. Use default if null.
        @return true on success.
    */
    bool LoadSettings(CSLModel& in_Model, 
        ConverterSettings& io_Settings, 
        const char* in_PropertyName = 0);

    //---------------------------------------------------------------------------
    // Action reference helpers.
    //---------------------------------------------------------------------------

    /** Parse an action curve reference and find the associated model.
        @return the model if found. May return NULL.
    */
    CSLModel* FindActionCurveReference(
        Context& io_Context,
        CSLAction& in_Action,
        CSLActionFCurve& in_Curve);

    /** Parse a static value reference and find the associated model.
        @return the model if found. May return NULL.
    */
    CSLModel* FindActionStaticValueReference(
        Context& io_Context,
        CSLAction& in_Action,
        CSLStaticValue& in_Value);

    //---------------------------------------------------------------------------
    // XSI connection point helper functions.
    //---------------------------------------------------------------------------

    /** Retrieve the connection point with the given name.
        @return the connection point or NULL if not found.
    */
    CSLConnectionPoint* FindConnectionPoint(CSLXSIMaterial& in_Mat, const char* in_Name);
    CSLShaderConnectionPoint* FindConnectionPoint(CSLXSIShader& in_Shader, const char* in_Name);

    //---------------------------------------------------------------------------
    // Custom property helper functions.
    //---------------------------------------------------------------------------

    /** Retrieve the custom propery set with the given name.
        @return the property set or NULL if not found.
    */
    CSLCustomPSet* FindCustomProperties(CSLTemplate& in_Tmpl, const char* in_Name);

    /// Verify if a custom property name is a known cutom property used in Gamebryo.
    bool IsKnownCustomProperty(const char* in_Name);

    /// Convert all custom properties to their proper equivalent.
    bool ConvertUnknownCustomProperties(
        Context& io_Context,
        CSLTemplate& in_Tmpl,
        NiAVObject& io_Object);

    /// Convert an XSI custom property set to NiExtraData on the specified object.
    bool ConvertUnknownCustomPropertySet(
        Context& io_Context,
        CSLCustomPSet& in_PSet,
        NiAVObject& io_Object);

    /// Convert a custom property parameter to NiExtraData
    bool ConvertCustomParameter(
        Context& io_Context,
        CSLVariantParameter& in_Param,
        NiAVObject& io_Object);

    //---------------------------------------------------------------------------
    // XSI parameter helper functions.
    //---------------------------------------------------------------------------

    /** Retrieve the typed parameter if it has the specified type.
        @return true if the parameter has the proper type.
    */
    bool GetTypedParameter(
        CdotXSIParams* in_pParam,
        int in_Type,
        SI_TinyVariant & out_Value);

    /** Retrieve the parameter with the given name a value.
        @return true if the parameter with the proper name and type is found.
    */
    bool GetTypedParameter(CSLTemplate& in_Tmpl, 
        const char * in_Name, 
        int in_Type, 
        SI_TinyVariant & out_Value);

    //@{
    /** Find the parameter with the given name and value.
        @return true if the parameter was found.
    */
    bool FindStringParameter(CSLTemplate& in_Tmpl,  
        const char * in_Name, 
        CSIBCString & out_Value);
    bool FindBooleanParameter(CSLTemplate& in_Tmpl, 
        const char * in_Name, 
        bool & out_Value);
    bool FindIntegerParameter(CSLTemplate& in_Tmpl, 
        const char * in_Name, 
        int & out_Value);
    bool FindLongParameter(CSLTemplate& in_Tmpl, 
        const char * in_Name, 
        long & out_Value);
    bool FindFloatParameter(CSLTemplate& in_Tmpl,   
        const char * in_Name, 
        float & out_Value);
    bool FindFloatSubParameter(CSLTemplate& in_Tmpl,
        const char * in_Name, 
        const char * in_SubName, 
        float& out_Value);
    bool FindColorParameter(Context & io_Context, 
        CSLTemplate& in_Tmpl, 
        const char * in_Name, 
        NiColor& out_Color);
    //@}

    //@{
    /** Retrieve the parameter with the given name and value.
        @return the parameter value if found with the proper type and name,
                else the default value provided.
    */
    const char * GetStringParameter(CSLTemplate& in_Tmpl, 
        const char * in_Name, 
        const char * in_DefaultValue = "");
    bool GetBooleanParameter(CSLTemplate& in_Tmpl,  
        const char * in_Name, 
        bool in_DefaultValue = false);
    int GetIntegerParameter(CSLTemplate& in_Tmpl,   
        const char * in_Name, 
        int in_DefaultValue = 0);
    long GetLongParameter(CSLTemplate& in_Tmpl,   
        const char * in_Name, 
        long in_DefaultValue = 0);
    float GetFloatParameter(CSLTemplate& in_Tmpl,   
        const char * in_Name, 
        float in_DefaultValue = 0.0f);
    float GetFloatSubParameter(CSLTemplate& in_Tmpl,
        const char * in_Name, 
        const char * in_SubName, 
        float in_DefaultValue = 0.0f);
    //@}

    //---------------------------------------------------------------------------
    // Magic preprocessor stuf.
    //---------------------------------------------------------------------------

    // Pre-processor magic to get L before something that may be itself a definition.
    #define XSI_GAMEBRYO_CONCAT(x,y)                        x##y
    #define XSI_GAMEBRYO_WIDE_STRING(x)                     XSI_GAMEBRYO_CONCAT(L,x)

    //---------------------------------------------------------------------------
    /// Converter settings property constants.
    ///
    /// @note: when adding a new conversion setting, thr following must be done:
    ///
    ///        1. Add new Set() and Get() functions in epg::ConversionSettings
    ///           class.
    ///        2. Add its name here.
    ///        3. Add a wide-string version of its name here.
    ///        4. Add a label and description for it in the XSI export plugin
    ///           header called "GamebryoExporterStrings.h".
    ///        5. Add the setting in the XSI Gamebryo custom property with
    ///           proper type in "GamebryoExporterProps.cpp".
    ///        6. Add the UI for the property page for the custom property
    ///           in the UI layout definition in "GamebryoExporterProps.cpp".
    ///        7. Add the loading of the new setting in LoadSettings() in
    ///           "XSIParamHelpers.cpp".
    ///        8. Possibly retrieve the setting in ExportGamebryo_Execute()
    ///           in "GamebryoExporterExport.cpp".
    ///        9. Use the setting during the conversion.
    //---------------------------------------------------------------------------

    // Default name for the property containing the settings.
    #define XSI_GAMEBRYO_EXPORT_OPTIONS_NAME                "ExportGamebryoOptions"

    #define XSI_GAMEBRYO_EXPORT_OPTIONS_NAME_W              \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_EXPORT_OPTIONS_NAME)

    // Property name for each setting.
    #define XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME     "GamebryoProcessingScript"
    #define XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME      "GamebryoExportingScript"
    #define XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_NAME        "GamebryoViewingScript"
    #define XSI_GAMEBRYO_PROCESSING_SCRIPT_ENC_SETTING_NAME "GamebryoProcessingScriptEnc"
    #define XSI_GAMEBRYO_EXPORTING_SCRIPT_ENC_SETTING_NAME "GamebryoExportingScriptEnc"
    #define XSI_GAMEBRYO_VIEWING_SCRIPT_ENC_SETTING_NAME   "GamebryoViewingScriptEnc"
    #define XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_NAME      "ExportSelection"
    #define XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_NAME        "ExportCameras"
    #define XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_NAME         "ExportLights"
    #define XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_NAME       "ExportTextures"
    #define XSI_GAMEBRYO_EXPORT_SHAPE_ANIMS_SETTING_NAME    "ExportShapeAnimations"
    #define XSI_GAMEBRYO_EXPORT_SKINNINGS_SETTING_NAME      "ExportSkinnings"
    #define XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_NAME   "LaunchAssetViewer"
    #define XSI_GAMEBRYO_IGNORE_ALL_ERRORS_SETTING_NAME     "IgnoreAllErrors"
    #define XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_NAME        "EmbedTextures"
    #define XSI_GAMEBRYO_TEXTURE_FORMAT_SETTING_NAME        "TextureFormat"
    #define XSI_GAMEBRYO_TEXTURE_COMPRESSION_SETTING_NAME   "TextureCompression"
    #define XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_NAME         "KeyframeSamplingRate"
    #define XSI_GAMEBRYO_LOG_FILENAME_SETTING_NAME          "LogFilename"
    #define XSI_GAMEBRYO_OUT_FILENAME_SETTING_NAME          "OutputFilename"
    #define XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_NAME       "TargetPlatform"
    #define XSI_GAMEBRYO_MESH_PROFILE_SETTING_NAME          "MeshProfileName"

    #define XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME_W   \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME)
    #define XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME_W    \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME)
    #define XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_NAME_W      \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_NAME)
    #define XSI_GAMEBRYO_PROCESSING_SCRIPT_ENC_SETTING_NAME_W   \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_PROCESSING_SCRIPT_ENC_SETTING_NAME)
    #define XSI_GAMEBRYO_EXPORTING_SCRIPT_ENC_SETTING_NAME_W    \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_EXPORTING_SCRIPT_ENC_SETTING_NAME)
    #define XSI_GAMEBRYO_VIEWING_SCRIPT_ENC_SETTING_NAME_W      \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_VIEWING_SCRIPT_ENC_SETTING_NAME)
    #define XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_NAME_W    \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_NAME)
    #define XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_NAME_W      \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_NAME)
    #define XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_NAME_W       \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_NAME)
    #define XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_NAME_W     \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_NAME)
    #define XSI_GAMEBRYO_EXPORT_SHAPE_ANIMS_SETTING_NAME_W  \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_EXPORT_SHAPE_ANIMS_SETTING_NAME)
    #define XSI_GAMEBRYO_EXPORT_SKINNINGS_SETTING_NAME_W    \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_EXPORT_SKINNINGS_SETTING_NAME)
    #define XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_NAME_W \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_NAME)
    #define XSI_GAMEBRYO_IGNORE_ALL_ERRORS_SETTING_NAME_W   \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_IGNORE_ALL_ERRORS_SETTING_NAME)
    #define XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_NAME_W      \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_NAME)
    #define XSI_GAMEBRYO_TEXTURE_FORMAT_SETTING_NAME_W      \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_TEXTURE_FORMAT_SETTING_NAME)
    #define XSI_GAMEBRYO_TEXTURE_COMPRESSION_SETTING_NAME_W \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_TEXTURE_COMPRESSION_SETTING_NAME)
    #define XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_NAME_W       \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_NAME)
    #define XSI_GAMEBRYO_LOG_FILENAME_SETTING_NAME_W        \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_LOG_FILENAME_SETTING_NAME)
    #define XSI_GAMEBRYO_OUT_FILENAME_SETTING_NAME_W        \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_OUT_FILENAME_SETTING_NAME)
    #define XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_NAME_W     \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_NAME)
    #define XSI_GAMEBRYO_MESH_PROFILE_SETTING_NAME_W        \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_MESH_PROFILE_SETTING_NAME)

    //---------------------------------------------------------------------------
    /// Crosswalk custom property containing the XSI Crosswalk export settings.
    //---------------------------------------------------------------------------

    #define XSI_GAMEBRYO_CROSSWALK_EXPORT_OPTIONS_NAME          \
        "ExportGamebryoCrosswalkOptions"
    #define XSI_GAMEBRYO_CROSSWALK_EXPORT_OPTIONS_NAME_W        \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_CROSSWALK_EXPORT_OPTIONS_NAME)

    //---------------------------------------------------------------------------
    /// Z-buffering custom property.
    //---------------------------------------------------------------------------

    #define XSI_GAMEBRYO_ZBUFFER_PROPERTY_NAME                  \
        "GamebryoZBuffer"
    #define XSI_GAMEBRYO_ZBUFFER_PROPERTY_NAME_W                \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_PROPERTY_NAME)

    #define XSI_GAMEBRYO_ZBUFFER_TEST_PARAM_NAME                \
        "Test"
    #define XSI_GAMEBRYO_ZBUFFER_TEST_PARAM_NAME_W              \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_TEST_PARAM_NAME)
    #define XSI_GAMEBRYO_ZBUFFER_WRITE_PARAM_NAME               \
        "Write"
    #define XSI_GAMEBRYO_ZBUFFER_WRITE_PARAM_NAME_W             \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_WRITE_PARAM_NAME)
    #define XSI_GAMEBRYO_ZBUFFER_FUNCTION_PARAM_NAME            \
        "Function"
    #define XSI_GAMEBRYO_ZBUFFER_FUNCTION_PARAM_NAME_W          \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_FUNCTION_PARAM_NAME)
    #define XSI_GAMEBRYO_ZBUFFER_ALWAYS_VALUE_NAME              \
        "Always"
    #define XSI_GAMEBRYO_ZBUFFER_ALWAYS_VALUE_NAME_W            \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_ALWAYS_VALUE_NAME)
    #define XSI_GAMEBRYO_ZBUFFER_LESS_VALUE_NAME                \
        "Less"
    #define XSI_GAMEBRYO_ZBUFFER_LESS_VALUE_NAME_W              \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_LESS_VALUE_NAME)
    #define XSI_GAMEBRYO_ZBUFFER_EQUAL_VALUE_NAME               \
        "Equal"
    #define XSI_GAMEBRYO_ZBUFFER_EQUAL_VALUE_NAME_W             \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_EQUAL_VALUE_NAME)
    #define XSI_GAMEBRYO_ZBUFFER_LESS_EQUAL_VALUE_NAME          \
        "LessEqual"
    #define XSI_GAMEBRYO_ZBUFFER_LESS_EQUAL_VALUE_NAME_W        \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_LESS_EQUAL_VALUE_NAME)
    #define XSI_GAMEBRYO_ZBUFFER_GREATER_VALUE_NAME             \
        "Greater"
    #define XSI_GAMEBRYO_ZBUFFER_GREATER_VALUE_NAME_W           \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_GREATER_VALUE_NAME)
    #define XSI_GAMEBRYO_ZBUFFER_NOT_EQUAL_VALUE_NAME           \
        "NotEqual"
    #define XSI_GAMEBRYO_ZBUFFER_NOT_EQUAL_VALUE_NAME_W         \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_NOT_EQUAL_VALUE_NAME)
    #define XSI_GAMEBRYO_ZBUFFER_GREATER_EQUAL_VALUE_NAME       \
        "GreaterEqual"
    #define XSI_GAMEBRYO_ZBUFFER_GREATER_EQUAL_VALUE_NAME_W     \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_GREATER_EQUAL_VALUE_NAME)
    #define XSI_GAMEBRYO_ZBUFFER_NEVER_VALUE_NAME               \
        "Never"
    #define XSI_GAMEBRYO_ZBUFFER_NEVER_VALUE_NAME_W             \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_ZBUFFER_NEVER_VALUE_NAME)

    //---------------------------------------------------------------------------
    /// Level-of-details custom property.
    //---------------------------------------------------------------------------

    #define XSI_GAMEBRYO_LOD_PROPERTY_NAME                      \
        "GamebryoLOD"
    #define XSI_GAMEBRYO_LOD_PROPERTY_NAME_W                    \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_LOD_PROPERTY_NAME)

    #define XSI_GAMEBRYO_LOD_GROUP_NAME_PARAM_NAME              \
        "GroupName"
    #define XSI_GAMEBRYO_LOD_GROUP_NAME_PARAM_NAME_W            \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_LOD_GROUP_NAME_PARAM_NAME)
    #define XSI_GAMEBRYO_LOD_DISTANCE_PARAM_NAME                \
        "Distance"
    #define XSI_GAMEBRYO_LOD_DISTANCE_PARAM_NAME_W              \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_LOD_DISTANCE_PARAM_NAME)

    //---------------------------------------------------------------------------
    /// Lighting custom property.
    ///
    /// The "GamebryoLighting" property is found only on lights.
    /// The "GamebryoShadow" property is found only on meshes.
    /// They serve to select affected objects for the light, enable
    /// or disable the shadow casting generation and to set the ignored
    /// meshes when computing shadows.
    ///
    /// Many parameters contain lists. Each list can be space- or comma-
    /// or semi-column- separated. The names in the list are arbitrary.
    /// They serve only to associate meshes with lights.
    ///
    /// The "LightGroups" parameter lists the groups the light or mesh
    /// is part of. A light group thus defines the affected meshes for
    /// a light: all meshes in the same group as the light will be lit.
    ///
    /// When exporting a scene, if a light has an associated model group
    /// (see XSI documentation about associated models for details),
    /// and it does *not* already have a "GamebryoLighting" property
    /// with a "LightGroups" parameter they are created with the light
    /// name as the group name. If multiple lights have the same name
    /// then a suffix will be added to subsequent lights to make
    /// the name unique.
    ///
    /// The "ShadowTechnique" parameter is found only on lights. 
    /// Its presence activate the shadow generation with the given
    /// technique. See the Gamebryo manual for valid technique names.
    /// By default, if the parameter exists but is empty, then the
    /// "NiStandardShadowTechnique" is used.
    ///
    /// The "IgnoredCasters" is found only on meshes. It lists
    /// the light groups that should *not* use this mesh to
    /// cast shadows.
    ///
    /// The "IgnoredReceivers" is found only on meshes. It lists
    /// the light groups that should *not* use this mesh to
    /// receive shadows.
    ///
    /// @note multiple lights can be part of the same group.
    ///       Beware that the automatic property generation
    ///       on meshes is based on the XSI associated models
    ///       for each light and thus all objects of all lists
    ///       will be part of the group.
    //---------------------------------------------------------------------------

    #define XSI_GAMEBRYO_LIGHTNING_PROPERTY_NAME                \
        "GamebryoLighting"
    #define XSI_GAMEBRYO_LIGHTNING_PROPERTY_NAME_W              \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_LIGHTNING_PROPERTY_NAME)

    #define XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME                \
        "LightGroups"
    #define XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME_W              \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME)
    #define XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_NAME            \
        "ShadowTechnique"
    #define XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_NAME_W          \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_NAME)
    #define XSI_GAMEBRYO_NONE_SHADOW_TECHNIQUE_NAME             \
        "None"
    #define XSI_GAMEBRYO_NONE_SHADOW_TECHNIQUE_NAME_W           \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_NONE_SHADOW_TECHNIQUE_NAME)
    #define XSI_GAMEBRYO_DEFAULT_SHADOW_TECHNIQUE_NAME          \
        "NiStandardShadowTechnique"
    #define XSI_GAMEBRYO_DEFAULT_SHADOW_TECHNIQUE_NAME_W        \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_DEFAULT_SHADOW_TECHNIQUE_NAME)
    #define XSI_GAMEBRYO_VSM_SHADOW_TECHNIQUE_NAME              \
        "NiVSMShadowTechnique"
    #define XSI_GAMEBRYO_VSM_SHADOW_TECHNIQUE_NAME_W            \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_VSM_SHADOW_TECHNIQUE_NAME)
    #define XSI_GAMEBRYO_PCF_SHADOW_TECHNIQUE_NAME              \
        "NiPCFShadowTechnique"
    #define XSI_GAMEBRYO_PCF_SHADOW_TECHNIQUE_NAME_W            \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_PCF_SHADOW_TECHNIQUE_NAME)

    #define XSI_GAMEBRYO_SHADOW_PROPERTY_NAME                   \
        "GamebryoShadow"
    #define XSI_GAMEBRYO_SHADOW_PROPERTY_NAME_W                 \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_SHADOW_PROPERTY_NAME)

    // See above for XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME
    // See above for XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME_W
    #define XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_NAME       \
        "IgnoredCasters"
    #define XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_NAME_W     \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_NAME)
    #define XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_NAME    \
        "IgnoredReceivers"
    #define XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_NAME_W  \
        XSI_GAMEBRYO_WIDE_STRING(XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_NAME)

}

#endif /* XSI_GAMEBRYO_CONVERTER_HELPERS_H */
