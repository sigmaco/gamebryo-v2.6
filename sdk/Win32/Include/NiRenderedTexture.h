
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

#ifndef NIRENDEREDTEXTURE_H
#define NIRENDEREDTEXTURE_H

#include "NiTexture.h"
#include "Ni2DBuffer.h"

class NiRenderer;

class NIMAIN_ENTRY NiRenderedTexture : public NiTexture
{
    NiDeclareRTTI;

protected:
    static FormatPrefs ms_kDefaultPrefs;
public:
    static NiRenderedTexture* Create(
        unsigned int uiWidth, unsigned int uiHeight, 
        NiRenderer* pkRenderer,
        Ni2DBuffer::MultiSamplePreference eMSAAPref);
    static NiRenderedTexture* Create(
        unsigned int uiWidth, unsigned int uiHeight, 
        NiRenderer* pkRenderer, FormatPrefs& kPrefs = ms_kDefaultPrefs,
        Ni2DBuffer::MultiSamplePreference eMSAAPref = 
        Ni2DBuffer::MULTISAMPLE_NONE);
    static NiRenderedTexture* Create(Ni2DBuffer* pkBuffer,
        NiRenderer* pkRenderer);

    virtual unsigned int GetWidth() const;
    virtual unsigned int GetHeight() const;
    virtual Ni2DBuffer* GetBuffer() const;

protected:
    NiRenderedTexture();
    Ni2DBufferPtr m_spBuffer;
};

typedef NiPointer<NiRenderedTexture> NiRenderedTexturePtr;

//---------------------------------------------------------------------------
#include "NiRenderedTexture.inl"
//---------------------------------------------------------------------------

#endif
