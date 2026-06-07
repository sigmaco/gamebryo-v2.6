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

//
// MyiDtExt.cpp
// ============
//
//      This module has functions that interface with the MDtAPI.  Most
// of these functions are end-all functions that set up an internal
// database that we will be using later.
//

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "GetPackingProfiles.h"
#include <NiPluginToolkitDefinitions.h>


MyiExportGlobals::
MyiExportGlobals(void)
{
    m_acCustomViewerScript[0] = '\0';
}

MyiExportGlobals gExport;

bool DtExt_HaveGui()
{
    return !gExport.m_bRunSilently;
}

//---------------------------------------------------------------------------
MyiExportGlobals::
~MyiExportGlobals(void)
{
}
//---------------------------------------------------------------------------
void MyiExportGlobals::Initialize()
{
    NiSystemDesc::GetSystemDesc().SetToolMode(true);
    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
        NiSystemDesc::RENDERER_GENERIC);

    m_fLinearUnitMultiplier = 1.0f;
    m_fFramesPerSecond      = 24.0f;
    m_iFilterType           = FILTER_BILERP;
    m_bSaveExport           = true;
    m_bViewExport           = false;

    m_bOptimizeMeshForViewer = false;

    m_fRed                  = 0.0f;
    m_fGreen                = 0.0f;
    m_fBlue                 = 0.0f;
    m_bLightingOptimizeSceneRootLights = true;
    m_bExportDefaultMayaCameras = true;
    m_bDefaultAmbientInTextureMaterial = true;
    m_bUseCurrentWorkingUnits = false;
    m_bFullScreen = false;
    m_bAutoCenterLODs = false;
    m_bFlipTexturesInsteadOfUVs = false;
    m_bAddDefaultAmbientForMayaLights = true;
    m_bGuaranteeObjectNamesAreUnique = false;
    m_bUseOldParticleSystemFields = false;


    m_iWindowWidth = 640;
    m_iWindowHeight = 480;

    m_iGenericTextureFormat = MyiExportGlobals::TEXTURE_DEFAULT;
    m_iGenericTextureReduction = TEXTURE_SIZE_REDUCTION_TEXTURE_DEFAULT;
    m_pcGenericTextureSaveDirectory = NULL;

    m_iXenonTextureFormat = MyiExportGlobals::TEXTURE_DEFAULT;
    m_iXenonTextureReduction = TEXTURE_SIZE_REDUCTION_TEXTURE_DEFAULT;
    m_pcXenonTextureSaveDirectory = NULL;

    m_iDX9TextureFormat = MyiExportGlobals::TEXTURE_DEFAULT;
    m_iDX9TextureReduction = TEXTURE_SIZE_REDUCTION_TEXTURE_DEFAULT;
    m_pcDX9TextureSaveDirectory = NULL;

    m_iD3D10TextureFormat = MyiExportGlobals::TEXTURE_DEFAULT;
    m_iD3D10TextureReduction = TEXTURE_SIZE_REDUCTION_TEXTURE_DEFAULT;
    m_pcD3D10TextureSaveDirectory = NULL;

    m_iPS3TextureFormat = MyiExportGlobals::TEXTURE_DEFAULT;
    m_iPS3TextureReduction = TEXTURE_SIZE_REDUCTION_TEXTURE_DEFAULT;
    m_pcPS3TextureSaveDirectory = NULL;

    m_iWiiTextureFormat = MyiExportGlobals::TEXTURE_DEFAULT;
    m_iWiiTextureReduction = TEXTURE_SIZE_REDUCTION_TEXTURE_DEFAULT;
    m_pcWiiTextureSaveDirectory = NULL;

    m_iTextureSaveLocation = TEXTURE_SAVE_INTERNAL;

    m_bAnimationExportAnimationRange = true;
    m_bAnimationLoopOverRange = false;

    m_iAnimationMaximumFrameRate = 250;
    m_fMinimumBoneInfluence = 0.1f;

    m_bRunSilently = false;
    m_bUseLastSavedScript = false;
    m_bForceDefaultScript = false;


    m_bExportKeyFrames = false;
    m_bKeyFrameMultipleSequencesPerKeyFrame = false;
    m_bKeyFrameFileName = false;
    m_bKeyFrameCharacterName = false;

    m_bAnimationAutoReduceKeyFrames = true;
    m_fAnimationMaxReductionError = 0.0f;

    m_bUseDbgSt = false;
    m_pcPS2ViewerDbgStIPAddress = NULL;
    m_iPS2FieldMode = 0;
    m_bPS2TwoCircuitAntialiasing = false;
    m_iPS2VideoFormat = 0;
    m_iPS2ZBufferDepth = 0;

    m_iStripifyType = 3;
    m_bStripifyForPreview=false;

    m_pcLoggerPath = NULL;

    NiUInt32 uiLen = 8; // size of "Default" plus terminator
    m_DefaultProfileName = NiAlloc(char, uiLen);
    NiStrcpy(m_DefaultProfileName, uiLen, "Default");

    char pcMayaLocation[MAX_PATH];

    // Get the location of maya and convert the directory seperators
    uiLen = 0;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    getenv_s(&uiLen, pcMayaLocation, MAX_PATH, "MAYA_LOCATION");
    NIASSERT(uiLen <= MAX_PATH);
#else
    NiStrcpy(pcMayaLocation, MAX_PATH, getenv("MAYA_LOCATION"));
