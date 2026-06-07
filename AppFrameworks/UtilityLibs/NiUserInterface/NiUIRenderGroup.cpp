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
#include "NiUserInterfacePCH.h"

#include "NiUIManager.h"
#include "NiUIRenderGroup.h"
#include <NiSourceTexture.h>
#include <NiAlphaProperty.h>
#include <NiMaterialProperty.h>
#include <NiTexturingProperty.h>
#include <NiWireFrameProperty.h>
#include <NiVertexColorProperty.h>

NiImplementRTTI(NiUIRenderGroup, NiUIBaseElement);

//---------------------------------------------------------------------------
NiUIRenderGroup::NiUIRenderGroup(unsigned char ucDepth) :
    NiUIBaseElement(false),
    m_ucDepth(ucDepth)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("NiUIRenderGroup constructor called before "
            "NiUIManager was initialized.  This group will not render "
            "correctly.");
    }
    else
    {
        NiUIManager::GetUIManager()->RegisterUIRenderGroup(this);
    }
}
//---------------------------------------------------------------------------
NiUIRenderGroup::~NiUIRenderGroup()
{
    m_spScreenElements = NULL;
    unsigned int uiIdx, uiSize;
    uiSize = m_kChildren.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        m_kChildren.GetAt(uiIdx) = NULL;
    }
}
//---------------------------------------------------------------------------
void NiUIRenderGroup::Draw(NiRenderer* pkRenderer)
{
    if (!m_bVisible)
        return;

    // First, draw all of the screen elements
    pkRenderer->SetScreenSpaceCameraData();
    m_spScreenElements->RenderImmediate(pkRenderer);

    // Then pass then news on to all children to draw their Ni2DString's
    unsigned int uiIdx, uiSize;
    uiSize = m_kChildren.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        m_kChildren.GetAt(uiIdx)->Draw(pkRenderer);
    }
}
//---------------------------------------------------------------------------
void NiUIRenderGroup::InitializeScreenElements()
{
    if (m_spScreenElements)
        return;

    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("NiUIRenderGroup::InitializeScreenElements called "
            "before NiUIManager was initialized.  This group will not render "
            "correctly.");
        return;
    }
    if (NiUIManager::GetUIManager()->GetAtlasMap()->GetAtlas() == NULL)
    {
        NiOutputDebugString("NiUIRenderGroup::InitializeScreenElements called "
            "before NiUIManager was properly initialized.  This group will "
            "not render correctly.");
        return;
    }
    
    m_spScreenElements = NiNew NiMeshScreenElements(false, true, 1);

    NiTexturingPropertyPtr spTexProp = NiNew NiTexturingProperty(
        NiUIManager::GetUIManager()->GetAtlasMap()->
        GetAtlas()->GetSourcePixelData());
    spTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    spTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_BILERP);
    m_spScreenElements->AttachProperty(spTexProp);
    spTexProp = NULL;

    NiVertexColorPropertyPtr spVertProp = NiNew NiVertexColorProperty();
    spVertProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    spVertProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spScreenElements->AttachProperty(spVertProp);
    spVertProp = NULL;

    NiMaterialPropertyPtr spMatProp = NiNew NiMaterialProperty();
    spMatProp->SetAmbientColor(NiColor::WHITE);
    spMatProp->SetAlpha(1.0f);
    spMatProp->SetDiffuseColor(NiColor::WHITE);
    m_spScreenElements->AttachProperty(spMatProp);
    spMatProp = NULL;

    NiAlphaPropertyPtr spAlphaProp = NiNew NiAlphaProperty();
    spAlphaProp->SetAlphaBlending(true);
    spAlphaProp->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
    spAlphaProp->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
    m_spScreenElements->AttachProperty(spAlphaProp);
    spAlphaProp = NULL;

    NiZBufferPropertyPtr spZBuffProp = NiNew NiZBufferProperty();
    spZBuffProp->SetZBufferTest(false);
    spZBuffProp->SetZBufferWrite(false);
    m_spScreenElements->AttachProperty(spZBuffProp);
    spZBuffProp = NULL;

    NIVERIFY(AttachResources(m_spScreenElements));

    m_spScreenElements->UpdateBound();
    m_spScreenElements->UpdateProperties();
    m_spScreenElements->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spScreenElements);
}
//---------------------------------------------------------------------------
unsigned int NiUIRenderGroup::GetChildElementCount() const
{
    return m_kChildren.GetSize();
}
//---------------------------------------------------------------------------
NiUIBaseElement* NiUIRenderGroup::GetChildElement(unsigned int uiElement) const
{
    return m_kChildren.GetAt(uiElement);
}
//---------------------------------------------------------------------------
