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

#include <NiSystem.h>
#include <NiVersion.h>
#include <NiStream.h>
#include "NiAnimationKey.h"
#include "NiEulerRotKey.h"

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED = 
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED = 
    GAMEBRYO_MODULE_VERSION_STRING(NiAnimation);
//---------------------------------------------------------------------------

NiAnimationKey::CreateFunction NiAnimationKey::ms_loaders[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::CurvatureFunction NiAnimationKey::ms_curves[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::InterpFunction NiAnimationKey::ms_interps[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::InterpFunction NiAnimationKey::ms_interpd1s[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::InterpFunction NiAnimationKey::ms_interpd2s[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::EqualFunction NiAnimationKey::ms_equals[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::SaveFunction NiAnimationKey::ms_saves[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::CopyFunction NiAnimationKey::ms_copies[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::ArrayFunction NiAnimationKey::ms_arrays[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::DeleteFunction NiAnimationKey::ms_deletes[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::InsertFunction NiAnimationKey::ms_inserts[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::FillDerivedValsFunction NiAnimationKey::ms_fillderived[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::CubicCoefsFunction NiAnimationKey::ms_cubiccoefs[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
NiAnimationKey::IsPosedFunction NiAnimationKey::ms_isposed[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];

unsigned char NiAnimationKey::ms_keysizes[
    NiAnimationKey::NUMKEYCONTENTS * NiAnimationKey::NUMKEYTYPES];
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiAnimationKey* NiAnimationKey::CreateFromStream(NiStream&, unsigned int)
{
    // abstract class, cannot create an object dynamically
    NIASSERT(false);
    return 0;
}
//---------------------------------------------------------------------------
void NiAnimationKey::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream, m_fTime);
}
//---------------------------------------------------------------------------
void NiAnimationKey::SaveToStream(NiStream&, NiAnimationKey*,
    unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiAnimationKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiStreamSaveBinary(stream, pkKey->m_fTime);
}
//---------------------------------------------------------------------------
void NiAnimationKey::Copy(NiAnimationKey*, const NiAnimationKey*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
NiAnimationKey* NiAnimationKey::CreateArray(unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
    return 0;
}
//---------------------------------------------------------------------------
void NiAnimationKey::DeleteArray(NiAnimationKey*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiAnimationKey::Interpolate(float, const NiAnimationKey*, 
    const NiAnimationKey*, void*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
bool NiAnimationKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    return key0.m_fTime == key1.m_fTime;
}
//---------------------------------------------------------------------------
void NiAnimationKey::FillDerivedVals(NiAnimationKey*, unsigned int,
    unsigned char)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiAnimationKey::CubicCoefs(const NiAnimationKey*, 
    unsigned int, unsigned int, void*, 
    void*, void*, void*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
bool NiAnimationKey::IsPosed(const NiAnimationKey*, 
    unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
    return false;
}
//---------------------------------------------------------------------------
bool NiAnimationKey::OkayToInsert(float fTime, const NiAnimationKey* pkKeys,
    unsigned int uiNumKeys, unsigned int& uiInsertAt, unsigned char ucSize)
{
    // Check to see if there's already a key at the time.
    // This is used as a helper by the other key Insert routines,
    // except NiEulerRotKey::Insert.

    // returns true if we should insert, false if there is already a key
    // at that time.

    // uiInsertAt is set to the proper insertion index.

    bool bOkay = true;
    for (uiInsertAt = 0; uiInsertAt < uiNumKeys; uiInsertAt++)
    {
        const NiAnimationKey* pkKey = pkKeys->GetKeyAt(uiInsertAt,
            ucSize);
        if (pkKey->GetTime() < fTime)
            continue;
        if (pkKey->GetTime() > fTime)
            break;

        NIASSERT(pkKey->GetTime() == fTime);
        bOkay = false;
        break;
    }

    return bOkay;
}
//---------------------------------------------------------------------------
void NiAnimationKey::CopySequence(KeyContent eContent, KeyType eType, 
    const NiAnimationKey* pkOrigKeys, unsigned int uiOrigNumKeys, 
    float fStartTime, float fEndTime, NiAnimationKey*& pkNewKeys,
    unsigned int& uiNewNumKeys)
{
    // CopySequence copies the keys in the sequence range from pkOrigKeys
    // to pkNewKeys.

    // Euler rotation keys are special case, the rest are all the same
    if (eType == EULERKEY)
    {
        NiEulerRotKey* pkEulerKey = (NiEulerRotKey*) pkOrigKeys->GetKeyAt(0,
            sizeof(NiEulerRotKey));

        NiFloatKey* apkNewFloatKeys[3];
        unsigned int auiNewNumFloatKeys[3];
        NiAnimationKey::KeyType aeFloatTypes[3];

        // Handle each of 3 sets of float keys
        for (unsigned int i = 0; i < 3; i++)
        {
            NiFloatKey* pkFloatKeys = pkEulerKey->GetKeys((unsigned char)i);
            unsigned int uiNumFloatKeys = 
                pkEulerKey->GetNumKeys((unsigned char)i);
            aeFloatTypes[i] = pkEulerKey->GetType((unsigned char)i);
            
            // CopySequence accepts NiAnimationKey pointer; must use temp var
            // to avoid unsafe pointer-to-reference cast
            NiAnimationKey* pkNewKey = (NiAnimationKey*) apkNewFloatKeys[i];

            // Get the keys for this sequence
            CopySequence(FLOATKEY, aeFloatTypes[i], pkFloatKeys,
                uiNumFloatKeys, fStartTime, fEndTime, pkNewKey, 
                auiNewNumFloatKeys[i]);

            // Set apkNewFloatKeys[i] after CopySequence has set temp var
            apkNewFloatKeys[i] = (NiFloatKey*)pkNewKey;
        }

        // Create the new Euler key with the copied keys
        pkNewKeys = NiNew NiEulerRotKey[1];
        uiNewNumKeys = 1;
        NiEulerRotKey* pkNewEulerKey = (NiEulerRotKey*)
            pkNewKeys->GetKeyAt(0, sizeof(NiEulerRotKey));
        pkNewEulerKey->Initialize(
            apkNewFloatKeys[0], auiNewNumFloatKeys[0], aeFloatTypes[0],
            apkNewFloatKeys[1], auiNewNumFloatKeys[1], aeFloatTypes[1],
            apkNewFloatKeys[2], auiNewNumFloatKeys[2], aeFloatTypes[2]);

        // It is no longer necessary to set the order here, since all
        // NiEulerRotKeys are assumed to have the same order: XYZ.
//        pkNewEulerKey->SetOrder(pkEulerKey->GetOrder());
    }
    else
    {
        // count keys in range
        uiNewNumKeys = 0;
        unsigned int uiKey;
        unsigned char ucSize = NiAnimationKey::GetKeySize(eContent, eType);
        for (uiKey = 0; uiKey < uiOrigNumKeys; uiKey++)
        {
            const NiAnimationKey* pkKey = pkOrigKeys->GetKeyAt(uiKey,
                ucSize);
            if (pkKey->GetTime() < fStartTime)
                continue;
            if (pkKey->GetTime() > fEndTime)
                break;
            
            uiNewNumKeys++;
        }   

        if (uiNewNumKeys)
        {
            // allocate space
            ArrayFunction pfnArrayFunc = GetArrayFunction(eContent, eType);
            NIASSERT(pfnArrayFunc);
            pkNewKeys = pfnArrayFunc(uiNewNumKeys);
    
            // copy keys in range

            CopyFunction pfnCopy = GetCopyFunction(eContent, eType);
            NIASSERT(pfnCopy);

            for (uiKey = 0, uiNewNumKeys = 0; uiKey < uiOrigNumKeys; uiKey++)
            {
                const NiAnimationKey* pkKey = pkOrigKeys->GetKeyAt(uiKey,
                    ucSize);

                if (pkKey->GetTime() < fStartTime)
                {
                    continue;
                }
                if (pkKey->GetTime() > fEndTime)
                {
                    break;
                }
            
                NiAnimationKey* pkDest = pkNewKeys->GetKeyAt(uiNewNumKeys,
                    ucSize);
                pfnCopy(pkDest, pkKey);
                pkDest->SetTime(pkKey->GetTime() - fStartTime);
                uiNewNumKeys++;
            }   
        }
        else
        {
            pkNewKeys = 0;
        }        
    }
}
//---------------------------------------------------------------------------
void NiAnimationKey::GuaranteeKeyAtStartAndEnd(KeyContent eContent,
    KeyType eType, NiAnimationKey*& pkKeys, unsigned int& uiNumKeys, 
    float fStartTime, float fEndTime)
{
    // This assertion will only be hit if the time range is zero. The function
    // will work correctly in this case, but this assertion will help catch
    // bad use cases.
    NIASSERT(fStartTime != fEndTime);

    NIASSERT(uiNumKeys != 0);

    if (eType == EULERKEY)
    {
        // Euler rot keys always have just one key - examine sub-keys
        NIASSERT(uiNumKeys == 1);
        NiEulerRotKey* pkERKey = (NiEulerRotKey*)pkKeys;
        for (unsigned int i = 0; i < 3; i++)
        {
            NiAnimationKey* pkERKeys = 
                (NiAnimationKey*)pkERKey->GetKeys((unsigned char)i);
            unsigned int uiNumERKeys = 
                pkERKey->GetNumKeys((unsigned char)i);
            KeyType eERType = pkERKey->GetType((unsigned char)i);

            // Arrays within an Euler key can be empty
            if (uiNumERKeys)
            {
                GuaranteeKeyAtStartAndEnd(FLOATKEY, eERType, pkERKeys, 
                    uiNumERKeys, fStartTime, fEndTime);
            }

            pkERKey->SetKeys ((unsigned char)i, (NiFloatKey*)pkERKeys);
            pkERKey->SetNumKeys ((unsigned char)i, uiNumERKeys);
            pkERKey->SetType ((unsigned char)i, eERType);
        }
        return;
    }

    InsertFunction fnInsert = GetInsertFunction(eContent, eType);
    NIASSERT(fnInsert);
    if (uiNumKeys == 1)
    {
        // If only one key at initial time, then return
        if (pkKeys->GetTime() == fStartTime)
            return;
    }

    // Valid animation key array - set initial and final keys if necessary
    fnInsert(fStartTime, pkKeys, uiNumKeys);
    fnInsert(fEndTime, pkKeys, uiNumKeys);
}
//---------------------------------------------------------------------------
