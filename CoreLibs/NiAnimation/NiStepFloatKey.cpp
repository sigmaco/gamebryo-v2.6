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

#include "NiInterpScalar.h"
#include "NiStepFloatKey.h"
#include <NiRTLib.h>
#include <NiStream.h>

NiImplementAnimationStream(NiStepFloatKey,FLOATKEY,STEPKEY);

//---------------------------------------------------------------------------
void NiStepFloatKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, NULL);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, NULL);
    SetInterpD2Function(eContent, eType, NULL);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, FillDerivedVals);
    SetInsertFunction(eContent, eType, Insert);
    SetCubicCoefsFunction(eContent, eType, CubicCoefs);
    SetIsPosedFunction(eContent, eType, IsPosed);
}
//---------------------------------------------------------------------------
void NiStepFloatKey::FillDerivedVals(NiAnimationKey*, unsigned int,
    unsigned char)
{
    // no derived values to fill in
}
//---------------------------------------------------------------------------
bool NiStepFloatKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    return NiFloatKey::Equal(key0,key1);
}
//---------------------------------------------------------------------------
void NiStepFloatKey::Interpolate(float fTime, const NiAnimationKey* pkKey0,
    const NiAnimationKey* pkKey1, void* pvResult)
{
    NiFloatKey* pkFloatKey0 = (NiFloatKey*)pkKey0;
    NiFloatKey* pkFloatKey1 = (NiFloatKey*)pkKey1;
    if (fTime < 1.0f)
        *(float*)pvResult = pkFloatKey0->GetValue();
    else
        *(float*)pvResult = pkFloatKey1->GetValue();
}
//---------------------------------------------------------------------------
void NiStepFloatKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiStepFloatKey* pkNewStep = (NiStepFloatKey*) pkNewKey;
    NiStepFloatKey* pkOrigStep = (NiStepFloatKey*) pkOrigKey;

    pkNewStep->m_fTime = pkOrigStep->m_fTime;
    pkNewStep->m_fValue = pkOrigStep->m_fValue;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiStepFloatKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiStepFloatKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiStepFloatKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiStepFloatKey* pkStepFloatKeyArray = (NiStepFloatKey*) pkKeyArray;
    NiDelete[] pkStepFloatKeyArray;
}
//---------------------------------------------------------------------------
bool NiStepFloatKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiStepFloatKey* pkStepFloatKeys = (NiStepFloatKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkStepFloatKeys, uiNumKeys, uiInsertAt,
        sizeof(NiStepFloatKey)))
    {
        NiStepFloatKey* pkNewKeys = NiNew NiStepFloatKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiStepFloatKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkStepFloatKeys, uiInsertAt *
            sizeof(NiStepFloatKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiStepFloatKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkStepFloatKeys[uiInsertAt],
                uiDestSize);
        }

        float f = NiFloatKey::GenInterpDefault(fTime, pkStepFloatKeys,
            STEPKEY, uiNumKeys,sizeof(NiStepFloatKey));

        NiStepFloatKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_fValue = f;

        uiNumKeys++;
        NiDelete[] pkStepFloatKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys,sizeof(NiStepFloatKey));
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiStepFloatKey::CubicCoefs(const NiAnimationKey* pkKeys, 
#ifdef NIDEBUG
    unsigned int uiNumKeys, 
#else
    unsigned int, 
#endif 
    unsigned int uiIndex, void* pvValue0, 
    void* pvOutTangent0, void* pvA0, void* pvB0)
{
    NIASSERT(uiIndex < uiNumKeys);
    const NiStepFloatKey* pkStepKeys = (const NiStepFloatKey*)pkKeys;
    *(float*)pvValue0 = pkStepKeys[uiIndex].m_fValue;
    *(float*)pvOutTangent0 = 0.0f;
    *(float*)pvA0 = 0.0f;
    *(float*)pvB0 = 0.0f;
}
//---------------------------------------------------------------------------
bool NiStepFloatKey::IsPosed(const NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys)
{
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys > 0);

    if (uiNumKeys > 1)
    {
        const NiStepFloatKey* pkStepKeys = (const NiStepFloatKey*)pkKeys;
        float fValue = pkStepKeys[0].m_fValue;
        for (unsigned int ui = 1; ui < uiNumKeys; ui++)
        {
            if (pkStepKeys[ui].m_fValue != fValue)
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
NiAnimationKey* NiStepFloatKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiStepFloatKey* pkStepFloatKeys = NiNew NiStepFloatKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkStepFloatKeys[ui].LoadBinary(stream);
    }
    return pkStepFloatKeys;
}
//---------------------------------------------------------------------------
void NiStepFloatKey::LoadBinary(NiStream& stream)
{
    NiFloatKey::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiStepFloatKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiStepFloatKey* pkStepFloatKeys = (NiStepFloatKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkStepFloatKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiStepFloatKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiFloatKey::SaveBinary(stream, pkKey);
}
//---------------------------------------------------------------------------
