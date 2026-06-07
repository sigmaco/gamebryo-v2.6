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
#ifndef NIDX9TEXTUREMANAGER_H
#define NIDX9TEXTUREMANAGER_H

#include "NiDX9Headers.h"
#include "NiDX9RendererLibType.h"
#include <NiTexture.h>
#include <NiRefObject.h>

class NiDX9Renderer;

class NIDX9RENDERER_ENTRY NiDX9TextureManager : public NiRefObject
{
public:
    NiDX9TextureManager(NiDX9Renderer* pkRenderer);
    ~NiDX9TextureManager();

    virtual LPDIRECT3DTEXTURE9 GetTexture(unsigned int uiWidth, 
        unsigned int uiHeight, unsigned int uiLevels, 
        unsigned int uiUsage, D3DFORMAT eFormat, D3DPOOL ePool);

    virtual LPDIRECT3DBASETEXTURE9 PrepareTextureForRendering(
        NiTexture* pNewTexIm, bool& bChanged, bool& bMipmap, bool& bNonPow2);

    virtual bool PrecacheTexture(NiTexture* pkIm);

    virtual unsigned int GetFormatFromTexture(NiTexture& kIm);

protected:
    LPDIRECT3DDEVICE9 m_pkD3DDevice9;
    NiDX9Renderer* m_pkRenderer;
};

typedef NiPointer<NiDX9TextureManager> NiDX9TextureManagerPtr;

#endif  //#ifndef NIDX9TEXTUREMANAGER_H
