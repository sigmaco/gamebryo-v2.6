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

#include "NiScriptTemplateManager.h"
#include "NiDefaultScriptReader.h"
#include "NiDefaultScriptWriter.h"
#include "NiScriptInfo.h"
#include "NiViewerPluginInfo.h"
#include "NiSystem.h"
#include "NiFileFinder.h"
#include "NiStringTokenizer.h"
#include "NiPluginToolkitDefinitions.h"
#include "NiScriptInfoSet.h"
#include "NiScriptConvertDialog.h"
#include "NiPlugin.h"

ReturnCode  NiScriptTemplateManager::ms_eLastReturn = SUCCESS;
char NiScriptTemplateManager::ms_acLastDirectory[1024] = {'\0'};
//---------------------------------------------------------------------------
NiImplementRootRTTI(NiScriptTemplateManager);
NiScriptTemplateManager* NiScriptTemplateManager::ms_pkThis = NULL;

//---------------------------------------------------------------------------
/// The public singleton creator.
void NiScriptTemplateManager::CreateInstance()
{ 
    ms_pkThis = NiNew NiScriptTemplateManager;
}

//---------------------------------------------------------------------------
/// The public singleton destroyer.
void NiScriptTemplateManager::DestroyInstance()
{ 
    NiDelete ms_pkThis;
    ms_pkThis = NULL;
}

//---------------------------------------------------------------------------
/// The public singleton accessor.
/// @return A pointer to the one and only instance of the class or
///         NULL if no instance exists.
NiScriptTemplateManager* NiScriptTemplateManager::GetInstance()
{ 
    return ms_pkThis;
}

//---------------------------------------------------------------------------
NiScriptTemplateManager::~NiScriptTemplateManager()
{
    m_kAllScripts.RemoveAllScripts();
}

//---------------------------------------------------------------------------
/// Add a script to the list of known scripts, 
ReturnCode NiScriptTemplateManager::AddScript(NiScriptInfo* pkInfo, 
    bool bTemplate, bool bSilentRunning)
{
    ReturnCode eReturn = 
        m_kAllScripts.AddScript(pkInfo, bTemplate, bSilentRunning);

    return Return(eReturn);   
}

//---------------------------------------------------------------------------
/// Add a script to the list of known scripts, 
ReturnCode NiScriptTemplateManager::AddScript(const char* pcPath, 
    bool bSilentRunning)
{
    NiString strExt = ExtractExtension(pcPath);
    NiScriptReader* pkReader = GetReaderForExtension(strExt);

    if (pkReader)
    {
        NiString strErrors;

        NiScriptInfo* pkInfo = OpenScript(pcPath, bSilentRunning);
        if (pkInfo == NULL)
            return GetLastReturnCode();
        return AddScript(pkInfo, true, bSilentRunning);
    }

    return Return(NO_READER_FOR_FILE);
}

//---------------------------------------------------------------------------
/// Add a script directory and all of the scripts in it
ReturnCode NiScriptTemplateManager::AddScriptDirectory(
    const char* pcPath, bool bRecurse, bool bSilentRunning)
{ 
    NiSprintf(ms_acLastDirectory, 1024, "%s", pcPath);
    unsigned int uiNumFiles = 0;

    for (unsigned int ui = 0; ui < m_kAllReaders.GetSize();ui++)
    {
        NiScriptReader* pkReader = m_kAllReaders.GetAt(ui);
        
        if (!pkReader)
            continue;

        NiStringTokenizer kTokenizer(pkReader->GetFileExtensions());
        NiString strExt = kTokenizer.GetNextToken("*;");
        while (!strExt.IsEmpty())
        {
            NiFileFinder kFinder(pcPath, bRecurse, strExt);
            
            while(kFinder.HasMoreFiles())
            {
                NiFoundFile* pkFile = kFinder.GetNextFile();
                if (pkFile)
                {
                    ReturnCode eReturn = 
                        AddScript(pkFile->m_strPath, bSilentRunning);
                    if (eReturn == SUCCESS)
                        uiNumFiles++;
                }
            }
            strExt = kTokenizer.GetNextToken("*;");
        }
    }
    
    if (uiNumFiles == 0)
    {
        if (!bSilentRunning)
        {
            NiString strErrors = "Unable to find any scripts at:\n\t";
            strErrors += pcPath;
            NiMessageBox(strErrors,"No Scripts Found Error");
        }
        return Return(NO_SCRIPTS_FOUND);
    }

    return Return(SUCCESS);
}
//---------------------------------------------------------------------------
/// Write a script out to file using the file extension to determine
/// which writer will be used.
ReturnCode NiScriptTemplateManager::WriteScript(
    NiScriptInfo* pkScriptInfo, const char* pcFilename, bool bSilentRunning)
{ 
    SaveScript(pcFilename, pkScriptInfo, bSilentRunning);
    return GetLastReturnCode();
}

