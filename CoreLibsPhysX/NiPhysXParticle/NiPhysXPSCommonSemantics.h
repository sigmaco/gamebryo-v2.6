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

#ifndef NIPHYSXPSCOMMONSEMANTICS_H
#define NIPHYSXPSCOMMONSEMANTICS_H

#include "NiPhysXParticleLibType.h"
#include <NiFixedString.h>

/**
    Define common semantics strings for physics-based particle system data.

    These strings are used by various classes in the NiPhysXParticle library
    to identify the type of particle data in a stream.
*/
class NIPHYSXPARTICLE_ENTRY NiPhysXPSCommonSemantics
{
public:
    /// The enumerated types used to lookup strings.
    enum CommonSemantics
    {
        /// An invalid semantic value.
        PHYSXPS_INVALID,

        /// The pose of the actors after a simulation step.
        PHYSXPS_ACTORPOSE,

        /// The target velocity of the particle actors prior to a simulation
        /// step.
        PHYSXPS_ACTORVELOCITY,

        /// The velocity of the particle actors at the end of a simulation
        /// step.
        PHYSXPS_ACTORLASTVELOCITY,

        /// The maximum number of semantics. Not valid for use as a semantic.
        PHYSXPS_SEMANTICSCOUNT
    };

    /// Return a string representing the actor pose semantic.
    static const NiFixedString& PARTICLEACTORPOSE();

    /// Return a string representing the actor velocity semantic.
    static const NiFixedString& PARTICLEACTORVELOCITY();

    /// Return a string representing the actor prior velocity semantic.
    static const NiFixedString& PARTICLEACTORLASTVELOCITY();
    
    /// @name Enumeration Lookup Functions
    ///
    /// These functions translate between an enumeration and a semantic string.
    /// @{
    static const NiFixedString& GetSemanticString(CommonSemantics eSemantic);
    static CommonSemantics GetSemanticEnum(const NiFixedString& kSemantic);
    /// @}
    
    /// @cond EMERGENT_INTERNAL

    /// @name Static Data Management
    /// @{
    static void _SDMInit();
    static void _SDMShutdown();
    /// @}

    /// @endcond

private:
    static NiFixedString ms_akSemantics[PHYSXPS_SEMANTICSCOUNT];
};

#include "NiPhysXPSCommonSemantics.inl"

#endif  // #ifndef NIPHYSXPSCOMMONSEMANTICS_H
