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

#include <NiDebug.h>

//---------------------------------------------------------------------------
inline NiPoint4::NiPoint4()
{
    /* */
}
//---------------------------------------------------------------------------
inline NiPoint4::NiPoint4(float fX, float fY, float fZ, float fW)
{
    Set(fX, fY, fZ, fW);
}
//---------------------------------------------------------------------------
inline NiPoint4::NiPoint4(const NiPoint3& kPt, float fW)
{
    Set(kPt, fW);
}
//---------------------------------------------------------------------------
inline NiPoint4::operator NiPoint3() 
{
    return NiPoint3(X(), Y(), Z());
}
//---------------------------------------------------------------------------
inline void NiPoint4::Set(const NiPoint3& kPt, float fW)
{
    Set(kPt.x, kPt.y, kPt.z, fW);
}
//---------------------------------------------------------------------------
inline void NiPoint4::Set3VectorW0(float fX, float fY, float fZ)
{
    Set(fX, fY, fZ, 0.0f);
}
//---------------------------------------------------------------------------
inline float NiPoint4::operator[](int i) const
{
    NIASSERT(i >= 0);
    NIASSERT(i < 4);
    return m_afPt[i];
}
//---------------------------------------------------------------------------
inline float NiPoint4::Set(unsigned int uiComponent, float fValue)
{
    NIASSERT(uiComponent < 4);
    m_afPt[uiComponent] = fValue;
    return fValue;
}
//---------------------------------------------------------------------------
inline float NiPoint4::X() const
{
    return (*this)[0];
}
//---------------------------------------------------------------------------
inline float NiPoint4::Y() const
{
    return (*this)[1];
}
//---------------------------------------------------------------------------
inline float NiPoint4::Z() const
{
    return (*this)[2];
}
//---------------------------------------------------------------------------
inline float NiPoint4::W() const
{
    return (*this)[3];
}
//---------------------------------------------------------------------------
inline void NiPoint4::SetX(float fX)
{
    m_afPt[0] = fX;
}
//---------------------------------------------------------------------------
inline void NiPoint4::SetY(float fY)
{
    m_afPt[1] = fY;
}
//---------------------------------------------------------------------------
inline void NiPoint4::SetZ(float fZ)
{
    m_afPt[2] = fZ;
}
//---------------------------------------------------------------------------
inline void NiPoint4::SetW(float fW)
{
    m_afPt[3] = fW;
}
//---------------------------------------------------------------------------
inline NiPoint4 NiPoint4::operator- () const
{
    return (*this) * -1;
}
//---------------------------------------------------------------------------
inline NiPoint4 NiPoint4::operator+ (const NiPoint4& kPt) const
{
    NiPoint4 kResult = *this;
    kResult += kPt;
    return kResult;
}
//---------------------------------------------------------------------------
inline NiPoint4 NiPoint4::operator- (const NiPoint4& kPt) const
{
    NiPoint4 kResult = *this;
    kResult -= kPt;
    return kResult;
}
//---------------------------------------------------------------------------
inline NiPoint4 NiPoint4::operator* (float fScalar) const
{
    NiPoint4 kResult = *this;
    kResult *= fScalar;
    return kResult;
}
//---------------------------------------------------------------------------
inline NiPoint4 NiPoint4::operator/ (float fScalar) const
{
    NiPoint4 kResult = *this;
    kResult /= fScalar;
    return kResult;
}
//---------------------------------------------------------------------------
inline bool NiPoint4::operator!=(const NiPoint4& kPt) const
{
    bool bEquals = (*this)==kPt;
    return !bEquals;
}
//---------------------------------------------------------------------------
inline void NiPoint4::CompressNormalizedIntoXY(const NiPoint3& point)
{
    NIASSERT(point.z > 0.0f);

    float factor = 1.0f / point.z;
    SetX(point.x * factor);
    SetY(point.y * factor);
}
//---------------------------------------------------------------------------
inline void NiPoint4::CompressNormalizedIntoZW(const NiPoint3& point)
{
    NIASSERT(point.z > 0.0f);

    float factor = 1.0f / point.z;
    SetZ(point.x * factor);
    SetW(point.y * factor);
}
//---------------------------------------------------------------------------
inline void NiPoint4::DecompressXYIntoNormalized(NiPoint3& point)
{
    point.x = X();
    point.y = Y();
    point.z = 1.0f;
    point.Unitize();
}
//---------------------------------------------------------------------------
inline void NiPoint4::DecompressHighDetailIntoNormalized(NiPoint3& point)
{
    if (W() == FLT_MAX)
    {
        point.x = X();
        point.y = Y();
        point.z = Z();
    }
    else
    {
        point.x = X();
        point.y = Y();
        point.z = 1.0f;
    }
    point.Unitize();
}
//---------------------------------------------------------------------------
inline void NiPoint4::DecompressZWIntoNormalized(NiPoint3& point)
{
    point.x = Z();
    point.y = W();
    point.z = 1.0f;
    point.Unitize();
}
//---------------------------------------------------------------------------
