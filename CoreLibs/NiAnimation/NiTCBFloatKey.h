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

#ifndef NITCBFLOATKEY_H
#define NITCBFLOATKEY_H

#include "NiFloatKey.h"

class NIANIMATION_ENTRY NiTCBFloatKey : public NiFloatKey
{
    NiDeclareAnimationStream;
public: 
    // construction
    NiTCBFloatKey ();
    NiTCBFloatKey (float fTime, float fValue, float fTension,
        float fContinuity, float fBias);

    // attributes
    inline void SetTension (float fTension);
    inline void SetContinuity (float fContinuity);
    inline void SetBias (float fBias);
    inline float GetTension () const;
    inline float GetContinuity () const;
    inline float GetBias () const;
    
protected:
    static void CalculateDVals (NiTCBFloatKey& result, float fValSub1,
        float fValPlus1, float fPreLen, float fNextLen);

    float m_fTension;
    float m_fContinuity;
    float m_fBias;

    // DS and DD hold the first derivative information defined by
    // the tension, continuity, and bias parameters.
    float m_fDS;
    float m_fDD;
};

NiRegisterAnimationStream(NiTCBFloatKey);

#include "NiTCBFloatKey.inl"

#endif
