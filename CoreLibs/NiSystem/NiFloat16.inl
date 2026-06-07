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
// http://www.emergent.net//

#include "NiDebug.h"

//---------------------------------------------------------------------------
inline NiFloat16::NiFloat16()
{
}
//---------------------------------------------------------------------------
inline void NiFloat16::Set(float f)
{
    NIASSERT(f <= FLOAT16_MAX && f >= -FLOAT16_MAX);

    if (f >= FLOAT16_MIN || f <= -FLOAT16_MIN)
    {
        union
        {
            float f;
            NiUInt32 ui;
        } v;
        v.f = f;
        NiUInt32 uiData = v.ui;
        m_uiData = NiUInt16(
            ((uiData >> 16) & 0x8000) | 
            (((((uiData >> 23) & 0xFF) - 127) + 15) << 10) | 
            ((uiData >> (23-10)) & 0x3FF));
    }
    else
    {
        m_uiData = 0;
    }
}
//---------------------------------------------------------------------------
inline float NiFloat16::Get() const
{
    if (m_uiData)
    {
        union 
        {
            float f;
            NiUInt32 ui;
        } v;
        NiUInt32 uiData = m_uiData;
        v.ui = 
            ((uiData & 0x8000) << 16) | 
            ((((uiData >> 10) & 0x1F) + (127-15)) << 23) | 
            ((uiData & 0x3FF) << (23-10));
        return v.f;
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
inline NiFloat16::NiFloat16(const NiFloat16& f)
{
    m_uiData = f.m_uiData;
}
//---------------------------------------------------------------------------
inline NiFloat16::NiFloat16(float f)
{
    Set(f);
}
//---------------------------------------------------------------------------
inline NiUInt16 NiFloat16::GetData() const
{
    return m_uiData;
}
//---------------------------------------------------------------------------
inline NiFloat16::operator float () const
{
    return Get();
}
//---------------------------------------------------------------------------
inline NiFloat16 NiFloat16::operator - () const
{
    NiFloat16 f;
    f.m_uiData = m_uiData ^ 0x8000;
    return f;
}
//---------------------------------------------------------------------------
inline NiFloat16& NiFloat16::operator = (NiFloat16 f)
{
    m_uiData = f.m_uiData;
    return *this;
}
//---------------------------------------------------------------------------
inline NiFloat16& NiFloat16::operator = (float f)
{
    Set(f);
    return *this;
}
//---------------------------------------------------------------------------
inline NiFloat16& NiFloat16::operator += (NiFloat16 f)
{
    Set(Get() + (float)f);
    return *this;
}
//---------------------------------------------------------------------------
inline NiFloat16& NiFloat16::operator += (float f)
{
    Set(Get() + f);
    return *this;
}
//---------------------------------------------------------------------------
inline NiFloat16& NiFloat16::operator -= (NiFloat16 f)
{
    Set(Get() - (float)f);
    return *this;
}
//---------------------------------------------------------------------------
inline NiFloat16& NiFloat16::operator -= (float f)
{
    Set(Get() - f);
    return *this;
}
//---------------------------------------------------------------------------
inline NiFloat16& NiFloat16::operator *= (NiFloat16 f)
{
    Set(Get() * (float)f);
    return *this;
}
//---------------------------------------------------------------------------
inline NiFloat16& NiFloat16::operator *= (float f)
{
    Set(Get() * f);
    return *this;
}
//---------------------------------------------------------------------------
inline NiFloat16& NiFloat16::operator /= (NiFloat16 f)
{
    Set(Get() / (float)f);
    return *this;
}
//---------------------------------------------------------------------------
inline NiFloat16& NiFloat16::operator /= (float f)
{
    Set(Get() / f);
    return *this;
}
//---------------------------------------------------------------------------
inline bool NiFloat16::operator == (NiFloat16 f) const
{
    return (m_uiData == f.m_uiData);
}
//---------------------------------------------------------------------------
inline bool NiFloat16::operator == (float f) const
{
    NiFloat16 f16(f);
    return (m_uiData == f16.m_uiData);
}
//---------------------------------------------------------------------------
inline bool NiFloat16::operator == (const double& d) const
{
    NiFloat16 f((float)d);
    return (m_uiData == f.m_uiData);
}
//---------------------------------------------------------------------------
inline bool NiFloat16::operator != (NiFloat16 f) const
{
    return (m_uiData != f.m_uiData);
}
//---------------------------------------------------------------------------
inline bool NiFloat16::operator != (float f) const
{
    NiFloat16 f16(f);
    return (m_uiData != f16.m_uiData);
}
//---------------------------------------------------------------------------
inline bool NiFloat16::operator != (const double& d) const
{
    NiFloat16 f((float)d);
    return (m_uiData != f.m_uiData);
}
//---------------------------------------------------------------------------
