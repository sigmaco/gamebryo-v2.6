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
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10UserDefinedDataBlock.h"
#include "NSBD3D10Utility.h"

#include <NiD3D10ShaderConstantMap.h>
#include <NiD3D10ShaderFactory.h>

//---------------------------------------------------------------------------
bool NSBD3D10UserDefinedDataBlock::AddEntry(char* pcKey, unsigned int uiFlags, 
    unsigned int uiSize, unsigned int uiStride, void* pvSource, 
    bool bCopyData)
{
    NSBD3D10CM_Entry* pkEntry = GetEntryByKey(pcKey);
    if (pkEntry)
    {
        NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            true, "* ERROR: NSBD3D10UserDefinedDataBlock::AddEntry\n"
            "    Failed to add entry %s\n"
            "    It already exists!\n", 
            (const char*)pkEntry->GetKey());
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, 0, 0xffffffff, 1, 0, 
        uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            true, "* ERROR: NSBD3D10UserDefinedDataBlock::AddEntry\n"
            "    Failed to create entry %s\n", 
            pcKey);
        return false;
    }

    m_kEntryList.AddTail(pkEntry);

    return true;
}
//---------------------------------------------------------------------------
NiD3D10ShaderConstantMap* NSBD3D10UserDefinedDataBlock::GetPixelConstantMap(
    NiShaderDesc*)
{
    NIASSERT(!"NSBD3D10UserDefinedDataBlock> GetPixelConstantMap should NOT "
        "be called!");
    return 0;
}
//---------------------------------------------------------------------------
NiD3D10ShaderConstantMap* NSBD3D10UserDefinedDataBlock::GetVertexConstantMap(
    NiShaderDesc*)
{
    NIASSERT(!"NSBD3D10UserDefinedDataBlock> GetVertexConstantMap should NOT "
        "be called!");
    return 0;
}
//---------------------------------------------------------------------------
bool NSBD3D10UserDefinedDataBlock::SaveBinary(NiBinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    return NSBD3D10ConstantMap::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NSBD3D10UserDefinedDataBlock::LoadBinary(NiBinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();
    return NSBD3D10ConstantMap::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBD3D10UserDefinedDataBlock::Dump(FILE* pf)
{
    NSBD3D10Utility::Dump(pf, true, "UserDefinedDataBlock\n");
    NSBD3D10Utility::IndentInsert();
    NSBD3D10Utility::Dump(pf, true, "Name = %s\n", m_pcName);
    NSBD3D10Utility::IndentInsert();
    NSBD3D10ConstantMap::Dump(pf);
    NSBD3D10Utility::IndentRemove();
    NSBD3D10Utility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
bool NSBD3D10UserDefinedDataBlock::SaveBinaryEntries(NiBinaryStream& kStream)
{
    return NSBD3D10ConstantMap::SaveBinaryEntries(kStream);
}
//---------------------------------------------------------------------------
bool NSBD3D10UserDefinedDataBlock::LoadBinaryEntries(NiBinaryStream& kStream)
{
    return NSBD3D10ConstantMap::LoadBinaryEntries(kStream);
}
//---------------------------------------------------------------------------
