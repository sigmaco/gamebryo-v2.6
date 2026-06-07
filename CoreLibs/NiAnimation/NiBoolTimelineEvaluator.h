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

#ifndef NIBOOLTIMELINEEVALUATOR_H
#define NIBOOLTIMELINEEVALUATOR_H

#include "NiBoolEvaluator.h"

class NIANIMATION_ENTRY NiBoolTimelineEvaluator : public NiBoolEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBoolTimelineEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBoolTimelineEvaluator();
    NiBoolTimelineEvaluator(NiBoolData* pkBoolData);

    // *** begin Emergent internal use only ***

    virtual bool GetChannelScratchPadInfo(unsigned int uiChannel, 
        bool bForceAlwaysUpdate, NiAVObjectPalette* pkPalette, 
        unsigned int& uiFillSize, bool& bSharedFillData, 
        NiScratchPadBlock& eSPBSegmentData, 
        NiBSplineBasisData*& pkBasisData) const;
    virtual bool InitChannelScratchPadData(unsigned int uiChannel, 
        NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData* pkSPBasisData, 
        bool bInitSharedData, NiAVObjectPalette* pkPalette, 
        NiPoseBufferHandle kPBHandle) const;

    static bool ConstantBoolTimelineFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    // *** end Emergent internal use only ***
};

NiSmartPointer(NiBoolTimelineEvaluator);

#endif  // #ifndef NIBOOLTIMELINEEVALUATOR_H
