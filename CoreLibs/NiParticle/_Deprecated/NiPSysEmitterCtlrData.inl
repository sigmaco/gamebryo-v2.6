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

//---------------------------------------------------------------------------
inline unsigned int NiPSysEmitterCtlrData::GetNumBirthRateKeys() const
{
    return m_uiNumBirthRateKeys;
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiPSysEmitterCtlrData::GetBirthRateKeys(
    unsigned int& uiNumKeys, NiFloatKey::KeyType& eType, 
    unsigned char& ucSize) const
{
    uiNumKeys = m_uiNumBirthRateKeys;
    eType = m_eBirthRateKeyType;
    ucSize = m_ucBirthRateSize;
    return m_pkBirthRateKeys;
}
//---------------------------------------------------------------------------
inline void NiPSysEmitterCtlrData::ReplaceBirthRateKeys(NiFloatKey* pkKeys,
    unsigned int uiNumKeys, NiFloatKey::KeyType eType)
{
    if (m_pkBirthRateKeys)
    {
        NiFloatKey::DeleteFunction pfnDeleteFunc =
            NiFloatKey::GetDeleteFunction(m_eBirthRateKeyType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkBirthRateKeys);
    }

    SetBirthRateKeys(pkKeys, uiNumKeys, eType);
}
//---------------------------------------------------------------------------
inline unsigned int NiPSysEmitterCtlrData::GetNumEmitterActiveKeys() const
{
    return m_uiNumEmitterActiveKeys;
}
//---------------------------------------------------------------------------
inline NiBoolKey* NiPSysEmitterCtlrData::GetEmitterActiveKeys(
    unsigned int& uiNumKeys, NiBoolKey::KeyType& eType, unsigned char& ucSize)
{
    uiNumKeys = m_uiNumEmitterActiveKeys;
    eType = m_eEmitterActiveKeyType;
    ucSize = m_ucEmitterActiveSize;
    return m_pkEmitterActiveKeys;
}
//---------------------------------------------------------------------------
inline void NiPSysEmitterCtlrData::ReplaceEmitterActiveKeys(NiBoolKey* pkKeys, 
    unsigned int uiNumKeys, NiBoolKey::KeyType eType)
{
    if (m_pkEmitterActiveKeys)
    {
        NiFloatKey::DeleteFunction pfnDeleteFunc =
            NiFloatKey::GetDeleteFunction(m_eEmitterActiveKeyType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkEmitterActiveKeys);
    }

    SetEmitterActiveKeys(pkKeys, uiNumKeys, eType);
}
//---------------------------------------------------------------------------
inline void NiPSysEmitterCtlrData::SetBirthRateKeys(NiFloatKey* pkKeys,
    unsigned int uiNumKeys, NiFloatKey::KeyType eType)
{
    if (!pkKeys || uiNumKeys == 0)
    {
        m_uiNumBirthRateKeys = 0;
        m_pkBirthRateKeys = NULL;
        m_eBirthRateKeyType = NiFloatKey::NOINTERP;
        m_ucBirthRateSize = 0;
        return;
    }

    m_ucBirthRateSize = NiFloatKey::GetKeySize(eType);
    
    m_uiNumBirthRateKeys = uiNumKeys;
    m_pkBirthRateKeys = pkKeys;
    m_eBirthRateKeyType = eType;
}
//---------------------------------------------------------------------------
inline void NiPSysEmitterCtlrData::SetEmitterActiveKeys(NiBoolKey* pkKeys, 
    unsigned int uiNumKeys, NiBoolKey::KeyType eType)
{
    if (!pkKeys || uiNumKeys == 0)
    {
        m_uiNumEmitterActiveKeys = 0;
        m_pkEmitterActiveKeys = NULL;
        eType = NiAnimationKey::NOINTERP;
        m_ucEmitterActiveSize = 0;
        return;
    }

    m_ucEmitterActiveSize = NiBoolKey::GetKeySize(eType);
    
    m_uiNumEmitterActiveKeys = uiNumKeys;
    m_pkEmitterActiveKeys = pkKeys;
    m_eEmitterActiveKeyType = eType;
}
//---------------------------------------------------------------------------
