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
#include "NiSystemPCH.h"

#include "NiLog.h"

//---------------------------------------------------------------------------
NiLogger::NiLogFile::NiLogFile()
{
     m_acName[0] = '\0';
}
//---------------------------------------------------------------------------
NiLogger::NiLogFile::~NiLogFile()
{
    CloseFile();
}
//---------------------------------------------------------------------------
void NiLogger::NiLogFile::CloseFile()
{
    if (m_pkFile)
    {
        fclose(m_pkFile);
    }
    m_pkFile = 0;
}
//---------------------------------------------------------------------------
bool NiLogger::NiLogFile::OpenFile(OpenMode eOpenMode, bool bCommitToDisk)
{
    char acOpenMode[5];

    switch (eOpenMode)
    {
    case OPEN_APPEND:
        NiStrcpy(acOpenMode, 5, "a+t");
        break;
    case OPEN_OVERWRITE:
        NiStrcpy(acOpenMode, 5, "wt");
        break;
    default:
        NIASSERT(!"NiLogger::OPEN_UNIQUENAME Not yet supported");
        m_pkFile = 0;
        return false;
    }

    if (bCommitToDisk)
        NiStrcat(acOpenMode, 5, "c");

#if _MSC_VER >= 1400
    if (fopen_s(&m_pkFile, m_acName, acOpenMode) != 0 || !m_pkFile)
        return false;
#else //#if _MSC_VER >= 1400
    m_pkFile = fopen(m_acName, acOpenMode);

    if (!m_pkFile)
        return false;
#endif //#if _MSC_VER >= 1400

   

    return true;
}
//---------------------------------------------------------------------------
void NiLogger::NiLogFile::Init(const char* pcName, OpenMode eOpenMode, 
    bool bFlushOnWrite, bool bCommitToDisk)
{
    NIASSERT(pcName);

    NiStrcpy(m_acName, 256, pcName);
    m_bFlushOnWrite = bFlushOnWrite;

    if (!OpenFile(eOpenMode, bCommitToDisk))
    {
        NiOutputDebugString(
            "NiLogger::NiLogFile::Init Error "
            "- Failed to open the following file:");
                NiOutputDebugString(pcName);
        m_acName[0] = '\0';
        m_pkFile = 0;        
    }
}
//---------------------------------------------------------------------------
void NiLogger::NiLogFile::Free()
{
    CloseFile();
    m_acName[0] = '\0';
}
//---------------------------------------------------------------------------
bool NiLogger::NiLogFile::IsActive()
{
    return m_acName[0] != '\0';
}
//---------------------------------------------------------------------------
void NiLogger::NiLogFile::Log(char* pcMessage)
{
    if (!m_pkFile)
        return;

    fwrite(pcMessage, sizeof(char), strlen(pcMessage), m_pkFile);
    if (m_bFlushOnWrite)
        fflush(m_pkFile);
}
//---------------------------------------------------------------------------
void NiLogger::FlushAllLogs()
{
    for (unsigned int uiLogID = 0; uiLogID < NiLogger::MAX_NUM_LOGFILES; 
        uiLogID++)
    {
        if (NiLogger::ms_akLogFile[uiLogID].IsActive())
            fflush(NiLogger::ms_akLogFile[uiLogID].m_pkFile);
    }
}
//---------------------------------------------------------------------------
