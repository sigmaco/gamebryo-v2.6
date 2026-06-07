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

#ifndef NIBSPLINECOMPCOLOREVALUATOR_H
#define NIBSPLINECOMPCOLOREVALUATOR_H

#include "NiBSplineColorEvaluator.h"

class NIANIMATION_ENTRY NiBSplineCompColorEvaluator :
    public NiBSplineColorEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineCompColorEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineCompColorEvaluator();
    NiBSplineCompColorEvaluator(NiBSplineData* pkData,
        NiBSplineData::Handle kColorACPHandle,
        NiBSplineBasisData* pkBasisData);

    virtual bool UsesCompressedControlPoints() const;
    
    float GetOffset(unsigned short usChannel) const;
    void SetOffset(float fOffset, unsigned short usChannel);

    float GetHalfRange(unsigned short usChannel) const;
    void SetHalfRange(float fHalfRange, unsigned short usChannel);
    
    // *** begin Emergent internal use only ***

    // Virtual function overrides from NiEvaluator.
    virtual bool GetChannelPosedValue(unsigned int uiChannel, 
        void* pvResult) const;
    virtual bool GetChannelScratchPadInfo(unsigned int uiChannel, 
        bool bForceAlwaysUpdate, NiAVObjectPalette* pkPalette, 
        unsigned int& uiFillSize, bool& bSharedFillData, 
        NiScratchPadBlock& eSPBSegmentData, 
        NiBSplineBasisData*& pkBasisData) const;
    virtual bool InitChannelScratchPadData(unsigned int uiChannel, 
        NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData* pkSPBasisData, 
        bool bInitSharedData, NiAVObjectPalette* pkPalette, 
        NiPoseBufferHandle kPBHandle) const;

    static bool BSplineCompColorFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    // *** end Emergent internal use only ***

protected:
    enum
    {
        COLORA_OFFSET = 0,
        COLORA_RANGE,
        NUM_SCALARS  
    };
    float m_afCompScalars[NUM_SCALARS];
};

NiSmartPointer(NiBSplineCompColorEvaluator);

#endif  // #ifndef NIBSPLINECOMPCOLOREVALUATOR_H
