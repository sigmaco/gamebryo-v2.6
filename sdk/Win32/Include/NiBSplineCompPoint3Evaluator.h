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

#ifndef NIBSPLINECOMPPOINT3EVALUATOR_H
#define NIBSPLINECOMPPOINT3EVALUATOR_H

#include "NiBSplinePoint3Evaluator.h"

class NIANIMATION_ENTRY NiBSplineCompPoint3Evaluator :
    public NiBSplinePoint3Evaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineCompPoint3Evaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineCompPoint3Evaluator();
    NiBSplineCompPoint3Evaluator(NiBSplineData* pkData,
        NiBSplineData::Handle kPoint3CPHandle,
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

    static bool BSplineCompPoint3FillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    // *** end Emergent internal use only ***
    
protected:
    enum
    {
        POINT3_OFFSET = 0,
        POINT3_RANGE,
        NUM_SCALARS  
    };
    float m_afCompScalars[NUM_SCALARS];
};

NiSmartPointer(NiBSplineCompPoint3Evaluator);

#endif  // #ifndef NIBSPLINECOMPPOINT3EVALUATOR_H
