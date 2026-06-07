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

#include "NiShadowCubeMap.h"
#include "NiShadowGenerator.h"
#include "NiRenderedCubeMap.h"
#include "NiShadowManager.h"

NiImplementRTTI(NiShadowCubeMap, NiShadowMap);

NiFixedString NiShadowCubeMap::ms_kMapName;

//---------------------------------------------------------------------------
bool NiShadowCubeMap::Initialize(NiShadowCubeMap* pkShadowCubeMap,
    unsigned int uiSize, NiTexture::FormatPrefs& kPrefs,
    const NiPixelFormat* pkDepthFormat, 
    NiTexturingProperty::ClampMode eClampMode,
    NiTexturingProperty::FilterMode eFilterMode)
{
    NIASSERT(pkShadowCubeMap);

    Destroy(pkShadowCubeMap);
    pkShadowCubeMap->SetTextureType(TT_CUBE);
    
    // Get renderer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Create RenderedCubeMap.
    NiRenderedCubeMap* pkCubeMap = NiRenderedCubeMap::Create(uiSize,
        pkRenderer, kPrefs);
    pkShadowCubeMap->m_spTexture = pkCubeMap;
    if (!pkShadowCubeMap->m_spTexture)
    {
        // Requested rendered texture could not be created on the current
        // hardware. Fail out.
        Destroy(pkShadowCubeMap);
        return false;
    }

    // Create ShadowRenderClicks.
    for (unsigned char ucIndex = 0; ucIndex < GetNumRenderClicks(); ucIndex++)
    {
        NiShadowRenderClick* pkRenderClick = NiNew NiShadowRenderClick;
        pkRenderClick->SetClearAllBuffers(true);
        pkRenderClick->SetValidator(
            NiShadowManager::GetShadowClickValidator());
        pkRenderClick->AppendRenderView(NiNew Ni3DRenderView(NiNew NiCamera,
            NiShadowManager::GetCullingProcess()));

        // Obtain compatible depth/stencil buffer.
        Ni2DBuffer* pkTexBuffer = 
            pkCubeMap->GetFaceBuffer((NiRenderedCubeMap::FaceID)ucIndex);
        NiDepthStencilBuffer* pkDSBuffer = 
            NiShadowManager::GetCompatibleDepthStencil(
                pkTexBuffer, pkDepthFormat);
        if (!pkDSBuffer)
        {
            // Requested depth/stencil buffer could not be created on the 
            // current hardware. Fail out.
            Destroy(pkShadowCubeMap);
            return false;
        }

        // Create RenderTargetGroup
        NiRenderTargetGroup* pkRenderTargetGroup = NiRenderTargetGroup::Create(
            pkTexBuffer, pkRenderer, pkDSBuffer);
        pkRenderClick->SetRenderTargetGroup(pkRenderTargetGroup);
        if (!pkRenderTargetGroup)
        {
            // Could not create requested render target group. Fail out.
            Destroy(pkShadowCubeMap);
            return false;
        }


        pkShadowCubeMap->m_aspRenderClicks[ucIndex] = pkRenderClick;
    }

    pkShadowCubeMap->SetClampMode(eClampMode);
    pkShadowCubeMap->SetFilterMode(eFilterMode);
    return true;
}
//---------------------------------------------------------------------------
void NiShadowCubeMap::Destroy(NiShadowCubeMap* pkShadowCubeMap)
{
    for (unsigned char uc = 0; uc < GetNumRenderClicks(); uc++)
    {
        if (pkShadowCubeMap->m_aspRenderClicks[uc])
            pkShadowCubeMap->m_aspRenderClicks[uc] = NULL;
    }

    pkShadowCubeMap->ClearFlags();
    pkShadowCubeMap->m_spTexture = NULL;
    pkShadowCubeMap->m_spRenderClick = NULL;
}
//---------------------------------------------------------------------------
