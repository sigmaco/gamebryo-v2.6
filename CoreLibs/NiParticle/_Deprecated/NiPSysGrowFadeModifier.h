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

#ifndef NIPSYSGROWFADEMODIFIER_H
#define NIPSYSGROWFADEMODIFIER_H

#include "NiPSysModifier.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSSimulatorGeneralStep.
*/
class NIPARTICLE_ENTRY NiPSysGrowFadeModifier : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    float GetGrowTime() const;
    unsigned short GetGrowGeneration() const;
    float GetFadeTime() const;
    unsigned short GetFadeGeneration() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysGrowFadeModifier();

    float m_fGrowTime;
    unsigned short m_usGrowGeneration;
    float m_fFadeTime;
    unsigned short m_usFadeGeneration;
};

NiSmartPointer(NiPSysGrowFadeModifier);

/// @endcond

#include "NiPSysGrowFadeModifier.inl"

#endif  // #ifndef NIPSYSGROWFADEMODIFIER_H
