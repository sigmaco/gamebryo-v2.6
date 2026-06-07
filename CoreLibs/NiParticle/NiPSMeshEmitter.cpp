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

#include "NiPSMeshEmitter.h"
#include <NiMesh.h>
#include <NiToolDataStream.h>
#include "NiPSParticleSystem.h"
#include <NiSkinningMeshModifier.h>
#include <NiDataStreamElementLock.h>

NiImplementRTTI(NiPSMeshEmitter, NiPSEmitter);

const NiUInt32 NiPSMeshEmitter::MAX_DEGENERATE_TRIANGLES = 6;

//---------------------------------------------------------------------------
NiPSMeshEmitter::NiPSMeshEmitter(
    const NiFixedString& kName,
    NiMesh* pkMeshEmitter,
    MeshEmissionType eMeshEmissionType,
    InitialVelocityType eInitialVelocityType,
    const NiPoint3& kEmitAxis,
    float fSpeed,
    float fSpeedVar,
    float fDeclination,
    float fDeclinationVar,
    float fPlanarAngle,
    float fPlanarAngleVar,
    const NiRGBA& kColor,
    float fRadius,
    float fRadiusVar,
    float fLifeSpan,
    float fLifeSpanVar,
    float fRotAngle,
    float fRotAngleVar,
    float fRotSpeed,
    float fRotSpeedVar,
    bool bRandomRotSpeedSign,
    const NiPoint3& kRotAxis,
    bool bRandomRotAxis) :
    NiPSEmitter(
        kName,
        fSpeed,
        fSpeedVar,
        fDeclination,
        fDeclinationVar,
        fPlanarAngle,
        fPlanarAngleVar,
        kColor,
        fRadius,
        fRadiusVar,
        fLifeSpan,
        fLifeSpanVar,
        fRotAngle,
        fRotAngleVar,
        fRotSpeed,
        fRotSpeedVar,
        bRandomRotSpeedSign,
        kRotAxis,
        bRandomRotAxis),
    m_kMeshEmitters(2),
    m_kEmitAxis(kEmitAxis),
    m_eMeshEmissionType(eMeshEmissionType),
    m_eInitialVelocityType(eInitialVelocityType)
{
    if (pkMeshEmitter)
    {
        AddMeshEmitter(pkMeshEmitter);
    }
}
//---------------------------------------------------------------------------
NiPSMeshEmitter::NiPSMeshEmitter() :
    m_kMeshEmitters(2),
    m_kEmitAxis(NiPoint3::UNIT_X),
    m_eMeshEmissionType(EMIT_FROM_VERTICES),
    m_eInitialVelocityType(VELOCITY_USE_NORMALS)
{
}
//---------------------------------------------------------------------------
bool NiPSMeshEmitter::ComputeInitialPositionAndVelocity(
    NiPSParticleSystem* pkParticleSystem,
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    const NiUInt32 uiMeshEmitterCount = GetMeshEmitterCount();
    if (uiMeshEmitterCount == 0)
    {
        return false;
    }

    // Randomly select a mesh from the mesh emitter array.
    NiUInt32 uiWhichEmitter = 0;
    if (uiMeshEmitterCount > 1)
    {
        uiWhichEmitter = (NiUInt32) (NiUnitRandom() * (float) 
            uiMeshEmitterCount);
        if (uiWhichEmitter == uiMeshEmitterCount)
        {
            uiWhichEmitter = uiMeshEmitterCount - 1;
        }
    }

    NiMesh* pkMeshEmitter = GetMeshEmitter(uiWhichEmitter);
    if (!pkMeshEmitter)
    {
        return false;
    }

    // We have separate paths to take if we are skinned, so check to see if
    // we have an NiSkinningMeshModifier.
    NiSkinningMeshModifier* pkSkin = NiGetModifier(NiSkinningMeshModifier,
        pkMeshEmitter);

    switch (m_eMeshEmissionType)
    {
        case EMIT_FROM_VERTICES:
            // Emit from the vertex positions themselves.
            if (pkSkin)
            {
                EmitFromSkinnedVertex(pkParticleSystem, pkMeshEmitter, pkSkin,
                    kPosition, kVelocity);
            }
            else
            {
                EmitFromVertex(pkParticleSystem, pkMeshEmitter, kPosition,
                    kVelocity);
            }
            break;
        case EMIT_FROM_FACE_CENTER:
        case EMIT_FROM_FACE_SURFACE:
            // Emit from the faces of triangles.
            if (pkSkin)
            {
                EmitFromSkinnedFace(pkParticleSystem, pkMeshEmitter, pkSkin,
                    kPosition, kVelocity);
            }
            else
            {
                EmitFromFace(pkParticleSystem, pkMeshEmitter, kPosition,
                    kVelocity);
            }
            break;
        case EMIT_FROM_EDGE_CENTER:
        case EMIT_FROM_EDGE_SURFACE:
            // Emit from the edges of triangles.
            if (pkSkin)
            {
                EmitFromSkinnedEdge(pkParticleSystem, pkMeshEmitter, pkSkin,
                    kPosition, kVelocity);
            }
            else
            {
                EmitFromEdge(pkParticleSystem, pkMeshEmitter, kPosition,
                    kVelocity);
            }
            break;
        default:
            NIASSERT(!"Unknown MeshEmissionType!");
            break;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::EmitFromVertex(
    NiPSParticleSystem* pkParticleSystem,
    NiMesh* pkMeshEmitter,
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    NiDataStreamElementLock kVerticesLock(
        pkMeshEmitter,
        NiCommonSemantics::POSITION(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_READ);
    NIASSERT(kVerticesLock.IsLocked());
    NiTStridedRandomAccessIterator<NiPoint3> kVerticesIter =
        kVerticesLock.begin<NiPoint3>();

    const NiUInt32 uiVertexCount = kVerticesLock.count();
    NIASSERT(uiVertexCount > 0);
    NiUInt32 uiWhichVertex = (NiUInt32) (NiUnitRandom() *
        (float) uiVertexCount);
    if (uiWhichVertex == uiVertexCount)
    {
        --uiWhichVertex;
    }

    // Compute random initial position in mesh.
    kPosition = kVerticesIter[uiWhichVertex];

    if (m_eInitialVelocityType == VELOCITY_USE_NORMALS)
    {
        NiDataStreamElementLock kNormalsLock(
            pkMeshEmitter,
            NiCommonSemantics::NORMAL(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_READ);
        if (kNormalsLock.IsLocked())
        {
            NiTStridedRandomAccessIterator<NiPoint3> kNormalsIter =
                kNormalsLock.begin<NiPoint3>();

            NiPoint3 kNormal = kNormalsIter[uiWhichVertex];

            float fSpeed = kVelocity.Length();
            kVelocity = fSpeed * kNormal;
        }
        else
        {
            NILOG("NiPSMeshEmitter: VELOCITY_USE_NORMALS specified for a mesh "
                "emitter without a %s data stream. Velocity not modified.\n",
                (const char*) NiCommonSemantics::NORMAL());
        }
    }

    TransformIntoCoordinateSpace(pkParticleSystem, pkMeshEmitter, kPosition,
        kVelocity);
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::EmitFromFace(
    NiPSParticleSystem* pkParticleSystem,
    NiMesh* pkMeshEmitter, 
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    // Lock indices stream.
    NiDataStreamPrimitiveLock kIndicesLock(pkMeshEmitter,
        NiDataStream::LOCK_READ);
    if (!kIndicesLock.IndexBufferLocked())
    {
        // A lock could not be acquired for the indices. Emit from vertex
        // instead.
        NILOG("NiPSMeshEmitter: EMIT_FROM_FACE_* specified for a mesh emitter "
            "without an %s data stream. Emitting from vertices instead.\n",
            (const char*) NiCommonSemantics::INDEX());
        EmitFromVertex(pkParticleSystem, pkMeshEmitter, kPosition, kVelocity);
        return;
    }

    // Compute indices for a random, non-degenerate triangle.
    NiUInt32 uiIndex0 = 0;
    NiUInt32 uiIndex1 = 0;
    NiUInt32 uiIndex2 = 0;
    ComputeIndices(kIndicesLock, 0, uiIndex0, uiIndex1, uiIndex2);

    // Unlock indices stream.
    kIndicesLock.Unlock();

    NiDataStreamElementLock kVerticesLock(
        pkMeshEmitter,
        NiCommonSemantics::POSITION(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_READ);
    NIASSERT(kVerticesLock.IsLocked());
    NiTStridedRandomAccessIterator<NiPoint3> kVerticesIter =
        kVerticesLock.begin<NiPoint3>();

    // Ascertain the center of the triangle by averaging all the vertices.
    NiPoint3 kVertex0 = kVerticesIter[uiIndex0];
    NiPoint3 kVertex1 = kVerticesIter[uiIndex1];
    NiPoint3 kVertex2 = kVerticesIter[uiIndex2];

    NiPoint3 kCenterpoint = (kVertex0 + kVertex1 + kVertex2) / 3.0f;
    kPosition = kCenterpoint;

    // We can emit along the vertex normals.
    if (m_eInitialVelocityType == VELOCITY_USE_NORMALS)
    {
        NiDataStreamElementLock kNormalsLock(
            pkMeshEmitter,
            NiCommonSemantics::NORMAL(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_READ);
        if (kNormalsLock.IsLocked())
        {
            NiTStridedRandomAccessIterator<NiPoint3> kNormalsIter =
                kNormalsLock.begin<NiPoint3>();

            // Compute the average normal.
            NiPoint3 kNormal0 = kNormalsIter[uiIndex0];
            NiPoint3 kNormal1 = kNormalsIter[uiIndex1];
            NiPoint3 kNormal2 = kNormalsIter[uiIndex2];
            NiPoint3 kAvgNormal = (kNormal0 + kNormal1 + kNormal2) / 3.0f;
            NiPoint3::UnitizeVector(kAvgNormal);
            float fSpeed = kVelocity.Length();
            kVelocity = fSpeed * kAvgNormal;
        }
        else
        {
            NILOG("NiPSMeshEmitter: VELOCITY_USE_NORMALS specified for a mesh "
                "emitter without a %s data stream. Velocity not modified.\n",
                (const char*) NiCommonSemantics::NORMAL());
        }
    }

    // We can emit from a random position on the surface of the triangle.
    if (m_eMeshEmissionType == EMIT_FROM_FACE_SURFACE)
    {
        NiPoint3 kDir1 = kVertex1 - kVertex0;
        NiPoint3 kDir2 = kVertex2 - kVertex0;

        float fSqrt = NiFastSqrt(NiUnitRandom());

        kPosition = kVertex0 + fSqrt * (NiUnitRandom() * kDir2 - kDir1) +
            kDir1;
    }

    TransformIntoCoordinateSpace(pkParticleSystem, pkMeshEmitter, kPosition,
        kVelocity);
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::EmitFromEdge(
    NiPSParticleSystem* pkParticleSystem,
    NiMesh* pkMeshEmitter, 
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    // Lock indices stream.
    NiDataStreamPrimitiveLock kIndicesLock(pkMeshEmitter,
        NiDataStream::LOCK_READ);
    if (!kIndicesLock.IndexBufferLocked())
    {
        // A lock could not be acquired for the indices. Emit from vertex
        // instead.
        NILOG("NiPSMeshEmitter: EMIT_FROM_EDGE_* specified for a mesh emitter "
            "without an %s data stream. Emitting from vertices instead.\n",
            (const char*) NiCommonSemantics::INDEX());
        EmitFromVertex(pkParticleSystem, pkMeshEmitter, kPosition, kVelocity);
        return;
    }

    // Compute indices for a random, non-degenerate triangle.
    NiUInt32 uiIndex0 = 0;
    NiUInt32 uiIndex1 = 0;
    NiUInt32 uiIndex2 = 0;
    ComputeIndices(kIndicesLock, 0, uiIndex0, uiIndex1, uiIndex2);

    // Unlock indices stream.
    kIndicesLock.Unlock();

    // Randomly select an edge of the triangle.
    switch (NiRand() % 3)
    {
        case 0:
            break;
        case 1:
            uiIndex0 = uiIndex1;
            uiIndex1 = uiIndex2;
            break;
        case 2:
            uiIndex1 = uiIndex2;
            break;
        default:
            break;
    }

    NiDataStreamElementLock kVerticesLock(
        pkMeshEmitter,
        NiCommonSemantics::POSITION(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_READ);
    NIASSERT(kVerticesLock.IsLocked());
    NiTStridedRandomAccessIterator<NiPoint3> kVerticesIter =
        kVerticesLock.begin<NiPoint3>();

    // Compute the centerpoint of the edge.
    NiPoint3 kVertex0 = kVerticesIter[uiIndex0];
    NiPoint3 kVertex1 = kVerticesIter[uiIndex1];

    NiPoint3 kCenterpoint = (kVertex0 + kVertex1) / 2.0f;
    kPosition = kCenterpoint;

    if (m_eInitialVelocityType == VELOCITY_USE_NORMALS)
    {
        NiDataStreamElementLock kNormalsLock(
            pkMeshEmitter,
            NiCommonSemantics::NORMAL(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_READ);
        if (kNormalsLock.IsLocked())
        {
            NiTStridedRandomAccessIterator<NiPoint3> kNormalsIter =
                kNormalsLock.begin<NiPoint3>();

            // Compute the average normal.
            NiPoint3 kNormal0 = kNormalsIter[uiIndex0];
            NiPoint3 kNormal1 = kNormalsIter[uiIndex1];
            NiPoint3 kAvgNormal = (kNormal0 + kNormal1) / 2.0f;
            NiPoint3::UnitizeVector(kAvgNormal);
            float fSpeed = kVelocity.Length();
            kVelocity = fSpeed * kAvgNormal;
        }
        else
        {
            NILOG("NiPSMeshEmitter: VELOCITY_USE_NORMALS specified for a mesh "
                "emitter without a %s data stream. Velocity not modified.\n",
                (const char*) NiCommonSemantics::NORMAL());
        }
    }

    if (m_eMeshEmissionType == EMIT_FROM_EDGE_SURFACE)
    {
        // Emit from a random position on the line segment.
        kPosition = kVertex0 + (kVertex1 - kVertex0) * NiUnitRandom();
    }

    TransformIntoCoordinateSpace(pkParticleSystem, pkMeshEmitter, kPosition,
        kVelocity);
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::EmitFromSkinnedVertex(
    NiPSParticleSystem* pkParticleSystem,
    NiMesh* pkMeshEmitter,
    NiSkinningMeshModifier* pkSkin,
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    // Select a random submesh.
    NiUInt32 uiSubmeshCount = pkMeshEmitter->GetSubmeshCount();
    NIASSERT(uiSubmeshCount > 0);
    NiUInt32 uiWhichSubmesh = 0;
    if (uiSubmeshCount > 1)
    {
        uiWhichSubmesh = (NiUInt32) (NiUnitRandom() * (float) uiSubmeshCount);
        uiWhichSubmesh = NiMin((NiInt32) uiWhichSubmesh, uiSubmeshCount - 1);
    }

    // Select a random vertex in that submesh.
    NiDataStreamRef* pkPositionsStreamRef =
        pkMeshEmitter->FindStreamRef(NiCommonSemantics::POSITION_BP(), 0,
        NiDataStreamElement::F_FLOAT32_3);
    NIASSERT(pkPositionsStreamRef);
    const NiDataStream::Region& kPositionsRegion =
        pkPositionsStreamRef->GetRegionForSubmesh(uiWhichSubmesh);
    const NiUInt32 uiVertexCount = kPositionsRegion.GetRange();
    NIASSERT(uiVertexCount > 0);
    NiUInt32 uiWhichVertex = (NiUInt32) (NiUnitRandom() *
        (float) uiVertexCount);
    if (uiWhichVertex == uiVertexCount)
    {
        --uiWhichVertex;
    }

    // We need to get the skinned mesh position, so we deform the vertex in
    // software.
    NiPoint3 kVertex;
    NiPoint3 kInvalidNormal = NiPoint3::UNIT_ALL * -NI_INFINITY;
    NiPoint3 kNormal = kInvalidNormal;
    pkSkin->DeformVertex(kVertex, kNormal, pkMeshEmitter, uiWhichSubmesh,
        uiWhichVertex);

    kPosition = kVertex;
    if (m_eInitialVelocityType == VELOCITY_USE_NORMALS)
    {
        if (kNormal != kInvalidNormal)
        {
            float fSpeed = kVelocity.Length();
            kVelocity = fSpeed * kNormal;
        }
        else
        {
            NILOG("NiPSMeshEmitter: VELOCITY_USE_NORMALS specified for a mesh "
                "emitter without a %s data stream. Velocity not modified.\n",
                (const char*) NiCommonSemantics::NORMAL_BP());
        }
    }
    
   TransformIntoCoordinateSpace(pkParticleSystem, pkMeshEmitter, kPosition,
       kVelocity);
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::EmitFromSkinnedFace(
    NiPSParticleSystem* pkParticleSystem,
    NiMesh* pkMeshEmitter,
    NiSkinningMeshModifier* pkSkin,
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    // Lock indices stream.
    NiDataStreamPrimitiveLock kIndicesLock(pkMeshEmitter,
        NiDataStream::LOCK_READ);
    if (!kIndicesLock.IndexBufferLocked())
    {
        // A lock could not be acquired for the indices. Emit from vertex
        // instead.
        NILOG("NiPSMeshEmitter: EMIT_FROM_FACE_* specified for a mesh emitter "
            "without an %s data stream. Emitting from vertices instead.\n",
            (const char*) NiCommonSemantics::INDEX());
        EmitFromSkinnedVertex(pkParticleSystem, pkMeshEmitter, pkSkin,
            kPosition, kVelocity);
        return;
    }

    // Select a random submesh.
    NiUInt32 uiSubmeshCount = pkMeshEmitter->GetSubmeshCount();
    NIASSERT(uiSubmeshCount > 0);
    NiUInt32 uiWhichSubmesh = 0;
    if (uiSubmeshCount > 1)
    {
        uiWhichSubmesh = (NiUInt32) (NiUnitRandom() * (float) uiSubmeshCount);
        uiWhichSubmesh = NiMin((NiInt32) uiWhichSubmesh, uiSubmeshCount - 1);
    }

    // Compute indices for a random, non-degenerate triangle.
    NiUInt32 uiIndex0 = 0;
    NiUInt32 uiIndex1 = 0;
    NiUInt32 uiIndex2 = 0;
    ComputeIndices(kIndicesLock, uiWhichSubmesh, uiIndex0, uiIndex1, uiIndex2);

    // Unlock indices stream.
    kIndicesLock.Unlock();

    // We must now deform the vertices of the triangle.
    NiPoint3 kVertex0, kVertex1, kVertex2;
    NiPoint3 kInvalidNormal = NiPoint3::UNIT_ALL * -NI_INFINITY;
    NiPoint3 kNormal0 = kInvalidNormal;
    NiPoint3 kNormal1 = kInvalidNormal;
    NiPoint3 kNormal2 = kInvalidNormal;
    pkSkin->DeformVertex(kVertex0, kNormal0, pkMeshEmitter, uiWhichSubmesh,
        uiIndex0);
    pkSkin->DeformVertex(kVertex1, kNormal1, pkMeshEmitter, uiWhichSubmesh,
        uiIndex1);
    pkSkin->DeformVertex(kVertex2, kNormal2, pkMeshEmitter, uiWhichSubmesh,
        uiIndex2);

    // Determine the average location of the vertices
    NiPoint3 kCenterpoint = (kVertex0 + kVertex1 + kVertex2) / 3.0f;
    kPosition = kCenterpoint;

    if (m_eInitialVelocityType == VELOCITY_USE_NORMALS)
    {
        if (kNormal0 != kInvalidNormal &&
            kNormal1 != kInvalidNormal &&
            kNormal2 != kInvalidNormal)
        {
            // Compute the average normal
            NiPoint3 kAvgNormal = (kNormal0 + kNormal1 + kNormal2) / 3.0f;
            NiPoint3::UnitizeVector(kAvgNormal);
            float fSpeed = kVelocity.Length();
            kVelocity = fSpeed * kAvgNormal;
        }
        else
        {
            NILOG("NiPSMeshEmitter: VELOCITY_USE_NORMALS specified for a mesh "
                "emitter without a %s data stream. Velocity not modified.\n",
                (const char*) NiCommonSemantics::NORMAL_BP());
        }
    }

    if (m_eMeshEmissionType == EMIT_FROM_FACE_SURFACE)
    {
        // Emit from a random position on the face of the skinned triangle.
        NiPoint3 kDir1 = kVertex1 - kVertex0;
        NiPoint3 kDir2 = kVertex2 - kVertex0;

        float fSqrt = NiFastSqrt(NiUnitRandom());

        kPosition = kVertex0 + fSqrt * (NiUnitRandom() * kDir2 - kDir1) + 
            kDir1;
    }

    TransformIntoCoordinateSpace(pkParticleSystem, pkMeshEmitter, kPosition,
        kVelocity);
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::EmitFromSkinnedEdge(
    NiPSParticleSystem* pkParticleSystem,
    NiMesh* pkMeshEmitter,
    NiSkinningMeshModifier* pkSkin,
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    // Lock indices stream.
    NiDataStreamPrimitiveLock kIndicesLock(pkMeshEmitter,
        NiDataStream::LOCK_READ);
    if (!kIndicesLock.IndexBufferLocked())
    {
        // A lock could not be acquired for the indices. Emit from vertex
        // instead.
        NILOG("NiPSMeshEmitter: EMIT_FROM_EDGE_* specified for a mesh emitter "
            "without an %s data stream. Emitting from vertices instead.\n",
            (const char*) NiCommonSemantics::INDEX());
        EmitFromSkinnedVertex(pkParticleSystem, pkMeshEmitter, pkSkin,
            kPosition, kVelocity);
        return;
    }

    // Select a random submesh.
    NiUInt32 uiSubmeshCount = pkMeshEmitter->GetSubmeshCount();
    NIASSERT(uiSubmeshCount > 0);
    NiUInt32 uiWhichSubmesh = 0;
    if (uiSubmeshCount > 1)
    {
        uiWhichSubmesh = (NiUInt32) (NiUnitRandom() * (float) uiSubmeshCount);
        uiWhichSubmesh = NiMin((NiInt32) uiWhichSubmesh, uiSubmeshCount - 1);
    }

    // Compute indices for a random, non-degenerate triangle.
    NiUInt32 uiIndex0 = 0;
    NiUInt32 uiIndex1 = 0;
    NiUInt32 uiIndex2 = 0;
    ComputeIndices(kIndicesLock, uiWhichSubmesh, uiIndex0, uiIndex1, uiIndex2);

    // Unlock indices stream.
    kIndicesLock.Unlock();

    // Randomly select an edge of the triangle.
    switch (NiRand() % 3)
    {
        case 0:
            break;
        case 1:
            uiIndex0 = uiIndex1;
            uiIndex1 = uiIndex2;
            break;
        case 2:
            uiIndex1 = uiIndex2;
            break;
        default:
            break;
    }

    // Deform the vertices of the edge.
    NiPoint3 kVertex0, kVertex1;
    NiPoint3 kInvalidNormal = NiPoint3::UNIT_ALL * -NI_INFINITY;
    NiPoint3 kNormal0 = kInvalidNormal;
    NiPoint3 kNormal1 = kInvalidNormal;
    pkSkin->DeformVertex(kVertex0, kNormal0, pkMeshEmitter, uiWhichSubmesh,
        uiIndex0);
    pkSkin->DeformVertex(kVertex1, kNormal1, pkMeshEmitter, uiWhichSubmesh,
        uiIndex1);

    // Compute the centerpoint
    NiPoint3 kCenterpoint = (kVertex0 + kVertex1) / 2.0f;
    kPosition = kCenterpoint;

    if (m_eInitialVelocityType == VELOCITY_USE_NORMALS)
    {
        if (kNormal0 != kInvalidNormal &&
            kNormal1 != kInvalidNormal)
        {
            // Compute the average normal
            NiPoint3 kAvgNormal = (kNormal0 + kNormal1) / 2.0f;
            NiPoint3::UnitizeVector(kAvgNormal);
            float fSpeed = kVelocity.Length();
            kVelocity = fSpeed * kAvgNormal;
        }
        else
        {
            NILOG("NiPSMeshEmitter: VELOCITY_USE_NORMALS specified for a mesh "
                "emitter without a %s data stream. Velocity not modified.\n",
                (const char*) NiCommonSemantics::NORMAL_BP());
        }
    }

    if (m_eMeshEmissionType == EMIT_FROM_EDGE_SURFACE)
    {
        kPosition = kVertex0 + (kVertex1 - kVertex0) * NiUnitRandom();
    }

    TransformIntoCoordinateSpace(pkParticleSystem, pkMeshEmitter, kPosition,
        kVelocity);
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::ComputeIndices(
    const NiDataStreamPrimitiveLock& kIndicesLock,
    const NiUInt32 uiSubmeshIdx,
    NiUInt32& uiIndex0,
    NiUInt32& uiIndex1,
    NiUInt32& uiIndex2)
{
    // Select a random triangle from the mesh.
    NiUInt32 uiTriCount = kIndicesLock.count(uiSubmeshIdx);
    NIASSERT(uiTriCount > 0);
    NiUInt32 uiWhichTri = (NiUInt32) (NiUnitRandom() * (float) uiTriCount);
    if (uiWhichTri == uiTriCount)
    {
        uiWhichTri = uiTriCount - 1;
    }

    NiUInt32 uiDegenerateScan;
    for (uiDegenerateScan = 0; uiDegenerateScan < MAX_DEGENERATE_TRIANGLES;
        ++uiDegenerateScan)
    {
        if (kIndicesLock.Has16BitIndexBuffer())
        {
            NiIndexedPrimitiveIterator16 kIndicesIter =
                kIndicesLock.BeginIndexed16(uiSubmeshIdx);
            uiIndex0 = kIndicesIter[uiWhichTri][0];
            uiIndex1 = kIndicesIter[uiWhichTri][1];
            uiIndex2 = kIndicesIter[uiWhichTri][2];
        }
        else
        {
            NIASSERT(kIndicesLock.Has32BitIndexBuffer());

            NiIndexedPrimitiveIterator32 kIndicesIter =
                kIndicesLock.BeginIndexed32(uiSubmeshIdx);
            uiIndex0 = kIndicesIter[uiWhichTri][0];
            uiIndex1 = kIndicesIter[uiWhichTri][1];
            uiIndex2 = kIndicesIter[uiWhichTri][2];
        }

        // Check for degenerate triangle.
        if (uiIndex0 == uiIndex1 ||
            uiIndex0 == uiIndex2 ||
            uiIndex1 == uiIndex2)
        {
            // Move to the next triangle and look for wrapping.
            if (++uiWhichTri >= uiTriCount)
            {
                uiWhichTri = 0;
            }
        }
        else
        {
            // Break out of the loop.
            break;
        }
    }

    // If this assertion is hit, we will be emitting from a degenerate
    // triangle.
    NIASSERT(uiDegenerateScan < MAX_DEGENERATE_TRIANGLES);
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::TransformIntoCoordinateSpace(
    NiPSParticleSystem* pkParticleSystem,
    NiAVObject* pkObject,
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    NIASSERT(pkParticleSystem && pkObject);

    // Modify velocity and position based on emitter object transforms.
    NiTransform kEmitter = pkObject->GetWorldTransform();
    NiTransform kPSys = pkParticleSystem->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kEmitterToPSys = kInvPSys * kEmitter;

    // Update position.
    kPosition = kEmitterToPSys * kPosition;

    // Update velocity
    switch (m_eInitialVelocityType)
    {
        case VELOCITY_USE_NORMALS:
            // Should have been precomputed by the Emit methods.
            kVelocity = kEmitterToPSys.m_Rotate * kVelocity;
            break;
        case VELOCITY_USE_RANDOM:
        {
            kVelocity.x = NiSymmetricRandom();
            kVelocity.y = NiSymmetricRandom();
            kVelocity.z = NiSymmetricRandom();
            kVelocity.Unitize();
            float fSpeed = m_fSpeed;
            if (m_fSpeedVar != 0.0f)
            {
                fSpeed += m_fSpeedVar * (NiUnitRandom() - 0.5f);
            }
            kVelocity *= fSpeed;
            break;
        }
        case VELOCITY_USE_DIRECTION:
        {
            // Emit along a specific axis relative to the emitter.
            NiPoint3 kDirection = kEmitterToPSys.m_Rotate * m_kEmitAxis;
            float fSpeed = m_fSpeed;
            if (m_fSpeedVar != 0.0f)
            {
                fSpeed += m_fSpeedVar * (NiUnitRandom() - 0.5f);
            }
            kDirection.Unitize();
            kVelocity = kDirection * fSpeed;
            break;
        }
        default:
            NIASSERT(!"Unknown InitialVelocityType!");
            break;
    }
}
//---------------------------------------------------------------------------
bool NiPSMeshEmitter::ValidateMesh(NiMesh* pkMesh)
{
    NIASSERT(pkMesh);

    // Only points, triangles, and tristrips are supported as mesh emitters.
    if (pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_POINTS &&
        pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRIANGLES &&
        pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRISTRIPS)
    {
        return false;
    }

    if (!ValidateDataStream(pkMesh, NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UNKNOWN, false))
    {
        return false;
    }

    NiSkinningMeshModifier* pkSkin = NiGetModifier(NiSkinningMeshModifier,
        pkMesh);
    if (pkSkin)
    {
        if (pkSkin->GetSoftwareSkinned())
        {
            if (!ValidateDataStream(pkMesh, NiCommonSemantics::POSITION_BP(), 0,
                NiDataStreamElement::F_UNKNOWN, true))
            {
                return false;
            }

            if (!ValidateDataStream(pkMesh, NiCommonSemantics::NORMAL_BP(), 0,
                NiDataStreamElement::F_UNKNOWN, false))
            {
                return false;
            }

            // This code ensures that ACCESS_CPU_READ remains set when MeshFinalize
            // interleaves data streams.
            if (!ValidateDataStream(pkMesh, NiCommonSemantics::POSITION(), 0,
                NiDataStreamElement::F_UNKNOWN, true))
            {
                return false;
            }

            if (!ValidateDataStream(pkMesh, NiCommonSemantics::NORMAL(), 0,
                NiDataStreamElement::F_UNKNOWN, false))
            {
                return false;
            }
        }
        else
        {
            if (!ValidateDataStream(pkMesh, NiCommonSemantics::POSITION_BP(),
                0, NiDataStreamElement::F_UNKNOWN, true))
            {
                return false;
            }

            if (!ValidateDataStream(pkMesh, NiCommonSemantics::NORMAL_BP(), 0,
                NiDataStreamElement::F_UNKNOWN, false))
            {
                return false;
            }

            if (!ValidateDataStream(pkMesh, NiCommonSemantics::BONE_PALETTE(),
                0, NiDataStreamElement::F_UNKNOWN, true))
            {
                return false;
            }
        }

        if (!ValidateDataStream(pkMesh, NiCommonSemantics::BLENDWEIGHT(), 0,
            NiDataStreamElement::F_UNKNOWN, true))
        {
            return false;
        }

        if (!ValidateDataStream(pkMesh, NiCommonSemantics::BLENDINDICES(), 0,
            NiDataStreamElement::F_UNKNOWN, true))
        {
            return false;
        }
    }
    else
    {
        if (!ValidateDataStream(pkMesh, NiCommonSemantics::POSITION(), 0,
            NiDataStreamElement::F_UNKNOWN, true))
        {
            return false;
        }

        if (!ValidateDataStream(pkMesh, NiCommonSemantics::NORMAL(), 0,
            NiDataStreamElement::F_UNKNOWN, false))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPSMeshEmitter::ValidateDataStream(
    NiMesh* pkMesh,
    const NiFixedString& kSemantic,
    NiUInt32 uiSemanticIndex,
    NiDataStreamElement::Format eFormat,
    bool bStreamRequired)
{
    NIASSERT(pkMesh);

    NiDataStreamRef* pkStreamRef = pkMesh->FindStreamRef(kSemantic,
        uiSemanticIndex, eFormat);
    if (!pkStreamRef)
    {
        NILOG("NiPSMeshEmitter: %s data stream not found on mesh emitter.\n",
                (const char*) kSemantic);
        return !bStreamRequired;
    }

    NiDataStream* pkStream = pkStreamRef->GetDataStream();
    NiToolDataStream* pkToolStream = NiDynamicCast(NiToolDataStream, pkStream);
    if (pkToolStream)
    {
        pkToolStream->MakeReadable();
    }
    else
    {
        if (!pkStream->IsLockRequestValid(NiDataStream::LOCK_READ))
        {
            NILOG("NiPSMeshEmitter: Invalid %s data stream on mesh emitter.\n",
                (const char*) kSemantic);
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSMeshEmitter);
//---------------------------------------------------------------------------
void NiPSMeshEmitter::CopyMembers(
    NiPSMeshEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSEmitter::CopyMembers(pkDest, kCloning);

    pkDest->m_kEmitAxis = m_kEmitAxis;
    pkDest->m_eMeshEmissionType = m_eMeshEmissionType;
    pkDest->m_eInitialVelocityType = m_eInitialVelocityType;
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSEmitter::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSMeshEmitter* pkDest = (NiPSMeshEmitter*) pkObject;

    const NiUInt32 uiMeshEmittersCount = m_kMeshEmitters.GetSize();
    for (NiUInt32 ui = 0; ui < uiMeshEmittersCount; ++ui)
    {
        NiMesh* pkMeshEmitter = m_kMeshEmitters.GetAt(ui);
        if (kCloning.m_pkCloneMap->GetAt(pkMeshEmitter, pkObject))
        {
            pkDest->m_kMeshEmitters.Add((NiMesh*) pkObject);
        }
        else
        {
            pkDest->m_kMeshEmitters.Add(pkMeshEmitter);
        }
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSMeshEmitter);
//---------------------------------------------------------------------------
void NiPSMeshEmitter::LoadBinary(NiStream& kStream)
{
    NiPSEmitter::LoadBinary(kStream);

    NiUInt32 uiMeshEmittersCount;
    NiStreamLoadBinary(kStream, uiMeshEmittersCount);
    for (NiUInt32 ui = 0; ui < uiMeshEmittersCount; ++ui)
    {
        m_kMeshEmitters.Add((NiMesh*) kStream.ResolveLinkID());
    }

    m_kEmitAxis.LoadBinary(kStream);
    NiStreamLoadEnum(kStream, m_eMeshEmissionType);
    NiStreamLoadEnum(kStream, m_eInitialVelocityType);
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::LinkObject(NiStream& kStream)
{
    NiPSEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSMeshEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSMeshEmitter::SaveBinary(NiStream& kStream)
{
    NiPSEmitter::SaveBinary(kStream);

    const NiUInt32 uiMeshEmittersCount = m_kMeshEmitters.GetSize();
    NiStreamSaveBinary(kStream, uiMeshEmittersCount);
    for (NiUInt32 ui = 0; ui < uiMeshEmittersCount; ++ui)
    {
        kStream.SaveLinkID(m_kMeshEmitters.GetAt(ui));
    }

    m_kEmitAxis.SaveBinary(kStream);
    NiStreamSaveEnum(kStream, m_eMeshEmissionType);
    NiStreamSaveEnum(kStream, m_eInitialVelocityType);
}
//---------------------------------------------------------------------------
bool NiPSMeshEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSMeshEmitter* pkDest = (NiPSMeshEmitter*) pkObject;

    if (pkDest->m_kMeshEmitters.GetSize() != m_kMeshEmitters.GetSize())
    {
        return false;
    }

    if (pkDest->m_kEmitAxis != m_kEmitAxis ||
        pkDest->m_eMeshEmissionType != m_eMeshEmissionType ||
        pkDest->m_eInitialVelocityType != m_eInitialVelocityType)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSMeshEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSMeshEmitter::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Mesh Emitter Count",
        GetMeshEmitterCount()));
}
//---------------------------------------------------------------------------
