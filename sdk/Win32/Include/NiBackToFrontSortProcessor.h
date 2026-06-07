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

#ifndef NIBACKTOFRONTSORTPROCESSOR_H
#define NIBACKTOFRONTSORTPROCESSOR_H

#include "NiRenderListProcessor.h"

#include "NiRenderObject.h"

class NIMAIN_ENTRY NiBackToFrontSortProcessor : public NiRenderListProcessor
{
    NiDeclareRTTI;

public:
    NiBackToFrontSortProcessor(bool bSortByClosestPoint = false);
    virtual ~NiBackToFrontSortProcessor();

    // Pre-processing function called before any objects are rendered.
    virtual void PreRenderProcessList(const NiVisibleArray* pkInput,
        NiVisibleArray& kOutput, void* pvExtraData);

    // Functions for specifying sort method.
    inline void SetSortByClosestPoint(bool bSortByClosestPoint);
    inline bool GetSortByClosestPoint() const;

protected:
    // Computes the depth of a geometry object along a particular view vector.
    inline float ComputeDepth(const NiRenderObject& kGeometry, const NiPoint3& kDir);

    // Sorts objects in the specified array using the depth values stored in
    // m_kObjectDepths. Requires m_kObjectDepths to be properly populated.
    void SortObjectsByDepth(NiVisibleArray& kArrayToSort, int l, int r);

    // Internal helper function for sorting.
    float ChoosePivot(int l, int r) const;

    // Array of object depths used during sorting.
    float* m_pfDepths;
    unsigned int m_uiAllocatedDepths;

    // Indicates whether bounding volume centers or closest points should be
    // used when sorting.
    bool m_bSortByClosestPoint;
};

NiSmartPointer(NiBackToFrontSortProcessor);

#include "NiBackToFrontSortProcessor.inl"

#endif  // #ifndef NIBACKTOFRONTSORTPROCESSOR_H
