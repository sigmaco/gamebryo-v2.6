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

#ifndef NIPSCOLLIDER_H
#define NIPSCOLLIDER_H

#include <NiObject.h>
#include "NiPSColliderDefinitions.h"

class NiPSParticleSystem;
class NiPSSpawner;

/**
    An abstract base class for all particle colliders.
    
    Multiple colliders can be owned by an NiPSSimulatorCollidersStep object,
    which is attached to the NiPSSimulator mesh modifier of a particle system.
    Each collider provides a block of memory that contains the data it needs
    to properly execute. In its Update function, it updates that data as
    necessary. The NiPSSimulatorCollidersStep object will concatenate the data
    from all its colliders into a single data buffer that gets sent to a
    Floodgate kernel.

    In order to apply its collision behavior inside the Floodgate kernel, two
    functions need to be added for each collider to the NiPSColliderHelpers
    class: one to detect a collision and one to handle it. For each particle,
    the NiPSSimulatorCollidersKernel Floodgate kernel loops over all the
    colliders, calling the collision detection function for each and providing
    that collider's data buffer to that function. The closest collider that
    returns a positive result will have its collision handler function called
    for the particle, which takes care of updating the particle's position and
    velocity appropriately.
    
    An NiPSSpawner object may be optionally associated with a collider, which
    will be used if the collider is set to spawn upon collision. There is also
    an option for particles to die upon collision.
*/
class NIPARTICLE_ENTRY NiPSCollider : public NiObject
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSCollider);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Returns the type of this collider.
    inline NiPSColliderDefinitions::ColliderType GetType() const;

    //@{
    /**
        Accesses the amount of bounce that particles will have upon collision.

        The bounce is multiplied by the particle velocity when the collision
        is handled, so a bounce of 1.0 has no effect.
    */
    inline float GetBounce() const;
    inline void SetBounce(float fBounce);
    //@}

    //@{
    /**
        Accesses whether or not new particles should be spawned when a
        particle collides.

        How particles get spawned is controlled by the spawner associated with
        the collider.

        @note A spawner must be set with SetSpawner when this value is true in
            order for any particles to spawn.
    */
    inline bool GetSpawnOnCollide() const;
    inline void SetSpawnOnCollide(bool bSpawnOnCollide);
    //@}

    //@{
    /// Accesses whether or not particles that collide should immediately die.
    inline bool GetDieOnCollide() const;
    inline void SetDieOnCollide(bool bDieOnCollide);
    //@}

    //@{
    /**
        Accesses the spawner to use when spawning upon collision.

        The spawner may belong to a different particle system than the
        collider, if particles are to be spawned from that system.

        @note A spawner should be set whenever SpawnOnCollide is true. If not,
            no particles will be spawned.
    */
    inline NiPSSpawner* GetSpawner() const;
    inline void SetSpawner(NiPSSpawner* pkSpawner);
    //@}

    //@{
    /**
        Accesses whether or not the collider is active.

        Inactive colliders will not be included during kernel execution.
    */
    inline bool GetActive() const;
    inline void SetActive(bool bActive);
    //@}

    /// Returns the size in bytes of the data block provided by the collider.
    virtual size_t GetDataSize() const = 0;

    /// Returns a pointer to the data block provided by the collider.
    virtual void* GetData() const = 0;

    /// Updates the data for the collider.
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /// @name Construction and Destruction
    //@{
    /**
        Protected main constructor.

        @param eType The type of collider.
        @param pkInputData A pointer to a structure containing the data for
            the collider.
        @param fBounce The bounce value for the collider.
        @param bSpawnOnCollide Whether or not new particles should be spawned
            upon collision.
        @param bDieOnCollide Whether or not particles should immediately die
            upon collision.
        @param pkSpawner The spawner object to use for the collider.
    */
    NiPSCollider(
        NiPSColliderDefinitions::ColliderType eType,
        NiPSColliderDefinitions::ColliderData* pkInputData,
        float fBounce,
        bool bSpawnOnCollide,
        bool bDieOnCollide,
        NiPSSpawner* pkSpawner);

    /// Protected default constructor for cloning and streaming only.
    NiPSCollider();
    //@}

    /**
        Copies the data stored in the ColliderData structure.

        This helper function should be called by derived classes during
        cloning to copy the ColliderData structure.
    */
    void CopyColliderData(NiPSCollider* pkDest);

    /**
        Loads the data stored in the ColliderData structure from a stream.

        This helper function should be called by derived classes to serialize
        the ColliderData structure.
    */
    void LoadColliderData(NiStream& kStream);

    /**
        Saves the data stored in the ColliderData structure to a stream.

        This helper function should be called by derived classes to serialize
        the ColliderData structure.
    */
    void SaveColliderData(NiStream& kStream);

    /// A pointer to the data for the collider.
    NiPSColliderDefinitions::ColliderData* m_pkInputData;

private:
    /// The spawner to use for the collider.
    NiPSSpawner* m_pkSpawner;

    /// The type of the collider.
    NiPSColliderDefinitions::ColliderType m_eType;

    /// Whether or not the collider is active.
    bool m_bActive;
};

NiSmartPointer(NiPSCollider);

#include "NiPSCollider.inl"

#endif  // #ifndef NIPSCOLLIDER_H
