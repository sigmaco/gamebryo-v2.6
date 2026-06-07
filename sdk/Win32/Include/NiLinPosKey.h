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

#ifndef NILINPOSKEY_H
#define NILINPOSKEY_H

#include "NiPosKey.h"

class NIANIMATION_ENTRY NiLinPosKey : public NiPosKey
{
    NiDeclareAnimationStream;
public:
    // construction
    NiLinPosKey ();
    NiLinPosKey (float fTime, const NiPoint3& pos);
};

NiRegisterAnimationStream(NiLinPosKey);

#include "NiLinPosKey.inl"

#endif


