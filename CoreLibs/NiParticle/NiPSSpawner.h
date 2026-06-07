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

#ifndef NIPSSPAWNER_H
#define NIPSSPAWNER_H

#include "NiParticleLibType.h"
#include <NiObject.h>

class NiPSParticleSystem;

/**
    Creates a new particle whose initial parameters are based on an existing
    particle.

    Each NiPSParticleSystem object owns a set of NiPSSpawner objects. It also
    references a "death spawner", which is used to spawn new particles when
    particles in that system are killed. Additionally, each NiPSCollider
    object can optionally reference a spawner from which particles should be
    spawned upon a collision.

    The spawner that is referenced by particle systems and particle colliders
    need not be owned by the same particle system. Thus, it is possible to
    spawn particles from a different particle system instead of from the
    current system. This allows spawned particles to look different from the
    original particles.

    Each time a particle is spawned, its generation value is incremented. The
    generation value is used by NiPSMeshParticleSystem objects to determine
    which master particle to use when emitting the particle. It is also used
    to grow and shrink particles over time.

    The number of particles to spawn can be set in this class, along with a
    randomization value. The speed, direction, and life span for spawned
    particles can also be randomized. All other particle paramters are copied
    from the original particle when a particle is spawned.
    
    The IDs of spawners are globally managed. At most 255 spawners may
    be in the application at any one time. After loading or otherwise
    creating 255 spawners, any attempt to add another spawner will
    assert in debug builds and fail in other configuration. Deleting a
    particle system and its spawners provides space for another spawner.
*/
class NIPARTICLE_ENTRY NiPSSpawner : public NiObject
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSSpawner);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Used to indicate an invalid spawner ID.
    static const NiUInt16 INVALID_ID;

    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param usNumSpawnGenerations The number of generations allowed for
            spawing particles. Particles whose generation equals or exceeds
            this value will not be spawned.
        @param fPercentageSpawned A ratio determining the likelihood of a
            particular particle being spawned. Must be between 0.0 and 1.0.
        @param uiMinNumToSpawn The minimum number of particles to spawn for
            any given original particle.
        @param uiMaxNumToSpawn The maximum number of particles to spawn for
            any given original particle.
        @param fSpawnSpeedChaos Determines how much the speed of the spawned
            particle will vary from that of the original particle.
        @param fSpawnDirChaos Determines how much the direction of the
            spawned particle will vary from that of the original particle.
        @param fLifeSpan The life span to assign to spawned particles.
        @param fLifeSpanVar Determines the amount by which the life span for
            the spawned particle will vary from that of the original particle.
    */
    NiPSSpawner(
        NiUInt16 usNumSpawnGenerations = 1,
        float fPercentageSpawned = 1.0f,
        NiUInt32 uiMinNumToSpawn = 1,
        NiUInt32 uiMaxNumToSpawn = 1,
        float fSpawnSpeedChaos = 0.0f,
        float fSpawnDirChaos = 0.0f,
        float fLifeSpan = 0.0f,
        float fLifeSpanVar = 0.0f);
    //@}

    /**
        Uses the stored spawning variables to determine how many particles to
        spawn, calling SpawnParticle to spawn each particle appropriately.

        @param fCurrentTime The current simulation time.
        @param fSpawnTime The time at which the spawn occurred. This value
            should be less than fCurrentTime.
        @param uiOldIndex The original particle index.
        @param pkOldPSystem The particle system containing the original
            particle.
    */
    virtual void SpawnParticles(
        float fCurrentTime,
        float fSpawnTime,
        NiUInt32 uiOldIndex,
        NiPSParticleSystem* pkOldPSystem);

    /**
        Spawns a single particle.

        @param fCurrentTime The current simulation time.
        @param fSpawnTime The time at which the spawn occurred. This value
            should be less than fCurrentTime.
        @param uiOldIndex The original particle index.
        @param pkOldPSystem The particle system containing the original
            particle.
    */
    virtual void SpawnParticle(
        float fCurrentTime,
        float fSpawnTime,
        NiUInt32 uiOldIndex,
        NiPSParticleSystem* pkOldPSystem);

    //@{
    /**
        Accesses the number of generations allowed for spawing particles.
        
        Particles whose generation equals or exceeds this value will not be
        spawned.
    */
    inline NiUInt16 GetNumSpawnGenerations() const;
    inline void SetNumSpawnGenerations(NiUInt16 usNumSpawnGenerations);
    //@}

    //@{
    /**
        Accesses a ratio determining the likelihood of a particular particle
        being spawned.
        
        This value must be between 0.0 and 1.0 and is multiplied by a random
        number to determine if a particular particle should get spawned.
    */
    inline float GetPercentageSpawned() const;
    inline void SetPercentageSpawned(float fPercentageSpawned);
    //@}

    //@{
    /**
        Accesses the minimum number of particles to spawn for any given
        original particle.

        This value may be the same as the maximum number of particles to
        spawn, in which case the same number of particles will always be
        spawned. If these values differ, the number of particles actually
        spawned will be randomly selected from this range.

        @note This value must be less than or equal to the maximum number of
            particles to spawn.
    */
    inline NiUInt32 GetMinNumToSpawn() const;
    inline void SetMinNumToSpawn(NiUInt32 uiMinNumToSpawn);
    //@}

    //@{
    /**
        Accesses the maximum number of particles to spawn for any given
        original particle.

        This value may be the same as the minimum number of particles to
        spawn, in which case the same number of particles will always be
        spawned. If these values differ, the number of particles actually
        spawned will be randomly selected from this range.

        @note This value must be greater than or equal to the minimum number
            of particles to spawn.
    */
    inline NiUInt32 GetMaxNumToSpawn() const;
    inline void SetMaxNumToSpawn(NiUInt32 uiMaxNumToSpawn);
    //@}

    //@{
    /**
        Accesses a value determining how much the speed of the spawned
        particle will vary from that of the original particle.

        This value must be between -1.0 and 1.0. Negative values will cause
        the spawned particle's speed to be randomly slowed down instead of
        sped up. Here are some examples of how the spawned speed is calculated
        with various speed chaos values:

        - -1.0: 0.0 <= SpawnedSpeed <= OriginalSpeed
        - -0.5: (OriginalSpeed * 0.5) <= SpawnedSpeed <= OriginalSpeed
        - 0.0: SpawnedSpeed == OriginalSpeed
        - 0.5: OriginalSpeed <= SpawnedSpeed <= (OriginalSpeed * 1.5)
        - 1.0: OriginalSpeed <= SpawnedSpeed <= (OriginalSpeed * 2.0)
    */
    inline float GetSpawnSpeedChaos() const;
    inline void SetSpawnSpeedChaos(float fSpawnSpeedChaos);
    //@}

    //@{
    /**
        Accesses a value determining how much the direction of the spawned
        particle will vary from that of the original particle.

        This value must be between 0.0 and 1.0 and indicates how much the
        direction spawned velocity is allowed to deviate from the original
        velocity. Here are some examples:
        
        - 0.0 indicates no variance.
        - 0.5 allows the spawned velocity to deviate from the original
          velocity by up to 90 degrees.
        - 1.0 causes the spawned velocity to be in any random direction.
    */
    inline float GetSpawnDirChaos() const;
    inline void SetSpawnDirChaos(float fSpawnDirChaos);
    //@}

    //@{
    /// Accesses the life span to assign to spawned particles.
    inline float GetLifeSpan() const;
    inline void SetLifeSpan(float fLifeSpan);
    //@}

    //@{
    /**
        Accesses a value determining the amount by which the life span for
        the spawned particle will vary from that of the original particle.

        The life span assigned to spawned particles will be evenly distributed
        over the range [LifeSpan - LifeSpanVar/2, LifeSpan + LifeSpanVar/2].
    */
    inline float GetLifeSpanVar() const;
    inline void SetLifeSpanVar(float fLifeSpanVar);
    //@}

    /// @cond EMERGENT_INTERNAL

    /// Called by the NiPSParticleSystem object when a spawner is added to it
    /// to provide a pointer to the owning particle system.
    void SetMasterPSystem(NiPSParticleSystem* pkMasterPSystem);

    /// @endcond

protected:
    /// A pointer to the particle system object that owns the spawner.
    NiPSParticleSystem* m_pkMasterPSystem;

    /// A ratio determining the likelihood of a particular particle being
    /// spawned.
    float m_fPercentageSpawned;

    /// Determines how much the speed of the spawned particle will vary from
    /// that of the original particle.
    float m_fSpawnSpeedChaos;

    /// Determines how much the direction of the spawned particle will vary
    /// from that of the original particle.
    float m_fSpawnDirChaos;

    /// The life span to assign to spawned particles.
    float m_fLifeSpan;

    /// Determines the amount by which the life span for the spawned particle
    /// will vary from that of the original particle.
    float m_fLifeSpanVar;

    /// The minimum number of particles to spawn for any given original
    /// particle.
    NiUInt32 m_uiMinNumToSpawn;

    /// The maximum number of particles to spawn for any given original
    /// particle.
    NiUInt32 m_uiMaxNumToSpawn;

    /// The number of generations allowed for spawning particles.
    NiUInt16 m_usNumSpawnGenerations;
};

NiSmartPointer(NiPSSpawner);

#include "NiPSSpawner.inl"

#endif  // #ifndef NIPSSPAWNER_H
