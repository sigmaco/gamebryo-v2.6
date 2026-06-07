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
inline NiInt32 NiMeshScreenElements::GetMaxPQuantity() const
{
    return (NiInt32)m_usMaxPQuantity;
}
//---------------------------------------------------------------------------
inline void NiMeshScreenElements::SetPGrowBy(NiInt32 iPGrowBy)
{
    m_usPGrowBy = (NiUInt16)(iPGrowBy > 0 ? iPGrowBy : 1);
}
//---------------------------------------------------------------------------
inline NiInt32 NiMeshScreenElements::GetPGrowBy() const
{
    return (NiInt32)m_usPGrowBy;
}
//---------------------------------------------------------------------------
inline NiInt32 NiMeshScreenElements::GetMaxVQuantity() const
{
    return (NiInt32)m_usMaxVQuantity;
}
//---------------------------------------------------------------------------
inline void NiMeshScreenElements::SetVGrowBy(NiInt32 iVGrowBy)
{
    m_usVGrowBy = (NiUInt16)(iVGrowBy > 0 ? iVGrowBy : 1);
}
//---------------------------------------------------------------------------
inline NiInt32 NiMeshScreenElements::GetVGrowBy() const
{
    return (NiInt32)m_usVGrowBy;
}
//---------------------------------------------------------------------------
inline NiInt32 NiMeshScreenElements::GetMaxTQuantity() const
{
    return (NiInt32)m_usMaxIQuantity/3;
}
//---------------------------------------------------------------------------
inline void NiMeshScreenElements::SetTGrowBy(NiInt32 iTGrowBy)
{
    m_usIGrowBy = (NiUInt16)(iTGrowBy > 0 ? 3 * iTGrowBy : 3);
}
//---------------------------------------------------------------------------
inline NiInt32 NiMeshScreenElements::GetTGrowBy() const
{
    return (NiInt32)m_usIGrowBy/3;
}
//---------------------------------------------------------------------------
inline NiInt32 NiMeshScreenElements::GetNumPolygons() const
{
    return (NiInt32)m_usPQuantity;
}
//---------------------------------------------------------------------------
inline bool NiMeshScreenElements::BoundNeedsUpdate() const
{
    return m_bBoundNeedsUpdate;
}
//---------------------------------------------------------------------------
inline NiMeshScreenElements::Polygon& NiMeshScreenElements::GetPolygon(
    NiInt32 iPolygon)
{
    return m_pPolygon[m_puiPIndexer[iPolygon]];
}
//---------------------------------------------------------------------------
inline const NiMeshScreenElements::Polygon& NiMeshScreenElements::GetPolygon(
    NiInt32 iPolygon) const
{
    return m_pPolygon[m_puiPIndexer[iPolygon]];
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshScreenElements::GetActiveVertexCount() const
{
    return GetVertexDataStream()->GetRegion(0).GetRange();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshScreenElements::GetActiveIndexCount() const
{
    const NiDataStream* pkStream = GetIndexDataStream();
    NIASSERT(pkStream->GetRegion(0).GetRange() % 3 == 0);
    return pkStream->GetRegion(0).GetRange();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshScreenElements::GetActiveTriangleCount() const
{
    return GetActiveIndexCount() / 3;
}
//---------------------------------------------------------------------------
inline bool NiMeshScreenElements::GetPolyVertexRange(NiUInt32 uiPolygon,
    NiUInt32& uiVertStart, NiUInt32& uiVertRange) const
{
    if (!IsValid(uiPolygon))
        return false;

    const Polygon& kPoly = GetPolygon(uiPolygon);
    if (kPoly.m_usNumVertices == 0)
        return false;

    uiVertStart = kPoly.m_usVOffset;
    uiVertRange = kPoly.m_usNumVertices;

    return true;
}
//---------------------------------------------------------------------------
inline bool NiMeshScreenElements::GetPolyIndexRange(NiUInt32 uiPolygon,
    NiUInt32& uiIndexStart, NiUInt32& uiIndexRange) const
{
    if (!IsValid(uiPolygon))
        return false;

    const Polygon& kPoly = GetPolygon(uiPolygon);
    if (kPoly.m_usNumVertices == 0)
        return false;

    uiIndexStart = kPoly.m_usIOffset;
    uiIndexRange = kPoly.m_usNumTriangles * 3;

    return true;
}
//---------------------------------------------------------------------------
inline NiDataStream* NiMeshScreenElements::GetVertexDataStream()
{
    NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    NIVERIFY(FindStreamRefAndElementBySemantic(
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
        pkRef, kElement));
    NIASSERT(GetSubmeshCount() == 1);
    NIASSERT(pkRef->GetRegionIndexForSubmesh(0) == 0);
    return pkRef->GetDataStream();
}
//---------------------------------------------------------------------------
inline const NiDataStream* NiMeshScreenElements::GetVertexDataStream() const
{
    const NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    NIVERIFY(FindStreamRefAndElementBySemantic(
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
        pkRef, kElement));
    NIASSERT(GetSubmeshCount() == 1);
    NIASSERT(pkRef->GetRegionIndexForSubmesh(0) == 0);
    return pkRef->GetDataStream();
}
//---------------------------------------------------------------------------
inline NiDataStream* NiMeshScreenElements::GetColorDataStream()
{
    NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    NIVERIFY(FindStreamRefAndElementBySemantic(NiCommonSemantics::COLOR(), 0,
        m_eColorFormat, pkRef, kElement));
    NIASSERT(GetSubmeshCount() == 1);
    NIASSERT(pkRef->GetRegionIndexForSubmesh(0) == 0);
    return pkRef->GetDataStream();
}
//---------------------------------------------------------------------------
inline const NiDataStream* NiMeshScreenElements::GetColorDataStream() const
{
    const NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    NIVERIFY(FindStreamRefAndElementBySemantic(NiCommonSemantics::COLOR(), 0,
        m_eColorFormat, pkRef, kElement));
    NIASSERT(GetSubmeshCount() == 1);
    NIASSERT(pkRef->GetRegionIndexForSubmesh(0) == 0);
    return pkRef->GetDataStream();
}
//---------------------------------------------------------------------------
inline NiDataStream* NiMeshScreenElements::GetNormalDataStream()
{
    NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    NIVERIFY(FindStreamRefAndElementBySemantic(NiCommonSemantics::NORMAL(), 0,
        NiDataStreamElement::F_FLOAT32_3, pkRef, kElement));
    NIASSERT(GetSubmeshCount() == 1);
    NIASSERT(pkRef->GetRegionIndexForSubmesh(0) == 0);
    return pkRef->GetDataStream();
}
//---------------------------------------------------------------------------
inline const NiDataStream* NiMeshScreenElements::GetNormalDataStream() const
{
    const NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    NIVERIFY(FindStreamRefAndElementBySemantic(NiCommonSemantics::NORMAL(), 0,
        NiDataStreamElement::F_FLOAT32_3, pkRef, kElement));
    NIASSERT(GetSubmeshCount() == 1);
    NIASSERT(pkRef->GetRegionIndexForSubmesh(0) == 0);
    return pkRef->GetDataStream();
}
//---------------------------------------------------------------------------
inline NiDataStream* NiMeshScreenElements::GetTexCoordDataStream(NiUInt32
    uiSet)
{
    NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    NIVERIFY(FindStreamRefAndElementBySemantic(NiCommonSemantics::TEXCOORD(),
        uiSet, NiDataStreamElement::F_FLOAT32_2, pkRef, kElement));
    NIASSERT(GetSubmeshCount() == 1);
    NIASSERT(pkRef->GetRegionIndexForSubmesh(0) == 0);
    return pkRef->GetDataStream();
}
//---------------------------------------------------------------------------
inline const NiDataStream* NiMeshScreenElements::GetTexCoordDataStream(
    NiUInt32 uiSet) const
{
    const NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    NIVERIFY(FindStreamRefAndElementBySemantic(NiCommonSemantics::TEXCOORD(),
        uiSet, NiDataStreamElement::F_FLOAT32_2, pkRef, kElement));
    NIASSERT(GetSubmeshCount() == 1);
    NIASSERT(pkRef->GetRegionIndexForSubmesh(0) == 0);
    return pkRef->GetDataStream();
}
//---------------------------------------------------------------------------
inline NiDataStream* NiMeshScreenElements::GetIndexDataStream()
{
    NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    NIVERIFY(FindStreamRefAndElementBySemantic(NiCommonSemantics::INDEX(), 0, 
        NiDataStreamElement::F_UINT16_1, pkRef, kElement));
    NIASSERT(GetSubmeshCount() == 1);
    NIASSERT(pkRef->GetRegionIndexForSubmesh(0) == 0);
    return pkRef->GetDataStream();
}
//---------------------------------------------------------------------------
inline const NiDataStream* NiMeshScreenElements::GetIndexDataStream() const
{
    const NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    NIVERIFY(FindStreamRefAndElementBySemantic(NiCommonSemantics::INDEX(), 0, 
        NiDataStreamElement::F_UINT16_1, pkRef, kElement));
    NIASSERT(GetSubmeshCount() == 1);
    NIASSERT(pkRef->GetRegionIndexForSubmesh(0) == 0);
    return pkRef->GetDataStream();
}
//---------------------------------------------------------------------------
inline NiDataStreamRef* NiMeshScreenElements::AddStream(
    const NiFixedString& kSemantic, NiUInt32 uiSemanticIndex,
    NiDataStreamElement::Format eFormat, NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, NiDataStream::Usage eUsage, 
    const void* pvSource, bool bForceToolDSCreate)
{   
    // Create the data stream
    NiDataStream* pkDataStream = 
        NiDataStream::CreateSingleElementDataStream(
        eFormat, uiCount, uiAccessMask, eUsage, pvSource, 
        true, bForceToolDSCreate);
    NIASSERT(pkDataStream);

    return AddStreamRef(pkDataStream, kSemantic, uiSemanticIndex);
}
//---------------------------------------------------------------------------
