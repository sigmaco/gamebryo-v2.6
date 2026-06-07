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

#ifndef NISTEPBOOLKEY_H
#define NISTEPBOOLKEY_H

#include "NiBoolKey.h"

class NIANIMATION_ENTRY NiStepBoolKey : public NiBoolKey
{
    NiDeclareAnimationStream;
public:
    // construction
    NiStepBoolKey ();
    NiStepBoolKey (float fTime, const bool bBool);
};

NiRegisterAnimationStream(NiStepBoolKey);

#include "NiStepBoolKey.inl"

#endif


