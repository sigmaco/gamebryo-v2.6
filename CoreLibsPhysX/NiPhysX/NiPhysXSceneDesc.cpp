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

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

NiImplementRTTI(NiPhysXSceneDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXSceneDesc::NiPhysXSceneDesc() : m_kCompartmentMap(7)
{
    m_kNxSceneDesc.setToDefault();
    
    for (NiUInt32 i = 0; i < 32; i++)
        for (NiUInt32 j = 0; j < 32; j++)
            m_abGroupCollisionFlags[i][j] = true;
 
    m_aeFilterOps[0] = NX_FILTEROP_OR;
    m_aeFilterOps[1] = NX_FILTEROP_OR;
    m_aeFilterOps[2] = NX_FILTEROP_AND;
    m_auiFilterConstants[0].bits0 = 0;
    m_auiFilterConstants[0].bits1 = 0;
    m_auiFilterConstants[0].bits2 = 0;
    m_auiFilterConstants[0].bits3 = 0;
    m_auiFilterConstants[1].bits0 = 0;
    m_auiFilterConstants[1].bits1 = 0;
    m_auiFilterConstants[1].bits2 = 0;
    m_auiFilterConstants[1].bits3 = 0;    
    m_bFilterBool = true;
}
//---------------------------------------------------------------------------
NiPhysXSceneDesc::~NiPhysXSceneDesc()
{
    NiFree(m_kNxSceneDesc.maxBounds);
    NiFree(m_kNxSceneDesc.limits);
    
    NiTMapIterator kIter = m_kCompartmentMap.GetFirstPos();
    while (kIter)
    {
        NiUInt32 uiID;
        NxCompartmentDesc* pkDesc;
        m_kCompartmentMap.GetNext(kIter, uiID, pkDesc);
        NiFree(pkDesc);
    }
    m_kCompartmentMap.RemoveAll();
    
    m_kActorsBackCompatable.RemoveAll();
    m_kJointsBackCompatable.RemoveAll();
    m_kMaterialsBackCompatable.RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXSceneDesc::SetNxSceneDesc(const NxSceneDesc& kSceneDesc)
{
    NiFree(m_kNxSceneDesc.maxBounds);
    NiFree(m_kNxSceneDesc.limits);

    m_kNxSceneDesc = kSceneDesc;

    if (kSceneDesc.maxBounds)
    {
        m_kNxSceneDesc.maxBounds = NiAlloc(NxBounds3, 1);
        *m_kNxSceneDesc.maxBounds = *kSceneDesc.maxBounds;
    }
    else
    {       
        m_kNxSceneDesc.maxBounds = 0;
    }
    if (kSceneDesc.limits)
    {
        m_kNxSceneDesc.limits = NiAlloc(NxSceneLimits, 1);
        *m_kNxSceneDesc.limits = *kSceneDesc.limits;
    }
    else
    {       
        m_kNxSceneDesc.limits = 0;
    }
}
//---------------------------------------------------------------------------
NxScene* NiPhysXSceneDesc::CreateScene()
{
    // Create the scene from a descriptor. Does not set up compartments.
    NxScene* pkScene;
    pkScene = NiPhysXManager::GetPhysXManager()->m_pkPhysXSDK->
        createScene(m_kNxSceneDesc);
    return pkScene;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXSceneDesc);
//---------------------------------------------------------------------------
void NiPhysXSceneDesc::CopyMembers(NiPhysXSceneDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->SetNxSceneDesc(m_kNxSceneDesc);
    
    for (NiUInt32 i = 0; i < 32; i++)
    {
        for (NiUInt32 j = 0; j < 32; j++)
        {
            pkDest->m_abGroupCollisionFlags[i][j] =
                m_abGroupCollisionFlags[i][j];
        }
    }
    
    pkDest->m_aeFilterOps[0] = m_aeFilterOps[0];
    pkDest->m_aeFilterOps[1] = m_aeFilterOps[1];
    pkDest->m_aeFilterOps[2] = m_aeFilterOps[2];
    pkDest->m_bFilterBool = m_bFilterBool;
    pkDest->m_auiFilterConstants[0] = m_auiFilterConstants[0];
    pkDest->m_auiFilterConstants[1] = m_auiFilterConstants[1];
    
    NiTMapIterator kIter = m_kCompartmentMap.GetFirstPos();
    while (kIter)
    {
        NiUInt32 uiID;
        NxCompartmentDesc* pkDesc;
        m_kCompartmentMap.GetNext(kIter, uiID, pkDesc);
        pkDest->m_kCompartmentMap.SetAt(uiID, pkDesc);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXSceneDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXSceneDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kNxSceneDesc.gravity);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.maxTimestep);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.maxIter);
    NiStreamSaveEnum(kStream, m_kNxSceneDesc.timeStepMethod);
    if (m_kNxSceneDesc.maxBounds)
    {
        NiStreamSaveBinary(kStream, NiBool(true));
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_kNxSceneDesc.maxBounds->min);
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_kNxSceneDesc.maxBounds->max);
    }
    else
    {
        NiStreamSaveBinary(kStream, NiBool(false));
    }
    if (m_kNxSceneDesc.limits)
    {
        NiStreamSaveBinary(kStream, NiBool(true));
        NiStreamSaveBinary(kStream, *(m_kNxSceneDesc.limits));
    }
    else
    {
        NiStreamSaveBinary(kStream, NiBool(false));
    }
    NiStreamSaveEnum(kStream, m_kNxSceneDesc.simType);
    NiStreamSaveBinary(kStream, NiBool(m_kNxSceneDesc.groundPlane));
    NiStreamSaveBinary(kStream, NiBool(m_kNxSceneDesc.boundsPlanes));

    NiStreamSaveBinary(kStream, m_kNxSceneDesc.flags);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.internalThreadCount);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.backgroundThreadCount);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.backgroundThreadPriority);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.threadMask);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.backgroundThreadMask);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.simThreadStackSize);
    NiStreamSaveEnum(kStream, m_kNxSceneDesc.simThreadPriority);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.workerThreadStackSize);
    NiStreamSaveEnum(kStream, m_kNxSceneDesc.workerThreadPriority);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.upAxis);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.subdivisionLevel);
    NiStreamSaveEnum(kStream, m_kNxSceneDesc.staticStructure);
    NiStreamSaveEnum(kStream, m_kNxSceneDesc.dynamicStructure);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.dynamicTreeRebuildRateHint);
    NiStreamSaveEnum(kStream, m_kNxSceneDesc.bpType);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.nbGridCellsX);
    NiStreamSaveBinary(kStream, m_kNxSceneDesc.nbGridCellsY);
    
    for (NiUInt32 i = 0; i < 32; i++)
        for (NiUInt32 j = 0; j < 32; j++)
            NiStreamSaveBinary(kStream, NiBool(m_abGroupCollisionFlags[i][j]));
 
    NiStreamSaveBinary(kStream, m_aeFilterOps[0]);
    NiStreamSaveBinary(kStream, m_aeFilterOps[1]);
    NiStreamSaveBinary(kStream, m_aeFilterOps[2]);
    NiStreamSaveBinary(kStream, m_auiFilterConstants[0].bits0);
    NiStreamSaveBinary(kStream, m_auiFilterConstants[0].bits1);
    NiStreamSaveBinary(kStream, m_auiFilterConstants[0].bits2);
    NiStreamSaveBinary(kStream, m_auiFilterConstants[0].bits3);
    NiStreamSaveBinary(kStream, m_auiFilterConstants[1].bits0);
    NiStreamSaveBinary(kStream, m_auiFilterConstants[1].bits1);
    NiStreamSaveBinary(kStream, m_auiFilterConstants[1].bits2);
    NiStreamSaveBinary(kStream, m_auiFilterConstants[1].bits3);
    NiStreamSaveBinary(kStream, NiBool(m_bFilterBool));
    
    NiStreamSaveBinary(kStream, m_kCompartmentMap.GetCount());
    NiTMapIterator kIter = m_kCompartmentMap.GetFirstPos();
    while (kIter)
    {
        NiUInt32 uiID;
        NxCompartmentDesc* pkDesc;
        m_kCompartmentMap.GetNext(kIter, uiID, pkDesc);
        
        NiStreamSaveBinary(kStream, uiID);
        NiStreamSaveEnum(kStream, pkDesc->type);
        NiStreamSaveBinary(kStream, pkDesc->deviceCode);
        NiStreamSaveBinary(kStream, pkDesc->gridHashCellSize);
        NiStreamSaveBinary(kStream, pkDesc->gridHashTablePower);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXSceneDesc);
