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

#ifndef NIPOINT2_H
#define NIPOINT2_H

#include "NiMath.h"
#include "NiStream.h"

class NIMAIN_ENTRY NiPoint2 : public NiMemObject
{
public:
    float x, y;

    inline NiPoint2 () { /**/ }
    NiPoint2 (float fX, float fY);

    inline float& operator[] (int i)
    {
        // warning:  safe for 4-byte aligned class member data
        const float* base = &x;
        return (float&) base[i];
    }

    inline const float& operator[] (int i) const
    {
        // warning:  safe for 4-byte aligned class member data
        const float* base = &x;
        return (float&) base[i];
    }

    inline bool operator== (const NiPoint2& pt) const;
    inline bool operator!= (const NiPoint2& pt) const;

    inline NiPoint2 operator+ (const NiPoint2& pt) const;
    inline NiPoint2 operator- (const NiPoint2& pt) const;
    inline float operator* (const NiPoint2& pt) const;
    inline NiPoint2 operator* (float fScalar) const;
    inline NiPoint2 operator/ (float fScalar) const;
    inline NiPoint2 operator- () const;
    NIMAIN_ENTRY friend NiPoint2 operator* (float fScalar,
        const NiPoint2& pt);
    static NiPoint2 ComponentProduct (const NiPoint2& p0, const NiPoint2& p1);

    inline NiPoint2& operator+= (const NiPoint2& pt);
    inline NiPoint2& operator-= (const NiPoint2& pt);
    inline NiPoint2& operator*= (float fScalar);
    inline NiPoint2& operator/= (float fScalar);

    inline float Length () const;
    inline float SqrLength () const;
    inline float Dot (const NiPoint2& pt) const;
    inline float Unitize ();

    static const NiPoint2 ZERO;
    static const NiPoint2 UNIT_X;
    static const NiPoint2 UNIT_Y;

    // *** begin Emergent internal use only ***
#ifndef __SPU__
    // streaming
    void LoadBinary (NiStream& stream);
    void SaveBinary (NiStream& stream);

    // Utility methods for streaming arrays of NiPoint2's.
    // Note that the array's must be pre-allocated for Loading, 
    // meaning that the external caller is responsible for
    // streaming the size.
    inline static void LoadBinary (NiStream& stream, NiPoint2* pkValues, 
        unsigned int uiNumValues);
    inline static void SaveBinary (NiStream& stream, NiPoint2* pkValues, 
        unsigned int uiNumValues);
    char* GetViewerString (const char* pPrefix) const;
#endif
    // *** end Emergent internal use only ***
};

//---------------------------------------------------------------------------
#include "NiRTLib.h"

//---------------------------------------------------------------------------
//  Inline include
#include "NiPoint2.inl"

//---------------------------------------------------------------------------

#endif

