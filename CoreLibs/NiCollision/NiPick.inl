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

//---------------------------------------------------------------------------
//  NiPick member functions
//---------------------------------------------------------------------------
inline NiPick::PickObjectPolicy* NiPick::GetPickObjectPolicy() const
{
    return m_spPickPolicy;
}
//---------------------------------------------------------------------------
inline void NiPick::SetPickObjectPolicy(NiPick::PickObjectPolicy* pkPolicy)
{
    if (pkPolicy == NULL) 
    {
        m_spPickPolicy = ms_spDefaultPickPolicy;
    }
    else
    {
        m_spPickPolicy = pkPolicy;
    }
}
//---------------------------------------------------------------------------
inline NiPick::PickObjectPolicy* NiPick::GetDefaultPickObjectPolicy()
{
    return ms_spDefaultPickPolicy;
}
//---------------------------------------------------------------------------
inline void NiPick::SetDefaultPickObjectPolicy(
    NiPick::PickObjectPolicy* pkPolicy)
{
    if (pkPolicy == NULL)
    {
        ms_spDefaultPickPolicy = NiNew NiPick::PickObjectPolicy();
    }
    else
    {
        ms_spDefaultPickPolicy = pkPolicy;
    }
}
//---------------------------------------------------------------------------
inline void NiPick::SetQueryType(QueryType eQueryType)
{
    m_eQueryType = eQueryType;
}
//---------------------------------------------------------------------------
inline NiPick::QueryType NiPick::GetQueryType() const
{
    return m_eQueryType;
}
//---------------------------------------------------------------------------
inline void NiPick::SetSortType(SortType eSortType)
{
    m_eSortType = eSortType;
}
//---------------------------------------------------------------------------
inline NiPick::SortType NiPick::GetSortType() const
{
    return m_eSortType;
}
//---------------------------------------------------------------------------
inline void NiPick::SetIntersectType(IntersectType eIntersectType)
{
    m_eIntersectType = eIntersectType;
}
//---------------------------------------------------------------------------
inline NiPick::IntersectType NiPick::GetIntersectType() const
{
    return m_eIntersectType;
}
//---------------------------------------------------------------------------
inline void NiPick::SetFallbackType(FallbackType eFallbackType)
{
    m_eFallbackType = eFallbackType;
}
//---------------------------------------------------------------------------
inline NiPick::FallbackType NiPick::GetFallbackType() const
{
    return m_eFallbackType;
}
//---------------------------------------------------------------------------
inline void NiPick::SetCoordinateType(CoordinateType eType)
{
    m_eCoordinateType = eType;
}
//---------------------------------------------------------------------------
inline NiPick::CoordinateType NiPick::GetCoordinateType() const
{
    return m_eCoordinateType;
}
//---------------------------------------------------------------------------
inline void NiPick::SetFrontOnly(bool bFrontOnly)
{
    m_bFrontOnly = bFrontOnly;
}
//---------------------------------------------------------------------------
inline bool NiPick::GetFrontOnly() const
{
    return m_bFrontOnly;
}
//---------------------------------------------------------------------------
inline void NiPick::SetObserveAppCullFlag(bool bObserveAppCullFlag)
{
    m_bObserveAppCullFlag = bObserveAppCullFlag;
}
//---------------------------------------------------------------------------
inline bool NiPick::GetObserveAppCullFlag() const
{
    return m_bObserveAppCullFlag;
}
//---------------------------------------------------------------------------
inline void NiPick::SetTarget(NiAVObject* pObject)
{
    m_spRoot = pObject;
}
//---------------------------------------------------------------------------
inline void NiPick::RemoveTarget()
{
    m_spRoot = 0;
}
//---------------------------------------------------------------------------
inline void NiPick::SetReturnTexture(bool bReturnTexture)
{
    m_bReturnTexture = bReturnTexture;
}
//---------------------------------------------------------------------------
inline bool NiPick::GetReturnTexture() const
{
    return m_bReturnTexture;
}
//---------------------------------------------------------------------------
inline void NiPick::SetReturnNormal(bool bReturnNormal)
{
    m_bReturnNormal = bReturnNormal;
}
//---------------------------------------------------------------------------
inline bool NiPick::GetReturnNormal() const
{
    return m_bReturnNormal;
}
//---------------------------------------------------------------------------
inline void NiPick::SetReturnSmoothNormal(bool bReturnSmoothNormal)
{
    m_bReturnSmoothNormal = bReturnSmoothNormal;
}
//---------------------------------------------------------------------------
inline bool NiPick::GetReturnSmoothNormal() const
{
    return m_bReturnSmoothNormal;
}
//---------------------------------------------------------------------------
inline void NiPick::SetReturnColor(bool bReturnColor)
{
    m_bReturnColor = bReturnColor;
}
//---------------------------------------------------------------------------
inline bool NiPick::GetReturnColor() const
{
    return m_bReturnColor;
}
//---------------------------------------------------------------------------
inline NiPick::Results& NiPick::GetResults()
{
    return m_pickResults;
}
//---------------------------------------------------------------------------
inline const NiPick::Results& NiPick::GetResults() const
{
    return m_pickResults;
}
//---------------------------------------------------------------------------
inline NiPick::Record* NiPick::Add(NiAVObject* pkObject)
{
    return m_pickResults.AddNew(pkObject);
}

