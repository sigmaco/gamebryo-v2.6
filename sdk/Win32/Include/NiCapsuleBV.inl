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
inline int NiCapsuleBV::Type() const
{
    return CAPSULE_BV;
}
//---------------------------------------------------------------------------
inline NiCapsuleBV::NiCapsuleBV(const NiCapsuleBV& kABV) :
    NiBoundingVolume()
{
    Copy(kABV);
}
//---------------------------------------------------------------------------
inline NiBoundingVolume* NiCapsuleBV::Create() const
{ 
    return NiNew NiCapsuleBV; 
}
//---------------------------------------------------------------------------
inline void NiCapsuleBV::SetCenter(const NiPoint3& kCenter)
{
    m_kCenter = kCenter;
    CalculateCapsule();
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiCapsuleBV::GetCenter() const
{
    return m_kCenter;
}
//---------------------------------------------------------------------------
inline void NiCapsuleBV::SetAxis(const NiPoint3& kAxis)
{
    m_kAxis = kAxis;
    CalculateCapsule();
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiCapsuleBV::GetAxis() const
{
    return m_kAxis;
}
//---------------------------------------------------------------------------
inline void NiCapsuleBV::SetExtent(const float fExtent)
{
    m_fExtent = fExtent;
    CalculateCapsule();
}
//---------------------------------------------------------------------------
inline float NiCapsuleBV::GetExtent() const
{
    return m_fExtent;
}
//---------------------------------------------------------------------------
inline void NiCapsuleBV::SetRadius(float fRadius)
{
    m_kCapsule.m_fRadius = fRadius;
    CalculateCapsule();
}
//---------------------------------------------------------------------------
inline float NiCapsuleBV::GetRadius() const
{
    return m_kCapsule.m_fRadius;
}
//---------------------------------------------------------------------------
inline const NiCapsule& NiCapsuleBV::GetCapsule() const
{
    return m_kCapsule;
}
//---------------------------------------------------------------------------
inline const NiSegment& NiCapsuleBV::GetSegment() const
{
    return m_kCapsule.m_kSegment;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiCapsuleBV::GetOrigin() const
{
    return m_kCapsule.m_kSegment.m_kOrigin;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiCapsuleBV::GetDirection() const
{
    return m_kCapsule.m_kSegment.m_kDirection;
}
//---------------------------------------------------------------------------
inline void NiCapsuleBV::CalculateCapsule()
{
    m_kCapsule.m_kSegment.m_kOrigin = 
        m_kCenter - m_kAxis * (m_fExtent - m_kCapsule.m_fRadius);
    m_kCapsule.m_kSegment.m_kDirection = 
        m_kAxis * (m_fExtent - m_kCapsule.m_fRadius) * 2.0f;
}
//---------------------------------------------------------------------------
