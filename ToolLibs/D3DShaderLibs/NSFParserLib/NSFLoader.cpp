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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NSFParserLibPCH.h"

#include "NSFLoader.h"
#include "NSFParsedShader.h"
#include <NiFilename.h>

#include <NiShaderLibraryDesc.h>
#include <NiShaderDesc.h>


//---------------------------------------------------------------------------
NSFLoader* NSFLoader::ms_pkLoader = 0;
extern NiTPointerList<NSFParsedShader*> g_kParsedShaderList;
extern int ParseShader(const char* pszFileName);
extern void ResetParser(); 
extern void CleanupParser();

//---------------------------------------------------------------------------
NSFLoader::NSFLoader()
{
    m_kNSFTextList.RemoveAll();
}
//---------------------------------------------------------------------------
NSFLoader::~NSFLoader()
{
    m_kNSFTextIter = m_kNSFTextList.GetHeadPos();
    while (m_kNSFTextIter)
    {
        char* pszName = m_kNSFTextList.GetNext(m_kNSFTextIter);
        if (pszName)
            NiFree(pszName);
    }
    m_kNSFTextList.RemoveAll();
}
//---------------------------------------------------------------------------
NSFLoader* NSFLoader::Create()
{
    if (!ms_pkLoader)
        ms_pkLoader = NiNew NSFLoader();

    return ms_pkLoader;
}
//---------------------------------------------------------------------------
void NSFLoader::Destroy()
{
    if (ms_pkLoader)
        NiDelete ms_pkLoader;

    ms_pkLoader = 0;
}
//---------------------------------------------------------------------------
unsigned int NSFLoader::ParseAllNSFFiles(const char* pszDirectory, 
    bool bRecurseSubDirs)
{
    FindAllNSFTextFiles(pszDirectory, bRecurseSubDirs);
    return LoadAllNSFTextFiles();
}
//---------------------------------------------------------------------------
unsigned int NSFLoader::GetTextFileCount()
{
    return m_kNSFTextList.GetSize();
}
//---------------------------------------------------------------------------
const char* NSFLoader::GetFirstTextFile()
{
    m_kNSFTextIter = m_kNSFTextList.GetHeadPos();
    
    if (m_kNSFTextIter)
        return m_kNSFTextList.GetNext(m_kNSFTextIter);
    return 0;
}
//---------------------------------------------------------------------------
const char* NSFLoader::GetNextTextFile()
{
    if (m_kNSFTextIter)
        return m_kNSFTextList.GetNext(m_kNSFTextIter);
    return 0;
}
//---------------------------------------------------------------------------
void NSFLoader::FindAllNSFTextFiles(const char* pszDirectory, 
    bool bRecurseDirectories)
{
    LoadAllNSFFilesInDirectory(pszDirectory, ".NSF", bRecurseDirectories,
        &m_kNSFTextList);
}
//---------------------------------------------------------------------------
bool NSFLoader::ProcessNSFFile(const char* pszFilename, const char* pszExt,
    NiTPointerList<char*>* pkFileList)
{
    if (!pszFilename || (strcmp(pszFilename, "") == 0))
        return false;

    NiFilename kFilename(pszFilename);
    if (NiStricmp(kFilename.GetExt(), pszExt) == 0)
    {
        NILOG(NIMESSAGE_GENERAL_0, "        Found %s File %s\n", pszExt, 
            pszFilename);

        // Add it to the list
        size_t stLen = strlen(pszFilename) + 1;
        char* pszNew = NiAlloc(char, stLen);
        NIASSERT(pszNew);

        NiStrcpy(pszNew, stLen, pszFilename);

        pkFileList->AddTail(pszNew);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
unsigned int NSFLoader::LoadAllNSFTextFiles()
{
    ResetParser(); 

    unsigned int uiCount = 0;

    const char* pszFile = GetFirstTextFile();
    while (pszFile)
    {
        NILOG(NIMESSAGE_GENERAL_0, "Attempting to parse %s\n", pszFile);

        if (ParseShader(pszFile) != 0)
        {
            NILOG(NIMESSAGE_GENERAL_0, "Failed to parse %s\n", pszFile);
        }
        else
        {
            // We will write the binary format out, and then let the binary
            // loading step actually add it...
            // This will 'auto-recompile' any shader files...
            // However, it requires that text files get loaded
            // prior to the binary list being created.
            NiFilename kFilename(pszFile);

            // Grab each shader out of the list
            NSFParsedShader* pkParsedShader;
            NSBShaderPtr spNSB;

            NiTListIterator iter;

            iter = g_kParsedShaderList.GetHeadPos();
            while (iter)
            {
                pkParsedShader = g_kParsedShaderList.GetNext(iter);
                if (!pkParsedShader)
                    continue;

                spNSB = pkParsedShader->GetShader();
                if (spNSB)
                {
                    NILOG(NIMESSAGE_GENERAL_0, 
                        "Parsed Shader %s\n", spNSB->GetName());

                    NILOG(NIMESSAGE_GENERAL_0, 
                        "Storing binary version of shader %s\n",
                        spNSB->GetName());

                    kFilename.SetFilename(spNSB->GetName());
                    kFilename.SetExt(".NSB");

                    char acFullPath[NI_MAX_PATH];
                    kFilename.GetFullPath(acFullPath, NI_MAX_PATH);

                    if (spNSB->Save(acFullPath))
                        uiCount++;

                    spNSB = 0;
                }
            }
        }

        NiTListIterator iter = g_kParsedShaderList.GetHeadPos();
        while (iter)
        {
            NSFParsedShader* pkParsedShader = 
                g_kParsedShaderList.GetNext(iter);
            if (pkParsedShader)
                NiDelete pkParsedShader;
        }
        g_kParsedShaderList.RemoveAll();

        pszFile = GetNextTextFile();
    }

    CleanupParser();

    return uiCount;
}
//---------------------------------------------------------------------------
