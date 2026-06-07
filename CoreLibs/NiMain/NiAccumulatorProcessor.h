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

#ifndef NIACCUMULATORPROCESSOR_H
#define NIACCUMULATORPROCESSOR_H

#include "NiRenderListProcessor.h"
#include "NiAccumulator.h"

class NIMAIN_ENTRY NiAccumulatorProcessor : public NiRenderListProcessor
{
    NiDeclareRTTI;

public:
    NiAccumulatorProcessor(NiAccumulator* pkAccumulator = NULL,
        bool bFinishAccumulating = true);

    // Pre-processing function called before any objects are rendered.
    virtual void PreRenderProcessList(const NiVisibleArray* pkInput,
        NiVisibleArray& kOutput, void* pvExtraData);

    // Functions for accessing the NiAccumulator to use when rendering.
    inline void SetAccumulator(NiAccumulator* pkAccumulator);
    inline NiAccumulator* GetAccumulator() const;

    // Functions for accessing whether or not the accumulator should be told
    // to finish accumulating.
    inline void SetFinishAccumulating(bool bFinishAccumulating);
    inline bool GetFinishAccumulating() const;

protected:
    // The NiAccumulator to use when rendering.
    NiAccumulatorPtr m_spAccumulator;

    // A dummy camera object that is used to pass into the NiAccumulator.
    NiCameraPtr m_spDummyCamera;

    // Indicates whether or not the accumulator should be told to finish
    // accumulating.
    bool m_bFinishAccumulating;
};

NiSmartPointer(NiAccumulatorProcessor);

#include "NiAccumulatorProcessor.inl"

#endif  // #ifndef NIACCUMULATORPROCESSOR_H
