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

#include "NiEulerRotKey.h"
#include <NiPoint3.h>


NiImplementAnimationStream(NiEulerRotKey,ROTKEY,EULERKEY);

//---------------------------------------------------------------------------
void NiEulerRotKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, NULL);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, NULL);
    SetInterpD2Function(eContent, eType, NULL);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, FillDerivedVals);
    SetInsertFunction(eContent, eType, Insert);
    SetCubicCoefsFunction(eContent, eType, NULL);
    SetIsPosedFunction(eContent, eType, IsPosed);
}
//---------------------------------------------------------------------------
void NiEulerRotKey::CleanUp()
{
    for (int i = 0; i < 3; i++)
    {
        if (m_apkKeys[i])
        {
            NiFloatKey::DeleteFunction pfnDeleteFunc =
                NiFloatKey::GetDeleteFunction(m_eType[i]);
            NIASSERT(pfnDeleteFunc);
            pfnDeleteFunc(m_apkKeys[i]);
        }

        m_apkKeys[i] = 0;
        m_uiNumKeys[i] = 0;
        m_eType[i] = NiAnimationKey::NOINTERP;
    }
}
//---------------------------------------------------------------------------
void NiEulerRotKey::ReplaceKeys(unsigned char ucIndex, unsigned int uiNumKeys,
    NiFloatKey* pkKeys)
{
    NIASSERT(ucIndex <= 2);

    // Delete the old data.
    if (m_apkKeys[ucIndex])
    {
        NiFloatKey::DeleteFunction pfnDeleteFunc =
            NiFloatKey::GetDeleteFunction(m_eType[ucIndex]);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_apkKeys[ucIndex]);
    }

    SetKeys(ucIndex, pkKeys);
    SetNumKeys(ucIndex, uiNumKeys);
}
//---------------------------------------------------------------------------
void NiEulerRotKey::Interpolate(float fTime, const NiAnimationKey* pkKey0,
    const NiAnimationKey* pkKey1, 
    void* pkResult)
{
    NI_UNUSED_ARG(pkKey1);
    NIASSERT(pkKey1 == 0);

    NiEulerRotKey* pkEuler = (NiEulerRotKey*) pkKey0;
    float angles[3];
    unsigned int i;
    for (i = 0; i < 3; i++)
    {
        if (pkEuler->m_uiNumKeys[i] == 0)
        {
            angles[i] = 0.0f;
        }
        else 
        {
            angles[i] = NiFloatKey::GenInterp(fTime, pkEuler->m_apkKeys[i],
                pkEuler->m_eType[i], pkEuler->m_uiNumKeys[i],
                pkEuler->m_uiLastIdx[i], pkEuler->m_ucSizes[i]);
        }
    }

    ((NiQuaternion*)pkResult)->FromAngleAxesXYZ(angles[0], angles[1], 
        angles[2]);
}
//---------------------------------------------------------------------------
bool NiEulerRotKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    const NiEulerRotKey& euler0 = (const NiEulerRotKey&) key0;
    const NiEulerRotKey& euler1 = (const NiEulerRotKey&) key1;

    for (unsigned int uiI = 0; uiI < 3; uiI++)
    {
        if (euler0.m_uiNumKeys[uiI] != euler1.m_uiNumKeys[uiI] ||
            euler0.m_eType[uiI] != euler1.m_eType[uiI] ||
            euler0.m_ucSizes[uiI] != euler1.m_ucSizes[uiI])
        {
            return false;
        }

        if (euler0.m_uiNumKeys[uiI] > 0)
        {
            NiFloatKey::EqualFunction equal =
                NiFloatKey::GetEqualFunction(euler0.m_eType[uiI]);
            NIASSERT(equal);

            for (unsigned int uiJ = 0; uiJ < euler0.m_uiNumKeys[uiI]; uiJ++)
            {
                if (!equal(*euler0.m_apkKeys[uiI]->GetKeyAt(uiJ, 
                    euler0.m_ucSizes[uiI]),
                    *euler1.m_apkKeys[uiI]->GetKeyAt(uiJ, 
                    euler1.m_ucSizes[uiI])))
                {
                    return false;
                }
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiEulerRotKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiEulerRotKey* pkNewEuler = (NiEulerRotKey*) pkNewKey;
    NiEulerRotKey* pkOrigEuler = (NiEulerRotKey*) pkOrigKey;

    
    for (int uiI = 0; uiI < 3; uiI++)
    {
        pkNewEuler->m_uiNumKeys[uiI] = pkOrigEuler->m_uiNumKeys[uiI];
        pkNewEuler->m_eType[uiI] = pkOrigEuler->m_eType[uiI];
        pkNewEuler->m_ucSizes[uiI] = pkOrigEuler->m_ucSizes[uiI];

        unsigned int uiNumKeys = pkOrigEuler->m_uiNumKeys[uiI];
        if (uiNumKeys)
        {
            NiFloatKey::ArrayFunction array =
                NiFloatKey::GetArrayFunction(pkOrigEuler->m_eType[uiI]);
            NIASSERT(array);
            pkNewEuler->m_apkKeys[uiI] = (NiFloatKey*) array(uiNumKeys);
            NIASSERT(pkNewEuler->m_apkKeys[uiI]);

            NiFloatKey::CopyFunction copy =
                NiFloatKey::GetCopyFunction(pkOrigEuler->m_eType[uiI]);
            NIASSERT(copy);

            for (unsigned int uiJ = 0; uiJ < uiNumKeys; uiJ++)
            {
                copy(pkNewEuler->m_apkKeys[uiI]->GetKeyAt(uiJ, 
                    pkNewEuler->m_ucSizes[uiI]),
                    pkOrigEuler->m_apkKeys[uiI]->GetKeyAt(uiJ,
                    pkOrigEuler->m_ucSizes[uiI]));
            }
        }
    }
}
//---------------------------------------------------------------------------
NiAnimationKey* NiEulerRotKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiEulerRotKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiEulerRotKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiEulerRotKey* pkEulerRotKeyArray = (NiEulerRotKey*) pkKeyArray;
    NiDelete[] pkEulerRotKeyArray;
}
//---------------------------------------------------------------------------
void NiEulerRotKey::FillDerivedVals(NiAnimationKey* pkKeys,
    unsigned int, unsigned char ucSize)
{
    NiEulerRotKey* pkEulerKey = (NiEulerRotKey*) pkKeys->GetKeyAt(0, ucSize);

    for (unsigned int uiI = 0; uiI < 3; uiI++)
    {
        if ( pkEulerKey->m_apkKeys[uiI] )
        {
            NiFloatKey::FillDerivedValsFunction derived =
                NiFloatKey::GetFillDerivedFunction(pkEulerKey->m_eType[uiI]);
            NIASSERT(derived);
            derived(pkEulerKey->m_apkKeys[uiI], pkEulerKey->m_uiNumKeys[uiI],
                pkEulerKey->m_ucSizes[uiI]);
        }
    }
}
//---------------------------------------------------------------------------
bool NiEulerRotKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    // Euler key insertion is a special case. Must insert a key in each
    // of the 3 float key arrays.

    if (uiNumKeys == 0)
    {
        pkKeys = NiNew NiEulerRotKey[1];
        uiNumKeys = 1;
    }

    NiEulerRotKey* pkEulerKey = (NiEulerRotKey*) pkKeys->GetKeyAt(0, 
        sizeof(NiEulerRotKey));

    bool bInsertedKey = false;

    // Handle each of 3 sets of float keys
    for (unsigned int i = 0; i < 3; i++)
    {
        NiFloatKey* pkFloatKeys;
        unsigned int uiNumFloatKeys;
        NiAnimationKey::KeyType eFloatType;

        pkFloatKeys = pkEulerKey->GetKeys((unsigned char)i);
        uiNumFloatKeys = pkEulerKey->GetNumKeys((unsigned char)i);

        if (uiNumFloatKeys == 0)
            continue;

        eFloatType = pkEulerKey->GetType((unsigned char)i);

        InsertFunction fnInsert = NiAnimationKey::GetInsertFunction(FLOATKEY,
            eFloatType);
        NIASSERT(fnInsert);

        // InsertFunction accepts NiAnimationKey pointer; must use temp var
        // to avoid unsafe pointer-to-reference cast
        NiAnimationKey* pkAnimKeys = (NiAnimationKey*)pkFloatKeys;

        if (fnInsert(fTime, pkAnimKeys, uiNumFloatKeys))
        {
            bInsertedKey = true;
        }
        // Set pkFloatKeys after InsertFunction has set temp var
        pkFloatKeys = (NiFloatKey*)pkAnimKeys;

        pkEulerKey->SetNumKeys((unsigned char)i, uiNumFloatKeys);
        pkEulerKey->SetKeys((unsigned char)i, pkFloatKeys);
    }

    if (bInsertedKey)
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiEulerRotKey::IsPosed(const NiAnimationKey* pkKeys, 
#ifdef NIDEBUG
    unsigned int uiNumKeys)
#else
    unsigned int )
