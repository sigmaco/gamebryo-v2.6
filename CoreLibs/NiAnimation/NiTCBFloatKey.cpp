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
#include "NiTCBFloatKey.h"
#include <NiRTLib.h>
#include <NiStream.h>

NiImplementAnimationStream(NiTCBFloatKey,FLOATKEY,TCBKEY);

//---------------------------------------------------------------------------
void NiTCBFloatKey::RegisterSupportedFunctions(KeyContent eContent,
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
bool NiTCBFloatKey::Equal(const NiAnimationKey& key0,
    const NiAnimationKey& key1)
{
    if (!NiFloatKey::Equal(key0, key1))
    {
        return false;
    }

    const NiTCBFloatKey& tcb0 = (const NiTCBFloatKey&)key0;
    const NiTCBFloatKey& tcb1 = (const NiTCBFloatKey&)key1;

    // Ignore the derived values of m_fDS and m_fDD.
    return tcb0.m_fTension == tcb1.m_fTension &&
           tcb0.m_fContinuity == tcb1.m_fContinuity &&
           tcb0.m_fBias == tcb1.m_fBias;
}
//---------------------------------------------------------------------------
void NiTCBFloatKey::Interpolate(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    const NiTCBFloatKey* pTcb0 = (const NiTCBFloatKey*) pKey0;
    const NiTCBFloatKey* pTcb1 = (const NiTCBFloatKey*) pKey1;

    // interpolate between this key and pQ
    *(float*)pResult = NiInterpScalar::TCB(fTime,pTcb0->GetValue(),
        pTcb0->m_fDD,pTcb1->GetValue(),pTcb1->m_fDS);
}
//---------------------------------------------------------------------------
void NiTCBFloatKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiTCBFloatKey* pkNewTCB = (NiTCBFloatKey*) pkNewKey;
    NiTCBFloatKey* pkOrigTCB = (NiTCBFloatKey*) pkOrigKey;

    pkNewTCB->m_fTime = pkOrigTCB->m_fTime;
    pkNewTCB->m_fValue = pkOrigTCB->m_fValue;
    pkNewTCB->m_fTension = pkOrigTCB->m_fTension;
    pkNewTCB->m_fContinuity = pkOrigTCB->m_fContinuity;
    pkNewTCB->m_fBias = pkOrigTCB->m_fBias;
    pkNewTCB->m_fDS = pkOrigTCB->m_fDS;
    pkNewTCB->m_fDD = pkOrigTCB->m_fDD;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiTCBFloatKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiTCBFloatKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiTCBFloatKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiTCBFloatKey* pkTCBFloatKeyArray = (NiTCBFloatKey*) pkKeyArray;
    NiDelete[] pkTCBFloatKeyArray;
}
//---------------------------------------------------------------------------
void NiTCBFloatKey::CalculateDVals(NiTCBFloatKey& result, float fValSub1,
    float fValPlus1, float fPreLen, float fNextLen)
{
    float diff1 = result.GetValue() - fValSub1;
    float diff2 = fValPlus1 - result.GetValue();

    float opc = 1.0f + result.GetContinuity();
    float omc = 1.0f - result.GetContinuity();
    float opb = 1.0f + result.GetBias();
    float omb = 1.0f - result.GetBias();
    float homt = 0.5f*(1.0f-result.GetTension());
    float homtopc = homt*opc;
    float homtomc = homt*omc;

    result.m_fDS = (homtomc*opb)*diff1 + (homtopc*omb)*diff2;
    result.m_fDD = (homtopc*opb)*diff1 + (homtomc*omb)*diff2;

    // These adjustments compensate for varying numbers of "inbetweens" 
    // that will be calculated between keys.  They are approximate. 
    // See Kochanek, D. H. U., Bartels, R. "Interpolating Splines with 
    // Local Tension, Continuity, and Bias Control", SIGGRAPH 1984.
    // The code here is an interpretation for time-based 
    // animations based on their discussion of adjustments for 
    // parameter step size.  The length of the segment between keys
    // is used to calculate the adjustment.

    float fInv = 2.0f/(fPreLen + fNextLen);
    result.m_fDS *= fPreLen*fInv;
    result.m_fDD *= fNextLen*fInv;
}
//---------------------------------------------------------------------------
void NiTCBFloatKey::FillDerivedVals(NiAnimationKey* pkKeys,
    unsigned int uiNumKeys, unsigned char)
{
    NiTCBFloatKey* pkTCBFloatKeys = (NiTCBFloatKey*) pkKeys;

    if ( uiNumKeys >= 2 )
    {
        // Extend the previous value a magnitude equivalent to the
        // difference between the first and second values.
        NiTCBFloatKey::CalculateDVals(pkTCBFloatKeys[0],
            (pkTCBFloatKeys[0].m_fValue * 2.0f) - pkTCBFloatKeys[1].m_fValue,
            pkTCBFloatKeys[1].m_fValue, 1, 1);

        unsigned int uiNumKeysM1 = uiNumKeys - 1;
        for (unsigned int uiI = 1; uiI < uiNumKeysM1; uiI++)
        {
            NiTCBFloatKey::CalculateDVals(pkTCBFloatKeys[uiI],
                pkTCBFloatKeys[uiI-1].m_fValue, 
                pkTCBFloatKeys[uiI+1].m_fValue,
                pkTCBFloatKeys[uiI].m_fTime - pkTCBFloatKeys[uiI-1].m_fTime,
                pkTCBFloatKeys[uiI+1].m_fTime - pkTCBFloatKeys[uiI].m_fTime);
        }

        // Extend the next value an amount equivalent to the
        // difference between the last and second.
        unsigned int uiNumKeysM2 = uiNumKeys - 2;
        NiTCBFloatKey::CalculateDVals(pkTCBFloatKeys[uiNumKeysM1],
            pkTCBFloatKeys[uiNumKeysM2].m_fValue,
            2.0f * pkTCBFloatKeys[uiNumKeysM1].m_fValue -
            pkTCBFloatKeys[uiNumKeysM2].m_fValue, 1, 1);
    }
}
//---------------------------------------------------------------------------
bool NiTCBFloatKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiTCBFloatKey* pkTCBFloatKeys = (NiTCBFloatKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkTCBFloatKeys, uiNumKeys, uiInsertAt,
        sizeof(NiTCBFloatKey)))
    {
        NiTCBFloatKey* pkNewKeys = NiNew NiTCBFloatKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiTCBFloatKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkTCBFloatKeys,
            uiInsertAt * sizeof(NiTCBFloatKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiTCBFloatKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkTCBFloatKeys[uiInsertAt],
                uiDestSize);
        }

        float f = NiFloatKey::GenInterpDefault(fTime, pkTCBFloatKeys,
            TCBKEY, uiNumKeys,sizeof(NiTCBFloatKey));

        // Default Tension = 0, Continuity = 0, Bias = 0.
        NiTCBFloatKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_fValue = f;
        pkNewKey->m_fTension = 0.0f;
        pkNewKey->m_fContinuity = 0.0f;
        pkNewKey->m_fBias = 0.0f;

        uiNumKeys++;
        NiDelete[] pkTCBFloatKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys, sizeof(NiTCBFloatKey));
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiTCBFloatKey::CubicCoefs(const NiAnimationKey* pkKeys, 
#ifdef NIDEBUG
    unsigned int uiNumKeys, 
#else
    unsigned int, 
#endif 
    unsigned int uiIndex, void* pvValue0, 
    void* pvOutTangent0, void* pvA0, void* pvB0)
{
    NIASSERT(uiNumKeys >= 2);
    NIASSERT(uiIndex < uiNumKeys - 1);

    const NiTCBFloatKey* pkTCBKeys = (const NiTCBFloatKey*)pkKeys;

    float fValue0 = pkTCBKeys[uiIndex].m_fValue;
    float fDD0 = pkTCBKeys[uiIndex].m_fDD;
    float fValue1 = pkTCBKeys[uiIndex + 1].m_fValue;
    float fDS1 = pkTCBKeys[uiIndex + 1].m_fDS;

    *(float*)pvValue0 = fValue0;
    *(float*)pvOutTangent0 = fDD0;

    // A = 3(X1 - X0) - (T1 + 2T0)
    *(float*)pvA0 = 3 * (fValue1 - fValue0) - (fDS1 + 2 * fDD0);

    // B = (T0 + T1) - 2(X1 - X0)
    *(float*)pvB0 = fDD0 + fDS1 - 2 * (fValue1 - fValue0);
}
//---------------------------------------------------------------------------
bool NiTCBFloatKey::IsPosed(const NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys)
{
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys >= 2);

    if (uiNumKeys > 1)
    {
        const NiTCBFloatKey* pkTCBKeys = (const NiTCBFloatKey*)pkKeys;
        float fValue = pkTCBKeys[0].m_fValue;
        for (unsigned int ui = 0; ui < uiNumKeys; ui++)
        {
            // Check for identical values and flat tangents.
            if (pkTCBKeys[ui].m_fValue != fValue ||
                pkTCBKeys[ui].m_fDS != 0.0f || 
                pkTCBKeys[ui].m_fDD != 0.0f)
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
NiAnimationKey* NiTCBFloatKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiTCBFloatKey* pkTCBFloatKeys = NiNew NiTCBFloatKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkTCBFloatKeys[ui].LoadBinary(stream);
    }
    return pkTCBFloatKeys;
}
//---------------------------------------------------------------------------
void NiTCBFloatKey::LoadBinary(NiStream& stream)
{
    NiFloatKey::LoadBinary(stream);

    NiStreamLoadBinary(stream, m_fTension);
    NiStreamLoadBinary(stream, m_fContinuity);
    NiStreamLoadBinary(stream, m_fBias);
}
//---------------------------------------------------------------------------
void NiTCBFloatKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiTCBFloatKey* pkTCBFloatKeys = (NiTCBFloatKey*)pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkTCBFloatKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiTCBFloatKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiFloatKey::SaveBinary(stream, pkKey);

    NiTCBFloatKey* pkTCBKey = (NiTCBFloatKey*)pkKey;
    NiStreamSaveBinary(stream, pkTCBKey->m_fTension);
    NiStreamSaveBinary(stream, pkTCBKey->m_fContinuity);
    NiStreamSaveBinary(stream, pkTCBKey->m_fBias);

    // Ignore the derived values of m_fDS and m_fDD.
}
//---------------------------------------------------------------------------