#endif
    MyiTexture::ReplaceChar(pcMayaLocation, '/', '\\');

    uiLen = (NiUInt32)strlen(pcMayaLocation) + 
        strlen("\\bin\\Plug-ins\\Gamebryo Scripts") + 1;
    m_pcScriptTemplatePath = NiAlloc(char, uiLen);

    NiStrcpy(m_pcScriptTemplatePath, uiLen, pcMayaLocation);
    NiStrcat(m_pcScriptTemplatePath, uiLen, 
        "\\bin\\Plug-ins\\Gamebryo Scripts");
}
//---------------------------------------------------------------------------
void MyiExportGlobals::Shutdown()
{
    if (m_pcPS2ViewerDbgStIPAddress)
    {
        NiFree(m_pcPS2ViewerDbgStIPAddress);
        m_pcPS2ViewerDbgStIPAddress = NULL;
    }

    if (m_pcScriptTemplatePath)
    {
        NiFree(m_pcScriptTemplatePath);
        m_pcScriptTemplatePath = NULL;
    }

    if (m_pcScriptPath)
    {
        NiFree(m_pcScriptPath);
        m_pcScriptPath = NULL;
    }
    if ( m_pcGenericTextureSaveDirectory)
    {
        NiFree(m_pcGenericTextureSaveDirectory);
        m_pcGenericTextureSaveDirectory = NULL;
    }

    if (m_pcXenonTextureSaveDirectory)
    {
        NiFree(m_pcXenonTextureSaveDirectory);
        m_pcXenonTextureSaveDirectory = NULL;
    }

    if (m_pcDX9TextureSaveDirectory)
    {
        NiFree(m_pcDX9TextureSaveDirectory);
        m_pcDX9TextureSaveDirectory = NULL;
    }

    if (m_pcD3D10TextureSaveDirectory)
    {
        NiFree(m_pcD3D10TextureSaveDirectory);
        m_pcD3D10TextureSaveDirectory = NULL;
    }

    if (m_pcPS3TextureSaveDirectory)
    {
        NiFree(m_pcPS3TextureSaveDirectory);
        m_pcPS3TextureSaveDirectory = NULL;
    }

    if (m_pcWiiTextureSaveDirectory)
    {
        NiFree(m_pcWiiTextureSaveDirectory);
        m_pcWiiTextureSaveDirectory = NULL;
    }

    if(m_pcLoggerPath)
    {
        NiFree(m_pcLoggerPath);
        m_pcLoggerPath = NULL;
    }
    if(m_DefaultProfileName)
    {
        NiFree(m_DefaultProfileName);
        m_DefaultProfileName = NULL;
    }
}
//---------------------------------------------------------------------------
void InitDtScene( const MFileObject& fileObject )
{
    NIASSERT(fileObject.isSet());

    // Copy filename from Maya Export File Save dialog
    char acFileName[MAX_PATH];
    NiStrcpy(acFileName, MAX_PATH, fileObject.name().asChar());

    // Strip off any Gamebryo file extensions (or the Maya .mb extension)
    // to get to the base filename. This prevents "foo.nif.nif" files from
    // being created.
    char* pcExtension = strrchr(acFileName, '.'); 
    while(pcExtension)
    {
        if (NiStricmp(pcExtension, ".nif") == 0 ||
            NiStricmp(pcExtension, ".kf") == 0 ||
            NiStricmp(pcExtension, ".kfm") == 0)
        {
            *pcExtension = '\0';
        }
        else if (NiStricmp(pcExtension, ".mb") == 0)
        {
            *pcExtension = '\0';
            gExport.m_bSaveExport = true;
        }
        else
        {
            // Found a non-Gamebryo or Maya extension, break
            // since this could be part of the filename.
            break;
        }

        pcExtension = strrchr(acFileName, '.'); 
    }

    DtExt_SceneInit(acFileName);
}
//---------------------------------------------------------------------------
void BuildDtDataBase( 
    const MString & options,
    MPxFileTranslator::FileAccessMode mode )
{    
    int iTimeSlider  = 0;
    int iAnimEnabled = 0;
    int i;

    DtExt_setTesselate( kTESSTRI );

    // Check for a Directory from a previous run
    if (gExport.m_pcGenericTextureSaveDirectory != NULL)
    {
        NiFree(gExport.m_pcGenericTextureSaveDirectory);
        gExport.m_pcGenericTextureSaveDirectory = NULL;
    }

    if (gExport.m_pcXenonTextureSaveDirectory != NULL)
    {
        NiFree(gExport.m_pcXenonTextureSaveDirectory);
        gExport.m_pcXenonTextureSaveDirectory = NULL;
    }

    if (gExport.m_pcDX9TextureSaveDirectory != NULL)
    {
        NiFree(gExport.m_pcDX9TextureSaveDirectory);
        gExport.m_pcDX9TextureSaveDirectory = NULL;
    }

    if (gExport.m_pcD3D10TextureSaveDirectory != NULL)
    {
        NiFree(gExport.m_pcD3D10TextureSaveDirectory);
        gExport.m_pcD3D10TextureSaveDirectory = NULL;
    }

    if (gExport.m_pcPS3TextureSaveDirectory != NULL)
    {
        NiFree(gExport.m_pcPS3TextureSaveDirectory);
        gExport.m_pcPS3TextureSaveDirectory = NULL;
    }

    if (gExport.m_pcWiiTextureSaveDirectory != NULL)
    {
        NiFree(gExport.m_pcWiiTextureSaveDirectory);
        gExport.m_pcWiiTextureSaveDirectory = NULL;
    }

    if(gExport.m_pcLoggerPath != NULL)
    {
        NiFree(gExport.m_pcLoggerPath);
        gExport.m_pcLoggerPath = NULL;
    }

    // Lets see how we entered this plug-in, either with the export all
    // or export selection flag set.
    if ( mode == MPxFileTranslator::kExportActiveAccessMode )
    { 
        DtExt_setWalkMode ( ACTIVE_Nodes );
    }
    else
    {
        DtExt_setWalkMode ( ALL_Nodes );
    }


    // Lets now do all of the option processing
    int iOptionsLength = options.length();
    const char *pcBigString;
    pcBigString = options.asChar();
    if (iOptionsLength > 0)
    {
        //Start parsing.
        MStringArray optionList;
        MStringArray theOption;

        options.split (';', optionList);

        //break out all the options.
        int iOptionListLength = optionList.length();
        for ( i = 0; i < iOptionListLength; ++i)
        {
            theOption.clear ();
            optionList[i].split ('=', theOption);

            char acMss2[256];
            
            int iTheOptionLength = theOption.length();
            if (iTheOptionLength >=1)
                NiStrcpy(acMss2, 256, theOption[0].asChar());

            if (iTheOptionLength > 1)
            {
                char acMss[256];
                NiStrcpy(acMss, 256, theOption[0].asChar());

                if (theOption[0] == MString ("MayaNifTextureSaveLocation"))
                {
                    gExport.m_iTextureSaveLocation = 
                        (int)(theOption[1].asInt() - 1);

                }
                else if (theOption[0] == MString ("Red"))
                    gExport.m_fRed = (float)(theOption[1].asInt() / 256.0f);
                else if (theOption[0] == MString ("Green"))
                {
                    int val = theOption[1].asInt();
                    NI_UNUSED_ARG(val);
                    gExport.m_fGreen = (float)(theOption[1].asInt() / 256.0f);
                }
                else if (theOption[0] == MString ("Blue"))
                {
                    int val = theOption[1].asInt();
                    NI_UNUSED_ARG(val);
                    gExport.m_fBlue = (float)(theOption[1].asInt() / 256.0f );
                }
                
                else if (theOption[0] == 
                    MString ("MayaNifExportForGamebryoPlatformType"))
                {
                    switch(theOption[1].asInt())
                    {
                    case 1:
                        NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
                            NiSystemDesc::RENDERER_GENERIC);
                        break;
                    case 2:
                        NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
                            NiSystemDesc::RENDERER_XENON);
                        break;
                    case 3:
                        NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
                            NiSystemDesc::RENDERER_DX9);
                        break;
                    case 4:
                        NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
                            NiSystemDesc::RENDERER_PS3);
                        break;
                    case 5:
                        NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
                            NiSystemDesc::RENDERER_WII);
                        break;
                    case 6:
                        NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
                            NiSystemDesc::RENDERER_D3D10);
                        break;
                    default:
                        NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
                            NiSystemDesc::RENDERER_GENERIC);
                        break;
                    }
                }
                else if (theOption[0] == 
                    MString ("MayaNifExportOptimizeMeshForViewer"))
                {
                    gExport.m_bOptimizeMeshForViewer = 
                        theOption[1].asInt() ? true : false;
                }
                else if (theOption[0] == MString ("Xtexres"))
                {
                    // Set the X size of the texture swatches to use  
                      DtExt_setXTextureRes ( theOption[1].asInt () );
                } 
                else if (theOption[0] == MString ("Ytexres"))
                {
                    // Set the Y size of the texture swatches to use  
                    DtExt_setYTextureRes ( theOption[1].asInt () );
                } 
                else if (theOption[0] == MString ("MaxXtexres"))
                {
                    // Set the Max X size of the texture swatches to use  
                    DtExt_setMaxXTextureRes( theOption[1].asInt () );
                }
                else if (theOption[0] == MString ("MaxYtexres"))
                {
                    // Set the Max Y size of the texture swatches to use  
                    DtExt_setMaxYTextureRes( theOption[1].asInt () );
                } 
                    // Lighting Options
                else if (theOption[0] == MString("LightingOptimizeSceneRoot"))
                    gExport.m_bLightingOptimizeSceneRootLights = 
                    theOption[1].asInt() ? true : false;

                    // EXPORT DEFAULT CAMERA OPTIONS
                else if (theOption[0] == 
                        MString("MayaNifExportDefaultCameras"))
                    gExport.m_bExportDefaultMayaCameras = 
                        (theOption[1].asInt() > 0);

                    // EXPORT WORKING UNITS
                else if (theOption[0] == MString("MayaNifExportWorkingUnits"))
                    gExport.m_bUseCurrentWorkingUnits = 
                        (theOption[1].asInt() > 0);

                    // EXPORT FLIP TEXTURES INSTEAD OF UV
                else if (theOption[0] == 
                    MString("MayaNifExportFlipTexturesInsteadOfUVs"))
                    gExport.m_bFlipTexturesInsteadOfUVs = 
                        (theOption[1].asInt() > 0);

                    // EXPORT ADD DEFAULT AMBIENT FOR MAYA LIGHTS
                else if (theOption[0] == 
                    MString("MayaNifExportAddDefaultAmbientForMayaLights"))
                    gExport.m_bAddDefaultAmbientForMayaLights = 
                        (theOption[1].asInt() > 0);

                    // EXPORT GUARANTEE OBJECT NAMES ARE UNIQUE
                else if (theOption[0] == 
                    MString("MayaNifExportGuaranteeObjectNamesAreUnique"))
                    gExport.m_bGuaranteeObjectNamesAreUnique = 
                        (theOption[1].asInt() > 0);

                    // WINDOW SIZE
                else if (theOption[0] == MString("MayaNifWindowSize"))
                {
                    switch(theOption[1].asInt())
                    {
                    case 1:
                        gExport.m_iWindowWidth = 640;
                        gExport.m_iWindowHeight = 480;
                        break;
                    case 2:
                        gExport.m_iWindowWidth = 800;
                        gExport.m_iWindowHeight = 600;
                        break;
                    case 3:
                        gExport.m_iWindowWidth = 1024;
                        gExport.m_iWindowHeight = 768;
                        break;
                    case 4:
                        gExport.m_iWindowWidth = 1280;
                        gExport.m_iWindowHeight = 1024;
                        break;
                    default:
                        DtExt_Err("Error:: Invalid Window Size:%d Reset to " 
                            "default 640/480\n", theOption[1].asInt());
                        gExport.m_iWindowWidth = 640;
                        gExport.m_iWindowHeight = 480;
                        break;
                    }
                }

                    // EXPORT FULL SCREEN
                else if (theOption[0] == MString("MayaNifExportFullScreen"))
                    gExport.m_bFullScreen = (theOption[1].asInt() > 0);


                    // EXPORT AUTO CENTER LODS
                else if (theOption[0] == 
                        MString("MayaNifExportAutoCenterLODs"))
                    gExport.m_bAutoCenterLODs = (theOption[1].asInt() > 0);

                    // Viewer
                else if (theOption[0] == MString("MayaNifGamebryoViewerType"))
                {
                    gExport.m_iViewer = theOption[1].asInt() - 1;
                }
                else if (theOption[0] == MString(
                    "MayaNifGamebryoCustomViewerName"))
                {
                    NiSprintf(gExport.m_acCustomViewerScript, NI_MAX_PATH, 
                        "%s", theOption[1].asChar());
                }
                else if (theOption[0] == 
                    MString("MayaNifExportTextureAmbientDefault"))
                {
                    gExport.m_bDefaultAmbientInTextureMaterial = 
                        (theOption[1].asInt() > 0);
                }
                // Generic Texture Options
                else if (theOption[0] == MString("MayaNifGenericTextureType"))
                {
                    DtConvertTextureFormatNameToEnum(theOption[1], 
                        gExport.m_iGenericTextureFormat);
                }
                else if (theOption[0] == 
                    MString("MayaNifGenericTextureReduction"))
                {
                    gExport.m_iGenericTextureReduction = 
                        theOption[1].asInt() - 1;

                }
                else if (theOption[0] == 
                    MString("MayaNifGenericTextureDirectory"))
                {
                    size_t stLen = strlen(theOption[1].asChar()) + 1;
                    gExport.m_pcGenericTextureSaveDirectory  = 
                        NiAlloc(char, stLen);
                    NiStrcpy(gExport.m_pcGenericTextureSaveDirectory, stLen,
                        theOption[1].asChar());
                }
                // XBox 360 Texture Options
                else if (theOption[0] == MString("MayaNifXenonTextureType"))
                {
                    DtConvertTextureFormatNameToEnum(theOption[1], 
                        gExport.m_iXenonTextureFormat);
                }
                else if (theOption[0] == 
                    MString("MayaNifXenonTextureReduction"))
                {
                    gExport.m_iXenonTextureReduction = theOption[1].asInt()- 1;
                }
                else if (theOption[0] == MString(
                    "MayaNifXenonTextureDirectory"))
                {
                    size_t stLen = strlen(theOption[1].asChar()) + 1;
                    gExport.m_pcXenonTextureSaveDirectory = 
                        NiAlloc(char, stLen);

                    NiStrcpy(gExport.m_pcXenonTextureSaveDirectory, stLen, 
                        theOption[1].asChar());
                }
                // DX9 Texture Options
                else if (theOption[0] == 
                    MString("MayaNifDX9TextureType"))
                {
                    DtConvertTextureFormatNameToEnum(theOption[1], 
                        gExport.m_iDX9TextureFormat);
                }
                else if (theOption[0] == 
                    MString("MayaNifDX9TextureReduction"))
                {
                    gExport.m_iDX9TextureReduction = 
                        theOption[1].asInt() - 1;
                }
                else if (theOption[0] == 
                    MString("MayaNifDX9TextureDirectory"))
                {
                    size_t stLen = strlen(theOption[1].asChar()) + 1;
                    gExport.m_pcDX9TextureSaveDirectory = 
                        NiAlloc(char, stLen);
                    NiStrcpy(gExport.m_pcDX9TextureSaveDirectory, stLen, 
                        theOption[1].asChar());
                }

                // D3D10 Texture Options
                else if (theOption[0] == 
                    MString("MayaNifD3D10TextureType"))
                {
                    DtConvertTextureFormatNameToEnum(theOption[1], 
                        gExport.m_iD3D10TextureFormat);
                }
                else if (theOption[0] == 
                    MString("MayaNifD3D10TextureReduction"))
                {
                    gExport.m_iD3D10TextureReduction = 
                        theOption[1].asInt() - 1;
                }
                else if (theOption[0] == 
                    MString("MayaNifD3D10TextureDirectory"))
                {
                    size_t stLen = strlen(theOption[1].asChar()) + 1;
                    gExport.m_pcD3D10TextureSaveDirectory = 
                        NiAlloc(char, stLen);
                    NiStrcpy(gExport.m_pcD3D10TextureSaveDirectory, stLen, 
                        theOption[1].asChar());
                }

                // PS3 Texture Options
                else if (theOption[0] == 
                    MString("MayaNifPS3TextureType"))
                {
                    DtConvertTextureFormatNameToEnum(theOption[1], 
                        gExport.m_iPS3TextureFormat);
                }
                else if (theOption[0] == 
                    MString("MayaNifPS3TextureReduction"))
                {
                    gExport.m_iPS3TextureReduction = 
                        theOption[1].asInt() - 1;
                }
                else if (theOption[0] == 
                    MString("MayaNifPS3TextureDirectory"))
                {
                    size_t stLen = strlen(theOption[1].asChar()) + 1;
                    gExport.m_pcPS3TextureSaveDirectory = 
                        NiAlloc(char, stLen);
                    NiStrcpy(gExport.m_pcPS3TextureSaveDirectory, stLen, 
                        theOption[1].asChar());
                }
                // Wii Texture Options
                else if (theOption[0] == 
                    MString("MayaNifWiiTextureType"))
                {
                    DtConvertTextureFormatNameToEnum(theOption[1],
                        gExport.m_iWiiTextureFormat);
                }
                else if (theOption[0] == 
                    MString("MayaNifWiiTextureReduction"))
                {
                    gExport.m_iWiiTextureReduction = 
                        theOption[1].asInt() - 1;
                }
                else if (theOption[0] == 
                    MString("MayaNifWiiTextureDirectory"))
                {
                    size_t stLen = strlen(theOption[1].asChar()) + 1;
                    gExport.m_pcWiiTextureSaveDirectory = 
                        NiAlloc(char, stLen);
                    NiStrcpy(gExport.m_pcWiiTextureSaveDirectory, stLen, 
                        theOption[1].asChar());
                }

