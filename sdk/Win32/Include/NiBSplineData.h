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

#ifndef NIBSPLINEDATA_H
#define NIBSPLINEDATA_H

#include <NiObject.h>
#include <NiStream.h>
#include "NiAnimationLibType.h"
#include "NiBSplineBasisData.h"

// NiBSplineData contains one or more sets of control points for use in 
// interpolation of open, uniform B-Splines. Since multiple 
// NiBSplineInterpolators  may have the same NiBSplineData, this value is 
// encapsulated in an NiObject container, allowing it to be cloned and 
// streamed appropriately. There are two distinct control point arrays
// stored in an NiBSplineData, compact and float. Each type of control 
// point has its associated methods and calling a method with the 
// incorrect type of handle will result in undefined behavior. 
// Note that this class will only interpolate B-Splines of degree 3.

class NIANIMATION_ENTRY NiBSplineData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineData ();
    virtual ~NiBSplineData ();

    typedef unsigned int Handle;
    enum 
    {
        INVALID_HANDLE = 0xFFFF
    };

    // Add a control point array of uiDimension dimensionality to
    // this data object. 
    Handle InsertControlPoints(const float* pafControlPoints, 
        unsigned int uiCount, unsigned int uiDimension);
    Handle InsertAndCompactControlPoints(const float* pafControlPoints, 
        unsigned int uiCount, unsigned int uiDimension, float& fOffset,
        float& fHalfRange);
    
    const float* GetControlPoint(Handle kHandle, 
        unsigned int uiIndex, unsigned int uiDimension) const;
    const short* GetCompactControlPoint(Handle kHandle, 
        unsigned int uiIndex, unsigned int uiDimension) const;

    // The spline is defined for 0 <= t <= 1.  If a t-value is outside [0,1],
    // an open spline clamps t to [0,1].  That is, if t > 1, t is set to 1;
    // if t < 0, t is set to 0.  A periodic spline wraps to to [0,1].  That
    // is, if t is outside [0,1], then t is set to t-floor(t).
    void GetValueDegree3(float fTime, float* afPos,
        unsigned int uiDimension, NiBSplineBasisData* pkBasisData,
        Handle kControlPointHandle) const;
    void GetCompactedValueDegree3(float fTime, float* afPos,
        unsigned int uiDimension, NiBSplineBasisData* pkBasisData,
        Handle kControlPointHandle, float fOffset,
        float fHalfRange) const;
    
protected:
    float* m_pafControlPoints;
    short* m_pasCompactControlPoints;
    
    unsigned int m_uiControlPointCount;
    unsigned int m_uiCompactControlPointCount;
};

NiSmartPointer(NiBSplineData);

//#include "NiBSplineData.inl"

#endif
