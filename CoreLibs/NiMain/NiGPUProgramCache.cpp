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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiGPUProgramCache.h"
#include "NiBool.h"
#include <NiSystem.h>

#define CACHE_FILE_VERSION 7
#define CACHE_FILE_STRING_HEADER "NiGPUProgram Cache File"

bool NiGPUProgramCache::ms_bOverwriteDeprecatedCacheFiles = true;

NiImplementRootRTTI(NiGPUProgramCache);
//---------------------------------------------------------------------------
NiGPUProgramCache::NiGPUProgramCache(unsigned int uiVersion, 
    bool bAutoWriteCacheToDisk, bool bLocked) : 
    m_kValidTargets(10), m_uiVersion(uiVersion), 
    m_bAutoWriteCacheToDisk(bAutoWriteCacheToDisk), m_bLoaded(false),
    m_bLocked(bLocked)
{
}
//---------------------------------------------------------------------------
NiGPUProgramCache::~NiGPUProgramCache() 
{
}
//---------------------------------------------------------------------------
NiGPUProgram* NiGPUProgramCache::FindCachedProgram(
    const char* pcName, NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms,
    bool& bFailedToCompilePreviously)
{
    NiFixedString kKey = pcName;
    NiGPUProgramDescPtr spDesc;
    bFailedToCompilePreviously = false;

    if (m_kCachedPrograms.GetAt(kKey, spDesc))
    {
        unsigned int uiNumUniforms = spDesc->m_kUniforms.GetSize();
        kUniforms.ReallocNoShrink(uiNumUniforms);
        for (unsigned int ui = 0; ui < uiNumUniforms; ui++)
        {
            kUniforms.Add(spDesc->m_kUniforms.GetAt(ui));
        }
        return spDesc->m_spProgram;
    }

    
    if (m_kFailedPrograms.GetAt(kKey, bFailedToCompilePreviously))
        return NULL;

    return NULL;
}
//---------------------------------------------------------------------------
NiGPUProgram* NiGPUProgramCache::GenerateProgram(
    const char*, const char*, 
    NiTObjectPtrSet<NiMaterialResourcePtr>&)
{
    return NULL;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::PruneUnusedConstants(
    NiGPUProgram* pkProgram, NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms)
{
    // Remove any uniforms that are not referenced by the GPU program
    const unsigned int uiUniformCount = kUniforms.GetSize();
    const unsigned int uiShaderVariableCount = pkProgram->GetVariableCount();

    // Mark the used uniform entries
    bool abKeepEntries[1024];
    NIASSERT(1024 > uiUniformCount);
    memset(abKeepEntries, 0, 1024*sizeof(bool));
    
    for (unsigned int uiUniformIdx = 0; uiUniformIdx < uiUniformCount;
        uiUniformIdx++)
    {
        NiMaterialResource* pkRes = kUniforms.GetAt(uiUniformIdx);
        if (pkRes)
        {
            const char* pcUniformName = pkRes->GetVariable();

#if defined(_PS3)
            // For array uniforms on the PS3, look up elements instead, as
            // the actual array name won't be marked as used in the PS3 shader
            // binaries
            const unsigned int uiResCount = pkRes->GetCount();
            char acArrayName[1024];
            for (unsigned int uiArrayElemIdx = 0;
                uiArrayElemIdx < uiResCount;
                ++uiArrayElemIdx)
            {
                if (uiResCount > 1)
                {
                    NiSprintf(acArrayName, 1024, "%s[%d]", pcUniformName,
                        uiArrayElemIdx);
                    pcUniformName = acArrayName;
                }
#else
            {
#endif
                // Look through the actual shader constants to see if any match
                // the uniform under test
                for (unsigned int ui = 0; ui < uiShaderVariableCount; ui++)
                {
                    const char* pcVarName = pkProgram->GetVariableName(ui);
                    if (strcmp(pcUniformName, pcVarName) == 0)
                    {
                        abKeepEntries[uiUniformIdx] = true;

                        // Break out to the next uniform, this one is good
#if defined(_PS3)
                        uiArrayElemIdx = uiResCount;
#endif
                        break;
                    }
                }
            }

        }
    }

    // Now remove the unmarked entries
    for (int iIdx = uiUniformCount - 1; iIdx >= 0; iIdx--)
    {
        if (!abKeepEntries[iIdx])
            kUniforms.OrderedRemoveAt(iIdx);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::Clear()
{
    m_kCachedPrograms.RemoveAll();
    m_kFailedPrograms.RemoveAll();
    m_bLoaded = false;
    return true;
}
//---------------------------------------------------------------------------
const NiTObjectPtrSet<NiFixedString>* 
    NiGPUProgramCache::GetValidCompileTargetsForPlatform() const
{
    return &m_kValidTargets;
}
//---------------------------------------------------------------------------
const NiFixedString& NiGPUProgramCache::GetShaderProfile() const
{
    return m_kShaderProfile;
}
//---------------------------------------------------------------------------
const NiFixedString& NiGPUProgramCache::GetWorkingDirectory() const
{
    return m_kWorkingDir;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::SetWorkingDirectory(const NiFixedString& kFilename)
{
    NiFixedString kOldWorkingDir = m_kWorkingDir;
    if (kFilename.Exists() && NiPath::IsRelative(kFilename))
    {
        char acString[NI_MAX_PATH];
        char acCWD[NI_MAX_PATH];

        if (NiPath::GetCurrentWorkingDirectory(acCWD, NI_MAX_PATH))
        {
            if (0 != NiPath::ConvertToAbsolute(acString, NI_MAX_PATH, 
                kFilename, acCWD))
            {
                m_kWorkingDir = acString;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else 
    {
        m_kWorkingDir = kFilename;
    }

    if (kOldWorkingDir != m_kWorkingDir)
        ComputeFullFilename();
    return true;
}
//---------------------------------------------------------------------------
void NiGPUProgramCache::ComputeFullFilename()
{
    if (!m_kWorkingDir.Exists() || !m_kFilename.Exists())
    {
        m_kPathAndFilename = NULL;
        m_bLoaded = false;
        return;
    }

    NiFixedString kCacheFile = m_kPathAndFilename;

    char acString[NI_MAX_PATH];
    NIVERIFY(NiPath::ConvertToAbsolute(acString, NI_MAX_PATH, 
        m_kFilename, m_kWorkingDir) > 0);
    m_kPathAndFilename = acString;

    if (kCacheFile != m_kPathAndFilename)
        Clear();
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::ValidateDirectory()
{
    if (m_kWorkingDir.Exists())
    {
        if (NiFile::DirectoryExists(m_kWorkingDir))
            return true;

        bool bSuccess = NiFile::CreateDirectoryRecursive(m_kWorkingDir);
        return bSuccess;
    }

    return false;
}

//---------------------------------------------------------------------------
NiString NiGPUProgramCache::GetPlatformSpecificInOutSemantic(
    const NiFixedString& kAgnosticSemantic)
{
    if (kAgnosticSemantic == "Position")
        return "POSITION";
    if (kAgnosticSemantic == "BlendWeight")
        return "BLENDWEIGHT";
    if (kAgnosticSemantic == "BlendIndices")
        return "BLENDINDICES";
    if (kAgnosticSemantic == "Normal")
        return "NORMAL";
    if (kAgnosticSemantic == "PSize")
        return "PSIZE";
    if (kAgnosticSemantic == "TexCoord")
        return "TEXCOORD";
    if (kAgnosticSemantic == "Tangent")
        return "TANGENT";
    if (kAgnosticSemantic == "Binormal")
        return "BINORMAL";
    if (kAgnosticSemantic == "TessFactor")
        return "TESSFACTOR";
    if (kAgnosticSemantic == "PositionT")
        return "POSITIONT";
    if (kAgnosticSemantic == "Color")
        return "COLOR";
    if (kAgnosticSemantic == "Fog")
        return "FOG";
    if (kAgnosticSemantic == "Depth")
        return "DEPTH";
    if (kAgnosticSemantic == "Sample")
        return "SAMPLE";
    NIASSERT(!"Could not find matching semantic");

    return "TEXCOORD";
}
//---------------------------------------------------------------------------
unsigned int NiGPUProgramCache::GetPlatformSpecificInOutSemanticOrder(
    const NiFixedString& kAgnosticSemantic)
{
    if (kAgnosticSemantic == "Position")
        return 0;
    if (kAgnosticSemantic == "BlendWeight")
        return 1;
    if (kAgnosticSemantic == "BlendIndices")
        return 2;
    if (kAgnosticSemantic == "Normal")
        return 3;
    if (kAgnosticSemantic == "PSize")
        return 4;
    if (kAgnosticSemantic == "TexCoord")
        return 5;
    if (kAgnosticSemantic == "Tangent")
        return 6;
    if (kAgnosticSemantic == "Binormal")
        return 7;
    if (kAgnosticSemantic == "TessFactor")
        return 8;
    if (kAgnosticSemantic == "PositionT")
        return 9;
    if (kAgnosticSemantic == "Color")
        return 10;
    if (kAgnosticSemantic == "Fog")
        return 11;
    if (kAgnosticSemantic == "Depth")
        return 12;
    if (kAgnosticSemantic == "Sample")
        return 13;
    NIASSERT(!"Could not find matching semantic");

    return 6;
}
//---------------------------------------------------------------------------
unsigned int NiGPUProgramCache::GetPlatformSpecificSemanticCount()
{
    return 13;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::Load()
{
    if (!ValidateDirectory())
    {
        NiOutputDebugString("NiGPUProgramCache Warning >");
        NiOutputDebugString("ValidateDirectory() failed.");
        NiOutputDebugString("\n");
        return false;
    }

    // No need to reload the same file
    if (m_bLoaded)
        return true;

    NiFile* pkFile = NiFile::GetFile(m_kPathAndFilename, NiFile::READ_ONLY);
    if (pkFile && true == *(pkFile))
    {
        // Write header
        const char* pcHeader = CACHE_FILE_STRING_HEADER;
        char acBuffer[512];
        NiStreamLoadBinary(*(pkFile), acBuffer, (unsigned int)(strlen(pcHeader) + 1));

        if (NiStricmp(acBuffer, pcHeader) != 0)
        {
            NiOutputDebugString("NiGPUProgramCache Warning >");
            NiOutputDebugString("Cache is not the correct file type and will "
                "not be loaded: \"");
            NiOutputDebugString((const char*)m_kPathAndFilename);
            NiOutputDebugString("\"\n");
            NiDelete pkFile;
            return false;
        }

        NiBool kBool;
        NiStreamLoadBinary(*(pkFile), kBool);

        bool bIsLittleEndian = kBool != 0 ? true : false;
        if (bIsLittleEndian && !NiSystemDesc::GetSystemDesc().IsLittleEndian())
            pkFile->SetEndianSwap(true);

        unsigned int uiCacheFileVersion;;
        NiStreamLoadBinary(*(pkFile), uiCacheFileVersion);

        if (uiCacheFileVersion < CACHE_FILE_VERSION)
        {
            NiOutputDebugString("NiGPUProgramCache Warning >");
            NiOutputDebugString("Cache file is older version and will not"
                " be loaded: \"");
            NiOutputDebugString((const char*)m_kPathAndFilename);
            NiOutputDebugString("\"\n");
            NiDelete pkFile;

            if (ms_bOverwriteDeprecatedCacheFiles)
            {
                NiOutputDebugString("Deprecated cache file will now be"
                    " overwritten with the current version. To change this "
                    "behavior, call NiGPUProgram::"
                    "SetOverwriteDeprecatedCacheFiles(false);\n");
                Save();
            }
            return false;
        }

        // Write Platform
        LoadString(*(pkFile), acBuffer, 512);
        if (NiStricmp(acBuffer, GetPlatform()) != 0)
        {
            NiOutputDebugString("NiGPUProgramCache Warning >");
            NiOutputDebugString("Cache is not the correct platform and will "
                "not be loaded: \"");
            NiOutputDebugString((const char*)m_kPathAndFilename);
            NiOutputDebugString("\"\n");
            NiDelete pkFile;
            return false;
        }

        // Write code id string
        LoadString(*(pkFile), acBuffer, 512);
        if (NiStricmp(acBuffer, GetPlatformSpecificCodeID()) != 0)
        {
            NiOutputDebugString("NiGPUProgramCache Warning >");
            NiOutputDebugString("Cache is not the correct code id and will "
                "not be loaded: \"");
            NiOutputDebugString((const char*)m_kPathAndFilename);
            NiOutputDebugString("\"\n");

            NiDelete pkFile;
            return false;
        }


        // Write shader profile
        LoadString(*(pkFile), acBuffer, 512);
        if (NiStricmp(acBuffer, GetShaderProfile()) != 0)
        {
            NiOutputDebugString("NiGPUProgramCache Warning >");
            NiOutputDebugString("Cache is not the correct shader profile and "
                "will not be loaded: \"");
            NiOutputDebugString((const char*)m_kPathAndFilename);
            NiOutputDebugString("\"\n");

            NiDelete pkFile;
            return false;
        }


        bool bAnyDeprecated = false;
#ifdef NIDEBUG
        unsigned int uiNumPrograms = m_kCachedPrograms.GetCount();
#endif
        while (pkFile->GetPosition() < pkFile->GetFileSize())
        {
            bool bDeprecated = false;
            bool bSuccess = LoadEntry(*(pkFile), bDeprecated);
            if (!bSuccess)
            {
                NiOutputDebugString("NiGPUProgramCache Warning >");
                NiOutputDebugString("Load Entry failed. Cache file will "
                    "not be loaded: \"");
                NiOutputDebugString((const char*)m_kPathAndFilename);
                NiOutputDebugString("\"\n");

                NiDelete pkFile;
                return false;
            }

            if (bDeprecated)
                bAnyDeprecated = true;
        }

#ifdef NIDEBUG
        unsigned int uiNumProgramsAdded = m_kCachedPrograms.GetCount() - 
            uiNumPrograms;
        NILOG("NiGPUProgramCache> %d programs were loaded"
            " from file: \"%s\"\n", uiNumProgramsAdded, 
            (const char*)m_kPathAndFilename);
#endif

        NiDelete pkFile;
        m_bLoaded = true;

        if (bAnyDeprecated && ms_bOverwriteDeprecatedCacheFiles)
        {
            NiOutputDebugString("Deprecated cache file will now be"
                " overwritten with the current version. To change this "
                "behavior, call NiGPUProgram::"
                "SetOverwriteDeprecatedCacheFiles(false);\n");
            Save();
        }
        return true;
    }

    NiDelete pkFile;

    NiOutputDebugString("NiGPUProgramCache Warning >");
    NiOutputDebugString("NiFile creation failed. Cache file does not exist "
        "and therefore cannot be loaded: \"");
    NiOutputDebugString((const char*)m_kPathAndFilename);
    NiOutputDebugString("\"\n");
    return false;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::IsLoaded()
{
    return m_bLoaded;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::IsLocked()
{
    return m_bLocked;
}
//---------------------------------------------------------------------------
void NiGPUProgramCache::AddFailedProgram(const char* pcProgram)
{
    NiFixedString kName = pcProgram;
    
    m_kFailedPrograms.SetAt(kName, true);

    if (m_bAutoWriteCacheToDisk)
    {
        if (!AppendEntry(NULL, kName))
        {
            NiOutputDebugString("Failed to write cache entry, \"");
            NiOutputDebugString((const char*) kName);
            NiOutputDebugString("\", to cache file: \"");
            NiOutputDebugString((const char*) m_kPathAndFilename);
            NiOutputDebugString("\"\n");
        }
    }
}
//---------------------------------------------------------------------------
void NiGPUProgramCache::SetOverwriteDeprecatedCacheFiles(bool bOverwrite)
{
    ms_bOverwriteDeprecatedCacheFiles = bOverwrite;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::Save()
{
    if (!ValidateDirectory())
    {
        NiOutputDebugString("NiGPUProgramCache Warning >");
        NiOutputDebugString("ValidateDirectory() failed.");
        NiOutputDebugString("\n");
        return false;
    }

    NiFile* pkFile = NiFile::GetFile(m_kPathAndFilename, NiFile::WRITE_ONLY);
    if (pkFile && true == *(pkFile))
    {
        // Write header
        const char* pcHeader = CACHE_FILE_STRING_HEADER;
        NiStreamSaveBinary(*(pkFile), pcHeader, (unsigned int)(strlen(pcHeader) + 1));

        NiBool kBool = NiSystemDesc::GetSystemDesc().IsLittleEndian() ? 1 : 0;
        NiStreamSaveBinary(*(pkFile), kBool);

        unsigned int uiCacheFileVersion = CACHE_FILE_VERSION;
        NiStreamSaveBinary(*(pkFile), uiCacheFileVersion);


        // Write Platform
        SaveString(*(pkFile), GetPlatform());

        // Write code id string
        SaveString(*(pkFile), GetPlatformSpecificCodeID());

        // Write shader profile
        SaveString(*(pkFile), GetShaderProfile());

        NiTMapIterator kIter = m_kCachedPrograms.GetFirstPos();

        while (kIter)
        {
            NiGPUProgramCache::NiGPUProgramDescPtr spDesc;
            NiFixedString kName;

            m_kCachedPrograms.GetNext(kIter, kName, spDesc);

            bool bSuccess = SaveEntry(*(pkFile), spDesc, kName);
            if (!bSuccess)
            {
                NiOutputDebugString("NiGPUProgramCache Warning >");
                NiOutputDebugString("SaveEntry failed. Cache file will "
                    "not be saved: \"");
                NiOutputDebugString((const char*)m_kPathAndFilename);
                NiOutputDebugString("\"\n");
            }
        }

        kIter = m_kFailedPrograms.GetFirstPos();
        while (kIter)
        {
            bool bFailed = true;
            NiFixedString kName;

            m_kFailedPrograms.GetNext(kIter, kName, bFailed);

            bool bSuccess = SaveEntry(*(pkFile), NULL, kName);
            if (!bSuccess)
            {
                NiOutputDebugString("NiGPUProgramCache Warning >");
                NiOutputDebugString("SaveEntry failed. Cache file will "
                    "not be saved: \"");
                NiOutputDebugString((const char*)m_kPathAndFilename);
                NiOutputDebugString("\"\n");
            }
        }


        NiDelete pkFile;
        return true;
    }

    NiOutputDebugString("NiGPUProgramCache Warning >");
    NiOutputDebugString("NiFile creation failed. Cache file will "
        "not be loaded: \"");
    NiOutputDebugString((const char*)m_kPathAndFilename);
    NiOutputDebugString("\"\n");
    return false;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::SaveEntry(NiBinaryStream& kStream, 
    NiGPUProgramCache::NiGPUProgramDesc* pkDesc, const NiFixedString& kName)
{
    NiStreamSaveBinary(kStream, m_uiVersion);
    
    SaveString(kStream, kName);

    NiBool bDidNotCompile = 0;
    if (NULL == pkDesc)
        bDidNotCompile = 1;

    NiStreamSaveBinary(kStream, bDidNotCompile);

    if (bDidNotCompile == 1)
        return true;

    if (!SaveGPUProgram(kStream, pkDesc->m_spProgram))
        return false;

    unsigned int uiResCount = pkDesc->m_kUniforms.GetSize();
    NiStreamSaveBinary(kStream, uiResCount);
    for (unsigned int ui = 0; ui < uiResCount; ui++)
    {
        NiMaterialResource* pkRes = pkDesc->m_kUniforms.GetAt(ui);
        SaveString(kStream, pkRes->GetType());
        SaveString(kStream, pkRes->GetSemantic());
        SaveString(kStream, pkRes->GetLabel());
        SaveString(kStream, pkRes->GetVariable());
        SaveString(kStream, pkRes->GetDefaultValue());

        unsigned int uiCount = pkRes->GetCount();
        NiStreamSaveBinary(kStream, uiCount);

        unsigned int uiSource = (unsigned int)pkRes->GetSource();
        NiStreamSaveBinary(kStream, uiSource);

        unsigned int uiExtraData = pkRes->GetExtraData();
        NiStreamSaveBinary(kStream, uiExtraData);

        unsigned int uiObjectType = (unsigned int)pkRes->GetObjectType();
        NiStreamSaveBinary(kStream, uiObjectType);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::LoadEntry(NiBinaryStream& kStream, bool& bDeprecated)
{
    unsigned int uiVersion;
    NiStreamLoadBinary(kStream, uiVersion);
    
    char acBuffer[512];
    LoadString(kStream, acBuffer, 512);
    NiFixedString kName = acBuffer;

    bool bSkip = false;

    // Skip the entry if its version number is older.
    if (uiVersion != m_uiVersion)
    {
        NiOutputDebugString("NiGPUProgramCache Warning >");
        NiOutputDebugString("Deprecated entry found: \"");
        NiOutputDebugString(acBuffer);
        NiOutputDebugString("\" in file \"");
        NiOutputDebugString((const char*)m_kPathAndFilename);
        NiOutputDebugString("\". It will not be loaded.\n");
        bDeprecated = true;
        bSkip = true;
    }

    // Skip the entry if it is already in the hash table
    {
        NiGPUProgramDescPtr spOtherDesc = NULL;
        if (m_kCachedPrograms.GetAt(kName, spOtherDesc))
        {
            NiOutputDebugString("NiGPUProgramCache Warning >");
            NiOutputDebugString("Attempting to load duplicate entry: \"");
            NiOutputDebugString(acBuffer);
            NiOutputDebugString("\" in file \"");
            NiOutputDebugString((const char*)m_kPathAndFilename);
            NiOutputDebugString("\". Version already in memory will be used"
                " instead.\n");
            bSkip = true;
        }
    }

    NiBool bDidNotCompilePreviously = 0;
    NiStreamLoadBinary(kStream, bDidNotCompilePreviously);

    if (bDidNotCompilePreviously == 1)
    {
        m_kFailedPrograms.SetAt(kName, true);
        return true;
    }

    NiGPUProgramPtr spProgram;
    if (!LoadGPUProgram(kStream, kName, spProgram, bSkip))
        return false;

    NiGPUProgramDesc* pkDesc = NULL;
    if (!bSkip)
    {
        pkDesc = NiNew NiGPUProgramDesc;
        pkDesc->m_spProgram = spProgram;
    }

    unsigned int uiResCount = 0;
    NiStreamLoadBinary(kStream, uiResCount);
    for (unsigned int ui = 0; ui < uiResCount; ui++)
    {
        if (!bSkip)
        {
            NiMaterialResource* pkRes = NiNew NiMaterialResource();
            LoadString(kStream, acBuffer, 512); //TYPE
            pkRes->SetType(acBuffer);

            LoadString(kStream, acBuffer, 512); //SEMANTIC
            pkRes->SetSemantic(acBuffer);

            LoadString(kStream, acBuffer, 512); //LABEL
            pkRes->SetLabel(acBuffer);

            LoadString(kStream, acBuffer, 512); //VARIABLE
            pkRes->SetVariable(acBuffer);

            LoadString(kStream, acBuffer, 512); //DEFAULT VALUE
            pkRes->SetDefaultValue(acBuffer);

            unsigned int uiValue;
            NiStreamLoadBinary(kStream, uiValue); // COUNT
            pkRes->SetCount(uiValue);

            NiStreamLoadBinary(kStream, uiValue); // SOURCE
            pkRes->SetSource((NiMaterialResource::Source)uiValue);

            NiStreamLoadBinary(kStream, uiValue); // EXTRADATA
            pkRes->SetExtraData(uiValue);

            NiStreamLoadBinary(kStream, uiValue); // OBJECT_TYPE
            pkRes->SetObjectType((NiShaderAttributeDesc::ObjectType) uiValue);

            pkDesc->m_kUniforms.Add(pkRes);
        }
        else
        {
            LoadString(kStream, acBuffer, 512); //TYPE
            LoadString(kStream, acBuffer, 512); //SEMANTIC
            LoadString(kStream, acBuffer, 512); //LABEL
            LoadString(kStream, acBuffer, 512); //VARIABLE
            LoadString(kStream, acBuffer, 512); //DEFAULT VALUE
            unsigned int uiValue;
            NiStreamLoadBinary(kStream, uiValue); // COUNT
            NiStreamLoadBinary(kStream, uiValue); // SOURCE
            NiStreamLoadBinary(kStream, uiValue); // EXTRADATA  
            NiStreamLoadBinary(kStream, uiValue); // OBJECT_TYPE

        }
    }

    if (!bSkip && pkDesc->m_spProgram != NULL)
    {
        m_kCachedPrograms.SetAt(kName, pkDesc);
    }
    else
    {
        NiGPUProgramDescPtr spDelete = pkDesc;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::AppendEntry(NiGPUProgramDesc* pkDesc, 
    const NiFixedString& kName)
{
    // If the cache file has never been created, perform a Save first to
    // prime the header and other information.
    if (!m_kPathAndFilename.Exists())
        return false;

    if (!NiFile::Access(m_kPathAndFilename, NiFile::READ_ONLY))
    {
        if (NiFile::Access(m_kPathAndFilename, NiFile::WRITE_ONLY))
        {
            if (!Save())
                return false;
        }
        else
        {
            return false;
        }
    }
    
    
    NiFile* pkFile = NiFile::GetFile(m_kPathAndFilename, 
        NiFile::APPEND_ONLY);

    if (pkFile && true == *(pkFile))
    {
        bool bSuccess = SaveEntry(*(pkFile), pkDesc, kName);
        NiDelete pkFile;
        return bSuccess;
    }

    NiDelete pkFile;

    return false;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::SaveString(NiBinaryStream& kStream, 
    const char* pcString)
{
    if (pcString == NULL)
    {
        unsigned int stLength = 0;
        NiStreamSaveBinary(kStream, stLength);
    }
    else
    {
        unsigned int stLength = (unsigned int)(strlen(pcString) + 1);
        NiStreamSaveBinary(kStream, stLength);
        NiStreamSaveBinary(kStream, pcString, stLength);
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiGPUProgramCache::LoadString(NiBinaryStream& kStream, 
    char* pcBuffer, size_t stBufferSize)
{
    if (stBufferSize == 0 || pcBuffer == NULL)
        return false;

    unsigned int stLength = 0;
    NiStreamLoadBinary(kStream, stLength);
    NIASSERT(stLength < stBufferSize);

    if (stLength == 0)
    {
        pcBuffer[0] = '\0';
        return true;
    }
    else
    {
        NiStreamLoadBinary(kStream, pcBuffer, stLength);
        pcBuffer[stLength + 1] = '\0';
        return true;
    }
}
//---------------------------------------------------------------------------
