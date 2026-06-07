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

#ifndef NIBSPLINETRANSFORMEVALUATOR_H
#define NIBSPLINETRANSFORMEVALUATOR_H

#include "NiBSplineEvaluator.h"
#include "NiQuatTransform.h"

class NIANIMATION_ENTRY NiBSplineTransformEvaluator : 
    public NiBSplineEvaluator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineTransformEvaluator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    enum Channel
    {
        POSITION = EVALPOSINDEX,
        ROTATION = EVALROTINDEX,
        SCALE = EVALSCALEINDEX
    };

    NiBSplineTransformEvaluator();
    NiBSplineTransformEvaluator(NiBSplineData* pkData,
        NiBSplineData::Handle kTransCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineData::Handle kRotCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineData::Handle kScaleCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineBasisData* pkBasisData = NULL);
    NiBSplineTransformEvaluator(const NiQuatTransform& kPoseValue);

    // Pose methods
    void SetPoseTranslate(const NiPoint3& kTranslate);
    void SetPoseRotate(const NiQuaternion& kRotate);
    void SetPoseScale(float fScale);
    void SetPoseValue(const NiQuatTransform& kPoseValue);

    // Implemented from NiBSplineEvaluator
    virtual unsigned short GetChannelCount() const;
    virtual unsigned int GetDimension(unsigned short usChannel) const;
    virtual unsigned int GetDegree(unsigned short usChannel) const;
    
    virtual NiBSplineData::Handle GetControlHandle(
        unsigned short usChannel) const;
    virtual void SetControlHandle(NiBSplineData::Handle kControlHandle, 
        unsigned short usChannel);

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

    static bool BSplineRotFillFunction(float fTime, 
        NiEvaluatorSPData* pkEvalSPData);

    // *** end Emergent internal use only ***

protected:
    // Only derived compact classes should call these constructors.
    NiBSplineTransformEvaluator(NiBSplineData* pkData,
        NiBSplineData::Handle kTransCPHandle,
        NiBSplineData::Handle kRotCPHandle,
        NiBSplineData::Handle kScaleCPHandle,
        NiBSplineBasisData* pkBasisData, 
        bool bCompactCPs);
    NiBSplineTransformEvaluator(const NiQuatTransform& kPoseValue, 
        bool bCompactCPs);

    void SetEvalChannelTypes();

    NiQuatTransform m_kTransformValue;

    // Handles into the NiBSplineData for access to control points
    NiBSplineData::Handle m_kTransCPHandle;
    NiBSplineData::Handle m_kRotCPHandle;
    NiBSplineData::Handle m_kScaleCPHandle;
};

NiSmartPointer(NiBSplineTransformEvaluator);

#endif  // #ifndef NIBSPLINETRANSFORMEVALUATOR_H