// **************
// Animation Options
// ************** 
                else if (theOption[0] == 
                    MString ("AnimationExportAnimationRange"))
                {
                    if ( theOption[1].asInt() == 1)
                        gExport.m_bAnimationExportAnimationRange = false;
                    else
                        gExport.m_bAnimationExportAnimationRange = true;
                }
                else if (theOption[0] == 
                    MString ("MayaNifAnimationLoopOverRange"))
                {
                    gExport.m_bAnimationLoopOverRange = 
                        theOption[1].asInt() ? true : false;
                }
                else if (theOption[0] == MString ("AnimationMaxFrameRate"))
                {
                    gExport.m_iAnimationMaximumFrameRate = 
                        theOption[1].asInt();
                }
                else if (theOption[0] == 
                    MString ("MayaNifAnimationMinimumBoneInfluence"))
                {
                    gExport.m_fMinimumBoneInfluence = 
                        (float)theOption[1].asInt() / 100.0f;
                }
                else if (theOption[0] == 
                    MString ("MayaNifAnimationSampleAllRotation"))
                {
                    gExport.m_bSampleAllRotation = theOption[1].asInt() == 0 ?
                        false : true;

                    //but more importantly
                    g_bSampleAllRotation = 
                        gExport.m_bSampleAllRotation;

                }
                else if (theOption[0] == 
                    MString ("MayaNifAnimationSampleRotationAsQuaternions"))
                {
                    gExport.m_bSampleRotationAsQuaternion = 
                        theOption[1].asInt() == 0 ? false : true;

                    //but more importantly
                    g_bSampleRotationAsQuaternion = 
                        gExport.m_bSampleRotationAsQuaternion;

                }
                else if (theOption[0] == 
                    MString ("MayaNifAnimationSampleAllTranslation"))
                {
                    gExport.m_bSampleAllTranslation = 
                        theOption[1].asInt() == 0 ? false : true;

                    //but more importantly
                    g_bSampleAllTranslation = 
                        gExport.m_bSampleAllTranslation;
                }

