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


#include "NiOptimize.h"
#include "NiOptimizeAnimation.h"

float NiOptimizeAnimation::ms_fSampleRate = 1.0f/30.0f;

//---------------------------------------------------------------------------
NiOptimizeAnimation::KeyContainer::KeyContainer()
{
    pkKeys = NULL;
    uiNumKeys = 0;
    eType = NiAnimationKey::NOINTERP;
}
//---------------------------------------------------------------------------
// Reduces redundant animation keys for all NiTransformControllers in the
// scene graph rooted at pkObject. fTolerance specifies a ratio of
// how far off keys are allowed to be to still be considered redundant.
void NiOptimizeAnimation::ReduceAnimationKeys(NiAVObject* pkObject, 
    float fTolerance, float)
{
    NiOptimize::ReduceAnimationKeys(pkObject, fTolerance);
}

//---------------------------------------------------------------------------
// Reduces redundant animation keys for the specified
// NiTransformController fTolerance specifies a ratio of how far off keys
// are allowed to be to still be considered redundant.
void NiOptimizeAnimation::ReduceAnimationKeys(NiTransformController* pkCtlr,
    float fTolerance, float)
{
    NiOptimize::ReduceAnimationKeys(pkCtlr, fTolerance);
}

//---------------------------------------------------------------------------
void NiOptimizeAnimation::MergeXYZFloatKeys(NiFloatKey* pkXFloatKeys, 
    NiFloatKey* pkYFloatKeys, NiFloatKey* pkZFloatKeys, 
    NiAnimationKey::KeyType eXFloatKeyType, 
    NiAnimationKey::KeyType eYFloatKeyType, 
    NiAnimationKey::KeyType eZFloatKeyType, unsigned int uiXNumKeys, 
    unsigned int uiYNumKeys, unsigned int uiZNumKeys,
    NiPosKey*& pkCompositeKeys, NiAnimationKey::KeyType& eCompositeKeyType,
    unsigned int& uiNumCompositeKeys)
{
    unsigned int uiTotalKeys = uiXNumKeys + uiYNumKeys + uiZNumKeys;
    unsigned int uiNumRealKeys = 0;    

    float* pafKeyTimes = NiAlloc(float, uiTotalKeys);

    unsigned char ucXSize = 0;
    unsigned char ucYSize = 0;
    unsigned char ucZSize = 0;

    // Phase 0:
    // Duplicate the keys coming in so that we don't disturb the 
    // outer calling method.
    {
        if (pkXFloatKeys)
        {
            ucXSize = NiFloatKey::GetKeySize(eXFloatKeyType);
            NiFloatKey* pkOriginalXFloatKeys =  pkXFloatKeys;
            NiFloatKey::CopyFunction pfnXCopy = 
                NiFloatKey::GetCopyFunction (eXFloatKeyType);
            NiFloatKey::ArrayFunction pfnXArray = 
                NiFloatKey::GetArrayFunction (eXFloatKeyType);

            pkXFloatKeys = (NiFloatKey*) pfnXArray(uiXNumKeys);
        
            for (unsigned int uiX = 0; uiX < uiXNumKeys; uiX++)
            {
                NiFloatKey* pkNewKey = pkXFloatKeys->GetKeyAt(uiX, ucXSize);
                NiFloatKey* pkOldKey = pkOriginalXFloatKeys->GetKeyAt(
                    uiX, ucXSize);
                pfnXCopy(pkNewKey, pkOldKey);
            }
        }
        
        if (pkYFloatKeys)
        {
            ucYSize = NiFloatKey::GetKeySize(eYFloatKeyType);
            NiFloatKey* pkOriginalYFloatKeys =  pkYFloatKeys;
            NiFloatKey::CopyFunction pfnYCopy = 
                NiFloatKey::GetCopyFunction (eYFloatKeyType);
            NiFloatKey::ArrayFunction pfnYArray = 
                NiFloatKey::GetArrayFunction (eYFloatKeyType);
            
            pkYFloatKeys = (NiFloatKey*) pfnYArray(uiYNumKeys);
            
            for (unsigned int uiY = 0; uiY < uiYNumKeys; uiY++)
            {
                NiFloatKey* pkNewKey = pkYFloatKeys->GetKeyAt(uiY, ucYSize);
                NiFloatKey* pkOldKey = pkOriginalYFloatKeys->GetKeyAt(uiY,
                    ucYSize);
                pfnYCopy(pkNewKey, pkOldKey);
            }
        }

        if (pkZFloatKeys)
        {
            ucZSize = NiFloatKey::GetKeySize(eZFloatKeyType);
            NiFloatKey* pkOriginalZFloatKeys =  pkZFloatKeys;
            NiFloatKey::CopyFunction pfnZCopy = 
                NiFloatKey::GetCopyFunction (eZFloatKeyType);
            NiFloatKey::ArrayFunction pfnZArray = 
                NiFloatKey::GetArrayFunction (eZFloatKeyType);
            pkZFloatKeys = (NiFloatKey*) pfnZArray(uiZNumKeys);
            for (unsigned int uiZ = 0; uiZ < uiZNumKeys; uiZ++)
            {
                NiFloatKey* pkNewKey = pkZFloatKeys->GetKeyAt(uiZ, ucZSize);
                NiFloatKey* pkOldKey = pkOriginalZFloatKeys->GetKeyAt(uiZ, 
                    ucZSize);
                pfnZCopy(pkNewKey, pkOldKey);
            }
        }
    }

    // Phase 1: 
    // Build up a list of all the unique key times in the animation track
    {
        // Build up a list of all the unique key times for the X track
        for (unsigned int uiX = 0; uiX < uiXNumKeys; uiX++)
        {
            NiFloatKey* pkCurXKey = pkXFloatKeys->GetKeyAt(uiX, ucXSize);
            float fTime = pkCurXKey->GetTime();
            bool bAlreadyAdded = false;

            for (unsigned int ui = 0; ui < uiNumRealKeys; ui++)
            {
                if (fTime == pafKeyTimes[ui])
                    bAlreadyAdded = true;
            }

            if (!bAlreadyAdded)
            {
                pafKeyTimes[uiNumRealKeys] = fTime;
                uiNumRealKeys++;
            }
        }

        // Build up a list of all the unique key times for the Y track
        for (unsigned int uiY = 0; uiY < uiYNumKeys; uiY++)
        {
            NiFloatKey* pkCurYKey = pkYFloatKeys->GetKeyAt(uiY, ucYSize);
            float fTime = pkCurYKey->GetTime();
            bool bAlreadyAdded = false;

            for (unsigned int ui = 0; ui < uiNumRealKeys; ui++)
            {
                if (fTime == pafKeyTimes[ui])
                    bAlreadyAdded = true;
            }

            if (!bAlreadyAdded)
            {
                pafKeyTimes[uiNumRealKeys] = fTime;
                uiNumRealKeys++;
            }
        }

        // Build up a list of all the unique key times for the Z track
        for (unsigned int uiZ = 0; uiZ < uiZNumKeys; uiZ++)
        {
            NiFloatKey* pkCurZKey = pkZFloatKeys->GetKeyAt(uiZ, ucZSize);
            float fTime = pkCurZKey->GetTime();
            bool bAlreadyAdded = false;

            for (unsigned int ui = 0; ui < uiNumRealKeys; ui++)
            {
                if (fTime == pafKeyTimes[ui])
                    bAlreadyAdded = true;
            }

            if (!bAlreadyAdded)
            {
                pafKeyTimes[uiNumRealKeys] = fTime;
                uiNumRealKeys++;
            }
        }
    }

    // Now, I need to sort the key times. This is accomplished through an 
    // in-place insertion sort.
    for (unsigned int uiSortIdx = 1; uiSortIdx < uiNumRealKeys; uiSortIdx++)
    {
        float fCurrentTime = pafKeyTimes[uiSortIdx];
        for (unsigned int uiInsertIdx = 0; uiInsertIdx < uiSortIdx;
            uiInsertIdx++)
        {
            // If our NiNew time is out of place in the array, we need to
            // reshuffle all of the previously sorted keys one place to the
            // right.
            if (pafKeyTimes[uiInsertIdx] > fCurrentTime)
            {
                float fTempTime = pafKeyTimes[uiInsertIdx];
                pafKeyTimes[uiInsertIdx] = fCurrentTime;
                for (unsigned int ui = uiInsertIdx + 1; ui < uiSortIdx + 1;
                    ui++)
                {
                    float fTemp2Time = pafKeyTimes[ui];
                    pafKeyTimes[ui] = fTempTime;
                    fTempTime = fTemp2Time;
                }
                break;
            }
        }
    }


    // Phase 2: 
    // Make sure that the individual tracks have keys that match up 
    // with the unique times we computed earlier
    {
        NiFloatKey::InsertFunction pfnXInsert = NULL;
        NiFloatKey::InsertFunction pfnYInsert = NULL;
        NiFloatKey::InsertFunction pfnZInsert = NULL;

        if (pkXFloatKeys)
            pfnXInsert = NiFloatKey::GetInsertFunction (eXFloatKeyType);
        if (pkYFloatKeys)
            pfnYInsert = NiFloatKey::GetInsertFunction (eYFloatKeyType);
        if (pkZFloatKeys)
            pfnZInsert = NiFloatKey::GetInsertFunction (eZFloatKeyType);

        for (unsigned int uiTimeIdx = 0; uiTimeIdx < uiNumRealKeys;
            uiTimeIdx++)
        {
            NiAnimationKey* pkXKeys = pkXFloatKeys;
            NiAnimationKey* pkYKeys = pkYFloatKeys;
            NiAnimationKey* pkZKeys = pkZFloatKeys;

            if (pkXFloatKeys && pfnXInsert)
                pfnXInsert(pafKeyTimes[uiTimeIdx], pkXKeys, uiXNumKeys);
            if (pkYFloatKeys && pfnYInsert)
                pfnYInsert(pafKeyTimes[uiTimeIdx], pkYKeys, uiYNumKeys);
            if (pkZFloatKeys && pfnZInsert)
                pfnZInsert(pafKeyTimes[uiTimeIdx], pkZKeys, uiZNumKeys);

            pkXFloatKeys = (NiFloatKey*) pkXKeys;
            pkYFloatKeys = (NiFloatKey*) pkYKeys;
            pkZFloatKeys = (NiFloatKey*) pkZKeys;
        }
    }

    // Phase 3: 
    // Create the composite key array using the homogenous keys
    // computed in phase 2.
    
    NiPosKey* pkRealKeys = NULL;
    unsigned char ucRealKeySize = 0;

    // Determine the type of the final animation key array
    if (eXFloatKeyType == NiAnimationKey::BEZKEY ||
        eYFloatKeyType == NiAnimationKey::BEZKEY ||
        eZFloatKeyType == NiAnimationKey::BEZKEY)
    {
        pkRealKeys = NiNew NiBezPosKey[uiNumRealKeys];
        eCompositeKeyType = NiAnimationKey::BEZKEY;
        ucRealKeySize = sizeof(NiBezPosKey);
    }
    else
    {
        pkRealKeys = NiNew NiLinPosKey[uiNumRealKeys];
        eCompositeKeyType = NiAnimationKey::LINKEY;
        ucRealKeySize = sizeof(NiLinPosKey);
    }

    // Fill in the composite keys
    {
        NiFloatKey::InterpFunction pfnXInterp = NULL;
        NiFloatKey::InterpFunction pfnYInterp = NULL;
        NiFloatKey::InterpFunction pfnZInterp = NULL;

        if (pkXFloatKeys)
            pfnXInterp = NiFloatKey::GetInterpFunction (eXFloatKeyType);
        if (pkYFloatKeys)
            pfnYInterp = NiFloatKey::GetInterpFunction (eYFloatKeyType);
        if (pkZFloatKeys)
            pfnZInterp = NiFloatKey::GetInterpFunction (eZFloatKeyType);

        for (unsigned int uiTimeIdx = 0; uiTimeIdx < uiNumRealKeys; 
            uiTimeIdx++)
        {
            float fTime = pafKeyTimes[uiTimeIdx];
            NiFloatKey* pkXKey = NULL; 
            NiFloatKey* pkYKey = NULL; 
            NiFloatKey* pkZKey = NULL; 
            float fXValue = 0.0f; 
            float fYValue = 0.0f; 
            float fZValue = 0.0f; 

            if (pkXFloatKeys)
                pkXKey = pkXFloatKeys->GetKeyAt(uiTimeIdx, ucXSize);
            if (pkYFloatKeys)
                pkYKey = pkYFloatKeys->GetKeyAt(uiTimeIdx, ucYSize);
            if (pkZFloatKeys)
                pkZKey = pkZFloatKeys->GetKeyAt(uiTimeIdx, ucZSize);
            
            if (pkXKey)
                fXValue = pkXKey->GetValue();
            if (pkYKey)
                fYValue = pkYKey->GetValue();
            if (pkZKey)
                fZValue = pkZKey->GetValue();

            NiPoint3 kPos(fXValue, fYValue, fZValue);
            NiPoint3 kInTan;
            NiPoint3 kOutTan;

            unsigned int uiNextIdx = uiTimeIdx+1;
            unsigned int uiPrevIdx = uiTimeIdx-1;

            if (uiTimeIdx == 0)
                uiPrevIdx = 0;
            if (uiNextIdx == uiNumRealKeys)
                uiNextIdx = uiTimeIdx;

            if (pkXKey)
            {
                if (eXFloatKeyType == NiAnimationKey::BEZKEY)
                {
                    kInTan.x = ((NiBezFloatKey*) pkXKey)->GetInTan();
                    kOutTan.x = ((NiBezFloatKey*) pkXKey)->GetOutTan();
                }
                else
                {
                    float fValue = pkXKey->GetValue();
                    NiFloatKey* pkPrevKey = 
                        pkXFloatKeys->GetKeyAt(uiPrevIdx, ucXSize);
                    NiFloatKey* pkNextKey = 
                        pkXFloatKeys->GetKeyAt(uiNextIdx, ucXSize);

                    kInTan.x = fValue - pkPrevKey->GetValue();
                    kOutTan.x = pkNextKey->GetValue() - fValue;
                }
            }
          
            if (pkYKey)
            {
                if (eYFloatKeyType == NiAnimationKey::BEZKEY)
                {
                    kInTan.y = ((NiBezFloatKey*) pkYKey)->GetInTan();
                    kOutTan.y = ((NiBezFloatKey*) pkYKey)->GetOutTan();
                }
                else
                {
                    float fValue = pkYKey->GetValue();
                    NiFloatKey* pkYPrevKey = 
                        pkYFloatKeys->GetKeyAt(uiPrevIdx, ucYSize);
                    NiFloatKey* pkYNextKey = 
                        pkYFloatKeys->GetKeyAt(uiNextIdx, ucYSize);
                    kInTan.y = fValue - pkYPrevKey->GetValue();
                    kOutTan.y = pkYNextKey->GetValue() - fValue;
                }
            }

            if (pkZKey)
            {
                if (eZFloatKeyType == NiAnimationKey::BEZKEY)
                {
                    kInTan.z = ((NiBezFloatKey*) pkZKey)->GetInTan();
                    kOutTan.z = ((NiBezFloatKey*) pkZKey)->GetOutTan();
                }
                else
                {
                    float fValue = pkZKey->GetValue();
                    NiFloatKey* pkZPrevKey = 
                        pkZFloatKeys->GetKeyAt(uiPrevIdx, ucZSize);
                    NiFloatKey* pkZNextKey = 
                        pkZFloatKeys->GetKeyAt(uiNextIdx, ucZSize);
                    kInTan.z = fValue - pkZPrevKey->GetValue();
                    kOutTan.z = pkZNextKey->GetValue() - fValue;
                }
            }


            if (uiPrevIdx == uiTimeIdx)
            {
                kInTan.x = 0.0f;
                kInTan.y = 0.0f;
                kInTan.z = 0.0f;
            }
            
            if (uiNextIdx == uiTimeIdx)
            {
                kOutTan.x = 0.0f;
                kOutTan.y = 0.0f;
                kOutTan.z = 0.0f;
            }
            NiPosKey* pkRealKey = (NiPosKey*) pkRealKeys->GetKeyAt(uiTimeIdx, 
                ucRealKeySize);
            pkRealKey->SetTime(fTime);
            pkRealKey->SetPos(kPos);
            if (eCompositeKeyType == NiAnimationKey::BEZKEY)
            {
                ((NiBezPosKey*)pkRealKey)->SetInTan(kInTan);
                ((NiBezPosKey*)pkRealKey)->SetOutTan(kOutTan);
            }
        }
    }
    
    // Phase 4: 
    // Clean up all the allocated keys and arrays we've created.

    NiFree(pafKeyTimes);
    
    NiFloatKey::DeleteFunction pfnXDelete = 
        NiFloatKey::GetDeleteFunction (eXFloatKeyType);
    NiFloatKey::DeleteFunction pfnYDelete = 
        NiFloatKey::GetDeleteFunction (eYFloatKeyType);
    NiFloatKey::DeleteFunction pfnZDelete = 
        NiFloatKey::GetDeleteFunction (eZFloatKeyType);

    pfnXDelete(pkXFloatKeys);
    pfnYDelete(pkYFloatKeys);
    pfnZDelete(pkZFloatKeys);

    pkCompositeKeys = pkRealKeys;
    uiNumCompositeKeys = uiNumRealKeys;
}

