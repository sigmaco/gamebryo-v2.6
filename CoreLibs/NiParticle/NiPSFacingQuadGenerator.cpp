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

// Precompiled Header
#include "NiParticlePCH.h"

#include "NiPoint2.h"
#include "NiPSFacingQuadGenerator.h"
#include "NiPSParticleSystem.h"
#include <NiCommonSemantics.h>
#include "NiPSCommonSemantics.h"
#include <NiSPWorkflow.h>
#include <NiCullingProcess.h>

NiImplementRTTI(NiPSFacingQuadGenerator, NiMeshModifier);

const NiUInt32 NiPSFacingQuadGenerator::MAX_SUPPORTED_PARTICLES = UINT_MAX / 6;
const NiUInt16 NiPSFacingQuadGenerator::MAX_PARTICLES_FOR_16BIT_INDICES =
    USHRT_MAX / 4;

//---------------------------------------------------------------------------
NiPSFacingQuadGenerator::NiPSFacingQuadGenerator() :
    m_kKernelStructIS(&m_kKernelStruct, 1),
    m_b32BitIndices(false),
    m_bColors(false),
    m_bAttached(false)
{
}
//---------------------------------------------------------------------------
bool NiPSFacingQuadGenerator::Attach(NiMesh* pkMesh)
{
    AddSubmitSyncPoint(NiSyncArgs::SYNC_VISIBLE);
    AddCompleteSyncPoint(NiSyncArgs::SYNC_RENDER);

    // Get pointer to NiPSParticleSystem.
    NIASSERT(NiIsKindOf(NiPSParticleSystem, pkMesh));
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) pkMesh;
    NIASSERT(pkParticleSystem);

    // Get maximum number of particles.
    const NiUInt32 uiMaxNumParticles = pkParticleSystem->GetMaxNumParticles();

    if (uiMaxNumParticles > MAX_SUPPORTED_PARTICLES)
    {
        NILOG("NiPSFacingQuadGenerator Error: MaxNumParticles is %d, which is "
            "too large to support 6 indices per particle. The mesh modifier "
            "cannot be attached.\n", uiMaxNumParticles);
        return false;
    }
    else if (uiMaxNumParticles == 0)
    {
        NILOG("NiPSFacingQuadGenerator Error: MaxNumParticles is 0. The mesh "
            "modifier will not be attached to a particle system that can "
            "contain no particles.\n");
        return false;
    }

    // Set the primitive type for the particle system.
    pkParticleSystem->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    const NiUInt32 uiNumVertices = uiMaxNumParticles * 4;
    const NiUInt32 uiNumIndices = uiMaxNumParticles * 6;

    // Add data stream for vertices.
    NiDataStream* pkDataStream = AddDataStream(
        pkParticleSystem,
        NiCommonSemantics::POSITION(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        uiNumVertices,
        NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_BLANK_COPY,
        sizeof(NiPoint3));
    m_kVertexOS.SetDataSource(pkDataStream, false);
    m_kVertexOS.SetStride(sizeof(NiPoint3) * 4);

    // Add data stream for indices.
    if (uiMaxNumParticles < MAX_PARTICLES_FOR_16BIT_INDICES)
    {
        // If the number of vertices is less than 65536, 16-bit indices can
        // be used.
        NiUInt16* pusIndices = NiAlloc(NiUInt16, uiNumIndices);
        NiUInt16 usStartingVertex = 0;
        for (NiUInt32 ui = 0; ui + 5 < uiNumIndices; ui += 6)
        {
            pusIndices[ui] = usStartingVertex;
            pusIndices[ui + 1] = usStartingVertex + 1;
            pusIndices[ui + 2] = usStartingVertex + 2;
            pusIndices[ui + 3] = usStartingVertex + 2;
            pusIndices[ui + 4] = usStartingVertex + 3;
            pusIndices[ui + 5] = usStartingVertex;
            usStartingVertex += 4;
        }
        AddDataStream(
            pkParticleSystem,
            NiCommonSemantics::INDEX(),
            0,
            NiDataStreamElement::F_UINT16_1,
            uiNumIndices,
            NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX_INDEX,
            NiObject::CLONE_SHARE,
            sizeof(NiUInt16),
            pusIndices);
        NiFree(pusIndices);
    }
    else
    {
        m_b32BitIndices = true;

        NiUInt32* puiIndices = NiAlloc(NiUInt32, uiNumIndices);
        NiUInt32 uiStartingVertex = 0;
        for (NiUInt32 ui = 0; ui + 5 < uiNumIndices; ui += 6)
        {
            puiIndices[ui] = uiStartingVertex;
            puiIndices[ui + 1] = uiStartingVertex + 1;
            puiIndices[ui + 2] = uiStartingVertex + 2;
            puiIndices[ui + 3] = uiStartingVertex + 2;
            puiIndices[ui + 4] = uiStartingVertex + 3;
            puiIndices[ui + 5] = uiStartingVertex;
            uiStartingVertex += 4;
        }
        AddDataStream(
            pkParticleSystem,
            NiCommonSemantics::INDEX(),
            0,
            NiDataStreamElement::F_UINT32_1,
            uiNumIndices,
            NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX_INDEX,
            NiObject::CLONE_SHARE,
            sizeof(NiUInt32),
            puiIndices);
        NiFree(puiIndices);
    }

    // Add data stream for normals.
    NiDataStream* pkNormals = AddDataStream(
        pkParticleSystem,
        NiCommonSemantics::NORMAL(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        1,
        NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_BLANK_COPY,
        sizeof(NiPoint3));
    pkNormals->SetGPUConstantSingleEntry(true);

    // Add data stream for texture coordinates.
#if !defined(_WII)
    NiDataStreamElement::Format eUVFormat = NiDataStreamElement::F_FLOAT32_2;
    NiUInt32 uiUVSize = sizeof(NiPoint2);
    NiPoint2* pkUVs = NiAlloc(NiPoint2, uiNumVertices);
    for (NiUInt32 ui = 0; ui + 3 < uiNumVertices; ui += 4)
    {
        pkUVs[ui] = NiPoint2::ZERO;
        pkUVs[ui + 1] = NiPoint2::UNIT_Y;
        pkUVs[ui + 2] = NiPoint2(1.0f, 1.0f);
        pkUVs[ui + 3] = NiPoint2::UNIT_X;
    }
#else // if !defined(_WII)
    NiDataStreamElement::Format eUVFormat = NiDataStreamElement::F_NORMUINT8_2;
    NiUInt32 uiUVSize = 2 * sizeof(NiUInt8);
    NiUInt8* pkUVs = NiAlloc(NiUInt8, 2 * uiNumVertices);
    for (NiUInt32 ui = 0; ui + 3 < uiNumVertices; ui += 4)
    {
        pkUVs[2 * ui + 0] = 0;
        pkUVs[2 * ui + 1] = 0;
        pkUVs[2 * ui + 2] = 0;
        pkUVs[2 * ui + 3] = 255;
        pkUVs[2 * ui + 4] = 255;
        pkUVs[2 * ui + 5] = 255;
        pkUVs[2 * ui + 6] = 255;
        pkUVs[2 * ui + 7] = 0;
    }
#endif // if !defined(_WII)
    AddDataStream(
        pkParticleSystem,
        NiCommonSemantics::TEXCOORD(),
        0,
        eUVFormat,
        uiNumVertices,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_SHARE,
        uiUVSize,
        pkUVs);
    NiFree(pkUVs);

    if (pkParticleSystem->HasColors())
    {
        m_bColors = true;

        // Add data stream for colors.
        pkDataStream = AddDataStream(
            pkParticleSystem,
            NiCommonSemantics::COLOR(),
            0,
            NiDataStreamElement::F_NORMUINT8_4,
            uiNumVertices,
            NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
            NiDataStream::USAGE_VERTEX,
            NiObject::CLONE_BLANK_COPY,
            sizeof(NiRGBA));
        m_kColorOS.SetDataSource(pkDataStream, false);
        m_kColorOS.SetStride(sizeof(NiRGBA) * 4);
    }

    // Set the number of submeshes on the particle system.
    pkParticleSystem->SetSubmeshCount(1);

    // Create Floodgate task
    m_spTask = NiSPTask::GetNewTask(6, 2);
    m_spTask->SetKernel(&m_kKernel);
    m_spTask->AddInput(&m_kKernelStructIS);
    m_spTask->AddInput(&m_kPositionIS);
    m_spTask->AddInput(&m_kRadiusIS);
    m_spTask->AddInput(&m_kSizeIS);
    m_spTask->AddInput(&m_kColorIS);
    m_spTask->AddInput(&m_kRotAngleIS);
    m_spTask->AddOutput(&m_kVertexOS);
    m_spTask->AddOutput(&m_kColorOS);

#ifdef _XENON
    // Set optimal block count for task.
    m_spTask->SetOptimalBlockCount((NiUInt32) ceil(1536.0f /
        m_kColorIS.GetStride()));
#endif  // #ifdef _XENON

    // Associate particle data with Floodgate streams.
    m_kPositionIS.SetData(pkParticleSystem->GetPositions());
    m_kRadiusIS.SetData(pkParticleSystem->GetRadii());
    m_kSizeIS.SetData(pkParticleSystem->GetSizes());
    m_kColorIS.SetData(pkParticleSystem->GetColors());
    m_kRotAngleIS.SetData(pkParticleSystem->GetRotationAngles());

    m_bAttached = true;

    return true;
}
//---------------------------------------------------------------------------
bool NiPSFacingQuadGenerator::Detach(NiMesh*)
{
    m_spTask = 0;

    m_b32BitIndices = false;
    m_bColors = false;
    m_bAttached = false;
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPSFacingQuadGenerator::SubmitTasks(NiMesh* pkMesh,
    NiSyncArgs* pkArgs, NiSPWorkflowManager* pkWFManager)
{
    NIASSERT(pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_ANY ||
        pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_VISIBLE);
    
    // If a previous workflow is in flight from another culling operation, 
    // finish that workflow before restarting.
    if (m_spWorkflow != NULL)
    {
        // pkArgs is ignored in this implementation of CompleteTasks making
        // it safe to just pass NULL.
        CompleteTasks(pkMesh, NULL);
    }

    // Get pointer to NiPSParticleSystem.
    NIASSERT(NiIsKindOf(NiPSParticleSystem, pkMesh));
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) pkMesh;
    NIASSERT(pkParticleSystem);

    // Get number of active particles.
    const NiUInt32 uiNumParticles = pkParticleSystem->GetNumParticles();

    // Do not add task if there are no particles.
    if (uiNumParticles == 0)
    {
        return false;
    }
    NIASSERT(uiNumParticles < MAX_SUPPORTED_PARTICLES);

    const NiUInt32 uiVertexCount = uiNumParticles * 4;
    const NiUInt32 uiIndexCount = uiNumParticles * 6;

    // Update block count for Floodgate streams to the number of active
    // particles.
    m_kPositionIS.SetBlockCount(uiNumParticles);
    m_kRadiusIS.SetBlockCount(uiNumParticles);
    m_kSizeIS.SetBlockCount(uiNumParticles);
    m_kColorIS.SetBlockCount(uiNumParticles);
    m_kRotAngleIS.SetBlockCount(uiNumParticles);

    // Set region range for vertices.
    NiDataStreamRef* pkRef = NULL;
    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::POSITION());
    NIASSERT(pkRef);
    pkRef->SetActiveCount(0, uiVertexCount);
    m_kVertexOS.SetBlockCount(uiNumParticles);

    // Set region range for indices.
    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
    NIASSERT(pkRef);
    pkRef->SetActiveCount(0, uiIndexCount);

    // Set region range for texture coordinates.
    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::TEXCOORD());
    NIASSERT(pkRef);
    pkRef->SetActiveCount(0, uiVertexCount);

    // Set region range for colors.
    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::COLOR());
    if (pkRef)
    {
        pkRef->SetActiveCount(0, uiVertexCount);
        m_kColorOS.SetBlockCount(uiNumParticles);
    }

    // Tell the particle system to update its cached primitive count. This is
    // necessary because the primitive count is changing each frame.
    pkMesh->UpdateCachedPrimitiveCount();

    // Get current camera.
    NIASSERT(pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_VISIBLE);
    NiCullingSyncArgs* pkCullingArgs = (NiCullingSyncArgs*) pkArgs;
    const NiCamera* pkCamera = pkCullingArgs->m_kCullingProcess.GetCamera();
    NIASSERT(pkCamera);

    // Compute model-space camera parameters.
    const NiMatrix3& kWorldRotate = pkParticleSystem->GetWorldRotate();
    NiPoint3 kModelCamUp = pkCamera->GetWorldUpVector() * kWorldRotate;
    NiPoint3 kModelCamRight = pkCamera->GetWorldRightVector() * kWorldRotate;

    // Compute and set normal vector for each quad.
    m_kKernelStruct.m_kNormal = kModelCamRight.Cross(kModelCamUp);

    // Set camera parameters.
    m_kKernelStruct.m_kCamUp = kModelCamUp;
    m_kKernelStruct.m_kCamRight = kModelCamRight;
    
    NiUInt32 uiTaskGroup = NiSyncArgs::GetTaskGroupID(
        NiSyncArgs::SYNC_VISIBLE, NiSyncArgs::SYNC_RENDER);
    m_spWorkflow = pkWFManager->AddRelatedTask(m_spTask, uiTaskGroup, false);

    // Set the normal explicitly, since there's only one piece of data and
    // there's no need to send it through NiFloodgate.
    {
        NiDataStreamRef* pkNormRef = 
            pkMesh->FindStreamRef(NiCommonSemantics::NORMAL());
        NIASSERT(pkNormRef);

        NiDataStream* pkNorm = pkNormRef->GetDataStream();
        NIASSERT(pkNorm);

        NiPoint3* pkData = (NiPoint3*)pkNorm->Lock(NiDataStream::LOCK_WRITE);
        NIASSERT(pkData);

        pkData[0] = m_kKernelStruct.m_kNormal;

        pkNorm->Unlock(NiDataStream::LOCK_WRITE);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPSFacingQuadGenerator::CompleteTasks(NiMesh*, NiSyncArgs*)
{
    if (m_spWorkflow)
    {
        NiStreamProcessor::Get()->Wait(m_spWorkflow);
        m_spWorkflow = 0;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPSFacingQuadGenerator::AreRequirementsMet(
    NiMesh* pkMesh, NiSystemDesc::RendererID) const
{
    if (!NiIsKindOf(NiPSParticleSystem, pkMesh))
    {
        return false;
    }
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) pkMesh;

    const bool b16BitIndices = (pkParticleSystem->GetMaxNumParticles() <
        MAX_PARTICLES_FOR_16BIT_INDICES);

    const NiDataStreamRef* pkRef = pkMesh->FindStreamRef(
        NiCommonSemantics::INDEX());
    if (pkRef && !ValidateStream(pkRef, 
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX,
        NiObject::CLONE_SHARE,
        b16BitIndices ? NiDataStreamElement::F_UINT16_1 :
            NiDataStreamElement::F_UINT32_1))
    {
        NILOG("NiPSFacingQuadGenerator> Indices are wrong format.\n");
        return false;
    }

    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::POSITION());
    if (pkRef && !ValidateStream(pkRef, 
        NiDataStream::ACCESS_GPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_BLANK_COPY,
        NiDataStreamElement::F_FLOAT32_3))
    {
        NILOG("NiPSFacingQuadGenerator> Positions are wrong format.\n");
        return false;
    }

    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::NORMAL());
    if (pkRef && !ValidateStream(pkRef, 
        NiDataStream::ACCESS_GPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_BLANK_COPY,
        NiDataStreamElement::F_FLOAT32_3))
    {
        NILOG("NiPSFacingQuadGenerator> Normals are wrong format.\n");
        return false;
    }

    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::TEXCOORD());
    if (pkRef && !ValidateStream(pkRef, 
        NiDataStream::ACCESS_GPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_SHARE,
#if !defined(_WII)
        NiDataStreamElement::F_FLOAT32_2))
