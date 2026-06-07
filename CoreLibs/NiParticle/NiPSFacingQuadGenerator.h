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

#ifndef NIPSFACINGQUADGENERATOR_H
#define NIPSFACINGQUADGENERATOR_H

#include "NiParticleLibType.h"
#include <NiMeshModifier.h>
#include <NiPSFacingQuadGeneratorKernel.h>
#include <NiSPTask.h>
#include <NiSPStream.h>
#include <NiCamera.h>
#include <NiDataStream.h>

class NiDataStreamRef;

/**
    A mesh modifier that uses particle system data to generate camera-facing
    quads.

    Each quad is actually two triangles. The texture coordinates for each quad
    are fixed and range from 0 to 1. The normals for each vertex are
    perpendicular to the quad face. The quads are rotated in the plane of the
    camera if rotation data exists. Vertex colors are used if color data
    exists.

    @note This mesh modifier must be attached to an NiPSParticleSystem object.
*/
class NIPARTICLE_ENTRY NiPSFacingQuadGenerator : public NiMeshModifier
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSFacingQuadGenerator);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    NiPSFacingQuadGenerator();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool Attach(NiMesh* pkMesh);
    virtual bool Detach(NiMesh* pkMesh);
    virtual bool SubmitTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs,
        NiSPWorkflowManager* pkWFManager);
    virtual bool CompleteTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs);
    virtual bool AreRequirementsMet(
        NiMesh* pkMesh,
        NiSystemDesc::RendererID eRenderer) const;
    virtual void RetrieveRequirements(NiMeshRequirements&
        kRequirements) const;
    //@}

protected:
    /**
        Helper function to add a data stream with the specified parameters
        to the mesh.

        If a data stream with the specified parameters already exists on the
        mesh, this function will assert that all the parameters match and then
        return that data stream instead.
    */
    static NiDataStream* AddDataStream(
        NiMesh* pkMesh,
        const NiFixedString& kSemantic,
        NiUInt32 uiSemanticIndex,
        NiDataStreamElement::Format eFormat,
        NiUInt32 uiCount,
        NiUInt8 uiAccessMask,
        NiDataStream::Usage eUsage,
        NiObject::CloningBehavior eCloningBehavior,
        size_t stElementSize,
        const void* pvData = NULL);

    /// Determine if this stream meets the requirements
    static bool ValidateStream(const NiDataStreamRef* pkStreamRef,
        NiUInt8 uiAccessMask,
        NiDataStream::Usage eUsage,
        NiObject::CloningBehavior eCloningBehavior,
        NiDataStreamElement::Format eFormat);

    /// The Floodgate kernel to use for this mesh modifier.
    NiPSFacingQuadGeneratorKernel m_kKernel;

    /// A structure containing the general input data for the Floodgate kernel.
    NiPSFacingQuadGeneratorKernelStruct m_kKernelStruct;

    /// Floodgate workflow and task.
    NiSPWorkflowPtr m_spWorkflow;
    NiSPTaskPtr m_spTask;

    /// @name Input Floodgate Streams
    //@{
    NiTSPFixedInput<NiPSFacingQuadGeneratorKernelStruct> m_kKernelStructIS;
    NiTSPStream<NiPoint3> m_kPositionIS;
    NiTSPStream<float> m_kRadiusIS;
    NiTSPStream<float> m_kSizeIS;
    NiTSPStream<NiRGBA> m_kColorIS;
    NiTSPStream<float> m_kRotAngleIS;
    //@}

    /// @name Output Floodgate Streams
    //@{
    NiTSPStream<NiPoint3> m_kVertexOS;
    NiTSPStream<NiRGBA> m_kColorOS;
    //@}

    /**
        Indicates whether or not the maximum number of particles requires
        32-bit indices.

        This value is set during the Attach function by comparing the max
        number of particles reported by the particle system to
        MAX_PARTICLES_FOR_16BIT_INDICES.
    */
    bool m_b32BitIndices;

    /**
        Indicates whether or not a color stream is required.

        This value is set during the Attach function by inspecting whether or
        not the particle system has color data.
    */
    bool m_bColors;

    /**
        Indicates whether or not the mesh modifier is currently attached to
        a particle system.

        This value is set to true during the Attach function and to false
        during the Detach function.
    */
    bool m_bAttached;

    /// The maximum number of particles supported by this mesh modifier.
    static const NiUInt32 MAX_SUPPORTED_PARTICLES;

    /// The maximum number of particles for which 16-bit indices can be used.
    static const NiUInt16 MAX_PARTICLES_FOR_16BIT_INDICES;
};

NiSmartPointer(NiPSFacingQuadGenerator);

#endif  // #ifndef NIPSFACINGQUADGENERATOR_H
