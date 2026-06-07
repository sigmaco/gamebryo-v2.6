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

#include <NiShaderFactory.h>
#include <NiShaderConstantMap.h>

#include "NSBUserDefinedDataBlock.h"
#include "NSBUtility.h"

//---------------------------------------------------------------------------
bool NSBUserDefinedDataBlock::AddEntry(char* pcKey, unsigned int uiFlags, 
    unsigned int uiSize, unsigned int uiStride, void* pvSource, 
    bool bCopyData)
{
    NSBCM_Entry* pkEntry = GetEntryByKey(pcKey);
    if (pkEntry)
    {
        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            true, "* ERROR: NSBUserDefinedDataBlock::AddEntry\n"
            "    Failed to add entry %s\n"
            "    It already exists!\n", 
            (const char*)pkEntry->GetKey());
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, 0, 0xffffffff, 1, 0, 
        uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            true, "* ERROR: NSBUserDefinedDataBlock::AddEntry\n"
            "    Failed to create entry %s\n", 
            pcKey);
        return false;
    }

    m_kEntryList.AddTail(pkEntry);

    return true;
}
//---------------------------------------------------------------------------
NiPlatformShaderConstantMap* NSBUserDefinedDataBlock::GetPixelConstantMap(
    NiShaderDesc*)
{
    NIASSERT(!"NSBUserDefinedDataBlock> GetPixelConstantMap should NOT be "
        "called!");
    return 0;
}
//---------------------------------------------------------------------------
NiPlatformShaderConstantMap* NSBUserDefinedDataBlock::GetVertexConstantMap(
    NiShaderDesc*)
{
    NIASSERT(!"NSBUserDefinedDataBlock> GetVertexConstantMap should NOT be "
        "called!");
    return 0;
}
//---------------------------------------------------------------------------
bool NSBUserDefinedDataBlock::SaveBinary(NiBinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    return NSBConstantMap::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NSBUserDefinedDataBlock::LoadBinary(NiBinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();
    return NSBConstantMap::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBUserDefinedDataBlock::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "UserDefinedDataBlock\n");
    NSBUtility::IndentInsert();
    NSBUtility::Dump(pf, true, "Name = %s\n", m_pcName);
    NSBUtility::IndentInsert();
    NSBConstantMap::Dump(pf);
    NSBUtility::IndentRemove();
    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
bool NSBUserDefinedDataBlock::SaveBinaryEntries(NiBinaryStream& kStream)
{
    return NSBConstantMap::SaveBinaryEntries(kStream);
}
//---------------------------------------------------------------------------
bool NSBUserDefinedDataBlock::LoadBinaryEntries(NiBinaryStream& kStream)
{
    return NSBConstantMap::LoadBinaryEntries(kStream);
}
//---------------------------------------------------------------------------
