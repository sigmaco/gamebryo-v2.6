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

#ifndef NIBLENDCOLORINTERPOLATOR_H
#define NIBLENDCOLORINTERPOLATOR_H

#include "NiBlendInterpolator.h"
#include <NiColor.h>

class NIANIMATION_ENTRY NiBlendColorInterpolator : public NiBlendInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBlendColorInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBlendColorInterpolator(bool bManagerControlled, float fWeightThreshold,
        unsigned char ucArraySize);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiColorA& kValue);
    virtual bool IsColorAValueSupported() const;

protected:
    // For cloning and streaming only.
    NiBlendColorInterpolator();

    // Virtual function overrides from base class.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator)
        const;
    bool StoreSingleValue(float fTime, NiObjectNET* pkInterpTarget,
        NiColorA& kValue);
    bool BlendValues(float fTime, NiObjectNET* pkInterpTarget,
        NiColorA& kValue);

    NiColorA m_kColorValue;
    bool m_bValidValue;
};

NiSmartPointer(NiBlendColorInterpolator);

#endif  // #ifndef NIBLENDCOLORINTERPOLATOR_H
