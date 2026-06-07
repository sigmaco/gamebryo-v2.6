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

#ifndef NITCBPOSKEY_H
#define NITCBPOSKEY_H

#include "NiPosKey.h"

class NIANIMATION_ENTRY NiTCBPosKey : public NiPosKey
{
    NiDeclareAnimationStream;
public:
    // construction
    NiTCBPosKey ();
    NiTCBPosKey (float fTime, const NiPoint3& pos, float fTension,
        float fContinuity, float fBias);

    // attributes
    inline void SetTension (float fTension);
    inline void SetContinuity (float fContinuity);
    inline void SetBias (float fBias);
    inline float GetTension () const;
    inline float GetContinuity () const;
    inline float GetBias () const;

protected:
    void CalculateDVals (const NiPoint3& ptSub1, const NiPoint3& ptPlus1,
        float fPreLen, float fNextLen);

    float m_fTension;
    float m_fContinuity;
    float m_fBias;

    // DS and DD hold the first derivative information defined by
    // the tension, continuity, and bias parameters.
    NiPoint3 m_DS;
    NiPoint3 m_DD;

    // A & B are calculated and stored for use with this interpolant
    // and the interpolant to the "right", i.e. the next interpolant 
    // specified at a non-decreasing time value.  In doing this, A & B
    // can be calculated once, rather than each time interpolation
    // is done using the two keys in question.
    NiPoint3 m_A;
    NiPoint3 m_B;
};

NiRegisterAnimationStream(NiTCBPosKey);

#include "NiTCBPosKey.inl"

#endif
