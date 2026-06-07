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

#ifndef NIBSPLINEFLOATINTERPOLATOR_H
#define NIBSPLINEFLOATINTERPOLATOR_H

#include "NiBSplineInterpolator.h"
#include "NiBSplineData.h"

class NIANIMATION_ENTRY NiBSplineFloatInterpolator :
    public NiBSplineInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineFloatInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineFloatInterpolator();
    NiBSplineFloatInterpolator(NiBSplineData* pkData,
        NiBSplineData::Handle kFloatCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineBasisData* pkBasisData = NULL);
    NiBSplineFloatInterpolator(float fPoseValue);

    enum Channel
    {
        FLOAT
    };

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        float& fValue);
    virtual bool IsFloatValueSupported() const;

    // Pose methods
    void SetPoseValue(float fPoseValue);

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
    float m_fFloatValue;
    // Handles into the NiBSplineData for access to control points
    NiBSplineData::Handle m_kFloatCPHandle;
};

NiSmartPointer(NiBSplineFloatInterpolator);

//#include "NiBSplineFloatInterpolator.inl"

#endif  // #ifndef NIBSPLINEFLOATINTERPOLATOR_H
