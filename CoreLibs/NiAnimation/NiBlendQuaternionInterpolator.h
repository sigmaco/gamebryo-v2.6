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

#ifndef NIBLENDQUATERNIONINTERPOLATOR_H
#define NIBLENDQUATERNIONINTERPOLATOR_H

#include "NiBlendInterpolator.h"
#include <NiQuaternion.h>

class NIANIMATION_ENTRY NiBlendQuaternionInterpolator :
    public NiBlendInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBlendQuaternionInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBlendQuaternionInterpolator(bool bManagerControlled,
        float fWeightThreshold, unsigned char ucArraySize);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiQuaternion& kValue);
    virtual bool IsQuaternionValueSupported() const;

protected:
    // For cloning and streaming only.
    NiBlendQuaternionInterpolator();

    // Virtual function overrides from base class.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator)
        const;
    bool StoreSingleValue(float fTime, NiObjectNET* pkInterpTarget,
        NiQuaternion& kValue);
    bool BlendValues(float fTime, NiObjectNET* pkInterpTarget,
        NiQuaternion& kValue);

    NiQuaternion m_kQuaternionValue;
};

NiSmartPointer(NiBlendQuaternionInterpolator);

#endif  // #ifndef NIBLENDQUATERNIONINTERPOLATOR_H
