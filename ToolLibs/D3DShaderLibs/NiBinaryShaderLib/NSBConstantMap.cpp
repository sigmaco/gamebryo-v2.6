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
#include "NiBinaryShaderLibPCH.h"

#include "NSBConstantMap.h"
#include "NSBShader.h"
#include "NSBUtility.h"

#include <NiShaderDesc.h>
#include <NiGlobalConstantEntry.h>
#include <NiShaderFactory.h>

//---------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry::~NSBCM_Entry()
{
}
//---------------------------------------------------------------------------
bool NSBConstantMap::NSBCM_Entry::SaveBinary(NiBinaryStream& kStream)
{
    NiFixedString::SaveFixedStringAsCString(kStream, m_kKey);
    NiStreamSaveBinary(kStream, m_uiFlags);
    NiStreamSaveBinary(kStream, m_uiExtra);
    NiStreamSaveBinary(kStream, m_uiShaderRegister);
    NiStreamSaveBinary(kStream, m_uiRegisterCount);
    NiFixedString::SaveFixedStringAsCString(kStream, m_kVariableName);
    NiStreamSaveBinary(kStream, m_uiDataSize);
    NiStreamSaveBinary(kStream, m_uiDataStride);

    if (!NiRendererUtility::WriteData(kStream, m_pvDataSource, m_uiDataSize,
        GetComponentSize()))
        return false;
    return true;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::NSBCM_Entry::LoadBinary(NiBinaryStream& kStream)
{
    NiFixedString::LoadCStringAsFixedString(kStream, m_kKey);

    NiStreamLoadBinary(kStream, m_uiFlags);
    NiStreamLoadBinary(kStream, m_uiExtra);
    NiStreamLoadBinary(kStream, m_uiShaderRegister);
    NiStreamLoadBinary(kStream, m_uiRegisterCount);

    if (NSBShader::GetReadVersion() > 0x00010001)
    {
        // Version 1.2 added variable names
        NiFixedString::LoadCStringAsFixedString(kStream, m_kVariableName);
    }

    // Read in the actual data (used for CM_Constant and CM_Attribute)
    NiStreamLoadBinary(kStream, m_uiDataSize);
    NiStreamLoadBinary(kStream, m_uiDataStride);

    // Read in the data
    unsigned int uiDataSize = 0;
    if (!NiRendererUtility::AllocateAndReadData(kStream, m_pvDataSource,
        uiDataSize, GetComponentSize()))
    {
        return false;
    }
    NIASSERT(uiDataSize == m_uiDataSize);
    m_uiDataSize = uiDataSize;

    m_bOwnData = true;
    return true;
}
//---------------------------------------------------------------------------
// Compute the fundamental size of an component in the entry
// (i.e., sizeof(float) for matrix4x4, not 16 or 16*sizeof(float))
unsigned int NSBConstantMap::NSBCM_Entry::GetComponentSize() const
{
    switch (GetAttributeType())
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
        {
            if (m_uiDataSize > 0)
            {
                // Only warn about undefined types if there is actual data,
                // i.e., only when we cannot properly endian convert without
                // knowing type
                NILOG("Warning: NSB constant map contains entries with "
                    "undefined types.  Re-generating the NSB from the NSF may "
                    "resolve the problem.\n");
            }

            // CM_Constant entries are constrained by the NSF parser to be
            // floats, so we can still return a reasonable value for them
            if (IsConstant())
                return sizeof(float);
            else
                return 1;
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        // unsigned ints
        return sizeof(unsigned int);
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        // floats
        return sizeof(float);
    default:
        // bytes or other data that makes no sense to swap
        return 1;
    }
}
//---------------------------------------------------------------------------
NSBConstantMap::NSBConstantMap() :
    m_uiProgramType(0)
{
    m_kEntryList.RemoveAll();
    m_kListIter_DX9 = 0;
    m_kEntryList_DX9.RemoveAll();
    m_kListIter_Xenon = 0;
    m_kEntryList_Xenon.RemoveAll();
    m_kListIter_PS3 = 0;
    m_kEntryList_PS3.RemoveAll();
    m_kListIter_D3D10 = 0;
    m_kEntryList_D3D10.RemoveAll();
}
//---------------------------------------------------------------------------
NSBConstantMap::~NSBConstantMap()
{
    NSBCM_Entry* pkEntry;

    m_kListIter = m_kEntryList.GetHeadPos();
    while (m_kListIter)
    {
        pkEntry = m_kEntryList.GetNext(m_kListIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList.RemoveAll();

    m_kListIter_DX9 = m_kEntryList_DX9.GetHeadPos();
    while (m_kListIter_DX9)
    {
        pkEntry = m_kEntryList_DX9.GetNext(m_kListIter_DX9);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList_DX9.RemoveAll();

    m_kListIter_Xenon = m_kEntryList_Xenon.GetHeadPos();
    while (m_kListIter_Xenon)
    {
        pkEntry = m_kEntryList_Xenon.GetNext(m_kListIter_Xenon);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList_Xenon.RemoveAll();

    m_kListIter_PS3 = m_kEntryList_PS3.GetHeadPos();
    while (m_kListIter_PS3)
    {
        pkEntry = m_kEntryList_PS3.GetNext(m_kListIter_PS3);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList_PS3.RemoveAll();

    m_kListIter_D3D10 = m_kEntryList_D3D10.GetHeadPos();
    while (m_kListIter_D3D10)
    {
        pkEntry = m_kEntryList_D3D10.GetNext(m_kListIter_D3D10);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList_D3D10.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NSBConstantMap::GetGlobalEntryCount()
{
    return m_kEntryList.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NSBConstantMap::GetPlatformEntryCount(
    NiShader::Platform ePlatform)
{
    switch (ePlatform)
    {
    case NiShader::NISHADER_DX9:
        return m_kEntryList_DX9.GetSize();
    case NiShader::NISHADER_XENON:
        return m_kEntryList_Xenon.GetSize();
    case NiShader::NISHADER_PS3:
        return m_kEntryList_PS3.GetSize();
    case NiShader::NISHADER_D3D10:
        return m_kEntryList_D3D10.GetSize();
    default:
        return 0;
    }
}
//---------------------------------------------------------------------------
unsigned int NSBConstantMap::GetTotalEntryCount()
{
    return GetGlobalEntryCount() 
        + GetPlatformEntryCount(NiShader::NISHADER_PS3)
        + GetPlatformEntryCount(NiShader::NISHADER_DX9)
        + GetPlatformEntryCount(NiShader::NISHADER_XENON)
        + GetPlatformEntryCount(NiShader::NISHADER_D3D10);
}
//---------------------------------------------------------------------------
bool NSBConstantMap::AddEntry(const char* pcKey, unsigned int uiFlags, 
    unsigned int uiExtra, unsigned int uiReg, unsigned int uiCount,
    const char* pcVariableName, unsigned int uiSize, unsigned int uiStride, 
    void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry = GetEntryByKey(pcKey);
    if (pkEntry && pkEntry->GetFlags() == uiFlags 
        && pkEntry->GetExtra() == uiExtra)
    {
        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            true, "* ERROR: NSBConstantMap::AddEntry\n"
            "    Failed to add entry %s\n"
            "    It already exists!\n", 
            (const char*)pkEntry->GetKey());
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount, 
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList.AddTail(pkEntry);

    return true;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry(unsigned int uiPlatformFlags, 
    const char* pcKey, unsigned int uiFlags, unsigned int uiExtra, 
    unsigned int uiReg, unsigned int uiCount, const char* pcVariableName, 
    unsigned int uiSize, unsigned int uiStride, void* pvSource, 
    bool bCopyData)
{
    bool bResult = true;

    if (uiPlatformFlags & NiShader::NISHADER_DX9)
    {
        if (!AddPlatformSpecificEntry_DX9(pcKey, uiFlags, uiExtra, uiReg, 
            uiCount, pcVariableName, uiSize, uiStride, pvSource, bCopyData))
        {
            bResult = false;
        }
    }
    if (uiPlatformFlags & NiShader::NISHADER_XENON)
    {
        if (!AddPlatformSpecificEntry_Xenon(pcKey, uiFlags, uiExtra, uiReg, 
            uiCount, pcVariableName, uiSize, uiStride, pvSource, bCopyData))
        {
            bResult = false;
        }
    }
    if (uiPlatformFlags & NiShader::NISHADER_PS3)
    {
        if (!AddPlatformSpecificEntry_PS3(pcKey, uiFlags, uiExtra, uiReg, 
            uiCount, pcVariableName, uiSize, uiStride, pvSource, bCopyData))
        {
            bResult = false;
        }
    }
    if (uiPlatformFlags & NiShader::NISHADER_D3D10)
    {
        if (!AddPlatformSpecificEntry_D3D10(pcKey, uiFlags, uiExtra, uiReg, 
            uiCount, pcVariableName, uiSize, uiStride, pvSource, bCopyData))
        {
            bResult = false;
        }
    }

    return bResult;
}
//---------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetFirstEntry()
{
    NSBCM_Entry* pkEntry = 0;

    m_kListIter = m_kEntryList.GetHeadPos();
    if (m_kListIter)
        pkEntry = m_kEntryList.GetNext(m_kListIter);
    return pkEntry;
}
//---------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetNextEntry()
{
    NSBCM_Entry* pkEntry = 0;

    if (m_kListIter)
        pkEntry = m_kEntryList.GetNext(m_kListIter);
    return pkEntry;
}
//---------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetFirstPlatformEntry(
    NiShader::Platform ePlatform)
{
    NSBCM_Entry* pkEntry = 0;

    NiTListIterator* pkIter = 0;
    NiTPointerList<NSBCM_Entry*>* pkEntryList = 0;

    if (!GetPlatformListPointers(ePlatform, pkIter, pkEntryList))
        return 0;

    *pkIter = pkEntryList->GetHeadPos();
    if (*pkIter)
        pkEntry = pkEntryList->GetNext(*pkIter);
    return pkEntry;
}
//---------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetNextPlatformEntry(
    NiShader::Platform ePlatform)
{
    NSBCM_Entry* pkEntry = 0;

    NiTListIterator* pkIter = 0;
    NiTPointerList<NSBCM_Entry*>* pkEntryList = 0;

    if (!GetPlatformListPointers(ePlatform, pkIter, pkEntryList))
        return 0;

    if (*pkIter)
        pkEntry = pkEntryList->GetNext(*pkIter);
    return pkEntry;
}
//---------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetEntryByKey(const char* pcKey)
{
    NSBCM_Entry* pkEntry = 0;

    m_kListIter = m_kEntryList.GetHeadPos();
    while (m_kListIter)
    {
        pkEntry = m_kEntryList.GetNext(m_kListIter);
        if (pkEntry)
        {
            if (pkEntry->GetKey()== pcKey)
                return pkEntry;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetPlatformEntryByKey(
    NiShader::Platform ePlatform, const char* pcKey)
{
    NSBCM_Entry* pkEntry = 0;

    NiTListIterator* pkIter = 0;
    NiTPointerList<NSBCM_Entry*>* pkEntryList = 0;

    if (!GetPlatformListPointers(ePlatform, pkIter, pkEntryList))
        return false;

    *pkIter = pkEntryList->GetHeadPos();
    while (*pkIter)
    {
        pkEntry = pkEntryList->GetNext(*pkIter);
        if (pkEntry)
        {
            if (pkEntry->GetKey() == pcKey)
                return pkEntry;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NSBConstantMap::GetEntryIndexByKey(const char* pcKey)
{
    NSBCM_Entry* pkEntry = 0;
    unsigned int uiIndex = 0;

    m_kListIter = m_kEntryList.GetHeadPos();
    while (m_kListIter)
    {
        pkEntry = m_kEntryList.GetNext(m_kListIter);
        if (pkEntry)
        {
            if (pkEntry->GetKey() == pcKey)
                return uiIndex;
        }
        uiIndex++;
    }

    return 0xffffffff;
}
//---------------------------------------------------------------------------
unsigned int NSBConstantMap::GetPlatformEntryIndexByKey(
    NiShader::Platform ePlatform, const char* pcKey)
{
    NiTListIterator* pkIter = 0;
    NiTPointerList<NSBCM_Entry*>* pkEntryList = 0;

    if (!GetPlatformListPointers(ePlatform, pkIter, pkEntryList))
        return 0;

    NSBCM_Entry* pkEntry = 0;
    unsigned int uiIndex = 0;

    *pkIter = pkEntryList->GetHeadPos();
    while (*pkIter)
    {
        pkEntry = pkEntryList->GetNext(*pkIter);
        if (pkEntry)
        {
            if (pkEntry->GetKey() == pcKey)
                return uiIndex;
        }
        uiIndex++;
    }

    return 0xffffffff;
}
//---------------------------------------------------------------------------
NiPlatformShaderConstantMap* NSBConstantMap::GetVertexConstantMap(
    NiShaderDesc* pkShaderDesc)
{
    if (m_uiProgramType != NiGPUProgram::PROGRAM_VERTEX)
        return 0;

    if (GetTotalEntryCount() == 0)
        return 0;

    NiPlatformShaderConstantMap* pkSCMV = NiNew NiPlatformShaderConstantMap(
        (NiGPUProgram::ProgramType) m_uiProgramType);
    if (!pkSCMV)
        return 0;

    NSBCM_Entry* pkEntry = GetFirstEntry();
    while (pkEntry)
    {
        if (!ProcessMapEntry(pkShaderDesc, pkEntry, pkSCMV))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* ERROR: NSBConstantMap::GetVertexConstantMap\n"
                "    Failed processing of entry %s\n", 
                (const char*)pkEntry->GetKey());
        }

        pkEntry = GetNextEntry();
    }

    // These sections are compile-time determined, and will only add the
    // shader constants that are specific to the platform for which the
    // library has been built.
#if defined(WIN32)
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_DX9);
    while (pkEntry)
    {
        if (!ProcessMapEntry(pkShaderDesc, pkEntry, pkSCMV))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* ERROR: NSBConstantMap::GetVertexConstantMap\n"
                "    Failed processing of entry %s\n", 
                pkEntry->GetKey());
        }

        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_DX9);
    }
#elif defined(_XENON)
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_XENON);
    while (pkEntry)
    {
        if (!ProcessMapEntry(pkShaderDesc, pkEntry, pkSCMV))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* ERROR: NSBConstantMap::GetVertexConstantMap\n"
                "    Failed processing of entry %s\n", 
                pkEntry->GetKey());
        }

        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_XENON);
    }
#elif defined(_PS3)
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_PS3);
    while (pkEntry)
    {
        if (!ProcessMapEntry(pkShaderDesc, pkEntry, pkSCMV))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* ERROR: NSBConstantMap::GetVertexConstantMap\n"
                "    Failed processing of entry %s\n", 
                (const char*)pkEntry->GetKey());
        }

        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_PS3);
    }
#else
    NIASSERT(!"No valid renderer was found.");
#endif

    return pkSCMV;
}
//---------------------------------------------------------------------------
NiPlatformShaderConstantMap* NSBConstantMap::GetGeometryConstantMap(
    NiShaderDesc*)
{
    return NULL;
}
//---------------------------------------------------------------------------
NiPlatformShaderConstantMap* NSBConstantMap::GetPixelConstantMap(
    NiShaderDesc* pkShaderDesc)
{
    if (m_uiProgramType != NiGPUProgram::PROGRAM_PIXEL)
        return 0;

    if (GetTotalEntryCount() == 0)
        return 0;

    NiPlatformShaderConstantMap* pkSCMP = NiNew NiPlatformShaderConstantMap(
        (NiGPUProgram::ProgramType)m_uiProgramType);
    if (!pkSCMP)
        return 0;

    NSBCM_Entry* pkEntry = GetFirstEntry();
    while (pkEntry)
    {
        if (!ProcessMapEntry(pkShaderDesc, pkEntry, pkSCMP))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* ERROR: NSBConstantMap::GetPixelConstantMap\n"
                "    Failed processing of entry %s\n", 
                (const char*)pkEntry->GetKey());
        }
        pkEntry = GetNextEntry();
    }

    // These sections are compile-time determined, and will only add the
    // shader constants that are specific to the platform the library has
    // be compiled for.
