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

#ifndef NIBSPLINECOMPFLOATINTERPOLATOR_H
#define NIBSPLINECOMPFLOATINTERPOLATOR_H

#include "NiBSplineFloatInterpolator.h"
#include "NiBSplineData.h"

class NIANIMATION_ENTRY NiBSplineCompFloatInterpolator :
    public NiBSplineFloatInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineCompFloatInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineCompFloatInterpolator();
    NiBSplineCompFloatInterpolator(NiBSplineData* pkData,
        NiBSplineData::Handle kFloatCPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineBasisData* pkBasisData = NULL);
    NiBSplineCompFloatInterpolator(const float kPoseValue);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        float& kValue);

    void SetOffset(float fOffset, unsigned short usChannel);
    void SetHalfRange(float fHalfRange, unsigned short usChannel);

    float GetOffset(unsigned short usChannel) const;
    float GetHalfRange(unsigned short usChannel) const;
    
    virtual bool UsesCompressedControlPoints() const;

protected:
    enum
    {
        FLOAT_OFFSET = 0,
        FLOAT_RANGE,
        NUM_SCALARS  
    };
    float m_afCompScalars[NUM_SCALARS];
};

NiSmartPointer(NiBSplineCompFloatInterpolator);

//#include "NiBSplineCompFloatInterpolator.inl"

#endif  // #ifndef NIBSPLINECOLORINTERPOLATOR_H
