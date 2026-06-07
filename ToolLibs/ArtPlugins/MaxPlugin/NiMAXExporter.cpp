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
#include "NiMAXExporter.h"
#include "NiMAXOptions.h"
#include <NiSkinningUtilities.h>
#include <NiSharedDataList.h>
#include <NiSceneGraphSharedData.h>
#include <NiRendererSharedData.h>
#include <NiExporterOptionsSharedData.h>
#include <NiTimerSharedData.h>
#include "NiMAXConverter.h"
#include "NiMAXAnimationConverter.h"
#include "NiMAXCameraConverter.h"
#include "NiMAXHierConverter.h"
#include "NiMAXLODConverter.h"
#include "NiMAXMeshConverter.h"
#include "NiMAXMorphConverter.h"
#include "NiMAXLightConverter.h"
#include "NiMAXMaterialConverter.h"
#include "NiMAXTextureConverter.h"
#include <NiScriptInfoDialogs.h>
#include "MiUtils.h"
#include "MiResource.h"
#include <NiDefaultScriptWriter.h>
#include <direct.h> // for _mkdir
#include <NiErrorPromptDialog.h>
#include "NiMAXPlatformMaterial.h"

//---------------------------------------------------------------------------
bool NiMAXExporter::ExportScene(
    Interface* pkIntf,
    ExportTarget eTarget,
    NiScriptInfo* pkTargetScriptInfo,
    bool bExportSelected)
{
    CHECK_MEMORY();
    bool bViewerRun = eTarget != TO_FILE;
    bool bSuccessful = false;
    NiFramework& kFramework = NiFramework::GetFramework();

    NiSharedDataList& kSharedData = kFramework.GetSharedDataList();

    kSharedData.Lock();
    kSharedData.DeleteAll();
    CHECK_MEMORY();

    // Use the settings saved in the INode root to setup the export settings
    // Otherwise, use the old config system.
    pkIntf->GetRootNode();
    SetCursor(LoadCursor(NULL, IDC_WAIT));

#ifndef NIDEBUG 
    try
#endif
    {
        float fSkinWeight = 
            (float)NiMAXOptions::GetInt(NI_SKINNING_THRESHOLD);
        fSkinWeight = fSkinWeight/100.0f;

        NiMAXConverter::Init();
        NiMAXConverter::SetViewerRun(bViewerRun);
        NiMAXConverter converter;
        CHECK_MEMORY();

        NiScriptInfoPtr spScript = NiMAXOptions::GetProcessScript();

        // Setup the necessary variables to run correctly
        if (bViewerRun)
        {
            SetupViewerRunStatics();
            NiScriptInfoDialogs::CompileImportExportInfo(
                kFramework.GetAppPath() + "temp.nif",  
                spScript, false, true);
            NiScriptInfoDialogs::CompileImportExportInfo(
                kFramework.GetAppPath() + "temp.nif",  
                pkTargetScriptInfo, false, true);
        }
        else
        {
            SetupExportStatics();
            NiScriptInfoDialogs::CompileImportExportInfo(
                NiMAXOptions::GetValue(NI_FILENAME),  
                spScript, false, true);
            NiScriptInfoDialogs::CompileImportExportInfo(
                NiMAXOptions::GetValue(NI_FILENAME),  
                pkTargetScriptInfo, false, true);
        }

        if (NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE) == true)
        {
            NiString kFileName = pkIntf->GetCurFileName();
            kFileName.Replace(" ", "");
            NiXMLLogger* pkLogger = NiMAXConverter::GetXMLLogger();
            if (pkLogger)
                pkLogger->StartElement(kFileName);
        }

        CHECK_MEMORY();
        int iStatus = converter.Convert(pkIntf, bExportSelected);
        CHECK_MEMORY();

        if (NiMAXConverter::AreAnyErrors())
        {
            if (NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE) == false)
            {
                int iReturn = NiErrorPromptDialog().DoModal();
                if (iReturn == IDYES)
                    NiMAXConverter::ShowLog();
            }
        }
        
        if (iStatus)
        {
            kSharedData.Unlock();
            NiMAXConverter::Shutdown();
            HandleError(iStatus);
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return false;
        }

        NiNode* pkRoot = converter.GetRoot();
        NiMAXOptions::RemoveOptionsExtraData(pkRoot);
        CHECK_MEMORY();

        // Guarantee a fully updated scene graph
        pkRoot->Update(0.0f);

        if (!PreProcess(pkRoot, converter, pkIntf, spScript, bViewerRun))
        {
            kSharedData.Unlock();
            NiMAXConverter::Shutdown();
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return false;
        }
        CHECK_MEMORY();

        // Run the script
        NiBatchExecutionResultPtr spResult = 
            kFramework.ExecuteScript(spScript);
        NiOutputDebugString("Executed process script...\n");
        bSuccessful = HandleScriptResults(spResult, spScript);

        if (bSuccessful)
        {
            bool bSelectiveUpdate, bRigid;
            pkRoot->SetSelectiveUpdateFlags(bSelectiveUpdate, false, bRigid);

            spResult = kFramework.ExecuteScript(pkTargetScriptInfo);
            NiOutputDebugString("Executed target script...\n");
            bSuccessful = HandleScriptResults(spResult, pkTargetScriptInfo);
            CHECK_MEMORY();

        }

        if (bSuccessful)
        {
            bSuccessful = PostProcess(pkRoot, converter, pkIntf,
                spScript, bViewerRun);
            NiOutputDebugString("Post Processing Done\n");
        }

        if (NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE) == true)
        {
            NiString kFileName = pkIntf->GetCurFileName();
            kFileName.Replace(" ", "");
            NiXMLLogger* pkLogger = NiMAXConverter::GetXMLLogger();
            if (pkLogger)
                pkLogger->EndElement(kFileName);
        }

        CHECK_MEMORY();

        // Shut everything down
        NiMAXConverter::Shutdown();
        CHECK_MEMORY();

    }
