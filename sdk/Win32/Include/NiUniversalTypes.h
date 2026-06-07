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

#ifndef NIUNIVERSALTYPES_H
#define NIUNIVERSALTYPES_H

#include <NiOS.h>

typedef __int8 NiInt8;
typedef unsigned __int8 NiUInt8;
typedef __int16 NiInt16;
typedef unsigned __int16 NiUInt16;
typedef __int32 NiInt32;
typedef unsigned __int32 NiUInt32;
typedef __int64 NiInt64;
typedef unsigned __int64 NiUInt64;

typedef HWND NiWindowRef;
typedef HWND NiStatusWindowRef;
typedef HDC NiContextRef;
typedef HGLRC NiOglRenderContextRef;
typedef HINSTANCE NiInstanceRef;
typedef HACCEL NiAcceleratorRef;
typedef HMODULE NiModuleRef;

typedef struct 
{
    HWND hWnd;
    UINT uiMsg;
    WPARAM wParam;
    LPARAM lParam;
} NiEventRecord, *NiEventRef;
typedef LPCREATESTRUCT NiCreateStructRef;
typedef HMENU NiMenuRef;

typedef unsigned short NiWChar;

#include "NiFloat16.h"

#if (_MSC_VER >= 1400) //VC8.0
#   define NI_RESTRICT __restrict
#else
#   define NI_RESTRICT
#endif

#endif // NIUNIVERSALTYPES_H