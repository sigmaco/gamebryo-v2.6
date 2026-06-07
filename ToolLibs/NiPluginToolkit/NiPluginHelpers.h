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

#ifndef NIPLUGINHELPERS_H
#define NIPLUGINHELPERS_H

#include "NiPluginToolkitLibType.h"
#include "NiPluginToolkitDefinitions.h"
#include "NiPlugin.h"
#include "NiString.h"
#include "NiImportPluginInfo.h"

class NIPLUGINTOOLKIT_ENTRY NiPluginHelpers
{
public:
    static bool DoFileDialog(NiPlugin* pkPlugin, NiString& strDirectory,
        NiString& strFile, NiString& strExtension,
        NiWindowRef hOwner = NULL, const char* pcStartLocation = NULL);

    static bool DoImportFileDialog(NiString& strDirectory,
        NiString& strFile, NiString& strExtension,
        NiWindowRef hOwner = NULL, const char* pcStartLocation = NULL);

    static bool DoExportFileDialog(NiString& strDirectory,
        NiString& strFile, NiString& strExtension,
        NiWindowRef hOwner = NULL, const char* pcStartLocation = NULL);

    static NiString ImportAndExecute(NiImportPluginInfo::ImportType eType
        = NiImportPluginInfo::REPLACE, const char* pcFullPath = NULL);

    static NiString ExportAndExecute(const char* pcFullPath = NULL);

    static void ParseFullPath(NiString strFullPath,
        NiString& strDirectory, NiString& strFile,
        NiString& strExtension);

    static unsigned int GetVersionFromString(const char* pcVersionString);

    static const char* GetLastImportDir();
    static const char* GetLastExportDir();

    // *** begin Emergent internal use only ***
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    static bool RunFileDialog(bool bImport, NiString& strDirectory,
        NiString& strFile, NiString& strExtension, NiWindowRef hOwner, 
        const char* pcStartLocation);
    static bool FileTypesMatch(NiString strExtension, NiPlugin* pkPlugin);
    static void SetLastImportDir(const char* pcLastImportDir);
    static void SetLastExportDir(const char* pcLastExportDir);

    static char* ms_pcLastImportDir;
    static char* ms_pcLastExportDir;
};

#endif  // #ifndef NIPLUGINHELPERS_H
