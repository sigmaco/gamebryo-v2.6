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

#ifndef NIMAXOPTIONS_H
#define NIMAXOPTIONS_H

#include "MaxImmerse.h"
#include <NiScriptInfoSet.h>

// persistent export options
#define NI_OPTIONS_SECTION "MAXIMMERSE"
#define MAXIMMERSE_VERSION_KEY "MIVersion"
#define NI_TEXTURES _T("Textures")
#define NI_USEOLDBEHAVIOR _T("UseOldBehavior")
#define NI_USEAREASUPERSPRAY _T("UseAreaSuperSpray")
#define NI_USEOLDSPINTIME _T("UseOldSpinTime")
#define NI_USEOLDNORMALS _T("UseOldNormals")
#define NI_IMAGE_FILE_DIR _T("ImageFileDir")
#define NI_EXPORT_PLATFORM _T("ExportPlatform")
#define NI_VIEWER_TYPE _T("ViewerType")
#define NI_CONVERT_CAMERAS _T("ExportCams")
#define NI_CONVERT_LIGHTS _T("ExportLights")
//#define NI_CONVERT_VERTEX_WEIGHTS _T("ExportVertexWeights")
#define NI_SCENEMESHPROFILENAME _T("SceneMeshProfile")
#define NI_USE_APPTIME _T("UseAppTime")
#define NI_CONVERT_CSASKEYFRAMES _T("ExportCSAsKeyframes")
#define NI_PER_TEXTURE_OVERRIDE _T("PerTextureOverride")
#define NI_PIXEL_LAYOUT _T("PixelLayout")
#define NI_EXTRACT_KEYFRAMES _T("ExtractKeyframes")
#define NI_EXTRACT_SEPERATE_SEQUENCES _T("ExtractToSepSequences")
#define NI_RESX _T("XRes")
#define NI_RESY _T("YRes")
#define NI_WINDOWED _T("Windowed")
#define NI_ORIGINAL_IMAGES _T("OriginalImages")
#define NI_SEPERATE_NIFS _T("SeparateNifs") 
#define NI_KEEP_IN_MAIN_NIF _T("MainNif")
#define NI_ORIGINAL_IMAGES_VRAM _T("OriginalImagesVRAM")
#define NI_VIEWER_STRIPIFY _T("ViewerStripify")
#define NI_VIEWPORT_MAX_INPUT_MODE _T("MiViewportMaxInputMode")
#define NI_VIEWPORT_MAX_ANIM_TIMING _T("MiViewportMaxAnimTiming")
#define NI_VIEWPORT_STRIPIFY _T("MiViewportStripify")
#define NI_VIEWPORT_MAX_UIMAP _T("MiViewportUIMap")
#define NI_FILENAME _T("NiFilename")
#define NI_SKINNING_THRESHOLD _T("SkinningThreshold")
#define NI_USE_TRUE_COLOR _T("TrueColor")
#define NI_USE_COMPRESSED_COLOR _T("CompressedColor")
#define NI_USE_PALETTED_COLOR _T("PalettedColor")
#define NI_USE_HIGH_COLOR _T("HighColor")
#define NI_WRITE_ERRORS_TO_FILE _T("WriteErrorsToFile")
#define NI_LOG_ERRORS_PROMPT   _T("ShowErrorLogPrompt")
#define NI_LAST_ERROR_PROMPT_RETURN   _T("LastErrorLogPromptReturn")
#define NI_USE_OLD_PARTICLE_EXPORTER _T("UseOldParticleExporter")
#define NI_ALLOW_NPOT_TEXTURES _T("AllowNpotTextures")

