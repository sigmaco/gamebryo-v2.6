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

#ifndef NIBASE64_H
#define NIBASE64_H

#include "NiEntityLibType.h"
#include <NiBool.h>
#include <NiRTLib.h>
#include <NiUniversalTypes.h>

class NIENTITY_ENTRY NiBase64
{
public: 
    static char* EncodeToBase64(
        const void* pvSrcBinaryBuffer, 
        int iSrcSize, 
        int iMaxSizeToRead = 0);

    static NiBool DecodeFromBase64(
        void* pvDestBinaryBuffer, 
        const char* pcSrcBase64Ascii,
        size_t& stBytesWritten);

private:
    static NiBool LoadFromBase64(
        NiUInt32& ui32Bits, 
        const char* pcSrcBase64, 
        int iBytes);

    static const unsigned char ms_aucBase64Char[65];
    static const unsigned char ms_aucBase64ToBit6[128];
};

#include "NiBase64.inl"

#endif // NIBASE64_H
