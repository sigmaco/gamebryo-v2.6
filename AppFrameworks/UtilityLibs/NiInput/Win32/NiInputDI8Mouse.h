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

#ifndef NIINPUTDI8MOUSE_H
#define NIINPUTDI8MOUSE_H

#include "NiInputMouse.h"
#include "NiDI8Headers.h"

class NIINPUT_ENTRY NiInputDI8Mouse : public NiInputMouse
{
    NiDeclareRTTI;

public:
    // construction
    NiInputDI8Mouse(NiInputDevice::Description* pkDescription,
        LPDIRECTINPUTDEVICE8 pkDIDevice, unsigned int uiUsage);
    // destruction
    ~NiInputDI8Mouse();

    //
    virtual NiInputErr UpdateDevice();
    virtual NiInputErr HandleRemoval();
    virtual NiInputErr HandleInsertion();

    // *** begin Emergent internal use only ***
    inline const LPDIRECTINPUTDEVICE8 GetDIDevice();
    // *** end Emergent internal use only ***

protected:
    void FlushBuffer();
    void UpdateImmediateData(DWORD dwData, unsigned int uiBtnOffset);
    void UpdateImmediatePositionData(int iX, int iY, int iZ);

    LPDIRECTINPUTDEVICE8 m_pkDIDevice;
    unsigned int m_uiBufferSize;

    unsigned int m_uiButtonTouchMask;
};

NiSmartPointer(NiInputDI8Mouse);

#include "NiInputDI8Mouse.inl"

#endif  //#ifndef NIINPUTDI8MOUSE_H
