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

#include "NiTCBPosKey.h"

NiImplementAnimationStream(NiTCBPosKey,POSKEY,TCBKEY);

//---------------------------------------------------------------------------
void NiTCBPosKey::RegisterSupportedFunctions(KeyContent eContent,
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
bool NiTCBPosKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    if (!NiPosKey::Equal(key0, key1))
    {
        return false;
    }

    const NiTCBPosKey& tcb0 = (const NiTCBPosKey&)key0;
    const NiTCBPosKey& tcb1 = (const NiTCBPosKey&)key1;

    // Ignore the derived values of m_DS, m_DD, m_A, and m_B.
    return tcb0.m_fTension == tcb1.m_fTension &&
           tcb0.m_fContinuity == tcb1.m_fContinuity &&
           tcb0.m_fBias == tcb1.m_fBias;
}
//---------------------------------------------------------------------------
void NiTCBPosKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiTCBPosKey* pkNewTCB = (NiTCBPosKey*) pkNewKey;
    NiTCBPosKey* pkOrigTCB = (NiTCBPosKey*) pkOrigKey;

    pkNewTCB->m_fTime = pkOrigTCB->m_fTime;
    pkNewTCB->m_Pos = pkOrigTCB->m_Pos;
    pkNewTCB->m_fTension = pkOrigTCB->m_fTension;
    pkNewTCB->m_fContinuity = pkOrigTCB->m_fContinuity;
    pkNewTCB->m_fBias = pkOrigTCB->m_fBias;
    pkNewTCB->m_DS = pkOrigTCB->m_DS;
    pkNewTCB->m_DD = pkOrigTCB->m_DD;
    pkNewTCB->m_A = pkOrigTCB->m_A;
    pkNewTCB->m_B = pkOrigTCB->m_B;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiTCBPosKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiTCBPosKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiTCBPosKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiTCBPosKey* pkTCBPosKeyArray = (NiTCBPosKey*) pkKeyArray;
    NiDelete[] pkTCBPosKeyArray;
}
//---------------------------------------------------------------------------
void NiTCBPosKey::Interpolate(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey*, void* pResult)
{
    NiTCBPosKey* pTCBKey0 = (NiTCBPosKey*) pKey0;

    // interpolate position
    *(NiPoint3*)pResult = pTCBKey0->m_Pos + (pTCBKey0->m_DD + 
        (pTCBKey0->m_A + pTCBKey0->m_B*fTime)*fTime)*fTime;
}
//---------------------------------------------------------------------------
void NiTCBPosKey::InterpolateD1(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey*, void* pResult)
{
    NiTCBPosKey* pTCBKey0 = (NiTCBPosKey*) pKey0;

    // interpolate first derivative
    *(NiPoint3*)pResult = pTCBKey0->m_DD + (2.0f*pTCBKey0->m_A +
        pTCBKey0->m_B*(3.0f*fTime))*fTime;
}
//---------------------------------------------------------------------------
void NiTCBPosKey::InterpolateD2(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey*, void* pResult)
{
    NiTCBPosKey* pTCBKey0 = (NiTCBPosKey*) pKey0;

    // interpolate second derivative
    *(NiPoint3*)pResult = 2.0f*pTCBKey0->m_A + pTCBKey0->m_B*(6.0f*fTime);
}
//---------------------------------------------------------------------------
void NiTCBPosKey::CalculateDVals(const NiPoint3& ptSub1, 
    const NiPoint3& ptPlus1, float fPreLen, float fNextLen)
{
    NiPoint3 diff1 = m_Pos - ptSub1;
    NiPoint3 diff2 = ptPlus1 - m_Pos;

    float opc = 1.0f + m_fContinuity;
    float omc = 1.0f - m_fContinuity;
    float opb = 1.0f + m_fBias;
    float omb = 1.0f - m_fBias;
    float homt = 0.5f*(1.0f-m_fTension);
    float homtopc = homt*opc;
    float homtomc = homt*omc;

    m_DS = (homtomc*opb)*diff1 + (homtopc*omb)*diff2;
    m_DD = (homtopc*opb)*diff1 + (homtomc*omb)*diff2;

    // These adjustments compensate for varying numbers of "inbetweens" 
    // that will be calculated between keys.  They are approximate. 
    // See Kochanek, D. H. U., Bartels, R. "Interpolating Splines with 
    // Local Tension, Continuity, and Bias Control", SIGGRAPH 1984.
    // The code here is an interpretation for time-based 
    // animations based on their discussion of adjustments for 
    // parameter step size.  The length of the segment between keys
    // is used to calculate the adjustment.

    float fInv = 2.0f/(fPreLen + fNextLen);
    m_DS *= fPreLen*fInv;
    m_DD *= fNextLen*fInv;
}
//---------------------------------------------------------------------------
void NiTCBPosKey::FillDerivedVals(NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys, unsigned char)
{
    NiTCBPosKey* pkTCBPosKeys = (NiTCBPosKey*) pkKeys;

    unsigned int uiNumKeysM1 = uiNumKeys - 1;
    unsigned int i;

    if ( uiNumKeys >= 2 )
    {
        // Extend the previous point a distance equivalent to the difference
        // between the first and second points on the line between the two.
        pkTCBPosKeys[0].CalculateDVals(2 * pkTCBPosKeys[0].m_Pos
            - pkTCBPosKeys[1].m_Pos, pkTCBPosKeys[1].m_Pos, 1, 1);
       
        for (i = 1; i < uiNumKeysM1; i++)
        {
            NiTCBPosKey* pkTCBM = &pkTCBPosKeys[i-1];
            NiTCBPosKey* pkTCBZ = &pkTCBPosKeys[i];
            NiTCBPosKey* pkTCBP = &pkTCBPosKeys[i+1];

            pkTCBZ->CalculateDVals(pkTCBM->m_Pos, pkTCBP->m_Pos,
                pkTCBZ->m_fTime - pkTCBM->m_fTime,
                pkTCBP->m_fTime - pkTCBZ->m_fTime);
        }

        // Extend the next point a distance equivalent to the
        // difference between the last and second to last points 
        // on the line between the two.
        unsigned int uiNumKeysM2 = uiNumKeys - 2;
        pkTCBPosKeys[uiNumKeysM1].CalculateDVals(
            pkTCBPosKeys[uiNumKeysM2].m_Pos,
            2.0f * pkTCBPosKeys[uiNumKeysM1].m_Pos -
            pkTCBPosKeys[uiNumKeysM2].m_Pos, 1, 1);
    }

    for (i = 0; i < uiNumKeysM1; i++)
    {
        NiTCBPosKey* pTCB0 = &pkTCBPosKeys[i];
        NiTCBPosKey* pTCB1 = &pkTCBPosKeys[i+1];

        // A = 3(P1 - P0) - (D1 + 2D0)
        pTCB0->m_A = 3.0f*(pTCB1->m_Pos - pTCB0->m_Pos)
            - (pTCB1->m_DS + 2.0f*pTCB0->m_DD);

        // B = (D0 + D1) - 2(P1 - P0)
        pTCB0->m_B = pTCB0->m_DD + pTCB1->m_DS
            - 2.0f*(pTCB1->m_Pos - pTCB0->m_Pos);
    }
}
//---------------------------------------------------------------------------
float NiTCBPosKey::ComputeMaxCurvature(NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiTCBPosKey* pkTCBPosKeys = (NiTCBPosKey*) pkKeys;

    float fMaxCurvature = 0.0f;

    for (unsigned int uiI = 0; uiI < uiNumKeys-1; uiI++)
    {
        NiTCBPosKey* pTCB0 = &pkTCBPosKeys[uiI];
        NiTCBPosKey* pTCB1 = &pkTCBPosKeys[uiI+1];

        for (int uiJ = 0; uiJ <= ms_iSampleSize; uiJ++)
        {
            float fTime = uiJ*GetMaxCurvatureSampleDelta();
            NiPoint3 d1, d2;
            InterpolateD1(fTime,pTCB0,pTCB1,&d1);
            InterpolateD2(fTime,pTCB0,pTCB1,&d2);
            float fCurvature = d1.Cross(d2).Length()/d1.SqrLength();
            if ( fCurvature > fMaxCurvature )
                fMaxCurvature = fCurvature;
        }
    }

    return fMaxCurvature;
}
//---------------------------------------------------------------------------
bool NiTCBPosKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiTCBPosKey* pkTCBPosKeys = (NiTCBPosKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkTCBPosKeys, uiNumKeys, uiInsertAt,
        sizeof(NiTCBPosKey)))
    {
        NiTCBPosKey* pkNewKeys = NiNew NiTCBPosKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiTCBPosKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkTCBPosKeys, 
            uiInsertAt * sizeof(NiTCBPosKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiTCBPosKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkTCBPosKeys[uiInsertAt], 
                uiDestSize);
        }

        NiPoint3 kPt = NiPosKey::GenInterpDefault(fTime, pkTCBPosKeys,
            TCBKEY, uiNumKeys,sizeof(NiTCBPosKey));

        // Default Tension = 0, Continuity = 0, Bias = 0.
        NiTCBPosKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_Pos = kPt;
        pkNewKey->m_fTension = 0.0f;
        pkNewKey->m_fContinuity = 0.0f;
        pkNewKey->m_fBias = 0.0f;

        uiNumKeys++;
        NiDelete[] pkTCBPosKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys,sizeof(NiTCBPosKey));
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiTCBPosKey::CubicCoefs(const NiAnimationKey* pkKeys, 
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

    const NiTCBPosKey* pkTCBPosKeys = (const NiTCBPosKey*)pkKeys;

    *(NiPoint3*)pvValue0 = pkTCBPosKeys[uiIndex].m_Pos;
    *(NiPoint3*)pvOutTangent0 = pkTCBPosKeys[uiIndex].m_DD;
    *(NiPoint3*)pvA0 = pkTCBPosKeys[uiIndex].m_A;
    *(NiPoint3*)pvB0 = pkTCBPosKeys[uiIndex].m_B;
}
//---------------------------------------------------------------------------
bool NiTCBPosKey::IsPosed(const NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys)
{
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys >= 2);

    if (uiNumKeys > 1)
    {
        const NiTCBPosKey* pkTCBKeys = (const NiTCBPosKey*)pkKeys;
        NiPoint3 kValue = pkTCBKeys[0].m_Pos;
        for (unsigned int ui = 0; ui < uiNumKeys; ui++)
        {
            // Check for identical values and flat tangents.
            if (pkTCBKeys[ui].m_Pos != kValue ||
                pkTCBKeys[ui].m_DS != NiPoint3::ZERO || 
                pkTCBKeys[ui].m_DD != NiPoint3::ZERO)
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
NiAnimationKey* NiTCBPosKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiTCBPosKey* pkTCBPosKeys = NiNew NiTCBPosKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkTCBPosKeys[ui].LoadBinary(stream);
    }
    return pkTCBPosKeys;
}
//---------------------------------------------------------------------------
void NiTCBPosKey::LoadBinary(NiStream& stream)
{
    NiPosKey::LoadBinary(stream);

    NiStreamLoadBinary(stream, m_fTension);
    NiStreamLoadBinary(stream, m_fContinuity);
    NiStreamLoadBinary(stream, m_fBias);
}
//---------------------------------------------------------------------------
void NiTCBPosKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiTCBPosKey* pkTCBPosKeys = (NiTCBPosKey*)pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkTCBPosKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiTCBPosKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiPosKey::SaveBinary(stream, pkKey);

    NiTCBPosKey* pkTCBKey = (NiTCBPosKey*)pkKey;
    NiStreamSaveBinary(stream, pkTCBKey->m_fTension);
    NiStreamSaveBinary(stream, pkTCBKey->m_fContinuity);
    NiStreamSaveBinary(stream, pkTCBKey->m_fBias);

    // Ignore the derived values of m_DS, m_DD, m_A, and m_B.
}
//---------------------------------------------------------------------------
