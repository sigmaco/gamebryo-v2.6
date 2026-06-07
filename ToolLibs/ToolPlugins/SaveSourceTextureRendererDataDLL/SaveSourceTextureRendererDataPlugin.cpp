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


#include "stdafx.h"
#include "SaveSourceTextureRendererDataPlugin.h"

#include <windows.h>

#include <NiFlipController.h>
#include <NiPixelData.h>
#include <NiPixelFormat.h>
#include <NiNode.h>
#include <NiSourceTexture.h>
#include <NiTextureEffect.h>
#include <NiTexturingProperty.h>
#include <NiVersion.h>

const char* const SaveSourceTextureRendererDataPlugin::
    ms_pcImageTilerDLLName[NiPersistentSrcTextureRendererData::
    NI_NUM_PLATFORM_IDS] =
{
 {""},
 {"NiXbox360ImageTiler"
    NI_DLL_SUFFIX
    ".dll"},
 {""},
 {""},
 {"NiWiiImageTiler"
    NI_DLL_SUFFIX
    ".dll"},
 {""}
};

// RTTI implementation macro.
NiImplementRTTI(SaveSourceTextureRendererDataPlugin, NiPlugin);

//---------------------------------------------------------------------------
SaveSourceTextureRendererDataPlugin::SaveSourceTextureRendererDataPlugin()
    : NiPlugin(
    "Save Source Texture Renderer Data Plug-in",  // Name.
    "1.0",  // Version.
    "A plug-in to save persistent source texture renderer data.",
        // Short description.
    "A plug-in to save persistent source renderer data for internal "
    "textures."), // Long description.
    m_bWriteResultsToLog(false), m_pkLogger(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
SaveSourceTextureRendererDataPlugin::~SaveSourceTextureRendererDataPlugin()
{
    FreeLibrary((HINSTANCE)m_hImageTilerDLL);
}
//---------------------------------------------------------------------------
NiPluginInfo* SaveSourceTextureRendererDataPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("SaveSourceTextureRendererDataPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool SaveSourceTextureRendererDataPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool SaveSourceTextureRendererDataPlugin::DoManagementDialog(
    NiPluginInfo*, NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
bool SaveSourceTextureRendererDataPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    // Verify that the NiPluginInfo object can actually be executed by
    // this plug-in. If we were to support previous versions, this is where
    // we would agree to handle the NiPluginInfo. Conversion would come in the 
    // actual Execute call.

    // Other useful values to check would be:
    // - the name of the plug-in,
    // - the application name if your plug-in is application-dependent 
    //   (which you can get from NiFramework)
    // - if any required parameters exist in this NiPluginInfo

    if (pkInfo->GetClassName() == "SaveSourceTextureRendererDataPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr SaveSourceTextureRendererDataPlugin::Execute(
    const NiPluginInfo*)
{
    // Coming into this method, CanExecute has already been called.
    // Therefore, we know that we can actually execute pkInfo.

    // We'll want to use the scene graph in this process plug-in,
    // so we'll go ahead and get the scene graph shared data.
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        GetSharedData(NiGetSharedDataType(NiSceneGraphSharedData));

    // Establish whether info, warnings, and errors go to a log file or to a
    // NiMessageBox.
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    if (pkExporterSharedData)
    {
        m_bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
        m_pkLogger = pkExporterSharedData->GetXMLLogger();
    }

    // If we do not find scene graph shared data, we cannot complete the
    // processing of the scene graph. Return failure.
    if (!pkSGSharedData)
    {
        // Pop up an NiMessageBox (or write to the log file), giving a more
        // useful warning.
        if ((m_bWriteResultsToLog) && (m_pkLogger))
        {
            m_pkLogger->LogElement("SaveSourceTextureRendererDataError",
                "The Save Source Texture Renderer Data Process Plug-in "
                "could not find the NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The Save Source Texture Renderer Data Process "
                "Plug-in could not find the NiSceneGraphSharedData!",
                "Scene Graph Shared Data Missing");
        }

        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    LoadImageTilerLibrary();

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (NiUInt32 ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui); 
        Traverse(spScene);
    }

    // Presumably, we have correctly executed our actions on the scene
    // graph. We return success in this case.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
void SaveSourceTextureRendererDataPlugin::Traverse(NiAVObject* pkAVObject)
{
    if (!pkAVObject)
        return;

    NiTexturingProperty* pkTexProp =
        (NiTexturingProperty*)pkAVObject->GetProperty(NiProperty::TEXTURING);
    if (pkTexProp)
    {
        // Look for regular maps.
        const NiTexturingProperty::NiMapArray& kMaps = 
            pkTexProp->GetMaps();
        for (NiUInt32 ui = 0; ui < kMaps.GetSize(); ui++)
        {
            NiTexturingProperty::Map* pkMap = kMaps.GetAt(ui);
            if (pkMap)
            {
                NiTexture* pkTexture = pkMap->GetTexture();
                if (pkTexture)
                    ProcessTexture(pkTexture);
            }
        }

        // Look for shader maps.
        for (NiUInt32 ui = 0; ui < pkTexProp->GetShaderArrayCount(); ui++)
        {
            NiTexturingProperty::ShaderMap* pkShaderMap =
                pkTexProp->GetShaderMap(ui);

            if (pkShaderMap)
            {
                NiTexture* pkTexture = pkShaderMap->GetTexture();
                if (pkTexture)
                    ProcessTexture(pkTexture);
            }
        }
    }

    // Check for NiFlipController objects.
    const NiPropertyList& kPropList = pkAVObject->GetPropertyList();
    NiTListIterator kPos = kPropList.GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProperty = kPropList.GetNext(kPos);
        if (pkProperty)
            ProcessFlipController(pkProperty->GetControllers());
    }

    ProcessFlipController(pkAVObject->GetControllers());

    NiNode* pkNode = NiDynamicCast(NiNode, pkAVObject);
    if (pkNode)
    {
        // Iterate over dynamic effects.
        const NiDynamicEffectList& kEffectList = pkNode->GetEffectList();
        NiTListIterator kPos = kEffectList.GetHeadPos();
        while (kPos)
        {
            NiDynamicEffect* pkEffect = kEffectList.GetNext(kPos);

            NiTextureEffect* pkTexEffect =
                NiDynamicCast(NiTextureEffect, pkEffect);
            if (pkTexEffect)
            {
                NiTexture* pkTexture = pkTexEffect->GetEffectTexture();
                if (pkTexture)
                    ProcessTexture(pkTexture);
            }
        }

        // Iterate over children.
        for (NiUInt32 ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                Traverse(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void SaveSourceTextureRendererDataPlugin::ProcessTexture(NiTexture* pkTexture)
{
    if (!NiIsKindOf(NiSourceTexture, pkTexture))
        return;

    NiSourceTexture* pkSrcTex = (NiSourceTexture*) pkTexture;

    if (pkSrcTex)
        CreatePersistentSrcRendererData(pkSrcTex);
}
//---------------------------------------------------------------------------
// Iterate through list of time controllers. If any NiFlipController is found,
// create PersistentSrcTextureRendererData for all textures referred to by the
// NiFlipController.
void SaveSourceTextureRendererDataPlugin::ProcessFlipController(
    NiTimeController* pkControl)
{
    for (/**/; pkControl != NULL; pkControl = pkControl->GetNext())
    {
        if (NiIsKindOf(NiFlipController, pkControl))
        {
            NiFlipController* pkFlip = (NiFlipController*) pkControl;

            unsigned int uiTextures = pkFlip->GetTextureArraySize();
            for (NiUInt32 ui = 0; ui < uiTextures; ui++)
            {
                NiTexture* pkTexture = pkFlip->GetTextureAt(ui);
                NIASSERT(pkTexture != NULL);
                ProcessTexture(pkTexture);
            }
        }
    }
}
//---------------------------------------------------------------------------
void SaveSourceTextureRendererDataPlugin::CreatePersistentSrcRendererData(
   NiSourceTexture* pkTexture)
{
    NiPersistentSrcTextureRendererData::PlatformID eTargetPlatform;
    switch (NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
    {
    case NiSystemDesc::RENDERER_DX9:
        eTargetPlatform = NiPersistentSrcTextureRendererData::NI_DX9;
        break;
    case NiSystemDesc::RENDERER_D3D10:
        eTargetPlatform = NiPersistentSrcTextureRendererData::NI_D3D10;
        break;
    case NiSystemDesc::RENDERER_XENON:
        eTargetPlatform = NiPersistentSrcTextureRendererData::NI_XENON;
        break;
    case NiSystemDesc::RENDERER_PS3:
        eTargetPlatform = NiPersistentSrcTextureRendererData::NI_PS3;
        break;
    case NiSystemDesc::RENDERER_WII:
        eTargetPlatform = NiPersistentSrcTextureRendererData::NI_WII;
        break;
    default:
        return;
        break;
    }

    // Create a persistent (streamable) copy of the data
    // (NiPersistentSourceTextureRendererData) using NiPixelData, and then
    // remove NiPixelData.  Also, conditionally, perform Xenon-specific texture
    // tiling, as appropriate.
    NiPersistentSrcTextureRendererData* pkPersistentSrcRendererData =
        NiPersistentSrcTextureRendererData::Create(pkTexture, eTargetPlatform);

    // Return if NULL, e.g., if persistent source texture renderer data already
    // exists.
    if (!pkPersistentSrcRendererData)
        return;

    // If tiling function is available use it
    if (m_pfnTileImageFunc[eTargetPlatform])
    {
        if (!m_pfnTileImageFunc[eTargetPlatform](pkPersistentSrcRendererData))
        {
            char acErrorMsg[300+NI_MAX_PATH];
            NiSprintf(acErrorMsg, sizeof(acErrorMsg), 
                "Error: Unable to tile texture (%s) \n"
                "in platform-specific format. This problem may occur if a \n"
                "texture was exported in a format that is not supported by \n"
                "the platform. Change the texture export settings for the \n"
                "texture and try again. The exported NIF is not valid.\n",
                pkTexture->GetFilename());

            if ((m_bWriteResultsToLog) && (m_pkLogger))
            {
                m_pkLogger->LogElement("SaveSourceTextureRendererDataError",
                    acErrorMsg);
            }
            else
            {
                NiMessageBox(acErrorMsg,  
                    "Error:  Platform-specific texture export failed");
            }
        }
    }

    // The streaming system will later stream out the newly created
    // NiPersistentSourceTextureRendererData objects.
}
//---------------------------------------------------------------------------
void SaveSourceTextureRendererDataPlugin::LoadImageTilerLibrary()
{
    for (int i=NiPersistentSrcTextureRendererData::NI_ANY;
        i<NiPersistentSrcTextureRendererData::NI_NUM_PLATFORM_IDS;
        ++i)
    {
        m_pfnTileImageFunc[i] = NULL;
        if (ms_pcImageTilerDLLName[i][0] != '\0')
        {
            m_hImageTilerDLL[i] = LoadLibrary(ms_pcImageTilerDLLName[i]);
            if (m_hImageTilerDLL[i])
            {
                m_pfnTileImageFunc[i] = (NiTileImageFunction)GetProcAddress(
                m_hImageTilerDLL[i], "TileImage");
            }
        }
    }
}
//---------------------------------------------------------------------------
