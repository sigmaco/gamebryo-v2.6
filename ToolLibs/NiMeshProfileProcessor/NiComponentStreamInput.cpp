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

#include <NiSystem.h>
#include <NiDataStreamLock.h>
#include "NiComponentStreamInput.h"
#include "NiMeshProfileOperations.h"

// Helper lock
//---------------------------------------------------------------------------
class StreamElementHelperLock : public NiDataStreamLock
{
public:
    inline StreamElementHelperLock(NiDataStream* pkDataStream,
        size_t stElementOffset, NiUInt8 uiLockFlags) : NiDataStreamLock(
        pkDataStream, stElementOffset, uiLockFlags) {}

    inline NiUInt8* GetPointerToBufferAtIndex(NiUInt32 uiIndex)
    {
        NIASSERT(m_pkDataStream);
        return (NiUInt8*)((m_pcBuffer + m_stElementOffset) + 
            m_pkDataStream->GetStride() * uiIndex);
    }
};
//---------------------------------------------------------------------------
void NiComponentStreamInput::LockInput()
{
    if (!m_kStreamRef.IsValid())
        return;
    
    if (!m_pkLock)
    {
        NiDataStreamPtr pkStream = m_kStreamRef.GetDataStream();

        NiDataStreamElement kElement =
            pkStream->GetElementDescAt(0);

        m_pkLock = NiNew StreamElementHelperLock(pkStream,
            kElement.GetOffset(), NiDataStream::LOCK_TOOL_READ);
    }
}
//---------------------------------------------------------------------------
void NiComponentStreamInput::UnlockInput()
{
    if (!m_pkLock)
        return;

    NiDelete m_pkLock;
    m_pkLock = 0;
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::SetInput(
    NiDataStreamRef& kStreamRef, NiUInt8 uiComp)
{
    if (!NiMeshProfileOperations::IsValidStreamRef(kStreamRef))
        return false;

    NiDataStream* pkStream = kStreamRef.GetDataStream();
    NiDataStreamElement kElement = pkStream->GetElementDescAt(0);

    // Stream must not be custom nor packed
    if (kElement.IsCustom())
        return false;

    if (kElement.IsPacked())
        return false;

    // Ensure that the componet index is legal for the streamRef
    if (uiComp >= kElement.GetComponentCount())
        return false;

    m_uiComponentOffset = kElement.GetComponentSize() * uiComp;
    m_kStreamRef = kStreamRef;
    m_eElementType = kElement.GetType();
    return true;
}
//---------------------------------------------------------------------------
template<typename T> bool NiComponentStreamInput::GetTValue(
    NiUInt32 uiIndex, T& val)
{
   NIASSERT(m_kStreamRef.IsValid());
    if(!m_kStreamRef.IsValid())
        return false;

    NIASSERT(m_pkLock);
    NiUInt8* pkBuf = m_pkLock->GetPointerToBufferAtIndex(uiIndex);
    val = *(T*)(pkBuf + m_uiComponentOffset);
    return true;
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::AssignBasis(StreamBasis& kBasis) const
{
    const NiDataStream* pkDS = m_kStreamRef.GetDataStream();

    kBasis.m_uiAccessMask = pkDS->GetAccessMask();
    kBasis.m_eCloningBehavior = pkDS->GetCloningBehavior();
    kBasis.m_eUsage = pkDS->GetUsage(); 

    kBasis.m_kElement = pkDS->GetElementDescAt(0);

    kBasis.m_uiNumSubmeshes = (NiUInt16)m_kStreamRef.GetSubmeshRemapCount();

    kBasis.m_kRegionRefs.RemoveAll();
    for(NiUInt32 uiSubmesh = 0;uiSubmesh < m_kStreamRef.GetSubmeshRemapCount();
        uiSubmesh++)
    {
        kBasis.m_kRegionRefs.Add(m_kStreamRef.GetRegionIndexForSubmesh(
            uiSubmesh));
    }

    kBasis.m_kRegions.RemoveAll();
    for(NiUInt32 uiRegion = 0; uiRegion < pkDS->GetRegionCount(); uiRegion++)
    {
        kBasis.m_kRegions.Add(pkDS->GetRegion(uiRegion));
    }

    kBasis.m_uiCount = pkDS->GetSize() / pkDS->GetStride();

    kBasis.m_bIsPerInstance = m_kStreamRef.IsPerInstance();

    kBasis.m_bIsValid = true;

    return true;
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::IsCompatible(const StreamBasis& kBasis) const
{
    // Requirements
    //    All must have same accessmask
    //    All must have same cloning behavior
    //    All must have same usage
    //    All must have same basic type
    //    All must have same normalization
    //    All must have same regioncount on both refs and streams
    //    All must have same start and ranges within regions
    //    All must have same frequency and function

    const NiDataStream* pkDS = m_kStreamRef.GetDataStream();

    //if (kBasis.m_uiAccessMask != pkDS->GetAccessMask())
    //    return false;

    if (kBasis.m_eCloningBehavior != pkDS->GetCloningBehavior())
        return false;

    if (kBasis.m_eUsage != pkDS->GetUsage())
        return false;

    NiDataStreamElement kElem = pkDS->GetElementDescAt(0);
    if (kBasis.m_kElement.GetType() != kElem.GetType())
        return false;

    if (kBasis.m_kElement.IsNormalized() != kElem.IsNormalized())
        return false;

    if (kBasis.m_uiNumSubmeshes != m_kStreamRef.GetSubmeshRemapCount())
        return false;

    // Check region references
    for (NiUInt32 uiSubmesh=0; uiSubmesh < m_kStreamRef.GetSubmeshRemapCount();
        uiSubmesh++)
    {
        if (kBasis.m_kRegionRefs.GetAt(uiSubmesh) !=
            m_kStreamRef.GetRegionIndexForSubmesh(uiSubmesh))
        {
            return false;
        }
    }

    if (kBasis.m_kRegions.GetSize() != pkDS->GetRegionCount())
        return false;

    // Check regions
    for(NiUInt32 uiRegion = 0; uiRegion < pkDS->GetRegionCount(); uiRegion++)
    {
        if (!kBasis.m_kRegions.GetAt(uiRegion).IsEqual(
            pkDS->GetRegion(uiRegion)))
        {
            return false;
        }
    }

    if (kBasis.m_uiCount != (pkDS->GetSize() / pkDS->GetStride()))
        return false;

    if (kBasis.m_bIsPerInstance != m_kStreamRef.IsPerInstance())
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::GetValue(NiUInt32 uiIndex, float& val)
{
    NIASSERT(m_eElementType == NiDataStreamElement::T_FLOAT32);
    return GetTValue<float>(uiIndex, val);
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::GetValue(NiUInt32 uiIndex, NiFloat16& val)
{
    NIASSERT(m_eElementType == NiDataStreamElement::T_FLOAT16);
    return GetTValue<NiFloat16>(uiIndex, val);
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::GetValue(NiUInt32 uiIndex, NiUInt32& val)
{
    NIASSERT(m_eElementType == NiDataStreamElement::T_UINT32);
    return GetTValue<NiUInt32>(uiIndex, val);
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::GetValue(NiUInt32 uiIndex, NiUInt16& val)
{
    NIASSERT(m_eElementType == NiDataStreamElement::T_UINT16);
    return GetTValue<NiUInt16>(uiIndex, val);
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::GetValue(NiUInt32 uiIndex, NiUInt8& val)
{
    NIASSERT(m_eElementType == NiDataStreamElement::T_UINT8);
    return GetTValue<NiUInt8>(uiIndex, val);
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::GetValue(NiUInt32 uiIndex, NiInt32& val)
{
    NIASSERT(m_eElementType == NiDataStreamElement::T_INT32);
    return GetTValue<NiInt32>(uiIndex, val);
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::GetValue(NiUInt32 uiIndex, NiInt16& val)
{
    NIASSERT(m_eElementType == NiDataStreamElement::T_INT16);
    return GetTValue<NiInt16>(uiIndex, val);
}
//---------------------------------------------------------------------------
bool NiComponentStreamInput::GetValue(NiUInt32 uiIndex, NiInt8& val)
{
    NIASSERT(m_eElementType == NiDataStreamElement::T_INT8);
    return GetTValue<NiInt8>(uiIndex, val);
}
//---------------------------------------------------------------------------

