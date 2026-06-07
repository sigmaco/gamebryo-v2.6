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

#ifndef NIEVALUATORSPDATA_H
#define NIEVALUATORSPDATA_H

#include <NiMath.h>
#include "NiAnimationLibType.h"
#include "NiScratchPadBlock.h"
#include "NiEvaluator.h"


// This class acts as the main entry point into the scratch pad 
// for a given evaluator channel.
class NIANIMATION_ENTRY NiEvaluatorSPData : public NiMemObject
{
public:
    // *** begin Emergent internal use only ***

    enum EvalChannelIndex
    {
        EVALBASEINDEX = NiEvaluator::EVALBASEINDEX,
        EVALPOSINDEX = NiEvaluator::EVALPOSINDEX,
        EVALROTINDEX = NiEvaluator::EVALROTINDEX,
        EVALSCALEINDEX = NiEvaluator::EVALSCALEINDEX,
    };

    enum
    {
        FLAGALWAYSUPDATE = 0x1, 
        FLAGSTEPFUNCTION = 0x2, 
        FLAGALTERNATESPDATA = 0x4,
    };

    // Constructor.
    NiEvaluatorSPData();
    virtual ~NiEvaluatorSPData();

    NiEvaluator* GetEvaluator() const;
    void SetEvaluator(NiEvaluator* pkEvaluator);

    NiInt16 GetLOD() const;
    void SetLOD(NiInt16 sLOD);

    EvalChannelIndex GetEvalChannelIndex() const;
    void SetEvalChannelIndex(EvalChannelIndex eEvalChannelIndex);

    NiUInt8 GetFlags() const;
    void SetFlags(NiUInt8 ucFlags);
    bool AlwaysUpdate() const;
    bool IsStepFunction() const;
    void SetStepFunction(bool bEnabled = true);
    bool IsAlternateSPData() const;
    void SetAlternateSPData(bool bEnabled = true);

    float GetSPSegmentMinTime() const;
    float GetSPSegmentMaxTime() const;
    void SetSPSegmentTimeRange(float fMinTime, float fMaxTime);
    bool IsSPSegmentDataValid(float fTime) const;

    NiScratchPadFillFunc GetSPFillFunc() const;
    void SetSPFillFunc(NiScratchPadFillFunc pfnSPFillFunc);

    void* GetSPFillData() const;
    void SetSPFillData(void* pvSPFillData);

    void* GetSPSegmentData() const;
    void SetSPSegmentData(void* pvSPSegmentData);

    // *** end Emergent internal use only ***

protected:
    NiEvaluator* m_pkEvaluator;
    NiInt16 m_sLOD; // LOD for corresponding bone
    NiUInt8 m_ucEvalChannelIndex;
    NiUInt8 m_ucFlags;
    // Range in local sequence time when the scratch pad segment data is valid.
    float m_fSPSegmentMinTime;
    float m_fSPSegmentMaxTime;
    NiScratchPadFillFunc m_pfnSPFillFunc;
    void* m_pvSPFillData; // do not free
    void* m_pvSPSegmentData; // do not free
};

#include "NiEvaluatorSPData.inl"

#endif  // #ifndef NIEVALUATORSPDATA_H
