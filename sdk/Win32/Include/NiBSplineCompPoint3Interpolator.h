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

#ifndef NIBSPLINECOMPPOINT3INTERPOLATOR_H
#define NIBSPLINECOMPPOINT3INTERPOLATOR_H

#include "NiBSplinePoint3Interpolator.h"
#include "NiPoint3.h"
#include "NiBSplineData.h"

class NIANIMATION_ENTRY NiBSplineCompPoint3Interpolator :
    public NiBSplinePoint3Interpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSplineCompPoint3Interpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBSplineCompPoint3Interpolator();
    NiBSplineCompPoint3Interpolator(NiBSplineData* pkData,
        NiBSplineData::Handle kPoint3CPHandle = NiBSplineData::INVALID_HANDLE,
        NiBSplineBasisData* pkBasisData = NULL);
    NiBSplineCompPoint3Interpolator(const NiPoint3& kPoseValue);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiPoint3& kValue);
    
    void SetOffset(float fOffset, unsigned short usChannel);
    void SetHalfRange(float fHalfRange, unsigned short usChannel);

    float GetOffset(unsigned short usChannel) const;
    float GetHalfRange(unsigned short usChannel) const;
    
    virtual bool UsesCompressedControlPoints() const;

protected:
    enum
    {
        POINT3_OFFSET = 0,
        POINT3_RANGE,
        NUM_SCALARS  
    };
    float m_afCompScalars[NUM_SCALARS];
};

NiSmartPointer(NiBSplineCompPoint3Interpolator);

//#include "NiBSplineCompPoint3Interpolator.inl"

#endif  // #ifndef NIBSPLINEPOINT3INTERPOLATOR_H
