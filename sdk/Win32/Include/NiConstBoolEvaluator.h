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

#ifndef NICONSTBOOLEVALUATOR_H
#define NICONSTBOOLEVALUATOR_H

#include "NiEvaluator.h"

class NIANIMATION_ENTRY NiConstBoolEvaluator : public NiEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiConstBoolEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    enum Channel
    {
        BOOL = EVALBASEINDEX
    };

    NiConstBoolEvaluator();
    NiConstBoolEvaluator(float fPosedBool);

    void SetPoseValue(float fPosedBool);

    // *** begin Emergent internal use only ***

    // Virtual function overrides from NiEvaluator.

    // GetChannelPosedValue returns a float via pvResult.
    virtual bool GetChannelPosedValue(unsigned int uiChannel, 
        void* pvResult) const;
    // UpdateChannel returns a float via pvResult.
    virtual bool UpdateChannel(float fTime, unsigned int uiChannel,
        NiEvaluatorSPData* pkEvalSPData, void* pvResult) const;
    virtual bool GetChannelScratchPadInfo(unsigned int uiChannel, 
        bool bForceAlwaysUpdate, NiAVObjectPalette* pkPalette, 
        unsigned int& uiFillSize, bool& bSharedFillData, 
        NiScratchPadBlock& eSPBSegmentData, 
        NiBSplineBasisData*& pkBasisData) const;
    virtual bool InitChannelScratchPadData(unsigned int uiChannel, 
        NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData* pkSPBasisData, 
        bool bInitSharedData, NiAVObjectPalette* pkPalette, 
        NiPoseBufferHandle kPBHandle) const;

    // *** end Emergent internal use only ***

protected:
    void SetEvalChannelTypes();

    float m_fBoolValue;
};

NiSmartPointer(NiConstBoolEvaluator);

#include "NiConstBoolEvaluator.inl"

#endif  // #ifndef NICONSTBOOLEVALUATOR_H
