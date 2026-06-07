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

#ifndef NIALPHASORTPROCESSOR_H
#define NIALPHASORTPROCESSOR_H

#include "NiBackToFrontSortProcessor.h"
#include "NiRenderObject.h"

class NIMAIN_ENTRY NiAlphaSortProcessor : public NiBackToFrontSortProcessor
{
    NiDeclareRTTI;

public:
    NiAlphaSortProcessor(bool bObserveNoSortHint = true,
        bool bSortByClosestPoint = false);

    // Pre-processing function called before any objects are rendered.
    virtual void PreRenderProcessList(const NiVisibleArray* pkInput,
        NiVisibleArray& kOutput, void* pvExtraData);

    // Functions for specifying whether or not to observe the no sort hint on
    // NiAlphaPropertys when sorting.
    inline void SetObserveNoSortHint(bool bObserveNoSortHint);
    inline bool GetObserveNoSortHint() const;

protected:
    // Indicates whether or not a geometry object is transparent and should be
    // sorted.
    inline bool IsTransparent(const NiRenderObject& kObject);

    // Indicates whether or not to observe the no sort hint on
    // NiAlphaPropertys when sorting.
    bool m_bObserveNoSortHint;
};

NiSmartPointer(NiAlphaSortProcessor);

#include "NiAlphaSortProcessor.inl"

#endif  // #ifndef NIALPHASORTPROCESSOR_H
