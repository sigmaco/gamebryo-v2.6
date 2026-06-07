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
//  NiPoint4 inline functions

//---------------------------------------------------------------------------
inline void NiPoint4::MakeZero()
{
    Set(0.0f, 0.0f, 0.0f, 0.0f);
}
//---------------------------------------------------------------------------
inline void NiPoint4::Set(float fX, float fY, float fZ, float fW)
{
    SetX(fX);
    SetY(fY);
    SetZ(fZ);
    SetW(fW);
}
//---------------------------------------------------------------------------
inline void NiPoint4::Set3VectorW0(const NiPoint3& kPt)
{
    Set(kPt, 0.0f);
}
//---------------------------------------------------------------------------
inline NiPoint4 NiPoint4::operator+= (const NiPoint4& kPt)
{
    m_afPt[0] += kPt.m_afPt[0];
    m_afPt[1] += kPt.m_afPt[1];
    m_afPt[2] += kPt.m_afPt[2];
    m_afPt[3] += kPt.m_afPt[3];
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint4 NiPoint4::operator-= (const NiPoint4& kPt)
{
    m_afPt[0] -= kPt.m_afPt[0];
    m_afPt[1] -= kPt.m_afPt[1];
    m_afPt[2] -= kPt.m_afPt[2];
    m_afPt[3] -= kPt.m_afPt[3];
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint4 NiPoint4::operator*= (float fScalar)
{
    m_afPt[0] = m_afPt[0] * fScalar;
    m_afPt[1] = m_afPt[1] * fScalar;
    m_afPt[2] = m_afPt[2] * fScalar;
    m_afPt[3] = m_afPt[3] * fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint4 NiPoint4::operator/= (float fScalar)
{
    m_afPt[0] = m_afPt[0] / fScalar;
    m_afPt[1] = m_afPt[1] / fScalar;
    m_afPt[2] = m_afPt[2] / fScalar;
    m_afPt[3] = m_afPt[3] / fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline bool NiPoint4::operator== (const NiPoint4& kPt) const
{
    return 
        kPt.m_afPt[0] == m_afPt[0] &&
        kPt.m_afPt[1] == m_afPt[1] &&
        kPt.m_afPt[2] == m_afPt[2] &&
        kPt.m_afPt[3] == m_afPt[3];
}
//---------------------------------------------------------------------------
