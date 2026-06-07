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

#ifndef NIBSPLINECOMPTRANSFORMEVALUATOR_H
#define NIBSPLINECOMPTRANSFORMEVALUATOR_H

#include "NiBSplineTransformEvaluator.h"

class NIANIMATION_ENTRY NiBSplineCompTransformEvaluator :
    public NiBSplineTransformEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineCompTransformEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineCompTransformEvaluator();
    NiBSplineCompTransformEvaluator(NiBSplineData* pkData,
        NiBSplineData::Handle kTransCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineData::Handle kRotCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineData::Handle kScaleCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineBasisData* pkBasisData = NULL);
    NiBSplineCompTransformEvaluator(NiQuatTransform kPoseValue);

    virtual bool UsesCompressedControlPoints() const;

    float GetOffset(unsigned short usChannel) const;
    void SetOffset(float fOffset, unsigned short usChannel);

    float GetHalfRange(unsigned short usChannel) const;
    void SetHalfRange(float fHalfRange, unsigned short usChannel);
    
    // *** begin Emergent internal use only ***

    // Virtual function overrides from NiEvaluator.
    virtual bool GetChannelScratchPadInfo(unsigned int uiChannel, 
        bool bForceAlwaysUpdate, NiAVObjectPalette* pkPalette, 
        unsigned int& uiFillSize, bool& bSharedFillData, 
        NiScratchPadBlock& eSPBSegmentData, 
        NiBSplineBasisData*& pkBasisData) const;
    virtual bool InitChannelScratchPadData(unsigned int uiChannel, 
        NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData* pkSPBasisData, 
        bool bInitSharedData, NiAVObjectPalette* pkPalette, 
        NiPoseBufferHandle kPBHandle) const;

    static bool BSplineCompRotFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    // *** end Emergent internal use only ***

protected:
    enum
    {
        POSITION_OFFSET = 0,
        POSITION_RANGE,
        ROTATION_OFFSET,
        ROTATION_RANGE,
        SCALE_OFFSET,
        SCALE_RANGE,
        NUM_SCALARS  
    };
    float m_afCompScalars[NUM_SCALARS];
};

NiSmartPointer(NiBSplineCompTransformEvaluator);

#endif  // #ifndef NIBSPLINECOMPTRANSFORMEVALUATOR_H
