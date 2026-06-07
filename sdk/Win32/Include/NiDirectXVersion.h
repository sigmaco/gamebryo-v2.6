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

#ifndef NIDIRECTXVERSION_H
#define NIDIRECTXVERSION_H

#if defined(WIN32)

// Specify which DirectX version to use as the DX renderer
// Currently, only the DirectX 9 renderer is supported.

// ***WARNING***
// Only one of these can be active at a time
#if !defined(_DX9)
#define _DX9
#endif //#if !defined(_DX9)

#endif //#if defined(WIN32)

#if defined (_DX9) && defined(_DEBUG)
    #define D3D_DEBUG_INFO
#endif //#if defined (_DX9) and defined(_DEBUG)

#endif //NIDIRECTXVERSION_H
