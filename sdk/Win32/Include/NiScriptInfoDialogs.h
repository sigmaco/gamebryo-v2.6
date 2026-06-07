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

#ifndef NISCRIPTINFODIALOGS_H
#define NISCRIPTINFODIALOGS_H

#include "NiPluginToolkitLibType.h"
#include "NiPluginToolkitDefinitions.h"

#include "NiScriptInfo.h"
class NiScriptInfoSet;

class NIPLUGINTOOLKIT_ENTRY NiScriptInfoDialogs
{
    public:
        /// Creates a modal dialog box that allows a user to create 
        /// and manage existing NiScriptInfo objects
        static NiScriptInfoPtr DoManagementDialog(NiScriptInfo* pkDefaultInfo,
            NiScriptInfoSet* pkInputSet, NiWindowRef kWindow, 
            NiString strTypes);

        /// Creates a modal dialog box that allows a user to select
        /// from existing NiScriptInfo objects
        static NiScriptInfoPtr DoSelectionDialog(NiScriptInfo* pkDefaultInfo,
            NiScriptInfoSet* pkInputSet, NiWindowRef kWindow, 
            NiString strTypes);

        /// Creates a modal dialog box that allows a user to save
        /// an existing script
        static ReturnCode DoScriptSaveDialog(NiScriptInfo* pkScript,
            NiWindowRef kWindow, bool bPromptForLoc = true);

        /// Creates a modal dialog box that allows a user to open
        /// scripts and adds them to the known script list
        static NiScriptInfo* DoScriptOpenDialog(NiWindowRef kWindow, 
            bool bAddToManager = true);

        // *** begin Emergent internal use only ***

        /// Useful method for generating the default script for execution. 
        /// It adds the generated default script to the NiScriptInfoSet if 
        /// it doesn't already exist
        /// Check performed:
        /// 1) The input script exists. If it doesn't, use the last script 
        /// executed by the plugin manager.
        /// 2) The input script uses valid plugins loaded by the framework. 
        /// If not, strip off the offending plugin info objects and return
        /// 3) If the script matches an existing template from the 
        /// NiScriptInfoSet, 
        /// check to see if it differs from the template. If it differs, 
        /// ask the user if they want to use the template or the original 
        /// input script.
        static NiScriptInfo* GenerateDefaultScript(NiScriptInfo* pkInfo, 
            NiScriptInfoSet*& pkInfoSet, bool bPromptUser = true, 
            bool bForceTemplateUse = false);

        /// Useful method for generating the default script info set for 
        /// the execution It adds the set of template scripts from the 
        /// NiScriptTemplateManager that use known Process, Export, 
        /// or Import plugins
        static NiScriptInfoSet* GenerateDefaultScriptInfoSet();

        /// Useful method for generating the import and export filenames 
        ///  for an NiScriptInfo object. 
        static void CompileImportExportInfo(const char* pcBaseFilename, 
            NiScriptInfo* pkScript, bool bPromptForFiles, bool bSilentRunning);

        static unsigned int CountNumberOfExportPluginInfos(
            NiScriptInfo* pkScript);
        
        static unsigned int CountNumberOfImportPluginInfos(
            NiScriptInfo* pkScript);

        // *** end Emergent internal use only ***

    protected:
        static bool FileTypesMatch(const char* pcExtension, 
            NiPluginInfo* pkPluginInfo);
};
#endif
