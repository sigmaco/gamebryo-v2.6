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

#ifndef NIBLENDTRANSFORMINTERPOLATOR_H
#define NIBLENDTRANSFORMINTERPOLATOR_H

#include "NiBlendInterpolator.h"
#include "NiQuatTransform.h"

class NIANIMATION_ENTRY NiBlendTransformInterpolator :
    public NiBlendInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBlendTransformInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBlendTransformInterpolator(bool bManagerControlled,
        float fWeightThreshold, unsigned char ucArraySize);

    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiQuatTransform& kValue);
    virtual bool IsTransformValueSupported() const;

    // *** begin Emergent internal use only ***
    void CreateCloneInPlace(NiBlendTransformInterpolator* pkClone, 
        NiCloningProcess& kCloning);

    // For cloning and streaming only.
    NiBlendTransformInterpolator();
    // *** end Emergent internal use only ***

protected:
    // Virtual function overrides from base class.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator)
        const;
    bool StoreSingleValue(float fTime, NiObjectNET* pkInterpTarget,
        NiQuatTransform& kValue);
    bool BlendValues(float fTime, NiObjectNET* pkInterpTarget,
        NiQuatTransform& kValue);
};

NiSmartPointer(NiBlendTransformInterpolator);

#endif  // #ifndef NIBLENDTRANSFORMINTERPOLATOR_H
