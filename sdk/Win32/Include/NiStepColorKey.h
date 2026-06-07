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

#ifndef NISTEPCOLORKEY_H
#define NISTEPCOLORKEY_H

#include "NiColorKey.h"


class NIANIMATION_ENTRY NiStepColorKey : public NiColorKey
{
    NiDeclareAnimationStream;
public:
    // construction
    NiStepColorKey ();
    NiStepColorKey (float fTime, const NiColorA& color);
};

NiRegisterAnimationStream(NiStepColorKey);

#include "NiStepColorKey.inl"

#endif


