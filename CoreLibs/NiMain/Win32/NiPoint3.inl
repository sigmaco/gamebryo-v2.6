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
//  NiPoint3 inline functions

//---------------------------------------------------------------------------
// The following macros are supplied only because the Visual C++ optimizer
// does a poor job in many situations of inlining the point operations.  The
// input parameters should be single identifiers (to avoid side effects by
// the textual substitution).

// result = pt0+pt1
#define NiPointPlusPoint(result,pt0,pt1) \
{ \
    result.x = (pt0).x + (pt1).x; \
    result.y = (pt0).y + (pt1).y; \
    result.z = (pt0).z + (pt1).z; \
}

// result = pt0-pt1
#define NiPointMinusPoint(result,pt0,pt1) \
{ \
    result.x = (pt0).x - (pt1).x; \
    result.y = (pt0).y - (pt1).y; \
    result.z = (pt0).z - (pt1).z; \
}

// result = Dot(pt0,pt1)
#define NiPointDotPoint(result,pt0,pt1) \
{ \
    result = (pt0).x * (pt1).x + (pt0).y * (pt1).y + (pt0).z * (pt1).z; \
}

// result = s*pt
#define NiScalarTimesPoint(result,s,pt) \
{ \
    result.x = s * (pt).x; \
    result.y = s * (pt).y; \
    result.z = s * (pt).z; \
}

// result = s*pt0+pt1
#define NiScalarTimesPointPlusPoint(result,s,pt0,pt1) \
{ \
    result.x = s * (pt0).x + (pt1).x; \
    result.y = s * (pt0).y + (pt1).y; \
    result.z = s * (pt0).z + (pt1).z; \
}

// result = s0*pt0+s1*pt1
#define NiScalarTimesPointPlusScalarTimesPoint(result,s0,pt0,s1,pt1) \
{ \
    result.x = s0 * (pt0).x + s1 * (pt1).x; \
    result.y = s0 * (pt0).y + s1 * (pt1).y; \
    result.z = s0 * (pt0).z + s1 * (pt1).z; \
}