#endif 
{
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys == 1);

    NiEulerRotKey* pkEulerKey = (NiEulerRotKey*)
        pkKeys->GetKeyAt(0, sizeof(NiEulerRotKey));
    NIASSERT(pkEulerKey);

    // Check each of 3 sets of float keys.
    for (unsigned char uc = 0; uc < 3; uc++)
    {
        unsigned int uiNumFloatKeys = pkEulerKey->GetNumKeys(uc);
        if (uiNumFloatKeys > 0)
        {
            NiFloatKey::KeyType eType = pkEulerKey->GetType(uc);
            IsPosedFunction isposed = NiFloatKey::GetIsPosedFunction(eType);
            NIASSERT(isposed);

            NiFloatKey* pkFloatKeys = pkEulerKey->GetKeys(uc);
            if (!isposed(pkFloatKeys, uiNumFloatKeys))
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
NiAnimationKey* NiEulerRotKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiEulerRotKey* pkEulerRotKeys = NiNew NiEulerRotKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkEulerRotKeys[ui].LoadBinary(stream);
    }
    return pkEulerRotKeys;
}
//---------------------------------------------------------------------------
void NiEulerRotKey::LoadBinary(NiStream& stream)
{
    for (unsigned int uiI = 0; uiI < 3; uiI++)
    {
        NiStreamLoadBinary(stream, m_uiNumKeys[uiI]);

        if (m_uiNumKeys[uiI] > 0)
        {
            // load type of rotation
            NiFloatKey::KeyType eType;
            NiStreamLoadEnum(stream, eType);

            m_eType[uiI] = eType;

            m_ucSizes[uiI] = NiFloatKey::GetKeySize(eType);
            
            // load rotations
            NiFloatKey::CreateFunction cf =
                NiFloatKey::GetCreateFunction(eType);
            m_apkKeys[uiI] = (NiFloatKey*) cf(stream, m_uiNumKeys[uiI]);
            NIASSERT(m_apkKeys[uiI]);
        }
    }
}
//---------------------------------------------------------------------------
void NiEulerRotKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiEulerRotKey* pkEulerRotKeys = (NiEulerRotKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkEulerRotKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiEulerRotKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiEulerRotKey* pkEuler = (NiEulerRotKey*) pkKey;

    for (unsigned int uiI = 0; uiI < 3; uiI++)
    {
        NiStreamSaveBinary(stream, pkEuler->m_uiNumKeys[uiI]);
        if (pkEuler->m_uiNumKeys[uiI] > 0)
        {
            // save type of rotation
            NiStreamSaveEnum(stream, pkEuler->m_eType[uiI]);

            NiFloatKey::SaveFunction sf =
                NiFloatKey::GetSaveFunction(pkEuler->m_eType[uiI]);
            NIASSERT(sf);
            sf(stream, pkEuler->m_apkKeys[uiI], pkEuler->m_uiNumKeys[uiI]);
        }
    }
}
//---------------------------------------------------------------------------