#ifndef NIDEBUG 
    catch (...)
    {
        NiMAXConverter::Shutdown();
        kSharedData.Unlock();
        NiOutputDebugString("An exception has occured during export and your"
            " Max file did not properly export.\n\n");
        NiLogger::FlushAllLogs();

        if (NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE) == true)
        {
            NiXMLLogger* pkLogger = NiMAXConverter::GetXMLLogger();
            if (pkLogger)
                pkLogger->LogElement("ERROR", "An exception has occured "
                "during export and your Max file did not properly export. ");
        }
        else
        {
            NiMessageBox("An exception has occured during export and your "
                "Max file did not properly export. Press OK to continue", 
                "Export Exception");
        }
        return false;
    }
#endif

    //NIASSERT(uiTotObjCnt == uiInitialCount);

    SetCursor(LoadCursor(NULL, IDC_ARROW));
    kSharedData.DeleteAll();
    kSharedData.Unlock();

    return bSuccessful;

}

//---------------------------------------------------------------------------
void NiMAXExporter::SetupViewerRunStatics()
{
    // export options
    NiMAXCameraConverter::SetConvertCameras(true);
    NiMAXLightConverter::SetConvertLights(true);        
    NiMAXMeshConverter::SetExportVertWeights(true);

    // animation options
    NiMAXConverter::SetUseAppTime(true);
    NiMAXAnimationConverter::SetConvertCSAsKeyframes(
        NiMAXOptions::GetBool(NI_CONVERT_CSASKEYFRAMES));

    // textures
    NiMAXTextureConverter::SetSaveTexturesSeparately(false);
    NiMAXTextureConverter::SetUseOrigExt(false);
    NiMAXTextureConverter::SetPerTextureOverride(
        NiMAXOptions::GetBool(NI_PER_TEXTURE_OVERRIDE));

    // image format
    NiDevImageConverter::Platform eImagePlatform = 
        NiMAXOptions::ConvertNiSystemDescRendererToDevImage(
        NiSystemDesc::GetSystemDesc().GetToolModeRendererID());

    NiMAXTextureConverter::SetPlatform(eImagePlatform);

    NiImageConverter::SetPlatformSpecificSubdirectory(NULL);


    if (!NiMAXTextureConverter::GetUseOrigExt())
    {
        NiTexture::FormatPrefs::PixelLayout eLayout = 
            (NiTexture::FormatPrefs::PixelLayout)
            NiMAXOptions::GetInt(NI_PIXEL_LAYOUT);
        if (eLayout == NiTexture::FormatPrefs::TRUE_COLOR_32)
            eLayout = NiTexture::FormatPrefs::PIX_DEFAULT;
        NiMAXTextureConverter::SetPixelLayout(eLayout);
    }
    else
    {
        NiMAXTextureConverter::SetPixelLayout(
            NiTexture::FormatPrefs::PIX_DEFAULT);
    }
}
//---------------------------------------------------------------------------
void NiMAXExporter::SetupExportStatics()
{
    // export options
    NiMAXCameraConverter::SetConvertCameras(
        NiMAXOptions::GetBool(NI_CONVERT_CAMERAS));
    NiMAXLightConverter::SetConvertLights(
        NiMAXOptions::GetBool(NI_CONVERT_LIGHTS));        
//    NiMAXMeshConverter::SetExportVertWeights(
//      NiMAXOptions::GetBool(NI_CONVERT_VERTEX_WEIGHTS));

    // animation options
    NiMAXConverter::SetUseAppTime(NiMAXOptions::GetBool(NI_USE_APPTIME));
    NiMAXAnimationConverter::SetConvertCSAsKeyframes(
        NiMAXOptions::GetBool(NI_CONVERT_CSASKEYFRAMES));

    // textures

    NiString strValue = NiMAXOptions::GetValue(NI_TEXTURES);
    bool bSeperateTextures = false;
    bool bOriginalExtension = false;
    bool bOriginalExtensionVRAM = false;
    if (strValue.EqualsNoCase(NI_SEPERATE_NIFS))
    {
        bSeperateTextures = true;
    }
    else if(strValue.EqualsNoCase(NI_ORIGINAL_IMAGES))
    {
        bSeperateTextures = true;
        bOriginalExtension = true;
    }
    else if(strValue.EqualsNoCase(NI_ORIGINAL_IMAGES_VRAM))
    {
        bSeperateTextures = true;
        bOriginalExtensionVRAM = true;
    }

    NiMAXTextureConverter::SetSaveTexturesSeparately(bSeperateTextures);
    NiMAXTextureConverter::SetUseOrigExt(bOriginalExtension);
    NiMAXTextureConverter::SetUseOrigExtVRAM(bOriginalExtensionVRAM);
    NiMAXTextureConverter::SetPerTextureOverride(
        NiMAXOptions::GetBool(NI_PER_TEXTURE_OVERRIDE));

    // image format
    NiDevImageConverter::Platform eImagePlatform = 
        NiMAXOptions::ConvertNiSystemDescRendererToDevImage(
        NiSystemDesc::GetSystemDesc().GetToolModeRendererID());

    NiMAXTextureConverter::SetPlatform(eImagePlatform);


    if (!NiMAXTextureConverter::GetUseOrigExt())
    {
        NiTexture::FormatPrefs::PixelLayout eLayout = 
            (NiTexture::FormatPrefs::PixelLayout)
            NiMAXOptions::GetInt(NI_PIXEL_LAYOUT);
        if (eLayout == NiTexture::FormatPrefs::TRUE_COLOR_32)
            eLayout = NiTexture::FormatPrefs::PIX_DEFAULT;
        NiMAXTextureConverter::SetPixelLayout(eLayout);
    }
    else
    {
        NiMAXTextureConverter::SetPixelLayout(
            NiTexture::FormatPrefs::PIX_DEFAULT);
    }

    NiImageConverter::SetPlatformSpecificSubdirectory(
        NiMAXOptions::GetValue(NI_IMAGE_FILE_DIR));
    
    if (NiMAXTextureConverter::GetSaveTexturesSeparately() &&
        !(NiMAXTextureConverter::GetUseOrigExt() ||
        NiMAXTextureConverter::GetUseOrigExtVRAM()))
    {
        _mkdir(NiImageConverter::GetPlatformSpecificSubdirectory());
    }
}
//---------------------------------------------------------------------------
bool NiMAXExporter::HandleScriptResults(
    NiBatchExecutionResult* pkResults,
    NiScriptInfo* pkScript)
{
    bool bSuccessful = true;
    NiString strResults = "The converter encountered the following errors:"
        " \n";

    if (!pkResults)
    {
        bSuccessful = false;
        strResults += "Invalid Batch Execution Result.\n";
    }
    else if (pkResults && !pkResults->WasSuccessful())
    {
        bSuccessful = false;
        unsigned int ui = 0;

        for (ui = 0; ui < pkResults->m_pkPluginResults.GetSize(); 
            ui++)
        {
            NiExecutionResult* pkExecResult = 
                pkResults->m_pkPluginResults.GetAt(ui);
            if (pkExecResult && pkExecResult->m_eReturnCode != 
                NiExecutionResult::EXECUTE_SUCCESS)
            {
                NiPluginInfo* pkInfo = pkScript->GetPluginInfoAt(ui);
                if (pkInfo)
                {
                    strResults += pkInfo->GetName() + " was not successful."
                        "\n";
                }
                else
                {
                    strResults += "Plugin #";
                    strResults += (char)ui;
                    strResults += " is not a valid Plugin\n";
                }
            }
        }
    }

    if (!bSuccessful)
    {
        if (NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE) == true)
        {
            NiXMLLogger* pkLogger = NiMAXConverter::GetXMLLogger();
            pkLogger->LogElement("ExportFailure", strResults);
        }
        else
        {
            NiMessageBox((const char*) strResults, "Export Failure");
        }
    }
    
    return bSuccessful;
}

