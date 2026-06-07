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

#include "NiMainPCH.h"
#include "NiSemanticAdapterTable.h"
#include "NiMaterial.h"
#include "NiBinaryLoadSave.h"
//---------------------------------------------------------------------------
// Implementation of NiSemanticAdapterTable
//---------------------------------------------------------------------------
NiSemanticAdapterTable::NiSemanticAdapterTable(
    NiUInt32 uiNumEntries) :
    m_uiFilledEntries(0),
    m_kAdapterTable(uiNumEntries)
{
    InitializeTable(uiNumEntries);
}
//---------------------------------------------------------------------------
NiSemanticAdapterTable::~NiSemanticAdapterTable()
{
    /* */
}
//---------------------------------------------------------------------------
NiUInt32 NiSemanticAdapterTable::GetFreeEntry()
{
    if (m_uiFilledEntries == 0)
    {
        if (m_kAdapterTable.GetSize() < 1)
            return m_kAdapterTable.Add(0);
        else
            return 0;
    }
    else if (m_uiFilledEntries < m_kAdapterTable.GetSize())
    {
        // Scan the table to find the last set entry
        NiUInt32 uiLastValidEntry = 0;
        for (NiUInt32 ui = 0; ui < m_kAdapterTable.GetSize(); ++ui)
        {
            if (m_kAdapterTable.GetAt(ui).m_kGenericSemantic.Exists())
                uiLastValidEntry = ui;
        }

        if (uiLastValidEntry + 1 < m_kAdapterTable.GetSize())
            return uiLastValidEntry + 1;
        else
            // Last valid entry was at the end of the table, grow the table
            return m_kAdapterTable.Add(0);
    }
    else
    {
        return m_kAdapterTable.Add(0);
    }
}
//---------------------------------------------------------------------------
NiSemanticAdapterTable& NiSemanticAdapterTable::operator=(
    const NiSemanticAdapterTable& kAdapterTable)
{
    const NiUInt32 uiTableEntries = 
        kAdapterTable.GetNumAllocatedTableEntries();
    InitializeTable(uiTableEntries);
    for (NiUInt32 i = 0; i < uiTableEntries; i++)
    {
        const SemanticMapping& kMapping = 
            kAdapterTable.m_kAdapterTable.GetAt(i);
        m_kAdapterTable.SetAt(i, kMapping);
    }

    m_uiFilledEntries = kAdapterTable.m_uiFilledEntries;

    return *this;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTable::operator==(const NiSemanticAdapterTable& kSAT)
    const
{
    // Must have the same number of entries to be equal
    if (kSAT.m_uiFilledEntries != m_uiFilledEntries)
        return false;
    if (kSAT.GetNumAllocatedTableEntries() < GetNumAllocatedTableEntries())
        return false;  // shouldn't be possible if the first test passed

    // Test each entry in turn
    const NiUInt32 uiTableEntryCount = GetNumAllocatedTableEntries();
    for (NiUInt32 ui = 0; ui < uiTableEntryCount; ++ui)
    {
        if (!(kSAT.m_kAdapterTable.GetAt(ui) == m_kAdapterTable.GetAt(ui)))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSemanticAdapterTable::InitializeTable(NiUInt32 uiNumEntries)
{
    // Remove all first so the reallocation doesn't transfer entries over
    m_kAdapterTable.RemoveAll();
    m_kAdapterTable.SetSize(uiNumEntries);
    m_uiFilledEntries = 0;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTable::SetGenericSemantic(NiUInt32 uiEntry, 
    const NiFixedString& kName, NiUInt8 uiIndex)
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
    {
        SemanticMapping kMapping = m_kAdapterTable.GetAt(uiEntry);

        // Non-null generic semantic used as indication of "filled" entry.
        if (!kMapping.m_kGenericSemantic.Exists() && kName.Exists())
            m_uiFilledEntries++;

        kMapping.m_kGenericSemantic = kName;
        kMapping.m_uiGenericSemanticIndex = uiIndex;
        m_kAdapterTable.SetAt(uiEntry, kMapping);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTable::SetRendererSemantic(NiUInt32 uiEntry, 
    const NiFixedString& kName, NiUInt8 uiIndex)
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
    {
        SemanticMapping kMapping = m_kAdapterTable.GetAt(uiEntry);
        kMapping.m_kRendererSemantic = kName;
        kMapping.m_uiRendererSemanticIndex = uiIndex;
        m_kAdapterTable.SetAt(uiEntry, kMapping);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTable::SetComponentCount(NiUInt32 uiEntry, 
    NiUInt8 uiComponentCount)
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
    {
        SemanticMapping kMapping = m_kAdapterTable.GetAt(uiEntry);
        kMapping.m_uiComponentCount = uiComponentCount;
        m_kAdapterTable.SetAt(uiEntry, kMapping);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTable::SetSharedRendererSemanticPosition(
    NiUInt32 uiEntry, NiUInt8 uiIndex)
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
    {
        SemanticMapping kMapping = m_kAdapterTable.GetAt(uiEntry);
        kMapping.m_uiSharedRendererSemanticPosition = uiIndex;
        m_kAdapterTable.SetAt(uiEntry, kMapping);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTable::CreateDefaultTable(
    const NiShaderDeclaration::ShaderRegisterEntry* pkSemantics, 
    NiUInt32 uiSemanticEntryCount)
{
    NIASSERT(pkSemantics);

    bool bSkinned = false;

    // First count number of valid entries
    NiUInt32 uiEntry = 0;
    for (; uiEntry < uiSemanticEntryCount; uiEntry++)
    {
        if (pkSemantics[uiEntry].m_uiUsageIndex == 
            NiMaterial::VS_INPUTS_TERMINATE_ARRAY)
        {
            break;
        }

        if (pkSemantics[uiEntry].m_eInput == 
            NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT)
        {
            bSkinned = true;
        }
    }
    const NiUInt32 uiEntryCount = uiEntry;

    // Resize table
    InitializeTable(uiEntryCount);

    const NiShaderDeclaration::ShaderRegisterEntry* pkCurrentSemantic = 
        pkSemantics;
    for (uiEntry = 0; uiEntry < uiEntryCount; uiEntry++)
    {
        NiFixedString kGenericSemantic;
        NiUInt32 uiGenericSemanticIndex = 0;
        NiShaderDeclaration::ShaderParameterToSemanticAndIndex(
            pkCurrentSemantic->m_eInput, kGenericSemantic, 
            uiGenericSemanticIndex, bSkinned);

        SetGenericSemantic(uiEntry, kGenericSemantic, 
            (NiUInt8)uiGenericSemanticIndex);
        NiFixedString kRendererSemantic = pkCurrentSemantic->m_kUsage;
        SetRendererSemantic(uiEntry, kRendererSemantic, 
            (NiUInt8)(pkCurrentSemantic->m_uiUsageIndex));

        SetComponentCount(uiEntry, 
            static_cast<NiUInt8>(NiShaderDeclaration::GetComponentCount(
            pkCurrentSemantic->m_eType)));
        SetSharedRendererSemanticPosition(uiEntry, 0);

        pkCurrentSemantic++;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiSemanticAdapterTable::LoadBinary(NiBinaryStream& kStream)
{
    // NOTE: This function uses an NiBinaryStream (rather than the
    //   typical NiStream used throughout NiMain) because some NSB*
    //   classes reuse this class.

    // Read in the number of entries
    unsigned int uiEntryCount;
    NiStreamLoadBinary(kStream, uiEntryCount);

    // Read in each entry
    InitializeTable(uiEntryCount);
    for (NiUInt32 ui = 0; ui < uiEntryCount; ++ui)
    {
        SemanticMapping& kMapping = m_kAdapterTable.GetAt(ui);
        kMapping.LoadBinary(kStream);

        // Non-null generic semantic used as indication of "filled" entry.
        if (kMapping.m_kGenericSemantic.Exists())
            m_uiFilledEntries++;
    }
}
//---------------------------------------------------------------------------
void NiSemanticAdapterTable::SaveBinary(NiBinaryStream& kStream) const
{
    // Write out the number of entries
    const unsigned int uiEntryCount = GetNumAllocatedTableEntries();
    NiStreamSaveBinary(kStream, uiEntryCount);

    // Write out each entry
    for (NiUInt32 ui = 0; ui < uiEntryCount; ++ui)
    {
        const SemanticMapping& kMapping = m_kAdapterTable.GetAt(ui);
        kMapping.SaveBinary(kStream);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Implementation of NiSemanticAdapterTable::SemanticMapping
//---------------------------------------------------------------------------
NiSemanticAdapterTable::SemanticMapping::SemanticMapping(NiUInt32) :
    m_kGenericSemantic(NULL),
    m_kRendererSemantic(NULL),
    m_uiGenericSemanticIndex(0),
    m_uiRendererSemanticIndex(0),
    m_uiComponentCount(0),
    m_uiSharedRendererSemanticPosition(0)
{
    // Ignore uiValue; it's only provided so "SemanticMapping kMapping = 0;"
    // is valid. This is needed so the class can be in an NiTArray.
}
//---------------------------------------------------------------------------
NiSemanticAdapterTable::SemanticMapping::SemanticMapping(
    const SemanticMapping& kMapping) :
    m_kGenericSemantic(kMapping.m_kGenericSemantic),
    m_kRendererSemantic(kMapping.m_kRendererSemantic),
    m_uiGenericSemanticIndex(kMapping.m_uiGenericSemanticIndex),
    m_uiRendererSemanticIndex(kMapping.m_uiRendererSemanticIndex),
    m_uiComponentCount(kMapping.m_uiComponentCount),
    m_uiSharedRendererSemanticPosition(
        kMapping.m_uiSharedRendererSemanticPosition)
{
    /* */
}
//---------------------------------------------------------------------------
NiSemanticAdapterTable::SemanticMapping& 
    NiSemanticAdapterTable::SemanticMapping::operator= (
    const NiSemanticAdapterTable::SemanticMapping& kMapping)
{
    m_kGenericSemantic = kMapping.m_kGenericSemantic;
    m_kRendererSemantic = kMapping.m_kRendererSemantic;
    m_uiGenericSemanticIndex = kMapping.m_uiGenericSemanticIndex;
    m_uiRendererSemanticIndex = kMapping.m_uiRendererSemanticIndex;
    m_uiComponentCount = kMapping.m_uiComponentCount;
    m_uiSharedRendererSemanticPosition = 
        kMapping.m_uiSharedRendererSemanticPosition;
    return *this;
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTable::SemanticMapping::operator== (
    const NiSemanticAdapterTable::SemanticMapping& kMapping) const
{
    return (m_kGenericSemantic == kMapping.m_kGenericSemantic &&
        m_kRendererSemantic == kMapping.m_kRendererSemantic &&
        m_uiGenericSemanticIndex == kMapping.m_uiGenericSemanticIndex &&
        m_uiRendererSemanticIndex == kMapping.m_uiRendererSemanticIndex &&
        m_uiComponentCount == kMapping.m_uiComponentCount &&
        m_uiSharedRendererSemanticPosition == 
        kMapping.m_uiSharedRendererSemanticPosition);
}
//---------------------------------------------------------------------------
bool NiSemanticAdapterTable::SemanticMapping::operator!= (
    const NiSemanticAdapterTable::SemanticMapping& kMapping) const
{
    return !(*this == kMapping);
}
//---------------------------------------------------------------------------
void NiSemanticAdapterTable::SemanticMapping::LoadBinary(
    NiBinaryStream& kStream)
{
    // NOTE: This function uses an NiBinaryStream (rather than the
    //   typical NiStream used throughout NiMain) because some NSB*
    //   classes reuse this class.

    NiFixedString::LoadCStringAsFixedString(kStream, m_kGenericSemantic);
    NiFixedString::LoadCStringAsFixedString(kStream, m_kRendererSemantic);

    NiStreamLoadBinary(kStream, m_uiGenericSemanticIndex);
    NiStreamLoadBinary(kStream, m_uiRendererSemanticIndex);
    NiStreamLoadBinary(kStream, m_uiComponentCount);
    NiStreamLoadBinary(kStream, m_uiSharedRendererSemanticPosition);
}
//---------------------------------------------------------------------------
void NiSemanticAdapterTable::SemanticMapping::SaveBinary(
    NiBinaryStream& kStream) const
{
    NiFixedString::SaveFixedStringAsCString(kStream, m_kGenericSemantic);
    NiFixedString::SaveFixedStringAsCString(kStream, m_kRendererSemantic);

    NiStreamSaveBinary(kStream, m_uiGenericSemanticIndex);
    NiStreamSaveBinary(kStream, m_uiRendererSemanticIndex);
    NiStreamSaveBinary(kStream, m_uiComponentCount);
    NiStreamSaveBinary(kStream, m_uiSharedRendererSemanticPosition);
}
//---------------------------------------------------------------------------
