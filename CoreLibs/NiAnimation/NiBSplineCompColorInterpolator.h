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

#ifndef NIBSPLINECOMPCOLORINTERPOLATOR_H
#define NIBSPLINECOMPCOLORINTERPOLATOR_H

#include "NiBSplineColorInterpolator.h"
#include "NiColor.h"
#include "NiBSplineData.h"

class NIANIMATION_ENTRY NiBSplineCompColorInterpolator :
    public NiBSplineColorInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineCompColorInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineCompColorInterpolator();
    NiBSplineCompColorInterpolator(NiBSplineData* pkData,
        NiBSplineData::Handle kColorACPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineBasisData* pkBasisData = NULL);
    NiBSplineCompColorInterpolator(const NiColorA& kPoseValue);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiColorA& kValue);
    
    void SetOffset(float fOffset, unsigned short usChannel);
    void SetHalfRange(float fHalfRange, unsigned short usChannel);

    float GetOffset(unsigned short usChannel) const;
    float GetHalfRange(unsigned short usChannel) const;
    
    virtual bool UsesCompressedControlPoints() const;

protected:
    enum
    {
        COLORA_OFFSET = 0,
        COLORA_RANGE,
        NUM_SCALARS  
    };
    float m_afCompScalars[NUM_SCALARS];
};

NiSmartPointer(NiBSplineCompColorInterpolator);

//#include "NiBSplineCompColorInterpolator.inl"

#endif  // #ifndef NIBSPLINECOMPCOLORINTERPOLATOR_H
