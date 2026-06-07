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

#ifndef NIBLENDPOINT3INTERPOLATOR_H
#define NIBLENDPOINT3INTERPOLATOR_H

#include "NiBlendInterpolator.h"
#include <NiPoint3.h>

class NIANIMATION_ENTRY NiBlendPoint3Interpolator : public NiBlendInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBlendPoint3Interpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBlendPoint3Interpolator(bool bManagerControlled, float fWeightThreshold,
        unsigned char ucArraySize);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiPoint3& kValue);
    virtual bool IsPoint3ValueSupported() const;

protected:
    // For cloning and streaming only.
    NiBlendPoint3Interpolator();

    // Virtual function overrides from base class.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator)
        const;
    bool StoreSingleValue(float fTime, NiObjectNET* pkInterpTarget,
        NiPoint3& kValue);
    bool BlendValues(float fTime, NiObjectNET* pkInterpTarget,
        NiPoint3& kValue);

    NiPoint3 m_kPoint3Value;
    bool m_bValidValue;
};

NiSmartPointer(NiBlendPoint3Interpolator);

#endif  // #ifndef NIBLENDPOINT3INTERPOLATOR_H
