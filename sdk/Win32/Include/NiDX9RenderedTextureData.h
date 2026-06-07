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
#ifndef NIDX9RENDEREDTEXTUREDATA_H
#define NIDX9RENDEREDTEXTUREDATA_H

#include "NiDX9TextureData.h"
#include "NiDX9Renderer.h"

class NiDX9TextureRenderTarget;
class NiRenderedTexture;

class NIDX9RENDERER_ENTRY NiDX9RenderedTextureData : public NiDX9TextureData
{
public:
    static NiDX9RenderedTextureData* Create(NiRenderedTexture* pkTexture, 
        NiDX9Renderer* pkRenderer);

    virtual ~NiDX9RenderedTextureData();

protected:
    NiDX9RenderedTextureData(NiRenderedTexture* pkTexture, 
        NiDX9Renderer* pkRenderer);

    const NiPixelFormat* CreateSurf(const NiRenderedTexture* pkTexture);
};

typedef NiPointer<NiDX9RenderedTextureData> NiDX9RenderedTextureDataPtr;

#include "NiDX9RenderedTextureData.inl"

#endif  //#ifndef NIDX9RENDEREDTEXTUREDATA_H
