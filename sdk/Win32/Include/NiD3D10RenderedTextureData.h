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

#ifndef NID3D10RENDEREDTEXTUREDATA_H
#define NID3D10RENDEREDTEXTUREDATA_H

#include "NiD3D10TextureData.h"
#include "NiD3D10Renderer.h"

class NiD3D10TextureRenderTarget;
class NiRenderedTexture;

class NID3D10RENDERER_ENTRY NiD3D10RenderedTextureData : 
    public NiD3D10TextureData
{
public:
    static NiD3D10RenderedTextureData* Create(NiRenderedTexture* pkTexture,
        Ni2DBuffer::MultiSamplePreference eMSAAPref);
    virtual ~NiD3D10RenderedTextureData();

protected:
    NiD3D10RenderedTextureData(NiRenderedTexture* pkTexture);

    bool PrepareTexture(Ni2DBuffer::MultiSamplePreference eMSAAPref);

    unsigned short m_usNumTextures;
};

NiSmartPointer(NiD3D10RenderedTextureData);

//#include "NiD3D10RenderedTextureData.inl"

#endif  //#ifndef NID3D10RENDEREDTEXTUREDATA_H
