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

#ifndef NIPOINT3_H
#define NIPOINT3_H

#include "NiMath.h"
#include "NiMemObject.h"
#ifndef __SPU__
#include "NiStream.h"
#endif

class NIMAIN_ENTRY NiPoint3 : public NiMemObject
{
public:
    float x, y, z;

    inline NiPoint3();
    inline NiPoint3 (float fX, float fY, float fZ);

    inline const float& operator[] (int i) const;
    inline float& operator[] (int i);

    inline bool operator== (const NiPoint3& pt) const;
    inline bool operator!= (const NiPoint3& pt) const;

    inline NiPoint3 operator+ (const NiPoint3& pt) const;
    inline NiPoint3 operator- (const NiPoint3& pt) const;
    inline float operator* (const NiPoint3& pt) const;
    inline NiPoint3 operator* (float fScalar) const;
    inline NiPoint3 operator/ (float fScalar) const;
    inline NiPoint3 operator- () const;
    NIMAIN_ENTRY friend NiPoint3 operator* (float fScalar, const NiPoint3& pt);
    static NiPoint3 ComponentProduct (const NiPoint3& p0, const NiPoint3& p1);

    inline NiPoint3& operator+= (const NiPoint3& pt);
    inline NiPoint3& operator-= (const NiPoint3& pt);
    inline NiPoint3& operator*= (float fScalar);
    inline NiPoint3& operator/= (float fScalar);

    float Length () const;
    inline float SqrLength () const;
    inline float Dot (const NiPoint3& pt) const;
    inline float Unitize ();
    inline NiPoint3 Cross (const NiPoint3& pt) const;
    NiPoint3 UnitCross (const NiPoint3& pt) const;

    // This finds one of the infinite set of perpendicular vectors
    inline NiPoint3 Perpendicular () const;

    static const NiPoint3 ZERO;
    static const NiPoint3 UNIT_X;
    static const NiPoint3 UNIT_Y;
    static const NiPoint3 UNIT_Z;
    static const NiPoint3 UNIT_ALL;

    // *** begin Emergent internal use only ***
#ifndef __SPU__
    // streaming
    void LoadBinary (NiStream& stream);
    void SaveBinary (NiStream& stream) const;

    // Utility methods for streaming arrays of NiPoint3's.
    // Note that the array's must be pre-allocated for Loading, 
    // meaning that the external caller is responsible for
    // streaming the size.
    inline static void LoadBinary (NiStream& stream, NiPoint3* pkValues, 
        unsigned int uiNumValues);
    inline static void SaveBinary (NiStream& stream, NiPoint3* pkValues, 
        unsigned int uiNumValues);
    
    char* GetViewerString (const char* pPrefix) const;

    static void _SDMInit();
    static void _SDMShutdown();
#endif
    // *** end Emergent internal use only ***

public:

    // Methods that use fast, but less precise, square root.

    // This algorithm was published as "A High Speed, Low Precision Square
    // Root", by Paul Lalonde and Robert Dawon, Dalhousie University, Halifax,
    // Nova Scotia, Canada, on pp. 424-6 of "Graphics Gems", edited by Andrew
    // Glassner, Academic Press, 1990.

    // WARNING:  These results are generally faster than their full-precision
    // counterparts (except on modern PC hardware), but are only worth 7 bits
    // of binary precision (1 in 128).
    // [A table for 7-bit precision requires 256 entries.]
    static void UnitizeVectors(NiPoint3* pkV, unsigned int uiNum, 
        unsigned int uiStride);
    inline static void UnitizeVector (NiPoint3& v);
    inline static float VectorLength (const NiPoint3& v);

    static void PointsPlusEqualFloatTimesPoints(NiPoint3* pkDst, float f,
        const NiPoint3* pkSrc, unsigned int uiVerts);
    static void WeightedPointsPlusWeightedPoints(NiPoint3* pkDst,
        float fWeight, const NiPoint3* pkSrc, unsigned int uiVerts);
#ifndef __SPU__

protected:
    static unsigned int* InitSqrtTable ();
    static unsigned int* ms_pSqrtTable;
#endif

};

#ifndef __SPU__
typedef NiTObjectSet<NiPoint3> NiPoint3Set;
#endif

//---------------------------------------------------------------------------
//  Inline include
#include "NiPoint3.inl"

//---------------------------------------------------------------------------

#endif