#else
        NiDataStreamElement::F_NORMUINT8_2))
#endif
    {
        NILOG("NiPSFacingQuadGenerator> TexCoords are wrong format.\n");
        return false;
    }

    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::COLOR());
    if (pkRef && !ValidateStream(pkRef, 
        NiDataStream::ACCESS_GPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_BLANK_COPY,
        NiDataStreamElement::F_NORMUINT8_4))
    {
        NILOG("NiPSFacingQuadGenerator> Colors are wrong format.\n");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPSFacingQuadGenerator::ValidateStream(const NiDataStreamRef* pkRef,
    NiUInt8 uiAccessMask, NiDataStream::Usage eUsage, 
    NiObject::CloningBehavior eCloningBehavior, 
    NiDataStreamElement::Format eFormat)
{
    NIASSERT(pkRef);
    return pkRef->GetUsage() == eUsage &&
        pkRef->GetAccessMask() & uiAccessMask &&
        pkRef->GetElementDescCount() == 1 &&
        pkRef->GetElementDescAt(0).GetFormat() == eFormat &&
        pkRef->GetDataStream() != NULL && 
        pkRef->GetDataStream()->GetCloningBehavior() == eCloningBehavior;
}
//---------------------------------------------------------------------------
void NiPSFacingQuadGenerator::RetrieveRequirements(NiMeshRequirements&
    kRequirements) const
{
    for (NiUInt32 ui = 0; ui < 2; ui++)
    {
        NiUInt32 uiSet = kRequirements.CreateNewRequirementSet();

        // Add INDEX requirement.
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::INDEX(), 0, 
            NiMeshRequirements::STRICT_INTERLEAVE, 0, NiDataStream::
            USAGE_VERTEX_INDEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC, m_b32BitIndices ?
            NiDataStreamElement::F_UINT32_1 : NiDataStreamElement::F_UINT16_1);

        // Add POSITION requirement.
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::POSITION(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, 1, NiDataStream::
            USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE, NiDataStreamElement::
            F_FLOAT32_3);

        // Add NORMAL requirement.
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::NORMAL(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, 2, NiDataStream::
            USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE, NiDataStreamElement::
            F_FLOAT32_3);

        // Add TEXCOORD requirement.
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::TEXCOORD(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, 3, NiDataStream::
            USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC, 
            NiDataStreamElement::F_FLOAT32_2);

        // Add COLOR requirement.
        if (ui == 0)
        {
            kRequirements.AddRequirement(uiSet, NiCommonSemantics::COLOR(), 0,
                NiMeshRequirements::STRICT_INTERLEAVE, 4, NiDataStream::
                USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
                NiDataStreamElement::F_NORMUINT8_4);
        }
    }
}
//---------------------------------------------------------------------------
NiDataStream* NiPSFacingQuadGenerator::AddDataStream(
    NiMesh* pkMesh,
    const NiFixedString& kSemantic,
    NiUInt32 uiSemanticIndex,
    NiDataStreamElement::Format eFormat,
    NiUInt32 uiCount,
    NiUInt8 uiAccessMask,
    NiDataStream::Usage eUsage,
    NiObject::CloningBehavior eCloningBehavior,
    size_t stElementSize,
    const void* pvData)
{
    NI_UNUSED_ARG(stElementSize);
    NIASSERT(pkMesh);

    NiDataStream* pkDataStream = NULL;

    NiDataStreamRef* pkStreamRef = pkMesh->FindStreamRef(kSemantic,
        uiSemanticIndex, eFormat);
    if (pkStreamRef)
    {
        pkDataStream = pkStreamRef->GetDataStream();
        NIASSERT(pkDataStream);

        // Assert that the stream found matches the one being asked for.
        NIASSERT(pkDataStream->GetAccessMask() & uiAccessMask);
        NIASSERT(pkDataStream->GetUsage() == eUsage);
        NIASSERT(pkDataStream->GetSize() == uiCount * stElementSize);
        NIASSERT(pkDataStream->GetCloningBehavior() == eCloningBehavior);
    }
    else
    {
        // Create data stream.
        pkDataStream = NiDataStream::CreateSingleElementDataStream(eFormat,
            uiCount, uiAccessMask, eUsage, pvData);
        NIASSERT(pkDataStream);
        pkDataStream->SetCloningBehavior(eCloningBehavior);

        // Create stream ref.
        pkStreamRef = pkMesh->AddStreamRef(pkDataStream, kSemantic,
            uiSemanticIndex);
        NIASSERT(pkStreamRef);
        pkStreamRef->BindRegionToSubmesh(0, 0);
    }

    NIASSERT(pkDataStream);

    pkDataStream->SetStreamable(false);

    return pkDataStream;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSFacingQuadGenerator);
//---------------------------------------------------------------------------
void NiPSFacingQuadGenerator::CopyMembers(
    NiPSFacingQuadGenerator* pkDest,
    NiCloningProcess& kCloning)
{
    NiMeshModifier::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSFacingQuadGenerator);
//---------------------------------------------------------------------------
void NiPSFacingQuadGenerator::LoadBinary(NiStream& kStream)
{
    NiMeshModifier::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSFacingQuadGenerator::LinkObject(NiStream& kStream)
{
    NiMeshModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSFacingQuadGenerator::RegisterStreamables(NiStream& kStream)
{
    return NiMeshModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSFacingQuadGenerator::SaveBinary(NiStream& kStream)
{
    NiMeshModifier::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSFacingQuadGenerator::IsEqual(NiObject* pkObject)
{
    return NiMeshModifier::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSFacingQuadGenerator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiMeshModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSFacingQuadGenerator::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
