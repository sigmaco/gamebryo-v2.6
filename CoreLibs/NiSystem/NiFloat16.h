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

#ifndef NIFLOAT16_H
#define NIFLOAT16_H

#include "NiSystemLibType.h"
#include "NiUniversalTypes.h"

#define FLOAT16_MAX 65504.0f
#define FLOAT16_MIN 6.1035156e-5f

class NISYSTEM_ENTRY NiFloat16
{
public:
    inline NiFloat16();

    inline void Set(float f);

    inline float Get() const;

    inline NiFloat16(const NiFloat16& f);

    inline NiFloat16(float f);

    inline NiUInt16 GetData() const;

    //--------------------
    // Conversion to float
    //--------------------
    inline operator float () const;

    //------------
    // Unary minus
    //------------
    inline NiFloat16 operator - () const;

    //-----------
    // Assignment
    //-----------
    inline NiFloat16& operator = (NiFloat16 f);
    inline NiFloat16& operator = (float f);

    inline NiFloat16& operator += (NiFloat16 f);
    inline NiFloat16& operator += (float f);

    inline NiFloat16& operator -= (NiFloat16 f);
    inline NiFloat16& operator -= (float f);

    inline NiFloat16& operator *= (NiFloat16 f);
    inline NiFloat16& operator *= (float f);

    inline NiFloat16& operator /= (NiFloat16 f);
    inline NiFloat16& operator /= (float f);

    //------------
    // Equality
    //------------
    inline bool operator == (NiFloat16 f) const;
    inline bool operator == (float f) const;
    inline bool operator == (const double& d) const;

    inline bool operator != (NiFloat16 f) const;
    inline bool operator != (float f) const;
    inline bool operator != (const double& d) const;

private:
    NiUInt16 m_uiData;
};

#include "NiFloat16.inl"

#endif // NIFLOAT16_H
