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

#ifndef NIRENDERLISTPROCESSOR_H
#define NIRENDERLISTPROCESSOR_H

#include "NiRefObject.h"
#include "NiSmartPointer.h"
#include "NiRTTI.h"

class NiVisibleArray;

class NIMAIN_ENTRY NiRenderListProcessor : public NiRefObject
{
    NiDeclareRootRTTI(NiRenderListProcessor);

public:
    // Pre-processing function called before any objects are rendered.
    virtual void PreRenderProcessList(const NiVisibleArray* pkInput,
        NiVisibleArray& kOutput, void* pvExtraData);

    // Post-processing function called after the obejcts in the previous
    // ouptut array have been rendered by the render click.
    virtual void PostRenderProcessList(NiVisibleArray& kPreviousOutput, 
        void* pvExtraData);
};

NiSmartPointer(NiRenderListProcessor);

#endif  // #ifndef NIRENDERLISTPROCESSOR_H
