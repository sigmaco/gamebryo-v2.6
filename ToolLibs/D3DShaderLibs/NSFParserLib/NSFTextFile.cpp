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

#include "NSFTextFile.h"

#include <NiSystem.h>

//---------------------------------------------------------------------------
NSFTextFile::NSFTextFile() :
    m_pcFilename(0), 
    m_pcData(0), 
    m_uiSize(0), 
    m_uiPos(0)
{
}
//---------------------------------------------------------------------------
NSFTextFile::~NSFTextFile()
{
    NiFree(m_pcData);
    NiFree(m_pcFilename);
}
//---------------------------------------------------------------------------
int NSFTextFile::Load(const char* pszFilename)
{
    FILE* pf = NULL;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (fopen_s(&pf, pszFilename, "rb") != 0 || pf == 0)
        return 1;
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    pf = fopen(pszFilename, "rb");
    if (!pf) 
        return 1;
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    size_t stLen = strlen(pszFilename) + 1;
    m_pcFilename = NiAlloc(char, stLen);
    NIASSERT(m_pcFilename);
    NiStrcpy(m_pcFilename, stLen, pszFilename);

    fseek(pf, 0, SEEK_END);
    m_uiSize = ftell(pf);
    m_uiPos = 0;
    fseek(pf, 0, SEEK_SET);
    
    m_pcData = NiAlloc(char, m_uiSize + 1);
    fread(m_pcData, 1, m_uiSize, pf);
    m_pcData[m_uiSize] = 0;

    fclose(pf);
    
    return 0;
}
//---------------------------------------------------------------------------
