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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiRenderedTexture.h"
#include "NiRenderer.h"

NiImplementRTTI(NiRenderedTexture, NiTexture);

NiTexture::FormatPrefs NiRenderedTexture::ms_kDefaultPrefs;

//---------------------------------------------------------------------------
NiRenderedTexture* NiRenderedTexture::Create(unsigned int uiWidth, 
    unsigned int uiHeight, NiRenderer* pkRenderer, 
    Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    return Create(uiWidth,uiHeight,pkRenderer,ms_kDefaultPrefs,eMSAAPref);

}
//---------------------------------------------------------------------------
NiRenderedTexture* NiRenderedTexture::Create(unsigned int uiWidth, 
    unsigned int uiHeight, NiRenderer* pkRenderer, FormatPrefs& kPrefs,
    Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    if (!(pkRenderer))
        return NULL;

    if (!(NiIsPowerOf2(uiWidth) && NiIsPowerOf2(uiHeight)))
    {
        if ((pkRenderer->GetFlags() & NiRenderer::CAPS_NONPOW2_TEXT) == 0 &&
            (pkRenderer->GetFlags() & 
            NiRenderer::CAPS_NONPOW2_CONDITIONAL_TEXT) == 0)
        {
            // Non-power-of-two textures are not supported at all
            return NULL;
        }
    }

    NiRenderedTexture* pkThis = NiNew2(NiMemHint::USAGE_TEXTURE) NiRenderedTexture;
    pkThis->m_kFormatPrefs = kPrefs;
    pkThis->m_spBuffer = Ni2DBuffer::Create(uiWidth, uiHeight);
    
    // Creating the rendered texture data should also create the
    // Ni2DBuffer renderer data.
    if (!pkRenderer->CreateRenderedTextureRendererData(pkThis,eMSAAPref))
    {
        NiDelete pkThis;
        return NULL;
    }

    NIASSERT(pkThis->m_spBuffer != NULL);
    return pkThis;
}
//---------------------------------------------------------------------------
NiRenderedTexture* NiRenderedTexture::Create(Ni2DBuffer* pkBuffer,
    NiRenderer* pkRenderer)
{
    if (!(pkRenderer) || !(pkBuffer))
        return NULL;

    NIASSERT(!pkBuffer->GetRendererData() && "NiRenderedTexture::Create() "
        "can only be called with a Ni2DBuffer that is not yet used elsewhere."
        );

    if (!(NiIsPowerOf2(pkBuffer->GetWidth()) && 
        NiIsPowerOf2(pkBuffer->GetHeight())))
    {
        if ((pkRenderer->GetFlags() & NiRenderer::CAPS_NONPOW2_TEXT) == 0 &&
            (pkRenderer->GetFlags() & 
            NiRenderer::CAPS_NONPOW2_CONDITIONAL_TEXT) == 0)
        {
            // Non-power-of-two textures are not supported at all
            return NULL;
        }
    }

    NiRenderedTexture* pkThis = NiNew2(NiMemHint::USAGE_TEXTURE) NiRenderedTexture;
    pkThis->m_spBuffer = pkBuffer;
    
    if (!pkRenderer->CreateRenderedTextureRendererData(pkThis))
    {
        NiDelete pkThis;
        return NULL;
    }

    NIASSERT(pkThis->m_spBuffer != NULL);
    return pkThis;
}
//---------------------------------------------------------------------------
Ni2DBuffer* NiRenderedTexture::GetBuffer() const
{
    return m_spBuffer;
}
//---------------------------------------------------------------------------
