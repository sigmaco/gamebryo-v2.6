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

#ifndef NIOPTIMIZEANIMATION_H
#define NIOPTIMIZEANIMATION_H

#include "NiOptimizationLibType.h"
#include <NiAnimation.h>


class NIOPTIMIZATION_ENTRY NiOptimizeAnimation
{
public:
    
    // Reduces redundant animation keys for all NiTransformControllers in the
    // scene graph rooted at pkObject. fTolerance and fRotationTolerance 
    // specifies a ratio of how far off keys are allowed to be to still
    // be considered redundant.
    static void ReduceAnimationKeys(NiAVObject* pkObject, float fTolerance, 
        float fRotationTolerance);

    // Reduces redundant animation keys for the specified
    // NiTransformController fTolerance and fRotationTolerance specifies a 
    // ratio of how far off keys are allowed to be to still be considered 
    // redundant.
    static void ReduceAnimationKeys(NiTransformController* pkCtlr,
        float fTolerance, float fRotationTolerance);

    // Create a composite pos key from individual X, Y, & Z float keys. When
    // possible,  this will create a Bezier Pos Key. The final result will be
    // passed by reference. Note that this is an expensive function.
    static void MergeXYZFloatKeys(NiFloatKey* pkXFloatKeys, 
        NiFloatKey* pkYFloatKeys, NiFloatKey* pkZFloatKeys,
        NiAnimationKey::KeyType eXFloatKeyType, 
        NiAnimationKey::KeyType eYFloatKeyType,
        NiAnimationKey::KeyType eZFloatKeyType,
        unsigned int uiXNumKeys, unsigned int uiYNumKeys,
        unsigned int uiZNumKeys, NiPosKey*& pkCompositeKeys,
        NiAnimationKey::KeyType& eCompositeKeyType,
        unsigned int& uiNumCompositeKeys);

    // Converts animation keys for all NiTransformControllers in the scene
    // graph rooted at pkObject. eType specifies what form the new keys should
    // take. fTolerance and fRotationTolerance specifies a ratio of how far off
    // keys are allowed to be to still be considered redundant.
    static void CurveFitAnimationKeys(NiAVObject* pkObject,
        NiAnimationKey::KeyType eTargetType, 
        float fTolerance, float fRotationTolerance);

    // Converts animation keys for the specified NiTransformControllers. 
    // eType specifies what form the new keys should take.
    // fTolerance and fRotationTolerance specifies a ratio of
    // how far off keys are allowed to be to still be considered redundant.
    static void CurveFitAnimationKeys(NiTransformController* pkCtlr,
        NiAnimationKey::KeyType eTargetType, float fTolerance, 
        float fRotationTolerance);
private:
    class KeyContainer
    {
        public:
            KeyContainer();
            NiAnimationKey* pkKeys;
            unsigned int uiNumKeys;
            NiAnimationKey::KeyType eType;
    };

    static KeyContainer CurveFitRotKeys(NiRotKey* pkRotKey,
        unsigned int uiNumRotKeys, NiRotKey::KeyType eRotType,
        NiAnimationKey::KeyType eType, float fBegin, float fEnd,
        float fSampleRate, float fTolerance, float fRotationTolerance);
    static KeyContainer CurveFitPosKeys(NiPosKey* pkPosKey,
        unsigned int uiNumPosKeys, NiPosKey::KeyType ePosType,
        NiAnimationKey::KeyType eType, float fBegin, float fEnd,
        float fSampleRate, float fTolerance, float fRotationTolerance);
    static KeyContainer CurveFitScaleKeys(NiFloatKey* pkScaleKey,
        unsigned int uiNumScaleKeys, NiFloatKey::KeyType eScaleType,
        NiAnimationKey::KeyType eType, float fBegin, float fEnd, 
        float fSampleRate, float fTolerance, float fRotationTolerance);

    static float ms_fSampleRate;
};


#endif  // #ifndef NiOptimizeAnimation_H
