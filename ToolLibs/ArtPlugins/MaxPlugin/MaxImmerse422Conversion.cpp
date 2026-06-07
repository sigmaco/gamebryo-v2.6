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

#include "MaxImmerse.h"
#include "MaxImmerse422Conversion.h"

#pragma warning(push)
// unreferenced formal parameter
#pragma warning(disable: 4100)
// nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)
// nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable: 4238)
// nonstandard extension used : 'default argument' : conversion from '' to ' &'
#pragma warning(disable: 4239)
// 'argument' : conversion from '' to '', signed/unsigned mismatch
#pragma warning(disable: 4245)
// '' : assignment operator could not be generated
#pragma warning(disable: 4512)

#undef STRICT // Avoid a warning b/c Maxscrpt.h defines STRICT too
#include "Maxscrpt.h"
#include "Name.h"
#include "Numbers.h"
#include "Strings.h"
#include "Arrays.h" 
#include "definsfn.h"   // generate static instances from def_x macros for     
                        // MaxScript
#pragma warning(pop)


#include "NiSceneGraphOptimizationDefines.h"
#include "NiStripifyDLLDefines.h"

#define NI_422_SECTION_NAME _T("MImmerse")
#define NI_422_TEXTURES_NODE_KEY _T("NetImmerseTextures")
#define NI_422_PLATFORM_NODE_KEY _T("NetImmerseTargetPlatform")
#define NI_422_IMAGE_FILE_DIR_NODE_KEY _T("NetImmerseImageFileDir")
#define NI_422_CONVERT_CAMERAS_NODE_KEY _T("NetImmerseExportCams")
#define NI_422_CONVERT_LIGHTS_NODE_KEY _T("NetImmerseExportLights")
#define NI_422_CONVERT_VERTEX_WEIGHTS_NODE_KEY \
    _T("NetImmerseExportVertexWeights")
#define NI_422_USE_APPTIME_NODE_KEY _T("NetImmerseUseAppTime")
#define NI_422_CONVERT_CSASKEYFRAMES_NODE_KEY \
    _T("NetImmerseExportCSAsKeyframes")
#define NI_422_PER_TEXTURE_OVERRIDE_NODE_KEY \
    _T("NetImmersePerTextureOverride")
#define NI_422_PIXEL_LAYOUT_NODE_KEY _T("NetImmersePixelLayout")
#define NI_422_EXTRACT_KEYFRAMES_NODE_KEY _T("NetImmerseExtractKeyframes")
#define NI_422_EXTRACT_SEPERATE_SEQUENCES_NODE_KEY \
    _T("NetImmerseExtractToSepSequences")
#define NI_422_CONVERT_UNUSED_UVS_NODE_KEY _T("NetImmerseConvertUnusedUVs")
#define NI_422_CONVERT_UNUSED_UVS _T("NetImmerseConvertUnusedUVs")
#define NI_422_IK_SOLVER _T("IKSolver")
#define NI_422_IK_SOLVE_MODE _T("IKSolveMode")
#define NI_422_TOOL_IPADDRESS _T("ToolIPAddress")
#define NI_422_TARGETNAME _T("TargetName")
#define NI_422_XBOXTARGETNAME _T("XBoxTargetName")
#define NI_422_FIELDMODE _T("FieldMode")
#define NI_422_PS2NTSC _T("PS2NTSC")
#define NI_422_PS2Z24 _T("PS2Z24")
#define NI_422_UIKEY _T("UISelection")
#define NI_422_UICUSTOMFILENAME _T("UISelectedFilename")
#define NI_422_CSASKEYFRAME _T("CSAsKeyframe")
#define NI_422_RENDERER_KEY _T("Renderer")
#define NI_422_RESX_KEY _T("XRes")
#define NI_422_RESY_KEY _T("YRes")
#define NI_422_WINDOWED_KEY _T("Windowed")
#define NI_422_VIEWER_STRIPIFY_KEY _T("ViewerStripify")
#define NI_422_CSASKEYFRAME_KEY _T("CSAsKeyframe")
#define NI_422_IK_SOLVER_KEY _T("IKSolver")
#define NI_422_IK_SOLVE_MODE_KEY _T("IKSolveMode")
#define NI_422_TOOL_IPADDRESS_KEY _T("ToolIPAddress")
#define NI_422_TARGETNAME_KEY _T("TargetName")
#define NI_422_XBOXTARGETNAME_KEY _T("XBoxTargetName")
#define NI_422_FIELDMODE_KEY _T("FieldMode")
#define NI_422_PS2NTSC_KEY _T("PS2NTSC")
#define NI_422_PS2Z24_KEY _T("PS2Z24")
#define NI_422_UIKEY _T("UISelection")
#define NI_422_UICUSTOMFILENAME _T("UISelectedFilename")
#define NI_422_TEXTURES _T("Textures")
#define NI_422_USEOLDBEHAVIOR _T("UseOldBehavior")
#define NI_422_USEAREASUPERSPRAY _T("UseAreaSuperSpray")
#define NI_422_USEOLDSPINTIME _T("UseOldSpinTime")
#define NI_422_USEOLDNORMALS _T("UseOldNormals")
#define NI_422_IMAGE_FILE_DIR _T("ImageFileDir")
#define NI_422_PLATFORM _T("Platform")
#define NI_422_OPTIMIZE_REMOVE_DUP_PROPERTIES_KEY \
    _T("NiOptimizeRemoveDupProperties")
