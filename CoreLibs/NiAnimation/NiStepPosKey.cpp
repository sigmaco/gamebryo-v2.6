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

#include "NiStepPosKey.h"

NiImplementAnimationStream(NiStepPosKey,POSKEY,STEPKEY);

//---------------------------------------------------------------------------
void NiStepPosKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, ComputeMaxCurvature);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, InterpolateD1);
    SetInterpD2Function(eContent, eType, InterpolateD2);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, FillDerivedVals);
    SetInsertFunction(eContent, eType, Insert);
    SetCubicCoefsFunction(eContent, eType, CubicCoefs);
    SetIsPosedFunction(eContent, eType, IsPosed);
}
//---------------------------------------------------------------------------
float NiStepPosKey::ComputeMaxCurvature(NiAnimationKey*, unsigned int)
{
    // curvature for a line segment is identically zero
    return 0.0f;
}
//---------------------------------------------------------------------------
void NiStepPosKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiStepPosKey* pkNewPos = (NiStepPosKey*) pkNewKey;
    NiStepPosKey* pkOrigPos = (NiStepPosKey*) pkOrigKey;

    pkNewPos->m_fTime = pkOrigPos->m_fTime;
    pkNewPos->m_Pos = pkOrigPos->m_Pos;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiStepPosKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiStepPosKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiStepPosKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiStepPosKey* pkStepPosKeyArray = (NiStepPosKey*) pkKeyArray;
    NiDelete[] pkStepPosKeyArray;
}
//---------------------------------------------------------------------------
void NiStepPosKey::Interpolate(float fTime, const NiAnimationKey* pkKey0,
    const NiAnimationKey* pkKey1, void* pvResult)
{
    NiPosKey* pkPosKey0 = (NiPosKey*)pkKey0;
    NiPosKey* pkPosKey1 = (NiPosKey*)pkKey1;
    if (fTime < 1.0f)
        *(NiPoint3*)pvResult = pkPosKey0->GetPos();
    else
        *(NiPoint3*)pvResult = pkPosKey1->GetPos();
}
//---------------------------------------------------------------------------
void NiStepPosKey::InterpolateD1(float, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    NiPosKey* pPosKey0 = (NiPosKey*)pKey0;
    NiPosKey* pPosKey1 = (NiPosKey*)pKey1;

    // interpolate first derivative
    *(NiPoint3*)pResult = pPosKey0->GetPos() - pPosKey1->GetPos();
}
//---------------------------------------------------------------------------
void NiStepPosKey::InterpolateD2(float, const NiAnimationKey*,
    const NiAnimationKey*, void* pResult)
{
    // interpolate second derivative
    *(NiPoint3*)pResult = NiPoint3::ZERO;
}
//---------------------------------------------------------------------------
bool NiStepPosKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiStepPosKey* pkStepPosKeys = (NiStepPosKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkStepPosKeys, uiNumKeys, uiInsertAt,
        sizeof(NiStepPosKey)))
    {
        NiStepPosKey* pkNewKeys = NiNew NiStepPosKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiStepPosKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkStepPosKeys, 
            uiInsertAt * sizeof(NiStepPosKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiStepPosKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkStepPosKeys[uiInsertAt],
                uiDestSize);
        }

        NiPoint3 kPt = NiPosKey::GenInterpDefault(fTime, pkStepPosKeys,
                STEPKEY, uiNumKeys,sizeof(NiStepPosKey));

        NiStepPosKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_Pos = kPt;

        uiNumKeys++;
        NiDelete[] pkStepPosKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys,sizeof(NiStepPosKey));
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiStepPosKey::CubicCoefs(const NiAnimationKey* pkKeys, 
#ifdef NIDEBUG
    unsigned int uiNumKeys, 
#else
    unsigned int, 
#endif 
    unsigned int uiIndex, void* pvValue0, 
    void* pvOutTangent0, void* pvA0, void* pvB0)
{
    NIASSERT(uiIndex < uiNumKeys);
    const NiStepPosKey* pkStepKeys = (const NiStepPosKey*)pkKeys;
    *(NiPoint3*)pvValue0 = pkStepKeys[uiIndex].m_Pos;
    *(NiPoint3*)pvOutTangent0 = NiPoint3::ZERO;
    *(NiPoint3*)pvA0 = NiPoint3::ZERO;
    *(NiPoint3*)pvB0 = NiPoint3::ZERO;
}
//---------------------------------------------------------------------------
bool NiStepPosKey::IsPosed(const NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys)
{
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys > 0);

    if (uiNumKeys > 1)
    {
        const NiStepPosKey* pkStepKeys = (const NiStepPosKey*)pkKeys;
        NiPoint3 kValue = pkStepKeys[0].m_Pos;
        for (unsigned int ui = 1; ui < uiNumKeys; ui++)
        {
            if (pkStepKeys[ui].m_Pos != kValue)
            {
                return false;
            }
        }
    }
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiAnimationKey* NiStepPosKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiStepPosKey* pkStepPosKeys = NiNew NiStepPosKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkStepPosKeys[ui].LoadBinary(stream);
    }
    return pkStepPosKeys;
}
//---------------------------------------------------------------------------
void NiStepPosKey::LoadBinary(NiStream& stream)
{
    NiPosKey::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiStepPosKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiStepPosKey* pkStepPosKeys = (NiStepPosKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkStepPosKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiStepPosKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiPosKey::SaveBinary(stream, pkKey);
}
//---------------------------------------------------------------------------
