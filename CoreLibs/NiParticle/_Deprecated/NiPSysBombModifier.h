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

#ifndef NIPSYSBOMBMODIFIER_H
#define NIPSYSBOMBMODIFIER_H

#include "NiPSysModifier.h"
#include <NiPoint3.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSBombForce.
*/
class NIPARTICLE_ENTRY NiPSysBombModifier : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    enum DecayType
    {
        NONE,
        LINEAR,
        EXPONENTIAL
    };
    enum SymmType
    {
        SPHERICAL,
        CYLINDRICAL,
        PLANAR
    };
    NiAVObject* GetBombObj() const;
    const NiPoint3& GetBombAxis() const;
    float GetDecay() const;
    float GetDeltaV() const;
    DecayType GetDecayType() const;
    SymmType GetSymmType() const;
    // *** end Emergent internal use only ***
    
protected:
    // For streaming only.
    NiPSysBombModifier();

    NiAVObject* m_pkBombObj;
    NiPoint3 m_kBombAxis;
    float m_fDecay;
    float m_fDeltaV;
    DecayType m_eDecayType;
    SymmType m_eSymmType;
};

NiSmartPointer(NiPSysBombModifier);

/// @endcond

#include "NiPSysBombModifier.inl"

#endif  // #ifndef NIPSYSBOMBMODIFIER_H
