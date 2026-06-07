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

#ifndef NIDX9DIRECT3DTEXTURE_H
#define NIDX9DIRECT3DTEXTURE_H

#include "NiTexture.h"

#include "NiDX9RendererLibType.h"
class NiRenderer;

class NIDX9RENDERER_ENTRY NiDX9Direct3DTexture : public NiTexture
{
    NiDeclareRTTI;

public:
    static NiDX9Direct3DTexture* Create(NiRenderer* pkRenderer);

    virtual unsigned int GetWidth() const;
    virtual unsigned int GetHeight() const;

    // *** begin Emergent internal use only ***
    inline void SetWidth(unsigned int uiWidth);
    inline void SetHeight(unsigned int uiHeight);
    // *** end Emergent internal use only ***

protected:
    NiDX9Direct3DTexture();

    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
};

typedef NiPointer<NiDX9Direct3DTexture> NiDX9Direct3DTexturePtr;

#include "NiDX9Direct3DTexture.inl"

#endif //NIDX9DIRECT3DTEXTURE_H
