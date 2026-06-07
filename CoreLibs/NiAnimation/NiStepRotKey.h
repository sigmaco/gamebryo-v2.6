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

#ifndef NISTEPROTKEY_H
#define NISTEPROTKEY_H

#include "NiRotKey.h"

class NIANIMATION_ENTRY NiStepRotKey : public NiRotKey
{
    NiDeclareAnimationStream;
public:
    // construction
    NiStepRotKey ();
    NiStepRotKey (float fTime, const NiQuaternion& quat);
    NiStepRotKey (float fTime, float fAngle, const NiPoint3& axis);
};

NiRegisterAnimationStream(NiStepRotKey);

#include "NiStepRotKey.inl"

#endif

