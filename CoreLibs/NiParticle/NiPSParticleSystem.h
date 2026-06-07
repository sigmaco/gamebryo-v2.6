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

#ifndef NIPSPARTICLESYSTEM_H
#define NIPSPARTICLESYSTEM_H

#include "NiParticleLibType.h"
#include <NiMesh.h>
#include "NiPSSimulator.h"
#include "NiPSBoundUpdater.h"
#include "NiPSEmitter.h"
#include "NiPSSpawner.h"
#include <NiTPtrSet.h>

#include <NiCamera.h>

/**
    Represents a particle system.

    This class is derived from NiMesh and overrides its update and rendering
    functions to implement a particle system simulation. It requires an
    NiPSSimulator object to be attached to it in order for the simulation to
    work properly. In most cases, a particle generator mesh modifier, such
    as NiPSFacingQuadGenerator, will also be attached to convert particle
    data into renderable data streams.

    All per-particle data is stored in dynamically allocated arrays owned by
    this object. These arrays are not shared between NiPSParticleSystem
    clones. Several checks are in place to ensure that the particle data is
    not accessed by more than one thread simultaneously. If an application
    needs to access the particle data, it must first call
    ForceSimulationToComplete to ensure that all mesh modifiers that may be
    using the data are forced to complete.

    @note The NiPSParticleSystem constructor is protected and cannot be called
        directly. The static Create function should be used instead.
*/
class NIPARTICLE_ENTRY NiPSParticleSystem : public NiMesh
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSParticleSystem);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Used to indicate failure for the AddParticle function.
    static const NiUInt32 INVALID_PARTICLE;

    /// @name Construction and Destruction
    //@{
    /**
        Creates an NiPSParticleSystem object with the specified parameters.

        This function takes care of creating an NiPSMeshParticleSystem object,
        attaching an NiPSSimulator to it, and optionally creating and
        attaching an NiPSFacingQuadGenerator mesh modifier.

        @param uiMaxNumParticles The maximum number of particles allowed by
            this system. This determines the allocated size of all data arrays
            and cannot be changed.
        @param bHasColors Whether or not color data will be allocated.
        @param bHasRotations Whether or not rotation angle and speed data will
            be allocated.
        @param bWorldSpace Whether or not the particle system will compute its
            particles in world-space or local-space.
        @param bDynamicBounds Whether or not an NiPSBoundUpdater object will
            be attached to the particle system.
        @param bCreateDefaultGenerator Whether or not an
            NiPSFacingQuadGenerator will be created and attached to the
            particle system.
        @param bAttachMeshModifiers Whether or not the simulator and generator
            mesh modifiers will be attached when they are added.
        @return The newly created NiPSParticleSystem object.
    */
    static NiPSParticleSystem* Create(
        NiUInt32 uiMaxNumParticles,
        bool bHasColors = false,
        bool bHasRotations = false,
        bool bWorldSpace = true,
        bool bDynamicBounds = true,
        bool bCreateDefaultGenerator = true,
        bool bAttachMeshModifiers = true);

    /// Virtual destructor.
    virtual ~NiPSParticleSystem();
    //@}

    /// @name Particle System Attribute Accessors
    //@{
    /// Returns the maximum number of particles in the system.
    inline NiUInt32 GetMaxNumParticles() const;

    /// Returns the number of currently active particles in the system.
    inline NiUInt32 GetNumParticles() const;

    /**
        Returns the last time at which the particle system was updated.

        This will be -NI_INFINITY if the particle system has not yet been
        updated.
    */
    inline float GetLastTime() const;

    /// Returns whether or not color data exists for the particle system.
    inline bool HasColors() const;

    /// Returns whether or not rotation angle and speed data exists for the
    /// particle system.
    inline bool HasRotations() const;

    /// Returns whether or not rotation axis data exists for the particle
    /// system.
    inline bool HasRotationAxes() const;

    /**
        Returns whether or not the particle system computes its particles in
        world-space instead of local-space.

        If this function returns true, the world transformation of the
        NiPSParticleSystem object has its translation and rotation set to the
        identity, and the particle positions are stored in world-space.
        Translating or rotating a world-space NiPSParticleSystem object has
        no effect on the particles.

        @note The world scale is still taken into effect with world-space
            particles in order to allow an entire scene to be scaled up or
            down. When the world scale is changed for a world-space particle
            system, an inverse scale is applied to all active particles so
            that they remain in the same world-space location.
    */
    inline bool GetWorldSpace() const;

    /**
        Sets whether or not the particle system computes its particles in
        world-space instead of local-space.

        @note Changing this value at runtime does not modify the positions of
            any particles that already exist. Thus, it may cause the particles
            to display in a different location. It is recommended that this
            function not be used at runtime and instead the proper value be
            provided to the static Create function.
    */
    inline void SetWorldSpace(bool bWorldSpace);
    //@}

    /**
        Returns the simulator mesh modifier attached to the particle system.

        The simulator is responsible for actually simulating the particle
        system, including the application of forces and colliders.
    */    
    inline NiPSSimulator* GetSimulator() const;

    /**
        Returns the bound updater for the particle system.

        If this function returns NULL, the particle system has a static
        bounding volume.
    */
    inline NiPSBoundUpdater* GetBoundUpdater() const;

    /**
        Sets the bound updater for the particle system.

        Setting the bound updater to NULL will result in a particle system
        with a static bounding volume.
    */
    inline void SetBoundUpdater(NiPSBoundUpdater* pkBoundUpdater);

    /// Removes all particles from the particle system.
    virtual void ResetParticleSystem();

    /// @name Emitter Management Functions
    //@{
    /// Returns the number of emitter objects owned by the particle system.
    inline NiUInt32 GetEmitterCount() const;

    /// Returns the emitter object stored at the specified index, which must
    /// be less than the value returned by GetEmitterCount.
    inline NiPSEmitter* GetEmitterAt(NiUInt32 uiIndex) const;

    /// Returns the first emitter object found with the specified name or NULL
    /// if no emitter is found with that name.
    NiPSEmitter* GetEmitterByName(const NiFixedString& kName) const;

    /// Adds the specified emitter object to the array of emitters for the
    /// particle system.
    inline void AddEmitter(NiPSEmitter* pkEmitter);

    /// Removes the emitter object stored at the specified index, which must
    /// be less than the value returned by GetEmitterCount.
    inline void RemoveEmitterAt(NiUInt32 uiIndex, bool bMaintainOrder = false);

    /// Removes all emitter objects from the particle system.
    inline void RemoveAllEmitters();
    //@}

    /**
        These functions manage spawners that spawn new particles into this
        particle system.
    */
    /// @name Spawner Management Functions
    //@{
    /// Returns the number of spawner objects owned by the particle system.
    inline NiUInt32 GetSpawnerCount() const;

    /// Returns the spawner object stored at the specified index, which must
    /// be less than the value returned by GetSpawnerCount.
    inline NiPSSpawner* GetSpawnerAt(NiUInt32 uiIndex) const;

    /** 
        Adds the specified spawner object to the array of spawners for the
        particle system.
    */
    void AddSpawner(NiPSSpawner* pkSpawner);

    /// Removes the spawner object stored at the specified index, which must
    /// be less than the value returned by GetSpawnerCount.
    void RemoveSpawnerAt(NiUInt32 uiIndex, bool bMaintainOrder = false);

    /// Removes all spawner objects from the particle system.
    void RemoveAllSpawners();
    //@}

    /**
        Returns the spawner that should be used when spawning on particle
        death.

        If this function returns false, then particles will not spawn on
        death.

        @note The death spawner may be owned by another particle system and
            thus may not be contained in this particle system's array of
            spawners.
    */
    inline NiPSSpawner* GetDeathSpawner() const;

    /**
        Sets the spawner to use when spawning on particle death.

        To disable spawning on death, the death spawner should be set to NULL.

        @note It is valid to set a death spawner owned by another particle
            system if particles should be spawned from that other system.
    */
    inline void SetDeathSpawner(NiPSSpawner* pkDeathSpawner);

    /**
        Forces the particle simulation to complete.
        
        This function forces all mesh modifiers to complete, ensuring that no
        Floodgate tasks are currently active that might be accessing the
        particle data.

        @note This function must be called by an application prior to
            attempting to access any particle data to ensure that mesh
            modifiers are not attempting to simultaneously use that data.
    */
    inline void ForceSimulationToComplete();

    /**
        These functions provide access to the per-particle data for all active
        particles.
        
        Each data array is of size MaxNumParticles, but only the first part of
        each array, up to NumParticles, contains valid data. The proper way to
        access data in these arrays it to iterate over their entries from 0 to
        GetNumParticles().
        
        @note These functions should only be called by an application after
            ForceSimulationToComplete has been called in order to avoid
            thread concurrency issues.
    */
    /// @name Particle Data Accessors
    //@{
    /// Returns the array of particle positions.
    inline NiPoint3* GetPositions();

    /// Returns the array of particle positions.
    inline const NiPoint3* GetPositions() const;

    /// Returns the array of particle velocities.
    inline NiPoint3* GetVelocities();

    /// Returns the array of particle velocities.
    inline const NiPoint3* GetVelocities() const;

    /// Returns the array of particle ages.
    inline float* GetAges();

    /// Returns the array of particle ages.
    inline const float* GetAges() const;

    /// Returns the array of particle life spans.
    inline float* GetLifeSpans();

    /// Returns the array of particle life spans.
    inline const float* GetLifeSpans() const;

    /// Returns the array of the last time each particle was updated.
    inline float* GetLastUpdateTimes();

    /// Returns the array of the last time each particle was updated.
    inline const float* GetLastUpdateTimes() const;

    /**
        Returns the array of particle flags.

        A particle flag is a 32-bit bitfield that holds various information,
        such as the particle generation and whether or not the particle should
        be killed or spawned during the next update.
       
        @note Particle flags are meant to be used internally only, but the
            data within them can be accessed with the NiPSFlagsHelpers class.
    */
    inline NiUInt32* GetFlags();

    /**
        Returns the array of particle flags.

        A particle flag is a 32-bit bitfield that holds various information,
        such as the particle generation and whether or not the particle should
        be killed or spawned during the next update.
       
        @note Particle flags are meant to be used internally only, but the
            data within them can be accessed with the NiPSFlagsHelpers class.
    */
    inline const NiUInt32* GetFlags() const;

    /**
        Returns the array of particle radii.

        @note The particle radius is multiplied by the particle size to arrive
            at the final size for the particle.
    */
    inline float* GetRadii();

    /**
        Returns the array of particle radii.

        @note The particle radius is multiplied by the particle size to arrive
            at the final size for the particle.
    */
    inline const float* GetRadii() const;

    /**
        Returns the array of particle sizes.

        @note The particle size is multiplied by the particle radius to arrive
            at the final size for the particle.
    */
    inline float* GetSizes();

    /**
        Returns the array of particle sizes.

        @note The particle size is multiplied by the particle radius to arrive
            at the final size for the particle.
    */
    inline const float* GetSizes() const;

    /**
        Returns the array of particle colors.

        This array may be NULL if no colors exist.
    */
    inline NiRGBA* GetColors();

    /**
        Returns the array of particle colors.

        This array may be NULL if no colors exist.
    */
    inline const NiRGBA* GetColors() const;

    /**
        Returns the array of particle rotation angles.

        This array may be NULL if no rotations exist.
    */
    inline float* GetRotationAngles();

    /**
        Returns the array of particle rotation angles.

        This array may be NULL if no rotations exist.
    */
    inline const float* GetRotationAngles() const;

    /**
        Returns the array of particle rotation speeds.

        This array may be NULL if no rotations exist.
    */
    inline float* GetRotationSpeeds();

    /**
        Returns the array of particle rotation speeds.

        This array may be NULL if no rotations exist.
    */
    inline const float* GetRotationSpeeds() const;

    /**
        Returns the array of particle rotation axes.

        This array will be NULL for all sprite-based particle systems. It may
        be NULL for mesh particles, also, if no rotations exist.
    */
    inline NiPoint3* GetRotationAxes();

    /**
        Returns the array of particle rotation axes.

        This array will be NULL for all sprite-based particle systems. It may
        be NULL for mesh particles, also, if no rotations exist.
    */
    inline const NiPoint3* GetRotationAxes() const;
    //@}
    
    /**
        The set of spawners that may spawn particles from this particle
        system is updated at each simulation step from information
        provided by colliders and the particle system itself. These functions
        are used to manage a dynamic mapping between the spawner ID stored in
        particle flags and pointers to the actual spawner object.
    */
    /// @name Spawner ID Functions
    //@{
    /// Returns the number of spawner objects owned by the particle system.
    NiUInt16 GetSpawnerID(NiPSSpawner* pkSpawner);

    /// Returns the number of spawner objects owned by the particle system.
    NiPSSpawner* GetSpawnerFromID(NiUInt16 uiID) const;
    
    /**
        Return the maximum valid ID.
    
        All IDs [0, ..., max - 1] are valid. The value will only be non-zero
        between the time the particle system is updated and the time
        simulation completes. 
    */
    NiUInt16 GetMaxValidSpawnerID() const;
    //@}    

    /// @cond EMERGENT_INTERNAL

    /**
        Locates an empty index in the particle data arrays.

        @return The index of the new particle or INVALID_PARTICLE if the
            maximum number of particles already exist.
    */
    NiUInt32 AddParticle();

    /**
        Resolves all particles that were added to the system during the
        previous frame.
        
        When bUpdatebOund is true this function will attempt to update the
        bounding volume to reflect the new particles' contribution.
        Derived class that implement this function must call the base class
        version with the bUpdateBound argument, and should not themselves
        update the bound.
    */
    virtual void ResolveAddedParticles(const bool bUpdateBound);

    /// Called after a new particle has been emitted or spawned to allow the
    /// system to perform any further processing.
    virtual void InitializeParticle(NiUInt32 uiNewParticle);

    /// Called by the NiPSSimulator object when it completes simulation to
    /// allow the particle system to perform post-processing.
    virtual void CompleteSimulation();

    /// @name Base Class Overrides
    //@{
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);
    virtual void SetSelectiveUpdateFlags(
        bool& bSelectiveUpdate,
        bool bSelectiveUpdateTransforms,
        bool& bRigid);
    virtual void RenderImmediate(NiRenderer* pkRenderer);
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSParticleSystem();

    /// Protected copy constructor to disable copy construction.
    NiPSParticleSystem(const NiPSParticleSystem&);

    /**
        Protected main constructor.

        Instances of this class should be created with the static Create
        function instead of with this constructor.

        @param pkSimulator A pointer to the simulator mesh modifier that will
            be attached to this particle system.
        @param uiMaxNumParticles The maximum number of particles allowed by
            this system. This determines the allocated size of all data
            arrays.
        @param bHasColors Whether or not color data will be allocated.
        @param bHasRotations Whether or not rotation angle and speed data will
            be allocated.
        @param bHasRotationAxes Whether or not rotation axis data will be
            allocated. This value should not be true if bHasRotations is false.
        @param bWorldSpace Whether or not the particle system will compute its
            particles in world-space or local-space.
        @param pkBoundUpdater A pointer to the bound updater to use with this
            particle system. This should be NULL if the particle system is to
            have a static bounding volume.
    */
    NiPSParticleSystem(
        NiPSSimulator* pkSimulator,
        NiUInt32 uiMaxNumParticles,
        bool bHasColors,
        bool bHasRotations,
        bool bHasRotationAxes,
        bool bWorldSpace,
        NiPSBoundUpdater* pkBoundUpdater);
    //@}

    /// @name Base Class Overrides
    //@{
    virtual void UpdateWorldData();
    virtual void UpdateWorldBound();
    //@}

    /// Called during Update prior to invoking mesh modifiers.
    void PreUpdate(NiUpdateProcess& kUpdate);

    /// Called during Update after invoking mesh modifiers (but possibly
    /// before they have completed). The function can be overridden in derived
    /// classes to perform additional post update operations.
    virtual void PostUpdate(NiUpdateProcess& kUpdate);

    /**
        Called by CompleteSimulation to update the particle data to account
        for changing scale values and deleted and spawned particles.

        This function is called before the bounding volume is updated.
    */
    virtual void UpdateParticlesUponCompletion();

    /// Resolves all particles that were marked to be killed or spawned during
    /// the previous simulation frame.
    void ResolveSpawnedAndRemovedParticles(float fTime);

    /**
        Removes the particle at the specified index.

        This function copies the particle data from the last valid index in
        the data arrays to the index of the removed particle to ensure that
        the data arrays remain packed.
    */
    virtual void RemoveParticle(NiUInt32 uiIndexToRemove);

    /**
        Allocates the particle data arrays.

        This function first calls FreeDataBuffers to ensure that any old data
        has been deleted. The old data is not copied to the new buffers by
        this function. All data buffers are allocated aligned to
        NIPSKERNEL_ALIGNMENT.

        @param uiBufferSize The number of elements to allocate in each array.
            This should correspond with the maximum number of particles.
        @param bAllocateColors Whether or not to allocate the color array.
        @param bAllocateRotations Whether or not to allocate the rotation
            angle and speed arrays.
        @param bAllocateRotationAxes Whether or not to allocate the rotation
            axis array.
    */
    void AllocateDataBuffers(
        NiUInt32 uiBufferSize,
        bool bAllocateColors,
        bool bAllocateRotations,
        bool bAllocateRotationAxes);

    /// Frees all particle data arrays, setting the data pointers to NULL.
    void FreeDataBuffers();

    /**
        The simulator used by this system.

        This simulator mesh modifier must also be attached to the
        NiPSParticleSystem object in the usual fashion.
    */
    NiPSSimulator* m_pkSimulator;

    /// The bound updater to use when computing a dynamic bounding volume.
    NiPSBoundUpdaterPtr m_spBoundUpdater;

    /// An array of emitter objects owned by the particle system.
    NiTObjectPtrSet<NiPSEmitterPtr> m_kEmitters;

    /// An array of spawner objects owned by the particle system.
    NiTObjectPtrSet<NiPSSpawnerPtr> m_kSpawners;

    /// The spawner object to use when particles die.
    NiPSSpawner* m_pkDeathSpawner;

    /// The last time at which the particle system was updated.
    float m_fLastTime;

    /// The maximum number of particles allowed to exist in the system.
    NiUInt32 m_uiMaxNumParticles;

    /// The number of currently active particles in the system.
    NiUInt32 m_uiNumParticles;

    /// A base index value above which new particles are added.
    NiUInt32 m_uiAddedParticlesBase;

    /// The number of particles that have been added since the last update.
    NiUInt32 m_uiNumAddedParticles;

    /// A structure used to track which particle objects should be killed or
    /// spawned.
    struct ParticleProcessInfo
    {
        /// Initializes m_uiIndex and m_ucSpawnerID to 0 and initializes
        /// m_bShouldSpawn and m_bShouldDie to false.
        ParticleProcessInfo();

        /// The index of the particle.
        NiUInt32 m_uiIndex;

        /// The ID of the spawner to use when spawning from this particle.
        NiUInt16 m_ucSpawnerID;

        /// Whether or not a new particle should be spawned based on this
        /// particle.
        bool m_bShouldSpawn;

        /// Whether or not this particle should be killed.
        bool m_bShouldDie;
    };

    /**
        An array of particles that need to be either spawned from or killed.

        This array is built during ResolveSpawnedAndRemovedParticles.
    */
    NiTPrimitiveSet<ParticleProcessInfo> m_kParticlesToProcess;

    /// The computed world transform before being modified to accommodate
    /// world-space particles.
    NiTransform m_kUnmodifiedWorld;

    /// @name Particle Data
    //@{
    /// The position of each particle.
    NiPoint3* m_pkPositions;

    /// The velocity of each particle.
    NiPoint3* m_pkVelocities;

    /// The age of each particle.
    float* m_pfAges;

    /// The life span of each particle.
    float* m_pfLifeSpans;

    /// The last time at which each particle was updated.
    float* m_pfLastUpdateTimes;

    /// A 32-big flag bitfield for each particle.
    NiUInt32* m_puiFlags;

    /// The radius of each particle.
    float* m_pfRadii;

    /// The size of each particle.
    float* m_pfSizes;

    /// The color of each particle.
    NiRGBA* m_pkColors;

    /// A rotation angle for each particle.
    float* m_pfRotationAngles;

    /// A rotation speed for each particle.
    float* m_pfRotationSpeeds;

    /// A rotation axis for each particle.
    NiPoint3* m_pkRotationAxes;
    //@}

    /**
        The set of spawners for a given simulation tick.
        
        This array maps spawner IDs (indexes into the array) to spawner
        pointers. The IDs are used by individual particles to record the
        spawner to use. In the Update phase of the particle system, every
        object that may set a spawner for a particle must first register
        the spawner and get an ID from this particle system, the one owning
        the particles. When the simulation tick is done, the array can
        be used to move fromthe ID back to the pointer.
    */
    NiTPrimitiveSet<NiPSSpawner*> m_kActiveSpawners;

    /**
        A record of the previous scale value used to correct the
        scale on world-space particles when the world scale of the particle
        system is changed.
    */
    float m_fPreviousWorldScale;

    /// Whether or not the particles are stored in world-space instead of
    /// local-space.
    bool m_bWorldSpace;

    /**
        Whether or not the simulation still needs to be completed.

        This value is used to catch multiple calls to Update without being
        rendered.
    */
    bool m_bCompleteSimulation;

    /**
        Whether or not the world scale has ever been set.

        This value is used to prevent applying an inverse world scale for
        world-space particles the first time that the scale is set.
    */
    bool m_bScaleSet;
};

NiSmartPointer(NiPSParticleSystem);

#include "NiPSParticleSystem.inl"

#endif  // #ifndef NIPSPARTICLESYSTEM_H