//---------------------------------------------------------------------------
/// Get a script info by name
NiScriptInfo* NiScriptTemplateManager::GetScript(const char* pcName)
{ 
    return m_kAllScripts.GetScript(pcName);
}
//---------------------------------------------------------------------------
/// Create a new script using all of the default plugins
NiScriptInfo* NiScriptTemplateManager::CreateNewScript()
{
    NiScriptInfo* pkScript = NiNew NiScriptInfo;
    pkScript->SetName("New Script");
    pkScript->SetType("Process");
    return pkScript;
}
//---------------------------------------------------------------------------
/// Adds a script reader to the list of known script readers
void NiScriptTemplateManager::AddScriptReader(NiScriptReader* pkReader)
{ 
    if (pkReader)
        m_kAllReaders.AddUnique(pkReader);
}
//---------------------------------------------------------------------------
/// Adds a script writer to the list of known script writers
void NiScriptTemplateManager::AddScriptWriter(NiScriptWriter* pkWriter)
{ 
    if (pkWriter)
        m_kAllWriters.AddUnique(pkWriter);
}

//---------------------------------------------------------------------------
/// Protected default constructor. This is protected to enforce the
/// Singleton pattern.
NiScriptTemplateManager::NiScriptTemplateManager()
{
    m_kAllScripts.IgnoreMissingPlugins(false);
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/// Find the file extension for the input path
NiString NiScriptTemplateManager::ExtractExtension(NiString strPath)
{
    int iMatch = strPath.FindReverse('.');
    if (iMatch >= 0)
        return strPath.GetSubstring(iMatch, strPath.Length());
    else
        return NiString("\0");
}

//---------------------------------------------------------------------------
/// Find the reader matching the input file extension
NiScriptReader* NiScriptTemplateManager::GetReaderForExtension(
    const char* pcExt)
{
    for (unsigned int ui=0; ui < m_kAllReaders.GetSize(); ui++)
    {
        NiScriptReader* pkReader = m_kAllReaders.GetAt(ui);
        if (pkReader)
        {
            int iMatch = pkReader->GetFileExtensions().Find(pcExt);
            if (iMatch > 0 )
                return pkReader;
        }
    }
    return NULL;
}

//---------------------------------------------------------------------------
/// Find the writer matching the input file extension
NiScriptWriter* NiScriptTemplateManager::GetWriterForExtension(
    const char* pcExt)
{
    for (unsigned int ui=0; ui < m_kAllWriters.GetSize(); ui++)
    {
        NiScriptWriter* pkWriter = m_kAllWriters.GetAt(ui);
        if (pkWriter)
        {
            int iMatch = pkWriter->GetFileExtensions().Find(pcExt);
            if (iMatch > 0 )
                return pkWriter;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
/// Get the number of known scripts
unsigned int NiScriptTemplateManager::GetScriptCount()
{
    return m_kAllScripts.GetScriptCount();
}

//---------------------------------------------------------------------------
/// Get the script at the position ui
NiScriptInfo* NiScriptTemplateManager::GetScriptAt(unsigned int ui)
{
    return m_kAllScripts.GetScriptAt(ui);
}
//---------------------------------------------------------------------------
unsigned int NiScriptTemplateManager::GetScriptIndex(const char* pcName)
{
    return m_kAllScripts.GetScriptIndex(pcName);
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiScriptTemplateManager::RemoveScript(const char* pcScript)
{
    NiScriptInfoPtr spScript = m_kAllScripts.RemoveScript(pcScript);
    return spScript;
}
//---------------------------------------------------------------------------
void NiScriptTemplateManager::RemoveAllScripts()
{
    m_kAllScripts.RemoveAllScripts();
}
//---------------------------------------------------------------------------
/// Read in the script, but does NOT add it to the Script Manager
NiScriptInfo* NiScriptTemplateManager::OpenScript(NiString strPath,
    bool bSilentRunning)
{
    NIASSERT(ms_pkThis);
    NiString kExt = ms_pkThis->ExtractExtension(strPath);
    NiScriptReader* pkReader = ms_pkThis->GetReaderForExtension(kExt);
    FILE * stream;

#if !defined(_MSC_VER)
#error File can only be built using Visual Studio
#endif //#if !defined(_MSC_VER)

#if _MSC_VER >= 1400
    fopen_s(&stream, (const char*) strPath, "r" );
#else //#if _MSC_VER >= 1400
    stream = fopen( (const char*) strPath, "r" );
#endif //#if _MSC_VER >= 1400

    if (!stream)
    {
        if (!bSilentRunning)
        {
            NiString strErrors = 
                "File \"" + strPath + "\" not found or unable"
                " to be loaded!";
            NiMessageBox(strErrors, "File error");
        }
        Return(FILE_NOT_FOUND);
        return NULL;
    }

    NiFileFinder kFinder(strPath);
    NiFoundFile* pkFile = kFinder.GetNextFile();
    
    if (!pkFile)
    {
        Return(FILE_NOT_FOUND);
        fclose( stream ); 
        return NULL;
    }

    char* acString = NiAlloc(char, pkFile->m_uiSize + 1);
    int iBytes = (int)fread( acString, sizeof(char), pkFile->m_uiSize, stream);

    if (iBytes < 0)
    {
        NiString strErrors = "Error loading file: " + strPath;
        NiFree(acString);
        fclose(stream);
        
        if (!bSilentRunning)
            NiMessageBox(strErrors, "Error loading file");
        Return(IO_FAILURE);
        return NULL;
    }

    // Mark the end of the string and skip the header.
    acString[iBytes] = '\0';
    NiString strFile(acString);

    NiScriptInfo* pkInfo = ParseScript(strFile, pkReader, bSilentRunning);
 
    if (!pkInfo)
    {
        NiString strErrors = "Error parsing script file: " + strPath;
        NiFree(acString);
        fclose(stream);

        if (!bSilentRunning)
            NiMessageBox(strErrors, "Error parsing script file");
        return NULL;
    }

    NiFree(acString);
    fclose( stream );

    // If we are dirty this means we need to save because of a convert.
    if (pkInfo->IsDirty())
    {
        SaveScript(strPath, pkInfo, bSilentRunning);
    }

    pkInfo->SetFile(strPath);
    pkInfo->MarkAsDirty(false);

    return pkInfo;
}

//---------------------------------------------------------------------------
/// Parses in a script composed of the input string, 
/// but does NOT add it to the Script Manager
NiScriptInfo* NiScriptTemplateManager::ParseScript(NiString strFile, 
    NiScriptReader* pkReader, bool bSilentRunning)
{
    if (strFile.IsEmpty())
    {
        Return(NO_READER_FOR_FILE);
        return NULL;
    }

    if (!pkReader)
    {   
        if (!bSilentRunning)
        {
            NiString strErrors = "\nNo Reader Specified!\n";
            NiMessageBox(strErrors, "Error parsing file");
        }
        Return(NO_READER_FOR_FILE);
        return NULL;
    }

    NiString strErrors;
    NiScriptInfo* pkInfo = pkReader->Parse(strFile, strErrors, bSilentRunning);

    if (!strErrors.IsEmpty())
    {
        if (!bSilentRunning)
        {
            strErrors = "Error parsing script: \n" + strFile + "\nERRORS:\n" 
                + strErrors;
            NiMessageBox(strErrors, "Error parsing file");
        }
        Return(FILE_PARSE_ERROR);
        return NULL;
    }

    Return(SUCCESS);
    return pkInfo;
}

//---------------------------------------------------------------------------
/// Write the script but does NOT add it to the Script Manager
NiScriptInfo* NiScriptTemplateManager::SaveScript(NiString strFilename, 
    NiScriptInfo* pkScript, bool bSilentRunning)
{
    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();
    NiString strExtension = pkManager->ExtractExtension(strFilename);
    NiScriptWriter* pkWriter = pkManager->GetWriterForExtension(strExtension);

    if (pkWriter)
    {
        FILE * stream;
 #if _MSC_VER >= 1400
        fopen_s(&stream, (const char*) strFilename, "w" );
#else //#if _MSC_VER >= 1400
        stream = fopen( (const char*) strFilename, "w" );
#endif //#if _MSC_VER >= 1400

        if (!stream)
        {
            if (!bSilentRunning)
            {
                NiString strMessage(512);
                strMessage.Format(
                    "File \"%s\" could not be opened for writing!\n"
                    "Check to make sure that the file is not write-protected.",
                    (const char*)strFilename);
                NiMessageBox(strMessage, "Error writing script!");
            }

            Return(IO_FAILURE);
            return NULL;
        }

        NiString strEncoding = pkWriter->Encode(pkScript);
        if (fprintf(stream, "%s", (const char*) strEncoding) < 0)
        {
            if (!bSilentRunning)
            {
                NiString strMessage(512);
                strMessage.Format("File \"%s\" could not be written to!\n"
                    "Check to make sure that the file is not write-protected.",
                    (const char*)strFilename);
                NiMessageBox(strMessage, "Error writing script!");
            }
            Return(IO_FAILURE);
            fclose( stream ); 
            return NULL;
        }
        fclose( stream ); 
    }
    else
    {
        if (!bSilentRunning)
        {
            NiString strMessage(512);
            strMessage.Format("Unable to find a writer for file \"%s\"",
                (const char*) strFilename);
            NiMessageBox(strMessage, "Error writing script!");
        }
        Return(NO_WRITER_FOR_FILE);
        return NULL;
    }
    pkScript->SetFile(strFilename);
    pkScript->MarkAsDirty(false);
    Return(SUCCESS);
    return pkScript;
}
//---------------------------------------------------------------------------
    /// Used internally to store the return code
ReturnCode NiScriptTemplateManager::Return(ReturnCode eReturn)
{
    ms_eLastReturn = eReturn;
    return eReturn;
}

//---------------------------------------------------------------------------
    /// Used internally to store the return code
ReturnCode NiScriptTemplateManager::GetLastReturnCode()
{
    return ms_eLastReturn;
}

//---------------------------------------------------------------------------
NiScriptInfoSet& NiScriptTemplateManager::GetScriptInfoSet()
{
    return m_kAllScripts;
}

//---------------------------------------------------------------------------
/// This method uses the name of the input script to locate
/// the script it needs to replace. If a script of the same name is not 
/// present, it adds to the end of this set. Otherwise, it copies over 
/// the script in the set.
NiScriptInfoPtr 
NiScriptTemplateManager::ReplaceScript(NiScriptInfo* pkNewScript)
{
    return m_kAllScripts.ReplaceScript(pkNewScript);
}
//---------------------------------------------------------------------------
/// Get a set containing pointers to all the script reader objects. Used
/// when writing files
NiScriptReaderPtrSet& NiScriptTemplateManager::GetAllReaders()
{
    return m_kAllReaders;
}

//---------------------------------------------------------------------------
/// Get a set containing pointers to all the script writer objects. Used
/// when writing files
NiScriptWriterPtrSet& NiScriptTemplateManager::GetAllWriters()
{
    return m_kAllWriters;
}

//---------------------------------------------------------------------------
const char* NiScriptTemplateManager::GetLastDirectoryAdded()
{
    return ms_acLastDirectory;
}
