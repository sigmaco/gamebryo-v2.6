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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#include "NiPhysXClothDest.h"
#include "NiPhysXClothModifier.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
// unreferenced formal parameter ...
#pragma warning(disable: 4100)
// assignment operator could not be generated ...
#pragma warning(disable: 4512)
// conversion from 'type' to 'type', possible loss of data ...
#pragma warning(disable: 4244)
// conversion from 'type' to 'type', signed/unsigned mismatch ...
#pragma warning(disable: 4245)
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

NiImplementRTTI(NiPhysXClothDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXClothDesc::NiPhysXClothDesc() : m_kClothName(NULL), m_kAttachments(0,5)
{
    NxClothDesc kClothDesc;
    kClothDesc.setToDefault();
    
    m_fThickness = kClothDesc.thickness;
    m_fDensity = kClothDesc.density;
    m_fBendingStiffness = kClothDesc.bendingStiffness;
    m_fStretchingStiffness = kClothDesc.stretchingStiffness;
    m_fDampingCoefficient = kClothDesc.dampingCoefficient;
    m_fFriction = kClothDesc.friction;
    m_fPressure = kClothDesc.pressure;
    m_fTearFactor = kClothDesc.tearFactor;
    m_fCollisionResponseCoefficient = kClothDesc.collisionResponseCoefficient;
    m_fAttachmentResponseCoefficient =
        kClothDesc.attachmentResponseCoefficient;
    m_fAttachmentTearFactor = kClothDesc.attachmentTearFactor;
    m_fToFluidResponseCoefficient = kClothDesc.toFluidResponseCoefficient;
    m_fFromFluidResponseCoefficient = kClothDesc.fromFluidResponseCoefficient;
    m_fMinAdhereVelocity = kClothDesc.minAdhereVelocity;
    m_fRelativeGridSpacing = kClothDesc.relativeGridSpacing;
    m_uiSolverIterations = kClothDesc.solverIterations;
    m_kExternalAcceleration = kClothDesc.externalAcceleration;
    m_kWindAcceleration = kClothDesc.windAcceleration;
    m_fWakeUpCounter = kClothDesc.wakeUpCounter;
    m_fSleepLinearVelocity = kClothDesc.sleepLinearVelocity;
    m_uiCollisionGroup = kClothDesc.collisionGroup;
    m_auiCollisionBits[0] = kClothDesc.groupsMask.bits0;
    m_auiCollisionBits[1] = kClothDesc.groupsMask.bits1;
    m_auiCollisionBits[2] = kClothDesc.groupsMask.bits2;
    m_auiCollisionBits[3] = kClothDesc.groupsMask.bits3;
    m_uiForceFieldMaterial = kClothDesc.forceFieldMaterial;
    m_uiFlags = kClothDesc.flags;

    m_uiNumStates = 0;
    m_pkPoses = 0;
    m_puiNumVertexPosns = 0;
    m_ppkVertexPosns = 0;
    m_puiNumTearIndices = 0;
    m_ppuiTearIndices = 0;
    m_ppkTearSplitPlanes = 0;

    m_spClothMesh = 0;
    
    m_pkCloth = 0;
    m_pkParentActor = 0;
    
    m_pkTargetMesh = 0;
    
    m_spDest = 0;
}
//---------------------------------------------------------------------------
NiPhysXClothDesc::~NiPhysXClothDesc()
{
    for (NiUInt32 ui = 0; ui < m_uiNumStates; ui++)
    {
        NiFree(m_ppkVertexPosns[ui]);
        NiFree(m_ppuiTearIndices[ui]);
        NiFree(m_ppkTearSplitPlanes[ui]);
    }
    NiFree(m_puiNumVertexPosns);
    NiFree(m_pkPoses);
    NiFree(m_ppkVertexPosns);
    NiFree(m_puiNumTearIndices);
    NiFree(m_ppuiTearIndices);
    NiFree(m_ppkTearSplitPlanes);

    m_spClothMesh = 0;
    m_spDest = 0;
    
    for (NiUInt32 ui = m_kAttachments.GetSize(); ui > 0; ui--)
        RemoveAttachmentAt(ui-1);
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::SetConstantParameters(NiFixedString& kClothName, 
    NiPhysXMeshDesc* pkMeshDesc, const NxReal fThickness, 
    const NxReal fDensity, const NxReal fBendingStiffness, 
    const NxReal fStretchingStiffness, const NxReal fDampingCoefficient, 
    const NxReal fFriction, const NxReal fPressure, const NxReal fTearFactor, 
    const NxReal fCollisionResponseCoefficient, 
    const NxReal fAttachmentResponseCoefficient, 
    const NxReal fAttachmentTearFactor, 
    const NxReal fToFluidResponseCoefficient, 
    const NxReal fFromFluidResponseCoefficient, 
    const NxReal fMinAdhereVelocity, const NxReal fRelativeGridSpacing, 
    const NxU32 uiSolverIterations, const NxVec3 kExternalAcceleration,
    const NxVec3 kWindAcceleration, const NxReal fWakeUpCounter, 
    const NxReal fSleepLinearVelocity, const NxCollisionGroup uiCollisionGroup,
    const NxGroupsMask kCollisionMask, const NxU16 uiForceFieldMaterial,
    const NxU32 uiFlags)
{
    m_kClothName = kClothName;
    m_spClothMesh = pkMeshDesc;
    m_fThickness = fThickness;
    m_fDensity = fDensity;
    m_fBendingStiffness = fBendingStiffness;
    m_fStretchingStiffness = fStretchingStiffness;
    m_fDampingCoefficient = fDampingCoefficient;
    m_fFriction = fFriction;
    m_fPressure = fPressure;
    m_fTearFactor = fTearFactor;
    m_fCollisionResponseCoefficient = fCollisionResponseCoefficient;
    m_fAttachmentResponseCoefficient = fAttachmentResponseCoefficient;
    m_fAttachmentTearFactor = fAttachmentTearFactor;
    m_fToFluidResponseCoefficient = fToFluidResponseCoefficient;
    m_fFromFluidResponseCoefficient = fFromFluidResponseCoefficient;
    m_fMinAdhereVelocity = fMinAdhereVelocity;
    m_fRelativeGridSpacing = fRelativeGridSpacing;
    m_uiSolverIterations = uiSolverIterations;
    m_kExternalAcceleration = kExternalAcceleration;
    m_kWindAcceleration = kWindAcceleration;
    m_fWakeUpCounter = fWakeUpCounter;
    m_fSleepLinearVelocity = fSleepLinearVelocity;
    m_uiCollisionGroup = uiCollisionGroup;
    m_auiCollisionBits[0] = kCollisionMask.bits0;
    m_auiCollisionBits[1] = kCollisionMask.bits1;
    m_auiCollisionBits[2] = kCollisionMask.bits2;
    m_auiCollisionBits[3] = kCollisionMask.bits3;
    m_uiForceFieldMaterial = uiForceFieldMaterial;
    m_uiFlags = uiFlags;
}
//---------------------------------------------------------------------------
bool NiPhysXClothDesc::ToClothDesc(NxClothDesc& kClothDesc,
    const NiUInt32 uiIndex, const NxMat34& kXform, const bool bKeepMeshes,
    const bool bCheckPlatform)
{
    kClothDesc.name = m_kClothName;
    if (uiIndex < m_uiNumStates)
    {
        kClothDesc.globalPose = kXform * m_pkPoses[uiIndex];
    }
    else
    {
        kClothDesc.globalPose = kXform;
    }
    kClothDesc.thickness = m_fThickness;
    kClothDesc.density = m_fDensity;
    kClothDesc.bendingStiffness = m_fBendingStiffness;
    kClothDesc.stretchingStiffness = m_fStretchingStiffness;
    kClothDesc.dampingCoefficient = m_fDampingCoefficient;
    kClothDesc.friction = m_fFriction;
    kClothDesc.pressure = m_fPressure;
    kClothDesc.tearFactor = m_fTearFactor;
    kClothDesc.collisionResponseCoefficient = m_fCollisionResponseCoefficient;
    kClothDesc.attachmentResponseCoefficient =
        m_fAttachmentResponseCoefficient;
    kClothDesc.attachmentTearFactor = m_fAttachmentTearFactor;
    kClothDesc.toFluidResponseCoefficient = m_fToFluidResponseCoefficient;
    kClothDesc.fromFluidResponseCoefficient = m_fFromFluidResponseCoefficient;
    kClothDesc.minAdhereVelocity = m_fMinAdhereVelocity;
    kClothDesc.relativeGridSpacing = m_fRelativeGridSpacing;
    kClothDesc.solverIterations = m_uiSolverIterations;
    kClothDesc.externalAcceleration = kXform.M * m_kExternalAcceleration;
    kClothDesc.windAcceleration = kXform.M * m_kWindAcceleration;
    kClothDesc.wakeUpCounter = m_fWakeUpCounter;
    kClothDesc.sleepLinearVelocity = m_fSleepLinearVelocity;
    kClothDesc.collisionGroup = m_uiCollisionGroup;
    kClothDesc.groupsMask.bits0 = m_auiCollisionBits[0];
    kClothDesc.groupsMask.bits1 = m_auiCollisionBits[1];
    kClothDesc.groupsMask.bits2 = m_auiCollisionBits[2];
    kClothDesc.groupsMask.bits3 = m_auiCollisionBits[3];
    kClothDesc.forceFieldMaterial = m_uiForceFieldMaterial;
    kClothDesc.flags = m_uiFlags;

    m_spClothMesh->ToClothDesc(kClothDesc, bKeepMeshes, bCheckPlatform);
    
    if (kClothDesc.clothMesh)
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::SetDest(NiPhysXClothDest* pkDest)
{
    m_spDest = pkDest;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// State management functions.
//---------------------------------------------------------------------------
void NiPhysXClothDesc::SetVertexPositions(const NiUInt16 uiNumPosns,
    const NxVec3* pkPosns, const NiUInt32 uiIndex)
{
    PresizeStateArrays(uiIndex + 1);
    
    m_puiNumVertexPosns[uiIndex] = uiNumPosns;

    NiFree(m_ppkVertexPosns[uiIndex]);
    
    size_t uiSize = sizeof(NxVec3) * uiNumPosns;
    m_ppkVertexPosns[uiIndex] = (NxVec3*)NiMalloc(uiSize);
    NIASSERT(m_ppkVertexPosns[uiIndex]);
    
    NiMemcpy(m_ppkVertexPosns[uiIndex], uiSize, pkPosns, uiSize);
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::SetTornVertices(const NiUInt16 uiNumTorn,
    const NiUInt16* pkIndexes, const NxVec3* pkNormals, const NiUInt32 uiIndex)
{
    PresizeStateArrays(uiIndex + 1);
    
    m_puiNumTearIndices[uiIndex] = uiNumTorn;

    NiFree(m_ppuiTearIndices[uiIndex]);
    NiFree(m_ppkTearSplitPlanes[uiIndex]);
    
    size_t uiSize = sizeof(NiUInt16) * uiNumTorn;
    m_ppuiTearIndices[uiIndex] = (NiUInt16*)NiMalloc(uiSize);
    NIASSERT(m_ppuiTearIndices[uiIndex]);
    
    NiMemcpy(m_ppuiTearIndices[uiIndex], uiSize, pkIndexes, uiSize);
    
    uiSize = sizeof(NxVec3) * uiNumTorn;
    m_ppkTearSplitPlanes[uiIndex] = (NxVec3*)NiMalloc(uiSize);
    NIASSERT(m_ppkTearSplitPlanes[uiIndex]);
    
    NiMemcpy(m_ppkTearSplitPlanes[uiIndex], uiSize, pkNormals, uiSize);
}
//---------------------------------------------------------------------------
NiUInt32 NiPhysXClothDesc::AddState(const NxMat34& kXform)
{
    if (!m_pkCloth)
        return 0;
    
    NiUInt32 uiIndex = m_uiNumStates;

    PresizeStateArrays(m_uiNumStates + 1);
    
    m_pkPoses[uiIndex].M.id();
    m_pkPoses[uiIndex].t.zero();
    
    m_puiNumVertexPosns[uiIndex] = (NiUInt16)m_pkCloth->getNumberOfParticles();
    
    NxVec3* pkPosns = NiAlloc(NxVec3, m_puiNumVertexPosns[uiIndex]);
    m_pkCloth->getPositions((void*)pkPosns);
    for (NiUInt32 ui = 0; ui < m_puiNumVertexPosns[uiIndex]; ui++)
        m_ppkVertexPosns[uiIndex][ui] = kXform * pkPosns[ui];
    NiFree(pkPosns);

    NxMeshData kMeshData = m_pkCloth->getMeshData();
    if (!kMeshData.parentIndicesBegin ||
        !kMeshData.numParentIndicesPtr)
    {
        return uiIndex;
    }
    
    // Count the number of torn vertices
    NiUInt8* pkParentIndices = (NiUInt8*)kMeshData.parentIndicesBegin;
    for (NiUInt32 ui = 0; ui < kMeshData.numParentIndicesPtr[0]; ui++)
    {
        NiUInt32 uiParentIndex = 0;
        if (kMeshData.parentIndicesByteStride == 2)
            uiParentIndex = *(NiUInt16*)pkParentIndices;
        else if (kMeshData.parentIndicesByteStride == 4)
            uiParentIndex = *(NiUInt32*)pkParentIndices;
        else
            NIASSERT(false && "NiPhysXClothDesc::AddState unknown index type");

        if (ui != uiParentIndex)
            m_puiNumTearIndices[uiIndex]++;
            
        pkParentIndices += kMeshData.parentIndicesByteStride;
    }
    
    // Set the tear vertices
    m_ppuiTearIndices[uiIndex] =
        NiAlloc(NiUInt16, m_puiNumTearIndices[uiIndex]);
    m_ppkTearSplitPlanes[uiIndex] =
        NiAlloc(NxVec3, m_puiNumTearIndices[uiIndex]);
    pkParentIndices = (NiUInt8*)kMeshData.parentIndicesBegin;
    NiUInt32 uiNext = 0;
    for (NiUInt32 ui = 0; ui < kMeshData.numParentIndicesPtr[0]; ui++)
    {
        NiUInt32 uiParentIndex = 0;
        if (kMeshData.parentIndicesByteStride == 2)
            uiParentIndex = *(NiUInt16*)pkParentIndices;
        else if (kMeshData.parentIndicesByteStride == 4)
            uiParentIndex = *(NiUInt32*)pkParentIndices;

        if (ui != uiParentIndex)
        {
            m_ppuiTearIndices[uiIndex][uiNext] = (NiUInt16)uiParentIndex;
            m_ppkTearSplitPlanes[uiIndex][uiNext] =
                m_ppkVertexPosns[uiIndex][ui] -
                m_ppkVertexPosns[uiIndex][uiParentIndex];
            m_ppkTearSplitPlanes[uiIndex][uiNext].normalize();
            ++uiNext;
        }
            
        pkParentIndices += kMeshData.parentIndicesByteStride;
    }

    return uiIndex;
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::RemoveStatesBeyond(const NiUInt32 uiState)
{
    if (m_uiNumStates <= uiState)
        return;

    for (NiUInt32 ui = uiState; ui < m_uiNumStates; ui++)
    {
        NiFree(m_ppkVertexPosns[ui]);
        NiFree(m_ppuiTearIndices[ui]);
        NiFree(m_ppkTearSplitPlanes[ui]);
    }

    m_uiNumStates = uiState;

    m_pkPoses = (NxMat34*)NiRealloc(
        (void*)m_pkPoses, sizeof(NxMat34) * m_uiNumStates);
    NIASSERT(m_pkPoses);

    m_puiNumVertexPosns = (NiUInt16*)NiRealloc(
        (void*)m_puiNumVertexPosns, sizeof(NiUInt16) * m_uiNumStates);
    NIASSERT(m_puiNumVertexPosns);

    m_ppkVertexPosns = (NxVec3**)NiRealloc(
        (void*)m_ppkVertexPosns, sizeof(NxVec3*) * m_uiNumStates);
    NIASSERT(m_ppkVertexPosns);

    m_puiNumTearIndices = (NiUInt16*)NiRealloc(
        (void*)m_puiNumTearIndices, sizeof(NiUInt16) * m_uiNumStates);
    NIASSERT(m_puiNumTearIndices);

    m_ppuiTearIndices = (NiUInt16**)NiRealloc(
        (void*)m_ppuiTearIndices, sizeof(NiUInt16*) * m_uiNumStates);
    NIASSERT(m_ppuiTearIndices);

    m_ppkTearSplitPlanes = (NxVec3**)NiRealloc(
        (void*)m_ppkTearSplitPlanes, sizeof(NxVec3*) * m_uiNumStates);
    NIASSERT(m_ppkTearSplitPlanes);
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::RemoveState(const NiUInt32 uiState)
{
    if (uiState >= m_uiNumStates)
        return;

    // Store the one we are going to remove
    NxVec3* ppkVertexPosns = m_ppkVertexPosns[uiState];
    NiUInt16* ppuiTearIndices = m_ppuiTearIndices[uiState];
    NxVec3* ppkTearSplitPlanes = m_ppkTearSplitPlanes[uiState];
    
    // Shuffle down
    for (NiUInt32 ui = uiState; ui < m_uiNumStates - 1; ui++)
    {
        m_pkPoses[ui] = m_pkPoses[ui + 1];
        m_puiNumVertexPosns[ui] = m_puiNumVertexPosns[ui + 1];
        m_ppkVertexPosns[ui] = m_ppkVertexPosns[ui + 1];
        m_puiNumTearIndices[ui] = m_puiNumTearIndices[ui + 1];
        m_ppuiTearIndices[ui] = m_ppuiTearIndices[ui + 1];
        m_ppkTearSplitPlanes[ui] = m_ppkTearSplitPlanes[ui + 1];
    }
    
    // Put the data to remove at the end and remove that state
    m_ppkVertexPosns[m_uiNumStates - 1] = ppkVertexPosns;
    m_ppuiTearIndices[m_uiNumStates - 1] = ppuiTearIndices;
    m_ppkTearSplitPlanes[m_uiNumStates - 1] = ppkTearSplitPlanes;
    RemoveStatesBeyond(m_uiNumStates - 1);
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::PresizeStateArrays(const NiUInt32 uiRequiredCount)
{
    if (m_uiNumStates >= uiRequiredCount)
        return;
        
    m_pkPoses = (NxMat34*)NiRealloc(
        (void*)m_pkPoses, sizeof(NxMat34) * uiRequiredCount);
    NIASSERT(m_pkPoses);

    m_puiNumVertexPosns = (NiUInt16*)NiRealloc(
        (void*)m_puiNumVertexPosns, sizeof(NiUInt16) * uiRequiredCount);
    NIASSERT(m_puiNumVertexPosns);

    m_ppkVertexPosns = (NxVec3**)NiRealloc(
        (void*)m_ppkVertexPosns, sizeof(NxVec3*) * uiRequiredCount);
    NIASSERT(m_ppkVertexPosns);

    m_puiNumTearIndices = (NiUInt16*)NiRealloc(
        (void*)m_puiNumTearIndices, sizeof(NiUInt16) * uiRequiredCount);
    NIASSERT(m_puiNumTearIndices);

    m_ppuiTearIndices = (NiUInt16**)NiRealloc(
        (void*)m_ppuiTearIndices, sizeof(NiUInt16*) * uiRequiredCount);
    NIASSERT(m_ppuiTearIndices);

    m_ppkTearSplitPlanes = (NxVec3**)NiRealloc(
        (void*)m_ppkTearSplitPlanes, sizeof(NxVec3*) * uiRequiredCount);
    NIASSERT(m_ppkTearSplitPlanes);

    for (NiUInt32 ui = m_uiNumStates; ui < uiRequiredCount; ui++)
    {
        m_pkPoses[ui].M.id();
        m_pkPoses[ui].t.zero();
        m_puiNumVertexPosns[ui] = 0;
        m_ppkVertexPosns[ui] = 0;
        m_puiNumTearIndices[ui] = 0;
        m_ppuiTearIndices[ui] = 0;
        m_ppkTearSplitPlanes[ui] = 0;
    }

    m_uiNumStates = uiRequiredCount;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Interactions with PhysX
//---------------------------------------------------------------------------
bool NiPhysXClothDesc::IsValid()
{
    bool bResult;
    
    // Do this with keep meshes true. If the mesh already exists, won't be
    // an issue at all. If it doesn't, we might create it but we won't
    // throw the data away. That makes this a suitable function to call
    // from an exporter. If the mesh is to be loaded from file, it will have
    // to have been saved already.
    NxClothDesc kClothDesc;
    if (!ToClothDesc(kClothDesc, 0, NiPhysXTypes::NXMAT34_ID, true, false))
        return false;

    bResult = kClothDesc.isValid();
    
    return bResult;
}
//---------------------------------------------------------------------------
NxCloth* NiPhysXClothDesc::CreateCloth(NxScene* pkScene,
    NxCompartment* pkCompartment, const NiUInt32 uiIndex,
    const NxMat34& kXform, const bool bKeepMeshes)
{
    NxClothDesc kClothDesc;
    if (!ToClothDesc(kClothDesc, uiIndex, kXform, bKeepMeshes))
        return 0;
    
    kClothDesc.compartment = pkCompartment;
            
    m_pkCloth = pkScene->createCloth(kClothDesc);
    if (!m_pkCloth)
    {
#ifdef NIDEBUG
        NiOutputDebugString(
            "NiPhysXClothDesc::CreateCloth: Cloth creation failed.\n");
        NiOutputDebugString(
            "NiPhysXClothDesc::CreateCloth: Look for a message from the "
            "PhysX SDK as to why this might have occurred.\n");
#endif
        return 0;
    }
    
    if (uiIndex < m_uiNumStates)
    {
        // Tear all the vertices
        if (m_puiNumTearIndices[uiIndex])
        {
            for (NiUInt16 ui = 0; ui < m_puiNumTearIndices[uiIndex]; ui++)
            {
                m_pkCloth->tearVertex(m_ppuiTearIndices[uiIndex][ui],
                    kXform * m_ppkTearSplitPlanes[uiIndex][ui]);
            }
        }

        // Set the vertex positions. We have to push them into world space.
        if (m_puiNumVertexPosns[uiIndex])
        {
            NxVec3* pkXformedPosns =
                NiAlloc(NxVec3, m_puiNumVertexPosns[uiIndex]);
            for (NiUInt32 ui = 0; ui < m_puiNumVertexPosns[uiIndex]; ui++)
                pkXformedPosns[ui] = kXform * m_ppkVertexPosns[uiIndex][ui];
            m_pkCloth->setPositions((void*)pkXformedPosns);
            NiFree(pkXformedPosns);
        }
    }

    // Create attachments
    NiUInt32 uiCount = m_kAttachments.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment *pkAttachment = m_kAttachments.GetAt(ui);
        if (!pkAttachment)
            continue;
            
        if (pkAttachment->m_spShape)
        {
            NxShape* pkShape = pkAttachment->m_spShape->GetShape();
            
            NxMat34 kWorldPose = pkShape->getGlobalPose();
            NxMat34 kWorldInv;
            kWorldPose.getInverse(kWorldInv);
            
            NIASSERT(pkShape != 0);
            if (pkAttachment->m_uiVertexCount)
            {
                for (NiUInt32 uj = 0;
                    uj < pkAttachment->m_uiVertexCount; uj++)
                {
                    NxVec3 kPt = kWorldInv * pkAttachment->m_pkPositions[uj];
                    m_pkCloth->attachVertexToShape(
                        pkAttachment->m_puiVertexIDs[uj], pkShape, kPt,
                        pkAttachment->m_puiFlags[uj]);
                }
            }
            else
            {
                m_pkCloth->attachToShape(pkShape,
                    pkAttachment->m_puiFlags[0]);
            }
        }
        else
        {
            if (pkAttachment->m_uiVertexCount)
            {
                for (NiUInt32 uj = 0;
                    uj < pkAttachment->m_uiVertexCount; uj++)
                {
                    m_pkCloth->attachVertexToGlobalPosition(
                        pkAttachment->m_puiVertexIDs[uj],
                        pkAttachment->m_pkPositions[uj]);
                }
            }
            else
            {
                m_pkCloth->attachToCollidingShapes(
                    pkAttachment->m_puiFlags[0]);
            }
        }
    }
    
    if (m_pkTargetMesh)
    {
        NiPhysXClothModifier* pkClothModifier = (NiPhysXClothModifier*)
            m_pkTargetMesh->GetModifierByType(&NiPhysXClothModifier::ms_RTTI);
        if (pkClothModifier)
        {
            NiUInt16 usSize;
            const NiUInt16* pusVertexMap;
            m_spClothMesh->GetVertexMap(usSize, pusVertexMap);
            pkClothModifier->SetCloth(m_pkCloth, pusVertexMap, m_pkTargetMesh);
            if (m_pkParentActor)
            {
                pkClothModifier->SetParentActor(m_pkParentActor->GetActor());
            }
        }
    }

    return m_pkCloth;
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::RemoveCloth(NxScene* pkScene)
{
    // De-activate any destination.
    if (m_pkTargetMesh)
    {
        NiPhysXClothModifier* pkClothModifier = (NiPhysXClothModifier*)
            m_pkTargetMesh->GetModifierByType(&NiPhysXClothModifier::ms_RTTI);
        if (pkClothModifier)
        {
            pkClothModifier->SetCloth(0, 0, m_pkTargetMesh);
            pkClothModifier->SetParentActor(0);
        }
    }

    if (m_pkCloth)
    {
        pkScene->releaseCloth(*m_pkCloth);
        m_pkCloth = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXClothDesc);
//---------------------------------------------------------------------------
void NiPhysXClothDesc::CopyMembers(NiPhysXClothDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    // copy object names
    if (kCloning.m_eCopyType == NiObjectNET::COPY_EXACT)
    {
        pkDest->m_kClothName = m_kClothName;
    }
    else if (kCloning.m_eCopyType == NiObjectNET::COPY_UNIQUE)
    {
        if (m_kClothName)
        {
            size_t stQuantity = strlen(m_kClothName) + 2;
            char* pcNamePlus = NiAlloc(char, stQuantity);
            NIASSERT(pcNamePlus);
            NiStrcpy(pcNamePlus, stQuantity, m_kClothName);
            pcNamePlus[stQuantity - 2] = kCloning.m_cAppendChar;
            pcNamePlus[stQuantity - 1] = 0;
            pkDest->m_kClothName = pcNamePlus;
            NiFree(pcNamePlus);
        }
    }

    // Cloth mesh data is shared
    pkDest->m_spClothMesh = m_spClothMesh;
    
    pkDest->m_fThickness = m_fThickness;
    pkDest->m_fDensity = m_fDensity;
    pkDest->m_fBendingStiffness = m_fBendingStiffness;
    pkDest->m_fStretchingStiffness = m_fStretchingStiffness;
    pkDest->m_fDampingCoefficient = m_fDampingCoefficient;
    pkDest->m_fFriction = m_fFriction;
    pkDest->m_fPressure = m_fPressure;
    pkDest->m_fTearFactor = m_fTearFactor;
    pkDest->m_fCollisionResponseCoefficient = m_fCollisionResponseCoefficient;
    pkDest->m_fAttachmentResponseCoefficient =
        m_fAttachmentResponseCoefficient;
    pkDest->m_fAttachmentTearFactor = m_fAttachmentTearFactor;
    pkDest->m_fToFluidResponseCoefficient = m_fToFluidResponseCoefficient;
    pkDest->m_fFromFluidResponseCoefficient = m_fFromFluidResponseCoefficient;
    pkDest->m_fMinAdhereVelocity = m_fMinAdhereVelocity;
    pkDest->m_fRelativeGridSpacing = m_fRelativeGridSpacing;
    pkDest->m_uiSolverIterations = m_uiSolverIterations;
    pkDest->m_kExternalAcceleration = m_kExternalAcceleration;
    pkDest->m_kWindAcceleration = m_kWindAcceleration;
    pkDest->m_fWakeUpCounter = m_fWakeUpCounter;
    pkDest->m_fSleepLinearVelocity = m_fSleepLinearVelocity;
    pkDest->m_uiCollisionGroup = m_uiCollisionGroup;
    pkDest->m_auiCollisionBits[0] = m_auiCollisionBits[0];
    pkDest->m_auiCollisionBits[1] = m_auiCollisionBits[1];
    pkDest->m_auiCollisionBits[2] = m_auiCollisionBits[2];
    pkDest->m_auiCollisionBits[3] = m_auiCollisionBits[3];
    pkDest->m_uiForceFieldMaterial = m_uiForceFieldMaterial;
    pkDest->m_uiFlags = m_uiFlags;

    // Copy all saved states. This allows clones to have distinct initial
    // conditions, which is probably not necessary, but we have no good way
    // of sharing right now.
    CopyStates(pkDest);
    
    // Clone attachments
    NiUInt32 uiCount = m_kAttachments.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment* pkAttachment = m_kAttachments.GetAt(ui);
        if (!pkAttachment)
            continue;
        ClothAttachment* pkCopy = NiNew ClothAttachment;
        pkCopy->m_spShape = pkAttachment->m_spShape;
        pkCopy->m_uiVertexCount = pkAttachment->m_uiVertexCount;
        if (pkAttachment->m_uiVertexCount)
        {
            pkCopy->m_puiVertexIDs =
                NiAlloc(NiUInt32, pkCopy->m_uiVertexCount);
            pkCopy->m_pkPositions =
                NiAlloc(NxVec3, pkCopy->m_uiVertexCount);
            pkCopy->m_puiFlags =
                NiAlloc(NiUInt32, pkCopy->m_uiVertexCount);
            for (NiUInt32 uj = 0; uj < pkAttachment->m_uiVertexCount; uj++)
            {
                pkCopy->m_puiVertexIDs[uj] = pkAttachment->m_puiVertexIDs[uj];
                pkCopy->m_pkPositions[uj] = pkAttachment->m_pkPositions[uj];
                pkCopy->m_puiFlags[uj] = pkAttachment->m_puiFlags[uj];
            }
        }
        else
        {
            pkCopy->m_puiFlags = NiAlloc(NiUInt32, 1);
            pkCopy->m_puiFlags[0] = pkAttachment->m_puiFlags[0];
        }
        pkDest->m_kAttachments.Add(pkCopy);
    }

    pkDest->m_pkCloth = 0;
    pkDest->m_pkParentActor = m_pkParentActor;
    pkDest->m_pkTargetMesh = m_pkTargetMesh;
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXClothDesc* pkClone = (NiPhysXClothDesc*)pkCloneObj;

    // Check for clones of other objects.
    if (m_spClothMesh)
    {
        NiObject* pkClonedMeshObj = 0;
        bCloned = kCloning.m_pkCloneMap->
            GetAt(m_spClothMesh, pkClonedMeshObj);        
        if (bCloned)
        {
            pkClone->m_spClothMesh = (NiPhysXMeshDesc*)pkClonedMeshObj;
        }
    }

    NiUInt32 uiCount = pkClone->m_kAttachments.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment* pkAttachment = pkClone->m_kAttachments.GetAt(ui);
        if (!pkAttachment->m_spShape)
            continue;
        bCloned = kCloning.m_pkCloneMap->
            GetAt(pkAttachment->m_spShape, pkCloneObj);        
        if (bCloned)
        {
            pkAttachment->m_spShape = (NiPhysXShapeDesc*)pkCloneObj;
        }
    }

    if (m_pkParentActor)
    {
        NiObject* pkClonedParentObj = 0;
        bCloned = kCloning.m_pkCloneMap->
            GetAt(m_pkParentActor, pkClonedParentObj);        
        if (bCloned)
        {
            pkClone->m_pkParentActor = (NiPhysXActorDesc*)pkClonedParentObj;
        }
    }
    
    if (m_pkTargetMesh)
    {
        NiObject* pkClonedMeshObj = 0;
        bCloned = kCloning.m_pkCloneMap->GetAt(
            m_pkTargetMesh, pkClonedMeshObj);
        if (bCloned)
        {
            pkClone->m_pkTargetMesh = (NiMesh*)pkClonedMeshObj;
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::CopyStates(NiPhysXClothDesc* pkDest)
{
    if (m_uiNumStates == 0)
        return;
    
    pkDest->m_uiNumStates = m_uiNumStates;

    pkDest->m_pkPoses = NiAlloc(NxMat34, pkDest->m_uiNumStates);
    pkDest->m_puiNumVertexPosns = NiAlloc(NiUInt16, pkDest->m_uiNumStates);
    pkDest->m_ppkVertexPosns = NiAlloc(NxVec3*, pkDest->m_uiNumStates);
    pkDest->m_puiNumTearIndices = NiAlloc(NiUInt16, pkDest->m_uiNumStates);
    pkDest->m_ppuiTearIndices = NiAlloc(NiUInt16*, pkDest->m_uiNumStates);
    pkDest->m_ppkTearSplitPlanes = NiAlloc(NxVec3*, pkDest->m_uiNumStates);
    
    for (NiUInt32 ui = 0; ui < m_uiNumStates; ui++)
    {
        pkDest->m_pkPoses[ui] = m_pkPoses[ui];
        
        pkDest->m_puiNumVertexPosns[ui] = m_puiNumVertexPosns[ui];
        
        if (m_puiNumVertexPosns[ui])
        {
            size_t uiSize = m_puiNumVertexPosns[ui] * sizeof(NxVec3);
            pkDest->m_ppkVertexPosns[ui] = (NxVec3*)NiMalloc(uiSize);
            NiMemcpy(pkDest->m_ppkVertexPosns[ui], uiSize,
                m_ppkVertexPosns[ui], uiSize);
        }
        else
        {
            pkDest->m_ppkVertexPosns[ui] = 0;
        }

        pkDest->m_puiNumTearIndices[ui] = m_puiNumTearIndices[ui];
        
        if (m_puiNumTearIndices[ui])
        {
            size_t uiSize = m_puiNumTearIndices[ui] * sizeof(NiUInt16);
            pkDest->m_ppuiTearIndices[ui] = (NiUInt16*)NiMalloc(uiSize);
            NiMemcpy(pkDest->m_ppuiTearIndices[ui], uiSize,
                m_ppuiTearIndices[ui], uiSize);

            uiSize = m_puiNumTearIndices[ui] * sizeof(NxVec3);
            pkDest->m_ppkTearSplitPlanes[ui] = (NxVec3*)NiMalloc(uiSize);
            NiMemcpy(pkDest->m_ppkTearSplitPlanes[ui], uiSize,
                m_ppkTearSplitPlanes[ui], uiSize);
        }
        else
        {
            pkDest->m_ppuiTearIndices[ui] = 0;
            pkDest->m_ppkTearSplitPlanes[ui] = 0;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXClothDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    kStream.RegisterFixedString(m_kClothName);

    if (m_spClothMesh)
        m_spClothMesh->RegisterStreamables(kStream);
        
    NiUInt32 uiCount = m_kAttachments.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment* pkAttachment = m_kAttachments.GetAt(ui);
        if (pkAttachment && pkAttachment->m_spShape)
            pkAttachment->m_spShape->RegisterStreamables(kStream);
    }

    if (m_pkParentActor)
        m_pkParentActor->RegisterStreamables(kStream);

    if (m_pkTargetMesh)
        m_pkTargetMesh->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    kStream.SaveFixedString(m_kClothName);

    kStream.SaveLinkID(m_spClothMesh);
    
    NiStreamSaveBinary(kStream, m_fThickness);
    NiStreamSaveBinary(kStream, m_fDensity);
    NiStreamSaveBinary(kStream, m_fBendingStiffness);
    NiStreamSaveBinary(kStream, m_fStretchingStiffness);
    NiStreamSaveBinary(kStream, m_fDampingCoefficient);
    NiStreamSaveBinary(kStream, m_fFriction);
    NiStreamSaveBinary(kStream, m_fPressure);
    NiStreamSaveBinary(kStream, m_fTearFactor);
    NiStreamSaveBinary(kStream, m_fCollisionResponseCoefficient);
    NiStreamSaveBinary(kStream, m_fAttachmentResponseCoefficient);
    NiStreamSaveBinary(kStream, m_fAttachmentTearFactor);
    NiStreamSaveBinary(kStream, m_fToFluidResponseCoefficient);
    NiStreamSaveBinary(kStream, m_fFromFluidResponseCoefficient);
    NiStreamSaveBinary(kStream, m_fMinAdhereVelocity);
    NiStreamSaveBinary(kStream, m_fRelativeGridSpacing);
    NiStreamSaveBinary(kStream, m_uiSolverIterations);
    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kExternalAcceleration);
    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kWindAcceleration);
    NiStreamSaveBinary(kStream, m_fWakeUpCounter);
    NiStreamSaveBinary(kStream, m_fSleepLinearVelocity);
    NiStreamSaveBinary(kStream, m_uiCollisionGroup);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[0]);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[1]);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[2]);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[3]);
    NiStreamSaveBinary(kStream, m_uiForceFieldMaterial);
    NiStreamSaveBinary(kStream, m_uiFlags);

    // Save states
    NiStreamSaveBinary(kStream, m_uiNumStates);
    for (NiUInt32 ui = 0; ui < m_uiNumStates; ui++)
    {
        NiPhysXTypes::NxMat34SaveBinary(kStream, m_pkPoses[ui]);

        NiStreamSaveBinary(kStream, m_puiNumVertexPosns[ui]);
        if (m_puiNumVertexPosns[ui])
        {
            NiStreamSaveBinary(kStream, m_ppkVertexPosns[ui],
                m_puiNumVertexPosns[ui]);
        }
        NiStreamSaveBinary(kStream, m_puiNumTearIndices[ui]);
        if (m_puiNumTearIndices[ui])
        {
            NiStreamSaveBinary(kStream, m_ppuiTearIndices[ui],
                m_puiNumTearIndices[ui]);
            NiStreamSaveBinary(kStream, m_ppkTearSplitPlanes[ui],
                m_puiNumTearIndices[ui]);
        }
    }

    // Save attachments
    NiUInt32 uiCount = m_kAttachments.GetSize();
    NiStreamSaveBinary(kStream, m_kAttachments.GetEffectiveSize());
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment* pkAttachment = m_kAttachments.GetAt(ui);
        if (!pkAttachment)
            continue;
        kStream.SaveLinkID(pkAttachment->m_spShape);
        NiStreamSaveBinary(kStream, pkAttachment->m_uiVertexCount);
        for (NiUInt32 uj = 0; uj < pkAttachment->m_uiVertexCount; uj++)
        {
            NiStreamSaveBinary(kStream, pkAttachment->m_puiVertexIDs[uj]);
            NiPhysXTypes::NxVec3SaveBinary(kStream,
                pkAttachment->m_pkPositions[uj]);
            NiStreamSaveBinary(kStream, pkAttachment->m_puiFlags[uj]);
        }
        if (pkAttachment->m_uiVertexCount == 0)
            NiStreamSaveBinary(kStream, pkAttachment->m_puiFlags[0]);
    }

    kStream.SaveLinkID(m_pkParentActor);

    kStream.SaveLinkID(m_pkTargetMesh);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXClothDesc);
//---------------------------------------------------------------------------
void NiPhysXClothDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.LoadFixedString(m_kClothName);

    m_spClothMesh = (NiPhysXMeshDesc*)kStream.ResolveLinkID();
    
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 0))
    {
        PresizeStateArrays(1);
        NiPhysXTypes::NxMat34LoadBinary(kStream, m_pkPoses[0]);
    }

    NiStreamLoadBinary(kStream, m_fThickness);
    NiStreamLoadBinary(kStream, m_fDensity);
    NiStreamLoadBinary(kStream, m_fBendingStiffness);
    NiStreamLoadBinary(kStream, m_fStretchingStiffness);
    NiStreamLoadBinary(kStream, m_fDampingCoefficient);
    NiStreamLoadBinary(kStream, m_fFriction);
    NiStreamLoadBinary(kStream, m_fPressure);
    NiStreamLoadBinary(kStream, m_fTearFactor);
    NiStreamLoadBinary(kStream, m_fCollisionResponseCoefficient);
    NiStreamLoadBinary(kStream, m_fAttachmentResponseCoefficient);
    NiStreamLoadBinary(kStream, m_fAttachmentTearFactor);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_fToFluidResponseCoefficient);
        NiStreamLoadBinary(kStream, m_fFromFluidResponseCoefficient);
        NiStreamLoadBinary(kStream, m_fMinAdhereVelocity);
        NiStreamLoadBinary(kStream, m_fRelativeGridSpacing);
    }

    NiStreamLoadBinary(kStream, m_uiSolverIterations);
    NiPhysXTypes::NxVec3LoadBinary(kStream, m_kExternalAcceleration);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_kWindAcceleration);
    }
    NiStreamLoadBinary(kStream, m_fWakeUpCounter);
    NiStreamLoadBinary(kStream, m_fSleepLinearVelocity);
    NiStreamLoadBinary(kStream, m_uiCollisionGroup);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[0]);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[1]);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[2]);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[3]);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_uiForceFieldMaterial);
    }
    NiStreamLoadBinary(kStream, m_uiFlags);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        // Load states
        NiUInt32 uiNumStates = 0;
        NiStreamLoadBinary(kStream, uiNumStates);
        PresizeStateArrays(uiNumStates);
        for (NiUInt32 ui = 0; ui < m_uiNumStates; ui++)
        {
            NiPhysXTypes::NxMat34LoadBinary(kStream, m_pkPoses[ui]);

            NiStreamLoadBinary(kStream, m_puiNumVertexPosns[ui]);
            if (m_puiNumVertexPosns[ui])
            {
                size_t uiSize = m_puiNumVertexPosns[ui] * sizeof(NxVec3);
                m_ppkVertexPosns[ui] = (NxVec3*)NiMalloc(uiSize);
                NiStreamLoadBinary(kStream, m_ppkVertexPosns[ui],
                    m_puiNumVertexPosns[ui]);
            }
            else
            {
                m_ppkVertexPosns[ui] = 0;
            }

            NiStreamLoadBinary(kStream, m_puiNumTearIndices[ui]);
            if (m_puiNumTearIndices[ui])
            {
                size_t uiSize = m_puiNumTearIndices[ui] * sizeof(NiUInt16);
                m_ppuiTearIndices[ui] = (NiUInt16*)NiMalloc(uiSize);
                NiStreamLoadBinary(kStream, m_ppuiTearIndices[ui],
                    m_puiNumTearIndices[ui]);

                uiSize = m_puiNumTearIndices[ui] * sizeof(NxVec3);
                m_ppkTearSplitPlanes[ui] = (NxVec3*)NiMalloc(uiSize);
                NiStreamLoadBinary(kStream, m_ppkTearSplitPlanes[ui],
                    m_puiNumTearIndices[ui]);
            }
            else
            {
                m_ppuiTearIndices[ui] = 0;
                m_ppkTearSplitPlanes[ui] = 0;
            }
        }
    }

    NiUInt32 uiCount;
    NiStreamLoadBinary(kStream, uiCount);
    m_kAttachments.SetSize(uiCount);
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment* pkAttachment = NiNew ClothAttachment;
        pkAttachment->m_spShape = (NiPhysXShapeDesc*)kStream.ResolveLinkID();
        NiStreamLoadBinary(kStream, pkAttachment->m_uiVertexCount);
        if (pkAttachment->m_uiVertexCount == 0)
        {
            pkAttachment->m_puiFlags = NiAlloc(NiUInt32, 1);
            NiStreamLoadBinary(kStream, pkAttachment->m_puiFlags[0]);
        }
        else
        {
            pkAttachment->m_puiVertexIDs =
                NiAlloc(NiUInt32, pkAttachment->m_uiVertexCount);
            pkAttachment->m_pkPositions =
                NiAlloc(NxVec3, pkAttachment->m_uiVertexCount);
            pkAttachment->m_puiFlags =
                NiAlloc(NiUInt32, pkAttachment->m_uiVertexCount);
            for (NiUInt32 uj = 0; uj < pkAttachment->m_uiVertexCount; uj++)
            {
                NiStreamLoadBinary(kStream, pkAttachment->m_puiVertexIDs[uj]);
                NiPhysXTypes::NxVec3LoadBinary(kStream,
                    pkAttachment->m_pkPositions[uj]);
                NiStreamLoadBinary(kStream, pkAttachment->m_puiFlags[uj]);
            }
        }

        m_kAttachments.SetAt(ui, pkAttachment);
    }

    m_pkParentActor = (NiPhysXActorDesc*)kStream.ResolveLinkID();
   
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 0))
    {
        m_spDest = (NiPhysXClothDest*)kStream.ResolveLinkID();
    }
    else
    {
        m_pkTargetMesh = (NiMesh*)kStream.ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXClothDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXClothDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXClothDesc* pkNewObject = (NiPhysXClothDesc*)pkObject;
    
    if (m_kClothName != pkNewObject->m_kClothName)
        return false;
    
    if (m_spClothMesh)
    {
        if (!pkNewObject->m_spClothMesh ||
            !m_spClothMesh->IsEqual(pkNewObject->m_spClothMesh))
        return false;
    }
    else
    {
        if (pkNewObject->m_spClothMesh)
            return false;
    }

    NxVec3 kDiffA =
        pkNewObject->m_kExternalAcceleration - m_kExternalAcceleration;
    if (kDiffA.magnitudeSquared() > 1.0e-8f)
        return false;

    NxVec3 kDiffWA =
        pkNewObject->m_kWindAcceleration - m_kWindAcceleration;
    if (kDiffWA.magnitudeSquared() > 1.0e-8f)
        return false;

    if (pkNewObject->m_fThickness != m_fThickness ||
        pkNewObject->m_fDensity != m_fDensity ||
        pkNewObject->m_fBendingStiffness != m_fBendingStiffness ||
        pkNewObject->m_fStretchingStiffness != m_fStretchingStiffness ||
        pkNewObject->m_fDampingCoefficient != m_fDampingCoefficient ||
        pkNewObject->m_fFriction != m_fFriction ||
        pkNewObject->m_fPressure != m_fPressure ||
        pkNewObject->m_fTearFactor != m_fTearFactor ||
        pkNewObject->m_fCollisionResponseCoefficient
            != m_fCollisionResponseCoefficient ||
        pkNewObject->m_fAttachmentResponseCoefficient !=
            m_fAttachmentResponseCoefficient ||
        pkNewObject->m_fAttachmentTearFactor != m_fAttachmentTearFactor ||
        pkNewObject->m_fToFluidResponseCoefficient != 
            m_fToFluidResponseCoefficient ||
        pkNewObject->m_fFromFluidResponseCoefficient != 
            m_fFromFluidResponseCoefficient ||
        pkNewObject->m_fMinAdhereVelocity != m_fMinAdhereVelocity ||
        pkNewObject->m_fRelativeGridSpacing != m_fRelativeGridSpacing ||
        pkNewObject->m_uiSolverIterations != m_uiSolverIterations ||
        pkNewObject->m_fWakeUpCounter != m_fWakeUpCounter ||
        pkNewObject->m_fSleepLinearVelocity != m_fSleepLinearVelocity ||
        pkNewObject->m_uiCollisionGroup != m_uiCollisionGroup ||
        pkNewObject->m_auiCollisionBits[0] != m_auiCollisionBits[0] ||
        pkNewObject->m_auiCollisionBits[1] != m_auiCollisionBits[1] ||
        pkNewObject->m_auiCollisionBits[2] != m_auiCollisionBits[2] ||
        pkNewObject->m_auiCollisionBits[3] != m_auiCollisionBits[3] ||
        pkNewObject->m_uiForceFieldMaterial != m_uiForceFieldMaterial ||
        pkNewObject->m_uiFlags != m_uiFlags)
        return false;


    if (pkNewObject->m_uiNumStates != m_uiNumStates)
        return false;
    for (NiUInt32 ui = 0 ; ui < m_uiNumStates; ui++)
    {            
        NxVec3 kDiffT = pkNewObject->m_pkPoses[ui].t - m_pkPoses[ui].t;
        if (kDiffT.magnitudeSquared() > 1.0e-8f)
            return false;
        NxMat33 kDiffM = pkNewObject->m_pkPoses[ui].M - m_pkPoses[ui].M;
        if (NiAbs(kDiffM.determinant() > 0.0001f))
            return false;
        
        if (m_puiNumVertexPosns[ui] != pkNewObject->m_puiNumVertexPosns[ui])
            return false;
        for (NiUInt32 uj = 0; uj < m_puiNumVertexPosns[ui]; uj++)
        {
            kDiffT = m_ppkVertexPosns[ui][uj] -
                pkNewObject->m_ppkVertexPosns[ui][uj];
            if (kDiffT.magnitudeSquared() > 1.0e-8f)
                return false;
        }
        
        if (m_puiNumTearIndices[ui] != pkNewObject->m_puiNumTearIndices[ui])
            return false;
        for (NiUInt32 uj = 0; uj < m_puiNumTearIndices[ui]; uj++)
        {
            if (m_ppuiTearIndices[ui][uj] !=
                pkNewObject->m_ppuiTearIndices[ui][uj])
            {
                return false;
            }
                
            kDiffT = m_ppkTearSplitPlanes[ui][uj] -
                pkNewObject->m_ppkTearSplitPlanes[ui][uj];
            if (kDiffT.magnitudeSquared() > 1.0e-8f)
                return false;
        }
    }
        
    NiUInt32 uiSizeThis = m_kAttachments.GetSize(); 
    NiUInt32 uiSizeThat = pkNewObject->m_kAttachments.GetSize();
    NiUInt32 uiThis = 0;
    NiUInt32 uiThat = 0;
    bool bDone = false;
    do
    {
        ClothAttachment* pkAttachmentThis = NULL;
        while (uiThis < uiSizeThis)
        {
            pkAttachmentThis = m_kAttachments.GetAt(uiThis);
            if (pkAttachmentThis)
                break;
            uiThis++;
        }

        ClothAttachment* pkAttachmentThat = NULL;
        while (uiThat < uiSizeThat)
        {
            pkAttachmentThat = pkNewObject->m_kAttachments.GetAt(uiThat);
            if (pkAttachmentThat)
                break;
            uiThat++;
        }

        if (uiThis == uiSizeThis && uiThat == uiSizeThat)
            bDone = true; // All is good
        else if (uiThis == uiSizeThis && uiThat != uiSizeThat)
            return false; // More systems in That than in This
        else if (uiThis != uiSizeThis && uiThat == uiSizeThat)
            return false; // More systems in This than in That
        else
        {
            if (pkAttachmentThis->m_spShape)
            {
                if (!pkAttachmentThat->m_spShape ||
                    !pkAttachmentThis->m_spShape->
                        IsEqual(pkAttachmentThat->m_spShape))
                {
                    return false;
                }
            }
            else if (pkAttachmentThat->m_spShape)
            {
                return false;
            }
            
            if (pkAttachmentThis->m_uiVertexCount !=
                pkAttachmentThat->m_uiVertexCount)
                return false;
            
            for (NiUInt32 ui = 0; ui < pkAttachmentThis->m_uiVertexCount;
                ui++)
            {
                NxVec3 kDiffP = pkAttachmentThat->m_pkPositions[ui]
                     - pkAttachmentThis->m_pkPositions[ui];
                if (kDiffP.magnitudeSquared() > 1.0e-8f)
                    return false;
                
                if (pkAttachmentThis->m_puiVertexIDs[ui] !=
                    pkAttachmentThat->m_puiVertexIDs[ui] ||
                    pkAttachmentThis->m_puiFlags[ui] !=
                    pkAttachmentThat->m_puiFlags[ui])
                {
                    return false;
                }
            }
            
            if (pkAttachmentThis->m_uiVertexCount == 0)
            {
                if (pkAttachmentThis->m_puiFlags[0] !=
                    pkAttachmentThat->m_puiFlags[0])
                {
                    return false;
                }
            }
        }    
        uiThis++;
        uiThat++;
    } while (!bDone);

    if (m_pkParentActor)
    {
        if (!pkNewObject->m_pkParentActor ||
            !m_pkParentActor->IsEqual(pkNewObject->m_pkParentActor))
        return false;
    }
    else
    {
        if (pkNewObject->m_pkParentActor)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiPhysXClothDesc::ClothAttachment::ClothAttachment()
{
    m_spShape = 0;
    m_uiVertexCount = 0;
    m_puiVertexIDs = 0;    
    m_pkPositions = 0;
    m_puiFlags = 0;
}
//---------------------------------------------------------------------------
NiPhysXClothDesc::ClothAttachment::~ClothAttachment()
{
    m_spShape = 0;
    NiFree(m_puiVertexIDs);
    NiFree(m_pkPositions);
    NiFree(m_puiFlags);
}
//---------------------------------------------------------------------------

