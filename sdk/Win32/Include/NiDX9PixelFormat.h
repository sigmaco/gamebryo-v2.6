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
#ifndef NIDX9PIXELFORMAT_H
#define NIDX9PIXELFORMAT_H

#include <NiSystem.h>
#include <NiPixelFormat.h>
#include "NiDX9RendererLibType.h"
#include "NiDX9Headers.h"

class NIDX9RENDERER_ENTRY NiDX9PixelFormat  : public NiMemObject
{
public:
    static D3DFORMAT DetermineD3DFormat(const NiPixelFormat& kDesc);
    static void InitFromD3DFormat(D3DFORMAT eD3DFmt, 
        NiPixelFormat& kFormat);
    static NiPixelFormat* CreateFromD3DFormat(D3DFORMAT eD3DFmt);
};    

#endif  //#ifndef NIDX9PIXELFORMAT_H
