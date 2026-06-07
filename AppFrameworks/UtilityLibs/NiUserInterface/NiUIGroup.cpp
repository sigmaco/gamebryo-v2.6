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

#include "NiUIGroup.h"
#include "NiUIManager.h"
#include "NiUIImageButton.h"

NiImplementRTTI(NiUIGroup, NiUIRenderGroup);

//---------------------------------------------------------------------------
NiUIGroup::NiUIGroup(const char* pcGroupName, float fHeaderHeight,
    bool bExpanded) :
    NiUIRenderGroup(2),
    m_fHeaderHeight(fHeaderHeight),
    m_spLabel(NULL),
    m_bExpanded(bExpanded),
    m_spExpandButton(NULL),
    m_iFocussedChild(-2)
{
    m_kExpandedSlot.Initialize(this, &NiUIGroup::Expand);
    NiColor kTextColor = NiColor::BLACK;
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("UIGroup initialized before the NiUIManager.  "
            "This can lead to unexpected results.");
    }
    else
    {
        kTextColor = NiColor(NiUIManager::GetUIManager()->GetAtlasMap()->
            GetTextColor(NiUIAtlasMap::TEXT_ON_UI_DEFAULT));
    }
    
    m_spLabel = NiNew NiUILabel(pcGroupName, NiColorA(0.0f, 0.0f, 0.0f, 0.0f),
        kTextColor);
    m_spLabel->SetParent(this);
    m_spLabel->SetAlignment(NiUILabel::VERT_MIDDLE, NiUILabel::LEFT);
    
    m_spExpandButton = NiNew NiUIImageButton();
    m_spExpandButton->SetState(bExpanded);
    m_spExpandButton->SetAtlasImageForState(NiUIImageButton::NORMAL, true,
        NiUIAtlasMap::UI_MIN_DEFAULT);
    m_spExpandButton->SetAtlasImageForState(NiUIImageButton::HIGHLIT, true,
        NiUIAtlasMap::UI_MIN_HIGH);
    m_spExpandButton->SetAtlasImageForState(NiUIImageButton::NORMAL, false,
        NiUIAtlasMap::UI_MAX_DEFAULT);
    m_spExpandButton->SetAtlasImageForState(NiUIImageButton::HIGHLIT, false,
        NiUIAtlasMap::UI_MAX_HIGH);
    m_spExpandButton->SetParent(this);
    m_spExpandButton->SubscribeToToggleEvent(&m_kExpandedSlot);
   
    m_bCanTakeFocus = true;
}
//---------------------------------------------------------------------------
NiUIGroup::~NiUIGroup()
{
    m_spExpandButton = NULL;
}
//---------------------------------------------------------------------------
void NiUIGroup::Expand(bool bExpand)
{
    m_bExpanded = bExpand;

    for (unsigned int ui = 0; ui < m_kChildren.GetSize(); ui++)
    {
        NiUIBaseElement* pkElement = m_kChildren.GetAt(ui);
        if (pkElement)
            pkElement->SetVisible(m_bExpanded);
    }

    UpdateRect();
    ReinitializeDisplayElements();
}
//---------------------------------------------------------------------------
void NiUIGroup::ComputeBounds()
{
    m_kBounds = GetPixelRect();
}
//---------------------------------------------------------------------------
void NiUIGroup::AddChild(NiUIBaseElement* pkChild)
{
    if (pkChild == NULL)
        return;

    NiUIBaseElementPtr spChild = pkChild;
    pkChild->SetVisible(m_bExpanded);
    m_kChildren.AddUnique(spChild);
    spChild->SetParent(this);
}
//---------------------------------------------------------------------------
void NiUIGroup::RemoveChild(NiUIBaseElement* pkChild)
{
    if (pkChild == NULL)
        return;
    NiUIRenderGroup::RemoveChild(pkChild);
}
//---------------------------------------------------------------------------
void NiUIGroup::UpdateRect()
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer == NULL)
    {
        NiOutputDebugString("NiUIGroup::UpdateRect(...) called before "
            "a valid NiRenderer has been created.  This is explicitly "
            "not allowed.");
        return;
    }

    NiPoint2 kOffset = m_kLocalOffset;
    NiPoint2 kDimensions = m_kDimensions;

    if (!m_bExpanded)
    {
        kDimensions.y = m_fHeaderHeight;
    }

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
NiUIBaseElement* NiUIGroup::GetFocusedElement()
{
    if (m_iFocussedChild == -2)
        return NULL;
    if (m_iFocussedChild == -1)
        return m_spExpandButton;
    else
        return m_kChildren.GetAt((unsigned int)m_iFocussedChild);
}
//---------------------------------------------------------------------------
void NiUIGroup::ReinitializeDisplayElements()
{
    if (m_spScreenElements == NULL)
        return;

    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString(
            "NiUIButton::ReinitializeDisplayElements called without "
            "NiUIManager initialization.  This is prohibited.");
        return;
    }

    if (m_bVisible)
    {
        const NiUIAtlasMap* pkAtlas = NiUIManager::GetUIManager()->
            GetAtlasMap();
    
        NiRect<float> kOuterUVs;
        NiRect<float> kInnerUVs;
        NiUIAtlasMap::UIElement kStateName = NiUIAtlasMap::UI_GROUP;

        kOuterUVs = pkAtlas->GetOuterBounds(kStateName);
        kInnerUVs = pkAtlas->GetInnerBounds(kStateName);
        
        // Compute internal rect
        bool bSuccess = InitializeNineQuad(m_spScreenElements, m_iBaseQuadIdx,
            m_kFrameOuterRectInNSC, m_kFrameInnerRectInNSC, kOuterUVs, 
            kInnerUVs, NiColorA::WHITE);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUIButton.  "
                "Probable cause: invalid uiBaseIdx passed in to "
                "NiUIButton::ReinitializeDisplayElements.");
        }
    }
    else
    {
        bool bSuccess = HideNineQuad(m_spScreenElements, m_iBaseQuadIdx);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUIButton.  "
                "Probable cause: invalid uiBaseIdx passed in to "
                "NiUIButton::ReinitializeDisplayElements.");
        }
    }
    ReinitializeChildDisplayElements();
}
//---------------------------------------------------------------------------
void NiUIGroup::EnterPressed(unsigned char ucButtonState)
{
    if (!m_bVisible)
        return;

    if (m_iFocussedChild == -1)
    {
        m_spExpandButton->EnterPressed(ucButtonState);
        return;
    }
    if ((m_iFocussedChild < 0) || 
        (((unsigned int)m_iFocussedChild) >= m_kChildren.GetSize()))
    {
        return;
    }
    m_kChildren.GetAt(m_iFocussedChild)->EnterPressed(ucButtonState);
}
//---------------------------------------------------------------------------
void NiUIGroup::LeftMouseButton(unsigned char ucButtonState)
{
    if (!m_bVisible)
        return;

    if (m_iFocussedChild == -1)
    {
        m_spExpandButton->LeftMouseButton(ucButtonState);
        return;
    }
    if ((m_iFocussedChild < 0) ||
        (((unsigned int)m_iFocussedChild) >= m_kChildren.GetSize()))
    {
        return;
    }
    m_kChildren.GetAt(m_iFocussedChild)->LeftMouseButton(ucButtonState);
}
//---------------------------------------------------------------------------
bool NiUIGroup::SetFocus(bool bFocus)
{
    int iOldFocus = m_iFocussedChild;
    
    if (bFocus == false)
    {
        m_iFocussedChild = -2;
    }
    else if (bFocus && m_iFocussedChild < 0)
    {
        m_iFocussedChild = -1;
        iOldFocus = -1;
    }
    
    if (iOldFocus == -1)
    {
        m_spExpandButton->SetFocus(bFocus);
        return true;
    }
    if ((iOldFocus < 0) || 
        (((unsigned int)iOldFocus) >= m_kChildren.GetSize()))
    {
        return true;
    }
    m_kChildren.GetAt(iOldFocus)->SetFocus(bFocus);
    return true;
}
//---------------------------------------------------------------------------
bool NiUIGroup::AdvanceFocus()
{
    ++m_iFocussedChild;
    if (m_iFocussedChild == -1)
    {
        m_spExpandButton->SetFocus(true);
        return true;
    }

    if (!m_spExpandButton->GetState())
    {
        // We've made it past the hide/show button and no one else is visible
        m_spExpandButton->SetFocus(false);
        m_iFocussedChild = -2;
        return false;
    }

    unsigned int uiFocussedChild = m_iFocussedChild;
    if (uiFocussedChild < m_kChildren.GetSize())
    {
        if (uiFocussedChild > 0)
            m_kChildren.GetAt(uiFocussedChild - 1)->SetFocus(false);
        else
            m_spExpandButton->SetFocus(false);

        while (!m_kChildren.GetAt(uiFocussedChild)->SetFocus(true))
        {
            m_kChildren.GetAt(uiFocussedChild)->SetFocus(false);
            ++uiFocussedChild;
            if (uiFocussedChild >= m_kChildren.GetSize())
            {
                m_iFocussedChild = -2;
                return false;
            }
        }
        m_iFocussedChild = uiFocussedChild;
        return true;
    }
    else
    {
        if ((uiFocussedChild == m_kChildren.GetSize()) && 
            (uiFocussedChild != 0))
        {
            m_kChildren.GetAt(uiFocussedChild - 1)->SetFocus(false);
        }
        else
        {
            m_spExpandButton->SetFocus(false);
        }
        m_iFocussedChild = -2;
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiUIGroup::RegressFocus()
{
    if (m_iFocussedChild < -1)
    {
        if (m_spExpandButton->GetState())
            m_iFocussedChild = m_kChildren.GetSize();
        else
            m_iFocussedChild = 0;
    }

    --m_iFocussedChild;

    if (m_iFocussedChild == -1)
    {
        if (m_spExpandButton->GetState() && (m_kChildren.GetSize() > 0))
            m_kChildren.GetAt(0)->SetFocus(false);
        m_spExpandButton->SetFocus(true);

        return true;
    }
    else if (m_iFocussedChild < -1)
    {
        m_spExpandButton->SetFocus(false);
        return false;
    }

    unsigned int uiFocussedChild = m_iFocussedChild;

    if ((uiFocussedChild + 1) < m_kChildren.GetSize())
        m_kChildren.GetAt(uiFocussedChild + 1)->SetFocus(false);
    while (!m_kChildren.GetAt(uiFocussedChild)->SetFocus(true))
    {
        m_kChildren.GetAt(uiFocussedChild)->SetFocus(false);
        if (uiFocussedChild == 0)
        {
            m_iFocussedChild = -1;
            m_spExpandButton->SetFocus(true);
            return true;
        }
        --uiFocussedChild;
    }
    m_iFocussedChild = uiFocussedChild;

    return true;
}
//---------------------------------------------------------------------------
bool NiUIGroup::IsInside(unsigned int uiMouseX, unsigned int uiMouseY)
{
    if ((uiMouseX >= m_kBounds.m_left) && (uiMouseX <= m_kBounds.m_right) &&
        (uiMouseY >= m_kBounds.m_top) && (uiMouseY <= m_kBounds.m_bottom))
    {
        unsigned int uiIdx, uiSize;
        uiSize = m_kChildren.GetSize();

        if (m_spExpandButton->MouseLocation(uiMouseX, uiMouseY))
        {
            // The next if takes the focus off of the old child that might 
            // have been focussed
            if ((m_iFocussedChild != -1) && (m_iFocussedChild >= 0) && 
                (((unsigned int)m_iFocussedChild) < uiSize))
            {
                m_kChildren.GetAt(m_iFocussedChild)->SetFocus(false);
            }
            m_iFocussedChild = -1;
            return true;
        }
        if (m_spExpandButton->GetState())
        {
            for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
            {
                if (m_kChildren.GetAt(uiIdx)->MouseLocation(uiMouseX, 
                    uiMouseY))
                {
                    // The next two ifs take the focus off of the old
                    // child that might have been focussed
                    if (m_iFocussedChild == -1)
                        m_spExpandButton->SetFocus(false);
                    if ((m_iFocussedChild != (int)uiIdx) && 
                        (m_iFocussedChild >= 0) && 
                        (((unsigned int)m_iFocussedChild) < uiSize))
                    {
                        m_kChildren.GetAt(m_iFocussedChild)->SetFocus(false);
                    }
                    m_iFocussedChild = uiIdx;
                    return true;
                }
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
unsigned int NiUIGroup::GetChildElementCount() const
{
    return NiUIRenderGroup::GetChildElementCount() + 2;
}
//---------------------------------------------------------------------------
NiUIBaseElement* NiUIGroup::GetChildElement(unsigned int uiElement) const
{
    unsigned int uiRGCount = NiUIRenderGroup::GetChildElementCount();
    if (uiElement < uiRGCount)
        return NiUIRenderGroup::GetChildElement(uiElement);

    uiElement -= uiRGCount;

    switch (uiElement)
    {
        case 0:
            return m_spExpandButton;
        case 1:
            return m_spLabel;
        default:
            return NULL;
    }
}
//---------------------------------------------------------------------------
void NiUIGroup::UpdateLayout()
{
    m_kFrameOuterRectInNSC = m_kNSCRect;
    m_kFrameInnerRectInNSC = ComputeInnerNSCRectPixelAccurateOuter(
        NiUIAtlasMap::UI_GROUP, m_kFrameOuterRectInNSC);

    NiPoint2 kOffset( 
        m_kFrameInnerRectInNSC.m_left - m_kFrameOuterRectInNSC.m_left,
        m_kFrameInnerRectInNSC.m_top - m_kFrameOuterRectInNSC.m_top);
    float fWidth = m_kFrameInnerRectInNSC.GetWidth();
    float fHeight = m_fHeaderHeight - kOffset.y * 2.0f;

    m_spLabel->SetOffset(kOffset.x, kOffset.y);
    m_spLabel->SetDimensions(0.9f * fWidth, fHeight);
    m_spExpandButton->SetOffset(0.9f * fWidth + kOffset.x, kOffset.y);
    m_spExpandButton->SetDimensions(0.1f * fWidth, fHeight);

    ComputeBounds();
}
//---------------------------------------------------------------------------
void NiUIGroup::Draw(NiRenderer* pkRenderer)
{
     NiUIRenderGroup::Draw(pkRenderer);
     if (m_bVisible)
     {
         m_spLabel->Draw(pkRenderer);
         m_spExpandButton->Draw(pkRenderer);
     }
}
//---------------------------------------------------------------------------
