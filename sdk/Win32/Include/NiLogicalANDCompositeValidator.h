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

#ifndef NILOGICALANDCOMPOSITEVALIDATOR_H
#define NILOGICALANDCOMPOSITEVALIDATOR_H

#include "NiCompositeValidator.h"

class NIMAIN_ENTRY NiLogicalANDCompositeValidator : public NiCompositeValidator
{
    NiDeclareRTTI;

public:
    // Function that returns true if all sub-validators return true and false
    // otherwise.
    virtual bool ValidateClick(NiRenderClick* pkRenderClick,
        unsigned int uiFrameID);
};

NiSmartPointer(NiLogicalANDCompositeValidator);

#endif  // #ifndef NILOGICALANDCOMPOSITEVALIDATOR_H
