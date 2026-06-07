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

#ifndef NIRENDERCLICKVALIDATOR_H
#define NIRENDERCLICKVALIDATOR_H

#include "NiRefObject.h"
#include "NiSmartPointer.h"
#include "NiRTTI.h"

class NiRenderClick;

class NIMAIN_ENTRY NiRenderClickValidator : public NiRefObject
{
    NiDeclareRootRTTI(NiRenderClickValidator);

public:
    // Returns whether or not to execute the specified render click for the
    // specified frame.
    virtual bool ValidateClick(NiRenderClick* pkRenderClick,
        unsigned int uiFrameID) = 0;
};

NiSmartPointer(NiRenderClickValidator);

#endif  // #ifndef NIRENDERCLICKVALIDATOR_H
