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
inline NiDeformablePointSet::NiDeformablePointSet(
    NiDeformablePoint::POINT_TYPE eType) :
    m_pkPoints(0),
    m_pkOriginalPointPool(0),
    m_uiMaxSize(0),
    m_uiCurrentSize(0),
    m_fGrowByPercent(0.1f),
    m_eType(eType),
    m_bDataIsValid(false)
{
}
//---------------------------------------------------------------------------
inline NiDeformablePointSet::~NiDeformablePointSet()
{    
    if (m_pkPoints)
    {
        m_pkOriginalPointPool = 0;
        NiFree(m_pkPoints);    
    }    
}
//---------------------------------------------------------------------------
inline NiDeformablePointSet* NiDeformablePointSet::Clone()
{
    NiDeformablePointSet* pkClone = 0;
    if (this->IsDataValid())
    {
        NiUInt32 uiNumPoints = this->GetNumPoints();
        pkClone = NiNew NiDeformablePointSet(this->GetPointType());
        pkClone->GrowTo(uiNumPoints);
        for (NiUInt32 ui = 0; ui < uiNumPoints; ui++)
        {
            NiDeformablePoint* pkPoint = this->GetAt(ui);
            pkClone->AddPoint(pkPoint->GetOriginalIndex(), 
                pkPoint->GetValue(), pkPoint->GetWorldLocation(),
                pkPoint->GetRadius(), pkPoint->GetSlopeAngle(),
                pkPoint->GetType(), true);
        }
        pkClone->ValidateData();

        pkClone->SetOriginalPointPool(this->GetOriginalPointPool());
    }
    
    return pkClone;
}
//---------------------------------------------------------------------------
inline NiDeformablePoint* NiDeformablePointSet::GetAt(NiUInt32 uiIndex) 
    const
{    
    if (uiIndex > m_uiCurrentSize || !m_bDataIsValid)     
        return NULL;
    
    return &m_pkPoints[uiIndex];
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDeformablePointSet::GetNumPoints() const
{
    return m_uiCurrentSize;
}
//---------------------------------------------------------------------------
inline void NiDeformablePointSet::SetPointType(
    NiDeformablePoint::POINT_TYPE eType)
{
    m_eType = eType;
}
//---------------------------------------------------------------------------
inline NiDeformablePoint::POINT_TYPE NiDeformablePointSet::GetPointType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline bool NiDeformablePointSet::IsDataValid() const
{
    return m_bDataIsValid;
}
//---------------------------------------------------------------------------
inline void NiDeformablePointSet::GrowTo(NiUInt32 uiSize)
{
    if (uiSize > m_uiMaxSize) 
    {
        m_pkPoints = (NiDeformablePoint*)NiRealloc(m_pkPoints, 
            uiSize * sizeof(NiDeformablePoint)
            );
        m_uiMaxSize = uiSize;

        // Initialize the type and original for safety
        for (NiUInt32 ui = m_uiCurrentSize; ui < m_uiMaxSize; ui++)
        {
            m_pkPoints[ui].SetOriginalIndex(0, m_eType);
        }
    }
}
//---------------------------------------------------------------------------
inline NiMemObject* NiDeformablePointSet::SetOriginalPointPool(
    NiMemObject* pkOriginalPointPool)
{
    if (m_pkOriginalPointPool == pkOriginalPointPool)
        return 0;

    NiMemObject* pkReturn = 0;
    switch (GetPointType())
    {
    case NiDeformablePoint::VERTEX:
        {
        NiTQuickReleaseObjectPool<NiTerrainVertex>* pkTempPool = 
            (NiTQuickReleaseObjectPool<NiTerrainVertex>*)
            (m_pkOriginalPointPool);
        if (pkTempPool)
        {            
            pkTempPool->DecRefCount();
            if (pkTempPool->GetRefCount() == 0)
            {
                pkReturn = m_pkOriginalPointPool;
            }
        }
        m_pkOriginalPointPool = pkOriginalPointPool;
        pkTempPool = 
            (NiTQuickReleaseObjectPool<NiTerrainVertex>*)
            (m_pkOriginalPointPool);

        if (pkTempPool)
            pkTempPool->IncRefCount();

        break;
        }
    case NiDeformablePoint::MASK_PIXEL:
        {
        NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>* pkTempPool = 
            (NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>*)
            (m_pkOriginalPointPool);
        if (pkTempPool)
        {            
            pkTempPool->DecRefCount();
            if (pkTempPool->GetRefCount() == 0)
            {
                pkReturn = m_pkOriginalPointPool;
            }
        }
        m_pkOriginalPointPool = pkOriginalPointPool;
        pkTempPool = (NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>*)
            (m_pkOriginalPointPool);

        if (pkTempPool)
            pkTempPool->IncRefCount();
        break;
        }
    default:
        NIASSERT(!"Invalid deformation type");  
        break;
    }

    return pkReturn;
}
//---------------------------------------------------------------------------
inline void NiDeformablePointSet::AddPoint(NiUInt32 uiPoolIndex, 
    float fValue, const NiPoint3& kWorldLocation, const float fRadius,
    float fSlope, NiDeformablePoint::POINT_TYPE eType, bool bActive)
{
    if (m_uiCurrentSize >= m_uiMaxSize) 
    {
        GrowTo(NiMax(m_uiMaxSize + (NiUInt32)(
            (1.0f + m_fGrowByPercent) * (float)m_uiMaxSize), 1));
    }

    NiDeformablePoint* pkPoint;

    pkPoint = &m_pkPoints[m_uiCurrentSize++];
    pkPoint->SetValue(fValue);
    pkPoint->SetWorldLocation(kWorldLocation);
    pkPoint->SetRadius(fRadius);
    pkPoint->SetSlopeAngle(fSlope);

    pkPoint->SetOriginalIndex(uiPoolIndex, eType);

    pkPoint->SetActive(bActive);
}
//---------------------------------------------------------------------------
inline void NiDeformablePointSet::SetGrowbyPercent(float fGrowBy)
{
    m_fGrowByPercent = NiMax(0.0f, NiMin(1.0f, fGrowBy));
}
//---------------------------------------------------------------------------
inline NiMemObject* NiDeformablePointSet::GetOriginalPointPool() const
{
    return m_pkOriginalPointPool;
}
//---------------------------------------------------------------------------
inline void NiDeformablePointSet::InvalidateData()
{
    m_bDataIsValid = false;
    m_uiCurrentSize = 0;
}
//---------------------------------------------------------------------------
inline void NiDeformablePointSet::ValidateData()
{
    m_bDataIsValid = (m_uiCurrentSize > 0 && m_uiMaxSize >= m_uiCurrentSize);
}
//---------------------------------------------------------------------------
