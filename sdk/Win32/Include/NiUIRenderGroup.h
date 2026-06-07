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

#ifndef NIUIRENDERGROUP_H
#define NIUIRENDERGROUP_H

#include "NiUserInterfaceLibType.h"
#include "NiUIBaseElement.h"
//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiUIRenderGroup : public NiUIBaseElement
{
    NiDeclareRTTI;
public:
    NiUIRenderGroup(unsigned char ucDepth = 0);
    virtual ~NiUIRenderGroup();

    virtual void Draw(NiRenderer* pkRenderer);

    void InitializeScreenElements();

    virtual void AddChild(NiUIBaseElement* pkChild);
    virtual void RemoveChild(NiUIBaseElement* pkChild);

    inline unsigned char GetDepth();
    virtual void EnterPressed(unsigned char ucButtonState);
    virtual void LeftMouseButton(unsigned char ucButtonState);
    virtual void SetVisible(bool bVisible);

    virtual unsigned int GetChildElementCount() const;
    virtual NiUIBaseElement* GetChildElement(unsigned int uiElement) const;

protected:
    virtual unsigned int NumQuadsRequired();

    NiTObjectPtrSet<NiUIBaseElementPtr> m_kChildren;
    unsigned char m_ucDepth;
};
NiSmartPointer(NiUIRenderGroup);
//---------------------------------------------------------------------------
#include "NiUIRenderGroup.inl"

#endif

