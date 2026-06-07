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

#ifndef NIINPUTERR_H
#define NIINPUTERR_H

#include "NiInputLibType.h"

enum NiInputErr
{
    NIIERR_OK                       = 0x00000000,
    NIIERR_GENERIC                  = 0x80000000,
    NIIERR_UNSUPPORTED              = 0x80000001,
    NIIERR_INVALIDPARAMS            = 0x80000002,
    NIIERR_INITFAILED               = 0x80000003,
    NIIERR_INVALIDDEVICE            = 0x80000004,
    NIIERR_DEVICENOTFOUND           = 0x80000005,
    NIIERR_DEVICELOST               = 0x80000006,
    NIIERR_DEVICECREATEFAIL         = 0x80000007,
    NIIERR_DEVICEINITFAIL           = 0x80000008,
    NIIERR_DEVICEERROR              = 0x80000009,
    NIIERR_DEVICECHANGE             = 0x8000000a,
    NIIERR_NOOPENPORTS              = 0x8000000b,
    NIIERR_APIFAILURE               = 0x8000000c,
    NIIERR_ALLOCFAILED              = 0x8000000d,
    NIIERR_INVALIDACTION            = 0x8000000e,
    NIIERR_NOTMAPPED                = 0x8000000f,
    NIIERR_MAPPINGFAILED            = 0x80000010,
    NIIERR_MAPPINGFAILED_GAMEPAD    = 0x80000011,
    NIIERR_MAPPINGFAILED_KEYBOARD   = 0x80000012,
    NIIERR_MAPPINGFAILED_MOUSE      = 0x80000013,
    NIIERR_MAPPING_TAKEN            = 0x80000014,
    NIIERR_INVALID_AXIS             = 0x80000020,
    // Enumeration status return values
    NIIERR_ENUM_NOTRUNNING          = 0x80000030,
    NIIERR_ENUM_NOTCOMPLETE         = 0x80000031,
    NIIERR_ENUM_FAILED              = 0x80000032,
    NIIERR_ENUM_COMPLETE            = 0x00000000
};

//static const char* NiInput_GetErrorString(NiInputErr eErr);

#endif  //#ifndef NIINPUTERR_H