// **************
// OBSOLETE OPTIONS
// ************** 

                // PC Texture Type this became the Generic texture type
                else if (theOption[0] == MString("MayaNifPCTextureType"))
                {
                    gExport.m_iGenericTextureFormat = theOption[1].asInt() - 1;
                }
                else if (theOption[0] == MString("MayaNifPCTextureReduction"))
                {
                    gExport.m_iGenericTextureReduction = 
                        theOption[1].asInt() - 1;
                }
                else if (theOption[0] == MString("MayaNifPCTextureDirectory"))
                {
                    size_t stLen = strlen(theOption[1].asChar()) + 1;
                    gExport.m_pcGenericTextureSaveDirectory  = 
                        NiAlloc(char, stLen);
                    NiStrcpy(gExport.m_pcGenericTextureSaveDirectory, stLen,
                        theOption[1].asChar());
                }
                else if (theOption[0] == 
                    MString ("MayaNifExportForPlatformType"))
                {
                    // If this code is hit, its due to legacy platforms
                    // like PS2, Gamecube, etc. We convert it to generic.
                    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
                        NiSystemDesc::RENDERER_GENERIC);
                }
                else if (theOption[0] == MString("MayaNifSceneViewerType"))
                {
                    // Default to the asset viewer
                    gExport.m_iViewer = 3;
                }
                // **************
                // Key Frame Extractor Options
                // ************** 
                else if (theOption[0] == 
                    MString ("MayaNifExportExtractedKeyFrames"))
                {
                    gExport.m_bExportKeyFrames = 
                        theOption[1].asInt() ? true : false;
                }
                else if (theOption[0] == MString ("KeyFrameSequencesPerKF"))
                {
                    gExport.m_bKeyFrameMultipleSequencesPerKeyFrame = 
                        (theOption[1].asInt() == 2) ? true : false;
                }
                else if (theOption[0] == 
                    MString ("KeyFrameExtractorFrameFileNameOptionsFile"))
                {
                    gExport.m_bKeyFrameFileName = 
                        theOption[1].asInt() ? true : false;
                }
                else if (theOption[0] == 
                    MString ("KeyFrameExtractorFrameFileNameOptionsCharacter"))
                {
                    gExport.m_bKeyFrameCharacterName = 
                        theOption[1].asInt() ? true : false;
                }


                // **************
                // PS2 Viewer Options
                // ************** 
                else if (theOption[0] == MString ("PS2ViewerDbgStIPAddress"))
                {
                    size_t stLen = strlen(theOption[1].asChar()) + 1;
                    gExport.m_pcPS2ViewerDbgStIPAddress = NiAlloc(char, stLen);
                    NiStrcpy(gExport.m_pcPS2ViewerDbgStIPAddress, stLen, 
                        theOption[1].asChar());
                }
                else if (theOption[0] == MString ("PS2StationType"))
                {
                    gExport.m_bUseDbgSt = (theOption[1].asInt() - 1) > 0;
                    NIASSERT(gExport.m_bUseDbgSt == 0 || 
                        gExport.m_bUseDbgSt==1);
                }
                else if (theOption[0] == MString ("MayaNifPS2VideoFormat"))
                {
                    gExport.m_iPS2VideoFormat = theOption[1].asInt() - 1;
                }
                else if (theOption[0] == MString ("MayaNifPS2ZBufferDepth"))
                {
                    gExport.m_iPS2ZBufferDepth = theOption[1].asInt() - 1;
                }
                else if (theOption[0] == MString ("PS2ViewerFieldMode"))
                {
                    gExport.m_iPS2FieldMode = theOption[1].asInt();
                }
                else if (theOption[0] == 
                    MString ("PS2ViewerTwoCircuitAntialiasing"))
                {
                    gExport.m_bPS2TwoCircuitAntialiasing = 
                        theOption[1].asInt() ? true : false;;
                }
                
                // Animation Options
                else if (theOption[0] == 
                    MString ("AnimationAutoReduceKeyFrames"))
                {
                    gExport.m_bAnimationAutoReduceKeyFrames = 
                        theOption[1].asInt() ? true : false;
                }
                else if (theOption[0] == 
                    MString ("AnimationMaxReductionError"))
                {
                    gExport.m_fAnimationMaxReductionError = 
                        theOption[1].asFloat();
                }
                // Stripify Options
                else if (theOption[0] == MString("MayaNifStripifyType"))
                {
                    gExport.m_iStripifyType = theOption[1].asInt()-1;
                }
                else if (theOption[0] == MString ("MayaNifStripifyForPreview"))
                {
                    gExport.m_bStripifyForPreview = 
                        theOption[1].asInt() ? true : false;
                }

                else if (theOption[0] == MString ("debug"))
                { 
                    int levelG = DtExt_Debug();
                    if ( (int) (theOption[1].asInt () ) )
                        levelG |= DEBUG_GEOMAT;
                    else 
                        levelG &= ~DEBUG_GEOMAT;
                        
                    DtExt_setDebug( levelG );
                    
                }
                else if (theOption[0] == MString ("debugC"))
                { 
                    int levelC = DtExt_Debug();
                    if ( (int) (theOption[1].asInt () ) )
                        levelC |= DEBUG_CAMERA;
                    else 
                        levelC &= ~DEBUG_CAMERA;

                    DtExt_setDebug( levelC );
                }
                else if (theOption[0] == MString ("debugL"))
                { 
                    int levelL = DtExt_Debug();
                    if ( (int) (theOption[1].asInt () ) )
                        levelL |= DEBUG_LIGHT;
                    else 
                        levelL &= ~DEBUG_LIGHT;
                        
                    DtExt_setDebug( levelL );
                } 
                else if (theOption[0] == MString ("reversed"))
                {
                    DtExt_setWinding( theOption[1].asInt() );
                } 
                else if (theOption[0] == MString ("tesselation"))
                {
                    if ( theOption[1].asInt() == 2 )
                        DtExt_setTesselate( kTESSQUAD );
                    else 
                        DtExt_setTesselate( kTESSTRI );

                    // Now come the translator specific options
                }


                else if (theOption[0] == MString ("hrcType"))
                {
                    switch ( theOption[1].asInt () - 1)
                    {
                        case VRHRC_FLAT:
                            DtExt_setOutputTransforms (kTRANSFORMMINIMAL);
                            DtExt_setParents (0);
                            break;
                            
                        case VRHRC_WORLD:
                            DtExt_setOutputTransforms (kTRANSFORMNONE);
                            DtExt_setParents (0);
                            break;
                            
                        case VRHRC_FULL:
                        default:
                            DtExt_setOutputTransforms (kTRANSFORMALL);
                            DtExt_setParents (1);
                            break;
                    }       
                }
                else if (theOption[0] == MString ("exportSel"))
                {
                    switch ( theOption[1].asInt () - 1)
                    {
                        case VRSEL_ALL:
                            DtExt_setWalkMode (ALL_Nodes);
                            break;
                        case VRSEL_ACTIVE:
                            DtExt_setWalkMode (ACTIVE_Nodes);
                            break;
                        case VRSEL_PICKED:
                            DtExt_setWalkMode (PICKED_Nodes);
                            break;
                    }      
                }
                else if (theOption[0] == MString ("timeslider"))
                    iTimeSlider = (int) (theOption[1].asInt ());
                else if (theOption[0] == MString ("animEnabled"))
                    iAnimEnabled = (int) (theOption[1].asInt ());
                else if (theOption[0] == MString ("animStart"))
                    DtFrameSetStart( (int) (theOption[1].asInt ()) );
                else if (theOption[0] == MString ("animEnd"))
                    DtFrameSetEnd( (int) (theOption[1].asInt ()) );
                else if (theOption[0] == MString ("animStep"))
                    DtFrameSetBy( (int) (theOption[1].asInt ()) );
                else if (theOption[0] == MString("UseLastSavedScript"))
                {
                    // Check for Running Silently
                    gExport.m_bUseLastSavedScript = 
                        theOption[1].asInt() ? true : false;
                }
                else if (theOption[0] == MString("RunSilently"))
                {
                    //First check to see if the is batch mode or prompt mode
                    MGlobal::MMayaState appState =  MGlobal::mayaState();

                    // If in batch automatically run silently
                    if(appState == MGlobal::kBatch)
                    {
                        gExport.m_bRunSilently = true;
                    }
                    else
                    {
                        // Check for Running Silently
                        gExport.m_bRunSilently = 
                            theOption[1].asInt() ? true : false;
                    }
                }
                else if(theOption[0] == MString("MayaNifUseOldCameraFOV"))
                {
                    if(theOption[1].asInt() == 1)
                    {
                        gExport.m_bUseOldCameraFOV = true;
                    }
                    else
                    {
                        gExport.m_bUseOldCameraFOV = false;
                    }
                }
                else if(theOption[0] == MString("MeshPackingProfile"))
                {
                    if(gExport.m_DefaultProfileName != NULL)
                    {
                        NiFree(gExport.m_DefaultProfileName);
                        gExport.m_DefaultProfileName = NULL;
                    }
                    MString kProfileName = GetPackingProfiles::GetProfileName(
                        theOption[1]);
                    unsigned int uiLen = kProfileName.length() + 1;
                    gExport.m_DefaultProfileName = NiAlloc(char, uiLen);
                    NiStrcpy(gExport.m_DefaultProfileName, uiLen, 
                        kProfileName.asChar());
                }
                else
                {
                    NILOG("Could not find option %s\n", theOption[0].asChar());
                }

            }
        }
    }

    // Initialize our Linear and Time Units
    if (gExport.m_bUseCurrentWorkingUnits)
        gExport.m_fLinearUnitMultiplier = (float)gMDtGetLinearUnitMultiplier();
    else
        gExport.m_fLinearUnitMultiplier = 1.0f;

    gExport.m_fFramesPerSecond = (float)gAnimControlGetFramesPerSecond();


    // Lets check the TimeSlider control now:
    if ( iTimeSlider )
    {
        MTime start( MAnimControl::minTime().value(), MTime::uiUnit() );
        DtFrameSetStart( (int) start.value() );

        MTime end( MAnimControl::maxTime().value(), MTime::uiUnit() );
        DtFrameSetEnd( (int) end.value() );
    }

    // Now see if the animation is really enabled.
    // Else we will set the end frame to the beginning frame automatically
    if ( !iAnimEnabled )
    {
        DtFrameSetEnd( DtFrameGetStart() );
    }

    // overide the m_bSaveExport and m_bViewExport if we are doing a NIF Viewer
    char *pcSceneName;
    DtSceneGetName(&pcSceneName);
    if (strcmp(pcSceneName,"NifViewerMode")==0)
    {
        gExport.m_bSaveExport = false;
        gExport.m_bViewExport = true;
    }
    else
    {
        gExport.m_bSaveExport = true;
        gExport.m_bViewExport = false;
    }

    //Need to be able to set the export path from mel
    //so it is queried this way
    //MString kValue;
    //MGlobal::executeCommand("optionVar -q GamebryoSilentLoggingPath",kValue);
    //unsigned int uiLen = strlen(kValue.asChar()) + 1;
    //gExport.m_pcLoggerPath = NiAlloc(char, uiLen);
    //NiStrcpy(gExport.m_pcLoggerPath, uiLen, 
    //    kValue.asChar());

}
//---------------------------------------------------------------------------
bool DtConvertTextureFormatNameToEnum(const MString& kTextureFormatString, 
    int& iTextureFormat)
{

    bool returnValue = true;

    if(kTextureFormatString == 
        "Platform Default (Match Default Platform Format)")
    {
        iTextureFormat = MyiExportGlobals::PLATFORM_DEFAULT;
    }
    else if(kTextureFormatString == "Texture Default (Match Source Image)" ||
        kTextureFormatString == "Default (Match Source Image)")
    {
        iTextureFormat = MyiExportGlobals::TEXTURE_DEFAULT;
    }
    else if(kTextureFormatString == "24/32 bit")
    {
        iTextureFormat = MyiExportGlobals::TRUE_COLOR;
    }
    else if(kTextureFormatString == "8 bit palletized")
    {
        iTextureFormat = MyiExportGlobals::PALETTE_8;
    }
    else if(kTextureFormatString == "16 bit")
    {
        iTextureFormat = MyiExportGlobals::HIGH_COLOR;
    }
    else if(kTextureFormatString == "Compress DXT1")
    {
        iTextureFormat = MyiExportGlobals::COMPRESS1;
    }
    else if(kTextureFormatString == "Compress DXT3")
    {
        iTextureFormat = MyiExportGlobals::COMPRESS3;
    }
    else if(kTextureFormatString == "Compress DXT5")
    {
        iTextureFormat = MyiExportGlobals::COMPRESS5;
    }
    else
    {
        returnValue = false;
    }

    return returnValue;
}

