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

#ifndef NID3D10HEADERS_H
#define NID3D10HEADERS_H

#include <D3D10.h>
#include <DXGI.h>
#include <D3DX10.h>

// Link in D3DX9 and D3DX10 so apps can still link and run on XP
#if defined(NIDEBUG)
    #pragma comment(lib, "D3DX9d.lib")
    #pragma comment(lib, "D3DX10d.lib")
#else //#if defined(NIDEBUG)
    #pragma comment(lib, "D3DX9.lib")
    #pragma comment(lib, "D3DX10.lib")
#endif //#if defined(NIDEBUG)

#endif // #ifndef NID3D10HEADERS_H