//---------------------------------------------------------------------------
inline NiPoint3::NiPoint3()
{
    /* */
}
//---------------------------------------------------------------------------
inline NiPoint3::NiPoint3(float fX, float fY, float fZ)
{
    x = fX;
    y = fY;
    z = fZ;
}
//---------------------------------------------------------------------------
inline const float& NiPoint3::operator[] (int i) const
{
    // warning:  safe for 4-byte aligned class member data
    const float* base = &x;
    return (float&) base[i];
}
//---------------------------------------------------------------------------
inline float& NiPoint3::operator[] (int i)
{
    // warning:  safe for 4-byte aligned class member data
    float* base = &x;
    return (float&) base[i];
}
//---------------------------------------------------------------------------
inline bool NiPoint3::operator== (const NiPoint3& kPt) const
{
    return (x == kPt.x && y == kPt.y && z == kPt.z);
}
//---------------------------------------------------------------------------
inline bool NiPoint3::operator!= (const NiPoint3& pt) const
{
    return !(*this == pt);
}
//---------------------------------------------------------------------------
inline NiPoint3 NiPoint3::operator+ (const NiPoint3& kPt) const
{
    return NiPoint3(x+kPt.x,y+kPt.y,z+kPt.z);
}
//---------------------------------------------------------------------------
inline NiPoint3 NiPoint3::operator- (const NiPoint3& kPt) const
{
    return NiPoint3(x-kPt.x,y-kPt.y,z-kPt.z);
}
//---------------------------------------------------------------------------
inline float NiPoint3::operator* (const NiPoint3& kPt) const
{
    return x*kPt.x+y*kPt.y+z*kPt.z;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiPoint3::operator* (float fScalar) const
{
    return NiPoint3(fScalar*x,fScalar*y,fScalar*z);
}
//---------------------------------------------------------------------------
inline NiPoint3 NiPoint3::operator/ (float fScalar) const
{
    float fInvScalar = 1.0f/fScalar;
    return NiPoint3(fInvScalar*x,fInvScalar*y,fInvScalar*z);
}
//---------------------------------------------------------------------------
inline NiPoint3 NiPoint3::operator- () const
{
    return NiPoint3(-x,-y,-z);
}
//---------------------------------------------------------------------------
inline NiPoint3 operator* (float fScalar, const NiPoint3& kPt)
{
    return NiPoint3(fScalar*kPt.x,fScalar*kPt.y,fScalar*kPt.z);
}
//---------------------------------------------------------------------------
inline NiPoint3& NiPoint3::operator+= (const NiPoint3& kPt)
{
    x += kPt.x;
    y += kPt.y;
    z += kPt.z;
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint3& NiPoint3::operator-= (const NiPoint3& kPt)
{
    x -= kPt.x;
    y -= kPt.y;
    z -= kPt.z;
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint3& NiPoint3::operator*= (float fScalar)
{
    x *= fScalar;
    y *= fScalar;
    z *= fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint3& NiPoint3::operator/= (float fScalar)
{
    float fInvScalar = 1.0f/fScalar;
    x *= fInvScalar;
    y *= fInvScalar;
    z *= fInvScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiPoint3::ComponentProduct(const NiPoint3& kP0,
                                            const NiPoint3& kP1)
{
    return NiPoint3(kP0.x*kP1.x,kP0.y*kP1.y,kP0.z*kP1.z);
}
//---------------------------------------------------------------------------
inline float NiPoint3::Length() const
{
    return NiSqrt(x*x+y*y+z*z);
}
//---------------------------------------------------------------------------
inline float NiPoint3::SqrLength() const
{
    return x*x+y*y+z*z;
}
//---------------------------------------------------------------------------
inline float NiPoint3::Dot(const NiPoint3& kPt) const
{
    return x*kPt.x+y*kPt.y+z*kPt.z;
}
//---------------------------------------------------------------------------
inline float NiPoint3::Unitize()
{
    float fLength = Length();

    if ( fLength > 1e-06f )
    {
        float fRecip = 1.0f/fLength;
        x *= fRecip;
        y *= fRecip;
        z *= fRecip;
    }
    else
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        fLength = 0.0f;
    }

    return fLength;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiPoint3::Cross(const NiPoint3& kPt) const
{
    return NiPoint3(y*kPt.z-z*kPt.y,z*kPt.x-x*kPt.z,x*kPt.y-y*kPt.x);
}
//---------------------------------------------------------------------------
inline NiPoint3 NiPoint3::UnitCross(const NiPoint3& kPt) const
{
    NiPoint3 cross(y*kPt.z-z*kPt.y,z*kPt.x-x*kPt.z,x*kPt.y-y*kPt.x);
    float fLength = cross.Length();
    if ( fLength > 1e-06f )
        return cross/fLength;
    else
        return NiPoint3(0.0f,0.0f,0.0f);
}
//---------------------------------------------------------------------------
// This algorithm was published as "A High Speed, Low Precision Square Root",
// by Paul Lalonde and Robert Dawon, Dalhousie University, Halifax, Nova
// Scotia, Canada, on pp. 424-6 of "Graphics Gems", edited by Andrew Glassner,
// Academic Press, 1990.

// These results are generally faster than their full-precision counterparts
// (except on modern PC hardware), but are only worth 7 bits of binary
// precision (1 in 128).
// [A table for 7-bit precision requires 256 entries.]
inline void NiPoint3::UnitizeVector(NiPoint3& v)
{ 
    float fLeng;
    short sExp;

    // This pointer allows us to treat the float as its integer bit
    // representation.
    unsigned int *pUIRep = (unsigned int*)&fLeng;

    // WARNING:  SERIOUS ALIASING going on here.  Be very careful with
    // optimization flags.

    // Compute the squared length normally.
    fLeng = v.x*v.x + v.y*v.y + v.z*v.z;

    // If the squared length is zero, exit.
    if(!(*pUIRep))
        fLeng = 0.0f;
    else
    {
        // Shift and mask the exponent from the float.
        sExp = static_cast<short>(((*pUIRep) >> 23) - 127);

        // Mask the exponent away.
        *pUIRep &= 0x7fffff;

        // If the exponent is odd, use the upper half of the square root
        // table.
        if(sExp & 0x1)
            *pUIRep |= 0x800000;

        // Compute the sqrt'ed exponent (divide by 2).
        sExp >>= 1;

        // Build the new floating point representation by ORing the looked-up
        // mantissa with the computed exponent.
        *pUIRep = ms_pSqrtTable[(*pUIRep) >> 16] | ((sExp + 127) << 23);

        fLeng = 1.0f/fLeng; // Invert the length.
    }

    v.x *= fLeng;
    v.y *= fLeng;
    v.z *= fLeng;
}
//---------------------------------------------------------------------------
// This algorithm was published as "A High Speed, Low Precision Square Root",
// by Paul Lalonde and Robert Dawon, Dalhousie University, Halifax, Nova
// Scotia, Canada, on pp. 424-6 of "Graphics Gems", edited by Andrew Glassner,
// Academic Press, 1990.

// These results are generally faster than their full-precision counterparts
// (except on modern PC hardware), but are only worth 7 bits of binary
// precision (1 in 128).
// [A table for 7-bit precision requires 256 entries.]
inline float NiPoint3::VectorLength(const NiPoint3& v)
{
    float fLeng;
    short sExp;

    // This pointer allows us to treat the float as its integer bit
    // representation.
    unsigned int *pUIRep = (unsigned int*)&fLeng;

    // WARNING:  SERIOUS ALIASING going on here.  Be very careful with
    // optimization flags.

    // Compute the squared length normally.
    fLeng = v.x*v.x + v.y*v.y + v.z*v.z;

    if(!(*pUIRep))  // If the squared length is zero, exit.
        return 0.0f;
    else
    {
        // Shift and mask the exponent from the float.
        sExp = static_cast<short>(((*pUIRep) >> 23) - 127);

        // Mask the exponent away.
        *pUIRep &= 0x7fffff;

        // If the exponent is odd, use the upper half of the square root
        // table.
        if(sExp & 0x1)
            *pUIRep |= 0x800000;

        // Compute the sqrt'ed exponent (divide by 2).
        sExp >>= 1;

        // Build the new floating point representation by ORing the looked-up
        // mantissa with the computed exponent.
        *pUIRep = ms_pSqrtTable[(*pUIRep) >> 16] | ((sExp + 127) << 23);

        return fLeng;
    }
}
//---------------------------------------------------------------------------
inline NiPoint3 NiPoint3::Perpendicular() const
{
    if(fabs(x) < fabs(y))
    {
        if(fabs(z) < fabs(x))
            return UnitCross(NiPoint3::UNIT_Z);
        else
            return UnitCross(NiPoint3::UNIT_X);
    }
    else
    {
        if(fabs(z) < fabs(y))
            return UnitCross(NiPoint3::UNIT_Z);
        else
            return UnitCross(NiPoint3::UNIT_Y);
    }
}
//---------------------------------------------------------------------------
inline void NiPoint3::LoadBinary(NiStream& stream, NiPoint3* pkValues, 
    unsigned int uiNumValues)
{
    NiStreamLoadBinary(stream, (float*)pkValues, uiNumValues*3);
}
//---------------------------------------------------------------------------
inline void NiPoint3::SaveBinary(NiStream& stream, NiPoint3* pkValues, 
    unsigned int uiNumValues)
{
    NiStreamSaveBinary(stream, (float*)pkValues, uiNumValues*3);
}
//---------------------------------------------------------------------------