#define NI_422_OPTIMIZE_REMOVE_CHILDLESS_NODES_KEY \
    _T("NiOptimizeRemoveChildlessNodes")
#define NI_422_OPTIMIZE_REMOVE_MULTIMAT_NODES_KEY \
    _T("NiOptimizeRemoveMultiMtlNodes")
#define NI_422_OPTIMIZE_REMOVE_SINGLE_CHILD_NODES_KEY \
    _T("NiOptimizeRemoveSingleChildNodes")
#define NI_422_OPTIMIZE_MERGE_SIBLING_NODES_KEY \
    _T("NiOptimizeMergeSiblingNodes")
#define NI_422_OPTIMIZE_TRISHAPES_KEY _T("NiOptimizeOptimizeTriShapes")
#define NI_422_OPTIMIZE_MERGE_SIBLING_TRISHAPES_KEY \
    _T("NiOptimizeMergeSiblingTriShapes")
#define NI_422_OPTIMIZE_NO_MERGE_ON_ALPHA_KEY \
    _T("NiOptimizeNoMergeOnAlpha")
#define NI_422_OPTIMIZE_REMOVE_EXTRA_UV_SETS_KEY \
    _T("NiOptimizeRemoveExtraUVSets")
#define NI_422_OPTIMIZE_REMOVE_EXTRA_NORMALS_KEY \
    _T("NiOptimizeRemoveUnnecessaryNormals")
#define NI_422_OPTIMIZE_REMOVE_EXTRA_VISCONTROLLERS_KEY \
    _T("NiOptimizeRemoveUnnecessaryVisControllers")
#define NI_422_OPTIMIZE_REMOVE_NAMES_KEY \
    _T("NiOptimizeRemoveNames")
#define NI_422_OPTIMIZE_NAME_UNNAMED_KEY \
    _T("NiOptimizeNameUnnamedObjects")
#define NI_422_OPTIMIZE_REMOVE_BAD_TEXPROPS_KEY \
    _T("NiOptimizeRemoveBadTexturingProps")
#define NI_422_OPTIMIZE_REMOVE_EXTRA_DATA_KEY \
    _T("NiOptimizeRemoveUnnecessaryExtraData")
#define NI_422_OPTIMIZE_REMOVE_SGO_KEEP_KEY \
    _T("NiOptimizeRemoveSgoKeepTags")
#define NI_422_OPTIMIZE_REMOVE_EXTRA_LOOKAT_KEY \
    _T("NiOptimizeRemoveUnnecessaryLookAtControllers")
#define NI_422_OPTIMIZE_REMOVE_HIDDEN_BONES_KEY \
    _T("NiOptimizeRemoveHiddenBoneGeometry")
