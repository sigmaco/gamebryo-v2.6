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
#include "NiBezFloatKey.h"
#include <NiRTLib.h>
#include <NiStream.h>

NiImplementAnimationStream(NiBezFloatKey,FLOATKEY,BEZKEY);

//---------------------------------------------------------------------------
void NiBezFloatKey::RegisterSupportedFunctions(KeyContent eContent,
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
void NiBezFloatKey::FillDerivedVals(NiAnimationKey*, unsigned int,
    unsigned char)
{
    // no derived values to fill in
}
//---------------------------------------------------------------------------
bool NiBezFloatKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    if ( !NiFloatKey::Equal(key0,key1) )
        return false;

    const NiBezFloatKey& bez0 = (const NiBezFloatKey&) key0;
    const NiBezFloatKey& bez1 = (const NiBezFloatKey&) key1;

    return (bez0.m_fInTan == bez1.m_fInTan &&
        bez0.m_fOutTan == bez1.m_fOutTan);
}
//---------------------------------------------------------------------------
void NiBezFloatKey::Interpolate(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    const NiBezFloatKey* pBez0 = (const NiBezFloatKey*) pKey0;
    const NiBezFloatKey* pBez1 = (const NiBezFloatKey*) pKey1;

    // interpolate between this key and pQ
    *(float*)pResult = NiInterpScalar::Bezier(fTime,pBez0->GetValue(),
        pBez0->GetOutTan(),pBez1->GetValue(),pBez1->GetInTan());
}
//---------------------------------------------------------------------------
void NiBezFloatKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiBezFloatKey* pkNewBez = (NiBezFloatKey*) pkNewKey;
    NiBezFloatKey* pkOrigBez = (NiBezFloatKey*) pkOrigKey;
    pkNewBez->m_fTime = pkOrigBez->m_fTime;
    pkNewBez->m_fValue = pkOrigBez->m_fValue;
    pkNewBez->m_fInTan = pkOrigBez->m_fInTan;
    pkNewBez->m_fOutTan = pkOrigBez->m_fOutTan;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiBezFloatKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiBezFloatKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiBezFloatKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiBezFloatKey* pkBezFloatKeyArray = (NiBezFloatKey*) pkKeyArray;
    NiDelete[] pkBezFloatKeyArray;
}
//---------------------------------------------------------------------------
bool NiBezFloatKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiBezFloatKey* pkBezFloatKeys = (NiBezFloatKey*) pkKeys;

    unsigned int uiByteSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkBezFloatKeys, uiNumKeys, uiInsertAt,
        sizeof(NiBezFloatKey)))
    {
        NiBezFloatKey* pkNewKeys = NiNew NiBezFloatKey[uiNumKeys + 1];
        uiByteSize = (uiNumKeys + 1) * sizeof(NiBezFloatKey);
        NiMemcpy(pkNewKeys, uiByteSize, pkBezFloatKeys,
            uiInsertAt * sizeof(NiBezFloatKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiByteSize = (uiNumKeys - uiInsertAt) * sizeof(NiBezFloatKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], 
                &pkBezFloatKeys[uiInsertAt], uiByteSize);
        }

        float f = NiFloatKey::GenInterpDefault(fTime, pkBezFloatKeys, BEZKEY,
            uiNumKeys, sizeof(NiBezFloatKey));

        NiBezFloatKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_fValue = f;
        pkNewKey->m_fInTan = 0.0f;
        pkNewKey->m_fOutTan = 0.0f;

        // adjust tangents of new key and adjacent keys

        // Check if inserting at head or tail of array
        // If so, tangents are 0.0f.
        if((uiInsertAt != 0) && (uiInsertAt != uiNumKeys))
        {
            // otherwise we're inserting in the middle of the list and need
            // to set the tangents.

            // Get the Keys and their values
            NiBezFloatKey* pkLastKey = &pkNewKeys[uiInsertAt - 1];
            float fLastValue = pkLastKey->GetValue();
            float fLastTime = pkLastKey->GetTime();
            float fLastOut = pkLastKey->GetOutTan();
    
            float fCurrentValue = pkNewKey->GetValue();
            float fCurrentTime = pkNewKey->GetTime();    
            float fCurrentIn, fCurrentOut;
    
            NiBezFloatKey* pkNextKey = &pkNewKeys[uiInsertAt + 1];
            float fNextValue = pkNextKey->GetValue();
            float fNextTime = pkNextKey->GetTime();
            float fNextIn = pkNextKey->GetInTan();
    
            // Handle the middle of the list
            NiInterpScalar::AdjustBezier( fLastValue, fLastTime, fLastOut,
                fNextValue, fNextTime, fNextIn, fCurrentTime, fCurrentValue, 
                fCurrentIn, fCurrentOut);
        
            // Update the tangents
            pkLastKey->SetOutTan(fLastOut);
    
            pkNewKey->SetInTan(fCurrentIn);
            pkNewKey->SetOutTan(fCurrentOut);
    
            pkNextKey->SetInTan(fNextIn);
        }

        uiNumKeys++;
        NiDelete[] pkBezFloatKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys, sizeof(NiBezFloatKey));
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiBezFloatKey::CubicCoefs(const NiAnimationKey* pkKeys, 
#ifdef NIDEBUG
    unsigned int uiNumKeys, 
#else
    unsigned int, 
#endif 
    unsigned int uiIndex, void* pvValue0, 
    void* pvOutTangent0, void* pvA0, void* pvB0)
{
    NIASSERT(uiIndex < uiNumKeys - 1);
    const NiBezFloatKey* pkBezKeys = (const NiBezFloatKey*)pkKeys;

    float fValue0 = pkBezKeys[uiIndex].m_fValue;
    float fOutTangent0 = pkBezKeys[uiIndex].m_fOutTan;
    float fValue1 = pkBezKeys[uiIndex + 1].m_fValue;
    float fInTangent1 = pkBezKeys[uiIndex + 1].m_fInTan;

    *(float*)pvValue0 = fValue0;
    *(float*)pvOutTangent0 = fOutTangent0;

    // A = 3(X1 - X0) - (T1 + 2T0)
    *(float*)pvA0 = 3 * (fValue1 - fValue0) - (fInTangent1 + 2 * fOutTangent0);

    // B = (T0 + T1) - 2(X1 - X0)
    *(float*)pvB0 = fOutTangent0 + fInTangent1 - 2 * (fValue1 - fValue0);
}
//---------------------------------------------------------------------------
bool NiBezFloatKey::IsPosed(const NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys)
{
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys > 0);

    if (uiNumKeys > 1)
    {
        const NiBezFloatKey* pkBezKeys = (const NiBezFloatKey*)pkKeys;
        if (pkBezKeys[0].m_fInTan != 0.0f ||
            pkBezKeys[0].m_fOutTan != 0.0f)
        {
            return false;
        }

        float fValue = pkBezKeys[0].m_fValue;
        for (unsigned int ui = 1; ui < uiNumKeys; ui++)
        {
            if (pkBezKeys[ui].m_fValue != fValue ||
                pkBezKeys[ui].m_fInTan != 0.0f ||
                pkBezKeys[ui].m_fOutTan != 0.0f)
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
NiAnimationKey* NiBezFloatKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiBezFloatKey* pkBezFloatKeys = NiNew NiBezFloatKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkBezFloatKeys[ui].LoadBinary(stream);
    }
    return pkBezFloatKeys;
}
//---------------------------------------------------------------------------
void NiBezFloatKey::LoadBinary(NiStream& stream)
{
    NiFloatKey::LoadBinary(stream);
    NiStreamLoadBinary(stream, m_fInTan);
    NiStreamLoadBinary(stream, m_fOutTan);
}
//---------------------------------------------------------------------------
void NiBezFloatKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiBezFloatKey* pkBezFloatKeys = (NiBezFloatKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkBezFloatKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiBezFloatKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiBezFloatKey* pkBezKey = (NiBezFloatKey*) pkKey;
    
    NiFloatKey::SaveBinary(stream, pkKey);
    NiStreamSaveBinary(stream, pkBezKey->m_fInTan);
    NiStreamSaveBinary(stream, pkBezKey->m_fOutTan);
}
//---------------------------------------------------------------------------
