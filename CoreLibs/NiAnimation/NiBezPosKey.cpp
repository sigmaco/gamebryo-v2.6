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

#include "NiBezPosKey.h"
#include "NiInterpScalar.h"

NiImplementAnimationStream(NiBezPosKey,POSKEY,BEZKEY);

//---------------------------------------------------------------------------
void NiBezPosKey::RegisterSupportedFunctions(KeyContent eContent,
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
bool NiBezPosKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    if (!NiPosKey::Equal(key0, key1))
    {
        return false;
    }

    const NiBezPosKey& bez0 = (const NiBezPosKey&)key0;
    const NiBezPosKey& bez1 = (const NiBezPosKey&)key1;

    // Ignore the derived values of m_A and m_B.
    return bez0.m_InTan == bez1.m_InTan &&
           bez0.m_OutTan == bez1.m_OutTan;
}
//---------------------------------------------------------------------------
void NiBezPosKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiBezPosKey* pkNewBez = (NiBezPosKey*) pkNewKey;
    NiBezPosKey* pkOrigBez = (NiBezPosKey*) pkOrigKey;

    pkNewBez->m_fTime = pkOrigBez->m_fTime;
    pkNewBez->m_Pos = pkOrigBez->m_Pos;
    pkNewBez->m_InTan = pkOrigBez->m_InTan;
    pkNewBez->m_OutTan = pkOrigBez->m_OutTan;
    pkNewBez->m_A = pkOrigBez->m_A;
    pkNewBez->m_B = pkOrigBez->m_B;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiBezPosKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiBezPosKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiBezPosKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiBezPosKey* pkBezPosKeyArray = (NiBezPosKey*) pkKeyArray;
    NiDelete[] pkBezPosKeyArray;
}
//---------------------------------------------------------------------------
void NiBezPosKey::Interpolate(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey*, void* pResult)
{
    NiBezPosKey* pBezKey0 = (NiBezPosKey*) pKey0;

    // interpolate function
    *(NiPoint3*)pResult = pBezKey0->GetPos() + (pBezKey0->m_OutTan + 
        (pBezKey0->m_A + pBezKey0->m_B*fTime)*fTime)*fTime;
}
//---------------------------------------------------------------------------
void NiBezPosKey::InterpolateD1(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey*, void* pResult)
{
    NiBezPosKey* pBezKey0 = (NiBezPosKey*) pKey0;

    // interpolate first derivative
    *(NiPoint3*)pResult = pBezKey0->m_OutTan + (2.0f*pBezKey0->m_A +
        pBezKey0->m_B*(3.0f*fTime))*fTime;
}
//---------------------------------------------------------------------------
void NiBezPosKey::InterpolateD2(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey*, void* pResult)
{
    NiBezPosKey* pBezKey0 = (NiBezPosKey*) pKey0;

    // interpolate second derivative
    *(NiPoint3*)pResult = 2.0f*pBezKey0->m_A + pBezKey0->m_B*(6.0f*fTime);
}
//---------------------------------------------------------------------------
void NiBezPosKey::FillDerivedVals(NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys, unsigned char)
{
    NiBezPosKey* pkBezPosKeys = (NiBezPosKey*) pkKeys;

    for (unsigned int uiI = 0; uiI < uiNumKeys-1; uiI++)
    {
        NiBezPosKey* pBez0 = &pkBezPosKeys[uiI];
        NiBezPosKey* pBez1 = &pkBezPosKeys[uiI+1];

        // A = 3(X1 - X0) - (T1 + 2T0)
        pBez0->m_A = 3*(pBez1->m_Pos - pBez0->m_Pos)
            - (pBez1->m_InTan + 2*pBez0->m_OutTan);

        // B = (T0 + T1) - 2(X1 - X0)
        pBez0->m_B = pBez0->m_OutTan + pBez1->m_InTan
            - 2*(pBez1->m_Pos - pBez0->m_Pos);
    }
}
//---------------------------------------------------------------------------
float NiBezPosKey::ComputeMaxCurvature(NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiBezPosKey* pkBezPosKeys = (NiBezPosKey*) pkKeys;

    float fMaxCurvature = 0.0f;
    for (unsigned int uiI = 0; uiI < uiNumKeys-1; uiI++)
    {
        NiBezPosKey* pBez0 = &pkBezPosKeys[uiI];
        NiBezPosKey* pBez1 = &pkBezPosKeys[uiI+1];

        for (int uiJ = 0; uiJ <= ms_iSampleSize; uiJ++)
        {
            float fTime = uiJ*GetMaxCurvatureSampleDelta();
            NiPoint3 d1,d2;
            InterpolateD1(fTime,pBez0,pBez1,&d1);
            InterpolateD2(fTime,pBez0,pBez1,&d2);
            float fCurvature = d1.Cross(d2).Length()/d1.SqrLength();
            if ( fCurvature > fMaxCurvature )
                fMaxCurvature = fCurvature;
        }
    }

    return fMaxCurvature;
}
//---------------------------------------------------------------------------
bool NiBezPosKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiBezPosKey* pkBezPosKeys = (NiBezPosKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkBezPosKeys, uiNumKeys, uiInsertAt,
        sizeof(NiBezPosKey)))
    {
        NiBezPosKey* pkNewKeys = NiNew NiBezPosKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiBezPosKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkBezPosKeys, 
            uiInsertAt * sizeof(NiBezPosKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiBezPosKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkBezPosKeys[uiInsertAt], 
                uiDestSize);
        }

        NiPoint3 kPt = NiPosKey::GenInterpDefault(fTime, pkBezPosKeys,
                BEZKEY, uiNumKeys, sizeof(NiBezPosKey));

        NiBezPosKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_Pos = kPt;
        pkNewKey->m_InTan = NiPoint3::ZERO;
        pkNewKey->m_OutTan = NiPoint3::ZERO;

        // adjust tangents of new key and adjacent keys

        // Check if inserting at head or tail of array
        // If so, tangents are zero.
        if((uiInsertAt != 0) && (uiInsertAt != uiNumKeys))
        {
            // otherwise we're inserting in the middle of the list and need
            // to set the tangents.

            // Get the Keys and their values
            NiBezPosKey* pkLastKey = &pkNewKeys[uiInsertAt - 1];
            NiPoint3 kLastValue = pkLastKey->GetPos();
            float fLastTime = pkLastKey->GetTime();
            NiPoint3 kLastOut = pkLastKey->GetOutTan();
    
            NiPoint3 kCurrentValue = pkNewKey->GetPos();
            float fCurrentTime = pkNewKey->GetTime();    
            NiPoint3 kCurrentIn, kCurrentOut;
    
            NiBezPosKey* pkNextKey = &pkNewKeys[uiInsertAt + 1];
            NiPoint3 kNextValue = pkNextKey->GetPos();
            float fNextTime = pkNextKey->GetTime();
            NiPoint3 kNextIn = pkNextKey->GetInTan();
    
            // Handle the middle of the list
            NiInterpScalar::AdjustBezier( kLastValue.x, fLastTime,
                kLastOut.x, kNextValue.x, fNextTime, kNextIn.x,
                fCurrentTime, kCurrentValue.x, kCurrentIn.x, 
                kCurrentOut.x);
            NiInterpScalar::AdjustBezier( kLastValue.y, fLastTime,
                kLastOut.y, kNextValue.y, fNextTime, kNextIn.y,
                fCurrentTime, kCurrentValue.y, kCurrentIn.y, 
                kCurrentOut.y);
            NiInterpScalar::AdjustBezier( kLastValue.z, fLastTime,
                kLastOut.z, kNextValue.z, fNextTime, kNextIn.z,
                fCurrentTime, kCurrentValue.z, kCurrentIn.z, 
                kCurrentOut.z);
        
            // Update the tangents
            pkLastKey->SetOutTan(kLastOut);
    
            pkNewKey->SetInTan(kCurrentIn);
            pkNewKey->SetOutTan(kCurrentOut);
    
            pkNextKey->SetInTan(kNextIn);
        }

        uiNumKeys++;
        NiDelete[] pkBezPosKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys, sizeof(NiBezPosKey));
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiBezPosKey::CubicCoefs(const NiAnimationKey* pkKeys, 
#ifdef NIDEBUG
    unsigned int uiNumKeys, 
#else
    unsigned int, 
#endif 
    unsigned int uiIndex, void* pvValue0, 
    void* pvOutTangent0, void* pvA0, void* pvB0)
{
    NIASSERT(uiIndex < uiNumKeys - 1);

    const NiBezPosKey* pkBezKeys = (const NiBezPosKey*)pkKeys;
    *(NiPoint3*)pvValue0 = pkBezKeys[uiIndex].m_Pos;
    *(NiPoint3*)pvOutTangent0 = pkBezKeys[uiIndex].m_OutTan;
    *(NiPoint3*)pvA0 = pkBezKeys[uiIndex].m_A;
    *(NiPoint3*)pvB0 = pkBezKeys[uiIndex].m_B;
}
//---------------------------------------------------------------------------
bool NiBezPosKey::IsPosed(const NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys)
{
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys > 0);

    if (uiNumKeys > 1)
    {
        const NiBezPosKey* pkBezKeys = (const NiBezPosKey*)pkKeys;
        NiPoint3 kValue = pkBezKeys[0].m_Pos;
        for (unsigned int ui = 0; ui < uiNumKeys; ui++)
        {
            // Check for identical values and flat tangents.
            if (pkBezKeys[ui].m_Pos != kValue ||
                pkBezKeys[ui].m_InTan != NiPoint3::ZERO ||
                pkBezKeys[ui].m_OutTan != NiPoint3::ZERO)
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
NiAnimationKey* NiBezPosKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiBezPosKey* pkBezPosKeys = NiNew NiBezPosKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkBezPosKeys[ui].LoadBinary(stream);
    }
    return pkBezPosKeys;
}
//---------------------------------------------------------------------------
void NiBezPosKey::LoadBinary(NiStream& stream)
{
    NiPosKey::LoadBinary(stream);

    m_InTan.LoadBinary(stream);
    m_OutTan.LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiBezPosKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiBezPosKey* pkBezPosKeys = (NiBezPosKey*)pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkBezPosKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiBezPosKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiPosKey::SaveBinary(stream, pkKey);

    NiBezPosKey* pkBezKey = (NiBezPosKey*)pkKey;
    pkBezKey->m_InTan.SaveBinary(stream);
    pkBezKey->m_OutTan.SaveBinary(stream);

    // Ignore the derived values of m_A and m_B.
}
//---------------------------------------------------------------------------
