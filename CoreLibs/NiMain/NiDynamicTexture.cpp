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

#include "NiDynamicTexture.h"
#include "NiRenderer.h"

NiImplementRTTI(NiDynamicTexture, NiTexture);

NiTexture::FormatPrefs NiDynamicTexture::ms_kDefaultPrefs;

//---------------------------------------------------------------------------
NiDynamicTexture* NiDynamicTexture::Create(unsigned int uiWidth, 
    unsigned int uiHeight, FormatPrefs& kPrefs, bool bTiledTexture)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (!(pkRenderer))
        return NULL;

    if (!(NiIsPowerOf2(uiWidth) && NiIsPowerOf2(uiHeight)))
    {
        if (((pkRenderer->GetFlags() & NiRenderer::CAPS_NONPOW2_TEXT) == 0) &&
            (pkRenderer->GetFlags() & 
            NiRenderer::CAPS_NONPOW2_CONDITIONAL_TEXT) == 0)
        {
            // Non-power-of-two textures are not supported at all
            return NULL;
        }
    }

    NiDynamicTexture* pkThis = NiNew NiDynamicTexture(uiWidth, uiHeight);
    pkThis->m_kFormatPrefs = kPrefs;
    pkThis->m_bTiled = bTiledTexture;
    
    if (!pkRenderer->CreateDynamicTextureRendererData(pkThis))
    {
        NiDelete pkThis;
        return NULL;
    }

    return pkThis;
}
//---------------------------------------------------------------------------
void* NiDynamicTexture::Lock(int& iPitch)
{
    // Enable editing of pixels.

    iPitch = 0;

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    RendererData* pkRData = GetRendererData();

    // Discard the entire pixel buffer so it may be refilled with pixels.
    void* pvBits = pkRenderer->LockDynamicTexture(pkRData, iPitch);

    return pvBits;
}
//---------------------------------------------------------------------------
bool NiDynamicTexture::UnLock() const
{
    // Mark an end to pixel edits.

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    RendererData* pkRData = GetRendererData();

    return pkRenderer->UnLockDynamicTexture(pkRData);
}
//---------------------------------------------------------------------------
