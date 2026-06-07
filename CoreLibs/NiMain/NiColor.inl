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
//  NiColor inline functions

//---------------------------------------------------------------------------
inline NiColor::NiColor(float fR, float fG, float fB)
{
    r = fR;
    g = fG;
    b = fB;
}
//---------------------------------------------------------------------------
inline NiColor& NiColor::operator= (float fScalar)
{
    r = fScalar;
    g = fScalar;
    b = fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline bool NiColor::operator== (const NiColor& c) const
{
    return (r == c.r && g == c.g && b == c.b);
}
//---------------------------------------------------------------------------
inline bool NiColor::operator!= (const NiColor& c) const
{
    return !(*this == c);
}
//---------------------------------------------------------------------------
inline NiColor NiColor::operator+ (const NiColor& c) const
{
    NiColor result = *this;
    result.r += c.r;
    result.g += c.g;
    result.b += c.b;
    return result;
}
//---------------------------------------------------------------------------
inline NiColor NiColor::operator- (const NiColor& c) const
{
    NiColor result = *this;
    result.r -= c.r;
    result.g -= c.g;
    result.b -= c.b;
    return result;
}
//---------------------------------------------------------------------------
inline NiColor NiColor::operator* (float fScalar) const
{
    NiColor result = *this;
    result.r *= fScalar;
    result.g *= fScalar;
    result.b *= fScalar;
    return result;
}
//---------------------------------------------------------------------------
inline NiColor NiColor::operator* (const NiColor &c) const
{
    NiColor result = *this;
    result.r *= c.r;
    result.g *= c.g;
    result.b *= c.b;
    return result;
}
//---------------------------------------------------------------------------
inline NiColor NiColor::operator/ (float fScalar) const
{
    NiColor result = *this;
    result.r /= fScalar;
    result.g /= fScalar;
    result.b /= fScalar;
    return result;
}
//---------------------------------------------------------------------------
inline NiColor NiColor::operator/ (const NiColor &c) const
{
    NiColor result = *this;
    result.r /= c.r;
    result.g /= c.g;
    result.b /= c.b;
    return result;
}
//---------------------------------------------------------------------------
inline NiColor NiColor::operator- () const
{
    return NiColor(-r,-g,-b);
}
//---------------------------------------------------------------------------
inline NiColor operator* (float fScalar, const NiColor& c)
{
    return c*fScalar;
}
//---------------------------------------------------------------------------
inline NiColor& NiColor::operator+= (const NiColor& c)
{
    r += c.r;
    g += c.g;
    b += c.b;
    return *this;
}
//---------------------------------------------------------------------------
inline NiColor& NiColor::operator-= (const NiColor& c)
{
    r -= c.r;
    g -= c.g;
    b -= c.b;
    return *this;
}
//---------------------------------------------------------------------------
inline NiColor& NiColor::operator*= (float fScalar)
{
    r *= fScalar;
    g *= fScalar;
    b *= fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline NiColor& NiColor::operator*= (const NiColor &c)
{
    r *= c.r;
    g *= c.g;
    b *= c.b;
    return *this;
}
//---------------------------------------------------------------------------
inline NiColor& NiColor::operator/= (float fScalar)
{
    r /= fScalar;
    g /= fScalar;
    b /= fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline NiColor& NiColor::operator/= (const NiColor &c)
{
    r /= c.r;
    g /= c.g;
    b /= c.b;
    return *this;
}
//---------------------------------------------------------------------------
inline void NiColor::Clamp()
{
    // Clamp to [0,1]^3.  Assumes that (r,g,b) >= (0,0,0).  This is a
    // reasonable assumption since colors are only added or multiplied in
    // the lighting system.  Note that clamping can cause significant
    // changes in the final color.
    if ( r > 1.0f )
        r = 1.0f;
    if ( g > 1.0f )
        g = 1.0f;
    if ( b > 1.0f )
        b = 1.0f;
}
//---------------------------------------------------------------------------
inline void NiColor::Scale()
{
    // Scale down by maximum component (preserves the final color).
    float fMax = r;
    if ( g > fMax )
        fMax = g;
    if ( b > fMax )
        fMax = b;

    if ( fMax > 1.0f )
    {
        float fInvMax = 1.0f/fMax;
        r *= fInvMax;
        g *= fInvMax;
        b *= fInvMax;
    }
}
//---------------------------------------------------------------------------
inline void NiColor::GetAs(NiRGBA& kColor) const
{
    // Node we do not clamp or scale down the values.  If these values are 
    // out of range you will get incorrect color values.  You should use the 
    // Scale() or Clamp() operations to make sure the values are in range.
    kColor.r() = (NiUInt8)(r * 255.0f);
    kColor.g() = (NiUInt8)(g * 255.0f);
    kColor.b() = (NiUInt8)(b * 255.0f);
    kColor.a() = 255;
}
#ifndef __SPU__
//---------------------------------------------------------------------------
inline void NiColor::LoadBinary(NiStream& stream, NiColor* pkValues, 
    unsigned int uiNumValues)
{
    NiStreamLoadBinary(stream, (float*)pkValues, uiNumValues*3);
}
//---------------------------------------------------------------------------
inline void NiColor::SaveBinary(NiStream& stream, NiColor* pkValues, 
    unsigned int uiNumValues)
{
    NiStreamSaveBinary(stream, (float*)pkValues, uiNumValues*3);
}
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline NiColorA::NiColorA(float fR, float fG, float fB, float fA)
{
    r = fR;
    g = fG;
    b = fB;
    a = fA;
}
//---------------------------------------------------------------------------
inline NiColorA& NiColorA::operator= (float fScalar)
{
    r = fScalar;
    g = fScalar;
    b = fScalar;
    a = fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline bool NiColorA::operator== (const NiColorA& c) const
{
    return (r == c.r && g == c.g && b == c.b && a == c.a);
}
//---------------------------------------------------------------------------
inline bool NiColorA::operator!= (const NiColorA& c) const
{
    return !(*this == c);
}
//---------------------------------------------------------------------------
inline NiColorA NiColorA::operator+ (const NiColorA& c) const
{
    NiColorA result = *this;
    result.r += c.r;
    result.g += c.g;
    result.b += c.b;
    result.a += c.a;
    return result;
}
//---------------------------------------------------------------------------
inline NiColorA NiColorA::operator- (const NiColorA& c) const
{
    NiColorA result = *this;
    result.r -= c.r;
    result.g -= c.g;
    result.b -= c.b;
    result.a -= c.a;
    return result;
}
//---------------------------------------------------------------------------
inline NiColorA NiColorA::operator* (float fScalar) const
{
    NiColorA result = *this;
    result.r *= fScalar;
    result.g *= fScalar;
    result.b *= fScalar;
    result.a *= fScalar;
    return result;
}
//---------------------------------------------------------------------------
inline NiColorA NiColorA::operator* (const NiColorA &c) const
{
    NiColorA result = *this;
    result.r *= c.r;
    result.g *= c.g;
    result.b *= c.b;
    result.a *= c.a;
    return result;
}
//---------------------------------------------------------------------------
inline NiColorA NiColorA::operator/ (float fScalar) const
{
    NiColorA result = *this;
    result.r /= fScalar;
    result.g /= fScalar;
    result.b /= fScalar;
    result.a /= fScalar;
    return result;
}
//---------------------------------------------------------------------------
inline NiColorA NiColorA::operator/ (const NiColorA &c) const
{
    NiColorA result = *this;
    result.r /= c.r;
    result.g /= c.g;
    result.b /= c.b;
    result.a /= c.a;
    return result;
}
//---------------------------------------------------------------------------
inline NiColorA NiColorA::operator- () const
{
    return NiColorA(-r,-g,-b,-a);
}
//---------------------------------------------------------------------------
inline NiColorA operator* (float fScalar, const NiColorA& c)
{
    return c*fScalar;
}
//---------------------------------------------------------------------------
inline NiColorA& NiColorA::operator+= (const NiColorA& c)
{
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
    return *this;
}
//---------------------------------------------------------------------------
inline NiColorA& NiColorA::operator-= (const NiColorA& c)
{
    r -= c.r;
    g -= c.g;
    b -= c.b;
    a -= c.a;
    return *this;
}
//---------------------------------------------------------------------------
inline NiColorA& NiColorA::operator*= (float fScalar)
{
    r *= fScalar;
    g *= fScalar;
    b *= fScalar;
    a *= fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline NiColorA& NiColorA::operator*= (const NiColorA &c)
{
    r *= c.r;
    g *= c.g;
    b *= c.b;
    a *= c.a;
    return *this;
}
//---------------------------------------------------------------------------
inline NiColorA& NiColorA::operator/= (float fScalar)
{
    r /= fScalar;
    g /= fScalar;
    b /= fScalar;
    a /= fScalar;
    return *this;
}
//---------------------------------------------------------------------------
inline NiColorA& NiColorA::operator/= (const NiColorA &c)
{
    r /= c.r;
    g /= c.g;
    b /= c.b;
    a /= c.a;
    return *this;
}
//---------------------------------------------------------------------------
inline void NiColorA::Clamp()
{
    // Clamp to [0,1]^4.  Assumes that (r,g,b,a) >= (0,0,0).  This is a
    // reasonable assumption since colors are only added or multiplied in
    // the lighting system.  Note that clamping can cause significant
    // changes in the final color.
    if ( r > 1.0f )
        r = 1.0f;
    if ( g > 1.0f )
        g = 1.0f;
    if ( b > 1.0f )
        b = 1.0f;
    if ( a > 1.0f )
        a = 1.0f;
}
//---------------------------------------------------------------------------
inline void NiColorA::Scale()
{
    // Scale down by maximum component (preserves the final color).
    float fMax = r;
    if ( g > fMax )
        fMax = g;
    if ( b > fMax )
        fMax = b;

    if ( fMax > 1.0f )
    {
        float fInvMax = 1.0f/fMax;
        r *= fInvMax;
        g *= fInvMax;
        b *= fInvMax;
    }

    if ( a > 1.0f )
        a = 1.0f;
}
//---------------------------------------------------------------------------
inline void NiColorA::GetAs(NiRGBA& kColor) const
{
    // Node we do not clamp or scale down the values.  If these values are 
    // out of range you will get incorrect color values.  You should use the 
    // Scale() or Clamp() operations to make sure the values are in range.
    kColor.r() = (NiUInt8)(r * 255.0f);
    kColor.g() = (NiUInt8)(g * 255.0f);
    kColor.b() = (NiUInt8)(b * 255.0f);
    kColor.a() = (NiUInt8)(a * 255.0f);
}
//---------------------------------------------------------------------------
#ifndef __SPU__
//---------------------------------------------------------------------------
inline void NiColorA::LoadBinary(NiStream& stream, NiColorA* pkValues, 
    unsigned int uiNumValues)
{
    NiStreamLoadBinary(stream, (float*)pkValues, uiNumValues*4);
}
//---------------------------------------------------------------------------
inline void NiColorA::SaveBinary(NiStream& stream, NiColorA* pkValues, 
    unsigned int uiNumValues)
{
    NiStreamSaveBinary(stream, (float*)pkValues, uiNumValues*4);
}
//---------------------------------------------------------------------------
#endif  
//---------------------------------------------------------------------------
inline NiRGBA::NiRGBA(NiUInt8 uiR, NiUInt8 uiG, NiUInt8 uiB, NiUInt8 uiA)
{
    r() = uiR;
    g() = uiG;
    b() = uiB;
    a() = uiA;
}
//---------------------------------------------------------------------------
inline NiUInt8& NiRGBA::r()
{
    return m_kColor.r;
}
//---------------------------------------------------------------------------
inline const NiUInt8& NiRGBA::r() const
{
    return m_kColor.r;
}
//---------------------------------------------------------------------------
inline NiUInt8& NiRGBA::g()
{
    return m_kColor.g;
}
//---------------------------------------------------------------------------
inline const NiUInt8& NiRGBA::g() const
{
    return m_kColor.g;
}
//---------------------------------------------------------------------------
inline NiUInt8& NiRGBA::b()
{
    return m_kColor.b;
}
//---------------------------------------------------------------------------
inline const NiUInt8& NiRGBA::b() const
{
    return m_kColor.b;
}
//---------------------------------------------------------------------------
inline NiUInt8& NiRGBA::a()
{
    return m_kColor.a;
}
//---------------------------------------------------------------------------
inline const NiUInt8& NiRGBA::a() const
{
    return m_kColor.a;
}
//---------------------------------------------------------------------------
inline NiRGBA& NiRGBA::operator= (NiUInt8 uiValue)
{
    r() = uiValue;
    g() = uiValue;
    b() = uiValue;
    a() = uiValue;
    return *this;
}
//---------------------------------------------------------------------------
inline bool NiRGBA::operator== (const NiRGBA& c) const
{
    return (r() == c.r() && g() == c.g() && b() == c.b() && a() == c.a());
}
//---------------------------------------------------------------------------
inline bool NiRGBA::operator!= (const NiRGBA& c) const
{
    return !(*this == c);
}
//---------------------------------------------------------------------------
inline NiRGBA NiRGBA::operator+ (const NiRGBA& c) const
{
    NiRGBA result;
    result.r() = (NiUInt8)NiMin(255, r() + c.r());
    result.g() = (NiUInt8)NiMin(255, g() + c.g());
    result.b() = (NiUInt8)NiMin(255, b() + c.b());
    result.a() = (NiUInt8)NiMin(255, a() + c.a());
    return result;
}
//---------------------------------------------------------------------------
inline NiRGBA NiRGBA::operator- (const NiRGBA& c) const
{
    NiRGBA result;
    result.r() = (NiUInt8)NiMax(0, r() - c.r());
    result.g() = (NiUInt8)NiMax(0, g() - c.g());
    result.b() = (NiUInt8)NiMax(0, b() - c.b());
    result.a() = (NiUInt8)NiMax(0, a() - c.a());
    return result;
}
//---------------------------------------------------------------------------
inline NiRGBA NiRGBA::operator* (float fScalar) const
{
    NiRGBA result;
    result.r() = (NiUInt8)NiMin(255.0f, (float)r() * fScalar);
    result.g() = (NiUInt8)NiMin(255.0f, (float)g() * fScalar);
    result.b() = (NiUInt8)NiMin(255.0f, (float)b() * fScalar);
    result.a() = (NiUInt8)NiMin(255.0f, (float)a() * fScalar);
    return result;
}
//---------------------------------------------------------------------------
inline NiRGBA NiRGBA::operator* (const NiRGBA &c) const
{
    NiRGBA result;
    result.r() = (NiUInt8)NiMin(255, r() * c.r());
    result.g() = (NiUInt8)NiMin(255, g() * c.g());
    result.b() = (NiUInt8)NiMin(255, b() * c.b());
    result.a() = (NiUInt8)NiMin(255, a() * c.a());
    return result;
}
//---------------------------------------------------------------------------
inline NiRGBA NiRGBA::operator/ (float fScalar) const
{
    NiRGBA result;
    result.r() = (NiUInt8)((float)r() / fScalar);
    result.g() = (NiUInt8)((float)g() / fScalar);
    result.b() = (NiUInt8)((float)b() / fScalar);
    result.a() = (NiUInt8)((float)a() / fScalar);
    return result;
}
//---------------------------------------------------------------------------
inline NiRGBA NiRGBA::operator/ (const NiRGBA &c) const
{
    NiRGBA result;
    result.r() = r() / c.r();
    result.g() = g() / c.g();
    result.b() = b() / c.b();
    result.a() = a() / c.a();
    return result;
}
//---------------------------------------------------------------------------
inline NiRGBA NiRGBA::operator- () const
{
    return NiRGBA(-r(),-g(),-b(),-a());
}
//---------------------------------------------------------------------------
inline NiRGBA operator* (float fScalar, const NiRGBA& c)
{
    return c*fScalar;
}
//---------------------------------------------------------------------------
inline NiRGBA& NiRGBA::operator+= (const NiRGBA& c)
{
    r() = (NiUInt8)NiMin(255, r() + c.r());
    g() = (NiUInt8)NiMin(255, g() + c.g());
    b() = (NiUInt8)NiMin(255, b() + c.b());
    a() = (NiUInt8)NiMin(255, a() + c.a());
    return *this;
}
//---------------------------------------------------------------------------
inline NiRGBA& NiRGBA::operator-= (const NiRGBA& c)
{
    r() = (NiUInt8)NiMax(0, r() - c.r());
    g() = (NiUInt8)NiMax(0, g() - c.g());
    b() = (NiUInt8)NiMax(0, b() - c.b());
    a() = (NiUInt8)NiMax(0, a() - c.a());
    return *this;
}
//---------------------------------------------------------------------------
inline NiRGBA& NiRGBA::operator*= (float fScalar)
{
    r() = (NiUInt8)NiMin(255.0f, (float)r() * fScalar);
    g() = (NiUInt8)NiMin(255.0f, (float)g() * fScalar);
    b() = (NiUInt8)NiMin(255.0f, (float)b() * fScalar);
    a() = (NiUInt8)NiMin(255.0f, (float)a() * fScalar);
    return *this;
}
//---------------------------------------------------------------------------
inline NiRGBA& NiRGBA::operator*= (const NiRGBA &c)
{
    r() = (NiUInt8)NiMin(255, r() * c.r());
    g() = (NiUInt8)NiMin(255, g() * c.g());
    b() = (NiUInt8)NiMin(255, b() * c.b());
    a() = (NiUInt8)NiMin(255, a() * c.a());
    return *this;
}
//---------------------------------------------------------------------------
inline NiRGBA& NiRGBA::operator/= (float fScalar)
{
    r() = (NiUInt8)((float)r() / fScalar);
    g() = (NiUInt8)((float)g() / fScalar);
    b() = (NiUInt8)((float)b() / fScalar);
    a() = (NiUInt8)((float)a() / fScalar);
    return *this;
}
//---------------------------------------------------------------------------
inline NiRGBA& NiRGBA::operator/= (const NiRGBA &c)
{
    r() = (NiUInt8)(r() / c.r());
    g() = (NiUInt8)(g() / c.g());
    b() = (NiUInt8)(b() / c.b());
    a() = (NiUInt8)(a() / c.a());
    return *this;
}
//---------------------------------------------------------------------------
inline void NiRGBA::GetAs(NiUInt32& uiColor) const
{
    // We simply return the color value as a NiUInt32
    uiColor = m_uiColor;
}
//---------------------------------------------------------------------------
inline void NiRGBA::GetAsBGRA(NiUInt32& uiColor) const
{
    // We need to flip b and r.
    NiRGBA result(b(), g(), r(), a());
    uiColor = result.m_uiColor;
}
//---------------------------------------------------------------------------
inline void NiRGBA::GetAs(NiColor& kColor) const
{
    kColor.r = ((float)r() / 255.0f);
    kColor.g = ((float)g() / 255.0f);
    kColor.b = ((float)b() / 255.0f);
}
//---------------------------------------------------------------------------
inline void NiRGBA::GetAs(NiColorA& kColor) const
{
    kColor.r = ((float)r() / 255.0f);
    kColor.g = ((float)g() / 255.0f);
    kColor.b = ((float)b() / 255.0f);
    kColor.a = ((float)a() / 255.0f);
}
//---------------------------------------------------------------------------
#ifndef __SPU__
//---------------------------------------------------------------------------
inline void NiRGBA::LoadBinary(NiStream& stream, NiRGBA* pkValues, 
    unsigned int uiNumValues)
{
    NiStreamLoadBinary(stream, (NiUInt8*)pkValues, uiNumValues*4);
}
//---------------------------------------------------------------------------
inline void NiRGBA::SaveBinary(NiStream& stream, NiRGBA* pkValues, 
    unsigned int uiNumValues)
{
    NiStreamSaveBinary(stream, (NiUInt8*)pkValues, uiNumValues*4);
}
//---------------------------------------------------------------------------
#endif  

