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

#ifndef NISCRIPTTEMPLATEMANAGER_H
#define NISCRIPTTEMPLATEMANAGER_H

#include "NiPluginToolkitLibType.h"
#include "NiPluginToolkitDefinitions.h"
#include <NiRTTI.h>
#include <NiTPtrSet.h>
#include "NiScriptInfo.h"
#include "NiScriptReader.h"
#include "NiScriptWriter.h"
#include "NiScriptInfoSet.h"

typedef NiTPtrSet<NiScriptReaderPtr, 
    NiTNewInterface<NiScriptReaderPtr> > NiScriptReaderPtrSet;
typedef NiTPtrSet<NiScriptWriterPtr, 
    NiTNewInterface<NiScriptWriterPtr> > NiScriptWriterPtrSet;

/// Class designed to load Scripts from files into memory as converted
/// NiScriptInfo template objects. Allows access to management dialogs 
/// to manipulate the NiScriptInfo templates.
class NIPLUGINTOOLKIT_ENTRY NiScriptTemplateManager : public NiMemObject
{
public:
    NiDeclareRootRTTI(NiScriptTemplateManager);

    /// The public singleton creator.
    static void CreateInstance();

    /// The public singleton destroyer.
    static void DestroyInstance();
    
    /// The public singleton accessor.
    /// @return A pointer to the one and only instance of the class or
    ///         NULL if no instance exists.
    static NiScriptTemplateManager* GetInstance();

    /// Read in the script, but does NOT add it to the Script Manager
    static NiScriptInfo* OpenScript(
        NiString strScriptFile, bool bSilentRunning = false);

    /// Parses in a script composed of the input string, 
    /// but does NOT add it to the Script Manager
    static NiScriptInfo* ParseScript(NiString strScriptFile, 
        NiScriptReader* pkReader, bool bSilentRunning = false);

    /// Write the script but does NOT add it to the Script Manager
    static NiScriptInfo* SaveScript(NiString strScriptFile, 
        NiScriptInfo* pkScript, bool bSilentRunning = false);

    /// Used by the static functions to indicate the success or type of failure
    static ReturnCode GetLastReturnCode();

    /// Add a script to the list of known scripts, 
    ReturnCode AddScript(NiScriptInfo* pkInfo, bool bTemplate = false, 
        bool bSilentRunning = false);

    /// Add a script to the list of known scripts, 
    ReturnCode AddScript(const char* pcPath, bool bSilentRunning = false);

    /// Add a script directory and all of the scripts in it
    ReturnCode AddScriptDirectory(const char* pcPath, bool bRecurse = true,
        bool bSilentRunning = false);

    /// Remove a script by name
    NiScriptInfoPtr RemoveScript(const char* pcScript);

    /// Remove all
    void RemoveAllScripts();

    /// Write a script out to file using the file extension to determine
    /// which writer will be used.
    ReturnCode WriteScript(NiScriptInfo* pkScriptInfo, const char* pcFilename,
        bool bSilentRunning = false);

    /// Get a script info by name
    NiScriptInfo* GetScript(const char* pcName);

    /// Get the number of known scripts
    unsigned int GetScriptCount();

    /// Get the script at the position ui
    NiScriptInfo* GetScriptAt(unsigned int ui);

    /// Get the index of the script by the given name
    unsigned int GetScriptIndex(const char* pcName);

    /// Create a new script using all of the default plugins
    static NiScriptInfo* CreateNewScript();

    /// Adds a script reader to the list of known script readers
    void AddScriptReader(NiScriptReader* pkReader);

    /// Adds a script writer to the list of known script writers
    void AddScriptWriter(NiScriptWriter* pkWriter);

    /// Find the file extension for the input path
    NiString ExtractExtension(NiString strPath);

    /// Find the reader matching the input file extension
    NiScriptReader* GetReaderForExtension(const char* pcExt);

    /// Find the writer matching the input file extension
    NiScriptWriter* GetWriterForExtension(const char* pcExt);

    /// Get the NiScriptInfo set used by the script manager
    NiScriptInfoSet& GetScriptInfoSet();

    /// This method uses the name of the input script to locate
    /// the script it needs to replace. If a script of the same name is not 
    /// present, it adds to the end of this set. Otherwise, it copies over 
    /// the script in the set.
    NiScriptInfoPtr ReplaceScript(NiScriptInfo* pkNewScript);
    
    /// Get a set containing pointers to all the script reader objects. Used
    /// when writing files
    NiScriptReaderPtrSet& GetAllReaders();

    /// Get a set containing pointers to all the script writer objects. Used
    /// when writing files
    NiScriptWriterPtrSet& GetAllWriters();

    /// Get the last directory added to the ScriptTemplateManager
    const char* GetLastDirectoryAdded();

protected:
    /// Protected default constructor. This is protected to enforce the
    /// Singleton pattern.
    NiScriptTemplateManager();

    /// Used internally to store the return code
    static ReturnCode Return(ReturnCode eReturn);

    /// Protected copy constructor. This is protected to enforce the
    /// Singleton pattern.
    NiScriptTemplateManager(NiScriptTemplateManager&){}

    /// Protected virtual destructor. This is protected to enforce the
    /// Singleton pattern.
    virtual ~NiScriptTemplateManager();
    
    /// Static 'this' pointer
    static NiScriptTemplateManager* ms_pkThis;

    /// Last known error code generated by ScriptManager
    static ReturnCode ms_eLastReturn;

    /// Last known directory used to open or save a file
    static char ms_acLastDirectory[1024];

    /// A set containing pointers to all the scripts controlled by the
    /// manager. Each script will only appear in this list once. 
    NiScriptInfoSet m_kAllScripts;

    /// A set containing pointers to all the script reader objects. Used
    /// when parsing files and directories
    NiScriptReaderPtrSet m_kAllReaders;

    /// A set containing pointers to all the script writer objects. Used
    /// when writing files
    NiScriptWriterPtrSet m_kAllWriters;
};

#endif
