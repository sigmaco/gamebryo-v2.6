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

#ifndef NID3D10DIRECT3DTEXTURE_H
#define NID3D10DIRECT3DTEXTURE_H

#include "NiTexture.h"

#include "NiD3D10RendererLibType.h"
class NiRenderer;

class NID3D10RENDERER_ENTRY NiD3D10Direct3DTexture : public NiTexture
{
    NiDeclareRTTI;

public:
    static NiD3D10Direct3DTexture* Create(NiRenderer* pkRenderer);

    virtual unsigned int GetWidth() const;
    virtual unsigned int GetHeight() const;

    // *** begin Emergent internal use only ***
    inline void SetWidth(unsigned int uiWidth);
    inline void SetHeight(unsigned int uiHeight);
    // *** end Emergent internal use only ***

protected:
    NiD3D10Direct3DTexture();

    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
};

typedef NiPointer<NiD3D10Direct3DTexture> NiD3D10Direct3DTexturePtr;

#include "NiD3D10Direct3DTexture.inl"

#endif //NID3D10DIRECT3DTEXTURE_H
