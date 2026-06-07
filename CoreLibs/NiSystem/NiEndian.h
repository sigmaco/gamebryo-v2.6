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

#ifndef NIENDIAN_H
#define NIENDIAN_H
#include "NiSystemLibType.h"
#include "NiRTLib.h"

class NISYSTEM_ENTRY NiEndian
{
public:
    // In-place swap of byte order for a 2-byte variable
    static void Swap16(char* pcValue, unsigned int uiCount = 1);

    // In-place swap of byte order for a 4-byte variable
    static void Swap32(char* pcValue, unsigned int uiCount = 1);

    // In-place swap of byte order for a 8-byte variable
    static void Swap64(char* pcValue, unsigned int uiCount = 1);
};

#include "NiEndian.inl"
#endif
