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

#ifndef NIBSPLINECOLORINTERPOLATOR_H
#define NIBSPLINECOLORINTERPOLATOR_H

#include "NiBSplineInterpolator.h"
#include "NiColor.h"
#include "NiBSplineData.h"

class NIANIMATION_ENTRY NiBSplineColorInterpolator :
    public NiBSplineInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineColorInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineColorInterpolator();
    NiBSplineColorInterpolator(NiBSplineData* pkData,
        NiBSplineData::Handle kColorACPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineBasisData* pkBasisData = NULL);
    NiBSplineColorInterpolator(const NiColorA& kPoseValue);

    enum Channel
    {
        COLORA
    };

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiColorA& kValue);
    virtual bool IsColorAValueSupported() const;

    // Pose methods
    void SetPoseValue(NiColorA& rkPoseValue);

    // Implemented from NiBSplineInterpolator.
    virtual unsigned short GetChannelCount() const;
    virtual unsigned int GetDimension(unsigned short usChannel) const;
    virtual unsigned int GetDegree(unsigned short usChannel) const;
    
    virtual bool GetChannelPosed(unsigned short usChannel) const;
    
    virtual NiBSplineData::Handle GetControlHandle(
        unsigned short usChannel) const;
    virtual void SetControlHandle(NiBSplineData::Handle kControlHandle, 
        unsigned short usChannel);

protected:
    NiColorA m_kColorAValue;
    
    // Handles into the NiBSplineData for access to control points.
    NiBSplineData::Handle m_kColorACPHandle;
};

NiSmartPointer(NiBSplineColorInterpolator);

//#include "NiBSplineColorInterpolator.inl"

#endif  // #ifndef NIBSPLINECOLORINTERPOLATOR_H
