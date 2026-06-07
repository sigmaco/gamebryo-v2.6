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
//  NiPlane inline functions

//---------------------------------------------------------------------------
inline int NiPlane::WhichSide(const NiPoint3& kPoint) const
{
    float fDistance = m_kNormal.x * kPoint.x + 
                      m_kNormal.y * kPoint.y + 
                      m_kNormal.z * kPoint.z - m_fConstant;

    if (fDistance < 0.0f)
        return NiPlane::NEGATIVE_SIDE;
    else if (fDistance > 0.0f)
        return NiPlane::POSITIVE_SIDE;
    else
        return NiPlane::NO_SIDE;
}
//---------------------------------------------------------------------------
inline bool NiPlane::operator==(const NiPlane& kPlane)
{
    return (m_kNormal == kPlane.m_kNormal && m_fConstant == 
        kPlane.m_fConstant);
}
//---------------------------------------------------------------------------
inline bool NiPlane::operator!=(const NiPlane& kPlane)
{
    return !(*this == kPlane);
}
//---------------------------------------------------------------------------
inline void NiPlane::SetNormal(const NiPoint3& kNormal)
{
    m_kNormal = kNormal;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPlane::GetNormal() const 
{ 
    return m_kNormal;
}
//---------------------------------------------------------------------------
inline void NiPlane::SetConstant(float fConstant) 
{ 
    m_fConstant = fConstant; 
}
//---------------------------------------------------------------------------
inline float NiPlane::GetConstant() const 
{
    return m_fConstant; 
}
//---------------------------------------------------------------------------
inline float NiPlane::Distance(const NiPoint3& kPoint) const   
{
    return m_kNormal * kPoint - m_fConstant;     
}
//---------------------------------------------------------------------------
