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

#include "XSIImportPlugin.h"
#include "XSIImportPluginHelpers.h"
#include "XSIImportPluginLogger.h"
#include "XSIImportPluginProgress.h"

#include "NiStandardSharedData.h"
#include "NiTransformController.h"
#include "NiNode.h"
#include "NiLog.h"
#include "NiMesh.h"
#include "NiMorphWeightsController.h"

#include "Scene.h"

#include "CrosswalkGamebryoConverter.h"
#include "CrosswalkGamebryoHelpers.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // RTTI implementation macro.
    //---------------------------------------------------------------------------
    NiImplementRTTI(XSIImportPlugin, NiImportPlugin);

    //---------------------------------------------------------------------------
    // XSI import plugin for the Gamebryo plugin toolkit.
    //---------------------------------------------------------------------------

    XSIImportPlugin::XSIImportPlugin() : NiImportPlugin(
        "XSI Import Plugin",                                    // name
        "1.0",                                                  // version
        "XSI import plugin",                                    // short description
        "XSI/Crosswalk 3.0 import plugin for Gamebryo " 
        NI_STR(GAMEBRYO_MAJOR_VERSION) "." NI_STR(GAMEBRYO_MINOR_VERSION))     // long description
    {
       // If any required internal variables need to be set, set them here
    }
    //---------------------------------------------------------------------------
    NiPluginInfo* XSIImportPlugin::GetDefaultPluginInfo()
    {
        // Create the default parameters for this plugin.
        NiPluginInfo* pkPluginInfo = NiNew NiImportPluginInfo;
        pkPluginInfo->SetName(m_strName);
        pkPluginInfo->SetVersion(m_strVersion);
        pkPluginInfo->SetClassName("XSIImportPlugin");
        pkPluginInfo->SetType("IMPORT");

        return pkPluginInfo;
    }
    //---------------------------------------------------------------------------
    bool XSIImportPlugin::HasManagementDialog()
    {
        // This plugin has no parameters that can be changed, so there is no
        // need to provide a management dialog. Return false to indicate this.
        // All parameters are read from the input XSI file.
        return false;
    }
    //---------------------------------------------------------------------------
    bool XSIImportPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent)
    {
        // This plugin has no management dialog, so return false.
        // All parameters are read from the input XSI file.
        return false;
    }
    //---------------------------------------------------------------------------
    bool XSIImportPlugin::CanImport(NiImportPluginInfo* pkInfo)
    {
        // Verify that the NiPluginInfo object can actually be executed by
        // this plugin. If we were to support previous versions, this is where
        // we would agree to handle the NiPluginInfo. Conversion would come in the 
        // actual Import call.

        // Other useful values to check would be:
        // - the name of the plugin,
        // - the application name if your plugin is application-dependent 
        //   (which you can get from NiFramework)
        // - if any required parameters exist in this NiPluginInfo

        if (pkInfo->GetClassName() == "XSIImportPlugin" &&
            VerifyVersionString(pkInfo->GetVersion()))
        {
            return true;
        }

        return false;
    }
    //---------------------------------------------------------------------------
    NiExecutionResultPtr XSIImportPlugin::Import(
        const NiImportPluginInfo* pkInfo)
    {
        // Coming into this method, CanImport has already been called.
        // Therefore, we know that we can actually execute pkInfo.

        // Grab an instance of the shared data list.
        NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
        NIASSERT(pkDataList);

        // Create logger.
        XSIImportPluginLogger logger(FindXMLLogger(pkDataList));

        // Get the file path specified by the user.
        NiString strFilePath = pkInfo->GetFullFilePath();
        if (strFilePath.IsEmpty())
        {
            logger.Log(LOG_ERROR, "No file name provided to convert.");
            return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
        }

        // We'll likely want to use the scene graph in this import plugin,
        // so we'll go ahead and get the scene graph shared data, creating it
        // if it does not already exist.
        NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
            GetSharedData(NiGetSharedDataType(NiSceneGraphSharedData));

        if (!pkSGSharedData)
        {
            // Create a new scene graph shared data here.
            pkSGSharedData = NiNew NiSceneGraphSharedData;
            pkDataList->Insert(pkSGSharedData);
        }

        // If you need to get or create further shared data, do it here...

        // Actual import code.
        {
            // For now, we use the default accumulating logger and will convert
            // the log entries after the conversion is done.
            CSLScene scene;
            const char * fileName = strFilePath;
            // Ugh... XSI CSLScene::Open() takes non-const char *... :-(
            SI_Error error = scene.Open(const_cast<char*>(fileName));
            if (SI_SUCCESS != error)
            {
                logger.Logf(LOG_ERROR, "Failed to open the scene \"%s\".", fileName);
                return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
            }
            error = scene.Read();
            if (SI_SUCCESS != error)
            {
                logger.Logf(LOG_ERROR, "Failed to read the scene \"%s\".", fileName);
                return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
            }

            if (!scene.Root())
            {
                logger.Logf(LOG_ERROR, "Failed because the scene in \"%s\" is empty.", fileName);
                return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
            }

            epg::ConverterSettings settings;
            epg::LoadSettings(*scene.Root(), settings);

            epg::Converter converter(settings);
            converter.SetLogger(logger);

            XSIImportProgress importProgress;
            converter.SetProgressMonitor(importProgress);

            if (! converter.Convert(scene))
            {
                logger.Logf(LOG_ERROR, "Failed to convert scene \"%s\".", fileName);
                return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
            }

            // Check the import type.
            if (pkInfo->GetImportType() == NiImportPluginInfo::ADD ||
                pkInfo->GetImportType() == NiImportPluginInfo::REPLACE)
            {
                epg::NodeMap & nodeMap = converter.GetNodeMap();
                epg::NodeMap::RootObjects & roots = nodeMap.GetRoots();
                typedef epg::NodeMap::RootObjects::iterator iter;
                for (iter pos = roots.begin(); pos != roots.end(); ++pos)
                {
                    NiAVObject * pkObject = *pos;
                    NiNode * pkNode = NiDynamicCast(NiNode, pkObject);
                    if (pkNode)
                    {
                        pkSGSharedData->AddRoot(pkNode, pkNode->GetName());
                    }
                    else
                    {
                        logger.Logf(LOG_WARNING, "Null node found in scene \"%s\".", fileName);
                    }
                }
            }
        }

        // Presumably, we have correctly executed our actions on the scene
        // graph. We return success in this case.
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
    }
    //---------------------------------------------------------------------------
    const char* XSIImportPlugin::GetFileDescriptorString()
    {
        // Return the name of this file type. This is used by file selection
        // dialogs.
        return "dotXSI Files (*.xsi)";
    }
    //---------------------------------------------------------------------------
    const char* XSIImportPlugin::GetFileTypeString()
    {
        // Return the extension used by this import plugin.
        return "*.xsi";
    }
    //---------------------------------------------------------------------------
}
