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

#include "NiUIBaseElement.h"
#include "NiUIManager.h"

NiImplementRootRTTI(NiUIBaseElement);

//---------------------------------------------------------------------------
NiUIBaseElement::NiUIBaseElement(bool bCanTakeFocus) : 
    NiRefObject(),
    m_kNSCRect(0.0f, 0.0f, 0.0f, 0.0f),
    m_kPixelRect(0, 0, 0, 0),
    m_kLocalOffset(NiPoint2(0.0f, 0.0f)),
    m_kDimensions(NiPoint2(0.0f, 0.0f)),
    m_pkParent(NULL),
    m_iBaseQuadIdx(0),
    m_spScreenElements(NULL),
    m_bFocussed(false),
    m_bVisible(true),
    m_bMouseInside(false),
    m_bCanTakeFocus(bCanTakeFocus)
{
}
//---------------------------------------------------------------------------
NiUIBaseElement::~NiUIBaseElement()
{
    m_spScreenElements = NULL;
}
//---------------------------------------------------------------------------
NiUIBaseElement* NiUIBaseElement::GetParent() const
{
    return m_pkParent;
}
//---------------------------------------------------------------------------
void NiUIBaseElement::SetParent(NiUIBaseElement* pkParent)
{
    m_pkParent = pkParent;
}
//---------------------------------------------------------------------------
bool NiUIBaseElement::AttachResources(NiMeshScreenElements* pkScreenQuads)
{
    NIASSERT(pkScreenQuads);
    if (pkScreenQuads)
    {
        unsigned int uiNumQuads = NumQuadsRequired();
        if (uiNumQuads != 0)
        {
            m_spScreenElements = pkScreenQuads;
            m_iBaseQuadIdx = InsertQuads(uiNumQuads, pkScreenQuads);
        }

        unsigned int uiChildCount = GetChildElementCount();
        for (unsigned int ui = 0; ui < uiChildCount; ui++)
        {
            NiUIBaseElement* pkElement = GetChildElement(ui);
            if (pkElement)
            {
                if (!pkElement->AttachResources(pkScreenQuads))
                    return false;
            }
        }

        ReinitializeDisplayElements();

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
int NiUIBaseElement::InsertQuads(unsigned int uiNumQuads,
    NiMeshScreenElements* pkElements)
{
    if (uiNumQuads == 0 || pkElements == NULL)
        return -1;

    int iStartHandle = pkElements->GetNumPolygons();
    for (int i = 0; i < (int)uiNumQuads; i++)
    {
        // Assert that the quads are contiguous
        bool bQuadsAreContiguous = pkElements->Insert(4) == (iStartHandle+i);
        NI_UNUSED_ARG(bQuadsAreContiguous);
        NIASSERT(bQuadsAreContiguous);
    }

    return iStartHandle;
}
//---------------------------------------------------------------------------
NiRect<float> NiUIBaseElement::OriginAndDimensionsToRect(float fX, float fY,
    float fWidth, float fHeight)
{
    NiRect<float> kRect;
    kRect.m_left = fX;
    kRect.m_right = fX + fWidth;
    kRect.m_top = fY;
    kRect.m_bottom = fY + fHeight;

    return kRect;
}
//---------------------------------------------------------------------------
void NiUIBaseElement::SetDimensions(float fWidth, float fHeight)
{
    m_kDimensions = NiPoint2(fWidth, fHeight);
}
//---------------------------------------------------------------------------
void NiUIBaseElement::SetOffset(float fLeftOffset, float fTopOffset)
{
    m_kLocalOffset = NiPoint2(fLeftOffset, fTopOffset);
}
//---------------------------------------------------------------------------
void NiUIBaseElement::SetPixelDimensions(unsigned int uiWidth, 
    unsigned int uiHeight)
{
    NIASSERT(NiUIManager::GetUIManager());
    NiPoint2 kDim = 
        NiUIManager::GetUIManager()->ConvertPixelDimensionsToNSCDimensions(
        uiWidth, uiHeight);
    SetDimensions(kDim.x, kDim.y);
}
//---------------------------------------------------------------------------
void NiUIBaseElement::SetPixelOffset(unsigned int uiLeftOffset, 
    unsigned int uiTopOffset)
{
    NIASSERT(NiUIManager::GetUIManager());
    NiPoint2 kDim = 
        NiUIManager::GetUIManager()->ConvertPixelDimensionsToNSCDimensions(
        uiLeftOffset, uiTopOffset);

    SetOffset(kDim.x, kDim.y);
}
//---------------------------------------------------------------------------
NiPoint2 NiUIBaseElement::GetOffset() const
{
    return m_kLocalOffset;
}

//---------------------------------------------------------------------------
NiPoint2 NiUIBaseElement::GetDimensions() const
{
    return m_kDimensions;
}
//---------------------------------------------------------------------------
void NiUIBaseElement::UpdateRect()
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer == NULL)
    {
        NiOutputDebugString("NiUIBaseElement::UpdateRect(...) called before "
            "a valid NiRenderer has been created.  This is explicitly "
            "not allowed.");
        return;
    }

    NiPoint2 kOffset = m_kLocalOffset;
    NiPoint2 kDimensions = m_kDimensions;

    if (m_pkParent)
    {
        NiRect<float> kParentRect = m_pkParent->GetNSCRect();
        kOffset.x += kParentRect.m_left;
        kOffset.y += kParentRect.m_top;
    }

    m_kNSCRect.m_left = kOffset.x;
    m_kNSCRect.m_right = kOffset.x + kDimensions.x;
    m_kNSCRect.m_top = kOffset.y;
    m_kNSCRect.m_bottom = kOffset.y + kDimensions.y;

    m_kPixelRect = NiUIManager::GetUIManager()->ConvertNSCToPixels(m_kNSCRect);

    UpdateLayout();
    UpdateChildRects();
    ReinitializeDisplayElements();
}
//---------------------------------------------------------------------------
void NiUIBaseElement::UpdateLayout()
{
}
//---------------------------------------------------------------------------
void NiUIBaseElement::UpdateChildRects()
{
    for (unsigned int ui = 0; ui < GetChildElementCount(); ui++)
    {
        NiUIBaseElement* pkElement = GetChildElement(ui);
        if (pkElement)
            pkElement->UpdateRect();
    }
}
//---------------------------------------------------------------------------
NiRect<float> NiUIBaseElement::GetNSCRect() const
{
    return m_kNSCRect;
}
//---------------------------------------------------------------------------
NiRect<unsigned int> NiUIBaseElement::GetPixelRect() const
{
    return m_kPixelRect;
}
//---------------------------------------------------------------------------
unsigned int NiUIBaseElement::GetChildElementCount() const
{
    return 0;
}
//---------------------------------------------------------------------------
NiUIBaseElement* NiUIBaseElement::GetChildElement(unsigned int) const
{
    return NULL;
}
//---------------------------------------------------------------------------
void NiUIBaseElement::ReinitializeDisplayElements()
{
    ReinitializeChildDisplayElements();
}
//---------------------------------------------------------------------------
void NiUIBaseElement::ReinitializeChildDisplayElements()
{
    unsigned int uiChildCount = GetChildElementCount();
    for (unsigned int ui = 0; ui < uiChildCount; ui++)
    {
        NiUIBaseElement* pkElement = GetChildElement(ui);
        if (pkElement)
        {
            pkElement->ReinitializeDisplayElements();
        }
    }
}
//---------------------------------------------------------------------------
NiRect<float> NiUIBaseElement::ComputeInnerNSCRectPixelAccurateOuter(
    NiUIAtlasMap::UIElement eElement, const NiRect<float>& kOuterNSCRect)
{
    NiUIManager* pkManager = NiUIManager::GetUIManager();
    const NiUIAtlasMap* pkAtlas = pkManager->GetAtlasMap();
    NIASSERT(pkAtlas);
    if (kOuterNSCRect.GetWidth() == 0 || kOuterNSCRect.GetHeight() == 0)
    {
        NiOutputDebugString("Zero area elements are being created.  This "
            "may lead to invalid results.");
    }

    // In this case, I want to preserve the pixel accuracy of the outer
    // region.  First, I need to get the difference between the
    // inner and outer regions in the skin texture (in NDC)
    NiRect<float> kInnerNSCRect = kOuterNSCRect;
    NiRect<float> kNDCDiff = pkAtlas->GetOuterBoundsDifference(eElement,
        NiUIAtlasMap::DEVICE_NDC_SPACE);

    // Transform it to NSC and apply it to the inner rect.
    NiPoint2 kDeltaNSC = pkManager->ConvertNDCDimensionsToNSCDimensions(
        kNDCDiff.m_left, kNDCDiff.m_top);
    kInnerNSCRect.m_left += kDeltaNSC.x;
    kInnerNSCRect.m_top += kDeltaNSC.y;

    kDeltaNSC = pkManager->ConvertNDCDimensionsToNSCDimensions(
        kNDCDiff.m_right, kNDCDiff.m_bottom);
    kInnerNSCRect.m_right -= kDeltaNSC.x;
    kInnerNSCRect.m_bottom -= kDeltaNSC.y;

    // This is necessary for the small quads.  It makes it so that if
    // the pixel accurate border wouldn't fit in the space by itself,
    // we scale that portion only so that it does fit.
    NiPoint2 kMidPt((kOuterNSCRect.m_left + kOuterNSCRect.m_right) * 0.5f,
        (kOuterNSCRect.m_top + kOuterNSCRect.m_bottom) * 0.5f);
    kInnerNSCRect.m_left = NiMin(kInnerNSCRect.m_left, kMidPt.x);
    kInnerNSCRect.m_top = NiMin(kInnerNSCRect.m_top, kMidPt.y);
    kInnerNSCRect.m_right = NiMax(kInnerNSCRect.m_right, kMidPt.x);
    kInnerNSCRect.m_bottom = NiMax(kInnerNSCRect.m_bottom, kMidPt.y);

    return kInnerNSCRect;
}
//---------------------------------------------------------------------------
NiRect<float> NiUIBaseElement::ComputeInnerNSCRectPixelAccurateInner(
    NiUIAtlasMap::UIElement eElement, const NiRect<float>& kOuterNSCRect)
{
    NiUIManager* pkManager = NiUIManager::GetUIManager();
    const NiUIAtlasMap* pkAtlas = pkManager->GetAtlasMap();
    NIASSERT(pkAtlas);
    if (kOuterNSCRect.GetWidth() == 0 || kOuterNSCRect.GetHeight() == 0)
    {
        NiOutputDebugString("Zero area elements are being created.  This "
            "may lead to invalid results.");
    }
    
    NiRect<float> kInnerNSCRect = kOuterNSCRect;
    // The k*Desired variables are the value I want to set this inner
    // quad to fit pixel accurate
    NiRect<float> kNDCDesired = pkAtlas->GetInnerBounds(eElement, 
        NiUIAtlasMap::DEVICE_NDC_SPACE);
    // These next few steps convert it to NSC instead of NDC
    NiRect<float> kNSCDesired;
    NiPoint2 kTmp = pkManager->ConvertNDCDimensionsToNSCDimensions(
        kNDCDesired.m_left, kNDCDesired.m_top);
    kNSCDesired.m_left = kTmp.x;
    kNSCDesired.m_top = kTmp.y;
    kTmp = pkManager->ConvertNDCDimensionsToNSCDimensions(
        kNDCDesired.m_right, kNDCDesired.m_bottom);
    kNSCDesired.m_right = kTmp.x;
    kNSCDesired.m_bottom = kTmp.y;

    // I'm getting the edge width: half the difference  between the whole 
    // rectangle's size and the portion I want in the middle
    NiPoint2 kDeltaNSC;
    kDeltaNSC.x = (kOuterNSCRect.GetWidth() - kNSCDesired.GetWidth()) * 0.5f;
    kDeltaNSC.y = (kOuterNSCRect.GetHeight() - kNSCDesired.GetHeight()) * 0.5f;
    // This is necessary for the cases where the whole rectangle is smaller
    // than the inner portion.  This makes it return the inner rectangle as
    // the whole rectangle
    kDeltaNSC.x = NiMax(kDeltaNSC.x, 0.0f);
    kDeltaNSC.y = NiMax(kDeltaNSC.y, 0.0f);

    // Now we just move the inner rectangle in from the outer rectangle by
    // the amount found above.
    kInnerNSCRect.m_left += kDeltaNSC.x;
    kInnerNSCRect.m_top += kDeltaNSC.y;

    kInnerNSCRect.m_right -= kDeltaNSC.x;
    kInnerNSCRect.m_bottom -= kDeltaNSC.y;

    return kInnerNSCRect;
}
//---------------------------------------------------------------------------
NiRect<float> NiUIBaseElement::ComputeAspectRatioPreservingInnerNSCRect(
        NiUIAtlasMap::UIElement eElement, const NiRect<float>& kOuterNSCRect)
{
    NiUIManager* pkManager = NiUIManager::GetUIManager();
    NIASSERT(pkManager);

    const NiUIAtlasMap* pkAtlasMap = pkManager->GetAtlasMap();
    NIASSERT(pkAtlasMap);
    if (kOuterNSCRect.GetWidth() == 0 || kOuterNSCRect.GetHeight() == 0)
    {
        NiOutputDebugString("Zero area elements are being created.  This "
            "may lead to invalid results.");
    }

    NiRect<float> kNSCImageRect = kOuterNSCRect;
    NiRect<float> kImageOuterRectInNDC = pkAtlasMap->GetOuterBounds(
        eElement, NiUIAtlasMap::DEVICE_NDC_SPACE);
    NiPoint2 kImageDimensionsInNSC = 
        pkManager->ConvertNDCDimensionsToNSCDimensions(
        kImageOuterRectInNDC.GetWidth(), kImageOuterRectInNDC.GetHeight());

    NiPoint2 kImageRectDimensionsInNSC(kNSCImageRect.GetWidth(),
        kNSCImageRect.GetHeight());

    NiPoint2 kImageScale(1.0f, 1.0f);
    NiPoint2 kDifferenceInNSC;
    NiPoint2 kRectangleRatios(
        kImageRectDimensionsInNSC.x / kImageDimensionsInNSC.x,
        kImageRectDimensionsInNSC.y / kImageDimensionsInNSC.y);
    
    // We've found the two possible multiplication ratios in order to make
    // a pixel accurate resize of the image.  If they are both greater than
    // one we just display the image at normal size (1, 1).
    // If either is less than one, we use the lesser value so that the
    // image does not exceed its rectangle
    if (kRectangleRatios.x >= 1.0f && kRectangleRatios.y >= 1.0f)
    {
        // kImageScale was initialized to this possibility
    }
    else if (kRectangleRatios.x <= kRectangleRatios.y)
    {
        kImageScale.x = kRectangleRatios.x;
        kImageScale.y = kRectangleRatios.x;
    }
    else 
    {
        kImageScale.x = kRectangleRatios.y;
        kImageScale.y = kRectangleRatios.y;
    }

    kImageDimensionsInNSC = NiPoint2::ComponentProduct(
        kImageDimensionsInNSC, kImageScale);

    kDifferenceInNSC = (kImageRectDimensionsInNSC - 
        kImageDimensionsInNSC) / 2.0f;
    
    kNSCImageRect.m_left += kDifferenceInNSC.x;
    kNSCImageRect.m_right -= kDifferenceInNSC.x;
    kNSCImageRect.m_top += kDifferenceInNSC.y;
    kNSCImageRect.m_bottom -= kDifferenceInNSC.y;

    return kNSCImageRect;
}
//---------------------------------------------------------------------------
bool NiUIBaseElement::InitializeQuad(NiMeshScreenElements* pkElement,
    int iPolygon, const NiRect<float>& kShapeRectInNSC,
    const NiRect<float>& kUVs, const NiColorA& kBackgroundColor)
{
    if (!pkElement || iPolygon < 0)
        return false;

    NiUIManager* pkManager = NiUIManager::GetUIManager();
    if (kShapeRectInNSC.GetWidth() == 0 ||  
        kShapeRectInNSC.GetHeight() == 0)
    {
        NiOutputDebugString("Zero area elements are being created.  This "
            "may lead to invalid results.");
    }

    NiRect<float> kShapeRectInNDC = pkManager->ConvertNSCToNDC(
        kShapeRectInNSC);

    bool bRectSet = pkElement->SetRectangle(iPolygon, 
        kShapeRectInNDC.m_left, kShapeRectInNDC.m_top,
        kShapeRectInNDC.GetWidth(),
        kShapeRectInNDC.GetHeight());

    if (!bRectSet)
        return false;

    bool bColorSet = pkElement->SetColors(iPolygon, 
        kBackgroundColor);

    if (!bColorSet)
        return false;

    bool bTextureSet = pkElement->SetTextures(iPolygon, 0,
        kUVs.m_left, kUVs.m_top,
        kUVs.m_right, kUVs.m_bottom);

    if (!bTextureSet)
        return false;

    return true;
    
}
//---------------------------------------------------------------------------
bool NiUIBaseElement::HideQuad(NiMeshScreenElements* pkElement, int iPolygon)
{
    if (pkElement)
    {
        unsigned int uiStopPt = iPolygon + 1;
        unsigned int uiIdx;
        // This eliminates the UI element 
        for (uiIdx = iPolygon; uiIdx < uiStopPt; ++uiIdx)
        {
            pkElement->SetRectangle(uiIdx, 0.0f, 0.0f, 0.0f, 0.0f);
        }
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiUIBaseElement::InitializeNineQuad(NiMeshScreenElements* pkElement, 
    int iPolygon, const NiRect<float>& kOuterRectInNSC, 
    const NiRect<float>& kInnerRectInNSC, const NiRect<float>& kOuterUVs, 
    const NiRect<float>& kInnerUVs, const NiColorA& kBackgroundColor)
{
    if (!pkElement || iPolygon < 0)
        return false;

    if ((kOuterRectInNSC.GetWidth() == 0 || 
        kOuterRectInNSC.GetHeight() == 0) ||
        (kInnerRectInNSC.GetWidth() == 0 || 
        kInnerRectInNSC.GetHeight() == 0))
    {
        NiOutputDebugString("Zero area elements are being created.  This "
            "may lead to invalid results.");
    }

    bool bRectSet = true;
    bool bColorSet = true;
    bool bTextureSet = true;

    NiUIManager* pkManager = NiUIManager::GetUIManager();

    // Convert from Normalized Safe Frame Coordinates (NSC) to
    // Normalized Device Coordinates (NDC)
    NiRect<float> kInnerRectInNDC = pkManager->ConvertNSCToNDC(
        kInnerRectInNSC);
    NiRect<float> kOuterRectInNDC = pkManager->ConvertNSCToNDC(
        kOuterRectInNSC);

    // Working rect is assumed to be in NDC 
    NiRect<float> kWorkingRect;

    // Top left corner
    kWorkingRect.m_left = kOuterRectInNDC.m_left;
    kWorkingRect.m_right = kInnerRectInNDC.m_left;
    kWorkingRect.m_top = kOuterRectInNDC.m_top;
    kWorkingRect.m_bottom = kInnerRectInNDC.m_top;
    bRectSet = bRectSet && pkElement->SetRectangle(iPolygon + 0, 
        kWorkingRect.m_left, kWorkingRect.m_top, 
        kWorkingRect.GetWidth(), kWorkingRect.GetHeight());

    // Top middle
    kWorkingRect.m_left = kInnerRectInNDC.m_left;
    kWorkingRect.m_right = kInnerRectInNDC.m_right;
    kWorkingRect.m_top = kOuterRectInNDC.m_top;
    kWorkingRect.m_bottom = kInnerRectInNDC.m_top;
    bRectSet = bRectSet && pkElement->SetRectangle(iPolygon + 1, 
        kWorkingRect.m_left, kWorkingRect.m_top, 
        kWorkingRect.GetWidth(), kWorkingRect.GetHeight());

    // Top right corner
    kWorkingRect.m_left = kInnerRectInNDC.m_right;
    kWorkingRect.m_right = kOuterRectInNDC.m_right;
    kWorkingRect.m_top = kOuterRectInNDC.m_top;
    kWorkingRect.m_bottom = kInnerRectInNDC.m_top;
    bRectSet = bRectSet && pkElement->SetRectangle(iPolygon + 2, 
        kWorkingRect.m_left, kWorkingRect.m_top, 
        kWorkingRect.GetWidth(), kWorkingRect.GetHeight());

    // Middle left side
    kWorkingRect.m_left = kOuterRectInNDC.m_left;
    kWorkingRect.m_right = kInnerRectInNDC.m_left;
    kWorkingRect.m_top = kInnerRectInNDC.m_top;
    kWorkingRect.m_bottom = kInnerRectInNDC.m_bottom;
    bRectSet = bRectSet && pkElement->SetRectangle(iPolygon + 3, 
        kWorkingRect.m_left, kWorkingRect.m_top, 
        kWorkingRect.GetWidth(), kWorkingRect.GetHeight());

    // Middle-middle element
    kWorkingRect.m_left = kInnerRectInNDC.m_left;
    kWorkingRect.m_right = kInnerRectInNDC.m_right;
    kWorkingRect.m_top = kInnerRectInNDC.m_top;
    kWorkingRect.m_bottom = kInnerRectInNDC.m_bottom;
    bRectSet = bRectSet && pkElement->SetRectangle(iPolygon + 4, 
        kWorkingRect.m_left, kWorkingRect.m_top, 
        kWorkingRect.GetWidth(), kWorkingRect.GetHeight());

    // Middle right side
    kWorkingRect.m_left = kInnerRectInNDC.m_right;
    kWorkingRect.m_right = kOuterRectInNDC.m_right;
    kWorkingRect.m_top = kInnerRectInNDC.m_top;
    kWorkingRect.m_bottom = kInnerRectInNDC.m_bottom;
    bRectSet = bRectSet && pkElement->SetRectangle(iPolygon + 5, 
        kWorkingRect.m_left, kWorkingRect.m_top, 
        kWorkingRect.GetWidth(), kWorkingRect.GetHeight());

    // Bottom left corner
    kWorkingRect.m_left = kOuterRectInNDC.m_left;
    kWorkingRect.m_right = kInnerRectInNDC.m_left;
    kWorkingRect.m_top = kInnerRectInNDC.m_bottom;
    kWorkingRect.m_bottom = kOuterRectInNDC.m_bottom;
    bRectSet = bRectSet && pkElement->SetRectangle(iPolygon + 6, 
        kWorkingRect.m_left, kWorkingRect.m_top, 
        kWorkingRect.GetWidth(), kWorkingRect.GetHeight());

    // Bottom middle edge
    kWorkingRect.m_left = kInnerRectInNDC.m_left;
    kWorkingRect.m_right = kInnerRectInNDC.m_right;
    kWorkingRect.m_top = kInnerRectInNDC.m_bottom;
    kWorkingRect.m_bottom = kOuterRectInNDC.m_bottom;
    bRectSet = bRectSet && pkElement->SetRectangle(iPolygon + 7, 
        kWorkingRect.m_left, kWorkingRect.m_top, 
        kWorkingRect.GetWidth(), kWorkingRect.GetHeight());

    // Bottom right corner
    kWorkingRect.m_left = kInnerRectInNDC.m_right;
    kWorkingRect.m_right = kOuterRectInNDC.m_right;
    kWorkingRect.m_top = kInnerRectInNDC.m_bottom;
    kWorkingRect.m_bottom = kOuterRectInNDC.m_bottom;
    bRectSet = bRectSet && pkElement->SetRectangle(iPolygon + 8, 
        kWorkingRect.m_left, kWorkingRect.m_top, 
        kWorkingRect.GetWidth(), kWorkingRect.GetHeight());

    for (unsigned int ui = 0; ui < 9; ui++)
    {
        bColorSet = bColorSet && pkElement->SetColors(
            iPolygon + ui, kBackgroundColor);
    }

    // Top left corner
    bTextureSet = bTextureSet && pkElement->SetTextures(iPolygon + 0, 
        0, kOuterUVs.m_left, kOuterUVs.m_top, 
        kInnerUVs.m_left, kInnerUVs.m_top);
    // Top middle edge
    bTextureSet = bTextureSet && pkElement->SetTextures(iPolygon + 1, 
        0, kInnerUVs.m_left, kOuterUVs.m_top, 
        kInnerUVs.m_right, kInnerUVs.m_top);
    // Top right corner
    bTextureSet = bTextureSet && pkElement->SetTextures(iPolygon + 2, 
        0, kInnerUVs.m_right, kOuterUVs.m_top, 
        kOuterUVs.m_right, kInnerUVs.m_top);
    // Middle left edge
    bTextureSet = bTextureSet && pkElement->SetTextures(iPolygon + 3, 
        0, kOuterUVs.m_left, kInnerUVs.m_top, 
        kInnerUVs.m_left, kInnerUVs.m_bottom);
    // Middle middle element
    bTextureSet = bTextureSet && pkElement->SetTextures(iPolygon + 4, 
        0, kInnerUVs.m_left, kInnerUVs.m_top, 
        kInnerUVs.m_right, kInnerUVs.m_bottom);
    // Middle right edge
    bTextureSet = bTextureSet && pkElement->SetTextures(iPolygon + 5, 
        0, kInnerUVs.m_right, kInnerUVs.m_top, 
        kOuterUVs.m_right, kInnerUVs.m_bottom);
    // Bottom left corner
    bTextureSet = bTextureSet && pkElement->SetTextures(iPolygon + 6, 
        0, kOuterUVs.m_left, kInnerUVs.m_bottom, 
        kInnerUVs.m_left, kOuterUVs.m_bottom);
    // Bottom middle edge
    bTextureSet = bTextureSet && pkElement->SetTextures(iPolygon + 7, 
        0, kInnerUVs.m_left, kInnerUVs.m_bottom, 
        kInnerUVs.m_right, kOuterUVs.m_bottom);
    // Bottom right corner
    bTextureSet = bTextureSet && pkElement->SetTextures(iPolygon + 8, 
        0, kInnerUVs.m_right, kInnerUVs.m_bottom, 
        kOuterUVs.m_right, kOuterUVs.m_bottom);

    if ((!bRectSet) || (!bColorSet) || (!bTextureSet))
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiUIBaseElement::HideNineQuad(NiMeshScreenElements* pkElement,
    int iPolygon)
{
    if (pkElement)
    {
        unsigned int uiStopPt = iPolygon + 9;
        unsigned int uiIdx;
        // This eliminates the UI element 
        for (uiIdx = iPolygon; uiIdx < uiStopPt; ++uiIdx)
        {
            pkElement->SetRectangle(uiIdx, 0.0f, 0.0f, 0.0f, 0.0f);
        }
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
