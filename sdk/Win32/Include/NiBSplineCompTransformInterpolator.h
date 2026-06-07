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

#ifndef NIBSPLINECOMPTRANSFORMINTERPOLATOR_H
#define NIBSPLINECOMPTRANSFORMINTERPOLATOR_H

#include "NiBSplineTransformInterpolator.h"

class NIANIMATION_ENTRY NiBSplineCompTransformInterpolator :
    public NiBSplineTransformInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineCompTransformInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineCompTransformInterpolator();
    NiBSplineCompTransformInterpolator(NiBSplineData* pkData,
        NiBSplineData::Handle kTransCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineData::Handle kRotCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineData::Handle kScaleCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineBasisData* pkBasisData = NULL);
    NiBSplineCompTransformInterpolator(NiQuatTransform kPoseValue);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiQuatTransform& kValue);

    void SetOffset(float fOffset, unsigned short usChannel);
    void SetHalfRange(float fHalfRange, unsigned short usChannel);

    float GetOffset(unsigned short usChannel) const;
    float GetHalfRange(unsigned short usChannel) const;
    
    virtual bool UsesCompressedControlPoints() const;

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

NiSmartPointer(NiBSplineCompTransformInterpolator);

#endif  
