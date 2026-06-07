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
//  NiRangeLODData inline functions

//---------------------------------------------------------------------------
inline void NiRangeLODData::SetCenter(const NiPoint3& kCenter)
{
    m_kCenter = kCenter;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiRangeLODData::GetCenter() const
{
    return m_kCenter;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiRangeLODData::GetWorldCenter() const
{
    return m_kWorldCenter;
}
//---------------------------------------------------------------------------
inline unsigned int NiRangeLODData::GetNumRanges() const
{
    return m_uiNumRanges;
}
//---------------------------------------------------------------------------
inline void NiRangeLODData::GetRange(unsigned int uiIndex, float& fNear,
    float& fFar) const
{
    NIASSERT(uiIndex < m_uiNumRanges);
    fNear = m_pkRanges[uiIndex].m_fNear;
    fFar = m_pkRanges[uiIndex].m_fFar;
}
//---------------------------------------------------------------------------
inline void NiRangeLODData::SetRange(unsigned int uiIndex, float fNear,
    float fFar)
{
    if(uiIndex >= m_uiNumRanges)
    {
        SetNumRanges(uiIndex + 1);
    }

    m_pkRanges[uiIndex].m_fNear = fNear;
    m_pkRanges[uiIndex].m_fFar = fFar;
}
//---------------------------------------------------------------------------
inline void NiRangeLODData::GetRangeSqr(unsigned int uiIndex, 
    float& fNearSqr, float& fFarSqr) const
{ 
    NIASSERT(uiIndex < m_uiNumRanges); 

    fNearSqr = m_pkRanges[uiIndex].m_fWorldNear * 
        m_pkRanges[uiIndex].m_fWorldNear; 

    fFarSqr = m_pkRanges[uiIndex].m_fWorldFar * 
        m_pkRanges[uiIndex].m_fWorldFar;   
}
//---------------------------------------------------------------------------
