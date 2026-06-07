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

#ifndef NITCBROTKEY_H
#define NITCBROTKEY_H

#include "NiRotKey.h"

class NIANIMATION_ENTRY NiTCBRotKey : public NiRotKey
{
    NiDeclareAnimationStream;
public:
    // construction
    NiTCBRotKey ();
    NiTCBRotKey (float fTime, float fAngle, const NiPoint3& axis);
    NiTCBRotKey (float fTime, const NiQuaternion& quat);
    
    // attributes
    inline void SetTension (float fTension);
    inline void SetContinuity (float fContinuity);
    inline void SetBias (float fBias);
    inline float GetTension () const;
    inline float GetContinuity () const;
    inline float GetBias () const;
    
protected:
    void CalculateDVals (const NiTCBRotKey* pPrev, const NiTCBRotKey* pNext);

    float m_fTension;
    float m_fContinuity;
    float m_fBias;

    // Intermediate quaternion values used for interpolation.
    NiQuaternion m_A;
    NiQuaternion m_B;
};

NiRegisterAnimationStream(NiTCBRotKey);

#include "NiTCBRotKey.inl"

#endif