//---------------------------------------------------------------------------
// Converts animation keys for all NiTransformControllers in the
// scene graph rooted at pkObject. eType specifies what form the NiNew keys
// should take. fTolerance and fRotationTolerance specifies a ratio of
// how far off keys are allowed to be to still be considered redundant.
void NiOptimizeAnimation::CurveFitAnimationKeys(NiAVObject* pkObject, 
    NiAnimationKey::KeyType eType, float fTolerance, float fRotationTolerance)
{
    // Reduces redundant animation keys for all NiTransformControllers in the
    // scene graph rooted at pkObject. fTolerance specifies a ratio of
    // how far off keys are allowed to be to still be considered redundant.

    const char* ppcTags[1] = {"NiOptAnimateKeep"};
    if (!NiOptimize::CheckForExtraDataTags(pkObject, ppcTags, 1))
    {
        NiTimeController* pkCtlr = pkObject->GetControllers();
        while (pkCtlr)
        {
            NiTransformController* pkKFCtlr =
                NiDynamicCast(NiTransformController, pkCtlr);
            if (pkKFCtlr)
            {
                CurveFitAnimationKeys(pkKFCtlr, eType, fTolerance, 
                    fRotationTolerance);
            }

            pkCtlr = pkCtlr->GetNext();
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                
                CurveFitAnimationKeys(pkChild, eType, fTolerance, 
                    fRotationTolerance);
            
            }
        }
    }
}

