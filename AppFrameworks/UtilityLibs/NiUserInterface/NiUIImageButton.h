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

#ifndef NIUIIMAGEBUTTON_H
#define NIUIIMAGEBUTTON_H

#include "NiUserInterfaceLibType.h"
#include "NiUIAtlasMap.h"
#include "NiUIBaseElement.h"

//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiUIImageButton : public NiUIBaseElement
{
    NiDeclareRTTI;
public:
    NiUIImageButton();
    virtual ~NiUIImageButton();

    // Rendering functions
    virtual void Draw(NiRenderer* pkRenderer);

    // Set and Get functions
    virtual void SetVisible(bool bVisible);
    inline void SetState(bool bToggleState);
    inline bool GetState() const;

    enum ImageState
    {
        NORMAL = 0,
        HIGHLIT,
        NUM_STATES
    };

    inline void SetAtlasImageForState(ImageState eState, bool bToggleState,
        NiUIAtlasMap::UIElement eElement);

    // Event functions
    virtual void EnterPressed(unsigned char ucButtonState);
    virtual void LeftMouseButton(unsigned char ucButtonState);
    virtual bool MouseLocation(unsigned int uiMouseX, unsigned int uiMouseY);
    inline void SubscribeToToggleEvent(NiUIBaseSlot1<bool>* pkSlot);

    virtual bool SetFocus(bool bFocus);

protected:
    enum {NUM_QUADS = 1};
    virtual unsigned int NumQuadsRequired();
    virtual void ReinitializeDisplayElements();
    virtual void UpdateLayout();

    // Communication members
    NiUISignal1<bool> m_kToggleEvent;

    bool m_bToggleState;
    ImageState m_eImageState;
    NiUIAtlasMap::UIElement m_aeOnElements[NUM_STATES];
    NiUIAtlasMap::UIElement m_aeOffElements[NUM_STATES];
};
NiSmartPointer(NiUIImageButton);
//---------------------------------------------------------------------------
#include "NiUIImageButton.inl"

#endif
