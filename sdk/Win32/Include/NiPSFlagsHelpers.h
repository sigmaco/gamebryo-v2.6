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

#ifndef NIPSFLAGSHELPERS_H
#define NIPSFLAGSHELPERS_H

#include "NiParticleLibType.h"
#include "NiUniversalTypes.h"

/**
    A helper class containing static functions that aid in dealing with
    particle flags.

    This class contains functions that get and set data in the particle
    flags bitfield. This is similar to the way in which NiDeclareFlags flags
    are handled.
*/
class NIPARTICLE_ENTRY NiPSFlagsHelpers
{
public:
    /// Gets the particle generation from the particle flags bitfield.
    static NiUInt16 GetGeneration(NiUInt32 uiFlags);

    /// Sets the particle generation in the particle flags bitfield.
    static void SetGeneration(NiUInt32& uiFlags, NiUInt16 usGeneration);

    /// Gets the spawner ID from the particle flags bitfield.
    static NiUInt16 GetSpawnerID(NiUInt32 uiFlags);

    /// Sets the spawner ID in the particle flags bitfield.
    static void SetSpawnerID(NiUInt32& uiFlags, NiUInt16 ucSpawnerID);

    /// Gets whether or not the particle should die from the particle flags
    /// bitfield.
    static bool GetShouldDie(NiUInt32 uiFlags);

    /// Sets whether or not the particle should die in the particle flags
    /// bitfield.
    static void SetShouldDie(NiUInt32& uiFlags, bool bShouldDie);

    /// Gets whether or not the particle should spawn from the particle
    /// flags bitfield.
    static bool GetShouldSpawn(NiUInt32 uiFlags);

    /// Sets whether or not the particle should spawn in the particle flags
    /// bitfield.
    static void SetShouldSpawn(NiUInt32& uiFlags, bool bShouldSpawn);

private:
    /// Enumerated values used to decode the bitfield.
    enum
    {
        GENERATION_MASK   = 0x00000FFF,
        GENERATION_POS    = 0,
        SPAWNER_ID_MASK   = 0x00FFF000,
        SPAWNER_ID_POS    = 12,
        SHOULD_DIE_MASK   = 0x01000000,
        SHOULD_SPAWN_MASK = 0x02000000
    };

    /// Gets a value from the bitfield.
    static NiUInt32 GetField(
        NiUInt32 uiFlags,
        NiUInt32 uiMask,
        NiUInt32 uiPos);

    /// Sets a value in the bitfield.
    static void SetField(
        NiUInt32& uiFlags,
        NiUInt32 uiVal,
        NiUInt32 uiMask,
        NiUInt32 uiPos);

    /// Gets a bit from the bitfield.
    static bool GetBit(NiUInt32 uiFlags, NiUInt32 uiMask);

    /// Sets a bit in the bitfield.
    static void SetBit(NiUInt32& uiFlags, bool bVal, NiUInt32 uiMask);
};

#include "NiPSFlagsHelpers.inl"

#endif  // #ifndef NIPSFLAGSHELPERS_H
