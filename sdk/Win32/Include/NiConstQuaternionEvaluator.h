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

#ifndef NICONSTQUATERNIONEVALUATOR_H
#define NICONSTQUATERNIONEVALUATOR_H

#include "NiEvaluator.h"
#include "NiQuaternion.h"

class NIANIMATION_ENTRY NiConstQuaternionEvaluator : public NiEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiConstQuaternionEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    enum Channel
    {
        ROTATION = EVALBASEINDEX
    };

    NiConstQuaternionEvaluator();
    NiConstQuaternionEvaluator(const NiQuaternion& kPoseValue);

    void SetPoseValue(const NiQuaternion& kPoseValue);

    // *** begin Emergent internal use only ***

    // Virtual function overrides from NiEvaluator.
    virtual bool GetChannelPosedValue(unsigned int uiChannel, 
        void* pvResult) const;
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

    NiQuaternion m_kQuaternionValue;
};

NiSmartPointer(NiConstQuaternionEvaluator);

#include "NiConstQuaternionEvaluator.inl"

#endif  // #ifndef NICONSTQUATERNIONEVALUATOR_H
