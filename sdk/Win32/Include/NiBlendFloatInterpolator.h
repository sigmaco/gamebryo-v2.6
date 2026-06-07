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

#ifndef NIBLENDFLOATINTERPOLATOR_H
#define NIBLENDFLOATINTERPOLATOR_H

#include "NiBlendInterpolator.h"

class NIANIMATION_ENTRY NiBlendFloatInterpolator : public NiBlendInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBlendFloatInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBlendFloatInterpolator(bool bManagerControlled, float fWeightThreshold,
        unsigned char ucArraySize);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        float& fValue);
    virtual bool IsFloatValueSupported() const;

protected:
    // For cloning and streaming only.
    NiBlendFloatInterpolator();

    // Virtual function overrides from base class.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator)
        const;
    bool StoreSingleValue(float fTime, NiObjectNET* pkInterpTarget,
        float& fValue);
    bool BlendValues(float fTime, NiObjectNET* pkInterpTarget, float& fValue);

    float m_fFloatValue;
};

NiSmartPointer(NiBlendFloatInterpolator);

#endif  // #ifndef NIBLENDFLOATINTERPOLATOR_H