// script-related options
#define NI_USE_TEMPLATE_FOR_SCRIPT  "USE_SCRIPT_TEMPLATES"
#define NI_PROCESS_SCRIPT_NAME      "PROCESS_SCRIPT_NAME"
#define NI_DX9_VIEWER_SCRIPT_NAME   "DX9_VIEWER_SCRIPT_NAME"
#define NI_D3D10_VIEWER_SCRIPT_NAME "D3D10_VIEWER_SCRIPT_NAME"
#define NI_CUSTOM_VIEWER_SCRIPT_NAME "CUSTOM_VIEWER_SCRIPT_NAME"
#define NI_ANIMATION_TOOL_SCRIPT_NAME   "ANIMATION_VIEWER_SCRIPT_NAME"
#define NI_XBOX_360_SCRIPT_NAME     "XBOX_360_VIEWER_SCRIPT_NAME"
#define NI_PS3_SCRIPT_NAME          "PS3_VIEWER_SCRIPT_NAME"
#define NI_WII_SCRIPT_NAME          "WII_VIEWER_SCRIPT_NAME"
#define NI_PHYSX_SCRIPT_NAME        "PHYSX_VIEWER_SCRIPT_NAME"
#define NI_VIEWPORT_SCRIPT_NAME     "VIEWPORT_SCRIPT_NAME"
#define NI_SCENEIMMERSE_SCRIPT_NAME "SCENEIMMERSE_VIEWER_SCRIPT_NAME"
#define NI_D3D10_SCENEIMMERSE_SCRIPT_NAME "D3D10_SCENEIMMERSE_VIEWER_SCRIPT_NAME"
#define NI_EXPORT_SCRIPT_NAME       "EXPORT_SCRIPT_NAME"
#define NI_DX9_VIEWER_SCRIPT        "DX9_VIEWER_SCRIPT"
#define NI_D3D10_VIEWER_SCRIPT      "D3D10_VIEWER_SCRIPT"
#define NI_CUSTOM_VIEWER_SCRIPT     "CUSTOM_VIEWER_SCRIPT"
#define NI_VIEWPORT_SCRIPT          "VIEWPORT_SCRIPT"
#define NI_SCENEIMMERSE_SCRIPT      "SCENEIMMERSE_VIEWER_SCRIPT"
#define NI_D3D10_SCENEIMMERSE_SCRIPT      "D3D10_SCENEIMMERSE_VIEWER_SCRIPT"
#define NI_EXPORT_SCRIPT            "EXPORT_SCRIPT"
#define NI_PROCESS_SCRIPT           "PROCESS_SCRIPT"
#define NI_ANIMATION_TOOL_SCRIPT    "ANIMATION_VIEWER_SCRIPT"
#define NI_XBOX_360_SCRIPT          "XBOX_360_VIEWER_SCRIPT"
#define NI_PS3_SCRIPT               "PS3_VIEWER_SCRIPT"
#define NI_WII_SCRIPT               "WII_VIEWER_SCRIPT"
#define NI_PHYSX_SCRIPT             "PHYSX_VIEWER_SCRIPT"

#define NI_DEFAULTSCENEMESHPROFILENAME _T("Default")

#define NI_NETWORK_SCRIPT_PATH      _T("NetworkScriptPath")

#include <NiString.h>
#include <NiCriticalSection.h>

class NiPluginInfo;

class NiMAXOptions
{
    public:
        static void CreateOptions();
        static void CreateViewerOptions();
        static void DestroyOptions();
        
        static bool MatchesCurrentVersion(Interface* pkInterface);
        static void SetDefaultOptions();
        static void SetDefaultViewerOptions();
        static void ReadOptions(Interface* pkInterface);
        static void WriteOptions(Interface* pkInterface);

        static void RemoveOptionsExtraData(NiNode* pkNode);
        static void RemoveDeprecatedOptions(Interface* pkIntf);

        static NiString GetValue(const char* strKey);
        static void SetValue(const char* strKey, const char* strValue);

        static bool GetBool(const char* strKey);
        static void SetBool(const char* strKey, bool b);
        static void ToggleBool(const char* strKey);

        static int GetInt(const char* strKey);
        static void SetInt(const char* strKey, int iValue);

