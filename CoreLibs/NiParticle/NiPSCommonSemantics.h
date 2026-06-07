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

#ifndef NIPSCOMMONSEMANTICS_H
#define NIPSCOMMONSEMANTICS_H

#include "NiParticleLibType.h"
#include <NiFixedString.h>

/**
    Defines common semantics strings for particle system data.

    These strings are used by various particle system classes to identify
    the type of particle data being referenced.
*/
class NIPARTICLE_ENTRY NiPSCommonSemantics
{
public:
    /// The enumerated types used to lookup strings.
    enum CommonSemantics
    {
        /// An invalid semantic value.
        PS_INVALID,

        /// The position of the particle.
        PS_PARTICLEPOSITION,

        /// The velocity of the particle.
        PS_PARTICLEVELOCITY,

        /// The age of the particle.
        PS_PARTICLEAGE,

        /// The life span of the particle.
        PS_PARTICLELIFESPAN,

        /// The last time at which the particle was updated.
        PS_PARTICLELASTUPDATE,

        /// A bit-field containing flags about the particle.
        PS_PARTICLEFLAGS,

        /// The radius of the particle. Size times radius is the final size.
        PS_PARTICLERADIUS,

        /// The size of the particle. Size times radius is the final size.
        PS_PARTICLESIZE,

        /// The axis about which the particle will rotate.
        PS_PARTICLEROTAXIS,

        /// The rotation angle for the particle about the rotation axis.
        PS_PARTICLEROTANGLE,

        /// The speed at which the particle is rotating.
        PS_PARTICLEROTSPEED,

        /// The color of the particle.
        PS_PARTICLECOLOR,

        /// The maximum number of semantics. Not valid for use as a semantic.
        PS_SEMANTICSCOUNT
    };

    /// @name Semantics Accessor Functions
    /// These functions return a string representing the specified semantic.
    //@{
    static const NiFixedString& PARTICLEPOSITION();
    static const NiFixedString& PARTICLEVELOCITY();
    static const NiFixedString& PARTICLEAGE();
    static const NiFixedString& PARTICLELIFESPAN();
    static const NiFixedString& PARTICLELASTUPDATE();
    static const NiFixedString& PARTICLEFLAGS();
    static const NiFixedString& PARTICLERADIUS();
    static const NiFixedString& PARTICLESIZE();
    static const NiFixedString& PARTICLEROTAXIS();
    static const NiFixedString& PARTICLEROTANGLE();
    static const NiFixedString& PARTICLEROTSPEED();
    static const NiFixedString& PARTICLECOLOR();
    //@}
    
    /// @name Enumeration Lookup Functions
    /// These functions translate between an enumeration and a semantic string.
    //@{
    static const NiFixedString& GetSemanticString(CommonSemantics eSemantic);
    static CommonSemantics GetSemanticEnum(const NiFixedString& kSemantic);
    //@}

    /// @cond EMERGENT_INTERNAL

    /// @name Static Data Management
    //@{
    static void _SDMInit();
    static void _SDMShutdown();
    //@}

    /// @endcond

private:
    /// Array mapping NiFixedString objects to CommonSemantics enumerations.
    static NiFixedString ms_akSemantics[PS_SEMANTICSCOUNT];
};

#include "NiPSCommonSemantics.inl"

#endif  // #ifndef NIPSCOMMONSEMANTICS_H
