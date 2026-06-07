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

#ifndef NISTEPPOSKEY_H
#define NISTEPPOSKEY_H

#include "NiPosKey.h"

class NIANIMATION_ENTRY NiStepPosKey : public NiPosKey
{
    NiDeclareAnimationStream;
public:
    // construction
    NiStepPosKey ();
    NiStepPosKey (float fTime, const NiPoint3& pos);
};

NiRegisterAnimationStream(NiStepPosKey);

#include "NiStepPosKey.inl"

#endif


