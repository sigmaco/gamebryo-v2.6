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
#include "NiStepBoolKey.h"
#include <NiRTLib.h>
#include <NiStream.h>

NiImplementAnimationStream(NiStepBoolKey,BOOLKEY,STEPKEY);

//---------------------------------------------------------------------------
void NiStepBoolKey::RegisterSupportedFunctions(KeyContent eContent,
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
void NiStepBoolKey::Interpolate(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    const NiStepBoolKey* pStep0 = (const NiStepBoolKey*) pKey0;
    const NiStepBoolKey* pStep1 = (const NiStepBoolKey*) pKey1;
    if (fTime < 1.0f)
        *(bool*)pResult = pStep0->GetBool();
    else
        *(bool*)pResult = pStep1->GetBool();
}
//---------------------------------------------------------------------------
void NiStepBoolKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiStepBoolKey* pkNewStep = (NiStepBoolKey*) pkNewKey;
    NiStepBoolKey* pkOrigStep = (NiStepBoolKey*) pkOrigKey;

    pkNewStep->m_fTime = pkOrigStep->m_fTime;
    pkNewStep->m_bBool = pkOrigStep->m_bBool;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiStepBoolKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiStepBoolKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiStepBoolKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiStepBoolKey* pkStepBoolKeyArray = (NiStepBoolKey*) pkKeyArray;
    NiDelete[] pkStepBoolKeyArray;
}
//---------------------------------------------------------------------------
bool NiStepBoolKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiStepBoolKey* pkStepBoolKeys = (NiStepBoolKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkStepBoolKeys, uiNumKeys, uiInsertAt,
        sizeof(NiStepBoolKey)))
    {
        NiStepBoolKey* pkNewKeys = NiNew NiStepBoolKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiStepBoolKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkStepBoolKeys,
            uiInsertAt * sizeof(NiStepBoolKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiStepBoolKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkStepBoolKeys[uiInsertAt],
                uiDestSize);
        }

        bool b = NiBoolKey::GenInterpDefault(fTime, pkStepBoolKeys,
            STEPKEY, uiNumKeys,sizeof(NiStepBoolKey));

        NiStepBoolKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_bBool = b;

        uiNumKeys++;
        NiDelete[] pkStepBoolKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys,sizeof(NiStepBoolKey));
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiStepBoolKey::CubicCoefs(const NiAnimationKey* pkKeys, 
#ifdef NIDEBUG
    unsigned int uiNumKeys, 
#else
    unsigned int, 
#endif 
    unsigned int uiIndex, void* pvValue0, 
    void* pvOutTangent0, void* pvA0, void* pvB0)
{
    NIASSERT(uiIndex < uiNumKeys);
    const NiStepBoolKey* pkStepKeys = (const NiStepBoolKey*)pkKeys;
    *(float*)pvValue0 = pkStepKeys[uiIndex].m_bBool ? 1.0f : 0.0f;
    *(float*)pvOutTangent0 = 0.0f;
    *(float*)pvA0 = 0.0f;
    *(float*)pvB0 = 0.0f;
}
//---------------------------------------------------------------------------
bool NiStepBoolKey::IsPosed(const NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys)
{
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys > 0);

    if (uiNumKeys > 1)
    {
        const NiStepBoolKey* pkStepKeys = (const NiStepBoolKey*)pkKeys;
        bool bValue = pkStepKeys[0].m_bBool;
        for (unsigned int ui = 1; ui < uiNumKeys; ui++)
        {
            if (pkStepKeys[ui].m_bBool != bValue)
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
NiAnimationKey* NiStepBoolKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiStepBoolKey* pkStepBoolKeys = NiNew NiStepBoolKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkStepBoolKeys[ui].LoadBinary(stream);
    }
    return pkStepBoolKeys;
}
//---------------------------------------------------------------------------
void NiStepBoolKey::LoadBinary(NiStream& stream)
{
    NiBoolKey::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiStepBoolKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiStepBoolKey* pkStepBoolKeys = (NiStepBoolKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkStepBoolKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiStepBoolKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiBoolKey::SaveBinary(stream, pkKey);
}
//---------------------------------------------------------------------------
