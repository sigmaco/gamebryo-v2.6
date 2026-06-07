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

#ifndef NIPOSEBUFFERCHANNELTYPE_H
#define NIPOSEBUFFERCHANNELTYPE_H

#include "NiAnimationLibType.h"

enum NiPoseBufferChannelType
{
    // Value must be a power of 2 (except PBINVALIDCHANNEL).
    PBCOLORCHANNEL = 1,
    PBBOOLCHANNEL = 2,
    PBFLOATCHANNEL = 4,
    PBPOINT3CHANNEL = 8,
    PBROTCHANNEL = 16,
    PBREFERENCEDCHANNEL = 32,

    PBINVALIDCHANNEL = 0x7fff,
};

#endif
