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
template <typename ITER_TYPE, typename DATA_TYPE>
inline ITER_TYPE CreateIndexedIteratorHelper(DATA_TYPE* pIndices, 
    NiPrimitiveType::Type ePrimitiveType, bool bIsEnd)
{
    NiUInt32 uiPrimCount = 0;
    NiUInt32 uiPrimStep = 0;
    NiBool bReverseOddWindings = false;

    switch (ePrimitiveType)
    {
    case NiPrimitiveType::PRIMITIVE_LINES:      
        uiPrimCount = 2;
        uiPrimStep = 2;
        break;       

    case NiPrimitiveType::PRIMITIVE_LINESTRIPS:       
        uiPrimCount = 2;
        uiPrimStep = 1;
        break;
        
    case NiPrimitiveType::PRIMITIVE_TRISTRIPS:       
        uiPrimCount = 3;
        uiPrimStep = 1;
        bReverseOddWindings = true;
        break;        

    case NiPrimitiveType::PRIMITIVE_POINTS:
        uiPrimCount = 1;
        uiPrimStep = 1;
        break;        

    case NiPrimitiveType::PRIMITIVE_QUADS:       
        uiPrimCount = 4;
        uiPrimStep = 4;
        break;        

    case NiPrimitiveType::PRIMITIVE_TRIANGLES:        
        uiPrimCount = 3;
        uiPrimStep = 3;
        break;        

    default:
        NIASSERT(false && "Invalid primitive type on the mesh.");
    }    

    if (bIsEnd)
    {
        // If this is an end iterator, we need to adjust the end pointer
        // to point to the first invalid primitive.
        ITER_TYPE kResult(pIndices + (uiPrimStep-uiPrimCount), 
            uiPrimCount, uiPrimStep, bReverseOddWindings, false);
        return kResult;
    }
    
    ITER_TYPE kResult(pIndices, uiPrimCount, uiPrimStep, bReverseOddWindings, 
        false);
    return kResult;
}
//---------------------------------------------------------------------------
template <typename ITER_TYPE>
inline ITER_TYPE CreateNonIndexedIteratorHelper(
    NiPrimitiveType::Type ePrimitiveType,
    NiUInt32 uiCount = 0, bool bIsEnd = false)
{
    NiUInt32 uiPrimCount;
    NiUInt32 uiPrimStep;
    NiBool bReverseOddWindings = false;

    switch (ePrimitiveType)
    {
    case NiPrimitiveType::PRIMITIVE_LINES:
        uiPrimCount = 2;
        uiPrimStep = 2;
        break;       

    case NiPrimitiveType::PRIMITIVE_LINESTRIPS:
        uiPrimCount = 2;
        uiPrimStep = 1;
        break;

    case NiPrimitiveType::PRIMITIVE_TRISTRIPS:
        uiPrimCount = 3;
        uiPrimStep = 1;
        bReverseOddWindings = true;
        break;

    case NiPrimitiveType::PRIMITIVE_POINTS:
        uiPrimCount = 1;
        uiPrimStep = 1;
        break;        

    case NiPrimitiveType::PRIMITIVE_QUADS:
        uiPrimCount = 4;
        uiPrimStep = 4;
        break;        

    case NiPrimitiveType::PRIMITIVE_TRIANGLES:
        uiPrimCount = 3;
        uiPrimStep = 3;
        break;        

    default:
        uiPrimCount = 0;
        uiPrimStep = 0;
        NIASSERT(false && "Invalid primitive type on the mesh.");        
    }    

    if (bIsEnd)
        uiCount += (uiPrimStep - uiPrimCount);

    return ITER_TYPE(uiPrimCount, uiPrimStep,
        (NiBool)uiCount, bReverseOddWindings,
        false);
}
//---------------------------------------------------------------------------
inline NiDataStreamPrimitiveLock::~NiDataStreamPrimitiveLock()
{
    Unlock();
}
//---------------------------------------------------------------------------
inline NiPrimitiveType::Type 
NiDataStreamPrimitiveLock::GetPrimitiveType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline NiIndexedPrimitiveIterator32 
NiDataStreamPrimitiveLock::BeginIndexed32(NiUInt32 uiSubmeshIdx) const
{
    // We already are guaranteed to get an index buffer by the time this
    // method is called. So we only need to make sure the indices are 32-bit.
    NIASSERT(m_pkRef && m_pcBuffer);
    NIASSERT(m_pkRef->GetElementDescCount() == 1);
    NIASSERT(m_pkRef->GetElementDescAt(0).GetFormat() == 
        NiDataStreamElement::F_UINT32_1);
    NIASSERT(m_bIndexed);
    
    const NiDataStream::Region& kRegion = 
        m_pkRef->GetRegionForSubmesh(uiSubmeshIdx);

    NiUInt32* pIndices = (NiUInt32*)kRegion.ComputeStart(m_pcBuffer, 
        m_pkRef->GetStride(), m_kElement.GetOffset());

    return CreateIndexedIteratorHelper<NiIndexedPrimitiveIterator32, NiUInt32>(
        pIndices, GetPrimitiveType(), false);
}
//---------------------------------------------------------------------------
inline NiIndexedPrimitiveIterator32 
NiDataStreamPrimitiveLock::EndIndexed32(NiUInt32 uiSubmeshIdx) const
{
    // We already are guaranteed to get an index buffer by the time this
    // method is called. So we only need to make sure the indices are 32-bit.
    NIASSERT(m_pkRef && m_pcBuffer);
    NIASSERT(m_pkRef->GetElementDescCount() == 1);
    NIASSERT(m_pkRef->GetElementDescAt(0).GetFormat() == 
        NiDataStreamElement::F_UINT32_1);
    NIASSERT(m_bIndexed);
    
    const NiDataStream::Region& kRegion = 
        m_pkRef->GetRegionForSubmesh(uiSubmeshIdx);

    NiUInt32* pIndices = (NiUInt32*)kRegion.ComputeEnd(m_pcBuffer, 
        m_pkRef->GetStride(), m_kElement.GetOffset());

    return CreateIndexedIteratorHelper<NiIndexedPrimitiveIterator32, NiUInt32>(
        pIndices, GetPrimitiveType(), true);    
}
//---------------------------------------------------------------------------
inline NiIndexedPrimitiveIterator16 
NiDataStreamPrimitiveLock::BeginIndexed16(NiUInt32 uiSubmeshIdx) const
{
    // We already are guaranteed to get an index buffer by the time this
    // method is called. So we only need to make sure the indices are 32-bit.
    NIASSERT(m_pkRef && m_pcBuffer);
    NIASSERT(m_pkRef->GetElementDescCount() == 1);
    NIASSERT(m_pkRef->GetElementDescAt(0).GetFormat() == 
        NiDataStreamElement::F_UINT16_1);
    NIASSERT(m_bIndexed);
    
    const NiDataStream::Region& kRegion = 
        m_pkRef->GetRegionForSubmesh(uiSubmeshIdx);

    NiUInt16* pIndices = (NiUInt16*)kRegion.ComputeStart(m_pcBuffer, 
        m_pkRef->GetStride(), m_kElement.GetOffset());

    return CreateIndexedIteratorHelper<NiIndexedPrimitiveIterator16, NiUInt16>(
        pIndices, GetPrimitiveType(), false);  
}
//---------------------------------------------------------------------------
inline NiIndexedPrimitiveIterator16
NiDataStreamPrimitiveLock::EndIndexed16(NiUInt32 uiSubmeshIdx) const
{
    // We already are guaranteed to get an index buffer by the time this
    // method is called. So we only need to make sure the indices are 32-bit.
    NIASSERT(m_pkRef && m_pcBuffer);
    NIASSERT(m_pkRef->GetElementDescCount() == 1);
    NIASSERT(m_pkRef->GetElementDescAt(0).GetFormat() == 
        NiDataStreamElement::F_UINT16_1);
    NIASSERT(m_bIndexed);
    
    const NiDataStream::Region& kRegion = 
        m_pkRef->GetRegionForSubmesh(uiSubmeshIdx);

    NiUInt16* pIndices = (NiUInt16*)kRegion.ComputeEnd(m_pcBuffer, 
        m_pkRef->GetStride(), m_kElement.GetOffset());

    return CreateIndexedIteratorHelper<NiIndexedPrimitiveIterator16, NiUInt16>(
        pIndices, GetPrimitiveType(), true); 
}
//---------------------------------------------------------------------------
inline NiNonIndexedPrimitiveIterator32 
NiDataStreamPrimitiveLock::BeginNonIndexed32(NiUInt32) const
{
    // When there is no index buffer we can generate indices but we need to
    // make sure that this is in fact the case.
    NIASSERT(!m_bIndexed && !m_pcBuffer);
    return CreateNonIndexedIteratorHelper<NiNonIndexedPrimitiveIterator32>
        (GetPrimitiveType());
}
//---------------------------------------------------------------------------
inline NiNonIndexedPrimitiveIterator32 
NiDataStreamPrimitiveLock::EndNonIndexed32(NiUInt32 uiSubmeshIdx) const
{
    // When there is no index buffer we can generate indices but we need to
    // make sure that this is in fact the case.
    NIASSERT(!m_bIndexed && !m_pcBuffer);
    return CreateNonIndexedIteratorHelper<NiNonIndexedPrimitiveIterator32>
        (GetPrimitiveType(), ComputeNonIndexedCount(uiSubmeshIdx), true);
}
//---------------------------------------------------------------------------
inline NiNonIndexedPrimitiveIterator16
NiDataStreamPrimitiveLock::BeginNonIndexed16(NiUInt32) const
{
    // When there is no index buffer we can generate indices but we need to
    // make sure that this is in fact the case.
    NIASSERT(!m_bIndexed && !m_pcBuffer);
    return CreateNonIndexedIteratorHelper<NiNonIndexedPrimitiveIterator16>
        (GetPrimitiveType());
}
//---------------------------------------------------------------------------
inline NiNonIndexedPrimitiveIterator16
NiDataStreamPrimitiveLock::EndNonIndexed16(NiUInt32 uiSubmeshIdx) const
{
    // When there is no index buffer we can generate indices but we need to
    // make sure that this is in fact the case.
    NIASSERT(!m_bIndexed && !m_pcBuffer);
    return CreateNonIndexedIteratorHelper<NiNonIndexedPrimitiveIterator16>
        (GetPrimitiveType(), ComputeNonIndexedCount(uiSubmeshIdx), true);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStreamPrimitiveLock::count(NiUInt32 uiSubmeshIdx) const
{
    NiUInt32 uiElementCount = 0; 

    if (m_pkRef)
        uiElementCount = m_pkRef->GetCount(uiSubmeshIdx);
    else
        uiElementCount = ComputeNonIndexedCount(uiSubmeshIdx);

    switch (GetPrimitiveType())
    {
        case NiPrimitiveType::PRIMITIVE_LINES:
            return uiElementCount / 2;            

        case NiPrimitiveType::PRIMITIVE_LINESTRIPS:
            return uiElementCount - 1;

        case NiPrimitiveType::PRIMITIVE_TRISTRIPS:
            return uiElementCount - 2;
            
        case NiPrimitiveType::PRIMITIVE_POINTS:
            return uiElementCount;            

        case NiPrimitiveType::PRIMITIVE_QUADS:
            return uiElementCount / 4;

        case NiPrimitiveType::PRIMITIVE_TRIANGLES:
            return uiElementCount / 3;            

        default:
            NIASSERT(false && "Invalid primitive type on the mesh.");
            return 0;
    }    
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStreamPrimitiveLock::ComputeNonIndexedCount(
    NiUInt32 uiSubmeshIdx) const
{
    NIASSERT(!m_bIndexed && !m_pcBuffer);
    NIASSERT(m_pkRef);
    return m_pkRef->GetCount(uiSubmeshIdx);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStreamPrimitiveLock::GetSubmeshCount() const
{
    return m_uiSubmeshCount;
}
//---------------------------------------------------------------------------
inline bool NiDataStreamPrimitiveLock::IndexBufferExists() const
{
    return m_bIndexed;
}
//---------------------------------------------------------------------------
inline bool NiDataStreamPrimitiveLock::IndexBufferLocked() const
{
    if (!m_bIndexed || !m_pcBuffer)
        return false;

    return true;   
}
//---------------------------------------------------------------------------
inline bool NiDataStreamPrimitiveLock::Has16BitIndexBuffer() const
{
    if (m_bIndexed &&
        m_pkRef &&
        m_pkRef->GetElementDescCount() == 1 &&
        m_pkRef->GetElementDescAt(0).GetFormat() == 
            NiDataStreamElement::F_UINT16_1)
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
inline bool NiDataStreamPrimitiveLock::Has32BitIndexBuffer() const
{
    if (m_bIndexed &&
        m_pkRef &&
        m_pkRef->GetElementDescCount() == 1 &&
        m_pkRef->GetElementDescAt(0).GetFormat() == 
        NiDataStreamElement::F_UINT32_1)
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
inline void NiDataStreamPrimitiveLock::Unlock()
{
    if (m_pcBuffer && m_pkRef)
    {
        NiDataStream* pkDS = m_pkRef->GetDataStream();
        if (pkDS)
            pkDS->Unlock(m_uiLockType);

        m_pcBuffer = NULL;
        m_pkRef = NULL;
    }
}
//---------------------------------------------------------------------------