#define NI_422_OPTIMIZE_REDUCE_ANIM_KEYS_KEY \
    _T("NiOptimizeReduceAnimationKeys")
#define NI_422_OPTIMIZE_ANIMATION_REDUCTION_TOLERANCE_KEY \
    _T("NiOptimizeAnimationKeyReductionTolerance")
#define NI_422_OPTIMIZE_SORT_BY_TEXPROP_KEY \
    _T("NiOptimizeSortByTexturingProp")
#define NI_422_OPTIMIZE_CONVERT_EXTERNAL_TEX2NIF_KEY \
    _T("NiOptimizeConvertExternalTexturesToNIF")
#define NI_422_OPTIMIZE_STRIPIFY_KEY _T("NiOptimizeStripify")
#define NI_422_OPTIMIZE_STRIPIFY_PLATFORM_KEY \
    _T("NiOptimizeStripifyPlatform")
#define NI_422_OPTIMIZE_STRIPIFY_ALGORITHM_KEY \
    _T("NiOptimizeStripifyAlgorithm")
#define NI_422_OPTIMIZE_STRIPIFY_CACHE_KEY \
    _T("NiOptimizeStripifyCacheSize")
#define NI_422_OPTIMIZE_CREATE_ABVS _T("NiOptimizeCreateABVs")
#define NI_422_OPTIMIZE_REMOVE_ABV_PROXY_GEOM \
    _T("NiOptimizeRemoveABVProxyGeometry")
#define NI_422_OPTIMIZE_EXPORT_HARDWARE_SKIN \
    _T("NiOptimizeExportHardwareSkin")
#define NI_422_OPTIMIZE_EXPORT_SOFTWARE_SKIN \
    _T("NiOptimizeExportSoftwareSkin")
#define NI_422_OPTIMIZE_BONES_PER_PARTITION \
    _T("NiOptimizeBonesPerPartition")
#define NI_422_TEXTURES_NODE_KEY _T("NetImmerseTextures")
#define NI_422_PLATFORM_NODE_KEY _T("NetImmerseTargetPlatform")
#define NI_422_IMAGE_FILE_DIR_NODE_KEY _T("NetImmerseImageFileDir")
#define NI_422_CONVERT_CAMERAS_NODE_KEY _T("NetImmerseExportCams")
#define NI_422_CONVERT_LIGHTS_NODE_KEY _T("NetImmerseExportLights")
#define NI_422_CONVERT_VERTEX_WEIGHTS_NODE_KEY \
    _T("NetImmerseExportVertexWeights")
#define NI_422_USE_APPTIME_NODE_KEY _T("NetImmerseUseAppTime")
#define NI_422_CONVERT_CSASKEYFRAMES_NODE_KEY \
     _T("NetImmerseExportCSAsKeyframes")
#define NI_422_PER_TEXTURE_OVERRIDE_NODE_KEY \
    _T("NetImmersePerTextureOverride")
#define NI_422_PIXEL_LAYOUT_NODE_KEY _T("NetImmersePixelLayout")
#define NI_422_EXTRACT_KEYFRAMES_NODE_KEY _T("NetImmerseExtractKeyframes")
#define NI_422_EXTRACT_SEPERATE_SEQUENCES_NODE_KEY \
    _T("NetImmerseExtractToSepSequences")
#define NI_422_CONVERT_UNUSED_UVS_NODE_KEY _T("NetImmerseConvertUnusedUVs")
#define NI_422_VIEWPORT_MAX_INPUT_MODE _T("MiViewportMaxInputMode")
#define NI_422_VIEWPORT_MAX_ANIM_TIMING _T("MiViewportMaxAnimTiming")
#define NI_422_VIEWPORT_STRIPIFY _T("MiViewportStripify")
#define NI_422_VIEWPORT_MAX_UIMAP _T("MiViewportUIMap")

enum StripifyPlatform
{
    PLATFORM_NULL       = -1,
    PLATFORM_DX7        = 0,    //  Win32, DirectX7
    PLATFORM_DX8,               //  Win32, DirectX8
    PLATFORM_OGL,               //  ?????, DirectX7
    PLATFORM_NGC,               //  Nintendo GameCube
    PLATFORM_PS2,               //  Sony PlayStation2
    PLATFORM_XBOX,              //  Microsoft Xbox
    //  New platforms go here
    PLATFORM_COUNT
};

