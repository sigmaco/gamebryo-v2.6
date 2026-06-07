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
#include "NiZBufferProperty.h"
#include "NiStencilProperty.h"
#include "NiShadowGenerator.h"
#include "NiVSMShadowTechnique.h"
#include "NiVSMBlurMaterial.h"
#include "NiShadowClickGenerator.h"
#include "NiRenderedTexture.h"
#include "NiScreenFillingRenderView.h"
#include "NiShaderFactory.h"
#include "NiSingleShaderMaterial.h"
#include "NiShaderFactory.h"
#include "VSMBlurMaterialNodeLibrary/NiVSMBlurMaterialNodeLibrary.h"

NiImplementRTTI(NiVSMShadowTechnique, NiShadowTechnique);

//---------------------------------------------------------------------------
NiVSMShadowTechnique::NiVSMShadowTechnique(const NiFixedString& kName,
    const NiFixedString kReadFragmentName, 
    const NiFixedString kWriteFragmentName, bool, bool) :
    NiShadowTechnique(kName, kReadFragmentName, kReadFragmentName, 
    kReadFragmentName, kWriteFragmentName, kWriteFragmentName, 
    kWriteFragmentName, m_bUseCubeMapForPointLight, m_bWriteBatchable),
    m_bCanNotFindShaders(false), m_bBlurKernelSizeChanged(false),
    m_uiBlurKernelSize(4), m_fMaxWidth(512), m_fMaxHeight(512), 
    m_spPostProcessTexture(NULL), m_spPostProcessRTG(NULL),
    m_spVSMBlurNodeLibrary(0), m_spGaussianBlur(0)
{
    /* */
}
//---------------------------------------------------------------------------
NiVSMShadowTechnique::NiVSMShadowTechnique(const NiFixedString& kName, 
     const NiFixedString kDirReadFragmentName, 
     const NiFixedString kPointReadFragmentName, 
     const NiFixedString kSpotReadFragmentName, 
     const NiFixedString kDirWriteFragmentName, 
     const NiFixedString kPointWriteFragmentName, 
     const NiFixedString kSpotWriteFragmentName, 
     bool bUseCubeMapForPointLight,
     bool bWriteBatchable) : NiShadowTechnique(kName, kDirReadFragmentName, 
     kPointReadFragmentName, kSpotReadFragmentName, kDirWriteFragmentName, 
     kPointWriteFragmentName, kSpotWriteFragmentName, bUseCubeMapForPointLight,
     bWriteBatchable), 
     m_bCanNotFindShaders(false), m_bBlurKernelSizeChanged(false),
     m_uiBlurKernelSize(4), m_fMaxWidth(512), m_fMaxHeight(512),
     m_spPostProcessTexture(NULL), m_spPostProcessRTG(NULL),
     m_spVSMBlurNodeLibrary(0),
     m_spGaussianBlur(0)
{
    /* */
}
//---------------------------------------------------------------------------
NiVSMShadowTechnique::~NiVSMShadowTechnique()
{
    m_spPostProcessTexture = NULL;
    m_spPostProcessRTG = NULL;
    m_spGaussianBlur = 0;
    m_spVSMBlurNodeLibrary = 0;
}
//---------------------------------------------------------------------------
void NiVSMShadowTechnique::AppendPostProcessRenderClicks(
    NiShadowMap* pkShadowMap, NiShadowClickGenerator* pkClickGenerator)
{
    // Only perform blurring on 2D shadow maps
    if (NiIsKindOf(NiShadowCubeMap, pkShadowMap))
        return;

    if(!m_uiBlurKernelSize || !PreparePostProcessData(pkShadowMap))
        return;

    float fHorzRatio = 
        (float)pkShadowMap->GetTexture()->GetWidth() / m_fMaxWidth;
    float fVertRatio = 
        (float)pkShadowMap->GetTexture()->GetHeight() / m_fMaxHeight;

    // Setup viewport to ensure a 1-1 texel mapping for horz blur
    NiRect<float> kViewport;
    kViewport.m_left = 0.0f;
    kViewport.m_top = 1.0f;
    kViewport.m_right = fHorzRatio;        
    kViewport.m_bottom = 1.0f - fVertRatio;

    // Create horizontal Gaussian blur click with size corrected viewport
    NiViewRenderClick* pkBlurXClick = 
        pkClickGenerator->PreparePostProcessClick(
        (NiTexture*)pkShadowMap->GetTexture(), 
        (NiRenderTargetGroup*)m_spPostProcessRTG, 
        (NiMaterial*)m_spGaussianBlur);
    pkBlurXClick->SetViewport(kViewport);

    NiScreenFillingRenderView* pkRenderView = NiDynamicCast(
        NiScreenFillingRenderView, 
        pkBlurXClick->GetRenderViews().GetHead());

    NIASSERT(pkRenderView);
    NiTexturingProperty* pkTexProp = (NiTexturingProperty*)
        pkRenderView->GetProperty(NiProperty::TEXTURING);

    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);

    NiZBufferProperty* pkZBuffProp = (NiZBufferProperty*)
        pkRenderView->GetProperty(NiProperty::ZBUFFER);

    if (!pkZBuffProp)
    {
        pkZBuffProp = NiNew NiZBufferProperty();
        pkZBuffProp->SetZBufferTest(false);
        pkZBuffProp->SetZBufferWrite(false);
        pkRenderView->GetScreenFillingQuad().AttachProperty(pkZBuffProp);
    }
    else
    {
        pkZBuffProp->SetZBufferTest(false);
        pkZBuffProp->SetZBufferWrite(false);
    }

    NiStencilProperty* pkStencilProp = (NiStencilProperty*)
        pkRenderView->GetProperty(NiProperty::STENCIL);
    if (!pkStencilProp)
    {
        pkStencilProp = NiNew NiStencilProperty();
        pkStencilProp->SetDrawMode(NiStencilProperty::DRAW_BOTH);
        pkRenderView->GetScreenFillingQuad().AttachProperty(pkStencilProp);
    }
    else
    {
        pkStencilProp->SetDrawMode(NiStencilProperty::DRAW_BOTH);
    }

    if (m_bBlurKernelSizeChanged)
    {
        pkRenderView->GetScreenFillingQuad().SetMaterialNeedsUpdate(true);
    }

    pkRenderView->GetScreenFillingQuad().UpdateProperties();
    pkClickGenerator->AddRenderClick(pkBlurXClick);

    // Create vertical Gaussian blur click
    NiViewRenderClick* pkBlurYClick = 
        pkClickGenerator->PreparePostProcessClick(
        (NiTexture*)m_spPostProcessTexture, (NiRenderTargetGroup*)
        pkShadowMap->GetRenderClick()->GetRenderTargetGroup(), 
        (NiMaterial*)m_spGaussianBlur);

    // Setup a texture transform on the vertical blur click to ensure a 
    // 1-1 texel mapping.
    pkRenderView = NiDynamicCast(NiScreenFillingRenderView, 
        pkBlurYClick->GetRenderViews().GetHead());

    NIASSERT(pkRenderView);
    pkTexProp = (NiTexturingProperty*)pkRenderView->GetProperty(
        NiProperty::TEXTURING);

    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);

    NIASSERT(pkTexProp);
    NiTextureTransform* pkTexTrans = pkTexProp->GetBaseTextureTransform();
    if (!pkTexTrans)
    {
        pkTexTrans = NiNew NiTextureTransform();
        pkTexProp->SetBaseTextureTransform(pkTexTrans);
    }

    pkZBuffProp = (NiZBufferProperty*)pkRenderView->GetProperty(
        NiProperty::ZBUFFER);

    if (!pkZBuffProp)
    {
        pkZBuffProp = NiNew NiZBufferProperty();
        pkZBuffProp->SetZBufferTest(false);
        pkZBuffProp->SetZBufferWrite(false);
        pkRenderView->GetScreenFillingQuad().AttachProperty(pkZBuffProp);
    }
    else
    {
        pkZBuffProp->SetZBufferTest(false);
        pkZBuffProp->SetZBufferWrite(false);
    }

    pkStencilProp = (NiStencilProperty*)pkRenderView->GetProperty(
        NiProperty::STENCIL);
    if (!pkStencilProp)
    {
        pkStencilProp = NiNew NiStencilProperty();
        pkStencilProp->SetDrawMode(NiStencilProperty::DRAW_BOTH);
        pkRenderView->GetScreenFillingQuad().AttachProperty(pkStencilProp);
    }
    else
    {
        pkStencilProp->SetDrawMode(NiStencilProperty::DRAW_BOTH);
    }

    if (m_bBlurKernelSizeChanged)
    {
        pkRenderView->GetScreenFillingQuad().SetMaterialNeedsUpdate(true);
    }

    NiPoint2 kScale = NiPoint2(fHorzRatio, -fVertRatio);
    pkTexTrans->SetScale(kScale);
    pkRenderView->GetScreenFillingQuad().UpdateProperties();
    pkClickGenerator->AddRenderClick(pkBlurYClick);

    m_bBlurKernelSizeChanged = false;
}
//---------------------------------------------------------------------------
void NiVSMShadowTechnique::PrepareShadowMap(NiShadowMap* pkShadowMap)
{
    pkShadowMap->SetFilterMode(NiTexturingProperty::FILTER_BILERP);
}
//---------------------------------------------------------------------------
bool NiVSMShadowTechnique::PreparePostProcessData(NiShadowMap* pkShadowMap)
{
    if (m_bCanNotFindShaders)
        return false;

    // Ensure that the max width and height is at least as large at the 
    // shadow map.
    if (pkShadowMap->GetTexture()->GetWidth() > m_fMaxWidth ||
        pkShadowMap->GetTexture()->GetHeight() > m_fMaxHeight)
    {
        // Note this may result in the existence of two sets of post process 
        // texture data for one frame due to smart pointer dependency.
        m_fMaxWidth = (float)pkShadowMap->GetTexture()->GetWidth();
        m_fMaxHeight = (float)pkShadowMap->GetTexture()->GetHeight();
        m_spPostProcessTexture = NULL;
        m_spPostProcessRTG = NULL;
        //m_spGaussianBlur = 0;
    }

    if (m_spPostProcessRTG)
        return true;

    m_spPostProcessTexture = NiRenderedTexture::Create(
        (unsigned int)m_fMaxWidth, (unsigned int)m_fMaxHeight, 
        NiRenderer::GetRenderer(), m_kFormatPrefs);

    // Create the RenderTargetGroup
    m_spPostProcessRTG = NiRenderTargetGroup::Create(
        m_spPostProcessTexture->GetBuffer(), NiRenderer::GetRenderer(), 
        false, false);
    NIASSERT(m_spPostProcessRTG);


    if (m_spGaussianBlur && m_spVSMBlurNodeLibrary)
        return true;

    m_spVSMBlurNodeLibrary = 
        NiVSMBlurMaterialNodeLibrary::CreateMaterialNodeLibrary();
    
    m_spGaussianBlur = NiNew NiVSMBlurMaterial(m_spVSMBlurNodeLibrary, false);

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    pkRenderer->SetDefaultProgramCache(m_spGaussianBlur);

    return true;
}
//---------------------------------------------------------------------------
void NiVSMShadowTechnique::SetBlurKernelSize(unsigned int uiKernelSize)
{
    // Ensure that we always have an even blur kernel size.
    if (uiKernelSize % 2 != 0)
        uiKernelSize--;

    if (m_uiBlurKernelSize != uiKernelSize)
        m_bBlurKernelSizeChanged  = true;

    m_uiBlurKernelSize = uiKernelSize;
}
//---------------------------------------------------------------------------
unsigned int NiVSMShadowTechnique::GetBlurKernelSize()
{
    return m_uiBlurKernelSize;
}
//---------------------------------------------------------------------------
