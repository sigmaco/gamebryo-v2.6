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

#include "NiTCBRotKey.h"

NiImplementAnimationStream(NiTCBRotKey,ROTKEY,TCBKEY);

//---------------------------------------------------------------------------
void NiTCBRotKey::RegisterSupportedFunctions(KeyContent eContent,
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
bool NiTCBRotKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    if (!NiRotKey::Equal(key0, key1))
    {
        return false;
    }

    const NiTCBRotKey& tcb0 = (const NiTCBRotKey&)key0;
    const NiTCBRotKey& tcb1 = (const NiTCBRotKey&)key1;

    // Ignore the derived values of m_A and m_B.
    return tcb0.m_fTension == tcb1.m_fTension &&
           tcb0.m_fContinuity == tcb1.m_fContinuity &&
           tcb0.m_fBias == tcb1.m_fBias;
}
//---------------------------------------------------------------------------
void NiTCBRotKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiTCBRotKey* pkNewTCB = (NiTCBRotKey*) pkNewKey;
    NiTCBRotKey* pkOrigTCB = (NiTCBRotKey*) pkOrigKey;

    pkNewTCB->m_fTime = pkOrigTCB->m_fTime;
    pkNewTCB->m_quat = pkOrigTCB->m_quat;
    pkNewTCB->m_fTension = pkOrigTCB->m_fTension;
    pkNewTCB->m_fContinuity = pkOrigTCB->m_fContinuity;
    pkNewTCB->m_fBias = pkOrigTCB->m_fBias;
    pkNewTCB->m_A = pkOrigTCB->m_A;
    pkNewTCB->m_B = pkOrigTCB->m_B;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiTCBRotKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiTCBRotKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiTCBRotKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiTCBRotKey* pkTCBRotKeyArray = (NiTCBRotKey*) pkKeyArray;
    NiDelete[] pkTCBRotKeyArray;
}
//---------------------------------------------------------------------------
void NiTCBRotKey::Interpolate(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    NiTCBRotKey* pTCB0 = (NiTCBRotKey*) pKey0;
    NiTCBRotKey* pTCB1 = (NiTCBRotKey*) pKey1;

    *(NiQuaternion*)pResult = NiQuaternion::Squad(fTime, pTCB0->m_quat,
        pTCB0->m_A, pTCB1->m_B, pTCB1->m_quat);
}
//---------------------------------------------------------------------------
void NiTCBRotKey::CalculateDVals(const NiTCBRotKey* pPrev, 
    const NiTCBRotKey* pNext)
{
    // Build logs and angles between prev, this, and next quats
    NiQuaternion logPrev =
        NiQuaternion::Log(NiQuaternion::UnitInverse(pPrev->m_quat)*m_quat);
    NiQuaternion logNext =
        NiQuaternion::Log(NiQuaternion::UnitInverse(m_quat)*pNext->m_quat);

    // Build multipliers 
    float fInv = 1.0f/(pNext->m_fTime - pPrev->m_fTime);
    float mt = 1.0f - m_fTension;
    float mc = 1.0f - m_fContinuity;
    float pc = 1.0f + m_fContinuity;
    float mb = 1.0f - m_fBias;
    float pb = 1.0f + m_fBias;
    float adjVal = (m_fTime - pPrev->m_fTime)*fInv;
    float d0 = adjVal * mt * pc * pb;
    float d1 = adjVal * mt * mc * mb;

    // Build outgoing tangent
    NiQuaternion Tout = d1 * logNext + d0 * logPrev;

    // Set A
    m_A = m_quat * NiQuaternion::Exp(0.5f * (Tout - logNext));

    // Adjust multipliers
    adjVal = (pNext->m_fTime - m_fTime)*fInv;
    d0 = adjVal * mt * mc * pb;
    d1 = adjVal * mt * pc * mb;

    // Build incoming tangent 
    NiQuaternion Tin = d1 * logNext + d0 * logPrev;

    // Set B
    m_B = m_quat * NiQuaternion::Exp(0.5f * (logPrev - Tin));
}
//---------------------------------------------------------------------------
void NiTCBRotKey::FillDerivedVals(NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys, unsigned char ucSize)
{
    if ( uiNumKeys >= 2 )
    {
        // consecutive quaternions are set to have an acute angle
        NiRotKey::FillDerivedVals(pkKeys, uiNumKeys, ucSize);

        // calculate the derived A & B quaternion values
        NiTCBRotKey* pkTCBRotKeys = (NiTCBRotKey*) pkKeys;

        pkTCBRotKeys[0].CalculateDVals(&pkTCBRotKeys[0], &pkTCBRotKeys[1]);

        unsigned int uiNumKeysM1 = uiNumKeys-1;
        for (unsigned int uiI = 1; uiI < uiNumKeysM1; uiI++)
        {
            pkTCBRotKeys[uiI].CalculateDVals(&pkTCBRotKeys[uiI-1],
                &pkTCBRotKeys[uiI+1]);
        }

        pkTCBRotKeys[uiNumKeysM1].CalculateDVals(&pkTCBRotKeys[uiNumKeys-2],
            &pkTCBRotKeys[uiNumKeysM1]);
    }
}
//---------------------------------------------------------------------------
bool NiTCBRotKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiTCBRotKey* pkTCBRotKeys = (NiTCBRotKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkTCBRotKeys, uiNumKeys, uiInsertAt,
        sizeof(NiTCBRotKey)))
    {
        NiTCBRotKey* pkNewKeys = NiNew NiTCBRotKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiTCBRotKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkTCBRotKeys, 
            uiInsertAt * sizeof(NiTCBRotKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiTCBRotKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkTCBRotKeys[uiInsertAt], 
                uiDestSize);
        }

        NiQuaternion kQ = NiRotKey::GenInterpDefault(fTime, pkTCBRotKeys,
            TCBKEY, uiNumKeys, sizeof(NiTCBRotKey));

        // Default Tension = 0, Continuity = 0, Bias = 0.
        NiTCBRotKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_quat = kQ;
        pkNewKey->m_fTension = 0.0f;
        pkNewKey->m_fContinuity = 0.0f;
        pkNewKey->m_fBias = 0.0f;

        uiNumKeys++;
        NiDelete[] pkTCBRotKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys, sizeof(NiTCBRotKey));
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiTCBRotKey::CubicCoefs(const NiAnimationKey* pkKeys, 
#ifdef NIDEBUG
    unsigned int uiNumKeys, 
#else
    unsigned int, 
#endif 
    unsigned int uiIndex, void* pvQuat0, 
    void* pvQuat1, void* pvOutQuat0, void* pvInQuat1)
{
    // Parameter names have been changed locally to reflect their semantics.
    // pvQuat0 is pvValue0. pvQuat1 is pvOutTangent0.
    // pvOutQuat0 is pvA0. pvInQuat1 is pvB0.
    NIASSERT(uiNumKeys >= 2);
    NIASSERT(uiIndex < uiNumKeys - 1);

    const NiTCBRotKey* pkTCBKeys = (const NiTCBRotKey*)pkKeys;
    *(NiQuaternion*)pvQuat0 = pkTCBKeys[uiIndex].m_quat;
    *(NiQuaternion*)pvQuat1 = pkTCBKeys[uiIndex + 1].m_quat;
    *(NiQuaternion*)pvOutQuat0 = pkTCBKeys[uiIndex].m_A;
    *(NiQuaternion*)pvInQuat1 = pkTCBKeys[uiIndex + 1].m_B;
}
//---------------------------------------------------------------------------
bool NiTCBRotKey::IsPosed(const NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys)
{
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys >= 2);

    if (uiNumKeys > 1)
    {
        const NiTCBRotKey* pkTCBKeys = (const NiTCBRotKey*)pkKeys;
        NiQuaternion kValue = pkTCBKeys[0].m_quat;
        for (unsigned int ui = 0; ui < uiNumKeys; ui++)
        {
            // Check for identical values and intermediate values.
            if (pkTCBKeys[ui].m_quat != kValue ||
                pkTCBKeys[ui].m_A != kValue || 
                pkTCBKeys[ui].m_B != kValue)
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
NiAnimationKey* NiTCBRotKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiTCBRotKey* pkTCBRotKeys = NiNew NiTCBRotKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkTCBRotKeys[ui].LoadBinary(stream);
    }
    return pkTCBRotKeys;
}
//---------------------------------------------------------------------------
void NiTCBRotKey::LoadBinary(NiStream& stream)
{
    NiRotKey::LoadBinary(stream);

    NiStreamLoadBinary(stream, m_fTension);
    NiStreamLoadBinary(stream, m_fContinuity);
    NiStreamLoadBinary(stream, m_fBias);
}
//---------------------------------------------------------------------------
void NiTCBRotKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiTCBRotKey* pkTCBRotKeys = (NiTCBRotKey*)pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkTCBRotKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiTCBRotKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiRotKey::SaveBinary(stream, pkKey);

    NiTCBRotKey* pkTCBKey = (NiTCBRotKey*)pkKey;
    NiStreamSaveBinary(stream, pkTCBKey->m_fTension);
    NiStreamSaveBinary(stream, pkTCBKey->m_fContinuity);
    NiStreamSaveBinary(stream, pkTCBKey->m_fBias);

    // Ignore the derived values of m_A and m_B.
}
//---------------------------------------------------------------------------
