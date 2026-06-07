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
#include "NiMAXOptions.h"
#undef STRICT // Avoid a warning b/c Maxscrpt.h defines STRICT too
#include "Maxscrpt.h"
#include "Name.h"
#include "Numbers.h"    
#include "Arrays.h"
#include "Strings.h"
#include "definsfn.h"   // generate static instances from def_x macros for 
                        // MaxScript
//---------------------------------------------------------------------------
Value* GetExportFlag_cf(Value**, int) 
{
    GetCOREInterface()->GetRootNode();
    Array* pkArray = NiExternalNew Array(25);
    Value* n_pkNameValue = NULL;
    NiString strValue;

    NiMAXOptions::ReadOptions(GetCOREInterface());

    n_pkNameValue = Name::intern(NI_TEXTURES);
    pkArray->append(n_pkNameValue);
    NiString strTextures = NiMAXOptions::GetValue(NI_TEXTURES);
    unsigned int uiLen = strTextures.Length() + 1;
    char* pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strTextures);
    pkArray->append(NiExternalNew String(pcString));
   
    n_pkNameValue = Name::intern(NI_EXPORT_PLATFORM);
    pkArray->append(n_pkNameValue);
    NiSystemDesc::RendererID ePlatform = (NiSystemDesc::RendererID)
        NiMAXOptions::GetInt(NI_EXPORT_PLATFORM);
    if (ePlatform == NiSystemDesc::RENDERER_GENERIC)
        pkArray->append(NiExternalNew String("GENERIC"));
    else if(ePlatform ==  NiSystemDesc::RENDERER_DX9)
        pkArray->append(NiExternalNew String("DX9"));
    else if(ePlatform ==  NiSystemDesc::RENDERER_PS3)
        pkArray->append(NiExternalNew String("PS3"));
    else if(ePlatform ==  NiSystemDesc::RENDERER_WII)
        pkArray->append(NiExternalNew String("WII"));
    else if(ePlatform ==  NiSystemDesc::RENDERER_XENON)
        pkArray->append(NiExternalNew String("XBOX360"));
    else if(ePlatform ==  NiSystemDesc::RENDERER_D3D10)
        pkArray->append(NiExternalNew String("D3D10"));
    else
        pkArray->append(NiExternalNew String(""));
    
    n_pkNameValue = Name::intern(NI_IMAGE_FILE_DIR);
    pkArray->append(n_pkNameValue);
    NiString strImageFileDir = NiMAXOptions::GetValue(NI_IMAGE_FILE_DIR);
    uiLen = strImageFileDir.Length() + 1;
    char* pcDir = NiAlloc(char, uiLen);
    NiSprintf(pcDir, uiLen, "%s", (const char*)strImageFileDir);
    pkArray->append(NiExternalNew String(pcDir));

    n_pkNameValue = Name::intern(NI_CONVERT_CAMERAS);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_CONVERT_CAMERAS) ? 
        &true_value : &false_value);

    n_pkNameValue = Name::intern(NI_CONVERT_LIGHTS);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_CONVERT_LIGHTS)?
        &true_value : &false_value);

    n_pkNameValue = Name::intern(NI_USE_APPTIME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_USE_APPTIME) ? 
        &true_value : &false_value);

    n_pkNameValue = Name::intern(NI_CONVERT_CSASKEYFRAMES);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_CONVERT_CSASKEYFRAMES)? 
        &true_value : &false_value);
    
    n_pkNameValue = Name::intern(NI_PER_TEXTURE_OVERRIDE);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_PER_TEXTURE_OVERRIDE)? 
        &true_value : &false_value);

    n_pkNameValue = Name::intern(NI_PIXEL_LAYOUT);
    pkArray->append(n_pkNameValue);
    NiTexture::FormatPrefs::PixelLayout eLayout = 
        (NiTexture::FormatPrefs::PixelLayout) 
        NiMAXOptions::GetInt(NI_PIXEL_LAYOUT);
    if(eLayout == NiTexture::FormatPrefs::HIGH_COLOR_16)
        pkArray->append(NiExternalNew String("HighColor16"));
    else if(eLayout ==  NiTexture::FormatPrefs::TRUE_COLOR_32)
        pkArray->append(NiExternalNew String("TrueColor32"));
    else if(eLayout ==  NiTexture::FormatPrefs::PIX_DEFAULT)
        pkArray->append(NiExternalNew String("PixDefault"));
    else if(eLayout ==  NiTexture::FormatPrefs::COMPRESSED)
        pkArray->append(NiExternalNew String("Compressed"));
    else if(eLayout ==  NiTexture::FormatPrefs::PALETTIZED_8)
        pkArray->append(NiExternalNew String("Palettized"));
    else
        pkArray->append(NiExternalNew String(""));

    n_pkNameValue = Name::intern(NI_EXTRACT_KEYFRAMES);
    pkArray->append(n_pkNameValue);
    if(!NiMAXOptions::GetBool(NI_EXTRACT_SEPERATE_SEQUENCES) &&
       !NiMAXOptions::GetBool(NI_EXTRACT_KEYFRAMES))
        pkArray->append(NiExternalNew String("KeepInNif"));
    else if(!NiMAXOptions::GetBool(NI_EXTRACT_SEPERATE_SEQUENCES) &&
             NiMAXOptions::GetBool(NI_EXTRACT_KEYFRAMES))
        pkArray->append(NiExternalNew String("SingleSequence"));
    else if(NiMAXOptions::GetBool(NI_EXTRACT_SEPERATE_SEQUENCES) &&
            NiMAXOptions::GetBool(NI_EXTRACT_KEYFRAMES))
        pkArray->append(NiExternalNew String("SeparateSequences"));
    else
    {
        throw RuntimeError ("Keyframe export setting must be: "
                "\"KeepInNif\", \"SingleSequence\", or"
                "\"SeparateSequences\"",
                n_pkNameValue);
    }
    
    n_pkNameValue = Name::intern(NI_USEOLDBEHAVIOR);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_USEOLDBEHAVIOR)? 
        &true_value : &false_value);
    
    n_pkNameValue = Name::intern(NI_USEOLDSPINTIME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_USEOLDSPINTIME)? 
        &true_value : &false_value);

    n_pkNameValue = Name::intern(NI_USEOLDNORMALS);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_USEOLDNORMALS)? 
        &true_value : &false_value);
       
    n_pkNameValue = Name::intern(NI_RESX );
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew Integer(NiMAXOptions::GetInt(NI_RESX)));
    
    n_pkNameValue = Name::intern(NI_RESY);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew Integer(NiMAXOptions::GetInt(NI_RESY)));
    
    n_pkNameValue = Name::intern(NI_SKINNING_THRESHOLD);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew Integer(NiMAXOptions::GetInt(
        NI_SKINNING_THRESHOLD)));
    
    n_pkNameValue = Name::intern(NI_WINDOWED);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_WINDOWED )?
        &true_value : &false_value);
    
    n_pkNameValue = Name::intern(NI_VIEWER_STRIPIFY);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_VIEWER_STRIPIFY)? 
        &true_value : &false_value);
    
    n_pkNameValue = Name::intern(NI_USE_TEMPLATE_FOR_SCRIPT);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiMAXOptions::GetBool(NI_USE_TEMPLATE_FOR_SCRIPT)? 
        &true_value : &false_value);
    
    n_pkNameValue = Name::intern(NI_NETWORK_SCRIPT_PATH);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_NETWORK_SCRIPT_PATH);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString)); 

    n_pkNameValue = Name::intern(NI_DX9_VIEWER_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_DX9_VIEWER_SCRIPT_NAME);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_D3D10_VIEWER_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_D3D10_VIEWER_SCRIPT_NAME);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));
    
    n_pkNameValue = Name::intern(NI_EXPORT_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_EXPORT_SCRIPT_NAME);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));
    
    n_pkNameValue = Name::intern(NI_PROCESS_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_PROCESS_SCRIPT_NAME);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_CUSTOM_VIEWER_SCRIPT_NAME);
    strValue = NiMAXOptions::GetValue(NI_CUSTOM_VIEWER_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_SCENEIMMERSE_SCRIPT_NAME);
    strValue = NiMAXOptions::GetValue(NI_SCENEIMMERSE_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_D3D10_SCENEIMMERSE_SCRIPT_NAME);
    strValue = NiMAXOptions::GetValue(NI_D3D10_SCENEIMMERSE_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_VIEWPORT_SCRIPT_NAME);
    strValue = NiMAXOptions::GetValue(NI_VIEWPORT_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_ANIMATION_TOOL_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_ANIMATION_TOOL_SCRIPT_NAME);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_XBOX_360_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_XBOX_360_SCRIPT_NAME);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_PS3_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_PS3_SCRIPT_NAME);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_WII_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_WII_SCRIPT_NAME);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_PHYSX_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_PHYSX_SCRIPT_NAME);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    n_pkNameValue = Name::intern(NI_FILENAME);
    pkArray->append(n_pkNameValue);
    strValue = NiMAXOptions::GetValue(NI_FILENAME);
    uiLen = strValue.Length() + 1;
    pcString = NiAlloc(char, uiLen);
    NiSprintf(pcString, uiLen, "%s", (const char*) strValue);
    pkArray->append(NiExternalNew String(pcString));

    return pkArray;
}
def_visible_primitive( GetExportFlag, "GetExport");
//---------------------------------------------------------------------------
Value* GetExportTags_cf(Value**, int) 
{
    GetCOREInterface()->GetRootNode();
    Array* pkArray = NiExternalNew Array(25);
    Value* n_pkNameValue = NULL;

    n_pkNameValue = Name::intern(NI_TEXTURES);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String(
        "\"OriginalImages\" || \"SeparateNifs\" || \"MainNif\""));
   
    n_pkNameValue = Name::intern(NI_EXPORT_PLATFORM);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("\"GENERIC\" || \"DX9\" || "
        "\"XBOX360\" || \"PS3\" || \"WII\" || \"D3D10\""));
    
    n_pkNameValue = Name::intern(NI_IMAGE_FILE_DIR);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("ANY"));

    n_pkNameValue = Name::intern(NI_CONVERT_CAMERAS);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));

    n_pkNameValue = Name::intern(NI_CONVERT_LIGHTS);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));

    n_pkNameValue = Name::intern(NI_USE_APPTIME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));

    n_pkNameValue = Name::intern(NI_CONVERT_CSASKEYFRAMES);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));
    
    n_pkNameValue = Name::intern(NI_PER_TEXTURE_OVERRIDE);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));

    n_pkNameValue = Name::intern(NI_PIXEL_LAYOUT);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("\"HighColor16\" || \"TrueColor32\" "
        "|| \"Compressed\" || \"Palettized\""));

    n_pkNameValue = Name::intern(NI_USEOLDBEHAVIOR);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));
    
    n_pkNameValue = Name::intern(NI_USEOLDSPINTIME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));
    
    n_pkNameValue = Name::intern(NI_USEOLDNORMALS);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));

    n_pkNameValue = Name::intern(NI_RESX );
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("INTEGER"));
    
    n_pkNameValue = Name::intern(NI_RESY);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("INTEGER"));
    
    n_pkNameValue = Name::intern(NI_SKINNING_THRESHOLD);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("INTEGER"));
    
    n_pkNameValue = Name::intern(NI_WINDOWED);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));
    
    n_pkNameValue = Name::intern(NI_VIEWER_STRIPIFY);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));
    
    n_pkNameValue = Name::intern(NI_USE_TEMPLATE_FOR_SCRIPT);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));
    
    n_pkNameValue = Name::intern(NI_NETWORK_SCRIPT_PATH);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("TRUE || FALSE"));
    
    n_pkNameValue = Name::intern(NI_DX9_VIEWER_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));

    n_pkNameValue = Name::intern(NI_D3D10_VIEWER_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));
    
    n_pkNameValue = Name::intern(NI_EXPORT_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));
    
    n_pkNameValue = Name::intern(NI_PROCESS_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));
    
    n_pkNameValue = Name::intern(NI_CUSTOM_VIEWER_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));
    
    n_pkNameValue = Name::intern(NI_SCENEIMMERSE_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));

    n_pkNameValue = Name::intern(NI_D3D10_SCENEIMMERSE_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));
    
    n_pkNameValue = Name::intern(NI_VIEWPORT_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));

    n_pkNameValue = Name::intern(NI_ANIMATION_TOOL_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));
    
    n_pkNameValue = Name::intern(NI_XBOX_360_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));

    n_pkNameValue = Name::intern(NI_PS3_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));

    n_pkNameValue = Name::intern(NI_WII_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));

    n_pkNameValue = Name::intern(NI_PHYSX_SCRIPT_NAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("SCRIPT"));

    n_pkNameValue = Name::intern(NI_FILENAME);
    pkArray->append(n_pkNameValue);
    pkArray->append(NiExternalNew String("ANY"));
    
    return pkArray;
}
def_visible_primitive( GetExportTags, "GetExportTags");
//---------------------------------------------------------------------------
Value* SetExportFlag_cf(Value** arg_list, int count) 
{
    GetCOREInterface()->GetRootNode();
    Value* n_pkBoolValue = Integer::intern(0);
    Value* n_pkNameValue = NULL;
    Value* n_pkIntValue = Integer::intern(0);

    NiMAXOptions::ReadOptions(GetCOREInterface());

    n_pkNameValue = Name::intern(NI_USEOLDBEHAVIOR);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
         bool bVal = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ? 
            true:false;
         NiMAXOptions::SetBool(NI_USEOLDBEHAVIOR, bVal); 
    }

    n_pkNameValue = Name::intern(NI_USEAREASUPERSPRAY);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
         bool bVal = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ? 
            true:false;
         NiMAXOptions::SetBool(NI_USEAREASUPERSPRAY, bVal); 
    }
    
    n_pkNameValue = Name::intern(NI_USEOLDSPINTIME);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        bool bVal = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ? 
            true:false;
        NiMAXOptions::SetBool(NI_USEOLDSPINTIME , bVal);
    }

    n_pkNameValue = Name::intern(NI_USEOLDNORMALS);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        bool bVal = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ? 
            true:false;
        NiMAXOptions::SetBool(NI_USEOLDNORMALS, bVal);
    }

    n_pkNameValue = Name::intern(NI_RESX);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        int iVal = int_key_arg(pkNameValue, n_pkIntValue, 640);
        NiMAXOptions::SetInt(NI_RESX , iVal);
    }

    n_pkNameValue = Name::intern(NI_RESY);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        int iVal = int_key_arg(pkNameValue, n_pkIntValue, 480);
        NiMAXOptions::SetInt(NI_RESY, iVal);
    }

    n_pkNameValue = Name::intern(NI_SKINNING_THRESHOLD);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        int iVal = int_key_arg(pkNameValue, n_pkIntValue, 5);
        if (iVal > 0 && iVal < 50)
            NiMAXOptions::SetInt(NI_SKINNING_THRESHOLD, iVal);   
    }

    n_pkNameValue = Name::intern(NI_WINDOWED);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        bool bVal = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ? 
            true:false;
        NiMAXOptions::SetBool(NI_WINDOWED, bVal); 
    }

    n_pkNameValue = Name::intern(NI_VIEWER_STRIPIFY);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        bool bVal = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ? 
            true:false;
        NiMAXOptions::SetBool(NI_VIEWER_STRIPIFY, bVal);
        
    }

    n_pkNameValue = Name::intern(NI_USE_TEMPLATE_FOR_SCRIPT);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        bool bVal = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ? 
            true:false;
        NiMAXOptions::SetBool(NI_USE_TEMPLATE_FOR_SCRIPT, bVal);
            
    }

    n_pkNameValue = Name::intern(NI_NETWORK_SCRIPT_PATH);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::SetValue(NI_NETWORK_SCRIPT_PATH, pcStr);
        
    }

    n_pkNameValue = Name::intern(NI_EXPORT_SCRIPT_NAME);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_EXPORT_SCRIPT_NAME, pcStr);
        
    }

    n_pkNameValue = Name::intern(NI_PROCESS_SCRIPT_NAME);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_PROCESS_SCRIPT_NAME, pcStr);
        
    }

    n_pkNameValue = Name::intern(NI_CUSTOM_VIEWER_SCRIPT_NAME);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_CUSTOM_VIEWER_SCRIPT_NAME, pcStr);
        
    }

    n_pkNameValue = Name::intern(NI_SCENEIMMERSE_SCRIPT_NAME);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_SCENEIMMERSE_SCRIPT_NAME, pcStr);
        
    }

    n_pkNameValue = Name::intern(NI_D3D10_SCENEIMMERSE_SCRIPT_NAME);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_D3D10_SCENEIMMERSE_SCRIPT_NAME, pcStr);
        
    }

    n_pkNameValue = Name::intern(NI_VIEWPORT_SCRIPT_NAME);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_VIEWPORT_SCRIPT_NAME, pcStr);
        
    }

    n_pkNameValue = Name::intern(NI_ANIMATION_TOOL_SCRIPT_NAME);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_ANIMATION_TOOL_SCRIPT_NAME, pcStr);
        
    }

    n_pkNameValue = Name::intern(NI_XBOX_360_SCRIPT_NAME);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_XBOX_360_SCRIPT_NAME, pcStr);
    }

    n_pkNameValue = Name::intern(NI_PS3_SCRIPT_NAME);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_PS3_SCRIPT_NAME, pcStr);
    }

    n_pkNameValue = Name::intern(NI_WII_SCRIPT_NAME);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_WII_SCRIPT_NAME, pcStr);
    }

    n_pkNameValue = Name::intern(NI_PHYSX_SCRIPT_NAME);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::HandleValue(NI_PHYSX_SCRIPT_NAME, pcStr);
    }

    n_pkNameValue = Name::intern(NI_FILENAME);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {   
        char* pcStr = key_arg(pkNameValue)->to_string();
        NiMAXOptions::SetValue(NI_FILENAME, pcStr);
    }

    n_pkNameValue = Name::intern(NI_TEXTURES);
    if(key_arg(pkNameValue ) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        char* pcTexturesExport = key_arg(pkNameValue)->to_string();
        if(NiStricmp(pcTexturesExport, "OriginalImages") == 0)
        {
            NiMAXOptions::SetValue(NI_TEXTURES, NI_ORIGINAL_IMAGES);
        }
        else if(NiStricmp(pcTexturesExport, "OriginalImagesVRAM") == 0)
        {
            NiMAXOptions::SetValue(NI_TEXTURES, NI_ORIGINAL_IMAGES_VRAM);
        }
        else if(NiStricmp(pcTexturesExport, "SeparateNifs") == 0)
        {
            NiMAXOptions::SetValue(NI_TEXTURES, NI_SEPERATE_NIFS);
        }
        else if(NiStricmp(pcTexturesExport, "MainNif") == 0)
        {
            NiMAXOptions::SetValue(NI_TEXTURES, NI_KEEP_IN_MAIN_NIF);
        }
        else
        {
            throw RuntimeError ("Texture export setting must be: "
                    "\"OriginalImages\", \"SeparateNifs\", or \"MainNif\"", 
                    n_pkIntValue);
        }
    }

    n_pkNameValue = Name::intern(NI_EXPORT_PLATFORM);
    if(key_arg(pkNameValue) != &undefined && 
       key_arg(pkNameValue) != &unsupplied)
    {
        char* pcTexturesExport = key_arg(pkNameValue)->to_string();
        if (NiStricmp(pcTexturesExport, "GENERIC") == 0 || 
            NiStricmp(pcTexturesExport, "ANY") == 0 ||
            NiStricmp(pcTexturesExport, "PC") == 0)
        {
            NiMAXOptions::SetInt(NI_EXPORT_PLATFORM, 
                NiSystemDesc::RENDERER_GENERIC);
        }
        else if (NiStricmp(pcTexturesExport, "XBOX360") == 0 ||
            NiStricmp(pcTexturesExport, "XENON") == 0)
        {
            NiMAXOptions::SetInt(NI_EXPORT_PLATFORM, 
                NiSystemDesc::RENDERER_XENON);
        }
        else if (NiStricmp(pcTexturesExport, "DX9") == 0)
        {
            NiMAXOptions::SetInt(NI_EXPORT_PLATFORM, 
                NiSystemDesc::RENDERER_DX9);
        }
        else if (NiStricmp(pcTexturesExport, "PS3") == 0)
        {
            NiMAXOptions::SetInt(NI_EXPORT_PLATFORM, 
                NiSystemDesc::RENDERER_PS3);
        }
        else if (NiStricmp(pcTexturesExport, "WII") == 0)
        {
            NiMAXOptions::SetInt(NI_EXPORT_PLATFORM, 
                NiSystemDesc::RENDERER_WII);
        }
        else if (NiStricmp(pcTexturesExport, "D3D10") == 0)
        {
            NiMAXOptions::SetInt(NI_EXPORT_PLATFORM, 
                NiSystemDesc::RENDERER_D3D10);
        }
        else
        {
            throw RuntimeError ("Platform export setting must be: "
                "ANY/GENERIC, DX9, XBOX360, PS3, D3D10, or WII", n_pkIntValue);
        }
    }

    n_pkNameValue = Name::intern(NI_IMAGE_FILE_DIR);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        NiMAXOptions::SetValue(NI_IMAGE_FILE_DIR, 
            key_arg(pkNameValue)->to_string());
    }

    n_pkNameValue = Name::intern(NI_SCENEMESHPROFILENAME);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        NiMAXOptions::SetValue(NI_SCENEMESHPROFILENAME, 
            key_arg(pkNameValue)->to_string());
    }

    n_pkNameValue = Name::intern(NI_CONVERT_CAMERAS);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        bool bCams = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ? 
            true:false;
        NiMAXOptions::SetBool(NI_CONVERT_CAMERAS, bCams);
    }

    n_pkNameValue = Name::intern(NI_CONVERT_LIGHTS);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        bool bLights = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ? 
            true:false;
        NiMAXOptions::SetBool(NI_CONVERT_LIGHTS, bLights);
    }

    n_pkNameValue = Name::intern(NI_USE_APPTIME);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        bool bAppTime = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ?
            true:false;
        NiMAXOptions::SetBool(NI_USE_APPTIME, bAppTime);
    }

    n_pkNameValue = Name::intern(NI_CONVERT_CSASKEYFRAMES);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        bool bCSasKF = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ? 
            true:false;
        NiMAXOptions::SetBool(NI_CONVERT_CSASKEYFRAMES, bCSasKF);
    }

    n_pkNameValue = Name::intern(NI_PER_TEXTURE_OVERRIDE);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        bool bTexOverride = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ?
            true:false;
        NiMAXOptions::SetBool(NI_PER_TEXTURE_OVERRIDE, bTexOverride);
    }

    n_pkNameValue = Name::intern(NI_PIXEL_LAYOUT);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        NiTexture::FormatPrefs::PixelLayout eLayout = 
            NiTexture::FormatPrefs::HIGH_COLOR_16;
        char* pcTexturesExport = key_arg(pkNameValue)->to_string();

        if(NiStricmp(pcTexturesExport, "HighColor16") == 0)
        {
            eLayout = NiTexture::FormatPrefs::HIGH_COLOR_16;
        }
        else if(NiStricmp(pcTexturesExport, "TrueColor32") == 0)
        {
            eLayout = NiTexture::FormatPrefs::PIX_DEFAULT;
        }
        else if(NiStricmp(pcTexturesExport, "PixDefault") == 0)
        {
            eLayout = NiTexture::FormatPrefs::PIX_DEFAULT;
        }
        else if(NiStricmp(pcTexturesExport, "Compressed") == 0)
        {
            eLayout = NiTexture::FormatPrefs::COMPRESSED;
        }
        else if(NiStricmp(pcTexturesExport, "Palettized") == 0)
        {
            eLayout = NiTexture::FormatPrefs::PALETTIZED_8;
        }
        else
        {
            throw RuntimeError ("Platform export setting must be: "
                    "HIGHCOLOR16 ,TRUECOLOR32, COMPRESSED, or PALETTIZED",
                    n_pkIntValue);
        }

        NiMAXOptions::SetInt(NI_PIXEL_LAYOUT, eLayout);
    }

    n_pkNameValue = Name::intern(NI_WRITE_ERRORS_TO_FILE);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        bool bFileErrors = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ?
            true:false;
        NiMAXOptions::SetBool(NI_WRITE_ERRORS_TO_FILE, bFileErrors);
    }

    n_pkNameValue = Name::intern(NI_LOG_ERRORS_PROMPT);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        bool bPromptErrors = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ?
            true:false;
        NiMAXOptions::SetBool(NI_LOG_ERRORS_PROMPT, bPromptErrors);
    }

    n_pkNameValue = Name::intern(NI_LAST_ERROR_PROMPT_RETURN);
    if(key_arg(pkNameValue) != &undefined && 
        key_arg(pkNameValue) != &unsupplied)
    {
        bool bShowErrors = bool_key_arg(pkNameValue, n_pkBoolValue, 1) ?
            true:false;
        NiMAXOptions::SetBool(NI_LAST_ERROR_PROMPT_RETURN, bShowErrors);
    }

    NiMAXOptions::WriteOptions(GetCOREInterface());
    return &ok;
}
def_visible_primitive( SetExportFlag, "SetExport");
//---------------------------------------------------------------------------
Value* GetExportValue_cf(Value** arg_list, int count) 
{
    one_typed_value_local(Value* result);
    // check we have 1 arg and that it's a string
    check_arg_count(openFile, 1, count);
    type_check(arg_list[0], String, "GetExportValue ValueName");
    char* pcName = arg_list[0]->to_string();
    Value* result;

    NiMAXOptions::ReadOptions(GetCOREInterface());
    NiString strValue = NiMAXOptions::GetValue(pcName);
    
    if (strValue.IsEmpty())
    {
        result = &undefined;
    }
    else
    {
        unsigned int uiLen = strValue.Length() + 1;
        char* pcNewString = NiAlloc(char, uiLen);
        NiSprintf(pcNewString, uiLen, "%s", (const char*) strValue);
        result = NiExternalNew String(pcNewString);
        NiFree(pcNewString);
    }
    // pop value locals
    return_value(result);
}

