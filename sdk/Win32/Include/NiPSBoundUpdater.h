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

#ifndef NIPSBOUNDUPDATER_H
#define NIPSBOUNDUPDATER_H

#include "NiParticleLibType.h"
#include <NiObject.h>

class NiPSParticleSystem;
class NiPSMeshParticleSystem;
class NiBound;

/**
    Updates the bounding volume for an NiPSParticleSystem object.

    This updates the bounding volume for both NiPSParticleSystem objects and
    NiPSMeshParticleSystem objects. For an NiPSParticleSystem object, the
    bound is computed by creating an axis-aligned minimum bounding box around
    the particle positions, taking the radius and size of the particles into
    account. For an NiPSMeshParticleSystem object, the bounding volumes of all
    mesh particles are merged.

    If a particle system does not have one of these objects attached, its
    bounding volume will not be updated. In that case, the model bounding
    volume on the particle system must be manually set to an appropriate
    value. Not having an NiPSBoundUpdater object attached also causes the
    bounding volume of the particle system to be treated differently with
    world-space particles. See the NiPSParticleSystem documentation for more
    details.
    
    The bounding volume computation is optimized by only updating the bound
    of (1 / UpdateSkip) of the total particles every frame. In practice,
    this means that the final bound is accounting for all the particles,
    but the locations of some particles may be up to UpdateSkip frames out
    of date. Reducing UpdateSkip will reduce the number of frames that
    a particle may be out of date, while increasing the number of particles
    processed on each frame. Set the UpdateSkip value to 1 if you would like
    every particle accurately bound on every frame. This may be necessary for
    particle systems with bounds that change rapidly. 
*/
class NIPARTICLE_ENTRY NiPSBoundUpdater : public NiObject
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSBoundUpdater);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    enum
    {
        /**
            Indicates that the UpdateSkip value should be automatically
            computed.
            
            When this UpdateSkip value is specified, the UpdateSkip is
            computed based on the maximum number of particles in the system
            according to the following expression: UpdateSkip =
            (MaxNumParticles / 50) + 1.
            
            The constant is 20 (rather than 50) if the particle system uses
            mesh particles.
        */ 
        AUTO_SKIP_UPDATE = -1
    };

    /// @name Constructor and Destructor
    //@{
    /**
        Main constructor.

        @param sUpdateSkip The initial UpdateSkip value to set.
    */
    NiPSBoundUpdater(NiInt16 sUpdateSkip = AUTO_SKIP_UPDATE);

    /// Virtual destructor.
    virtual ~NiPSBoundUpdater();
    //@}

    /**
        Updates the bounding volume for the specified particle system.

        For an NiPSParticleSystem object, the bound is computed by creating an
        axis-aligned minimum bounding box around the particle positions,
        taking the radius and size of the particles into account. For an
        NiPSMeshParticleSystem object, the bounding volumes of all mesh
        particles are merged.
        
        The bounding volume computation is optimized by only computing the
        bound of (1 / UpdateSkip) of the total particles every frame. This
        object maintains UpdateSkip bounding volumes, each bounding
        NumParticles / UpdateSkip particles. For example, if UpdateSkip is 2
        there will be two bounds, one bounding particles 0, 2, 4, 6 etc,
        and the other bounding particle 1, 3, 5, 7, etc. Each call to
        UpdateBound updates one of these bounds, with successive calls
        updating successive bounds. All of the partial bounds are then merged
        into the final bound. In practice, this means that the final bound
        is accounting for all the particles, but the locations of some
        particles may be up to UpdateSkip frames out of date.

        If, after this computation, the bounding radius is zero and at least
        one particle exists in the system, that radius is set to be the first
        entry in the radii particle data array.

        @param pkParticleSystem The particle system for which the bound should
            be computed.
    */
    virtual void UpdateBound(NiPSParticleSystem* pkParticleSystem);

    /// @name UpdateSkip Functions
    //@{
    /// Gets the UpdateSkip value.
    inline NiInt16 GetUpdateSkip() const;

    /// Sets the UpdateSkip value.
    ///
    /// The initial value is set from the parameter to the constructor.
    void SetUpdateSkip(NiInt16 sUpdateSkip);

    /// Resets the stored UpdateSkip bounding volumes to the identity.
    void ResetUpdateSkipBounds();
    //@}

protected:
    /// The UpdateSkip value.
    NiInt16 m_sUpdateSkip;

    /// Used to determine which subset of particles should be used to compute
    /// the bounding volume.
    NiUInt16 m_usUpdateCount;

    /// An array of bounding volumes used by the UpdateSkip functionality.
    NiBound* m_pkSkipBounds;
};

NiSmartPointer(NiPSBoundUpdater);

#include "NiPSBoundUpdater.inl"

#endif  // #ifndef NIPSBOUNDUPDATER_H
