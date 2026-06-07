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
inline NiAdditionalGeometryData::NiAGDDataStream::NiAGDDataStream() :
    m_uiType(AGD_NITYPE_INVALID), m_uiUnitSize(0),
    m_uiTotalSize(0), m_uiStride(0), m_uiBlockIndex(0), m_uiBlockOffset(0)
{
   m_uFlags = 0;
   SetConsistency(NiGeometryData::MUTABLE);
}
//---------------------------------------------------------------------------
inline NiAdditionalGeometryData::NiAGDDataBlock::NiAGDDataBlock() :
    m_uiDataBlockSize(0), m_pucDataBlock(0)
{
}
//---------------------------------------------------------------------------
inline void 
NiAdditionalGeometryData::NiAGDDataStream::SetConsistency(
    NiGeometryData::Consistency eConsistency)
{
    unsigned char ucConsistency = ConvertFromConsistency(eConsistency);
    m_uFlags = static_cast<unsigned char>(
        (m_uFlags & ~AGD_CONSISTENCY_MASK) | ucConsistency);
}
//---------------------------------------------------------------------------
inline NiGeometryData::Consistency 
NiAdditionalGeometryData::NiAGDDataStream::GetConsistency() const
{
    return ConvertToConsistency(static_cast<unsigned char>(m_uFlags & AGD_CONSISTENCY_MASK));
}
//---------------------------------------------------------------------------
inline void NiAdditionalGeometryData::NiAGDDataStream::SetKeep(
    bool bKeep)
{
   SetBit(bKeep, AGD_KEEP_MASK);
}
//---------------------------------------------------------------------------
inline bool NiAdditionalGeometryData::NiAGDDataStream::GetKeep() const
{
    return GetBit(AGD_KEEP_MASK);
}
//---------------------------------------------------------------------------
inline NiGeometryData::Consistency 
NiAdditionalGeometryData::NiAGDDataStream::ConvertToConsistency(
    unsigned char ucFlags) const
{
    switch (ucFlags)
    {
        case AGD_MUTABLE:
            return NiGeometryData::MUTABLE;
        case AGD_STATIC:
            return NiGeometryData::STATIC;
        case AGD_VOLATILE:
            return NiGeometryData::VOLATILE;
    }

    NIASSERT(!"Invalid consistency found!");
    return NiGeometryData::STATIC;
}
//---------------------------------------------------------------------------
inline unsigned char 
NiAdditionalGeometryData::NiAGDDataStream::ConvertFromConsistency(
    NiGeometryData::Consistency eConsistency) const
{
    switch (eConsistency)
    {
        case NiGeometryData::MUTABLE:
            return AGD_MUTABLE;
        case NiGeometryData::STATIC:
            return AGD_STATIC;
        case NiGeometryData::VOLATILE:
            return AGD_VOLATILE;
        default:
            NIASSERT(!"Invalid consistency found!");
            return AGD_STATIC;
    }
}
//---------------------------------------------------------------------------
inline void NiAdditionalGeometryData::IncRevisionID()
{
    m_uiRevID++;
}
//---------------------------------------------------------------------------
inline unsigned int NiAdditionalGeometryData::GetRevisionID() const
{
    return m_uiRevID;
}
//---------------------------------------------------------------------------
inline void NiAdditionalGeometryData::SetRevisionID(unsigned int uiRevID)
{
    m_uiRevID = uiRevID;
}
//---------------------------------------------------------------------------
inline unsigned short NiAdditionalGeometryData::GetVertexCount()
{
    return m_usVertexCount;
}
//---------------------------------------------------------------------------
inline NiAdditionalGeometryData::NiAdditionalGeometryData(
    unsigned short usVertexCount)
{
    m_uiRevID = 0;
    m_usVertexCount = usVertexCount;
    m_uiDataStreamCount = 0;
    m_pkDataStreamEntries = 0;
    m_aDataBlocks.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiAdditionalGeometryData::NiAdditionalGeometryData(
    unsigned short usVertexCount, unsigned int uiDataStreamCount)
{
    m_uiRevID = 0;
    m_usVertexCount = usVertexCount;
    m_uiDataStreamCount = uiDataStreamCount;
    m_pkDataStreamEntries = 
        NiNew NiAGDDataStream[uiDataStreamCount];
    m_aDataBlocks.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiAdditionalGeometryData::NiAdditionalGeometryData()
{
    m_uiRevID = 0;
    m_uiDataStreamCount = 0;
    m_pkDataStreamEntries = 0;
    m_usVertexCount = 0;
    m_aDataBlocks.RemoveAll();
}
//---------------------------------------------------------------------------
