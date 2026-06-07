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
#include <NiParticlePCH.h>

#include "NiPSysEmitterCtlrData.h"
#include <NiStream.h>

NiImplementRTTI(NiPSysEmitterCtlrData, NiObject);

//---------------------------------------------------------------------------
NiPSysEmitterCtlrData::NiPSysEmitterCtlrData() :
    m_uiNumBirthRateKeys(0),
    m_pkBirthRateKeys(NULL),
    m_eBirthRateKeyType(NiFloatKey::NOINTERP),
    m_uiNumEmitterActiveKeys(0),
    m_pkEmitterActiveKeys(NULL),
    m_eEmitterActiveKeyType(NiBoolKey::NOINTERP)
{
}
//---------------------------------------------------------------------------
NiPSysEmitterCtlrData::~NiPSysEmitterCtlrData()
{
    if (m_pkBirthRateKeys)
    {
        NiFloatKey::DeleteFunction pfnDeleteFunc =
            NiFloatKey::GetDeleteFunction(m_eBirthRateKeyType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkBirthRateKeys);
    }

    if (m_pkEmitterActiveKeys)
    {
        NiBoolKey::DeleteFunction pfnDeleteFunc =
            NiBoolKey::GetDeleteFunction(m_eEmitterActiveKeyType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkEmitterActiveKeys);
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysEmitterCtlrData);
//---------------------------------------------------------------------------
void NiPSysEmitterCtlrData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    unsigned int uiNumBirthRateKeys;
    NiStreamLoadBinary(kStream, uiNumBirthRateKeys);
    if (uiNumBirthRateKeys > 0)
    {
        NiFloatKey::KeyType eBirthRateKeyType;
        NiStreamLoadEnum(kStream, eBirthRateKeyType);
        unsigned char ucSize = NiFloatKey::GetKeySize(eBirthRateKeyType);
        
        NiFloatKey::CreateFunction pfnCreateFunc =
            NiFloatKey::GetCreateFunction(eBirthRateKeyType);
        NIASSERT(pfnCreateFunc);
        NiFloatKey* pkBirthRateKeys = (NiFloatKey*) pfnCreateFunc(kStream,
            uiNumBirthRateKeys);
        NIASSERT(pkBirthRateKeys);
        NiFloatKey::FillDerivedValsFunction pfnFillDerivedFunc =
            NiFloatKey::GetFillDerivedFunction(eBirthRateKeyType);
        NIASSERT(pfnFillDerivedFunc);
        pfnFillDerivedFunc(pkBirthRateKeys, uiNumBirthRateKeys, ucSize);
        ReplaceBirthRateKeys(pkBirthRateKeys, uiNumBirthRateKeys,
            eBirthRateKeyType);
    }

    unsigned int uiNumEmitterActiveKeys;
    NiStreamLoadBinary(kStream, uiNumEmitterActiveKeys);
    if (uiNumEmitterActiveKeys > 0)
    {
        NiBoolKey::KeyType eEmitterActiveKeyType = NiBoolKey::STEPKEY;
        if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 104))
            NiStreamLoadEnum(kStream, eEmitterActiveKeyType);

        unsigned char ucSize = NiBoolKey::GetKeySize(eEmitterActiveKeyType);
        
        NiBoolKey::CreateFunction pfnCreateFunc =
            NiBoolKey::GetCreateFunction(eEmitterActiveKeyType);
        NIASSERT(pfnCreateFunc);
        NiBoolKey* pkEmitterActiveKeys = (NiBoolKey*) pfnCreateFunc(kStream,
            uiNumEmitterActiveKeys);
        NIASSERT(pkEmitterActiveKeys);
        NiBoolKey::FillDerivedValsFunction pfnFillDerivedFunc =
            NiBoolKey::GetFillDerivedFunction(eEmitterActiveKeyType);
        NIASSERT(pfnFillDerivedFunc);
        pfnFillDerivedFunc(pkEmitterActiveKeys, uiNumEmitterActiveKeys, 
            ucSize);
        ReplaceEmitterActiveKeys(pkEmitterActiveKeys, uiNumEmitterActiveKeys,
            eEmitterActiveKeyType);
    }
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlrData::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterCtlrData::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlrData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiNumBirthRateKeys);
    if (m_uiNumBirthRateKeys > 0)
    {
        NiStreamSaveEnum(kStream, m_eBirthRateKeyType);
        NiFloatKey::SaveFunction pfnSaveFunc = NiFloatKey::GetSaveFunction(
            m_eBirthRateKeyType);
        NIASSERT(pfnSaveFunc);
        pfnSaveFunc(kStream, m_pkBirthRateKeys, m_uiNumBirthRateKeys);
    }

    NiStreamSaveBinary(kStream, m_uiNumEmitterActiveKeys);
    for (unsigned int ui = 0; ui < m_uiNumEmitterActiveKeys; ui++)
    if (m_uiNumEmitterActiveKeys > 0)
    {
        NiStreamSaveEnum(kStream, m_eEmitterActiveKeyType);
        NiBoolKey::SaveFunction pfnSaveFunc = NiBoolKey::GetSaveFunction(
            m_eEmitterActiveKeyType);
        NIASSERT(pfnSaveFunc);
        pfnSaveFunc(kStream, m_pkEmitterActiveKeys, m_uiNumEmitterActiveKeys);
    }
}
//---------------------------------------------------------------------------
bool NiPSysEmitterCtlrData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysEmitterCtlrData* pkDest = (NiPSysEmitterCtlrData*) pkObject;

    if (pkDest->m_uiNumBirthRateKeys != m_uiNumBirthRateKeys ||
        pkDest->m_eBirthRateKeyType != m_eBirthRateKeyType ||
        pkDest->m_uiNumEmitterActiveKeys != m_uiNumEmitterActiveKeys ||
        pkDest->m_ucBirthRateSize != m_ucBirthRateSize)
    {
        return false;
    }

    NiFloatKey::EqualFunction pfnEqualFunc = NiFloatKey::GetEqualFunction(
        m_eBirthRateKeyType);
    NIASSERT(pfnEqualFunc);
    unsigned int ui;
    for (ui = 0; ui < m_uiNumBirthRateKeys; ui++)
    {
        if (!pfnEqualFunc(*pkDest->m_pkBirthRateKeys->GetKeyAt(ui,
            m_ucBirthRateSize),
            *m_pkBirthRateKeys->GetKeyAt(ui, m_ucBirthRateSize)))
        {
            return false;
        }
    }

    for (ui = 0; ui < m_uiNumEmitterActiveKeys; ui++)
    {
        if (!pfnEqualFunc(*pkDest->m_pkEmitterActiveKeys->GetKeyAt(ui,
            m_ucEmitterActiveSize),
            *m_pkEmitterActiveKeys->GetKeyAt(ui, m_ucEmitterActiveSize)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