//---------------------------------------------------------------------------
void NiPhysXSceneDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
    
    NiBool bVal;

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 8))
    {
        // Stream in the m_kNxSceneDesc.broadphase from PhysX 2.3.2
        // which was streamed in Gamebryo version 2.2 and earlier.
        NiSceneDescNxBroadPhaseType kBF;
        NiStreamLoadBinary(kStream, kBF);
    }
    
    NiPhysXTypes::NxVec3LoadBinary(kStream, m_kNxSceneDesc.gravity);
    NiStreamLoadBinary(kStream, m_kNxSceneDesc.maxTimestep);
    NiStreamLoadBinary(kStream, m_kNxSceneDesc.maxIter);
    NiStreamLoadEnum(kStream, m_kNxSceneDesc.timeStepMethod);
    NiStreamLoadBinary(kStream, bVal);
    if (bVal)
    {
        m_kNxSceneDesc.maxBounds = NiAlloc(NxBounds3, 1);
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_kNxSceneDesc.maxBounds->min);
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_kNxSceneDesc.maxBounds->max);
    }
    else
    {
        m_kNxSceneDesc.maxBounds = 0;
    }
    NiStreamLoadBinary(kStream, bVal);
    if (bVal)
    {
        m_kNxSceneDesc.limits = NiAlloc(NxSceneLimits, 1);
        NiStreamLoadBinary(kStream, *(m_kNxSceneDesc.limits));
    }
    else
    {
        m_kNxSceneDesc.limits = 0;
    }
    NiStreamLoadEnum(kStream, m_kNxSceneDesc.simType);
    
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 0))
    {
        NiSceneDescNxHwSceneType kHWST;
        NiSceneDescNxHwPipelineSpec kHWPS;
        NiStreamLoadEnum(kStream, kHWST);
        NiStreamLoadEnum(kStream, kHWPS);
    }
    
    NiStreamLoadBinary(kStream, bVal);
    m_kNxSceneDesc.groundPlane = bVal ? true : false;
    NiStreamLoadBinary(kStream, bVal);
    m_kNxSceneDesc.boundsPlanes = bVal ? true : false;

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 8))
    {
        // Stream in the m_kNxSceneDesc.collisionDetection from PhysX 2.3.2
        // which was streamed in Gamebryo version 2.2 and earlier.
        NiStreamLoadBinary(kStream, bVal);
    }
    else if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 0))
    {
        // Gamebryo 2.2.1 and earlier didn't have the hardware scene manager
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.flags);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.internalThreadCount);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadCount);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.threadMask);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadMask);
    }
    else if (kStream.GetFileVersion() == NiStream::GetVersion(20, 3, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.flags);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.internalThreadCount);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadCount);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.threadMask);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadMask);
        
        // Have to deal with back compatable managed hardware scenes
        NiUInt32 uiNumHWScenes;
        NiStreamLoadBinary(kStream, uiNumHWScenes);
        for (NiUInt32 ui = 1; ui <= uiNumHWScenes; ui++)
        {
            NxCompartmentDesc* pkCompartment = NiAlloc(NxCompartmentDesc, 1);
            pkCompartment->type = NX_SCT_RIGIDBODY;
            m_kCompartmentMap.SetAt(ui, pkCompartment);
        }
    }
    else if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 1)
        && kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 6))
    {
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.flags);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.internalThreadCount);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadCount);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.threadMask);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadMask);
        
        // Have to deal with back compatable managed hardware scenes
        NiUInt32 uiNumHWScenes;
        NiStreamLoadBinary(kStream, uiNumHWScenes);
        for (NiUInt32 ui = 1; ui <= uiNumHWScenes; ui++)
        {
            NxCompartmentDesc* pkCompartment = NiAlloc(NxCompartmentDesc, 1);
            pkCompartment->type = NX_SCT_RIGIDBODY;
            m_kCompartmentMap.SetAt(ui, pkCompartment);
        }

        NiStreamLoadBinary(kStream, m_kNxSceneDesc.simThreadStackSize);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.simThreadPriority);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.workerThreadStackSize);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.workerThreadPriority);
    }
    else if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 6)
        && kStream.GetFileVersion() < NiStream::GetVersion(20, 5, 0, 3))
    {
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.flags);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.internalThreadCount);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadCount);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.threadMask);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadMask);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.simThreadStackSize);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.simThreadPriority);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.workerThreadStackSize);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.workerThreadPriority);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.upAxis);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.subdivisionLevel);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.staticStructure);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.dynamicStructure);
    }
    else if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 5, 0, 3))
    {
        //Gamebryo 2.6 adds backgroundThreadPriority and dynamicTreeRebuildRateHint
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.flags);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.internalThreadCount);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadCount);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadPriority);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.threadMask);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.backgroundThreadMask);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.simThreadStackSize);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.simThreadPriority);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.workerThreadStackSize);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.workerThreadPriority);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.upAxis);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.subdivisionLevel);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.staticStructure);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.dynamicStructure);
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.dynamicTreeRebuildRateHint);
    }
    else
    {
        NIASSERT(false && "What version of NIF file are we loading?");
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadEnum(kStream, m_kNxSceneDesc.bpType);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.nbGridCellsX);
        NiStreamLoadBinary(kStream, m_kNxSceneDesc.nbGridCellsY);
    }
    
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 2))
    {
        NiUInt32 uiSize;
        NiStreamLoadBinary(kStream, uiSize);
        m_kActorsBackCompatable.SetSize(uiSize);
        for (NiUInt32 i = 0; i < uiSize; i++)
        {
            m_kActorsBackCompatable.SetAt(i,
                (NiPhysXActorDesc*)kStream.ResolveLinkID());
        }
        NiStreamLoadBinary(kStream, uiSize);
        m_kJointsBackCompatable.SetSize(uiSize);
        for (NiUInt32 i = 0; i < uiSize; i++)
        {
            m_kJointsBackCompatable.SetAt(i,
                (NiPhysXJointDesc*)kStream.ResolveLinkID());
        }
        NiStreamLoadBinary(kStream, uiSize);
        for (NiUInt32 i = 0; i < uiSize; i++)
        {
            NiUInt16 usKey;
            NiStreamLoadBinary(kStream, usKey);
            NiPhysXMaterialDescPtr spMat =
                (NiPhysXMaterialDesc*)kStream.ResolveLinkID();
            m_kMaterialsBackCompatable.SetAt(usKey, spMat);
        }
    }
    
    for (NiUInt32 i = 0; i < 32; i++)
    {
        for (NiUInt32 j = 0; j < 32; j++)
        {
            if (kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 8))
            {
                NiStreamLoadBinary(kStream, m_abGroupCollisionFlags[i][j]);
            }
            else
            {
                NiStreamLoadBinary(kStream, bVal);
                m_abGroupCollisionFlags[i][j] = bVal ? true : false;
            }
        }
    }
 
    NiStreamLoadBinary(kStream, m_aeFilterOps[0]);
    NiStreamLoadBinary(kStream, m_aeFilterOps[1]);
    NiStreamLoadBinary(kStream, m_aeFilterOps[2]);
    NiStreamLoadBinary(kStream, m_auiFilterConstants[0].bits0);
    NiStreamLoadBinary(kStream, m_auiFilterConstants[0].bits1);
    NiStreamLoadBinary(kStream, m_auiFilterConstants[0].bits2);
    NiStreamLoadBinary(kStream, m_auiFilterConstants[0].bits3);
    NiStreamLoadBinary(kStream, m_auiFilterConstants[1].bits0);
    NiStreamLoadBinary(kStream, m_auiFilterConstants[1].bits1);
    NiStreamLoadBinary(kStream, m_auiFilterConstants[1].bits2);
    NiStreamLoadBinary(kStream, m_auiFilterConstants[1].bits3);
    NiStreamLoadBinary(kStream, bVal);
    m_bFilterBool = bVal ? true : false;
    
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 2))
        NiStreamLoadBinary(kStream, m_uiNumStatesBackCompatable);
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 6))
    {
        NiUInt32 uiCount;
        NiStreamLoadBinary(kStream, uiCount);
        for (NiUInt32 ui = 0; ui < uiCount; ui++)
        {
            NiUInt32 uiID;
            NiStreamLoadBinary(kStream, uiID);
            
            NxCompartmentDesc* pkDesc = NiAlloc(NxCompartmentDesc, 1);
            NiStreamLoadEnum(kStream, pkDesc->type);
            NiStreamLoadBinary(kStream, pkDesc->deviceCode);
            NiStreamLoadBinary(kStream, pkDesc->gridHashCellSize);
            NiStreamLoadBinary(kStream, pkDesc->gridHashTablePower);
            m_kCompartmentMap.SetAt(uiID, pkDesc);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXSceneDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXSceneDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXSceneDesc* pkNewObject = (NiPhysXSceneDesc*)pkObject;

    if (m_kNxSceneDesc.maxTimestep != 
            pkNewObject->m_kNxSceneDesc.maxTimestep ||
        m_kNxSceneDesc.maxIter != pkNewObject->m_kNxSceneDesc.maxIter ||
        m_kNxSceneDesc.timeStepMethod !=
        pkNewObject->m_kNxSceneDesc.timeStepMethod ||
        m_kNxSceneDesc.simType != pkNewObject->m_kNxSceneDesc.simType ||
        m_kNxSceneDesc.groundPlane != 
            pkNewObject->m_kNxSceneDesc.groundPlane ||
        m_kNxSceneDesc.boundsPlanes != 
            pkNewObject->m_kNxSceneDesc.boundsPlanes ||
        m_kNxSceneDesc.flags != 
            pkNewObject->m_kNxSceneDesc.flags ||
        m_kNxSceneDesc.internalThreadCount != 
            pkNewObject->m_kNxSceneDesc.internalThreadCount ||
        m_kNxSceneDesc.backgroundThreadCount != 
            pkNewObject->m_kNxSceneDesc.backgroundThreadCount ||
        m_kNxSceneDesc.backgroundThreadPriority !=
            pkNewObject->m_kNxSceneDesc.backgroundThreadPriority ||
        m_kNxSceneDesc.threadMask != 
            pkNewObject->m_kNxSceneDesc.threadMask ||
        m_kNxSceneDesc.backgroundThreadMask != 
            pkNewObject->m_kNxSceneDesc.backgroundThreadMask ||
        m_kNxSceneDesc.simThreadStackSize != 
            pkNewObject->m_kNxSceneDesc.simThreadStackSize ||
        m_kNxSceneDesc.simThreadPriority != 
            pkNewObject->m_kNxSceneDesc.simThreadPriority ||
        m_kNxSceneDesc.workerThreadStackSize != 
            pkNewObject->m_kNxSceneDesc.workerThreadStackSize ||
        m_kNxSceneDesc.workerThreadPriority != 
            pkNewObject->m_kNxSceneDesc.workerThreadPriority ||
        m_kNxSceneDesc.upAxis != pkNewObject->m_kNxSceneDesc.upAxis ||
        m_kNxSceneDesc.subdivisionLevel != 
            pkNewObject->m_kNxSceneDesc.subdivisionLevel ||
        m_kNxSceneDesc.staticStructure != 
            pkNewObject->m_kNxSceneDesc.staticStructure ||
        m_kNxSceneDesc.dynamicStructure != 
            pkNewObject->m_kNxSceneDesc.dynamicStructure ||
        m_kNxSceneDesc.dynamicTreeRebuildRateHint !=
            pkNewObject->m_kNxSceneDesc.dynamicTreeRebuildRateHint ||
        m_kNxSceneDesc.bpType != pkNewObject->m_kNxSceneDesc.bpType ||
        m_kNxSceneDesc.nbGridCellsX !=
            pkNewObject->m_kNxSceneDesc.nbGridCellsX ||
        m_kNxSceneDesc.nbGridCellsY !=
            pkNewObject->m_kNxSceneDesc.nbGridCellsY)
    {
        return false;
    }

    NiPoint3 kNewP;
    NiPoint3 kThisP;

    NiPhysXTypes::NxVec3ToNiPoint3(m_kNxSceneDesc.gravity, kThisP);
    NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_kNxSceneDesc.gravity, kNewP);
    if (kNewP != kThisP)
        return false;

    if (m_kNxSceneDesc.maxBounds)
    {
        if (!pkNewObject->m_kNxSceneDesc.maxBounds)
            return false;
        NiPhysXTypes::NxVec3ToNiPoint3(m_kNxSceneDesc.maxBounds->min, kThisP);
        NiPhysXTypes::NxVec3ToNiPoint3(
            pkNewObject->m_kNxSceneDesc.maxBounds->min, kNewP);
        if (kNewP != kThisP)
            return false;
        NiPhysXTypes::NxVec3ToNiPoint3(m_kNxSceneDesc.maxBounds->max, kThisP);
        NiPhysXTypes::NxVec3ToNiPoint3(
            pkNewObject->m_kNxSceneDesc.maxBounds->max, kNewP);
        if (kNewP != kThisP)
            return false;
    }
    else
    {
        if (pkNewObject->m_kNxSceneDesc.maxBounds)
            return false;
    }
    if (m_kNxSceneDesc.limits)
    {
        if (!pkNewObject->m_kNxSceneDesc.limits)
            return false;
        if (m_kNxSceneDesc.limits->maxNbActors !=
            pkNewObject->m_kNxSceneDesc.limits->maxNbActors ||
            m_kNxSceneDesc.limits->maxNbBodies !=
            pkNewObject->m_kNxSceneDesc.limits->maxNbBodies ||
            m_kNxSceneDesc.limits->maxNbStaticShapes
            != pkNewObject->m_kNxSceneDesc.limits->maxNbStaticShapes ||
            m_kNxSceneDesc.limits->maxNbDynamicShapes
            != pkNewObject->m_kNxSceneDesc.limits->maxNbDynamicShapes ||
            m_kNxSceneDesc.limits->maxNbJoints !=
            pkNewObject->m_kNxSceneDesc.limits->maxNbJoints)
            return false;
    }
    else
    {
        if (pkNewObject->m_kNxSceneDesc.limits)
            return false;
    }
    
    if (m_aeFilterOps[0] != pkNewObject->m_aeFilterOps[0] ||
        m_aeFilterOps[1] != pkNewObject->m_aeFilterOps[1] ||
        m_aeFilterOps[2] != pkNewObject->m_aeFilterOps[2] ||
        m_auiFilterConstants[0].bits0 !=
        pkNewObject->m_auiFilterConstants[0].bits0 ||
        m_auiFilterConstants[0].bits1 !=
        pkNewObject->m_auiFilterConstants[0].bits1 ||
        m_auiFilterConstants[0].bits2 !=
        pkNewObject->m_auiFilterConstants[0].bits2 ||
        m_auiFilterConstants[0].bits3 !=
        pkNewObject->m_auiFilterConstants[0].bits3 ||
        m_auiFilterConstants[1].bits0 !=
        pkNewObject->m_auiFilterConstants[1].bits0 ||
        m_auiFilterConstants[1].bits1 !=
        pkNewObject->m_auiFilterConstants[1].bits1 ||
        m_auiFilterConstants[1].bits2 !=
        pkNewObject->m_auiFilterConstants[1].bits2 ||
        m_auiFilterConstants[1].bits3 !=
        pkNewObject->m_auiFilterConstants[1].bits3 ||
        m_bFilterBool != pkNewObject->m_bFilterBool)
    {
        return false;
    }
    
    if (m_kCompartmentMap.GetCount() !=
        pkNewObject->m_kCompartmentMap.GetCount())
    {
        return false;
    }
    NiTMapIterator kIter = m_kCompartmentMap.GetFirstPos();
    while (kIter)
    {
        NiUInt32 uiID;
        NxCompartmentDesc* pkDesc;
        m_kCompartmentMap.GetNext(kIter, uiID, pkDesc);
        
        NxCompartmentDesc* pkOtherDesc;
        if (!pkNewObject->m_kCompartmentMap.GetAt(uiID, pkOtherDesc))
            return false;
            
        if (pkDesc->type != pkOtherDesc->type ||
            pkDesc->deviceCode != pkOtherDesc->deviceCode ||
            pkDesc->gridHashCellSize != pkOtherDesc->gridHashCellSize ||
            pkDesc->gridHashTablePower != pkOtherDesc->gridHashTablePower)
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
