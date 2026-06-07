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

#ifndef NIPSYSMODIFIER_H
#define NIPSYSMODIFIER_H

#include <NiParticleLibType.h>
#include <NiObject.h>
#include <NiFixedString.h>

class NiParticleSystem;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSEmitter, NiPSForce, and NiPSCollider.
*/
class NIPARTICLE_ENTRY NiPSysModifier : public NiObject
{
    NiDeclareRTTI;
    NiDeclareAbstractStream;

public:
    // *** begin Emergent internal use only ***
    enum
    {
        ORDER_KILLOLDPARTICLES = 0,
        ORDER_EMITTER          = 1000,
        ORDER_SPAWN            = 2000,
        ORDER_GENERAL          = 3000,
        ORDER_FORCE            = 4000,
        ORDER_COLLIDER         = 5000,
        ORDER_POSUPDATE        = 6000,
        ORDER_POSTPOSUPDATE    = 6500,
        ORDER_BOUNDUPDATE      = 7000
    };
    const NiFixedString& GetName() const;
    unsigned int GetOrder() const;
    void SetSystemPointer(NiParticleSystem* pkTarget);
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysModifier();

    NiFixedString m_kName;
    unsigned int m_uiOrder;
    NiParticleSystem* m_pkTarget;
    bool m_bActive;
};

NiSmartPointer(NiPSysModifier);

/// @endcond

#include "NiPSysModifier.inl"

#endif  // #ifndef NIPSYSMODIFIER_H
