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

// Precompiled Header
#include "NiAnimationPCH.h"

#include <NiStream.h>
#include <NiMath.h>
#include "NiColorKey.h"

NiImplementAnimationStream(NiColorKey,COLORKEY,NOINTERP);

NiColorA NiColorKey::ms_kDefault = NiColorA::BLACK;

//---------------------------------------------------------------------------
void NiColorKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, NULL);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, NULL);
    SetInterpD2Function(eContent, eType, NULL);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, NULL);
    SetInsertFunction(eContent, eType, NULL);
    SetCubicCoefsFunction(eContent, eType, NULL);
    SetIsPosedFunction(eContent, eType, NULL);
}
//---------------------------------------------------------------------------
NiColorA NiColorKey::GenInterp(float fTime, NiColorKey* pkKeys,
    NiColorKey::KeyType eType, unsigned int uiNumKeys,
    unsigned int& uiLastIdx, unsigned char ucSize)
{
    NIASSERT(uiNumKeys != 0);
    if (uiNumKeys == 1)
        return pkKeys->GetKeyAt(0, ucSize)->GetColor();

    unsigned int uiNumKeysM1 = uiNumKeys - 1;

    // This code assumes that the time values in the keys are ordered by
    // increasing value.  The search can therefore begin at uiLastIdx rather
    // than zero each time.  The idea is to provide an O(1) lookup based on
    // time coherency of the keys.

    float fLastTime = pkKeys->GetKeyAt(uiLastIdx, ucSize)->GetTime();
    if ( fTime < fLastTime )
    {
        uiLastIdx = 0;
        fLastTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
    }

    unsigned int uiNextIdx;
    float fNextTime = 0.0f;
    for (uiNextIdx = uiLastIdx + 1; uiNextIdx <= uiNumKeysM1; uiNextIdx++)
    {
        fNextTime = pkKeys->GetKeyAt(uiNextIdx, ucSize)->GetTime();
        if ( fTime <= fNextTime )
            break;

        uiLastIdx++;
        fLastTime = fNextTime;
    }

    NIASSERT(uiNextIdx < uiNumKeys);

    // interpolate the keys, requires that the time is normalized to [0,1]
    float fNormTime = (fTime - fLastTime)/(fNextTime - fLastTime);
    NiColorKey::InterpFunction interp = 
        NiColorKey::GetInterpFunction(eType);
    NIASSERT( interp );
    NiColorA kResult;
    interp(fNormTime, pkKeys->GetKeyAt(uiLastIdx, ucSize),
        pkKeys->GetKeyAt(uiNextIdx, ucSize), &kResult);
    return kResult;
}
//---------------------------------------------------------------------------
void NiColorKey::GenCubicCoefs(float fTime, NiColorKey* pkKeys,
    KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
    unsigned char ucSize, float& fTime0, float& fTime1, 
    NiColorA& kValue0, NiColorA& kOutTangent0,
    NiColorA& kA0, NiColorA& kB0)
{
    NIASSERT(uiNumKeys != 0);
    if (uiNumKeys == 1)
    {
        // Hold this constant value forever.
        kValue0 = pkKeys->GetKeyAt(0, ucSize)->GetColor();
        kOutTangent0 = NiColorA(0, 0, 0, 0);
        kA0 = NiColorA(0, 0, 0, 0);
        kB0 = NiColorA(0, 0, 0, 0);
        fTime0 = 0.0f;
        fTime1 = NI_INFINITY;
        uiLastIdx = 0;
        return;
    }

    unsigned int uiNumKeysM1 = uiNumKeys - 1;

    // This code assumes that the time values in the keys are ordered by
    // increasing value.  The search can therefore begin at uiLastIdx rather
    // than zero each time.  The idea is to provide an O(1) lookup based on
    // time coherency of the keys.

    // Copy the last index to a stack variable here to ensure that each thread
    // has its own consistent copy of the value. The stack variable is copied
    // back to the reference variable at the end of this function.
    unsigned int uiStackLastIdx = uiLastIdx;

    float fLastTime = pkKeys->GetKeyAt(uiStackLastIdx, ucSize)->GetTime();
    if (fTime < fLastTime)
    {
        uiStackLastIdx = 0;
        fLastTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
    }

    unsigned int uiNextIdx;
    float fNextTime = 0.0f;
    for(uiNextIdx = uiStackLastIdx + 1; uiNextIdx <= uiNumKeysM1; uiNextIdx++)
    {
        fNextTime = pkKeys->GetKeyAt(uiNextIdx, ucSize)->GetTime();

        // Don't allow step functions to halt when the 
        // time matches the second key.
        if ((fTime < fNextTime) || (fTime == fNextTime && eType != STEPKEY))
        {
            break;
        }

        uiStackLastIdx++;
        fLastTime = fNextTime;
    }
    
    if (eType == STEPKEY && fTime >= fNextTime && uiNextIdx == uiNumKeys)
    {
        // Step function is at the last key.
        kValue0 = pkKeys->GetKeyAt(uiNumKeysM1, ucSize)->GetColor();
        kOutTangent0 = NiColorA(0, 0, 0, 0);
        kA0 = NiColorA(0, 0, 0, 0);
        kB0 = NiColorA(0, 0, 0, 0);
        fTime0 = fNextTime;
        fTime1 = fNextTime;
        uiLastIdx = uiNumKeysM1;
    }
    else
    {
        // Extract the cubic coefficients from the keys.
        NIASSERT(uiNextIdx < uiNumKeys);
        NiAnimationKey::CubicCoefsFunction cubiccoefs = 
            NiColorKey::GetCubicCoefsFunction(eType);
        NIASSERT(cubiccoefs);
        cubiccoefs(pkKeys, uiNumKeys, uiStackLastIdx, &kValue0, 
            &kOutTangent0, &kA0, &kB0);
        fTime0 = fLastTime;
        fTime1 = fNextTime;
        uiLastIdx = uiStackLastIdx;
    }
}
//---------------------------------------------------------------------------
NiColorA NiColorKey::GenInterpDefault(float fTime, NiColorKey* pkKeys,
    KeyType eType, unsigned int uiNumKeys, unsigned char ucSize)
{
    if (uiNumKeys)
    {
        // GenInterp no longer handles out-of-range times
        if (fTime < pkKeys->GetKeyAt(0, ucSize)->GetTime())
            return pkKeys->GetKeyAt(0, ucSize)->GetColor();
        else if (fTime > pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetTime())
            return pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetColor();
        else
        {
            unsigned int uiLastIdx = 0;
            return NiColorKey::GenInterp(fTime, pkKeys, eType, uiNumKeys,
                uiLastIdx, ucSize);
        }
    }
    else
    {
        return ms_kDefault;
    }
}
//---------------------------------------------------------------------------
bool NiColorKey::Equal(const NiAnimationKey& key0, const NiAnimationKey& key1)
{
    if ( !NiAnimationKey::Equal(key0,key1) )
        return false;

    const NiColorKey& color0 = (const NiColorKey&) key0;
    const NiColorKey& color1 = (const NiColorKey&) key1;

    return color0.m_Color == color1.m_Color;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiAnimationKey* NiColorKey::CreateFromStream(NiStream&, unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
    return 0;
}
//---------------------------------------------------------------------------
void NiColorKey::LoadBinary(NiStream& stream)
{
    NiAnimationKey::LoadBinary(stream);

    m_Color.LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiColorKey::SaveToStream(NiStream&, NiAnimationKey*,
    unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiColorKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiAnimationKey::SaveBinary(stream, pkKey);

    NiColorKey* pkColorKey = (NiColorKey*) pkKey;
    pkColorKey->m_Color.SaveBinary(stream);
}
//---------------------------------------------------------------------------
