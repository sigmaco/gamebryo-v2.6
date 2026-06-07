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

#ifndef NIUIBASEELEMENT_H
#define NIUIBASEELEMENT_H

#include "NiUserInterfaceLibType.h"
#include <NiMeshScreenElements.h>
#include <NiRefObject.h>
#include <NiRenderer.h>
#include <NiRTTI.h>
#include <NiPoint2.h>
#include <NiTPtrSet.h>
#include "NiUISignal.h"
#include "NiUISlot.h"
#include "NiUIAtlasMap.h"

//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiUIBaseElement : public NiRefObject
{
    NiDeclareRootRTTI(NiUIBaseElement);
public:
    NiUIBaseElement(bool bCanTakeFocus);
    virtual ~NiUIBaseElement();

    NiUIBaseElement* GetParent() const;
    void SetParent(NiUIBaseElement* pkParent);

    // Interact with rectangle in Normalized SafeFrame Coordinates
    void SetDimensions(float fWidth, float fHeight);
    void SetOffset(float fLeftOffset, float fTopOffset);
    NiPoint2 GetOffset() const;
    NiPoint2 GetDimensions() const;
    NiRect<float> GetNSCRect() const;

    // Interact with rectangle in Pixel coordinates.
    // Note that the pixel offset is relative to the upper left corner
    // of the safe frame.
    NiRect<unsigned int> GetPixelRect() const;
    void SetPixelDimensions(unsigned int uiWidth, unsigned int uiHeight);
    void SetPixelOffset(unsigned int uiLeftOffset, unsigned int uiTopOffset);

    // Rendering functions
    virtual void Draw(NiRenderer* pkRenderer) = 0;

    // Event function
    virtual void EnterPressed(unsigned char ucButtonState) = 0;
    virtual void LeftMouseButton(unsigned char ucButtonState) = 0;

    virtual bool SetFocus(bool bFocus);

    inline bool IsVisible();
    virtual void SetVisible(bool bVisible);

    inline void SubscribeShown(NiUIBaseSlot0* pkSlot);
    inline void SubscribeHidden(NiUIBaseSlot0* pkSlot);
    inline void SubscribeResize(NiUIBaseSlot1<NiRect<float> >* pkSlot);
    inline void SubscribeGotFocus(NiUIBaseSlot0* pkSlot);
    inline void SubscribeLostFocus(NiUIBaseSlot0* pkSlot);

    virtual bool MouseLocation(unsigned int uiMouseX, unsigned int uiMouseY);

    virtual bool AttachResources(NiMeshScreenElements* pkScreenQuads);
    // Called to remove any trace of the child element
    virtual unsigned int GetChildElementCount() const;
    virtual NiUIBaseElement* GetChildElement(unsigned int uiElement) const;

    virtual void UpdateRect();

protected:
    virtual unsigned int NumQuadsRequired() = 0;

    virtual bool IsInside(unsigned int uiX, unsigned int uiY);
    // Called to indicate that some aspect of the rendering has changed
    // and all visual elements need to be rebuilt.
    virtual void UpdateLayout();
    virtual void UpdateChildRects();
    virtual void ReinitializeDisplayElements();
    virtual void ReinitializeChildDisplayElements();

    // Utility functions for computing pixel accurate to NSC rectangles
    static NiRect<float> ComputeInnerNSCRectPixelAccurateInner(
        NiUIAtlasMap::UIElement eElement, const NiRect<float>& kOuterNSCRect);
    static NiRect<float> ComputeInnerNSCRectPixelAccurateOuter(
        NiUIAtlasMap::UIElement eElement, const NiRect<float>& kOuterNSCRect);
    static NiRect<float> ComputeAspectRatioPreservingInnerNSCRect(
        NiUIAtlasMap::UIElement eElement, const NiRect<float>& kOuterNSCRect);

    static int InsertQuads(unsigned int uiNumQuads,
        NiMeshScreenElements* pkElements);
    static NiRect<float> OriginAndDimensionsToRect(float fX, float fY,
        float fWidth, float fHeight);

    // Fill in the vertex data in Normalized Safe Frame Coordinates (NSC) 
    // for a standard quad with a single UV channel.
    static bool InitializeQuad(NiMeshScreenElements* pkElement, int iPolygon,
        const NiRect<float>& kShapeRectInNSC, const NiRect<float>& kUVs, 
        const NiColorA& kBackgroundColor);
    static bool HideQuad(NiMeshScreenElements* pkElement, int iPolygon);

    // Fill in the vertex data for the following shape, defined by
    // an inner and outer quad in Normalized Safe Frame Coordinates (NSC):
    //                Outer
    //             ____________
    //            |_|________|_|
    //            | |        | |   
    //            | | Inner  | |
    //            |_|________|_|
    //            |_|________|_|
    static bool InitializeNineQuad(NiMeshScreenElements* pkElement, 
        int iPolygon, const NiRect<float>& kOuterRectInNSC, 
        const NiRect<float>& kInnerRectInNSC, const NiRect<float>& kOuterUVs, 
        const NiRect<float>& kInnerUVs, const NiColorA& kBackgroundColor);
    static bool HideNineQuad(NiMeshScreenElements* pkElement, int iPolygon);

    // Although these are redundant and fairly easy to calculate from
    // each other, they are both used during render time, so memory is
    // sacrificed for CPU efficiency
    NiRect<float> m_kNSCRect;
    NiRect<unsigned int> m_kPixelRect;

    NiPoint2 m_kLocalOffset;
    NiPoint2 m_kDimensions;
    NiUIBaseElement* m_pkParent;
    
    NiUISignal0 m_kShown;
    NiUISignal0 m_kHidden;
    NiUISignal0 m_kGotFocus;
    NiUISignal0 m_kLostFocus;
    NiUISignal1<NiRect<float> > m_kResized;

    int m_iBaseQuadIdx;
    NiMeshScreenElementsPtr m_spScreenElements;

    bool m_bFocussed;
    bool m_bVisible;
    bool m_bMouseInside;
    bool m_bCanTakeFocus;
};
NiSmartPointer(NiUIBaseElement);
//---------------------------------------------------------------------------
#include "NiUIBaseElement.inl"
//---------------------------------------------------------------------------

#endif