#if defined(WIN32)
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_DX9);
    while (pkEntry)
    {
        if (!ProcessMapEntry(pkShaderDesc, pkEntry, pkSCMP))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* ERROR: NSBConstantMap::GetPixelConstantMap\n"
                "    Failed processing of entry %s\n", 
                pkEntry->GetKey());
        }
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_DX9);
    }
#elif defined(_XENON)
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_XENON);
    while (pkEntry)
    {
        if (!ProcessMapEntry(pkShaderDesc, pkEntry, pkSCMP))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* ERROR: NSBConstantMap::GetPixelConstantMap\n"
                "    Failed processing of entry %s\n", 
                pkEntry->GetKey());
        }
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_XENON);
    }
#elif defined(_PS3)
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_PS3);
    while (pkEntry)
    {
        if (!ProcessMapEntry(pkShaderDesc, pkEntry, pkSCMP))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, "* ERROR: NSBConstantMap::GetPixelConstantMap\n"
                "    Failed processing of entry %s\n", 
                (const char*)pkEntry->GetKey());
        }
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_PS3);
    }
#else
    NIASSERT(!"No valid renderer was found.");
#endif

    return pkSCMP;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::SaveBinary(NiBinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiProgramType);
    if (!SaveBinaryEntries(kStream))
        return false;
    return true;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiProgramType);
    if (!LoadBinaryEntries(kStream))
        return false;
    return true;
}
//---------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::CreateEntry(const char* pcKey, 
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg, 
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize, 
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
    if (pkEntry)
    {
        pkEntry->SetKey(pcKey);
        pkEntry->SetFlags(uiFlags);
        pkEntry->SetExtra(uiExtra);
        pkEntry->SetShaderRegister(uiReg);
        pkEntry->SetRegisterCount(uiCount);
        pkEntry->SetVariableName(pcVariableName);
        pkEntry->SetData(uiSize, uiStride, pvSource, bCopyData);
    }

    return pkEntry;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry_DX9(const char* pcKey, 
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg, 
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize, 
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry = GetPlatformEntryByKey(NiShader::NISHADER_DX9,
        pcKey);
    if (pkEntry)
    {
        // Once error tracking is complete, store this error
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount, 
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList_DX9.AddTail(pkEntry);

    return true;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry_Xenon(const char* pcKey, 
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg, 
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize, 
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry = GetPlatformEntryByKey(NiShader::NISHADER_XENON,
        pcKey);
    if (pkEntry)
    {
        // Once error tracking is complete, store this error
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount, 
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList_Xenon.AddTail(pkEntry);

    return true;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry_PS3(const char* pcKey, 
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg, 
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize, 
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry = GetPlatformEntryByKey(NiShader::NISHADER_PS3, 
        pcKey);
    if (pkEntry)
    {
        // Once error tracking is complete, store this error
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount, 
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList_PS3.AddTail(pkEntry);

    return true;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry_D3D10(const char* pcKey, 
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg, 
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize, 
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry
        = GetPlatformEntryByKey(NiShader::NISHADER_D3D10, pcKey);
    if (pkEntry)
    {
        // Once error tracking is complete, store this error
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount, 
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList_D3D10.AddTail(pkEntry);

    return true;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::GetPlatformListPointers(NiShader::Platform ePlatform,
    NiTListIterator*& pkIter, NiTPointerList<NSBCM_Entry*>*& pkEntryList)
{
    pkIter = 0;
    pkEntryList = 0;

    switch (ePlatform)
    {
    case NiShader::NISHADER_DX9:
        pkIter = &m_kListIter_DX9;
        pkEntryList = &m_kEntryList_DX9;
        return true;
    case NiShader::NISHADER_XENON:
        pkIter = &m_kListIter_Xenon;
        pkEntryList = &m_kEntryList_Xenon;
        return true;
    case NiShader::NISHADER_PS3:
        pkIter = &m_kListIter_PS3;
        pkEntryList = &m_kEntryList_PS3;
        return true;
    case NiShader::NISHADER_D3D10:
        pkIter = &m_kListIter_D3D10;
        pkEntryList = &m_kEntryList_D3D10;
        return true;
    default:
        return false;   // Invalid platform.
    }
}
//---------------------------------------------------------------------------
bool NSBConstantMap::ProcessMapEntry(NiShaderDesc* pkShaderDesc,
    NSBCM_Entry* pkEntry, 
    NiPlatformShaderConstantMap* pkSCM)
{
    if (!pkEntry || !pkSCM)
        return false;

    NiShaderError err;

    if (pkEntry->IsGlobal())
    {
        // We have to register the constant with the renderer
        NiShaderAttributeDesc::AttributeType eType = 
            NiShaderConstantMapEntry::GetAttributeType(
            pkEntry->GetFlags());
        if (!NiShaderFactory::RegisterGlobalShaderConstant(
            pkEntry->GetKey(), eType, pkEntry->GetDataSize(), 
            pkEntry->GetDataSource()))
        {
            NIASSERT(!"Failed to add global constant!");
        }

        // Retrieve the global entry
        NiShaderFactory* pkShaderFactory = NiShaderFactory::GetInstance();

        NiGlobalConstantEntry* pkGlobal = 
            pkShaderFactory->GetGlobalShaderConstantEntry(
            pkEntry->GetKey());
        NIASSERT(pkGlobal);

        err = pkSCM->AddEntry(pkEntry->GetKey(), pkEntry->GetFlags(),
            0, pkEntry->GetShaderRegister(), pkEntry->GetRegisterCount(), 
            pkEntry->GetVariableName(), pkEntry->GetDataSize(), 
            pkEntry->GetDataStride(), pkGlobal->GetDataSource());

        // Release the entry immediately; the shader constant map should 
        // maintain a reference to the entry.
        NiShaderFactory::ReleaseGlobalShaderConstant(pkEntry->GetKey());
    }
    else if (pkEntry->IsAttribute())
    {
        bool bAllocatedMemory = false;

        // Find default value in case attribute is not on geometry.
        const NiShaderAttributeDesc* pkDesc = 
            pkShaderDesc->GetAttribute(pkEntry->GetKey());
        if (pkDesc == NULL)
            return false;

        const char* pcValue;
        bool bValue;
        float afValue[16];
        float* pfValue = afValue;
        unsigned int uiValue;

        void* pvDataSource = afValue;
        unsigned int uiDataSize = 0;
        unsigned int uiDataStride = sizeof(afValue[0]);
        switch (pkDesc->GetType())
        {
        case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            pkDesc->GetValue_Bool(bValue);
            pvDataSource = &bValue;
            uiDataSize = sizeof(bValue);
            uiDataStride = sizeof(bValue);
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            pkDesc->GetValue_UnsignedInt(uiValue);
            pvDataSource = &uiValue;
            uiDataSize = sizeof(uiValue);
            uiDataStride = sizeof(uiValue);
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            pkDesc->GetValue_Float(*afValue);
            uiDataSize = uiDataStride;
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            NiPoint2 kValue;
            pkDesc->GetValue_Point2(kValue);
            afValue[0] = kValue.x;
            afValue[1] = kValue.y;
            uiDataSize = 2 * uiDataStride;
            break;
        }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            NiPoint3 kValue;
            pkDesc->GetValue_Point3(kValue);
            afValue[0] = kValue.x;
            afValue[1] = kValue.y;
            afValue[2] = kValue.z;
            uiDataSize = 3 * uiDataStride;
            break;
        }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            pkDesc->GetValue_Point4(pfValue);
            uiDataSize = 4 * uiDataStride;
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            NiMatrix3 kValue;
            pkDesc->GetValue_Matrix3(kValue);
            afValue[0] = kValue.GetEntry(0, 0);
            afValue[1] = kValue.GetEntry(0, 1);
            afValue[2] = kValue.GetEntry(0, 2);
            afValue[3] = kValue.GetEntry(1, 0);
            afValue[4] = kValue.GetEntry(1, 1);
            afValue[5] = kValue.GetEntry(1, 2);
            afValue[6] = kValue.GetEntry(2, 0);
            afValue[7] = kValue.GetEntry(2, 1);
            afValue[8] = kValue.GetEntry(2, 2);
            uiDataSize = 9 * uiDataStride;
            break;
        }
        case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            pkDesc->GetValue_Matrix4(pfValue, 16 * sizeof(float));
            uiDataSize = 16 * uiDataStride;
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            NiColorA kValue;
            pkDesc->GetValue_ColorA(kValue);
            afValue[0] = kValue.r;
            afValue[1] = kValue.g;
            afValue[2] = kValue.b;
            afValue[3] = kValue.a;
            uiDataSize = 4 * uiDataStride;
            break;
        }
        case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            pkDesc->GetValue_String(pcValue);
            pvDataSource = (void*)pcValue;
            uiDataSize = (unsigned int)((pcValue == NULL || *pcValue == '\0') ?
                0 : strlen(pcValue) * sizeof(*pcValue));
            uiDataStride = sizeof(*pcValue);
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        {
            NiShaderAttributeDesc::AttributeType eType;
            unsigned int uiElementSize;
            unsigned int uiNumElements;
            pkDesc->GetArrayParams(
                eType,
                uiElementSize,
                uiNumElements);

            // get copy of data
            uiDataStride = uiElementSize;
            uiDataSize = uiElementSize*uiNumElements;
            pvDataSource = NiAlloc(char, uiDataSize);
            bAllocatedMemory = true;
            pkDesc->GetValue_Array(pvDataSource, uiDataSize);

            break;
        }
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
        default:
            uiDataSize = 0;
            uiDataStride = 0;
            pvDataSource = NULL;
            break;
        }

        size_t stBufSize = strlen(pkEntry->GetKey()) + 1;
        char* pcMapping = NiAlloc(char, stBufSize);
        NiStrcpy(pcMapping, stBufSize, pkEntry->GetKey());
        char* pcPtr = strstr(pcMapping, "@@");
        if (pcPtr)
            *pcPtr = '\0';

        err = pkSCM->AddEntry(pcMapping, pkEntry->GetFlags(),
            pkEntry->GetExtra(), pkEntry->GetShaderRegister(), 
            pkEntry->GetRegisterCount(), pkEntry->GetVariableName(), 
            uiDataSize, uiDataStride, pvDataSource, true);
        
        if (bAllocatedMemory)
        {
            NiFree(pvDataSource);
        }
        NiFree(pcMapping);
    }
    else
    {
        size_t stBufSize = strlen(pkEntry->GetKey()) + 1;
        char* pcMapping = NiAlloc(char, stBufSize);
        NiStrcpy(pcMapping, stBufSize, pkEntry->GetKey());
        char* pcPtr = strstr(pcMapping, "@@");
        if (pcPtr)
            *pcPtr = '\0';

        err = pkSCM->AddEntry(pcMapping, pkEntry->GetFlags(),
            pkEntry->GetExtra(), pkEntry->GetShaderRegister(),
            pkEntry->GetRegisterCount(), pkEntry->GetVariableName(), 
            pkEntry->GetDataSize(), pkEntry->GetDataStride(), 
            pkEntry->GetDataSource());

        NiFree(pcMapping);
    }
    
    if (err != NISHADERERR_OK)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::SaveBinaryEntries(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_kEntryList.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    NSBCM_Entry* pkEntry = GetFirstEntry();
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextEntry();
    }

    if (uiTestCount != uiCount)
        return false;

    // Version 1.4 added support for platform-specific constant map entries.
    // DX9 comes first
    uiCount = m_kEntryList_DX9.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    uiTestCount = 0;

    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_DX9);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_DX9);
    }

    if (uiTestCount != uiCount)
        return false;

    // Then Xenon
    uiCount = m_kEntryList_Xenon.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    uiTestCount = 0;

    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_XENON);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_XENON);
    }

    if (uiTestCount != uiCount)
        return false;

    // Then PS3
    uiCount = m_kEntryList_PS3.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    uiTestCount = 0;

    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_PS3);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_PS3);
    }

    if (uiTestCount != uiCount)
        return false;
    // Then D3D10.
    uiCount = m_kEntryList_D3D10.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    uiTestCount = 0;

    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_D3D10);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_D3D10);
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBConstantMap::LoadBinaryEntries(NiBinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    unsigned int ui = 0;
    for (; ui < uiCount; ui++)
    {
        NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
        NIASSERT(pkEntry);

        if (!pkEntry->LoadBinary(kStream))
            return false;

        m_kEntryList.AddTail(pkEntry);
    }

    if (NSBShader::GetReadVersion() >= 0x00010004)
    {
        // Version 1.4 added support for platform-specific constant map 
        // entries

        if (NSBShader::GetReadVersion() < 0x00010008)
        {
            // Version 1.8 removed the DX8 support

            NiStreamLoadBinary(kStream, uiCount);

            NSBCM_Entry kEntry;
            for (ui = 0; ui < uiCount; ui++)
            {
                if (!kEntry.LoadBinary(kStream))
                    return false;
            }
        }

        // DX9 comes next
        NiStreamLoadBinary(kStream, uiCount);

        for (ui = 0; ui < uiCount; ui++)
        {
            NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
            NIASSERT(pkEntry);

            if (!pkEntry->LoadBinary(kStream))
                return false;

            m_kEntryList_DX9.AddTail(pkEntry);
        }

        if (NSBShader::GetReadVersion() < 0x00010008)
        {
            // Version 1.8 removed the Xbox support

            NiStreamLoadBinary(kStream, uiCount);

            NSBCM_Entry kEntry;
            for (ui = 0; ui < uiCount; ui++)
            {
                if (!kEntry.LoadBinary(kStream))
                    return false;
            }
        }
        else //NSBShader::GetReadVersion() >= 0x00010008
        {
            // Version 1.8 added the Xenon support

            // Then Xenon
            NiStreamLoadBinary(kStream, uiCount);

            for (ui = 0; ui < uiCount; ui++)
            {
                NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
                NIASSERT(pkEntry);

                if (!pkEntry->LoadBinary(kStream))
                    return false;

                m_kEntryList_Xenon.AddTail(pkEntry);
            }
        }
        if (NSBShader::GetReadVersion() >= 0x00010012)
        {
            // Version 1.12 added PS3 support
            NiStreamLoadBinary(kStream, uiCount);

            for (ui = 0; ui < uiCount; ui++)
            {
                NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
                NIASSERT(pkEntry);

                if (!pkEntry->LoadBinary(kStream))
                    return false;

                m_kEntryList_PS3.AddTail(pkEntry);
            }
        }
        if (NSBShader::GetReadVersion() >= 0x00010013)
        {
            // Version 1.13 added D3D10 and geometry shader support.
            NiStreamLoadBinary(kStream, uiCount);

            for (ui = 0; ui < uiCount; ui++)
            {
                NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
                NIASSERT(pkEntry);

                if (!pkEntry->LoadBinary(kStream))
                    return false;

                m_kEntryList_D3D10.AddTail(pkEntry);
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBConstantMap::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "      ProgamType = %d\n", m_uiProgramType);
    NSBUtility::Dump(pf, true, "Entry Count = %d\n", m_kEntryList.GetSize());
    
    NSBUtility::IndentInsert();

    NSBCM_Entry* pkEntry = GetFirstEntry();
    while (pkEntry)
    {
        NSBUtility::Dump(pf, true, "%16s - ", (const char*)pkEntry->GetKey());
        if (pkEntry->IsConstant())
            NSBUtility::Dump(pf, false, "Constant  ");
        if (pkEntry->IsDefined())
            NSBUtility::Dump(pf, false, "Defined   ");
        if (pkEntry->IsAttribute())
            NSBUtility::Dump(pf, false, "Attribute ");

        if (pkEntry->IsBool())
            NSBUtility::Dump(pf, false, "BOOL    ");
        if (pkEntry->IsString())
            NSBUtility::Dump(pf, false, "STRING  ");
        if (pkEntry->IsUnsignedInt())
            NSBUtility::Dump(pf, false, "UINT    ");
        if (pkEntry->IsFloat())
            NSBUtility::Dump(pf, false, "FLOAT   ");
        if (pkEntry->IsPoint2())
            NSBUtility::Dump(pf, false, "POINT2  ");
        if (pkEntry->IsPoint3())
            NSBUtility::Dump(pf, false, "POINT3  ");
        if (pkEntry->IsPoint4())
            NSBUtility::Dump(pf, false, "POINT4  ");
        if (pkEntry->IsMatrix3())
            NSBUtility::Dump(pf, false, "MATRIX3 ");
        if (pkEntry->IsMatrix4())
            NSBUtility::Dump(pf, false, "MATRIX4 ");
        if (pkEntry->IsColor())
            NSBUtility::Dump(pf, false, "COLOR   ");
        if (pkEntry->IsTexture())
            NSBUtility::Dump(pf, false, "TEXTURE ");

        NSBUtility::Dump(pf, false, "Ex    = %3d ", 
            pkEntry->GetExtra());
        NSBUtility::Dump(pf, false, "Reg   = %3d ", 
            pkEntry->GetShaderRegister());
        NSBUtility::Dump(pf, false, "Count = %3d ", 
            pkEntry->GetRegisterCount());
        NSBUtility::Dump(pf, false, "\n");

        pkEntry = GetNextEntry();
    }

    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
