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

#include "NiStream.h"
#include "NiMemStream.h"
#include "NiSystem.h"
#include "NiTexture.h"
#include "NiTexturePalette.h"
#include "NiNode.h"
#include <NiFile.h>
#include <NiFilename.h>
#include <NiRTLib.h>
#include <NiSearchPath.h>

//---------------------------------------------------------------------------

// Define a version of NiMemMarker that explicitly ignores the return value
// in order to avoid 'warning: statement has no effect' in shipping
#define NiMemMarkerNR(markerType, classifier, string) \
    (void)NiMemMarker(markerType, classifier, string)

//---------------------------------------------------------------------------
// Version Information
//
// Header for NIF.  First string in header has version number at the end of
// string.  The version number has format "d.d.d.d" where each '.' separated
// element is a number between 0 and 255.  The elements indicate: 
// * The first element is the major release version number.  
// * The second element is the minor release version number.
// * The third element is the patch release version number.
// * The fourth element is an Emergent internal development revision number.

#include "NiVersion.h"

const unsigned int NiStream::ms_uiNifMinVersion =
    NiStream::GetVersion(10, 1, 0, 114);
const unsigned int NiStream::ms_uiNifMaxVersion = 
    NiStream::GetVersion(NIF_MAJOR_VERSION, NIF_MINOR_VERSION, 
    NIF_PATCH_VERSION, NIF_INTERNAL_VERSION);
const unsigned int NiStream::ms_uiNifMinUserDefinedVersion =
    NiStream::GetVersion(0, 0, 0, 0);
const unsigned int NiStream::ms_uiNifMaxUserDefinedVersion = 
    NiStream::GetVersion(0, 0, 0, 0);
const unsigned int NiStream::NULL_LINKID = 0xffffffff;
NiCriticalSection NiStream::ms_kCleanupCriticalSection;
//---------------------------------------------------------------------------

// hash table for CreateObject functions
NiTStringPointerMap<NiStream::CreateFunction>* NiStream::ms_pkLoaders = 0;

NiStream::PostProcessFinalFunction NiStream::ms_pkPostProcessFinalFunction =
    NULL;

// Array for post-processing functions.
NiStream::PostProcessFunctionArray* NiStream::ms_pkPostProcessFunctions =
    NULL;

enum
{
    MAX_RTTI_LEN = 256,
    MAX_RTTI_ARG_COUNT = MAX_RTTI_LEN / 2
};