        static NiScriptInfoPtr GetExportScript();
        static NiScriptInfoPtr GetProcessScript();
        static NiScriptInfoPtr GetDX9ViewerScript();
        static NiScriptInfoPtr GetD3D10ViewerScript();
        static NiScriptInfoPtr GetSceneImmerseViewerScript();
        static NiScriptInfoPtr GetD3D10SceneImmerseViewerScript();
        static NiScriptInfoPtr GetCustomViewerScript();
        static NiScriptInfoPtr GetViewportScript();
        static NiScriptInfoPtr GetAnimationViewerScript();
        static NiScriptInfoPtr GetXenonViewerScript();
        static NiScriptInfoPtr GetPS3ViewerScript();
        static NiScriptInfoPtr GetWiiViewerScript();
        static NiScriptInfoPtr GetPhysXViewerScript();

        static void SetProcessScript(NiScriptInfo* pkScript);
        static void SetExportScript(NiScriptInfo* pkScript);
        static void SetDX9ViewerScript(NiScriptInfo* pkScript);
        static void SetD3D10ViewerScript(NiScriptInfo* pkScript);
        static void SetD3D10SceneImmerseViewerScript(NiScriptInfo* pkScript);
        static void SetSceneImmerseViewerScript(NiScriptInfo* pkScript);
        static void SetCustomViewerScript(NiScriptInfo* pkScript);
        static void SetViewportScript(NiScriptInfo* pkScript);
        static void SetAnimationViewerScript(NiScriptInfo* pkScript);
        static void SetXenonViewerScript(NiScriptInfo* pkScript);
        static void SetPS3ViewerScript(NiScriptInfo* pkScript);
        static void SetWiiViewerScript(NiScriptInfo* pkScript);
        static void SetPhysXViewerScript(NiScriptInfo* pkScript);

        static NiSystemDesc::RendererID
            ConvertDevImageToNiSystemDescRenderer(
            NiDevImageConverter::Platform ePlatform);

        static NiDevImageConverter::Platform
            ConvertNiSystemDescRendererToDevImage(
            NiSystemDesc::RendererID ePlatform);

        static bool IsCustomViewerAvailable();
        static bool IsDX9ViewerAvailable();
        static bool IsD3D10ViewerAvailable();
        static bool IsSceneImmerseViewerAvailable();
        static bool IsAnimationViewerAvailable();
        static bool IsXenonViewerAvailable();
        static bool IsPS3ViewerAvailable();
        static bool IsWiiViewerAvailable();
        static bool IsPhysXViewerAvailable();

        static NiScriptInfoSet* GenerateAvailableProcessScripts();
        static NiScriptInfoSet* GenerateAvailableExportScripts();

        static unsigned int GetOptionsCount();
        static NiString GetNameAt(unsigned int ui);
        static NiString GetValueAt(unsigned int ui);

        static NiCriticalSection g_kOptionsCS;

        static NiString GetNetworkScriptDirectory();
        static void HandleValue(NiString strKey, NiString strValue);
        
    protected:
        static NiPluginInfo* ms_pkDatabase;
        static bool HandleDeprecatedNodeOptions(Interface* pkInterface);
        static void HandleDeprecatedINIOptions(Interface* pkInterface);
        static bool SaveInNode(NiString strKey);
        static bool ms_bFirst;

        static NiScriptInfoPtr ms_spLastExportScript;
        static NiScriptInfoPtr ms_spLastProcessScript;
        static NiScriptInfoPtr ms_spLastDX9ViewerScript;
        static NiScriptInfoPtr ms_spLastD3D10ViewerScript;
        static NiScriptInfoPtr ms_spLastSceneImmerseViewerScript;
        static NiScriptInfoPtr ms_spLastD3D10SceneImmerseViewerScript;
        static NiScriptInfoPtr ms_spLastCustomViewerScript;
        static NiScriptInfoPtr ms_spLastViewportScript;
        static NiScriptInfoPtr ms_spLastAnimationToolViewerScript;
        static NiScriptInfoPtr ms_spLastXenonViewerScript;
        static NiScriptInfoPtr ms_spLastPS3ViewerScript;
        static NiScriptInfoPtr ms_spLastWiiViewerScript;
        static NiScriptInfoPtr ms_spLastPhysXViewerScript;
};
#endif