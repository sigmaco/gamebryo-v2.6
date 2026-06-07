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

#ifndef NIPOINT4_H
#define NIPOINT4_H

#include "NiPoint3.h"
#include <NiOS.h>

#ifdef __SPU__
#include <vectormath/cpp/vectormath_aos.h>
#endif

#if defined (WIN32) || defined(_XENON)
__declspec(align(16))
#elif defined (_PS3) || defined(_WII)
#else 
    #error unknown platform
#endif

/**
    This class encapsulates a 4 value point with coordinates x,y,z,w.
    The interface is generally cross platform while the implementation
    is platform specific.
*/
class NIMAIN_ENTRY NiPoint4
{
public:
    /// Constructor
    inline NiPoint4 ();
    /// Constructor
    inline NiPoint4 (float fX, float fY, float fZ, float fW);
    /// Constructor
    inline NiPoint4 (const NiPoint3& kPt, float fW = 1.0f);

    /// Conversion from NiPoint4 to NiPoint3. Simply drops the w component.
    inline operator NiPoint3();

#if defined (WIN32) || defined(_WII)
#elif defined(_XENON)
    /// Constructor
    inline NiPoint4 (XMVECTOR vPt);
#elif defined (_PS3)
    static void PointsPlusEqualFloatTimesPoints(NiPoint4* pkDst, float f,
        const NiPoint4* pkSrc, unsigned int uiVerts);
#   ifdef __SPU__
    /// Constructor
    inline NiPoint4 (vec_float4 vPt);
#   else
    /// Constructor
    inline NiPoint4 (vector float vPt);
#   endif
#else 
    #error unknown platform
#endif

    // type conversion operators
#if defined (WIN32) || defined(_WII)
#elif defined(_XENON)
    inline operator const XMVECTOR () const { return m_vfPt; }
#elif defined (_PS3)
#else 
#error unknown platform
#endif

    /// Zeroes the fields of the NiPoint4
    inline void MakeZero();

    /// Set the NiPoint4 values
    inline void Set(float fX, float fY, float fZ, float fW);
    /// Set the NiPoint4 values
    inline void Set(const NiPoint3& kPt, float fW);

    /// Set3VectorW0 functions set xyz, with w set to 0.
    inline void Set3VectorW0(const NiPoint3& kPt);
    /// Set3VectorW0 functions set xyz, with w set to 0.
    inline void Set3VectorW0(float fX, float fY, float fZ);

    // element access
    inline float operator[](int i) const;
    inline float Set(unsigned int uiComponent, float fValue);
    inline float X() const;
    inline float Y() const;
    inline float Z() const;
    inline float W() const;
    inline void SetX(float fX);
    inline void SetY(float fY);
    inline void SetZ(float fZ);
    inline void SetW(float fW);

    inline NiPoint4 operator- () const;
    inline NiPoint4 operator+ (const NiPoint4& kPt) const;
    inline NiPoint4 operator- (const NiPoint4& kPt) const;
    inline NiPoint4 operator* (float fScalar) const;
    inline NiPoint4 operator/ (float fScalar) const;
    inline NiPoint4 operator+= (const NiPoint4& kPt);
    inline NiPoint4 operator-= (const NiPoint4& kPt);
    inline NiPoint4 operator*= (float fScalar);
    inline NiPoint4 operator/= (float fScalar);

    inline bool operator==(const NiPoint4& kPt) const;
    inline bool operator!=(const NiPoint4& kPt) const;

#if defined(_XENON)
    static void UnitizeVectors(
        NiPoint4* p4In, 
        unsigned int uiCount,
        NiPoint3* p3Out);
#endif

    /*
        Given a normalized NiPoint3, scales the X and Y so that Z equals 1.
        Sets our X = point.X, Y = point.Y
    */
    inline void CompressNormalizedIntoXY(const NiPoint3& point);

    /*
        Given a normalized NiPoint3, scales the X and Y so that Z equals 1.
        Sets our Z = point.X, W = point.Y
    */
    inline void CompressNormalizedIntoZW(const NiPoint3& point);

    inline void DecompressXYIntoNormalized(NiPoint3& point);

    inline void DecompressZWIntoNormalized(NiPoint3& point);

    inline void DecompressHighDetailIntoNormalized(NiPoint3& point);

    static const NiPoint4 ZERO;


    union
    {
        // Use of these members is not portable, 
        // only use them in platform specific code.
#if defined (WIN32) || defined(_WII)
#elif defined(_XENON)
        XMVECTOR m_vfPt;
#elif defined (_PS3)
#   ifdef __SPU__
        vec_float4 m_vfPt;
#   else
        vector float m_vfPt;
#   endif
#else 
#error unknown platform
#endif

        // *** begin Emergent internal use only ***
    
        // Use of this member is strongly discouraged. 
        // Future platforms may not support this union, and its use may be 
        // less efficient then api above.
        float m_afPt[4];

        // *** end Emergent internal use only ***
    };
};

#include "NiPoint4_Platform.inl"
#include "NiPoint4.inl"

#endif // NIPOINT4_H
