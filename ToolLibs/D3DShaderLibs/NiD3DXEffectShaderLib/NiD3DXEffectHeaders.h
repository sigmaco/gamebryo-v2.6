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

#ifndef NID3DXEFFECTHEADERS_H
#define NID3DXEFFECTHEADERS_H

#include <NiD3DRendererHeaders.h>

#if defined(WIN32)
    #include "NiD3DXEffectHeaders_DX9.h"
#elif defined(_XENON)
    #include "NiD3DXEffectHeaders_Xenon.h"
#endif

#endif  //NID3DXEFFECTHEADERS_H