//---------------------------------------------------------------------------
bool NiMAXExporter::AddCameras(
    NiNode* pkRoot,
    NiMAXConverter&,
    bool bViewerRun)
{
    // Copy over the current viewer camera
    // Add MAX camera if there aren't any other cameras in the scene
    if (bViewerRun && NiMAXCameraConverter::GetCamList().IsEmpty())
    {
        NiMAXCameraConverter::CameraInfo *pTemp;
        NiPoint3 offset = NiPoint3::ZERO;
    
        float fAnimStart = GetCOREInterface()->GetAnimRange().Start() 
            * SECONDSPERTICK;
        pTemp = NiNew NiMAXCameraConverter::CameraInfo;
        NiMAXCameraConverter::ConvertDefaultCamera(GetCOREInterface(), 
            pkRoot, pTemp, fAnimStart, offset);
        // Attach the newly created camera to the scene
        // We have to pack the rotations and translations into the camera
        // from the parent created by CameraConverter to make it possible to
        // tumble the camera correctly
        pkRoot->AttachChild(pTemp->spCam);
        pTemp->spCam->SetTranslate(pTemp->spCamNode->GetTranslate());
        pTemp->spCam->SetRotate(pTemp->spCamNode->GetRotate()*
            pTemp->spCam->GetRotate());

        pTemp->spCam->SetName("<Default MAX Viewport Camera>");
        NiDelete pTemp;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiMAXExporter::AddSceneGraphSharedData(
    NiNode* pkRoot,
    NiMAXConverter&,
    bool)
{
    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedData = kFramework.GetSharedDataList();
    pkRoot->SetSelectiveUpdate(true);
    pkRoot->SetSelectiveUpdateTransforms(true);
    pkRoot->SetSelectiveUpdatePropertyControllers(true);
    pkRoot->SetSelectiveUpdateRigid(false);

    NiSceneGraphSharedDataPtr spSGSharedData = NiNew NiSceneGraphSharedData();
    spSGSharedData->AddRoot(pkRoot, NiMAXOptions::GetValue(NI_FILENAME), 
        true);
#ifdef NIDEBUG
    NiSharedDataList::ErrorCode eCode = 
#endif
        kSharedData.Insert(spSGSharedData);
    NIASSERT(eCode != SUCCESS);
    return true;

}
//---------------------------------------------------------------------------
bool NiMAXExporter::AddAnimationSharedData(
    NiNode*,
    NiMAXConverter&,
    bool)
{
    int iMAXFrameRate = GetFrameRate();
    int iMAXTicksPerFrame = GetTicksPerFrame();
    float fTimeConversionFactor = (float)(iMAXFrameRate*iMAXTicksPerFrame);
    float fStartTime = (float) GetCOREInterface()->GetAnimRange().Start();
    float fEndTime = (float)GetCOREInterface()->GetAnimRange().End();
    float fAnimationLoopStartTime = fStartTime / fTimeConversionFactor;
    float fAnimationLoopEndTime = fEndTime / fTimeConversionFactor;

    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedData = kFramework.GetSharedDataList();
    NiTimerSharedDataPtr spTimer = NiNew NiTimerSharedData();

    spTimer->SetStartTime(fAnimationLoopStartTime);
    spTimer->SetEndTime(fAnimationLoopEndTime);
    spTimer->SetTimeMode(NiTimerSharedData::LOOP);

#ifdef NIDEBUG
    NiSharedDataList::ErrorCode eCode = 
#endif
        kSharedData.Insert(spTimer);
    NIASSERT(eCode != SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
bool NiMAXExporter::AddRendererSharedData(
    NiNode*,
    NiMAXConverter& kConverter,
    bool)
{
    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedData = kFramework.GetSharedDataList();
    NiRendererSharedDataPtr spRendererSharedData = 
        NiNew NiRendererSharedData();

    NiColor kBackgroundColor = kConverter.GetBackGround();
    spRendererSharedData->SetBackgroundColor(kBackgroundColor);
    spRendererSharedData->SetFullscreen(!NiMAXOptions::GetBool(NI_WINDOWED));
    NiPoint2 kSize;
    kSize.x = (float) NiMAXOptions::GetInt(NI_RESX);
    kSize.y = (float) NiMAXOptions::GetInt(NI_RESY);
    spRendererSharedData->SetWindowSize(kSize);
#ifdef NIDEBUG
    NiSharedDataList::ErrorCode eCode = 
#endif
        kSharedData.Insert(spRendererSharedData);
    NIASSERT(eCode != SUCCESS);
    return true;
}

//---------------------------------------------------------------------------
bool NiMAXExporter::PostProcess(
    NiNode*,
    NiMAXConverter&, 
    Interface*,
    NiScriptInfo*,
    bool)
{
    NiFramework& kFramework = NiFramework::GetFramework();
    kFramework.GetSharedDataList();

    return true;
}

//---------------------------------------------------------------------------
bool NiMAXExporter::PreProcess(
    NiNode* pkRoot,
    NiMAXConverter& kConverter, 
    Interface*,
    NiScriptInfo* pkScript,
    bool bViewerRun)
{
    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedData = kFramework.GetSharedDataList();

    if (!AddCameras(pkRoot, kConverter, bViewerRun) || 
        !AddSceneGraphSharedData(pkRoot, kConverter, bViewerRun) ||
        !AddAnimationSharedData(pkRoot, kConverter, bViewerRun)  ||
        !AddRendererSharedData(pkRoot, kConverter, bViewerRun)
        )
    {
        return false;
    }

    NiExporterOptionsSharedDataPtr spSharedData = 
        NiNew NiExporterOptionsSharedData();
    spSharedData->SetCurrentScript(pkScript);
    spSharedData->SetViewerRun(bViewerRun);
    spSharedData->SetOptimizeMeshForViewer(
        NiMAXOptions::GetBool(NI_VIEWER_STRIPIFY));
    spSharedData->SetWriteResultsToLog(
        NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
    spSharedData->SetXMLLogger(NiMAXConverter::GetXMLLogger());
#ifdef NIDEBUG
    NiSharedDataList::ErrorCode eCode = 
#endif
        kSharedData.Insert(spSharedData);
    NIASSERT(eCode != SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
