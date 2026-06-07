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
#include "NiShadowClickGenerator.h"
#include "NiScreenFillingRenderView.h"

NiImplementRootRTTI(NiShadowClickGenerator);

//---------------------------------------------------------------------------
NiShadowClickGenerator::NiShadowClickGenerator() : 
    m_uiActivePostProcessClicks(0)
{
}
//---------------------------------------------------------------------------
NiShadowClickGenerator::~NiShadowClickGenerator()
{
}
//---------------------------------------------------------------------------
void NiShadowClickGenerator::PrepareShadowGenerators(
    const NiTPointerList<NiShadowGeneratorPtr>& kGenerators)
{
    NiTListIterator kIter = kGenerators.GetHeadPos();
    while (kIter)
    {
        NiShadowGenerator* pkGenerator = kGenerators.GetNext(kIter);
        PrepareShadowGenerator(pkGenerator);
    }
}
//---------------------------------------------------------------------------
void NiShadowClickGenerator::PrepareShadowGenerator(
    NiShadowGenerator* pkGenerator)
{
    NIASSERT(pkGenerator);

    NiDynamicEffect* pkDynEffect = pkGenerator->GetAssignedDynamicEffect();

    switch (pkDynEffect->GetEffectType())
    {
        case NiDynamicEffect::SPOT_LIGHT:
        case NiDynamicEffect::SHADOWSPOT_LIGHT:
            PrepareSpotLightShadowGenerator(pkGenerator);
            break;
        case NiDynamicEffect::POINT_LIGHT:
        case NiDynamicEffect::SHADOWPOINT_LIGHT:
            PreparePointLightShadowGenerator(pkGenerator);
            break;
        case NiDynamicEffect::DIR_LIGHT:
        case NiDynamicEffect::SHADOWDIR_LIGHT:
            PrepareDirectionalLightShadowGenerator(pkGenerator);
            break;
        default:
            // Unsupported dynamic effect; do nothing.
            break;
    }
}
//---------------------------------------------------------------------------
NiViewRenderClick* NiShadowClickGenerator::PreparePostProcessClick(
    NiTexture* pkSourceTexture, NiRenderTargetGroup* pkDestRTG, 
    NiMaterial* pkPostProcessMat)
{
    NiViewRenderClick* pkClick = NULL;

    // Check to see if we need a new post processing render click
    if (m_uiActivePostProcessClicks >= kPostProcessClicks.GetSize())
    {
        pkClick = NiNew NiViewRenderClick();
        
        NiScreenFillingRenderView* pkView =
            NiScreenFillingRenderView::Create();
        NiTexturingProperty* pkTexProp = NiNew NiTexturingProperty();
        pkTexProp->SetBaseMap( NiNew NiTexturingProperty::Map() );
        pkView->AttachProperty(pkTexProp);        
        pkClick->AppendRenderView(pkView);
        pkClick->SetClearAllBuffers(true);

        kPostProcessClicks.Add(pkClick);
    }
    else
    {
        pkClick = kPostProcessClicks.GetAt(m_uiActivePostProcessClicks);
    }

    NIASSERT(pkClick);
    NiScreenFillingRenderView* pkRenderView = NiDynamicCast(
        NiScreenFillingRenderView, pkClick->GetRenderViews().GetHead());

    NIASSERT(pkRenderView);
    pkRenderView->GetScreenFillingQuad().ApplyAndSetActiveMaterial(
        pkPostProcessMat);
    
    NiTexturingProperty* pkTexProp =
        NiDynamicCast(NiTexturingProperty, 
        pkRenderView->GetProperty(NiProperty::TEXTURING));
    
    NiTexturingProperty::Map* pkBaseMap = pkTexProp->GetBaseMap();
    pkBaseMap->SetTexture(pkSourceTexture);

    pkClick->SetRenderTargetGroup(pkDestRTG);

    m_uiActivePostProcessClicks++;
    return pkClick;
}
//---------------------------------------------------------------------------
void NiShadowClickGenerator::CleanAllPostProcessClicks()
{
    unsigned int uiSize = kPostProcessClicks.GetSize();
    
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiViewRenderClickPtr spClick = kPostProcessClicks.GetAt(ui);
        spClick->SetRenderTargetGroup(NULL);
    }

    m_uiActivePostProcessClicks = 0;
}
//---------------------------------------------------------------------------
