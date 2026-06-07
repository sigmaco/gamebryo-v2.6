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

#ifndef NIPHYSXFLUIDCOMMONSEMANTICS_H
#define NIPHYSXFLUIDCOMMONSEMANTICS_H

#include "NiPhysXFluidLibType.h"
#include <NiFixedString.h>

/**
    Defines common semantics strings for fluid particle system data.

    These strings are used by various classes in the NiPhysXFluid library to
    identify the type of particle data found in a stream.
*/
class NIPHYSXFLUID_ENTRY NiPhysXFluidCommonSemantics
{
public:
    /// The enumerated types used to lookup strings.
    enum CommonSemantics
    {
        /// An invalid semantic value.
        PHYSXFLUID_INVALID,

        /// The position of a fluid particle, in PhysX coordinates.
        PHYSXFLUID_POSITION,

        /// The maximum number of semantics. Not valid for use as a semantic.
        PHYSXFLUID_SEMANTICSCOUNT
    };

    /// Return a string representing the fluid position semantic.
    static const NiFixedString& PHYSXFLUIDPOSITION();
    
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
    static NiFixedString ms_akSemantics[PHYSXFLUID_SEMANTICSCOUNT];
};

#include "NiPhysXFluidCommonSemantics.inl"

#endif  // #ifndef NIPHYSXFLUIDCOMMONSEMANTICS_H
