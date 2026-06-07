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

#ifndef NIDX9DIRECT3DTEXTUREDATA_H
#define NIDX9DIRECT3DTEXTUREDATA_H

#include "NiDX9TextureData.h"
class NiDX9Direct3DTexture;

class NIDX9RENDERER_ENTRY NiDX9Direct3DTextureData : public NiDX9TextureData
{
public:
    static NiDX9Direct3DTextureData* Create(NiDX9Direct3DTexture* pkTexture, 
        NiDX9Renderer* pkRenderer, D3DBaseTexturePtr pkD3DTexture);

protected:
    NiDX9Direct3DTextureData(NiDX9Direct3DTexture* pkTexture, 
        NiDX9Renderer* pkRenderer);
};

#endif  //#ifndef NIDX9DIRECT3DTEXTUREDATA_H
