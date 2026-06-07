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

#ifndef NIBSPLINEPOINT3INTERPOLATOR_H
#define NIBSPLINEPOINT3INTERPOLATOR_H

#include "NiBSplineInterpolator.h"
#include "NiPoint3.h"
#include "NiBSplineData.h"

class NIANIMATION_ENTRY NiBSplinePoint3Interpolator :
    public NiBSplineInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplinePoint3Interpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplinePoint3Interpolator();
    NiBSplinePoint3Interpolator(NiBSplineData* pkData,
        NiBSplineData::Handle kPoint3CPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineBasisData* pkBasisData = NULL);
    NiBSplinePoint3Interpolator(const NiPoint3& kPoseValue);

    enum Channel
    {
        POINT3
    };

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiPoint3& kValue);
    virtual bool IsPoint3ValueSupported() const;

    // Pose methods
    void SetPoseValue(const NiPoint3& rkPoseValue);

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
    NiPoint3 m_kPoint3Value;

    // Handles into the NiBSplineData for access to control points
    NiBSplineData::Handle m_kPoint3CPHandle;
};

NiSmartPointer(NiBSplinePoint3Interpolator);

//#include "NiBSplinePoint3Interpolator.inl"

#endif  // #ifndef NIBSPLINEPOINT3INTERPOLATOR_H
