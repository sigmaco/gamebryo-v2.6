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

#include "NSFParsedShader.h"

//---------------------------------------------------------------------------
NSFParsedShader::NSFParsedShader() :
    m_spNSBShader(0)
{
}
//---------------------------------------------------------------------------
NSFParsedShader::~NSFParsedShader()
{
    m_spNSBShader = 0;
}
//---------------------------------------------------------------------------
NSBShader* NSFParsedShader::GetShader()
{
    if (m_spNSBShader == 0)
        m_spNSBShader = NiNew NSBShader();

    return m_spNSBShader;
}
//---------------------------------------------------------------------------
bool NSFParsedShader::Save(char* pszFilename)
{
    // Open a binary stream
    NiFile* pkFile = NiFile::GetFile(pszFilename, NiFile::WRITE_ONLY);
    if (!pkFile)
        return false;

    bool bResult = SaveBinary(*pkFile);
    NiDelete pkFile;

    return bResult;
}
//---------------------------------------------------------------------------
bool NSFParsedShader::SaveBinary(NiBinaryStream& kStream)
{
    if (!m_spNSBShader)
        return false;

    return m_spNSBShader->SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NSFParsedShader::Load(char* pszFilename)
{
    // Open a binary stream
    NiFile* pkFile = NiFile::GetFile(pszFilename, NiFile::READ_ONLY);
    if (!pkFile)
        return false;

    bool bResult = LoadBinary(*pkFile);
    NiDelete pkFile;

    return bResult;
}
//---------------------------------------------------------------------------
bool NSFParsedShader::LoadBinary(NiBinaryStream& kStream)
{
    if (m_spNSBShader)
    {
        // Warn them it exists?
    }

    m_spNSBShader = 0;

    m_spNSBShader = NiNew NSBShader();
    if (!m_spNSBShader)
        return false;

    return m_spNSBShader->LoadBinary(kStream);
}
//---------------------------------------------------------------------------