enum StripifyAlgorithm
{
    ALGORITHM_NI        = 0,    //  NetImmerse
    ALGORITHM_NV,               //  nVidia
    //  New algorithms go here
    ALGORITHM_COUNT
};

//  Pre-defined caches...
enum StripifyCacheSize
{
    CACHE_GEFORCE1_2    = 0,    //  GeForce1 or GeForce2
    CACHE_GEFORCE3,             //  GeForce3
    //  New caches go here
    CACHE_COUNT
};

bool ReadConfigFromNode(INode* pkNode, NiPluginInfo* pkStripify, 
                NiPluginInfo* pkOptimize, NiPluginInfo* pkABV);
void ReadBehaviorConfig(Interface* pkIntf, NiPluginInfo* pkStripify, 
                NiPluginInfo* pkOptimize, NiPluginInfo* pkABV);
void ReadConfig(Interface* pkIntf, NiPluginInfo* pkStripify, 
                NiPluginInfo* pkOptimize, NiPluginInfo* pkABV);
bool ReadOptimizationSettingsFromNode(INode* pkNode, NiPluginInfo* 
    pkStripify, NiPluginInfo* pkOptimize, NiPluginInfo* pkABV);

//---------------------------------------------------------------------------
bool MaxImmerse422Conversion::NeedsINIConversion(Interface* pkInterface)
{
    TCHAR acFilename[MAX_PATH];

    GetCfgFilename(pkInterface, acFilename, MAX_PATH);
    char acString[1024];

    GetPrivateProfileSectionNames(acString, 1024, acFilename);
    
    NiString strTemp(acString);

    if (strTemp.Find(NI_OPTIONS_SECTION) != NIPT_INVALID_INDEX)
        return false;
    if (strTemp.Find(NI_422_SECTION_NAME) != NIPT_INVALID_INDEX)
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
bool MaxImmerse422Conversion::NeedsNodeConversion(Interface* pkInterface)
{
    INode* pkNode = pkInterface->GetRootNode();
    if (pkNode)
    {
        TSTR strBuf;
        pkNode->GetUserPropBuffer(strBuf);
        NiString strTemp = (const char*) strBuf;
        if (strTemp.IsEmpty())
            return false;
        if (strTemp.Find(MAXIMMERSE_VERSION_KEY) == NIPT_INVALID_INDEX)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void MaxImmerse422Conversion::ConvertINIOptions(Interface* pkInterface)
{

    ReadConfig(pkInterface, NULL, NULL, NULL);

}
//---------------------------------------------------------------------------
void MaxImmerse422Conversion::ConvertNodeOptions(Interface* pkInterface)
{
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    INode* pkNode = pkInterface->GetRootNode();
    if (pkNode) 
    {
        NiScriptInfo* pkInfo = NiNew NiScriptInfo();
        pkInfo->SetName("422 Converted Script");
        pkInfo->SetType("Process");
        
        NiPluginInfoPtr pkOptimize = NULL;
        NiPluginInfoPtr pkABV = NULL;
        NiPluginInfoPtr pkStripify = NULL;
        NiPluginPtr spPlugin = NULL;

        if (!pkABV)
        {
            spPlugin = pkManager->GetPluginByClass("NiCreateABVPlugin");

            if (spPlugin)
            {
                pkABV = spPlugin->GetDefaultPluginInfo();

                pkInfo->AddPluginInfo(pkABV);
            }
        }

        if (!pkOptimize)
        {
            spPlugin = pkManager->GetPluginByClass(
                "NiSceneGraphOptimizationPlugin");

            if (spPlugin)
            {
                pkOptimize = spPlugin->GetDefaultPluginInfo();
                pkInfo->AddPluginInfo(pkOptimize);
            }
        }

        if (!pkStripify)
        {
            spPlugin = 
                pkManager->GetPluginByClass("NiStripifyGeometryPlugin");

            if (spPlugin)
            {
                pkStripify = spPlugin->GetDefaultPluginInfo();
                pkInfo->AddPluginInfo(pkStripify);
            }
        }


        ReadOptimizationSettingsFromNode(pkNode, pkStripify,
            pkOptimize, pkABV);
        ReadConfigFromNode(pkNode, pkStripify,
            pkOptimize, pkABV);

        NiMAXOptions::SetProcessScript(pkInfo);

    }
}

//---------------------------------------------------------------------------
bool ReadOptimizationSettingsFromNode(INode* pkNode, 
    NiPluginInfo*, NiPluginInfo* pkOptimize, NiPluginInfo* pkABV)
{
    int iVal = 0;
    
    if(pkNode == NULL)
        return false;

    NiScriptInfo* pkScript = NiMAXOptions::GetProcessScript();
    
    if (!pkScript)
        return false;
    
    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_CREATE_ABVS,iVal) && 
       iVal == 1)
    {
        if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_ABV_PROXY_GEOM, 
            iVal) && pkABV)
           pkABV->SetBool("RemoveProxyGeometry",ToBool(iVal));
    }
    else
    {
        pkScript->RemovePlugin(pkABV);
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_DUP_PROPERTIES_KEY, 
        iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_DUP_PROPERTIES,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_CHILDLESS_NODES_KEY,
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_CHILDLESS_NODES ,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_MULTIMAT_NODES_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_MULTIMTL_NODES ,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(
        NI_422_OPTIMIZE_REMOVE_SINGLE_CHILD_NODES_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_SINGLE_CHILD_NODES ,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_MERGE_SIBLING_NODES_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_MERGE_SIBLING_NODES ,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_TRISHAPES_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_OPTIMIZE_TRISHAPES,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_MERGE_SIBLING_TRISHAPES_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_MERGE_SIBLING_TRISHAPES ,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_NO_MERGE_ON_ALPHA_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_NO_MERGE_ON_ALPHA ,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_EXTRA_UV_SETS_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_EXTRA_UVSETS ,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_EXTRA_NORMALS_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_UNNECESSARY_NORMALS,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(
       NI_422_OPTIMIZE_REMOVE_EXTRA_VISCONTROLLERS_KEY, 
       iVal) && pkOptimize)
    {
       pkOptimize->SetBool(OPT_REMOVE_UNNECESSARY_VISCONTROLLERS,
          ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_NAMES_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_NAMES,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_NAME_UNNAMED_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_NAME_UNNAMED_OBJECTS,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_BAD_TEXPROPS_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_BAD_TEXTURING_PROPS ,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_EXTRA_DATA_KEY, 
       iVal) && pkOptimize)
    {
       pkOptimize->SetBool(OPT_REMOVE_UNNECESSARY_EXTRA_DATA,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_SGO_KEEP_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_SGOKEEP_TAGS,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_EXTRA_LOOKAT_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_UNNECESSARY_LOOKATCONTROLLERS,
            ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REMOVE_HIDDEN_BONES_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REMOVE_HIDDEN_BONE_GEOMETRY ,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_REDUCE_ANIM_KEYS_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_REDUCE_ANIMATION_KEYS,ToBool(iVal));
    }
    
    float fVal = 0.0;
    if(pkNode->GetUserPropFloat(
       NI_422_OPTIMIZE_ANIMATION_REDUCTION_TOLERANCE_KEY, 
       fVal) && pkOptimize)
    {
       pkOptimize->SetValue(OPT_ANIMATION_KEY_REDUCTION_TOLERANCE, 
           NiString::FromFloat(fVal));
    }
    
    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_SORT_BY_TEXPROP_KEY, 
       iVal) && pkOptimize)
    {
        pkOptimize->SetBool(OPT_SORT_BY_TEXTURING_PROP,ToBool(iVal));
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_EXPORT_HARDWARE_SKIN, iVal) 
        && pkOptimize)
    {
        pkOptimize->SetBool(OPT_EXPORT_HARDWARE_SKIN, ToBool(iVal));
    }
    
    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_EXPORT_SOFTWARE_SKIN, iVal) 
        && pkOptimize)
    {
        pkOptimize->SetBool(OPT_EXPORT_SOFTWARE_SKIN,ToBool(iVal));
    }
    
    if(pkNode->GetUserPropInt(NI_422_OPTIMIZE_BONES_PER_PARTITION, iVal) 
        && pkOptimize)
    {
        pkOptimize->SetInt(OPT_BONES_PER_PARTITION, iVal);
    }
    
    return true;
}
//---------------------------------------------------------------------------
void ReadConfig(Interface* pkIntf, NiPluginInfo* pkStripify, 
                NiPluginInfo* pkOptimize, NiPluginInfo* pkABV)
{
    TCHAR acFilename[MAX_PATH];
    TCHAR acInput[40];

    GetCfgFilename(pkIntf, acFilename, MAX_PATH);

    int iTex = GetPrivateProfileInt(NI_422_SECTION_NAME, 
        NI_422_TEXTURES, IDC_TEX_SEPARATE, acFilename);

    switch (iTex)
    {
    case IDC_TEX_ORIG:
        NiMAXOptions::SetValue(NI_TEXTURES, NI_ORIGINAL_IMAGES);
        break;
    case IDC_TEX_ORIG_VRAM:
        NiMAXOptions::SetValue(NI_TEXTURES, NI_ORIGINAL_IMAGES_VRAM);
        break;
    case IDC_TEX_SEPARATE:
        NiMAXOptions::SetValue(NI_TEXTURES, NI_SEPERATE_NIFS);
        break;
    case IDC_TEX_MAINNIF:
        NiMAXOptions::SetValue(NI_TEXTURES, NI_KEEP_IN_MAIN_NIF);
        break;
    }

    NiDevImageConverter::Platform ePlatform = 
        (NiDevImageConverter::Platform)
        GetPrivateProfileInt(NI_422_SECTION_NAME, 
        NI_422_PLATFORM, NiDevImageConverter::ANY, acFilename);

    NiSystemDesc::RendererID eNewRenderer = 
        NiMAXOptions::ConvertDevImageToNiSystemDescRenderer(ePlatform);

    NiMAXOptions::SetInt(NI_EXPORT_PLATFORM, (int)eNewRenderer);

    GetPrivateProfileString(NI_422_SECTION_NAME, NI_422_IMAGE_FILE_DIR,
                            _T(""), acInput, 40, acFilename);
    NiMAXOptions::SetValue(NI_IMAGE_FILE_DIR, acInput);

    ReadBehaviorConfig(pkIntf, pkStripify,
            pkOptimize, pkABV);
}
//---------------------------------------------------------------------------
void ReadBehaviorConfig(Interface* pkIntf, NiPluginInfo*, 
                NiPluginInfo*, NiPluginInfo*)
{
    TCHAR acFilename[MAX_PATH];

    GetCfgFilename(pkIntf, acFilename, MAX_PATH);

    int iBehavior = GetPrivateProfileInt(NI_422_SECTION_NAME, 
        NI_422_USEOLDBEHAVIOR, false, acFilename);

    NiMAXOptions::SetBool(NI_USEOLDBEHAVIOR, ToBool(iBehavior));

    int iAreaSuperSpray = GetPrivateProfileInt(NI_422_SECTION_NAME, 
        NI_422_USEAREASUPERSPRAY, false, acFilename);


    NiMAXOptions::SetBool(NI_USEAREASUPERSPRAY, ToBool(iAreaSuperSpray));

    int iUseOldSpinTime = GetPrivateProfileInt(NI_422_SECTION_NAME, 
        NI_422_USEOLDSPINTIME, false, acFilename);

    NiMAXOptions::SetBool(NI_USEOLDSPINTIME, ToBool(iUseOldSpinTime));

    int iUseOldNormals = GetPrivateProfileInt(NI_422_SECTION_NAME, 
        NI_422_USEOLDNORMALS, false, acFilename);

    NiMAXOptions::SetBool(NI_USEOLDNORMALS, ToBool(iUseOldNormals));
}
//---------------------------------------------------------------------------
bool ReadConfigFromNode(INode* pkNode, NiPluginInfo* pkStripify, 
                NiPluginInfo*, NiPluginInfo*)
{
    if(pkNode == NULL)
        return false;

    int iVal = 0;
    TSTR kStr;

    if(!pkNode->GetUserPropInt(NI_422_TEXTURES_NODE_KEY, iVal))
        return false;

    switch (iVal)
    {
    case IDC_TEX_ORIG:
        NiMAXOptions::SetValue(NI_TEXTURES, NI_ORIGINAL_IMAGES);
        break;
    case IDC_TEX_ORIG_VRAM:
        NiMAXOptions::SetValue(NI_TEXTURES, NI_ORIGINAL_IMAGES_VRAM);
        break;
    case IDC_TEX_SEPARATE:
        NiMAXOptions::SetValue(NI_TEXTURES, NI_SEPERATE_NIFS);
        break;
    case IDC_TEX_MAINNIF:
        NiMAXOptions::SetValue(NI_TEXTURES, NI_KEEP_IN_MAIN_NIF);
        break;
    }

    if(pkNode->GetUserPropBool(NI_422_OPTIMIZE_STRIPIFY_KEY, iVal))
    {
        if (iVal == 1 && pkStripify)
        {
            pkStripify->SetValue(STRIP_PC_SETTINGS, STRIP_PC_NEXTGEN_VALUE);
            pkStripify->SetValue(STRIP_NEXTGENXBOX_SETTINGS, 
                STRIP_NEXTGENXBOX_VALUE);

            StripifyPlatform eStripifyPlatform = PLATFORM_DX8;
            StripifyAlgorithm eStripifyAlgorithm = ALGORITHM_NV;
            StripifyCacheSize eStripifyCacheSize = CACHE_GEFORCE3;

            if (pkNode->GetUserPropInt(
                NI_422_OPTIMIZE_STRIPIFY_PLATFORM_KEY, iVal))
            {
                eStripifyPlatform = (StripifyPlatform) iVal;
            }
            if (pkNode->GetUserPropInt(
                NI_422_OPTIMIZE_STRIPIFY_ALGORITHM_KEY,
                iVal))
            {
                eStripifyAlgorithm = (StripifyAlgorithm)iVal;
            }
            if (pkNode->GetUserPropInt(NI_422_OPTIMIZE_STRIPIFY_CACHE_KEY,
                iVal))
            {
                eStripifyCacheSize = (StripifyCacheSize) iVal;
            }

            if (eStripifyPlatform == PLATFORM_DX8 && 
                eStripifyAlgorithm == ALGORITHM_NV)
            {
                if (eStripifyCacheSize == CACHE_GEFORCE3)
                {
                    pkStripify->SetValue(STRIP_PC_SETTINGS, 
                        STRIP_PC_NEXTGEN_VALUE);
                }
                else
                {
                    pkStripify->SetValue(STRIP_PC_SETTINGS, 
                        STRIP_PC_FIRSTGEN_VALUE);
                }
            }
            else if (eStripifyPlatform == PLATFORM_XBOX)
            {
                pkStripify->SetValue(STRIP_NEXTGENXBOX_SETTINGS, 
                    STRIP_NEXTGENXBOX_VALUE);
            }
            else
            {
                pkStripify->SetValue(STRIP_PC_SETTINGS, 
                    STRIP_PC_UNACCELERATED_VALUE );
            }
        }
        else if (pkStripify)
        {
            pkStripify->SetValue(STRIP_PC_SETTINGS, 
                STRIP_DO_NOT_STRIPIFY_VALUE);
            pkStripify->SetValue(STRIP_NEXTGENXBOX_SETTINGS, 
                STRIP_DO_NOT_STRIPIFY_VALUE);
        }
    }

    if(!pkNode->GetUserPropInt(NI_422_PLATFORM_NODE_KEY, iVal ))
        return false;
    iVal = NiMAXOptions::ConvertDevImageToNiSystemDescRenderer(
        (NiDevImageConverter::Platform) iVal);
    NiMAXOptions::SetInt(NI_EXPORT_PLATFORM, iVal);

    if(!pkNode->GetUserPropString(NI_422_IMAGE_FILE_DIR_NODE_KEY, kStr))
        return false;
    NiMAXOptions::SetValue(NI_IMAGE_FILE_DIR, kStr);

    if(!pkNode->GetUserPropBool(NI_422_CONVERT_CAMERAS_NODE_KEY, iVal))
        return false;
    NiMAXOptions::SetBool(NI_CONVERT_CAMERAS, ToBool(iVal));

    if(!pkNode->GetUserPropBool(NI_422_CONVERT_LIGHTS_NODE_KEY, iVal))
        return false;
    NiMAXOptions::SetBool(NI_CONVERT_LIGHTS, ToBool(iVal));

    if(!pkNode->GetUserPropBool(NI_422_CONVERT_VERTEX_WEIGHTS_NODE_KEY, 
        iVal))
        return false;
    //NiMAXOptions::SetBool(NI_CONVERT_VERTEX_WEIGHTS, ToBool(iVal));

    if(!pkNode->GetUserPropBool(NI_422_USE_APPTIME_NODE_KEY, iVal))
        return false;
    NiMAXOptions::SetBool(NI_USE_APPTIME, ToBool(iVal));

    if(!pkNode->GetUserPropBool(NI_422_CONVERT_CSASKEYFRAMES_NODE_KEY, 
        iVal))
        return false;
    NiMAXOptions::SetBool(NI_CONVERT_CSASKEYFRAMES, ToBool(iVal));

    if(!pkNode->GetUserPropBool(NI_422_EXTRACT_KEYFRAMES_NODE_KEY, 
        iVal))
        return false;
    bool bExtractKeyframes = ToBool(iVal);
    NiMAXOptions::SetBool(NI_EXTRACT_KEYFRAMES, bExtractKeyframes);

    if(!pkNode->GetUserPropBool(NI_422_EXTRACT_SEPERATE_SEQUENCES_NODE_KEY, 
        iVal))
        return false;
    bool bExtractSeparateSequences = ToBool(iVal);
    NiMAXOptions::SetBool(NI_EXTRACT_SEPERATE_SEQUENCES,
        bExtractSeparateSequences);

    // Set the export script depending on the extraction options.
    const char* pcExportScriptName = NULL;
    if (bExtractKeyframes)
    {
        if (bExtractSeparateSequences)
        {
            pcExportScriptName = "Multiple KF Export with NIF";
        }
        else
        {
            pcExportScriptName = "Single KF Export with NIF";
        }
    }
    else
    {
        pcExportScriptName = "Default Export Script";
    }
    NiScriptTemplateManager* pkScriptManager = NiScriptTemplateManager::
        GetInstance();
    NIASSERT(pkScriptManager);
    NiScriptInfo* pkExportScript = pkScriptManager->GetScript(
        pcExportScriptName);
    NIASSERT(pkExportScript);
    NiMAXOptions::SetExportScript(pkExportScript);

    if(!pkNode->GetUserPropBool(NI_422_PER_TEXTURE_OVERRIDE_NODE_KEY, 
        iVal))
        return false;
    NiMAXOptions::SetBool(NI_PER_TEXTURE_OVERRIDE, ToBool(iVal));


    if(!pkNode->GetUserPropInt(NI_422_PIXEL_LAYOUT_NODE_KEY, iVal))
        return false;

    switch (iVal)
    {
        case 0:
            iVal = NiTexture::FormatPrefs::PALETTIZED_8;
            break;
        case 1:
            iVal = NiTexture::FormatPrefs::HIGH_COLOR_16;
            break;
        case 2:
            iVal = NiTexture::FormatPrefs::TRUE_COLOR_32;
            break;
        case 3:
            iVal = NiTexture::FormatPrefs::COMPRESSED;
            break;
        case 4:
            iVal = NiTexture::FormatPrefs::BUMPMAP;
            break;
        case 5:
        default:
            iVal = NiTexture::FormatPrefs::PIX_DEFAULT;
            break;
    }

    NiMAXOptions::SetInt(NI_PIXEL_LAYOUT, iVal);

    return true;
}
//---------------------------------------------------------------------------

