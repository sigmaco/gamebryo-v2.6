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
//  NiPoint2 inline functions

//---------------------------------------------------------------------------
inline NiPoint2::NiPoint2(float fX, float fY)
{
    x = fX;
    y = fY;
}
//---------------------------------------------------------------------------
inline bool NiPoint2::operator== (const NiPoint2& pt) const
{
    return (x == pt.x && y == pt.y);
}
//---------------------------------------------------------------------------
inline bool NiPoint2::operator!= (const NiPoint2& pt) const
{
    return !operator==(pt);
}
//---------------------------------------------------------------------------
inline NiPoint2 NiPoint2::operator+ (const NiPoint2& pt) const
{
    NiPoint2 result = *this;
    result.x += pt.x;
    result.y += pt.y;
    return result;
}
//---------------------------------------------------------------------------
inline NiPoint2 NiPoint2::operator- (const NiPoint2& pt) const
{
    NiPoint2 result = *this;
    result.x -= pt.x;
    result.y -= pt.y;
    return result;
}
//---------------------------------------------------------------------------
inline float NiPoint2::operator* (const NiPoint2& pt) const
{
    return x*pt.x+y*pt.y;
}
//---------------------------------------------------------------------------
inline NiPoint2 NiPoint2::operator* (float fScalar) const
{
    NiPoint2 result = *this;
    result.x *= fScalar;
    result.y *= fScalar;
    return result;
}
//---------------------------------------------------------------------------
inline NiPoint2 NiPoint2::operator/ (float fScalar) const
{
    NiPoint2 result = *this;
    result.x /= fScalar;
    result.y /= fScalar;
    return result;
}
//---------------------------------------------------------------------------
inline NiPoint2 NiPoint2::operator- () const
{
    return NiPoint2(-x,-y);
}
//---------------------------------------------------------------------------
inline NiPoint2 operator* (float fScalar, const NiPoint2& pt)
{
    return pt*fScalar;
}
//---------------------------------------------------------------------------
inline NiPoint2& NiPoint2::operator+= (const NiPoint2& pt)
{
    x += pt.x;
    y += pt.y;
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint2& NiPoint2::operator-= (const NiPoint2& pt)
{
    x -= pt.x;
    y -= pt.y;
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint2& NiPoint2::operator*= (float fScalar)
{
    x *= fScalar;
    y *= fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint2& NiPoint2::operator/= (float fScalar)
{
    x /= fScalar;
    y /= fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint2 NiPoint2::ComponentProduct(const NiPoint2& p0,
                                            const NiPoint2& p1)
{
    return NiPoint2(p0.x*p1.x,p0.y*p1.y);
}
//---------------------------------------------------------------------------
inline float NiPoint2::Length() const
{
    return NiSqrt(x*x+y*y);
}
//---------------------------------------------------------------------------
inline float NiPoint2::SqrLength() const
{
    return x*x+y*y;
}
//---------------------------------------------------------------------------
inline float NiPoint2::Dot(const NiPoint2& pt) const
{
    return x*pt.x+y*pt.y;
}
//---------------------------------------------------------------------------
inline float NiPoint2::Unitize()
{
    float fLength = Length();

    if ( fLength > 1e-06f )
    {
        float fRecip = 1.0f / fLength;
        x *= fRecip;
        y *= fRecip;
    }
    else
    {
        x = 0.0f;
        y = 0.0f;
        fLength = 0.0f;
    }

    return fLength;
}
#ifndef __SPU__
//---------------------------------------------------------------------------
inline void NiPoint2::LoadBinary(NiStream& stream, NiPoint2* pkValues, 
    unsigned int uiNumValues)
{
    NiStreamLoadBinary(stream, (float*)pkValues, uiNumValues*2);
}
//---------------------------------------------------------------------------
inline void NiPoint2::SaveBinary(NiStream& stream, NiPoint2* pkValues, 
    unsigned int uiNumValues)
{
    NiStreamSaveBinary(stream, (float*)pkValues, uiNumValues*2);
}
//---------------------------------------------------------------------------
#endif
