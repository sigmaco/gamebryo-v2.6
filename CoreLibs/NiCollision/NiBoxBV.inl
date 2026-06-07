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
inline int NiBoxBV::Type() const
{
    return BOX_BV;
}
//---------------------------------------------------------------------------
inline NiBoxBV::NiBoxBV(const NiBoxBV& kABV) :
    NiBoundingVolume()
{
    SetBox(kABV.GetBox());
}
//---------------------------------------------------------------------------
inline NiBoundingVolume* NiBoxBV::Create() const 
{ 
    return NiNew NiBoxBV; 
}
//---------------------------------------------------------------------------
inline void NiBoxBV::SetBox(const NiBox& kBox)
{
    m_kBox = kBox;
}
//---------------------------------------------------------------------------
inline const NiBox& NiBoxBV::GetBox() const
{
    return m_kBox;
}
//---------------------------------------------------------------------------
inline void NiBoxBV::SetCenter(const NiPoint3& kCenter)
{
    m_kBox.m_kCenter = kCenter;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiBoxBV::GetCenter() const
{
    return m_kBox.m_kCenter;
}
//---------------------------------------------------------------------------
inline void NiBoxBV::SetAxis(int iWhich, const NiPoint3& kAxis)
{
    NIASSERT(0 <= iWhich && iWhich <= 2);
    m_kBox.m_akAxis[iWhich] = kAxis;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiBoxBV::GetAxis(int iWhich) const
{
    NIASSERT(0 <= iWhich && iWhich <= 2);
    return m_kBox.m_akAxis[iWhich];
}
//---------------------------------------------------------------------------
inline const NiPoint3* NiBoxBV::GetAxes() const
{
    return m_kBox.m_akAxis;
}
//---------------------------------------------------------------------------
inline void NiBoxBV::SetExtent(int iWhich, float fExtent)
{
    NIASSERT(0 <= iWhich && iWhich <= 2);
    m_kBox.m_afExtent[iWhich] = fExtent;
}
//---------------------------------------------------------------------------
inline float NiBoxBV::GetExtent(int iWhich) const
{
    NIASSERT(0 <= iWhich && iWhich <= 2);
    return m_kBox.m_afExtent[iWhich];
}
//---------------------------------------------------------------------------
inline const float* NiBoxBV::GetExtents() const
{
    return m_kBox.m_afExtent;
}
//---------------------------------------------------------------------------
