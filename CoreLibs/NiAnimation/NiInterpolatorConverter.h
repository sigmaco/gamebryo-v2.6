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

#ifndef NIINTERPOLATORCONVERTER_H
#define NIINTERPOLATORCONVERTER_H

#include "NiEvaluator.h"

class NiInterpolator;
class NiBSplineCompTransformInterpolator;
class NiBSplineTransformInterpolator;
class NiTransformInterpolator;
class NiLookAtInterpolator;
class NiPathInterpolator;

class NIANIMATION_ENTRY NiInterpolatorConverter
{
public:
    static NiEvaluator* Convert(NiInterpolator* pkInterpolator, 
        const NiEvaluator::IDTag& kIDTag, bool bCollapseInterpolator);

protected:
    static NiEvaluator* ConvertColorInterpolator(
        NiInterpolator* pkInterpolator);
    static NiEvaluator* ConvertBoolInterpolator(
        NiInterpolator* pkInterpolator);
    static NiEvaluator* ConvertFloatInterpolator(
        NiInterpolator* pkInterpolator);
    static NiEvaluator* ConvertPoint3Interpolator(
        NiInterpolator* pkInterpolator);
    static NiEvaluator* ConvertQuaternionInterpolator(
        NiInterpolator* pkInterpolator);
    static NiEvaluator* ConvertTransformInterpolator(
        NiInterpolator* pkInterpolator, 
        const NiFixedString& kAVObjectName,
        bool bCollapseInterpolator);

    static NiEvaluator* ConvertBSplineCompTransformInterpolator(
        NiBSplineCompTransformInterpolator* pkBSCTInterp, 
        const NiQuatTransform& kPosedValue);
    static NiEvaluator* ConvertBSplineTransformInterpolator(
        NiBSplineTransformInterpolator* pkBSTInterp, 
        const NiQuatTransform& kPosedValue);
    static NiEvaluator* ConvertTransformInterpolator(
        NiTransformInterpolator* pkTInterp, 
        const NiQuatTransform& kPosedValue);
    static NiEvaluator* ConvertLookAtInterpolator(
        NiLookAtInterpolator* pkLookAtInterp, 
        const NiFixedString& kAVObjectName, 
        bool bCollapseInterpolator);
    static NiEvaluator* ConvertPathInterpolator(
        NiPathInterpolator* pkPathInterp);

    static NiFixedString ConvertCtlrType(const NiFixedString& kCtlrType);
};

#endif  // #ifndef NIINTERPOLATORCONVERTER_H
