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

#include "NiStepColorKey.h"

NiImplementAnimationStream(NiStepColorKey,COLORKEY,STEPKEY);

//---------------------------------------------------------------------------
void NiStepColorKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, NULL);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, NULL);
    SetInterpD2Function(eContent, eType, NULL);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, NULL);
    SetInsertFunction(eContent, eType, Insert);
    SetCubicCoefsFunction(eContent, eType, CubicCoefs);
    SetIsPosedFunction(eContent, eType, IsPosed);
}
//---------------------------------------------------------------------------
void NiStepColorKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiStepColorKey* pkNewColor = (NiStepColorKey*) pkNewKey;
    NiStepColorKey* pkOrigColor = (NiStepColorKey*) pkOrigKey;

    pkNewColor->m_fTime = pkOrigColor->m_fTime;
    pkNewColor->m_Color = pkOrigColor->m_Color;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiStepColorKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiStepColorKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiStepColorKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiStepColorKey* pkStepColorKeyArray = (NiStepColorKey*) pkKeyArray;
    NiDelete[] pkStepColorKeyArray;
}
//---------------------------------------------------------------------------
void NiStepColorKey::Interpolate(float fTime, const NiAnimationKey* pkKey0,
    const NiAnimationKey* pkKey1, void* pvResult)
{
    NiColorKey* pkColorKey0 = (NiColorKey*)pkKey0;
    NiColorKey* pkColorKey1 = (NiColorKey*)pkKey1;
    if (fTime < 1.0f)
        *(NiColorA*)pvResult = pkColorKey0->GetColor();
    else
        *(NiColorA*)pvResult = pkColorKey1->GetColor();
}
//---------------------------------------------------------------------------
bool NiStepColorKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiStepColorKey* pkStepColorKeys = (NiStepColorKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkStepColorKeys, uiNumKeys, uiInsertAt,
        sizeof(NiStepColorKey)))
    {
        NiStepColorKey* pkNewKeys = NiNew NiStepColorKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiStepColorKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkStepColorKeys, uiInsertAt *
            sizeof(NiStepColorKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiStepColorKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkStepColorKeys[uiInsertAt],
                uiDestSize);
        }

        NiColorA kColor = NiColorKey::GenInterpDefault(fTime, pkStepColorKeys,
                STEPKEY, uiNumKeys, sizeof(NiStepColorKey));

        NiStepColorKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_Color = kColor;

        uiNumKeys++;
        NiDelete[] pkStepColorKeys;
        pkKeys = pkNewKeys;
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiStepColorKey::CubicCoefs(const NiAnimationKey* pkKeys, 
#ifdef NIDEBUG
    unsigned int uiNumKeys, 
#else
    unsigned int, 
#endif 
    unsigned int uiIndex, void* pvValue0, 
    void* pvOutTangent0, void* pvA0, void* pvB0)
{
    NIASSERT(uiIndex < uiNumKeys);
    const NiStepColorKey* pkStepKeys = (const NiStepColorKey*)pkKeys;
    *(NiColorA*)pvValue0 = pkStepKeys[uiIndex].m_Color;
    *(NiColorA*)pvOutTangent0 = NiColorA(0, 0, 0, 0);
    *(NiColorA*)pvA0 = NiColorA(0, 0, 0, 0);
    *(NiColorA*)pvB0 = NiColorA(0, 0, 0, 0);
}
//---------------------------------------------------------------------------
bool NiStepColorKey::IsPosed(const NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys)
{
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys > 0);

    if (uiNumKeys > 1)
    {
        const NiStepColorKey* pkStepKeys = (const NiStepColorKey*)pkKeys;
        NiColorA kValue = pkStepKeys[0].m_Color;
        for (unsigned int ui = 1; ui < uiNumKeys; ui++)
        {
            if (pkStepKeys[ui].m_Color != kValue)
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
NiAnimationKey* NiStepColorKey::CreateFromStream(NiStream& kStream,
    unsigned int uiNumKeys)
{
    NiStepColorKey* pkStepColorKeys = NiNew NiStepColorKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkStepColorKeys[ui].LoadBinary(kStream);
    }
    return pkStepColorKeys;
}
//---------------------------------------------------------------------------
void NiStepColorKey::LoadBinary(NiStream& kStream)
{
    NiColorKey::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiStepColorKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiStepColorKey* pkStepColorKeys = (NiStepColorKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkStepColorKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiStepColorKey::SaveBinary(NiStream& kStream, NiAnimationKey* pkKey)
{
    NiColorKey::SaveBinary(kStream, pkKey);
}
//---------------------------------------------------------------------------