def_visible_primitive( GetExportValue, "GetExportValue");
//---------------------------------------------------------------------------
Value* DoRootNodeUserPropDialog_cf(Value**, int) 
{
    // check we have 1 arg and that it's a string
    Interface* pkIntf = GetCOREInterface();
    NiMAXOptions::g_kOptionsCS.Lock();   
    INode* pkNode = pkIntf->GetRootNode();
    TSTR tBuffer;
    if (pkNode)
    {
        pkNode->GetUserPropBuffer(tBuffer);
        NiMessageBox((const char*)tBuffer, "Root Node User Properties");
    }

    NiMAXOptions::g_kOptionsCS.Unlock();
    return &ok;
}
def_visible_primitive( DoRootNodeUserPropDialog, "DoRootNodeUserPropDialog");
//---------------------------------------------------------------------------
Value* DoINIFileUserPropDialog_cf(Value**, int) 
{
    // check we have 1 arg and that it's a string
    Interface* pkIntf = GetCOREInterface();
    NiMAXOptions::g_kOptionsCS.Lock();   
    TCHAR acFilename[MAX_PATH];
    GetCfgFilename(pkIntf, acFilename, MAX_PATH);
    char acString[10000];
    FILE* pFile = NULL;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (fopen_s(&pFile, acFilename, "r") != 0)
        pFile = NULL;
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    pFile = fopen(acFilename, "r");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (pFile)
    {
        while (!feof(pFile))
        {
          int count = (int)fread( acString, sizeof( char ), 10000, pFile );
          if (ferror(pFile))
          {
             return &ok;
          }
          acString[count+1] = '\0';
        }

        NiMessageBox((const char*)acString, "INI User Properties");
        fclose(pFile);
    }
    NiMAXOptions::g_kOptionsCS.Unlock();
    return &ok;
}
def_visible_primitive( DoINIFileUserPropDialog, "DoINIFileUserPropDialog");
//---------------------------------------------------------------------------
