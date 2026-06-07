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

#ifndef NIUIMANAGERRENDERCLICK_H
#define NIUIMANAGERRENDERCLICK_H

#include "NiUserInterfaceLibType.h"
#include <NiRenderClick.h>

class NIUSERINTERFACE_ENTRY NiUIManagerRenderClick : public NiRenderClick
{
    NiDeclareRTTI;

public:
    NiUIManagerRenderClick();

    // Functions for reporting statistics about the most recent frame.
    virtual unsigned int GetNumObjectsDrawn() const;
    virtual float GetCullTime() const;
    virtual float GetRenderTime() const;

protected:
    virtual void PerformRendering(unsigned int uiFrameID);

    // Rendering statistics.
    float m_fRenderTime;
};

NiSmartPointer(NiUIManagerRenderClick);

#endif  // #ifndef NIUIMANAGERRENDERCLICK_H
