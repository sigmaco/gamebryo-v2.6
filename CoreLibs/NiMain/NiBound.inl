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
//  NiBound inline functions
#include "NiPlane.h"
//---------------------------------------------------------------------------
inline NiBound::NiBound()
{
}
//---------------------------------------------------------------------------
inline void NiBound::SetCenter(const NiPoint3& kCenter)
{
    m_kCenter = kCenter;
}
//---------------------------------------------------------------------------
inline void NiBound::SetCenter(float x, float y, float z)
{
    m_kCenter = NiPoint3(x, y, z);
}
//---------------------------------------------------------------------------
inline void NiBound::SetRadius(float fRadius)
{
    m_fRadius = fRadius;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiBound::GetCenter() const
{
    return m_kCenter;
}
//---------------------------------------------------------------------------
inline float NiBound::GetRadius() const
{
    return m_fRadius;
}
//---------------------------------------------------------------------------
inline void NiBound::SetCenterAndRadius(const NiPoint3& kCenter,
    float fRadius)
{
    m_kCenter = kCenter;
    m_fRadius = fRadius;
}
//---------------------------------------------------------------------------
inline bool NiBound::operator!=(const NiBound& kBound)
{
    return !(*this == kBound);
}
//---------------------------------------------------------------------------
inline int NiBound::WhichSide(const NiPlane& kPlane) const
{
    float fDistance = kPlane.Distance(m_kCenter);

    if (fDistance <= -m_fRadius)
        return NiPlane::NEGATIVE_SIDE;
    else if (fDistance >= m_fRadius)
        return NiPlane::POSITIVE_SIDE;
    else
        return NiPlane::NO_SIDE;
}
//---------------------------------------------------------------------------
inline void NiBound::Update(const NiBound& bound, const NiTransform& xform)
{
    // rigid motion does not change the bounding sphere radius
    m_kCenter = xform.m_fScale * (xform.m_Rotate * bound.m_kCenter) + 
        xform.m_Translate;
    m_fRadius = xform.m_fScale * bound.m_fRadius;
}
//---------------------------------------------------------------------------