//---------------------------------------------------------------------------
void SetDtTextureSearchPaths( void )
{
    // Now lets setup some paths to do basic texture file searching
    // for those textures with relative paths

    MStringArray wSpacePaths;
    MStringArray rPaths;
    MString      usePath;
    MString      separator;
    
    MGlobal::executeCommand( "workspace -q -rd", wSpacePaths );
    MGlobal::executeCommand( "workspace -q -rtl", rPaths );
    
    if ( DtExt_getTextureSearchPath() )
        separator.set( "|" );
    else
        separator.set( "" );

    unsigned int i;
    for (i = 0; i < wSpacePaths.length (); ++i)
    {   
        for ( unsigned int j = 0; j < rPaths.length(); j++ )
        {   
            usePath = usePath + separator + wSpacePaths[i] + MString( "/" ) + 
                rPaths[j];    
            separator.set( "|" );
            
            if ( rPaths[j] == MString( "sourceImages" ) )
                usePath = usePath + separator + wSpacePaths[i] + MString( "/" )
                + MString( "sourceimages" );
        }
    }
    
    DtExt_addTextureSearchPath( (char *)usePath.asChar() );
}
//---------------------------------------------------------------------------
char *MDt_GetPathName (char *LFullStr, char *LRet, int LMaxLen)
{
    register int    LC;
    register int    LLen,
                    LLastSlashPos;

    LLastSlashPos = -1;
    for (LC = 0; LFullStr[LC] != '\0'; LC++)
    {
        if (LFullStr[LC] == '/')
            LLastSlashPos = LC;
    }

    LLen = LLastSlashPos + 1;

    if (LRet == NULL)
        return (NULL);

    LC = 0;
    if ((LRet != NULL) && ((LLen + 1) <= LMaxLen))
    {
        for (; LC < LLen; LC++)
            LRet[LC] = LFullStr[LC];
    }
    LRet[LC] = '\0';

    return (LRet);
}
//---------------------------------------------------------------------------
bool DtImageLibFileSave(int iTextureID, char *pcPath)
{
    char    acCmd[2048];

    unsigned char *pcImage;
    DtTextureGetImageByID( iTextureID, &pcImage );

    char *pcName;
    DtTextureGetNameID( iTextureID, &pcName);

    int iW, iH, iD;
    DtTextureGetImageSizeByID( iTextureID, &iW, &iH, &iD);
    
    MStatus Status;
    IFFimageWriter Writer;

    
    char    acTmpFile[1024];
    NiStrcpy(acTmpFile, 1024, pcPath);
    NiStrcat(acTmpFile, 1024, "TMPGARBAGE");

    // The below line does not work.
    // If it had, this would be very eloquent, but as it is,
    // we have to create a temporary file and do a system call.
    Writer.outFilter("imgcvt -t tga");
    Status = Writer.open( acTmpFile );

    if ( Status != MS::kSuccess )
    {
        NIASSERT(0);
        return false;
    }

    Writer.setSize( iW, iH );
    Writer.setBytesPerChannel( 1 );
    Writer.setType( ILH_RGBA );
 
    float zBuffer[1024];
    Status = Writer.writeImage( (byte *)pcImage, zBuffer, 0 );
               
    if ( Status != MS::kSuccess )
    {
        NIASSERT(0);
        return false;
    }
                               
    Writer.close();
    
    // Below is needed because MDtAPI filter call does not work.
    int iFormat = 0;

    switch ( iFormat )
    {
        case 1:
            NiSprintf(acCmd, 2048, "imgcvt -t sgi %s %s",acTmpFile,pcPath );
            break;
        case 2:
            NiSprintf(acCmd, 2048, "imgcvt -t als %s %s",acTmpFile,pcPath );
            break;
        case 3:
            NiSprintf(acCmd, 2048, "imgcvt -t jpg %s %s",acTmpFile,pcPath );
            break;
        default:
            NiSprintf(acCmd, 2048, "imgcvt -t tga %s %s",acTmpFile,pcPath );
            break;
    }

    system( acCmd );

#if defined(_MSC_VER) && _MSC_VER >= 1400
    _unlink( acTmpFile );
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    unlink( acTmpFile );
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    return true;
}
//---------------------------------------------------------------------------
