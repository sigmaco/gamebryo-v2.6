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

#include "NiMeshPCH.h"

#include <NiBool.h>
#include "NiDataStreamRef.h"

//--------------------------------------------------------------------------
NiDataStreamRef::NiDataStreamRef(const NiDataStreamRef& kOther) :
    m_bIsPerInstance(kOther.m_bIsPerInstance), 
    m_uiSubmeshToRegionIndexMapCount(kOther.m_uiSubmeshToRegionIndexMapCount)
{
    m_spDataStream = kOther.m_spDataStream;
    m_pkSemantics = NULL;

    // Copy over the semantics
    if (m_spDataStream)
    {
        m_pkSemantics = NiNew Semantic[m_spDataStream->GetElementDescCount()];
        for (NiUInt32 uj = 0; uj < m_spDataStream->GetElementDescCount(); uj++)
        {
            m_pkSemantics[uj] = kOther.m_pkSemantics[uj];
        }
    }

    // Copy the other ref's submesh to region map
    if (kOther.m_uiSubmeshToRegionIndexMapCount != 0)
    {
        m_puiSubmeshToRegionIndexMap = NiAlloc(NiUInt16, 
            kOther.m_uiSubmeshToRegionIndexMapCount);
    }
    else
    {
        m_puiSubmeshToRegionIndexMap = NULL;
    }
    
    for (NiUInt32 ui = 0; ui < kOther.m_uiSubmeshToRegionIndexMapCount; ui++)
    {
        m_puiSubmeshToRegionIndexMap[ui] = 
            kOther.m_puiSubmeshToRegionIndexMap[ui];
    }
    
}
//--------------------------------------------------------------------------
NiDataStreamRef& NiDataStreamRef::operator=(const NiDataStreamRef& kSrc)
{
    Reset();
    m_spDataStream = kSrc.m_spDataStream;

    m_pkSemantics = NULL;

    // Copy over the semantics
    if (m_spDataStream && m_spDataStream->GetElementDescCount())
    {
        m_pkSemantics = NiNew Semantic[m_spDataStream->GetElementDescCount()];
        for (NiUInt32 uj = 0; uj < m_spDataStream->GetElementDescCount(); uj++)
        {
            m_pkSemantics[uj] = kSrc.m_pkSemantics[uj];
        }
    }

    // Copy the other ref's submesh to region map
    m_uiSubmeshToRegionIndexMapCount = kSrc.m_uiSubmeshToRegionIndexMapCount;

    if (kSrc.m_uiSubmeshToRegionIndexMapCount != 0)
    {
        m_puiSubmeshToRegionIndexMap = NiAlloc(NiUInt16, 
            kSrc.m_uiSubmeshToRegionIndexMapCount);
    }
    else
    {
        m_puiSubmeshToRegionIndexMap = NULL;
    }

    for (NiUInt32 ui = 0; ui < kSrc.m_uiSubmeshToRegionIndexMapCount; ui++)
    {
        m_puiSubmeshToRegionIndexMap[ui] = 
            kSrc.m_puiSubmeshToRegionIndexMap[ui];
    }

    m_bIsPerInstance = kSrc.m_bIsPerInstance;

    return *this;
}
//--------------------------------------------------------------------------
void NiDataStreamRef::SetDataStream(NiDataStream* pkDataStream)
{
    // Because the element counts are specifically mapped to the datastream
    // NiDataStreamElement count, we need to be careful to stay in sync.
    if (pkDataStream == NULL)
    {
        NiDelete [] m_pkSemantics;
        m_pkSemantics = 0;
    }
    else if (pkDataStream == m_spDataStream)
    {
        // do nothing, we've already assigned these semantics
    }
    else
    {
        // If the counts are the same, we can skip the next step. Otherwise,
        // we need to copy over the exist values as long as they fit inside
        // the new semantic array

        NiUInt32 uiNewElementSetCount = pkDataStream->GetElementDescCount();
        NiUInt32 uiOldElementSetCount = 0;
        
        if (m_spDataStream)
            uiOldElementSetCount = m_spDataStream->GetElementDescCount();

        if (uiNewElementSetCount && 
            uiNewElementSetCount != uiOldElementSetCount)
        {
            Semantic* pkNewSemantics = NiNew Semantic[uiNewElementSetCount];

            for (NiUInt32 ui = 0; ui < uiOldElementSetCount && 
                ui < uiNewElementSetCount; ui++)
            {
                pkNewSemantics[ui] = m_pkSemantics[ui];
            }

            NiDelete [] m_pkSemantics;
            m_pkSemantics = pkNewSemantics;
        }
    }

    m_spDataStream = pkDataStream;
}
//--------------------------------------------------------------------------
NiUInt32 NiDataStreamRef::GetUniqueCount() const
{
    NIASSERT(m_uiSubmeshToRegionIndexMapCount > 0);
    NIASSERT(m_puiSubmeshToRegionIndexMap);

    NiUInt32 uiUniqueCount = 0;
    for (NiUInt32 ui = 0; ui < m_uiSubmeshToRegionIndexMapCount; ui++)
    {
        NiUInt16 uiRemapIdx = m_puiSubmeshToRegionIndexMap[ui];
        bool bFound = false;
        for (NiUInt32 uiPrior = 0; uiPrior < ui; uiPrior++)
        {
            if (uiRemapIdx == m_puiSubmeshToRegionIndexMap[uiPrior])
            {
                bFound = true;
                continue;
            }
        }

        if (!bFound)
        {
            uiUniqueCount += m_spDataStream->GetRegion(uiRemapIdx).GetRange();
        }
    }

    return uiUniqueCount;
}
//--------------------------------------------------------------------------
void NiDataStreamRef::BindRegionToSubmesh(NiUInt32 uiSubmeshIdx, 
    const NiDataStream::Region& kRegion)
{
    if (m_uiSubmeshToRegionIndexMapCount <= uiSubmeshIdx)
        SetSubmeshCount(uiSubmeshIdx + 1);

    NIASSERT(m_spDataStream);
    NiUInt32 uiRegionIdx = UINT_MAX;
    for (NiUInt32 ui = 0; ui < m_spDataStream->GetRegionCount(); ui++)
    {
        NiDataStream::Region& kTestRegion = m_spDataStream->GetRegion(ui);
        if (kTestRegion.IsEqual(kRegion))
        {
            uiRegionIdx = ui;
            continue;
        }
    }

    if (uiRegionIdx == UINT_MAX)
    {
        uiRegionIdx = m_spDataStream->AddRegion(kRegion);
    }

    m_puiSubmeshToRegionIndexMap[uiSubmeshIdx] = (NiUInt16) uiRegionIdx;
}
//--------------------------------------------------------------------------
NiUInt32 NiDataStreamRef::FindElementDescIndex(
    const NiFixedString& kSemantic, NiUInt32 uiSemanticIdx) const
{
    NiUInt32 uiElementDescCount = GetElementDescCount();
    for (NiUInt32 ui = 0; ui < uiElementDescCount; ui++)
    {
        if (m_pkSemantics[ui].m_kName == kSemantic &&
            m_pkSemantics[ui].m_uiIndex == uiSemanticIdx)
        {
            return ui;
        }
    }

    return UINT_MAX;
}
//--------------------------------------------------------------------------
NiUInt32 NiDataStreamRef::GetSemanticCount(
    const NiFixedString& kSemantic) const
{
    NiUInt32 uiReturn = 0;
    NiUInt32 uiElementDescCount = GetElementDescCount();
    for (NiUInt32 ui = 0; ui < uiElementDescCount; ui++)
    {
        if (m_pkSemantics[ui].m_kName == kSemantic)
        {
            ++uiReturn;
        }
    }
    return uiReturn;
}
//--------------------------------------------------------------------------
void NiDataStreamRef::SetSubmeshCount(NiUInt32 uiSubmeshCount, 
    NiUInt16 uiRemainderFill)
{
    NiUInt32 uiPriorCount = m_uiSubmeshToRegionIndexMapCount;
    if (uiSubmeshCount != uiPriorCount)
    {
        m_uiSubmeshToRegionIndexMapCount = (NiUInt16) uiSubmeshCount;
        m_puiSubmeshToRegionIndexMap = (NiUInt16*)
            NiRealloc(m_puiSubmeshToRegionIndexMap, 
            sizeof(NiUInt16)*m_uiSubmeshToRegionIndexMapCount);
        NIASSERT(m_puiSubmeshToRegionIndexMap);
    }

    for (NiUInt32 ui = uiPriorCount; ui < uiSubmeshCount; ui++)
    {
        m_puiSubmeshToRegionIndexMap[ui] = uiRemainderFill;
    }
}
//--------------------------------------------------------------------------
bool NiDataStreamRef::IsEqual(const NiDataStreamRef* pkOther) const
{
    if (pkOther == NULL)
        return false;

    // compare instancing flags
    if (m_bIsPerInstance != pkOther->m_bIsPerInstance)
        return false;

    // Compare datastreams
    if (m_spDataStream != pkOther->m_spDataStream)
    {
        if (m_spDataStream == NULL || pkOther->m_spDataStream == NULL)
            return false;

        if (!m_spDataStream->IsEqual(pkOther->m_spDataStream))
            return false;
    }

    // compare submesh to region maps
    if (m_uiSubmeshToRegionIndexMapCount != 
        pkOther->m_uiSubmeshToRegionIndexMapCount)
    {
        return false;
    }

    for (NiUInt32 ui = 0; ui < m_uiSubmeshToRegionIndexMapCount; ui++)
    {
        if (m_puiSubmeshToRegionIndexMap[ui] != 
            pkOther->m_puiSubmeshToRegionIndexMap[ui])
        {
            return false;
        }
    }

    // compare semantics
    if (m_spDataStream != NULL && pkOther->m_spDataStream != NULL)
    {
        if (m_spDataStream->GetElementDescCount() !=
            pkOther->m_spDataStream->GetElementDescCount())
        {
            return false;
        }

        NiUInt32 uiElementDescCount = m_spDataStream->GetElementDescCount();
        for (NiUInt32 uj = 0; uj < uiElementDescCount; uj++)
        {
            if (m_pkSemantics[uj].m_kName != 
                pkOther->m_pkSemantics[uj].m_kName)
            {
                return false;
            }

            if (m_pkSemantics[uj].m_uiIndex != 
                pkOther->m_pkSemantics[uj].m_uiIndex)
            {
                return false;
            }
        }
    }
    else if (m_spDataStream == NULL && pkOther->m_spDataStream == NULL)
    {
        // Do nothing.
    }
    else
    {
        return false;
    }

    return true;
}
//--------------------------------------------------------------------------
bool NiDataStreamRef::IsValid(NiString* pkInvalidDescription) const
{
    // Check whether a data stream exists.
    if (m_spDataStream == NULL)
    {
        if (pkInvalidDescription)
        {
            pkInvalidDescription->Format(
                "m_spDataStream pointer NULL.");
        }
        return false;
    }

    // Check whether there is at least one entry in the submesh-to-region map.
    NiUInt32 uiDSSubmeshCount = GetSubmeshRemapCount();
    if (uiDSSubmeshCount == 0)
    {
        if (pkInvalidDescription)
        {
            pkInvalidDescription->Format(
                "Submesh count is zero.");
        }
        return false;
    }

    // Iterate over each entry in the submesh-to-region map and
    // check whether it references a valid region.
    NiUInt32 uiDSRegionCount = m_spDataStream->GetRegionCount();
    for (NiUInt32 uiDSSubmesh = 0;
        uiDSSubmesh < uiDSSubmeshCount;
        uiDSSubmesh++)
    {
        if (GetRegionIndexForSubmesh(uiDSSubmesh) >= uiDSRegionCount)
        {
            if (pkInvalidDescription)
            {
                pkInvalidDescription->Format(
                    "SubmeshToRegionIndexMap for submesh %d returned %d, "
                    "which is larger than the maximum valid region %d.",
                    uiDSSubmesh, 
                    GetRegionIndexForSubmesh(uiDSSubmesh),
                    uiDSRegionCount);
            }
            return false;
        }
    }

    return true;
}
//--------------------------------------------------------------------------
void NiDataStreamRef::LoadBinary(NiStream& kStream)
{
    m_spDataStream = (NiDataStream*)kStream.ResolveLinkID();

    NiBool kPerInstance;
    NiStreamLoadBinary(kStream, kPerInstance);
    m_bIsPerInstance = NIBOOL_IS_TRUE(kPerInstance);

    // Load the submesh to region index map
    NiStreamLoadBinary(kStream, m_uiSubmeshToRegionIndexMapCount);
    m_puiSubmeshToRegionIndexMap = NiAlloc(NiUInt16,
        m_uiSubmeshToRegionIndexMapCount);

    for (NiUInt32 ui = 0; ui < m_uiSubmeshToRegionIndexMapCount; ui++)
    {
        NiStreamLoadBinary(kStream, m_puiSubmeshToRegionIndexMap[ui]);
    }
    
    // Load the semantics
    NiUInt32 uiElementDescCount = 0;
    NiStreamLoadBinary(kStream, uiElementDescCount);
    m_pkSemantics = NiNew Semantic[uiElementDescCount];
    for (NiUInt32 ui = 0; ui < uiElementDescCount; ui++)
    {
        kStream.LoadFixedString(m_pkSemantics[ui].m_kName);
        NiStreamLoadBinary(kStream, m_pkSemantics[ui].m_uiIndex);
    }
}
//--------------------------------------------------------------------------
void NiDataStreamRef::SaveBinary(NiStream& kStream)
{
    // If you hit this assert, you need to check GetStreamable() 
    // before streaming out this ref.
    NIASSERT(m_spDataStream->GetStreamable() == true);

    // Save data stream w/ LinkID
    m_spDataStream->SaveLinkID(kStream);

    NiBool kPerInstance = m_bIsPerInstance;
    NiStreamSaveBinary(kStream, kPerInstance);

    // Save the submesh to region index map
    NiStreamSaveBinary(kStream, m_uiSubmeshToRegionIndexMapCount);
    for (NiUInt32 ui = 0; ui < m_uiSubmeshToRegionIndexMapCount; ui++)
    {
        NiStreamSaveBinary(kStream, m_puiSubmeshToRegionIndexMap[ui]);
    }
    
    // Save the semantics
    NiUInt32 uiElementDescCount = GetElementDescCount();
    NiStreamSaveBinary(kStream, uiElementDescCount);
    for (NiUInt32 ui = 0; ui < uiElementDescCount; ui++)
    {
        kStream.SaveFixedString(m_pkSemantics[ui].m_kName);
        NiStreamSaveBinary(kStream, m_pkSemantics[ui].m_uiIndex);
    }
}
//--------------------------------------------------------------------------
bool NiDataStreamRef::RegisterStreamables(NiStream& kStream)
{
    if (m_spDataStream == NULL)
        return true;

    if (m_spDataStream->GetStreamable() == false)
        return true;

    m_spDataStream->RegisterStreamables(kStream);

    NiUInt32 uiElementDescCount = m_spDataStream->GetElementDescCount();
    for (NiUInt32 uj = 0; uj < uiElementDescCount; uj++)
    {
        kStream.RegisterFixedString(m_pkSemantics[uj].m_kName);
    }

    return true;
}
//--------------------------------------------------------------------------
NiInt32 NiDataStreamRef::GetMaxIndexBySemantic(
    const NiFixedString& kSemantic) const
{
    NiInt32 iReturn = -1;
    NiUInt32 uiElementDescCount = m_spDataStream->GetElementDescCount();
    for (NiUInt32 uj = 0; uj < uiElementDescCount; uj++)
    {
        if (kSemantic == m_pkSemantics[uj].m_kName)
        {
            if (iReturn < (NiInt32)m_pkSemantics[uj].m_uiIndex)
                iReturn = (NiInt32)m_pkSemantics[uj].m_uiIndex;
        }
    }

    return iReturn;
}
//--------------------------------------------------------------------------
