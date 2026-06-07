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

#ifndef NIPSMESHEMITTER_H
#define NIPSMESHEMITTER_H

#include "NiPSEmitter.h"
#include <NiDataStreamElement.h>
#include <NiDataStreamPrimitiveLock.h>

class NiMesh;
class NiSkinningMeshModifier;

/**
    Emits particles from one or more NiMesh objects using a variety of
    options.

    NiMesh objects associated with this class are called mesh emitters. A
    random mesh emitter is selected for each particle emission, and the data
    on that mesh is used to compute the initial position and velocity of the
    particle.

    Particles can be emitted from vertices, faces, or edges of a mesh emitter,
    and the normal at the emission point can be used to determine the
    direction of the initial velocity. Skinned meshes are supported, with the
    skinning for the appropriate vertices being performed in software to
    obtain the final emission position.

    @note Certain criteria must be met for any NiMesh object that is to be
        used as a mesh emitter. See the documentation for the AddMeshEmitter
        function for a list of these requirements.
*/
class NIPARTICLE_ENTRY NiPSMeshEmitter : public NiPSEmitter
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSMeshEmitter);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Determines how the data on the mesh emitter will be used to compute
    /// the emission position for a particle.
    enum MeshEmissionType
    {
        /// Emit from a random vertex in the mesh.
        EMIT_FROM_VERTICES = 0,

        /// Emit from the center of a random triangle in the mesh.
        EMIT_FROM_FACE_CENTER,

        /// Emit from the center of a random edge of a random triangle in the
        /// mesh.
        EMIT_FROM_EDGE_CENTER,

        /// Emit from a random position inside a random triangle in the mesh.
        EMIT_FROM_FACE_SURFACE,

        /// Emit from a random position along a random edge of a random
        /// triangle in the mesh.
        EMIT_FROM_EDGE_SURFACE,

        /// An invalid value indicating the maximum number of defined
        /// MeshEmissionType enumerations.
        EMIT_MAX
    };

    /// Determines how the initial velocity direction for a particle is
    /// computed.
    enum InitialVelocityType
    {
        /// Use the vertex/triangle normals to determine the velocity
        /// direction.
        VELOCITY_USE_NORMALS = 0,

        /// Use a random velocity direction.
        VELOCITY_USE_RANDOM,

        /// Use the direction specified in the emit axis as the velocity
        /// direction.
        VELOCITY_USE_DIRECTION,

        /// An invalid value indicating the maximum number of defined
        /// InitialVelocityType enumerations.
        VELOCITY_MAX
    };

    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kName The name of the emitter.
        @param pkMeshEmitter An NiMesh object to use as a mesh emitter. Mesh
            emitters can be added after construction with the AddMeshEmitter
            function.
        @param eMeshEmissionType The method by which the initial particle
            position will be computed.
        @param eInitialVelocityType The method by which the initial particle
            velocity direction will be computed.
        @param kEmitAxis The axis to use when computing the initial particle
            velocity direction. This axis is only used if eInitialVelocityType
            is VELOCITY_USE_DIRECTION.
        @param fSpeed The speed assigned to new particles.
        @param fSpeedVar The speed variation for new particles. The resulting
            speed values assigned to new particles will be evenly
            distributed over the range [fSpeed - fSpeedVar/2,
            fSpeed + fSpeedVar/2].
        @param fDeclination The declination angle in radians from the positive
            z-axis for the velocity vector of newly created particles. The
            expected range of declination is from 0.0, which will set the
            velocity vector to [0,0,1], to NI_PI, which will set the velocity
            vector to [0,0,-1].
        @param fDeclinationVar The declination variation in radians for new
            particles. The resulting declination values assigned to new
            particles will be evenly distributed over the range
            [fDeclination - fDeclinationVar, fDeclination + fDeclinationVar].
        @param fPlanarAngle The planar angle in radians about the z-axis from
            the positive x-axis that will serve as the plane in which the
            declination will occur. The expected range of the planar angle is
            [0, 2*NI_PI).
        @param fPlanarAngleVar The planar angle variation in radians for new
            particles. The resulting planar angle values assigned to new
            particles will be evenly distributed over the range
            [fPlanarAngle - fPlanarAngleVar, fPlanarAngle + fPlanarAngleVar].
        @param kColor The color assigned to new particles.
        @param fRadius The radius assigned to new particles.
        @param fRadiusVar The radius variation for new particles. The resulting
            radius values assigned to new particles will be evenly distributed
            over the range [fRadius - fRadiusVar, fRadius + fRadiusVar]. If
            fRadiusVar is larger than fRadius, it is possible that the radius
            will be set to a negative value. In this case, the particle will
            be flipped, and the bounding volume may be computed incorrectly.
            Thus, it is recommended that fRadiusVar be less than or equal to
            fRadius.
        @param fLifeSpan The life span in seconds assigned to new particles.
        @param fLifeSpanVar The life span variation for new particles. The
            resulting life span values assigned to new particles will be
            evenly distributed over the range [fLifeSpan - fLifeSpanVar/2,
            fLifeSpan + fLifeSpanVar/2].
        @param fRotAngle The rotation angle in radians assigned to new
            particles.
        @param fRotAngleVar The rotation angle variation in radians for new
            particles. The resulting rotation angle values assigned to new
            particles will be evenly distributed over the range
            [fRotAngle - fRotAngleVar, fRotAngle + fRotAngleVar].
        @param fRotSpeed The rotation speed in radians per second assigned to
            new particles.
        @param fRotSpeedVar The rotation speed variation in radians per second
            for new particles. The resulting rotation speed values assigned to
            new particles will be evenly distributed over the range
            [fRotSpeed - fRotSpeedVar, fRotSpeed + fRotSpeedVar].
        @param bRandomRotSpeedSign Whether or not the rotation speed sign
            should be randomly flipped when being assigned to new particles.
        @param kRotAxis The rotation axis assigned to new particles. This
            value is only used if bRandomRotAxis is false.
        @param bRandomRotAxis Whether or not a random rotation axis will be
            assigned to new particles.
    */
    NiPSMeshEmitter(
        const NiFixedString& kName,
        NiMesh* pkMeshEmitter = NULL,
        MeshEmissionType eMeshEmissionType = EMIT_FROM_VERTICES,
        InitialVelocityType eInitialVelocityType = VELOCITY_USE_NORMALS,
        const NiPoint3& kEmitAxis = NiPoint3::UNIT_X,
        float fSpeed = 1.0f,
        float fSpeedVar = 0.0f,
        float fDeclination = 0.0f,
        float fDeclinationVar = 0.0f,
        float fPlanarAngle = 0.0f,
        float fPlanarAngleVar = 0.0f,
        const NiRGBA& kColor = NiRGBA::WHITE,
        float fRadius = 1.0f,
        float fRadiusVar = 0.0f,
        float fLifeSpan = 1.0f,
        float fLifeSpanVar = 0.0f,
        float fRotAngle = 0.0f,
        float fRotAngleVar = 0.0f,
        float fRotSpeed = 0.0f,
        float fRotSpeedVar = 0.0f,
        bool bRandomRotSpeedSign = false,
        const NiPoint3& kRotAxis = NiPoint3::UNIT_X,
        bool bRandomRotAxis = true);
    //@}

    /// @name Mesh Emitter Management Functions
    //@{
    /**
        Adds an NiMesh object that should be used for particle emission.

        An NiMesh object passed to this function must meet certain criteria in
        order to be successfully added:

        - pkMeshEmitter->GetPrimitiveType() must return
          NiPrimitiveType::PRIMITIVE_POINTS, 
          NiPrimitiveType::PRIMITIVE_TRIANGLES, or
          NiPrimitiveType::PRIMITIVE_TRISTRIPS.
        - If the mesh has an NiSkinningMeshModifier attached, it must have the
          following data streams in the following formats (optional data
          streams are indicated with an asterisk but must have the indicated
          format if they are present):
            - POSITION_BP - F_FLOAT32_3 or F_FLOAT16_4 or F_FLOAT16_3
            - NORMAL_BP* - F_FLOAT32_3 or F_FLOAT16_4 or F_FLOAT16_3
            - BLENDWEIGHT - F_FLOAT32_3 or F_FLOAT32_4
            - BLENDINDICES - F_UINT8_4 or F_INT16_4
            - BONE_PALETTE* - F_UNIT16_1
        - If the mesh is not skinned, it must have the following data streams
          in the following formats (optional streams are again marked with
          an asterisk):
            - POSITION - F_FLOAT32_3 or F_FLOAT16_4 or F_FLOAT16_3
            - NORMAL* - F_FLOAT32_3 or F_FLOAT16_4 or F_FLOAT16_3
        - All data streams listed above (with the addition of the INDEX data
          stream) must allow for CPU reading. If the data stream is an
          NiToolDataStream, it will automatically be made readable. Otherwise,
          validation will fail.

        @param pkMeshEmitter The NiMesh object to use as a mesh emitter.
        @return Whether or not the mesh emitter was successfully added.
    */
    inline bool AddMeshEmitter(NiMesh* pkMeshEmitter);

    /**
        Removes the mesh emitter at the specified index.

        Calling this function may have the result of changing the indices
        and/or the order of other mesh emitters.
    */
    inline void RemoveMeshEmitter(NiUInt32 uiIndex);

    /// Removes all mesh emitters.
    inline void RemoveAllMeshEmitters();

    /**
        Returns the mesh emitter at the specified index.

        The index passed in must be less than the value returned by
        GetMeshEmitterCount.
    */
    inline NiMesh* GetMeshEmitter(NiUInt32 uiIndex);

    /// Returns the number of mesh emitters.
    inline NiUInt32 GetMeshEmitterCount();
    //@}

    //@{
    /// Accesses the method by which the initial particle position will be
    /// computed.
    MeshEmissionType GetMeshEmissionType();
    inline void SetMeshEmissionType(MeshEmissionType eMeshEmissionType);
    //@}

    //@{
    /// Accesses the method by which the initial particle velocity direction
    /// will be computed.
    InitialVelocityType GetInitialVelocityType();
    inline void SetInitialVelocityType(
        InitialVelocityType eInitialVelocityType);
    //@}

    //@{
    /// Accesses the direction that will be used for the particle velocity if
    /// the InitialVelocityType is VELOCITY_USE_DIRECTION.
    inline const NiPoint3& GetEmitAxis();
    inline void SetEmitAxis(const NiPoint3& kEmitAxis);
    //@}

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSMeshEmitter();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool ComputeInitialPositionAndVelocity(
        NiPSParticleSystem* pkParticleSystem,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);
    //@}

    /**
        Computes an initial particle position and velocity based on
        a random vertex in the specified mesh emitter.

        @param pkParticleSystem The particle system from which to emit the
            particle.
        @param pkMeshEmitter The mesh whose data streams will be used to
            compute the particle position and velocity.
        @param kPosition The computed initial particle position.
        @param kVelocity The computed initial particle velocity.
    */
    void EmitFromVertex(
        NiPSParticleSystem* pkParticleSystem,
        NiMesh* pkMeshEmitter,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);

    /**
        Computes an initial particle position and velocity based on
        a random triangle in the specified mesh emitter.

        @param pkParticleSystem The particle system from which to emit the
            particle.
        @param pkMeshEmitter The mesh whose data streams will be used to
            compute the particle position and velocity.
        @param kPosition The computed initial particle position.
        @param kVelocity The computed initial particle velocity.
    */
    void EmitFromFace(
        NiPSParticleSystem* pkParticleSystem,
        NiMesh* pkMeshEmitter,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);

    /**
        Computes an initial particle position and velocity based on
        a random edge of a random triangle in the specified mesh emitter.

        @param pkParticleSystem The particle system from which to emit the
            particle.
        @param pkMeshEmitter The mesh whose data streams will be used to
            compute the particle position and velocity.
        @param kPosition The computed initial particle position.
        @param kVelocity The computed initial particle velocity.
    */
    void EmitFromEdge(
        NiPSParticleSystem* pkParticleSystem,
        NiMesh* pkMeshEmitter,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);

    /**
        Computes an initial particle position and velocity based on
        a random skinned vertex in the specified mesh emitter.

        @param pkParticleSystem The particle system from which to emit the
            particle.
        @param pkMeshEmitter The mesh whose data streams will be used to
            compute the particle position and velocity.
        @param pkSkin The skinning mesh modifier attached to the mesh emitter.
        @param kPosition The computed initial particle position.
        @param kVelocity The computed initial particle velocity.
    */
    void EmitFromSkinnedVertex(
        NiPSParticleSystem* pkParticleSystem,
        NiMesh* pkMeshEmitter,
        NiSkinningMeshModifier* pkSkin,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);

    /**
        Computes an initial particle position and velocity based on
        a random skinned triangle in the specified mesh emitter.

        @param pkParticleSystem The particle system from which to emit the
            particle.
        @param pkMeshEmitter The mesh whose data streams will be used to
            compute the particle position and velocity.
        @param pkSkin The skinning mesh modifier attached to the mesh emitter.
        @param kPosition The computed initial particle position.
        @param kVelocity The computed initial particle velocity.
    */
    void EmitFromSkinnedFace(
        NiPSParticleSystem* pkParticleSystem,
        NiMesh* pkMeshEmitter,
        NiSkinningMeshModifier* pkSkin,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);

    /**
        Computes an initial particle position and velocity based on
        a random edge of a random skinned triangle in the specified mesh
        emitter.

        @param pkParticleSystem The particle system from which to emit the
            particle.
        @param pkMeshEmitter The mesh whose data streams will be used to
            compute the particle position and velocity.
        @param pkSkin The skinning mesh modifier attached to the mesh emitter.
        @param kPosition The computed initial particle position.
        @param kVelocity The computed initial particle velocity.
    */
    void EmitFromSkinnedEdge(
        NiPSParticleSystem* pkParticleSystem,
        NiMesh* pkMeshEmitter,
        NiSkinningMeshModifier* pkSkin,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);

    /**
        A helper function to select a random triangle from the INDEX data
        stream of a mesh and return the three indices.

        This function abstracts away whether the INDEX stream contains 16-bit
        or 32-bit indices.

        @param kIndicesLock A data stream lock object that contains the data
            from the INDEX data stream on a mesh emitter. It is assumed that
            this object has been successfully locked and contains valid data.
        @param uiSubmeshIdx The submesh that should be used.
        @param uiIndex0 A computed index for a random triangle.
        @param uiIndex1 A computed index for a random triangle.
        @param uiIndex2 A computed index for a random triangle.
    */
    void ComputeIndices(
        const NiDataStreamPrimitiveLock& kIndicesLock,
        const NiUInt32 uiSubmeshIdx,
        NiUInt32& uiIndex0,
        NiUInt32& uiIndex1,
        NiUInt32& uiIndex2);

    /**
        A helper function to transform the provided position and velocity from
        the space of the emitter into the space of the particle system.

        This function also uses the InitialVelocityType to compute the final
        velocity value.

        @param pkParticleSystem The particle system from which to emit the
            particle.
        @param pkObject The mesh emitter used to compute the position and
            velocity.
        @param kPosition The initial position of the particle. This function
            will modify this value.
        @param kVelocity The initial velocity of the particle. This function
            will modify this value.
    */
    void TransformIntoCoordinateSpace(
        NiPSParticleSystem* pkParticleSystem,
        NiAVObject* pkObject,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);

    /**
        Validates whether or not the provided NiMesh object can be added as a
        mesh emitter.

        @note See the documentation for the AddMeshEmitter function for a list
            of mesh requirements.
    */
    bool ValidateMesh(NiMesh* pkMesh);

    /**
        Validates whether or not a particular data stream on an NiMesh object
        is valid to be used as a mesh emitter.

        @note See the documentation for the AddMeshEmitter function for a list
            of mesh requirements.
    */
    bool ValidateDataStream(
        NiMesh* pkMesh,
        const NiFixedString& kSemantic,
        NiUInt32 uiSemanticIndex,
        NiDataStreamElement::Format eFormat,
        bool bStreamRequired);

    /// A set of mesh emitters to use when emitting particles.
    NiTPrimitiveSet<NiMesh*> m_kMeshEmitters;

    /// The direction of the initial particle velocity if the
    /// InitialVelocityType is VELOCITY_USE_DIRECTION.
    NiPoint3 m_kEmitAxis;

    /// The method by which the initial particle position will be computed.
    MeshEmissionType m_eMeshEmissionType;

    /// The method by which the initial particle velocity direction will be
    /// computed.
    InitialVelocityType m_eInitialVelocityType;

    /// The maximum allowed number of attempts for avoiding degenerate
    /// triangles.
    static const NiUInt32 MAX_DEGENERATE_TRIANGLES;
};

NiSmartPointer(NiPSMeshEmitter);

#include "NiPSMeshEmitter.inl"

#endif  // #ifndef NIPSMESHEMITTER_H
