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

#include "NiShadowGenerator.h"
#include "NiShadowMap.h"
#include "NiShadowManager.h"
#include "NiStandardMaterial.h"

NiImplementRootRTTI(NiShadowMap);
NiFixedString NiShadowMap::ms_akMapName[NiStandardMaterial::LIGHT_MAX];

//---------------------------------------------------------------------------
bool NiShadowMap::Initialize(NiShadowMap* pkShadowMap, 
    unsigned int uiWidth, unsigned int uiHeight, 
    NiTexture::FormatPrefs& kPrefs, 
    const NiPixelFormat* pkDepthFormat, 
    NiTexturingProperty::ClampMode eClampMode,
    NiTexturingProperty::FilterMode eFilterMode)
{
    NIASSERT(pkShadowMap);

#if defined(_WII)
    // Rendered textures bigger than ~640x512 are not supported on the Wii.
    // Limit shadow map sizes to 512 so that artists can select 1024 maps
    // on other platforms that will still work on the Wii.
#if defined(NIDEBUG)
    if (uiWidth > 512 || uiHeight > 512)
    {
        NiOutputDebugString(
            "NiShadowMap: Shadow maps larger than "
            "512x512 are not supported on the Wii.  The size "
            "of the shadow map has been automatically reduced.\n");
    }
#endif
    uiWidth = (uiWidth > 512) ? 512 : uiWidth;
    uiHeight = (uiHeight > 512) ? 512 : uiHeight;
#endif

    Destroy(pkShadowMap);
    pkShadowMap->SetTextureType(TT_SINGLE);

    // Get renderer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Create RenderedTexture
    pkShadowMap->m_spTexture = NiRenderedTexture::Create(uiWidth, 
        uiHeight, pkRenderer, kPrefs);
    if (!pkShadowMap->m_spTexture)
    {
        // Requested rendered texture could not be created on the current
        // hardware. Fail out.
        Destroy(pkShadowMap);
        return false;
    }

    // Create ShadowRenderClick.
    pkShadowMap->m_spRenderClick = NiNew NiShadowRenderClick;
    pkShadowMap->m_spRenderClick->SetClearAllBuffers(true);
    pkShadowMap->m_spRenderClick->SetValidator(
        NiShadowManager::GetShadowClickValidator());
    pkShadowMap->m_spRenderClick->AppendRenderView(NiNew Ni3DRenderView(
        NiNew NiCamera, NiShadowManager::GetCullingProcess()));

    // Obtain compatible depth/stencil buffer.
    Ni2DBuffer* pkTexBuffer = pkShadowMap->m_spTexture->GetBuffer();
    NiDepthStencilBuffer* pkDSBuffer = 
        NiShadowManager::GetCompatibleDepthStencil(
            pkTexBuffer, pkDepthFormat);
    if (!pkDSBuffer)
    {
        // Requested depth/stencil buffer could not be created on the current
        // hardware. Fail out.
        Destroy(pkShadowMap);
        return false;
    }

    // Create RenderTargetGroup
    NiRenderTargetGroup* pkRenderTargetGroup = NiRenderTargetGroup::Create(
        pkTexBuffer, pkRenderer, pkDSBuffer);
    if (!pkRenderTargetGroup)
    {
        // Could not create requested render target group. Fail out.
        Destroy(pkShadowMap);
        return false;
    }

    pkShadowMap->m_spRenderClick->SetRenderTargetGroup(pkRenderTargetGroup);
    pkShadowMap->SetClampMode(eClampMode);
    pkShadowMap->SetFilterMode(eFilterMode);
    pkShadowMap->SetMaxAnisotropy(1);

    return true;
}
//---------------------------------------------------------------------------
void NiShadowMap::Destroy(NiShadowMap* pkShadowMap)
{
    pkShadowMap->ClearFlags();
    pkShadowMap->m_spTexture = NULL;
    pkShadowMap->m_spRenderClick = NULL;
}
//---------------------------------------------------------------------------
