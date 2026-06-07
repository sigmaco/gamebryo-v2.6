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
inline NiDeformablePoint::NiDeformablePoint() :
    m_kWorldLocation(NiPoint3::ZERO),
    m_fRadius(0.0f),
    m_fValue(0.0f),
    m_uiOriginalIndex(0),
    m_eType(VERTEX),
    m_bHasChanged(false)
{
}
//---------------------------------------------------------------------------
inline void NiDeformablePoint::SetOriginalIndex(NiUInt32 uiIndex, 
    NiDeformablePoint::POINT_TYPE eType)
{
    m_uiOriginalIndex = uiIndex;
    m_eType = eType;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDeformablePoint::GetOriginalIndex() const
{
    return m_uiOriginalIndex;
}
//---------------------------------------------------------------------------
inline float NiDeformablePoint::GetValue() const
{
    return m_fValue;
}
//---------------------------------------------------------------------------
inline void NiDeformablePoint::SetValue(float fValue)
{
    m_fValue = fValue;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiDeformablePoint::GetWorldLocation() const
{
    return m_kWorldLocation;
}
//---------------------------------------------------------------------------
inline void NiDeformablePoint::SetWorldLocation(const NiPoint3& kWorldLoc)
{
    m_kWorldLocation = kWorldLoc;
}
//---------------------------------------------------------------------------
inline float NiDeformablePoint::GetSlopeAngle() const
{
    return m_fSlopeAngle;
}
//---------------------------------------------------------------------------
inline void NiDeformablePoint::SetSlopeAngle(float fSlope)
{
    m_fSlopeAngle = fSlope;
}
//---------------------------------------------------------------------------
inline float NiDeformablePoint::GetRadius() const
{
    return m_fRadius;
}
//---------------------------------------------------------------------------
inline void NiDeformablePoint::SetRadius(const float fRadius)
{
    m_fRadius = fRadius;
}
//---------------------------------------------------------------------------
inline NiDeformablePoint::POINT_TYPE NiDeformablePoint::GetType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline bool NiDeformablePoint::GetHasChanged() const
{
    return m_bHasChanged;
}
//---------------------------------------------------------------------------
inline void NiDeformablePoint::SetHasChanged(bool bVal)
{
    m_bHasChanged = bVal;
}
//---------------------------------------------------------------------------
inline bool NiDeformablePoint::IsActive() const
{
    return m_bActive;
}
//---------------------------------------------------------------------------
inline void NiDeformablePoint::SetActive(bool bVal)
{
    m_bActive = bVal;
}
