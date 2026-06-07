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

#ifndef NIUIIMAGE_H
#define NIUIIMAGE_H

#include "NiUserInterfaceLibType.h"
#include <NiRect.h>
#include "NiUIBaseElement.h"
//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiUIImage : public NiUIBaseElement
{
    NiDeclareRTTI;
public:
    NiUIImage();
    NiUIImage(NiRect<float> kTextureCoords);
    virtual ~NiUIImage();

    virtual void Draw(NiRenderer* pkRenderer);

    inline void SetTextureCoords(NiRect<float> kTextureCoords);
    virtual void EnterPressed(unsigned char ucButtonState);
    virtual void LeftMouseButton(unsigned char ucButtonState);
protected:
    virtual unsigned int NumQuadsRequired();
    virtual void ReinitializeDisplayElements();

    NiRect<float> m_kTextureCoords;
};
NiSmartPointer(NiUIImage);
//---------------------------------------------------------------------------
#include "NiUIImage.inl"

#endif

