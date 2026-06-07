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
inline NiHalfSpaceBV::NiHalfSpaceBV(const NiHalfSpaceBV& kABV) :
    NiBoundingVolume()
{
    SetPlane(kABV.GetPlane());
    m_kCenter = kABV.GetCenter();
}
//---------------------------------------------------------------------------
inline NiHalfSpaceBV::NiHalfSpaceBV(const NiPoint3& kPoint, 
    const NiPoint3& kNormal)
{
    m_kCenter = kPoint;
    NiPlane kPlane;
    kPlane.SetNormal(kNormal);
    kPlane.SetConstant(kNormal * kPoint);
    SetPlane(kPlane);
}
//---------------------------------------------------------------------------
inline void NiHalfSpaceBV::SetCenter(NiPoint3& kCenter)
{
    m_kCenter = kCenter;
    m_kPlane.SetConstant(m_kPlane.GetNormal() * kCenter);
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiHalfSpaceBV::GetCenter() const
{
    return m_kCenter;
}
//---------------------------------------------------------------------------
inline void NiHalfSpaceBV::SetNormal(NiPoint3& kNormal)
{
    // ensure the normal is unitized for collision testing
    NiPoint3 kTemp = kNormal;
    kTemp.Unitize();

    m_kPlane.SetNormal(kTemp);
    m_kPlane.SetConstant(kTemp * m_kCenter);
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiHalfSpaceBV::GetNormal() const
{
    return m_kPlane.GetNormal();
}
//---------------------------------------------------------------------------
inline int NiHalfSpaceBV::Type() const
{
    return HALFSPACE_BV;
}
//---------------------------------------------------------------------------
inline NiBoundingVolume* NiHalfSpaceBV::Create() const 
{ 
    return NiNew NiHalfSpaceBV; 
}
//---------------------------------------------------------------------------
inline const NiPlane& NiHalfSpaceBV::GetPlane() const
{
    return m_kPlane;
}
//---------------------------------------------------------------------------
