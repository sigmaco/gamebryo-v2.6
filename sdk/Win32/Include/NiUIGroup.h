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

#ifndef NIUIGROUP_H
#define NIUIGROUP_H

//---------------------------------------------------------------------------
#include "NiUserInterfaceLibType.h"
#include <NiString.h>
#include <NiPoint2.h>
#include <NiInput.h>
#include "NiUIRenderGroup.h"
#include "NiUIImageButton.h"
#include "NiUILabel.h"

//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiUIGroup : public NiUIRenderGroup
{
    NiDeclareRTTI;
public:
    NiUIGroup(const char* pcGroupName, float fHeaderHeight,
        bool bExpanded = true);
    virtual ~NiUIGroup();

    virtual void AddChild(NiUIBaseElement* pkChild);
    virtual void RemoveChild(NiUIBaseElement* pkChild);

    virtual bool AdvanceFocus();
    virtual bool RegressFocus();
    virtual bool SetFocus(bool bFocus);
    virtual void EnterPressed(unsigned char ucButtonState);
    virtual bool IsInside(unsigned int uiMouseX, unsigned int uiMouseY);
    virtual void LeftMouseButton(unsigned char ucButtonState);
    virtual void SetVisible(bool bVisible);

    inline void SubscribeToExpandToggleEvent(NiUIBaseSlot1<bool>* pkSlot);
    inline void SetExpandState(bool bExpand);

    virtual void Draw(NiRenderer* pkRenderer);
    
    virtual unsigned int GetChildElementCount() const;
    virtual NiUIBaseElement* GetChildElement(unsigned int uiElement) const;

    virtual void UpdateRect();

    virtual NiUIBaseElement* GetFocusedElement();

protected:
    NiUIGroup() {}

    enum {NUM_QUADS = 9};
    virtual unsigned int NumQuadsRequired();
    virtual void ReinitializeDisplayElements();
    void ComputeBounds();

    virtual void UpdateLayout();
    float m_fHeaderHeight;

    void Expand(bool bExpand);

    NiUILabelPtr m_spLabel;
    bool m_bExpanded;

    NiRect<float> m_kFrameOuterRectInNSC;
    NiRect<float> m_kFrameInnerRectInNSC;

    NiUIImageButtonPtr m_spExpandButton;
    NiUIMemberSlot1<NiUIGroup, bool> m_kExpandedSlot;
    NiRect<unsigned int> m_kBounds;
    int m_iFocussedChild;
};
NiSmartPointer(NiUIGroup);
//---------------------------------------------------------------------------
#include "NiUIGroup.inl"

#endif