//---------------------------------------------------------------------------
inline NiPick::Record* NiPick::GetLastAddedRecord()
{
    return m_pLastAddedRecord;
}
//---------------------------------------------------------------------------
inline unsigned int NiPick::GetSize()
{ 
    return m_pickResults.GetEffectiveSize();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiPick::Record member functions
//---------------------------------------------------------------------------
inline NiPick::Record::Record(NiAVObject* pObject)
{
    Initialize(pObject);
}
//---------------------------------------------------------------------------
inline NiAVObject* NiPick::Record::GetAVObject()
{
    return m_spObject;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::Initialize (NiAVObject* pObject)
{
    m_spObject = pObject;
    m_spProxyParent = 0;
    SetInvalidIndices();
}
//---------------------------------------------------------------------------
inline NiNode* NiPick::Record::GetParent()
{
    if ( m_spProxyParent )
        return m_spProxyParent;
    else
        return m_spObject->GetParent();
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetProxyParent(NiNode* pProxyParent)
{
    m_spProxyParent = pProxyParent;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetIntersection(const NiPoint3& intersect)
{
    m_intersect = intersect;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPick::Record::GetIntersection() const
{
    return m_intersect;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetDistance(float fDistance)
{
    m_fDistance = fDistance;
}
//---------------------------------------------------------------------------
inline float NiPick::Record::GetDistance() const
{
    return m_fDistance;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetTriangleIndex(unsigned int uiIndex)
{
    m_uiTriangleIndex = uiIndex;
}
//---------------------------------------------------------------------------
inline unsigned int NiPick::Record::GetTriangleIndex() const
{
    return m_uiTriangleIndex;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetVertexIndices(unsigned int uiIndex0,
    unsigned int uiIndex1, unsigned int uiIndex2)
{
    m_uiVertexIndex[0] = uiIndex0;
    m_uiVertexIndex[1] = uiIndex1;
    m_uiVertexIndex[2] = uiIndex2;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::GetVertexIndices(unsigned int& uiIndex0,
    unsigned int& uiIndex1, unsigned int& uiIndex2) const
{
    uiIndex0 = m_uiVertexIndex[0];
    uiIndex1 = m_uiVertexIndex[1];
    uiIndex2 = m_uiVertexIndex[2];
}
//---------------------------------------------------------------------------
inline bool NiPick::Record::GetIndicesValid() const
{
    return m_uiTriangleIndex != UINT_MAX ||
        (m_uiVertexIndex[0] != UINT_MAX &&
        m_uiVertexIndex[1] != UINT_MAX &&
        m_uiVertexIndex[2] != UINT_MAX);
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetInvalidIndices()
{
    m_uiTriangleIndex = UINT_MAX;
    m_uiVertexIndex[0] = UINT_MAX;
    m_uiVertexIndex[1] = UINT_MAX;
    m_uiVertexIndex[2] = UINT_MAX;
    m_uiSubmeshIndex = UINT_MAX;
}
//---------------------------------------------------------------------------
inline unsigned int NiPick::Record::GetSubmeshIndex() const
{
    return m_uiSubmeshIndex;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetSubmeshIndex(unsigned int uiSubmeshIdx)
{
    m_uiSubmeshIndex = uiSubmeshIdx;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetTexture(const NiPoint2& texture)
{
    m_texture = texture;
}
//---------------------------------------------------------------------------
inline const NiPoint2& NiPick::Record::GetTexture() const
{
    return m_texture;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetNormal(const NiPoint3& normal)
{
    m_normal = normal;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPick::Record::GetNormal() const
{
    return m_normal;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetColor(const NiColorA& color)
{
    m_color = color;
}
//---------------------------------------------------------------------------
inline const NiColorA& NiPick::Record::GetColor() const
{
    return m_color;
}
//---------------------------------------------------------------------------
inline void NiPick::Record::SetRayInsideABV (const bool bRayInsideABV)
{
    m_bRayInsideABV = bRayInsideABV;
}
//---------------------------------------------------------------------------
inline const bool NiPick::Record::GetRayInsideABV() const
{
    return m_bRayInsideABV;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiPick::Results member functions
//---------------------------------------------------------------------------
inline NiPick::Results::Results(unsigned int uiInitialSize, 
    unsigned int uiGrowBy) :
    NiTPrimitiveArray<Record*>(uiInitialSize, uiGrowBy),
    m_uiValidResults(0)
{
}
//---------------------------------------------------------------------------
inline NiPick::Results::~Results()
{
    for (unsigned int ui = 0; ui < NiTPrimitiveArray<Record*>::GetSize(); 
        ui++)
    {
        NiDelete NiTPrimitiveArray<Record*>::GetAt(ui);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiPick::Results::GetSize() const
{
    return m_uiValidResults;
}
//---------------------------------------------------------------------------
inline unsigned int NiPick::Results::GetEffectiveSize() const
{
    return m_uiValidResults;
}
//---------------------------------------------------------------------------
inline NiPick::Record* NiPick::Results::GetAt (unsigned int uiIndex) const
{
    return NiTPrimitiveArray<Record*>::GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiPick::Record* NiPick::Results::RemoveAtAndFill(unsigned int uiIndex)
{
    // calling remove on an invalid index?
    NIASSERT(m_uiValidResults && uiIndex < m_uiValidResults);

    // remove at uiIndex and fill from the last valid result (decrementing
    // the number of valid results)

    Record* pkRec = NiTPrimitiveArray<Record*>::RemoveAt(uiIndex);
    m_uiValidResults--;

    // Since array is packed tightly with valid records and then invalid
    // records, we need to do two swaps to maintain this condition.

    // swap last valid record with uiIndex
    NiTPrimitiveArray<Record*>::SetAt(uiIndex, GetAt(m_uiValidResults));
    // swap last invalid record with last record
    const unsigned int uiLast = NiTPrimitiveArray<Record*>::GetSize() - 1;
    NiTPrimitiveArray<Record*>::SetAt(m_uiValidResults,
        NiTPrimitiveArray<Record*>::GetAt(uiLast));
    // set last invalid record to zero
    NiTPrimitiveArray<Record*>::SetAt(uiLast, NULL);

    // decrement number of used array elements
    m_usSize--;

    return pkRec;
}
//---------------------------------------------------------------------------
inline void NiPick::Results::ClearArray()
{
    m_uiValidResults = 0;
}
//---------------------------------------------------------------------------
inline NiPick::Record* NiPick::Results::AddNew(NiAVObject* pObject)
{
    Record* pkRecord;
    if (m_uiValidResults < NiTPrimitiveArray<Record*>::GetSize())
    {
        // Previously allocated record.  Use it.
        pkRecord = NiTPrimitiveArray<Record*>::GetAt(m_uiValidResults);
        NIASSERT(pkRecord);
        pkRecord->Initialize(pObject);
    }
    else
    {
        // There doesn't exist any records for us to use.  Allocate a new
        // one and put it on the end of the array.
        pkRecord = NiNew NiPick::Record(pObject);
        NIASSERT(pkRecord);
        Add(pkRecord);
    }

    m_uiValidResults++;
    return pkRecord;
}
//---------------------------------------------------------------------------
