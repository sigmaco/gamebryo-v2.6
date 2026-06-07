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

#ifndef NIPSYSEMITTERCTLRDATA_H
#define NIPSYSEMITTERCTLRDATA_H

#include <NiParticleLibType.h>
#include <NiObject.h>
#include <NiFloatKey.h>
#include <NiBoolKey.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSEmitParticlesCtlr.
*/
class NIPARTICLE_ENTRY NiPSysEmitterCtlrData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    virtual ~NiPSysEmitterCtlrData();

    // *** begin Emergent internal use only ***
    inline unsigned int GetNumBirthRateKeys() const;
    inline NiFloatKey* GetBirthRateKeys(unsigned int& uiNumKeys,
        NiFloatKey::KeyType& eType, unsigned char& ucSize) const;
    inline void ReplaceBirthRateKeys(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);

    inline unsigned int GetNumEmitterActiveKeys() const;
    inline NiBoolKey* GetEmitterActiveKeys(unsigned int& uiNumKeys,
        NiBoolKey::KeyType& eType, unsigned char& ucSize);
    inline void ReplaceEmitterActiveKeys(NiBoolKey* pkKeys, unsigned int uiNumKeys,
        NiBoolKey::KeyType eType);

    inline void SetBirthRateKeys(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiFloatKey::KeyType eType);
    inline void SetEmitterActiveKeys(NiBoolKey* pkKeys, unsigned int uiNumKeys,
        NiBoolKey::KeyType eType);
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysEmitterCtlrData();

    unsigned int m_uiNumBirthRateKeys;
    NiFloatKey* m_pkBirthRateKeys;
    NiFloatKey::KeyType m_eBirthRateKeyType;
    unsigned char m_ucBirthRateSize;

    unsigned int m_uiNumEmitterActiveKeys;
    NiBoolKey* m_pkEmitterActiveKeys;
    NiBoolKey::KeyType m_eEmitterActiveKeyType;
    unsigned char m_ucEmitterActiveSize;
};

NiSmartPointer(NiPSysEmitterCtlrData);

/// @endcond

#include "NiPSysEmitterCtlrData.inl"

#endif  // #ifndef NIPSYSEMITTERCTLRDATA_H
