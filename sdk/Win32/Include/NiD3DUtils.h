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
//---------------------------------------------------------------------------
#ifndef NID3DUTILS_H
#define NID3DUTILS_H

#include <NiOS.h>

//  The NiD3DUtils file contains some common functions which can be used
//  in numerous places throughout the renderer.
//---------------------------------------------------------------------------
static inline int FastFloatToInt(float fValue)
{
#if defined(_XENON)
    // There is not currently a fast path implemented for Xenon.
    return (int)fValue;
#else   //#if defined(_XENON)
    int iValue;
    _asm fld fValue
    _asm fistp iValue
    return iValue;
#endif  //#if defined(_XENON)
}
//---------------------------------------------------------------------------
static inline DWORD F2DW(float fValue) 
{ 
    return *((DWORD*)&fValue); 
}
//---------------------------------------------------------------------------

#endif  //#ifndef NID3DUTILS_H