//---------------------------------------------------------------------------
// Converts animation keys for the specified NiTransformControllers. 
// eType specifies what form the NiNew keys should take.
// fTolerance and fRotationTolerance specifies a ratio of
// how far off keys are allowed to be to still be considered redundant.
void NiOptimizeAnimation::CurveFitAnimationKeys(NiTransformController* pkCtlr,
    NiAnimationKey::KeyType eType, float fTolerance, float fRotationTolerance)
{
    NiInterpolator* pkInterp = pkCtlr->GetInterpolator();
    if (NiIsKindOf(NiTransformInterpolator, pkInterp))
    {
        NiTransformInterpolator* pkTransformInterp = 
            (NiTransformInterpolator*)pkInterp;

        NiTransformData* pkData = pkTransformInterp->GetTransformData();
        if (pkData)
        {
            unsigned int uiNumRotKeys;
            NiRotKey::KeyType eRotType;
            unsigned char ucRotSize;
            NiRotKey* pkRotKey = pkData->GetRotAnim(uiNumRotKeys,
                eRotType, ucRotSize);

            unsigned int uiNumPosKeys;
            NiPosKey::KeyType ePosType;
            unsigned char ucPosSize;
            NiPosKey* pkPosKey = pkData->GetPosAnim(uiNumPosKeys,
                ePosType, ucPosSize);

            unsigned int uiNumScaleKeys;
            NiFloatKey::KeyType eScaleType;
            unsigned char ucScaleSize;
            //NiFloatKey* pkScaleKey = 
            pkData->GetScaleAnim(uiNumScaleKeys, eScaleType, ucScaleSize);
            float fBegin = pkCtlr->GetBeginKeyTime();
            float fEnd = pkCtlr->GetEndKeyTime();

            if (uiNumRotKeys > 2 || eRotType == NiAnimationKey::EULERKEY)
            {
                if (eType != eRotType)
                {
                    NiOptimizeAnimation::KeyContainer kRotContainer = 
                        CurveFitRotKeys(pkRotKey, uiNumRotKeys, eRotType,
                            eType, fBegin, fEnd, ms_fSampleRate, fTolerance,
                            fRotationTolerance);

                    if (kRotContainer.pkKeys != pkRotKey)
                    {
                        pkData->ReplaceRotAnim(
                            (NiRotKey*)kRotContainer.pkKeys, 
                            kRotContainer.uiNumKeys, kRotContainer.eType);
                    }
                }
            }
            
            
            if (uiNumPosKeys > 2)
            {
                if (eType != ePosType)
                {
                    NiOptimizeAnimation::KeyContainer kPosContainer = 
                        CurveFitPosKeys(pkPosKey, uiNumPosKeys, ePosType,
                            eType, fBegin, fEnd, ms_fSampleRate, fTolerance,
                            fRotationTolerance);

                    if (kPosContainer.pkKeys != pkPosKey)
                    {
                        pkData->ReplacePosAnim(
                            (NiPosKey*)kPosContainer.pkKeys, 
                            kPosContainer.uiNumKeys, kPosContainer.eType);
                    }
                }
            }

            /*if (uiNumScaleKeys > 2)
            {
                if (eType != eScaleType)
                {
                    NiOptimizeAnimation::KeyContainer kScaleContainer = 
                        CurveFitScaleKeys(pkScaleKey, uiNumScaleKeys,
                        eScaleType, eType,
                        fBegin, fEnd, ms_fSampleRate, fTolerance,
                        fScaleationTolerance);

                    if (kScaleContainer.pkKeys != pkScaleKey)
                    {
                        pkData->ReplaceScaleData(
                            (NiScaleKey*)kScaleContainer.pkKeys, 
                            kScaleContainer.uiNumKeys, kScaleContainer.eType);
                    }
                }
            }
            */
        }
    }

}
//---------------------------------------------------------------------------
NiOptimizeAnimation::KeyContainer NiOptimizeAnimation::CurveFitRotKeys(
    NiRotKey* pkRotKey, unsigned int uiNumRotKeys, 
    NiRotKey::KeyType eRotType, NiAnimationKey::KeyType eDestType,
    float fBegin, float fEnd, float fSampleRate, float,
    float)
{
    NiOptimizeAnimation::KeyContainer kKeys;
    unsigned int uiNumKeys = 0;
    NiRotKey* pkNewRotKey = NULL;
    unsigned char ucSize = NiRotKey::GetKeySize(eRotType);
    
    unsigned char ucDestSize = NiRotKey::GetKeySize(eDestType);
    
    // Try to just copy the keys as-is
    if (eDestType == NiAnimationKey::BEZKEY && 
        eRotType == NiAnimationKey::LINKEY)
    {
        uiNumKeys = uiNumRotKeys;
        NiBezRotKey* pkBezRotKey = NiNew NiBezRotKey[uiNumKeys];
        pkNewRotKey = pkBezRotKey;
        for (unsigned int ui = 0; ui < uiNumKeys; ui++)
        {
            pkBezRotKey[ui].SetTime(pkRotKey->GetKeyAt(ui, ucSize)->
                GetTime());
            pkBezRotKey[ui].SetQuaternion(pkRotKey->GetKeyAt(ui, ucSize)->
                GetQuaternion());
        }
    }
    else // Otherwise, sample the keys
    {
        float fIncTime = fSampleRate;
        float fNumKeys = (fEnd - fBegin)/fIncTime;
        uiNumKeys = (unsigned int)(fNumKeys + 1.0f);
        
        NiAnimationKey::ArrayFunction pNewFunc = 
            NiAnimationKey::GetArrayFunction(NiAnimationKey::ROTKEY, 
            eDestType);
        NIASSERT(pNewFunc);
        pkNewRotKey = (NiRotKey*) pNewFunc(uiNumKeys);

        float fTime = fBegin;
        unsigned int uiLastIdx = 0;
        for (unsigned int ui = 0; ui < uiNumKeys; ui++)
        {
            NiQuaternion kQuat = NiRotKey::GenInterp(fTime, pkRotKey, 
                eRotType, uiNumRotKeys, uiLastIdx, ucSize);
            pkNewRotKey->GetKeyAt(ui, ucDestSize)->SetTime(fTime);
            pkNewRotKey->GetKeyAt(ui, ucDestSize)->SetQuaternion(kQuat);
            fTime += fIncTime;
        }
    }

    // We have to set up the data for interpolation
    NiAnimationKey::FillDerivedValsFunction pDerivedValsFunc = 
        NiAnimationKey::GetFillDerivedFunction(NiAnimationKey::ROTKEY,
            eDestType);
    if (pDerivedValsFunc)
        pDerivedValsFunc(pkNewRotKey, uiNumKeys, ucDestSize);

    kKeys.eType = eDestType;
    kKeys.pkKeys = pkNewRotKey;
    kKeys.uiNumKeys = uiNumKeys;
    return kKeys;
}
//---------------------------------------------------------------------------
NiOptimizeAnimation::KeyContainer NiOptimizeAnimation::CurveFitPosKeys(
    NiPosKey* pkPosKey, unsigned int uiNumPosKeys, NiPosKey::KeyType ePosType, 
    NiAnimationKey::KeyType eDestType, float fBegin, float fEnd, 
    float fSampleRate, float, float)
{
    NiOptimizeAnimation::KeyContainer kKeys;
    unsigned int uiNumKeys = 0;
    NiPosKey* pkNewPosKey = NULL;
    unsigned char ucSize = NiPosKey::GetKeySize(ePosType);
    unsigned char ucDestSize = NiPosKey::GetKeySize(eDestType);
    
    // Try to just copy the keys as-is
    if (eDestType == NiAnimationKey::BEZKEY &&
        ePosType == NiAnimationKey::LINKEY)
    {
        uiNumKeys = uiNumPosKeys;
        NiBezPosKey* pkBezPosKey = NiNew NiBezPosKey[uiNumKeys];
        pkNewPosKey = pkBezPosKey;
        for (unsigned int ui = 0; ui < uiNumKeys; ui++)
        {
            pkBezPosKey[ui].SetTime(pkPosKey->GetKeyAt(ui, ucSize)->GetTime());
            pkBezPosKey[ui].SetPos(pkPosKey->GetKeyAt(ui, ucSize)->GetPos());
        }
    }
    else // Otherwise, sample the keys
    {
        float fIncTime = fSampleRate;
        float fNumKeys = (fEnd - fBegin)/fIncTime;
        uiNumKeys = (unsigned int)(fNumKeys + 1.0f);
        
        NiAnimationKey::ArrayFunction pNewFunc =
            NiAnimationKey::GetArrayFunction(
                NiAnimationKey::POSKEY, eDestType);
        NIASSERT(pNewFunc);
        pkNewPosKey = (NiPosKey*) pNewFunc(uiNumKeys);

        float fTime = fBegin;
        unsigned int uiLastIdx = 0;
        for (unsigned int ui = 0; ui < uiNumKeys; ui++)
        {
            NiPoint3 kPos = NiPosKey::GenInterp(fTime, pkPosKey, ePosType,
                uiNumPosKeys, uiLastIdx, ucSize);
            pkNewPosKey->GetKeyAt(ui, ucDestSize)->SetTime(fTime);
            pkNewPosKey->GetKeyAt(ui, ucDestSize)->SetPos(kPos);
            fTime += fIncTime;
        }
    }

    // Compute the tangents
    if (eDestType == NiAnimationKey::BEZKEY)
    {
        NiBezPosKey* pkBezKeys = (NiBezPosKey*) pkNewPosKey;
        for (unsigned int ui = 0; ui < uiNumKeys; ui++)
        {
            NiPoint3 kInTan;
            NiPoint3 kOutTan;
            if (ui == 0)
                kInTan = NiPoint3::ZERO;
            else
                kInTan = pkBezKeys[ui].GetPos() - pkBezKeys[ui-1].GetPos();

            if (ui == uiNumKeys - 1)
                kOutTan = NiPoint3::ZERO;
            else
                kOutTan = pkBezKeys[ui+1].GetPos() - pkBezKeys[ui].GetPos();
            
            pkBezKeys[ui].SetInTan(kInTan);
            pkBezKeys[ui].SetOutTan(kOutTan);
        }
    }

    // We have to set up the data for interpolation
    NiAnimationKey::FillDerivedValsFunction pDerivedValsFunc = 
        NiAnimationKey::GetFillDerivedFunction(NiAnimationKey::POSKEY,
            eDestType);
    if (pDerivedValsFunc)
        pDerivedValsFunc(pkNewPosKey, uiNumKeys, ucDestSize);

    kKeys.eType = eDestType;
    kKeys.pkKeys = pkNewPosKey;
    kKeys.uiNumKeys = uiNumKeys;
    return kKeys;
}
//---------------------------------------------------------------------------
NiOptimizeAnimation::KeyContainer NiOptimizeAnimation::CurveFitScaleKeys( 
    NiFloatKey* pkScaleKey, unsigned int uiNumScaleKeys, 
    NiFloatKey::KeyType eScaleType, NiAnimationKey::KeyType, 
    float fBegin, float fEnd, float fSampleRate, float, 
    float)
{
    unsigned char ucSize = NiFloatKey::GetKeySize(eScaleType);
    
    NiOptimizeAnimation::KeyContainer kKeys;
    float fIncTime = fSampleRate;
    float fNumKeys = (fEnd - fBegin)/fIncTime;
    unsigned int uiNumKeys = (unsigned int)(fNumKeys + 1.0f);
            
    NiLinFloatKey* pkLinScaleKey = NiNew NiLinFloatKey[uiNumKeys];
    float fTime = fBegin;
    unsigned int uiLastIdx = 0;
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        float fValue = NiFloatKey::GenInterp(fTime, pkScaleKey, eScaleType,
            uiNumScaleKeys, uiLastIdx, ucSize);
        pkLinScaleKey[ui].SetTime(fTime);
        pkLinScaleKey[ui].SetValue(fValue);
        fTime += fIncTime;
    }

    kKeys.eType = NiAnimationKey::LINKEY;
    kKeys.pkKeys = pkLinScaleKey;
    kKeys.uiNumKeys = uiNumKeys;
    return kKeys;
}
//---------------------------------------------------------------------------
