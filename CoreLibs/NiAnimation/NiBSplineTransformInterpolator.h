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

#ifndef NIBSPLINETRANSFORMINTERPOLATOR_H
#define NIBSPLINETRANSFORMINTERPOLATOR_H

#include "NiBSplineInterpolator.h"
#include "NiQuatTransform.h"
#include "NiBSplineData.h"

class NIANIMATION_ENTRY NiBSplineTransformInterpolator :
    public NiBSplineInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineTransformInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineTransformInterpolator();
    NiBSplineTransformInterpolator(NiBSplineData* pkData,
        NiBSplineData::Handle kTransCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineData::Handle kRotCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineData::Handle kScaleCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineBasisData* pkBasisData = NULL);
    NiBSplineTransformInterpolator(const NiQuatTransform& kPoseValue);

    enum Channel
    {
        POSITION,
        ROTATION,
        SCALE
    };

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiQuatTransform& kValue);
    virtual bool IsTransformValueSupported() const;

    // Pose methods
    void SetPoseTranslate(const NiPoint3& kTranslate);
    void SetPoseRotate(const NiQuaternion& kRotate);
    void SetPoseScale(float fScale);
    void SetPoseValue(const NiQuatTransform& kPoseValue);

    // Implemented from NiBSplineInterpolator
    virtual unsigned short GetChannelCount() const;
    virtual unsigned int GetDimension(unsigned short usChannel) const;
    virtual unsigned int GetDegree(unsigned short usChannel) const;
    
    virtual bool GetChannelPosed(unsigned short usChannel) const;
    
    virtual NiBSplineData::Handle GetControlHandle(
        unsigned short usChannel) const;
    virtual void SetControlHandle(NiBSplineData::Handle kControlHandle, 
        unsigned short usChannel);

protected:
    NiQuatTransform m_kTransformValue;

    // Handles into the NiBSplineData for access to control points
    NiBSplineData::Handle m_kTransCPHandle;
    NiBSplineData::Handle m_kRotCPHandle;
    NiBSplineData::Handle m_kScaleCPHandle;
};

NiSmartPointer(NiBSplineTransformInterpolator);

//#include "NiBSplineTransformInterpolator.inl"

#endif  // #ifndef NIBSPLINETRANSFORMINTERPOLATOR_H