//---------------------------------------------------------------------------
void NiStream::_SDMInit()
{
    // init create function map
    ms_pkLoaders = NiNew NiTStringPointerMap<CreateFunction>(59, false);

    // Init post-processing function array.
    ms_pkPostProcessFunctions = NiNew PostProcessFunctionArray(0, 3);
    ms_pkPostProcessFinalFunction = NULL;
}
//---------------------------------------------------------------------------
void NiStream::_SDMShutdown()
{
    NiDelete ms_pkLoaders;
    NiDelete ms_pkPostProcessFunctions;
}
//---------------------------------------------------------------------------
bool NiStream::GetSaveAsLittleEndian()
{
    return m_bSaveLittleEndian;
}
//---------------------------------------------------------------------------
void NiStream::SetSaveAsLittleEndian(bool bLittle)
{
    m_bSaveLittleEndian = bLittle;
}
//---------------------------------------------------------------------------
bool NiStream::GetSourceIsLittleEndian()
{
    return m_bSourceIsLittleEndian;
}
//---------------------------------------------------------------------------
NiStream::NiStream()
    :
    m_kGroups(0), 
    m_kObjects(0, 1024), 
    m_kTopObjects(0), 
    m_kFixedStrings(0, 128),
    m_pkThread(0), 
    m_pkBGLoadProc(0),
    m_ePriority(NiThread::BELOW_NORMAL), 
    m_kAffinity(),
    m_bPrepareModifiers(true)
{
    ResetLastErrorInfo();
    
    // input and output streams
    m_pkIstr = 0;
    m_pkOstr = 0;

    // file version 
    // Initialize to maximum number for this rare case:
    // Code involving serialization requiring a newer version than
    // the client is currently using, is provided to client. That 
    // serialization code needs to be aware of the version the client is using.
    // It then saves to the older format, rather than just the latest format.
    m_uiNifFileVersion = ms_uiNifMaxVersion;
    m_uiNifFileUserDefinedVersion = ms_uiNifMaxUserDefinedVersion;

    // endianness export/import option (default to platform endianness)
    m_bSourceIsLittleEndian = m_bSaveLittleEndian = 
        NiSystemDesc::GetSystemDesc().IsLittleEndian();
    
    // filename storage
    m_acFileName[0] = '\0';

    // image sharing
    m_spTexturePalette = NiNew NiDefaultTexturePalette;

    // linking
    m_uiLinkIndex = 0;
    m_uiLinkBlockIndex = 0;

    m_pkSearchPath = NiNew NiSearchPath;

    m_eBackgroundLoadStatus = IDLE;

    // error messages
    ResetLastErrorInfo();
}
//---------------------------------------------------------------------------
NiStream::~NiStream()
{
    NiDelete m_pkIstr;
    NiDelete m_pkOstr;

    RemoveAllObjects();

    m_spTexturePalette = 0;

    NiDelete m_pkSearchPath;
    
    // If any background load was done, make sure it was properly shut down:
    NIASSERT(m_eBackgroundLoadStatus == IDLE);

    NiDelete m_pkBGLoadProc;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// top level object list
//---------------------------------------------------------------------------
void NiStream::InsertObject(NiObject* pkObject)
{
#ifdef NIDEBUG
    NIASSERT(pkObject);

    for (unsigned int i = 0; i < m_kTopObjects.GetSize(); i++)
    {
        NIASSERT(pkObject != m_kTopObjects.GetAt(i));
    }
#endif
    
    m_kTopObjects.Add(pkObject);
}
//---------------------------------------------------------------------------
void NiStream::RemoveObject(NiObject* pkObject)
{
    for (unsigned int i = 0; i < m_kTopObjects.GetSize(); i++)
    {
        if (pkObject == m_kTopObjects.GetAt(i))
        {
            m_kTopObjects.RemoveAt(i);
            return;
        }
    }
}
//---------------------------------------------------------------------------
void NiStream::RemoveAllObjects()
{
    ms_kCleanupCriticalSection.Lock();
    NIASSERT(m_kRegisterMap.GetCount() == 0);

    m_kConversionMap.RemoveAll();
    m_kTopObjects.RemoveAll();
    m_kFixedStrings.RemoveAll();
    m_kObjectSizes.RemoveAll();
    ms_kCleanupCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
bool NiStream::RegisterSaveObject(NiObject* pkObject)
{
    NIASSERT(pkObject);

    unsigned int uiLinkID;

    // return false if object is already registered.
    if (m_kRegisterMap.GetAt(pkObject, uiLinkID))
        return false;

    uiLinkID = m_kObjects.GetSize();
    m_kRegisterMap.SetAt(pkObject, uiLinkID);
    m_kObjects.Add(pkObject);
    return true;
}
//---------------------------------------------------------------------------
bool NiStream::RegisterFixedString(const NiFixedString& kString)
{
    // Determine if the string is already in the array.
    if (kString.Exists() && GetStringID(kString) == NULL_LINKID)
    {
        m_kFixedStrings.AddFirstEmpty(kString);
    }

    return true;
}   
//---------------------------------------------------------------------------
unsigned int NiStream::GetStringID(const NiFixedString& kString)
{
    if (kString.Exists())
    {
        for (unsigned int ui = 0; ui < m_kFixedStrings.GetSize(); ui++)
        {
            if (kString == m_kFixedStrings.GetAt(ui))
            {
                return ui;
            }
        }
    }

    return NULL_LINKID;
}
//---------------------------------------------------------------------------
void NiStream::ChangeObject(NiObject* pkNewObject, 
    bool bLink)
{
    //In 3.2 to 4.2.2 the first argument was m_kObjects.GetSize() -1.
    //This is not always the case in 5.0+ and has problems if a
    //background load has been cancelled on the stream.
    const StreamObjectArrayElement &kCurrent = m_kObjects.GetAt(m_uiLoad);
    NiObject* pkObjectToLink = bLink ? 
        pkNewObject : kCurrent.GetObjectToLink();
    m_kObjects.SetAt(m_uiLoad, 
        StreamObjectArrayElement(pkNewObject, 
        pkObjectToLink));
}
//---------------------------------------------------------------------------
void NiStream::ReadLinkID()
{
    unsigned int uiLinkID;
    NiStreamLoadBinary(*this, uiLinkID);
    m_kLinkIDs.Add(uiLinkID);
}
//---------------------------------------------------------------------------
NiObject* NiStream::ResolveLinkID()
{
    // As of NIF version 5.0.0.1, all objects are created before LoadBinary
    // is called for any object, so this function can be called by LoadBinary
    // to read a link id and immediately resolve it, rather than the process
    // of reading the link id with ReadLinkID in LoadBinary and then resolving
    // the link if by calling GetObjectFromLinkID in LinkObject
    
    NIASSERT(GetFileVersion() >= GetVersion(5, 0, 0, 1));

    unsigned int uiLinkID;
    NiStreamLoadBinary(*this, uiLinkID);

    if (uiLinkID == NULL_LINKID)
        return NULL;

    return m_kObjects.GetAt(uiLinkID);
}
//---------------------------------------------------------------------------
NiObject* NiStream::GetObjectFromLinkID()
{
    NIASSERT(m_uiLinkIndex < m_kLinkIDs.GetSize());

    unsigned int uiLinkID = m_kLinkIDs.GetAt(m_uiLinkIndex++);

    if (uiLinkID == NULL_LINKID)
        return NULL;

    NIASSERT(uiLinkID < m_kObjects.GetSize());
    return m_kObjects.GetAt(uiLinkID);
}
//---------------------------------------------------------------------------
unsigned int NiStream::ReadMultipleLinkIDs()
{
    unsigned int uiNum;
    NiStreamLoadBinary(*this, uiNum);

    if (GetFileVersion() >= GetVersion(20, 2, 0, 5)
        &&
        m_uiLoad < m_kObjectSizes.GetSize())
    {
        const unsigned int uiMaxLinkIDs = 
            m_kObjectSizes.GetAt(m_uiLoad) / sizeof(unsigned int);

        if (uiNum > uiMaxLinkIDs)
        {
            NILOG(NIMESSAGE_GENERAL_0, "NiStream Serialization error."
                "ReadMultipleLinkIDs() attempted to load too many IDs.\n"
                "Load code must be out of sync from Save code.\n");
            NIASSERT(!"NiStream Serialization error."
                "ReadMultipleLinkIDs() attempted to load too many IDs.\n"
                "Load code must be out of sync from Save code.\n");
        }
    }

    m_kLinkIDBlocks.Add(uiNum);
    for (unsigned int ui = 0; ui < uiNum; ui++)
    {
        ReadLinkID();
    }
    return uiNum;
}
//---------------------------------------------------------------------------
unsigned int NiStream::GetNumberOfLinkIDs()
{
    NIASSERT(m_uiLinkBlockIndex < m_kLinkIDBlocks.GetSize());

    return m_kLinkIDBlocks.GetAt(m_uiLinkBlockIndex++);
}
//---------------------------------------------------------------------------
void NiStream::SetNumberOfLinkIDs(unsigned int uiLinks)
{
    m_kLinkIDBlocks.Add(uiLinks);
}
//---------------------------------------------------------------------------
unsigned int NiStream::GetLinkIDFromObject(const NiObject* pkObject) const
{
    if (pkObject == NULL)
        return NULL_LINKID;

    // If pkObject has not been registered, NULL_LINKID will be returned. This
    // will happen if an object is attempting to stream a pointer to an object
    // it has not registered for streaming. This is a valid action when the
    // streaming object does not hold a reference to the object it is 
    // streaming the pointer to.
    unsigned int uiLinkID = NULL_LINKID;
    m_kRegisterMap.GetAt(pkObject, uiLinkID);

    return uiLinkID;
}
//---------------------------------------------------------------------------
// loading
//---------------------------------------------------------------------------
bool NiStream::LoadHeader()
{
    // read NIF file header
    const int iLineCount = 128;
    char acLine[iLineCount];

    m_pkIstr->GetLine(acLine, iLineCount);
    
    if (strstr(acLine, "File Format") == NULL)
    {
        m_uiLastError = NOT_NIF_FILE;
        NiStrcpy(m_acLastErrorMessage, NI_MAX_PATH, "Not a NIF file");
        return false;
    }

    // Load the header as little endian
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    m_pkIstr->SetEndianSwap(!bPlatformLittle);

    NiStreamLoadBinary(*this, m_uiNifFileVersion);

    if (m_uiNifFileVersion <  ms_uiNifMinVersion)
    {
        m_uiLastError = OLDER_VERSION;
        NiStrcpy(m_acLastErrorMessage, NI_MAX_PATH, 
            "NIF version is too old.");
        return false;
    }
    
    if (m_uiNifFileVersion > ms_uiNifMaxVersion)
    {
        m_uiLastError = LATER_VERSION;
        NiStrcpy(m_acLastErrorMessage, NI_MAX_PATH, "Unknown NIF version.");
        return false;
    }

    // Old files all little endian by default.
    m_bSourceIsLittleEndian = true;
    if (m_uiNifFileVersion >= GetVersion(20, 0, 0, 3))
    {
        NiStreamLoadBinary(*this, m_bSourceIsLittleEndian);
    }

    if (m_bSourceIsLittleEndian != bPlatformLittle)
    {
        if (NiBinaryStream::GetEndianMatchHint())
        {
            m_uiLastError = ENDIAN_MISMATCH;
            NiStrcpy(m_acLastErrorMessage, NI_MAX_PATH, 
                "Endian mismatch.");
            return false;
        }
        else
        {
            // Warn user about endian swapping
            NiSprintf(m_acLastErrorMessage, NI_MAX_PATH,
                "Warning: %s has to be endian swapped.\n",
                m_acFileName ? m_acFileName : "Stream");
            NiOutputDebugString(m_acLastErrorMessage);
        }
    }

    // Load the User Defined NIF Version
    if (m_uiNifFileVersion >= GetVersion(10, 0, 1, 8))
    {
        NiStreamLoadBinary(*this, m_uiNifFileUserDefinedVersion);
    }

    if (m_uiNifFileUserDefinedVersion < ms_uiNifMinUserDefinedVersion)
    {
        m_uiLastError = OLDER_VERSION;
        NiStrcpy(m_acLastErrorMessage, NI_MAX_PATH,
            "NIF user defined version is too old.");
        return false;
    }
    
    if (m_uiNifFileUserDefinedVersion > ms_uiNifMaxUserDefinedVersion)
    {
        m_uiLastError = LATER_VERSION;
        NiStrcpy(m_acLastErrorMessage, NI_MAX_PATH,
            "Unknown NIF user defined version.");
        return false;
    }


    unsigned int uiObjects;
    NiStreamLoadBinary(*this, uiObjects);
    m_kObjects.SetSize(uiObjects);

    // Set endianness for the rest of the file
    m_pkIstr->SetEndianSwap(m_bSourceIsLittleEndian != bPlatformLittle);

    return true;
}
//---------------------------------------------------------------------------
void NiStream::LoadTopLevelObjects()
{
    unsigned int uiTopObjects;

    NIASSERT(m_kTopObjects.GetSize() == 0);
    NiStreamLoadBinary(*this, uiTopObjects);
    m_kTopObjects.SetSize(uiTopObjects);
    for (unsigned int i = 0; i < uiTopObjects; i++)
    {
        unsigned int uiLinkID;
        NiStreamLoadBinary(*this, uiLinkID);
        NiObject* pkObject;
        if (uiLinkID == NULL_LINKID)
        {
            pkObject = NULL;
        }
        else
        {
            NIASSERT(uiLinkID < m_kObjects.GetSize());
            pkObject = m_kObjects.GetAt(uiLinkID);
        }
        m_kTopObjects.SetAt(i, pkObject);
    }
}
//---------------------------------------------------------------------------
void NiStream::RTTIError(const char* pcRTTI)
{
    // provide some hints to the user about the potential problems
    NiOutputDebugString(pcRTTI);
    NiOutputDebugString(": unable to find loader for class.\n"
        "* Make sure you are linking in the library "
        "that contains this class.\n"
        "* Make sure one of your source files "
        "includes the top level header file for that library\n\n");
    
    m_uiLastError = NO_CREATE_FUNCTION;
    NiStrcpy(m_acLastErrorMessage, NI_MAX_PATH, pcRTTI);
    NiStrcat(m_acLastErrorMessage, NI_MAX_PATH,
        ": cannot find create function.");
}
//---------------------------------------------------------------------------
NiObject* NiStream::CreateObjectByRTTI(const char* pcRTTI, const char** pcArgs,
    unsigned int uiArgCount)
{
    CreateFunction pfnCreate;
    bool bFound = ms_pkLoaders->GetAt(pcRTTI, pfnCreate);
    if (!bFound)
        return NULL;

    return pfnCreate(pcArgs, uiArgCount);
}
//---------------------------------------------------------------------------
// This class is meant to assist in parsing and handling RTTI objects
class LoadRTTIHelper : public NiMemObject
{
public:
    LoadRTTIHelper()
    {
        m_pfnFunction = NULL;
        m_pcArgs = NULL;
    }

    ~LoadRTTIHelper()
    {
        NiFree(m_pcArgs);
    }

    // This method will take the input buffer and loader map and 
    // finds the create function for that RTTI name. If any arguments
    // were specfied, this method will store them for later use.
    inline bool ParseRTTINameAndArgs(char* pcBuffer, 
        unsigned int uiMaxBufferSize, 
        NiTStringPointerMap<NiStream::CreateFunction>* pkLoaders)
    {
        NIASSERT(pkLoaders);
        NIASSERT(pcBuffer);

        const char* pcName = pcBuffer;
        m_pcArgs = NULL;
        m_uiNumArgs = 0;
        m_pfnFunction = NULL;

        for (unsigned int ui = 0; ui < uiMaxBufferSize; ui++)
        {
            if (pcBuffer[ui] == NiStream::ms_cRTTIDelimiter)
            {
                pcBuffer[ui] = '\0';
                if (m_pcArgs == NULL)
                {
                    m_pcArgs = &pcBuffer[ui + 1];
                }
                else
                {
                    m_uiNumArgs++;
                }

            }
            else if (pcBuffer[ui] == '\0')
            {
                if (m_pcArgs != NULL)
                {
                    ptrdiff_t kDiff = (&(pcBuffer[ui]) - m_pcArgs);
                    const char* pcSrcArgs = m_pcArgs;
                    m_pcArgs = NiAlloc(char, kDiff + 1);
                    NiMemcpy(m_pcArgs, kDiff + 1, pcSrcArgs, kDiff + 1);
                    m_uiNumArgs++;
                    NIASSERT(MAX_RTTI_ARG_COUNT > m_uiNumArgs);
                }
                break;
            }
        }

        return pkLoaders->GetAt(pcName, m_pfnFunction);
    }

    // Determine whether or not the instance has a valid create function
    inline bool IsValid()
    {
        return m_pfnFunction != NULL;
    }

    // Create the object using the previously found create function.
    // If arguments were specified, they will be passed along to the create
    // function.
    inline NiObject* CreateObject()
    {
        NIASSERT(m_pfnFunction);
        if (m_uiNumArgs == 0)
        {
            return m_pfnFunction(0, 0);
        }
        else
        {
            const char* ppcArgs[MAX_RTTI_ARG_COUNT];
            memset(ppcArgs, 0, sizeof(char*) * MAX_RTTI_ARG_COUNT);
            char* pcArgs = m_pcArgs;
            char* pcCurrentArg = m_pcArgs;
            unsigned int uiArg = 0;

            while (uiArg != m_uiNumArgs)
            {
                if (*(pcArgs) == '\0')
                {
                    ppcArgs[uiArg] = pcCurrentArg;
                    pcCurrentArg = pcArgs + 1;
                    uiArg++;
                }
                pcArgs++;
            }
            return m_pfnFunction(ppcArgs, m_uiNumArgs);
        }
    }

    NiStream::CreateFunction m_pfnFunction;
    char* m_pcArgs;
    unsigned int m_uiNumArgs;
};
//---------------------------------------------------------------------------
bool NiStream::LoadRTTI()
{
    unsigned short usRTTICount;
    NiStreamLoadBinary(*this, usRTTICount);

    LoadRTTIHelper* pkRTTICreate = NiNew LoadRTTIHelper[usRTTICount];
    NIASSERT(pkRTTICreate != NULL);

    unsigned int i;
    for (i = 0; i < usRTTICount; i++)
    {
        char aucRTTI[MAX_RTTI_LEN];
        LoadRTTIString(aucRTTI);

        // Determine if a create function exists for the RTTI name
        if (!pkRTTICreate[i].ParseRTTINameAndArgs(aucRTTI, MAX_RTTI_LEN,
            ms_pkLoaders))
        {
            RTTIError(aucRTTI);
        }
    }

    for (i = 0; i < m_kObjects.GetAllocatedSize(); i++)
    {
        unsigned short usRTTI;
        bool bSkippable = false;
     
        NiStreamLoadBinary(*this, usRTTI);
        if (GetFileVersion() >= NiStream::GetVersion(20, 2, 0, 5) &&
           (usRTTI & SKIPPABLE_MASK) != 0)
        {
            bSkippable = true;
            usRTTI &= ~SKIPPABLE_MASK;
        }

        NIASSERT(usRTTI < usRTTICount);
        bool bFound = pkRTTICreate[usRTTI].IsValid();
        if (!bFound && !bSkippable) // create not found and cannot be skipped
        {
            NiDelete [] pkRTTICreate;
            return false;
        }
        else if (!bFound) // create not found, but can be skipped, set to NULL
        {
            m_kObjects.SetAt(i, NULL);
        }
        else // Create found, so create an object
        {
            NiObject* pkObject = pkRTTICreate[usRTTI].CreateObject();
            m_kObjects.SetAt(i, pkObject);
        }
    }

    NiDelete [] pkRTTICreate;

    return true;
}
//---------------------------------------------------------------------------
bool NiStream::LoadObjectSizeTable()
{
    m_kObjectSizes.SetSize(m_kObjects.GetSize());
    for (unsigned int ui = 0; ui < m_kObjects.GetSize(); ui++)
    {
        unsigned int uiSizeInBytes = 0;
        NiStreamLoadBinary(*this, uiSizeInBytes);
        m_kObjectSizes.SetAt(ui, uiSizeInBytes);
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiStream::LoadFixedStringTable()
{
    unsigned int uiStringCount;
    NiStreamLoadBinary(*this, uiStringCount);
    m_kFixedStrings.SetSize(uiStringCount);

    unsigned int uiMaxStringSize;
    NiStreamLoadBinary(*this, uiMaxStringSize);
   
    char* pcString = NiAlloc(char, uiMaxStringSize + 1);

    for (unsigned short us = 0; us < uiStringCount; us++)
    {
        unsigned int uiLength = 0;
        NiStreamLoadBinary(*this, uiLength);
        NIASSERT(uiLength <= uiMaxStringSize);

        if (uiLength > 0)
            m_pkIstr->Read(pcString, uiLength);
        pcString[uiLength] = 0;

        m_kFixedStrings.SetAt(us, NiFixedString(pcString));
    }

    NiFree(pcString);
    return true;
}
//---------------------------------------------------------------------------
bool NiStream::LoadObject()
{
    CreateFunction pfnCreate;
    char aucRTTI[MAX_RTTI_LEN];
    LoadRTTIString(aucRTTI);
    
    bool bFound = ms_pkLoaders->GetAt(aucRTTI, pfnCreate);
    if (!bFound)
    {
        RTTIError(aucRTTI);
        return false;
    }
    
    NiObject* pkObject = pfnCreate(0,0);
    m_kObjects.Add(pkObject);
    pkObject->LoadBinary(*this);
    return true;
}
//---------------------------------------------------------------------------
void NiStream::LoadObjectGroups()
{
    // load number of groups
    unsigned int uiNumGroups;
    NiStreamLoadBinary(*this, uiNumGroups);

    ++uiNumGroups; // add extra for null group at index 0
    m_kGroups.SetSize(uiNumGroups);
    m_kGroups.SetAt(0, 0); 
    for (unsigned int uiGroup = 1; uiGroup < uiNumGroups; uiGroup++)
    {
        unsigned int uiSize;
        NiStreamLoadBinary(*this, uiSize);
        NiObjectGroup* pkGroup = NiNew NiObjectGroup(uiSize);
        m_kGroups.SetAt(uiGroup, pkGroup);
    }

}
//---------------------------------------------------------------------------
NiObjectGroup* NiStream::GetGroupFromID(unsigned int uiID) const
{
    return m_kGroups.GetAt(uiID);
}
//---------------------------------------------------------------------------
unsigned int NiStream::GetIDFromGroup(NiObjectGroup* pkGroup) const
{
    for (unsigned int uiGroup = 0; uiGroup < m_kGroups.GetSize(); uiGroup++)
    {
        if (pkGroup == m_kGroups.GetAt(uiGroup))
        {
            return uiGroup;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
bool NiStream::LoadStream()
{
    if (!LoadHeader())
        return false;

    // Initialize m_uiLoad, m_uiLink, and m_uiPostLink so that progress of
    // background loading thread can be estimated. This has to be postponed
    // until after the header has been read because 
    // m_kObjects.GetAllocatedSize is used in progress estimation.
    m_uiLoad = m_uiLink = m_uiPostLink = 0;

    //Removing all objects can release shared resources.  We must lock this
    //with the stream cleanup critical section.
    RemoveAllObjects();
    
    bool bNew = (GetFileVersion() >= GetVersion(5, 0, 0, 1));
    if (bNew)
    {
        if (!LoadRTTI())
            return false;
    }

    if (GetFileVersion() >= GetVersion(20, 2, 0, 5))
    {
        if (!LoadObjectSizeTable())
            return false;
    }

    if (GetFileVersion() >= GetVersion(20, 1, 0, 1))
    {
        if (!LoadFixedStringTable())
            return false;
    }

    // read object groups
    if (GetFileVersion() >= GetVersion(5, 0, 0, 6))
    {
        LoadObjectGroups();
    }

    // read list of objects
    unsigned int uiObjects = m_kObjects.GetAllocatedSize();

    for ( ; m_uiLoad < uiObjects; m_uiLoad++)
    {
        if (m_eBackgroundLoadStatus == CANCELLING)
        {
            FreeLoadData();
            return false;
        }
        else if (m_eBackgroundLoadStatus == PAUSING)
        {
            DoThreadPause();
        }

        if (bNew)
        {
            NiObject* pkObject = m_kObjects.GetAt(m_uiLoad);
            if (pkObject)
            {
            #if NIDEBUG
                unsigned int uiCurrentPos = m_pkIstr->GetPosition();
            #endif
                pkObject->LoadBinary(*this);
            #if NIDEBUG
                if (m_kObjectSizes.GetSize() != 0)
                {
                    unsigned int uiBytesRead = m_pkIstr->GetPosition() -
                        uiCurrentPos;
                    unsigned int uiStreamedSizeInBytes = 
                        m_kObjectSizes.GetAt(m_uiLoad);
                    NIASSERT(uiBytesRead == uiStreamedSizeInBytes);
                }
            #endif
            }
            else 
            {
            #if NIDEBUG
                unsigned int uiCurrentPosition = m_pkIstr->GetPosition();
            #endif
                NIASSERT(m_kObjectSizes.GetSize() != 0);
                unsigned int uiStreamedSizeInBytes = 
                    m_kObjectSizes.GetAt(m_uiLoad);
                if (uiStreamedSizeInBytes != 0)
                    m_pkIstr->Seek(uiStreamedSizeInBytes);
                NIASSERT(m_pkIstr->GetPosition() == uiCurrentPosition + 
                    uiStreamedSizeInBytes);
            }
        }
        else
        {
            if (!LoadObject())
            {
                FreeLoadData();
                return false;
            }
        }
    }

    LoadTopLevelObjects();

    // linking phase
    for ( ; m_uiLink < uiObjects; m_uiLink++)
    {
        if (m_eBackgroundLoadStatus == PAUSING)
        {
            DoThreadPause();
        }
        NiObject* pkObject = m_kObjects.GetAt(m_uiLink).GetObjectToLink();
        if (pkObject)
            pkObject->LinkObject(*this);
    }

    // post-link phase
    for ( ; m_uiPostLink < uiObjects; m_uiPostLink++)
    {
        if (m_eBackgroundLoadStatus == PAUSING)
        {
            DoThreadPause();
        }
        NiObject* pkObject = 
            (NiObject*)m_kObjects.GetAt(m_uiPostLink);
        if (pkObject)
            pkObject->PostLinkObject(*this);
    }

    if (m_eBackgroundLoadStatus == CANCELLING)
    {
        FreeLoadData();
        return false;
    }
    else if (m_eBackgroundLoadStatus == PAUSING)
    {
        DoThreadPause();
    }

    // Post-processing phase.
    if (ms_pkPostProcessFunctions->GetEffectiveSize() > 0)
    {
        for (unsigned int uj = 0; uj < ms_pkPostProcessFunctions->GetSize(); 
            uj++)
        {
            PostProcessFunction pfnFunc = ms_pkPostProcessFunctions->GetAt(uj);
            if (pfnFunc)
            {
                pfnFunc(*this, m_kTopObjects);
            }
        }

        if (ms_pkPostProcessFinalFunction)
        {
            ms_pkPostProcessFinalFunction(*this, m_kTopObjects);
        }
    }

    if (m_eBackgroundLoadStatus == CANCELLING)
    {
        FreeLoadData();
        return false;
    }
    else if (m_eBackgroundLoadStatus == PAUSING)
    {
        DoThreadPause();
    }

    SetSelectiveUpdateFlagsForOldVersions();

    FreeLoadData();

    return true;
}
//---------------------------------------------------------------------------
bool NiStream::Load(const char* pcFileName)
{
    // save filename & convert to Mac format if needed
    NIASSERT(pcFileName);

    NiStrcpy(m_acFileName, NI_MAX_PATH, pcFileName);
    NiPath::Standardize(m_acFileName);

    NiMemMarkerNR(NI_MEM_MARKER_BEGIN, __FUNCTION__, m_acFileName);

    m_pkSearchPath->SetReferencePath(m_acFileName);

    // create new input file stream
    NiFile* kIst = NiFile::GetFile(m_acFileName, NiFile::READ_ONLY);
    if (!kIst || !*kIst)
    {
        NiDelete kIst;
        m_uiLastError = FILE_NOT_LOADED;
        NiStrcpy(m_acLastErrorMessage, NI_MAX_PATH, "Cannot open file.");
        NiMemMarkerNR(NI_MEM_MARKER_END, __FUNCTION__, m_acFileName);
        return false;
    }

    bool bResult = Load(kIst);
    NiDelete kIst;

    NiMemMarkerNR(NI_MEM_MARKER_END, __FUNCTION__, m_acFileName);

    return bResult;
}
//---------------------------------------------------------------------------
bool NiStream::Load(char* pcBuffer, int iBufferSize)
{
    NiMemMarkerNR(NI_MEM_MARKER_BEGIN, __FUNCTION__, (const void*)pcBuffer);

    // new input character stream
    NiMemStream kIstr(pcBuffer, iBufferSize);

    bool bResult = Load(&kIstr);

    NiMemMarkerNR(NI_MEM_MARKER_END, __FUNCTION__, (const void*)pcBuffer);

    return bResult;
}
//---------------------------------------------------------------------------
bool NiStream::Load(NiBinaryStream* pkIstr)
{
    NIASSERT(pkIstr != NULL);

    if (!*pkIstr)
    {
        m_pkIstr = 0;
        return false;
    }

    NiMemMarkerNR(NI_MEM_MARKER_BEGIN, __FUNCTION__, pkIstr);

    m_pkIstr = pkIstr;

    bool bResult = LoadStream();

    m_pkIstr = 0;

    NiMemMarkerNR(NI_MEM_MARKER_END, __FUNCTION__, pkIstr);

    return bResult;
}
//---------------------------------------------------------------------------
#if defined(_WII)
bool NiStream::LoadRemote(const char* pcFileName)
{
    NIASSERT(pcFileName);

    // The Wii NiStream.Save function uses the NiWiIRemoteFile 
    // class to save data across the NDEV USB connection to the host 
    // PC.  Sometimes during testing we would like to reload data 
    // that we just saved in this way.  Since the standard Load 
    // method loads via the DVD drive, on the Wii we can use
    // this method.

    NiWiiRemoteFile* pkRemoteFile = 
        NiExternalNew NiWiiRemoteFile(
            pcFileName,
            NiWiiRemoteFile::OPEN,
            false);

    unsigned int uiFileSize = pkRemoteFile->GetFileSize();
    bool bResult = false;
    if (pkRemoteFile->IsOpen())
    {
        char* pcBuffer = (char*)NiMalloc(uiFileSize);
        if (pkRemoteFile->Read(pcBuffer, uiFileSize) &&
            Load(pcBuffer, uiFileSize))
        {
            bResult = true;
        }
        NiFree(pcBuffer);
    }
    NiExternalDelete pkRemoteFile;
    return bResult;
}
#endif
//---------------------------------------------------------------------------
// background loading
//---------------------------------------------------------------------------
void NiStream::BackgroundLoadBegin(const char *pcFileName)
{
    m_pkIstr = NULL;

    // Save the name. Don't even try to open the file at this point, since
    // that will cause a significant stall.
    NIASSERT(strlen(pcFileName) < NI_MAX_PATH);
    NiStrcpy(m_acFileName, NI_MAX_PATH, pcFileName);
    BackgroundLoadBegin();
}
//---------------------------------------------------------------------------
void NiStream::BackgroundLoadBegin(NiBinaryStream* pkIstr)
{
    m_pkIstr = pkIstr;
    BackgroundLoadBegin();
}
//---------------------------------------------------------------------------
void NiStream::BackgroundLoadEstimateProgress(LoadState& kLoadState)
{
    if (m_uiLoad == UINT_MAX)
    {
        // Background thread hasn't run far enough to even initialize
        // the variables that are used to estimate progress.
        kLoadState.m_fReadProgress = kLoadState.m_fLinkProgress = 0.0f;
    }
    else
    {
        unsigned int uiObjects = m_kObjects.GetAllocatedSize();
        kLoadState.m_fReadProgress = (float) m_uiLoad / uiObjects;
        kLoadState.m_fLinkProgress = (float) (m_uiLink + m_uiPostLink) /
            (2 * uiObjects);
    }
}
//---------------------------------------------------------------------------
void NiStream::BackgroundLoadOnExit()
{
    // The purpose of this virtual function is to serve as a callback that
    // can be overridden by derived classes to allow applications to run code
    // in the background loading thread without stealing cycles from the main 
    // thread and causing framerate hits. Typical chores that would be handled
    // by this function in derived classes include creating property and
    // effect states and precaching geometry for the loaded objects.

    // This function is called regardless of whether the background load
    // succeeded. Call BackgroundLoadGetExitStatus to determine whether
    // the background load succeeded (true -> success. false -> failure).
}
//---------------------------------------------------------------------------
int NiStream::RegisterLoader(const char* pcName, CreateFunction pfnFunc)
{    
#ifdef NIDEBUG
    bool bRegistered;
    CreateFunction pfnExistingFunc;
    bRegistered = ms_pkLoaders->GetAt(pcName, pfnExistingFunc);
    NIASSERT(!bRegistered || pfnExistingFunc == pfnFunc);
#endif

    ms_pkLoaders->SetAt(pcName, pfnFunc);
    return 0;
}
//---------------------------------------------------------------------------
void NiStream::UnregisterLoader(const char* pcClassName)
{
    ms_pkLoaders->RemoveAt(pcClassName);
}
//---------------------------------------------------------------------------
void NiStream::SetPostProcessFinalFunction(PostProcessFinalFunction pfnFunc)
{
    ms_pkPostProcessFinalFunction = pfnFunc;
}
//---------------------------------------------------------------------------
void NiStream::RegisterPostProcessFunction(PostProcessFunction pfnFunc)
{
#ifdef NIDEBUG
    // Assert that this function has not already been registered.
    for (unsigned int ui = 0; ui < ms_pkPostProcessFunctions->GetSize(); ui++)
    {
        PostProcessFunction pfnExistingFunc = ms_pkPostProcessFunctions
            ->GetAt(ui);
        NIASSERT(pfnExistingFunc != pfnFunc);
    }
#endif

    ms_pkPostProcessFunctions->AddFirstEmpty(pfnFunc);
}
//---------------------------------------------------------------------------
void NiStream::UnregisterPostProcessFunction(PostProcessFunction pfnFunc)
{
    for (unsigned int ui = 0; ui < ms_pkPostProcessFunctions->GetSize(); ui++)
    {
        PostProcessFunction pfnExistingFunc = ms_pkPostProcessFunctions
            ->GetAt(ui);
        if (pfnExistingFunc == pfnFunc)
        {
            ms_pkPostProcessFunctions->RemoveAt(ui);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void NiStream::SetSelectiveUpdateFlagsTTTFRecursive(NiAVObject* pkAVObject)
{
    pkAVObject->SetSelectiveUpdate(true);
    pkAVObject->SetSelectiveUpdateTransforms(true);
    pkAVObject->SetSelectiveUpdatePropertyControllers(true);
    pkAVObject->SetSelectiveUpdateRigid(false);

    NiNode* pkNode = NiDynamicCast(NiNode, pkAVObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                SetSelectiveUpdateFlagsTTTFRecursive(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void NiStream::SetSelectiveUpdateFlagsForOldVersions()
{
    // Set selective update flags
    if (GetFileVersion() < GetVersion(4, 1, 0, 12))
    {
        for (unsigned int i = 0; i < m_kObjects.GetSize(); i++)
        {

            NiObject* pkObject = m_kObjects.GetAt(i);

            if (NiIsKindOf(NiAVObject, pkObject))
            {
                NiAVObject* pkAVObject = (NiAVObject*) pkObject;

                if (pkAVObject->GetParent() == NULL)
                {
                    SetSelectiveUpdateFlagsTTTFRecursive(pkAVObject);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// saving
//---------------------------------------------------------------------------
void NiStream::SaveHeader()
{
    NIASSERT(m_pkOstr);

    // Header is always entirely in little endian.
    // If not little endian, then swap bytes for the header.
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    m_pkOstr->SetEndianSwap(!bPlatformLittle);

    m_pkOstr->PutS("Gamebryo File Format, Version "
        GAMEBRYO_NIF_VERSION_STRING"\n");
    NiStreamSaveBinary(*this, ms_uiNifMaxVersion);
    NiStreamSaveBinary(*this, m_bSaveLittleEndian);
    NiStreamSaveBinary(*this, ms_uiNifMaxUserDefinedVersion);
    NiStreamSaveBinary(*this, m_kObjects.GetSize());

    // Once header is written, swap the rest of the stream if requested.
    m_pkOstr->SetEndianSwap(bPlatformLittle ^ m_bSaveLittleEndian);
}
//---------------------------------------------------------------------------
void NiStream::SaveTopLevelObjects()
{
    unsigned int uiTopObjects = m_kTopObjects.GetSize();
    NiStreamSaveBinary(*this, uiTopObjects);

    for (unsigned int i = 0; i < uiTopObjects; i++)
    {
        SaveLinkID(m_kTopObjects.GetAt(i));
    }
}
//---------------------------------------------------------------------------
void NiStream::SaveRTTI()
{
    NiTStringPointerMap<unsigned short> kRTTIMap;

    char acRTTIName[MAX_RTTI_LEN];
    unsigned int i;
    for (i = 0; i < m_kObjects.GetSize(); i++)
    {
        unsigned short usRTTI;
        NiObject* pkObject = m_kObjects.GetAt(i);
        
        NIVERIFY(pkObject->GetStreamableRTTIName(acRTTIName, MAX_RTTI_LEN));

        if (!kRTTIMap.GetAt(acRTTIName, usRTTI))
        {
            NIASSERT(kRTTIMap.GetCount() < USHRT_MAX);
            kRTTIMap.SetAt(acRTTIName, (unsigned short)(kRTTIMap.GetCount()));
        }
    }

    unsigned short usRTTICount = (unsigned short) kRTTIMap.GetCount();
    const char** ppcRTTI = NiAlloc(const char*, usRTTICount);
    NIASSERT(ppcRTTI != NULL);

    NiTMapIterator pos = kRTTIMap.GetFirstPos();

    while (pos)
    {
        const char* pcRTTI;
        unsigned short usRTTI;
        kRTTIMap.GetNext(pos, pcRTTI, usRTTI);
        ppcRTTI[usRTTI] = pcRTTI;
    }

    NiStreamSaveBinary(*this, usRTTICount);
    for (i = 0; i < usRTTICount; i++)
    {
        SaveCString(ppcRTTI[i]);
    }

    NiFree(ppcRTTI);

    for (i = 0; i < m_kObjects.GetSize(); i++)
    {
        NiObject* pkObject = m_kObjects.GetAt(i);
        unsigned short usRTTI;
        NIVERIFY(pkObject->GetStreamableRTTIName(acRTTIName, MAX_RTTI_LEN));
        NIVERIFY(kRTTIMap.GetAt(acRTTIName, usRTTI));
        NIASSERT(usRTTI < usRTTICount);
        bool bSkippable = pkObject->StreamCanSkip();
        if (bSkippable)
            usRTTI |= SKIPPABLE_MASK; 
        NiStreamSaveBinary(*this, usRTTI);
    }
}
//---------------------------------------------------------------------------
unsigned int NiStream::PreSaveObjectSizeTable()
{
    unsigned int uiOffset = m_pkOstr->GetPosition();

    unsigned int uiStandInValue = 0;
    for (unsigned int ui = 0; ui < m_kObjects.GetSize(); ui++)
    {
        NiStreamSaveBinary(*this, uiStandInValue);
    }

    return uiOffset;
}
//---------------------------------------------------------------------------
bool NiStream::SaveObjectSizeTable(unsigned int uiStartOffset)
{
    NIASSERT(m_kObjects.GetSize() == m_kObjectSizes.GetSize());
    int iOffsetFromCurrentPosition = (int)m_pkOstr->GetPosition() -
        (int) uiStartOffset;
    m_pkOstr->Seek(-iOffsetFromCurrentPosition);
    NIASSERT(m_pkOstr->GetPosition() == uiStartOffset);
    for (unsigned int ui = 0; ui < m_kObjectSizes.GetSize(); ui++)
    {
        unsigned int uiObjectSize = m_kObjectSizes.GetAt(ui);
        NiStreamSaveBinary(*this, uiObjectSize);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiStream::SaveFixedStringTable()
{
    NiStreamSaveBinary(*this, (unsigned int)m_kFixedStrings.GetSize());

    size_t stMaxLength = 0;
    for (unsigned short us = 0; us < m_kFixedStrings.GetSize(); us++)
    {
        const NiFixedString& kString = m_kFixedStrings.GetAt(us);
        size_t stLength = kString.GetLength();
        if (stLength > stMaxLength)
            stMaxLength = stLength;
    }

    NIASSERT(stMaxLength < UINT_MAX);
    unsigned int uiMaxLength = (unsigned int) stMaxLength;
    NiStreamSaveBinary(*this, uiMaxLength);


    for (unsigned short us = 0; us < m_kFixedStrings.GetSize(); us++)
    {
        const NiFixedString& kString = m_kFixedStrings.GetAt(us);
        size_t stLength = kString.GetLength();
        NIASSERT(stLength < (size_t)UINT_MAX);
        unsigned int uiLength = (unsigned int) stLength;
        const char* pcString = (const char*)kString;

        NiStreamSaveBinary(*this, uiLength);
        if (uiLength > 0)
            m_pkOstr->Write(pcString, uiLength);
    }
}
//---------------------------------------------------------------------------
void NiStream::UpdateObjectGroups()
{
    // make array of object groups, and sum buffer size requirements for
    // each object in the group

    // first, create object group array, init buffer sizes to zero,
    m_kGroups.SetSize(1);
    m_kGroups.SetAt(0, 0);  // first entry is null group
    unsigned int i;
    for (i = 0; i < m_kObjects.GetSize(); i++)
    {
        NiObject* pkObject = m_kObjects.GetAt(i);
        if (pkObject == NULL)
            continue;

        NiObjectGroup* pkGroup = pkObject->GetGroup();
        if (pkGroup && !GetIDFromGroup(pkGroup))
        {
            m_kGroups.Add(pkGroup);
            pkGroup->SetSize(0);
        }
    }

    // sum up allocation requirements for each object belonging to a group
    for (i = 0; i < m_kObjects.GetSize(); i++)
    {
        NiObject* pkObject = m_kObjects.GetAt(i);
        if (pkObject == NULL)
            continue;

        NiObjectGroup* pkGroup = pkObject->GetGroup();
        if (pkGroup)
        {
            pkGroup->SetSize(pkObject->GetBlockAllocationSize() +
                pkGroup->GetSize());
        }
    }
}
//---------------------------------------------------------------------------
void NiStream::SaveObjectGroups()
{
    // save number of groups
    NiStreamSaveBinary(*this, m_kGroups.GetSize() - 1);

    // for each group
    for (unsigned int i = 1; i < m_kGroups.GetSize(); i++)
    {
        // save size
        NiStreamSaveBinary(*this, m_kGroups.GetAt(i)->GetSize());
    }
}
//---------------------------------------------------------------------------
void NiStream::RegisterObjects()
{
    for (unsigned int i = 0; i < m_kTopObjects.GetSize(); i++)
    {
        NiObject* pkObject = m_kTopObjects.GetAt(i);
        NIASSERT(pkObject);
        pkObject->RegisterStreamables(*this);
    }
}
//---------------------------------------------------------------------------
bool NiStream::SaveStream()
{
    NIASSERT(m_kObjects.GetSize() == 0);
    NIASSERT(m_kRegisterMap.GetCount() == 0);

    RegisterObjects();
    m_kObjectSizes.SetSize(m_kObjects.GetSize());

    SaveHeader();
    SaveRTTI();
    unsigned int uiStartingOffset = PreSaveObjectSizeTable();
    SaveFixedStringTable();

    {
        // recompute object groups
        UpdateObjectGroups();

        // save object groups
        SaveObjectGroups();
    }

    // save list of objects
    for (unsigned int i = 0; i < m_kObjects.GetSize(); i++)
    {
        NiObject* pkObject = m_kObjects.GetAt(i);
        unsigned int uiStartInBytes = m_pkOstr->GetPosition();
        pkObject->SaveBinary(*this);
        unsigned int uiSizeInBytes = m_pkOstr->GetPosition() - uiStartInBytes;
        NIASSERT(uiStartInBytes != 0);
        m_kObjectSizes.SetAt(i, uiSizeInBytes);
    }

    // Saving linkids of top level objects at end of file makes it easier to
    // load them. By the time the linkids are read in at load time, all
    // objects will have been created, so the linkids can be resolved
    // immediately and don't to be stored.

    SaveTopLevelObjects();
    SaveObjectSizeTable(uiStartingOffset);

    m_kObjects.RemoveAll();
    m_kRegisterMap.RemoveAll();

    return true;
}
//---------------------------------------------------------------------------
bool NiStream::Save(const char* pcFileName)
{
    NIASSERT(pcFileName != NULL);

    NiStrcpy(m_acFileName, NI_MAX_PATH, pcFileName);
    NiPath::Standardize(m_acFileName);
    m_pkSearchPath->SetReferencePath(m_acFileName);
    
    NiMemMarkerNR(NI_MEM_MARKER_BEGIN, __FUNCTION__, m_acFileName);

    // create new output file stream
    bool bResult = false;

#if defined(_WII)
    // The Wii can only use the NiFile interface to load, not save
    // (no harddrive).  For saving data, the NiWiiRemoteFile
    // class must be used.
    NiWiiRemoteFile* pkRemoteFile = NiExternalNew NiWiiRemoteFile(
        m_acFileName, 
        NiWiiRemoteFile::CREATE,
        false);

    if (pkRemoteFile->IsOpen())
    {
        // First we'll save the stream to a memory stream, then we'll write
        // it to disk using the NiWiiRemoteFile.
        NiMemStream kMemStream;
        bResult = Save(&kMemStream);

        if (bResult)
        {
            bResult = pkRemoteFile->Write(
                kMemStream.Str(), 
                kMemStream.GetSize());
            kMemStream.Freeze(false); // Allow the Str() to be freed.
        }
    }
    NiExternalDelete pkRemoteFile;
#else
    NiFile* pkOstr = NiFile::GetFile(m_acFileName, NiFile::WRITE_ONLY);
    
    if ((!pkOstr) || (!(*pkOstr)))
    {
        NiDelete pkOstr;
        NiMemMarkerNR(NI_MEM_MARKER_END, __FUNCTION__, m_acFileName);
        return false;
    }    

    bResult = Save(pkOstr);
    NiDelete pkOstr;
#endif


    NiMemMarkerNR(NI_MEM_MARKER_END, __FUNCTION__, m_acFileName);
    return bResult;
}
//---------------------------------------------------------------------------
bool NiStream::Save(char*& pcBuffer, int& iBufferSize)
{
    NiMemMarkerNR(NI_MEM_MARKER_BEGIN, __FUNCTION__, (const void*)pcBuffer);

    // create new output character stream
    NiMemStream kMemStream;

    bool bResult = Save(&kMemStream);

    iBufferSize = kMemStream.GetSize();
    pcBuffer = (char *) kMemStream.Str();

    NiMemMarkerNR(NI_MEM_MARKER_END, __FUNCTION__, (const void*)pcBuffer);

    return bResult;
}
//---------------------------------------------------------------------------
bool NiStream::Save(NiBinaryStream* pkOstr)
{
    NIASSERT(pkOstr != NULL);

    NiMemMarkerNR(NI_MEM_MARKER_BEGIN, __FUNCTION__, pkOstr);

    m_pkOstr = pkOstr;

    bool bResult = SaveStream();

    m_pkOstr = 0;

    NiMemMarkerNR(NI_MEM_MARKER_END, __FUNCTION__,  pkOstr);

    return bResult;
}
//---------------------------------------------------------------------------
void NiStream::FreeLoadData()
{
    ms_kCleanupCriticalSection.Lock();
#ifdef NIDEBUG
    // m_kObjects has a smart pointer reference to each of its objects.
    // There should be at least one more smart pointer reference to each 
    // object. If there isn't, then the object shouldn't be in the NIF file.

    NiTStringPointerMap<unsigned int> kTypeToCount;

    char acRTTIName[MAX_RTTI_LEN];

    unsigned int i;
    for (i = 0; i < m_kObjects.GetSize(); i++)
    {
        const StreamObjectArrayElement &kObject = m_kObjects.GetAt(i);
        NiObject *pkObject = kObject;
        NiObject *pkObjectToLink = kObject.GetObjectToLink();
        unsigned uiReferencesHeldByNiStream = pkObjectToLink == pkObject?
            2 : 1;

        if (pkObject && pkObject->GetRefCount() == uiReferencesHeldByNiStream)
        {
            if (NiSourceTexture::GetDestroyAppDataFlag() &&
                NiSourceTexture::GetUsePreloading() &&
                NiIsKindOf(NiPixelData, pkObject))
            {
                // NiPixelData may have been released intentionally
                // when the NiSourceTexture was precached
                continue;
            }
            unsigned int uiCount;
            NIVERIFY(pkObject->GetStreamableRTTIName(acRTTIName, 
                MAX_RTTI_LEN));

            if (kTypeToCount.GetAt(acRTTIName, uiCount))
            {
                kTypeToCount.SetAt(acRTTIName, uiCount + 1);
            }
            else
            {
                kTypeToCount.SetAt(acRTTIName, 1);
            }
        }
    }
    char acMsg[2 * NI_MAX_PATH];
    NiTMapIterator pos = kTypeToCount.GetFirstPos();
    while (pos)
    {
        const char* pcRTTIName;
        unsigned int uiCount;
        kTypeToCount.GetNext(pos, pcRTTIName, uiCount);

        NiSprintf(acMsg, 2 * NI_MAX_PATH, 
            "Warning: %s contains %d unreferenced %s "
            "objects. These objects will be NiDeleted. Re-exporting the NIF "
            "file will usually eliminate this warning.\n", m_acFileName,
            uiCount, pcRTTIName);
        NiOutputDebugString(acMsg);
    }
#endif //NIDEBUG

    m_kObjects.RemoveAll();
    m_kLinkIDs.RemoveAll();
    m_kLinkIDBlocks.RemoveAll();
    m_uiLinkIndex = 0;
    m_uiLinkBlockIndex = 0;
    ms_kCleanupCriticalSection.Unlock();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// string handlers
//---------------------------------------------------------------------------
void NiStream::LoadCString(char*& pcString)
{
    NIASSERT(m_pkIstr && pcString == 0);

    int iLength;
    NiStreamLoadBinary(*this, iLength);
    if (iLength > 0)
    {
        pcString = NiAlloc(char, iLength + 1);
        NIASSERT(pcString);
        m_pkIstr->Read(pcString, iLength);
        pcString[iLength] = 0;
    }
    else
    {
        pcString = 0;
    }
}
//---------------------------------------------------------------------------
void NiStream::LoadRTTIString(char* pcString)
{
    // pcString should have space for MAX_RTTI_LEN characters.

    NIASSERT(m_pkIstr);

    unsigned int uiLength;
    NiStreamLoadBinary(*this, uiLength);
    NIASSERT(uiLength > 0 && uiLength < MAX_RTTI_LEN);
    m_pkIstr->Read(pcString, uiLength);
    pcString[uiLength] = 0;
}
//---------------------------------------------------------------------------
void NiStream::SaveCString(const char* pcString)
{
    NIASSERT(m_pkOstr);

    size_t stLength = (pcString ? strlen(pcString) : 0);
    NiStreamSaveBinary(*this, stLength);
    if (stLength > 0)
    {
        m_pkOstr->Write(pcString, (unsigned int) stLength);
    }
}
//---------------------------------------------------------------------------
void NiStream::LoadFixedString(NiFixedString& kString)
{
    unsigned int uiStringID;
    NiStreamLoadBinary(*this, uiStringID);

    if (NULL_LINKID == uiStringID)
    {
        kString = NULL;
    }
    else
    {
        NIASSERT(uiStringID < m_kFixedStrings.GetSize());
        kString = m_kFixedStrings.GetAt(uiStringID);
    }
}
//---------------------------------------------------------------------------
void NiStream::LoadCStringAsFixedString(NiFixedString& kString)
{
    NIASSERT(m_pkIstr);

    int iLength;
    NiStreamLoadBinary(*this, iLength);
    if (iLength >= 1024)
    {
        char* pcString = 0;
        pcString = NiAlloc(char, iLength + 1);
        NIASSERT(pcString);
        m_pkIstr->Read(pcString, iLength);
        pcString[iLength] = 0;
        kString = pcString;
        NiFree(pcString);
    }
    else if (iLength > 0)
    {
        char acString[1024];
        m_pkIstr->Read(acString, iLength);
        acString[iLength] = 0;
        kString = acString;
    }
    else
    {
        kString = 0;
    }
}
//---------------------------------------------------------------------------
void NiStream::SaveFixedString(const NiFixedString& kString)
{
    if (!kString.Exists())
    {
        NiStreamSaveBinary(*this, NULL_LINKID);
    }
    else
    {
        unsigned int uiStringID = GetStringID(kString);
        
        // If you hit this assert, you are trying to save an NiFixedString
        // without calling RegisterFixedString on it in the 
        // RegisterStreamables phase of saving the file.
        //
        // Please add the following line to the RegisterStreamables function 
        // the calling object:
        //      kStream.RegisterFixedString(kString);

        NIASSERT(uiStringID != NULL_LINKID);
        
        NiStreamSaveBinary(*this, uiStringID);
    }
}
//---------------------------------------------------------------------------
unsigned int NiStream::GetVersionFromString(const char* pcVersionString)
{
    // The version number has format "d.d.d.d" where each '.' 
    // separated element is a number between 0 and 255.  The elements 
    // indicate: 
    // * The first element is the major release version number.  
    // * The second element is the minor release version number.
    // * The third element is the patch release version number.
    // * The fourth element is an Emergent internal development revision 
    //   number.
    //

    enum
    {
        MAX_VERSION_LEN = 15 // "255.255.255.255"
    };

    char acVS[MAX_VERSION_LEN + 1];
    NiStrcpy(acVS, MAX_VERSION_LEN + 1, pcVersionString);

    int iShift = 24;
    unsigned int uiVersion = 0;
    char* pcContext;
    char* pcD = NiStrtok(acVS, ".", &pcContext);
    NIASSERT(pcD);
    while (pcD) 
    {
        int iD = atoi(pcD);
        NIASSERT((iD >= 0) && (iD <= 255));
        NIASSERT(iShift >= 0);
        uiVersion |= (iD << iShift);
        iShift -= 8;
        pcD = NiStrtok(NULL, ".", &pcContext);
    }

    return uiVersion;
}

//---------------------------------------------------------------------------
// image palette access
//---------------------------------------------------------------------------
void NiStream::SetTexturePalette(NiTexturePalette* pkTexturePalette)
{
    //since setting the palette can dec some pointers, we crit sec it.
    ms_kCleanupCriticalSection.Lock();
    m_spTexturePalette = pkTexturePalette;
    ms_kCleanupCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
NiTexturePalette* NiStream::GetTexturePalette() const
{
    return m_spTexturePalette;
}
//---------------------------------------------------------------------------
void NiStream::SetTexture(const char* pcURL, NiTexture* pkTexture)
{
    m_spTexturePalette->SetTexture(pcURL, pkTexture);
}
//---------------------------------------------------------------------------
NiTexture* NiStream::GetTexture(const char* pcURL,
    NiTexture* pkSelf = NULL) const
{
    if (m_spTexturePalette)
        return m_spTexturePalette->GetTexture(pcURL, pkSelf);
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiStream::SetSearchPath(NiSearchPath* pkSearchPath)
{ 
    if (!pkSearchPath)
        return;

    NiDelete m_pkSearchPath;

    m_pkSearchPath = pkSearchPath;
}
//---------------------------------------------------------------------------
// Background Loading Methods
//---------------------------------------------------------------------------
unsigned int NiStream::BackgroundLoadProcedure::ThreadProcedure(
    NiThread*)
{
    m_pkStream->BackgroundLoad();
    return 0;
}
//---------------------------------------------------------------------------
void NiStream::BackgroundLoad()
{
    m_bBackgroundLoadExitStatus = (m_pkIstr == NULL) ?
        Load(m_acFileName) : LoadStream();

    // If a pause or cancel has been requested, we should do so at this point
    // since BackgroundLoadOnExit may do a non-trivial amount of work in 
    // a derived class.
    if (m_eBackgroundLoadStatus == CANCELLING)
    {
        // Cancel out and don't run BackgroundLoadOnExit
        m_bBackgroundLoadExitStatus = false;
        m_kSemaphore.Signal();
        return;
    }
    else if (m_eBackgroundLoadStatus == PAUSING)
    {
        DoThreadPause();
    }

    BackgroundLoadOnExit();
    m_kSemaphore.Signal();
}
//---------------------------------------------------------------------------
void NiStream::BackgroundLoadBegin()
{
    NIASSERT(m_eBackgroundLoadStatus == IDLE);

    // Set m_uiLoad to UINT_MAX so we can determine in BackgroundLoadPoll
    // whether the background thread has run far enough to set the variables
    // we use to compute progress.
    m_uiLoad = UINT_MAX;

    if (m_pkBGLoadProc == 0)
    {
        m_pkBGLoadProc = NiNew BackgroundLoadProcedure(this);
    }

    NIASSERT(m_pkThread == 0);
    m_pkThread = NiThread::Create(m_pkBGLoadProc);
    NIASSERT(m_pkThread);

    m_pkThread->SetThreadAffinity(m_kAffinity);

    m_pkThread->SetPriority(m_ePriority);
    m_pkThread->Resume();

    m_eBackgroundLoadStatus = LOADING;
}
//---------------------------------------------------------------------------
NiStream::ThreadStatus NiStream::BackgroundLoadPoll(LoadState* pkLoadState)
{
    if (m_eBackgroundLoadStatus == LOADING ||
        m_eBackgroundLoadStatus == CANCELLING ||
        m_eBackgroundLoadStatus == PAUSING)
    {
        int iPoll = m_kSemaphore.GetCount();
 
        // If the semaphore has been signalled, the thread is done processing.
        // As such, CANCELLING or PAUSING are irrelevant. The appropriate
        // values have been set to m_bBackgroundLoadExitStatus. Cleanup should
        // occur to set the status to IDLE.
        if (iPoll > 0)
        {
            m_kSemaphore.Wait();
            BackgroundLoadCleanup();
        }
    }

    if (pkLoadState != NULL && (m_eBackgroundLoadStatus == LOADING ||
        m_eBackgroundLoadStatus == PAUSED))
    {
        BackgroundLoadEstimateProgress(*pkLoadState);
    }

    return m_eBackgroundLoadStatus;
}
//---------------------------------------------------------------------------
bool NiStream::BackgroundLoadFinish()
{
    if (m_eBackgroundLoadStatus == PAUSED)
    {
        BackgroundLoadResume();
    }

    if (m_eBackgroundLoadStatus == LOADING ||
        m_eBackgroundLoadStatus == CANCELLING)
    {
        m_kSemaphore.Wait();
        BackgroundLoadCleanup();
    }

    return m_bBackgroundLoadExitStatus;
}
//---------------------------------------------------------------------------
void NiStream::BackgroundLoadCancel()
{
    if (m_eBackgroundLoadStatus == PAUSED)
    {
        BackgroundLoadResume();
    }

    if (m_eBackgroundLoadStatus == LOADING)
    {
        m_eBackgroundLoadStatus = CANCELLING;
    }
}

//---------------------------------------------------------------------------
void NiStream::BackgroundLoadPause()
{
    if (m_eBackgroundLoadStatus == LOADING)
    {
        m_eBackgroundLoadStatus = PAUSING;
    }
}
//---------------------------------------------------------------------------
void NiStream::BackgroundLoadResume()
{
    if (m_eBackgroundLoadStatus == PAUSED)
    {
        m_pkThread->Resume();
        m_eBackgroundLoadStatus = LOADING;
    }
}
//---------------------------------------------------------------------------
void NiStream::BackgroundLoadCleanup()
{
    NiDelete m_pkThread;
    m_pkThread = 0;
    m_eBackgroundLoadStatus = IDLE;
}
//---------------------------------------------------------------------------
void NiStream::DoThreadPause()
{
    m_eBackgroundLoadStatus = PAUSED;
    m_pkThread->Suspend();
}
//---------------------------------------------------------------------------
NiStream::StreamObjectArrayElement::StreamObjectArrayElement(
    NiObject* pkObject)
    :
    m_spObject(pkObject), 
    m_spObjectToLink(pkObject)
{
}
//---------------------------------------------------------------------------
NiStream::StreamObjectArrayElement::StreamObjectArrayElement(
    NiObject *pkObject, 
    NiObject *pkObjectToLink)
    : 
    m_spObject(pkObject), 
    m_spObjectToLink(pkObjectToLink)
{
}
//---------------------------------------------------------------------------
NiStream::StreamObjectArrayElement::~StreamObjectArrayElement()
{
    m_spObject = NULL;
    m_spObjectToLink = NULL;
}
//---------------------------------------------------------------------------
NiStream::StreamObjectArrayElement::StreamObjectArrayElement(
    const StreamObjectArrayElement& kOther) 
{
    *this = kOther; 
}
//---------------------------------------------------------------------------
NiStream::StreamObjectArrayElement& 
    NiStream::StreamObjectArrayElement::operator= ( 
    const StreamObjectArrayElement& kOther)
{
    m_spObject = kOther.m_spObject;
    m_spObjectToLink = kOther.m_spObjectToLink;
    return *this;
}
//---------------------------------------------------------------------------
NiStream::StreamObjectArrayElement::operator NiObject*() const
{
    return m_spObject;
}
//---------------------------------------------------------------------------
NiObject* NiStream::StreamObjectArrayElement::operator->() const
{
    return m_spObject;
}
//---------------------------------------------------------------------------
NiObject* NiStream::StreamObjectArrayElement::GetObjectToLink() const
{
    return m_spObjectToLink;
}
//---------------------------------------------------------------------------
