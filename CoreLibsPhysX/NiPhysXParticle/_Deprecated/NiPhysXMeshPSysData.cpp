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
#include "NiPhysXParticlePCH.h"

#include "NiPhysXMeshPSysData.h"
#include "NiPhysXParticle.h"
#include <NiBool.h>

NiImplementRTTI(NiPhysXMeshPSysData, NiMeshPSysData);

//---------------------------------------------------------------------------
NiPhysXMeshPSysData::NiPhysXMeshPSysData()
{
}
//---------------------------------------------------------------------------
NiPhysXMeshPSysData::~NiPhysXMeshPSysData()
{
    m_spScene = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshPSysData);
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::LoadBinary(NiStream& kStream)
{
    NiMeshPSysData::LoadBinary(kStream);

    m_spScene = (NiPhysXScene*)kStream.ResolveLinkID();

    NiBool bVal;
    
    // Load Fill Pools on Load
    NiStreamLoadBinary(kStream, bVal);
    m_bFillActorPoolsOnLoad = (bVal != 0);

    // Load Fill Pools on Load
    NiStreamLoadBinary(kStream, bVal);
    m_bKeepsActorMeshes = (bVal != 0);

    // Load the Default Pool Size
    NiStreamLoadBinary(kStream, m_uiDefaultActorPoolSize);

    NiUInt32 uiNumGenerations;
    NiUInt32 uiGenerationPoolSize;

    NiStreamLoadBinary(kStream, uiNumGenerations);

    for (NiUInt32 ui = 0; ui < uiNumGenerations; ui++)
    {
        NiStreamLoadBinary(kStream, uiGenerationPoolSize);
    }
    m_kActorPoolCenter.LoadBinary(kStream);
    m_kActorPoolDim.LoadBinary(kStream);

    NiUInt32 uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    m_kActorDescriptors.SetSize(uiSize);
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXActorDesc* pkActor = (NiPhysXActorDesc*)
            kStream.ResolveLinkID();
        m_kActorDescriptors.SetAt(ui, pkActor);
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::LinkObject(NiStream& kStream)
{
    NiMeshPSysData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysData::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshPSysData::RegisterStreamables(kStream))
        return false;
    
    if (m_spScene) 
        m_spScene->RegisterStreamables(kStream);
    
    for (NiUInt32 ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActor = m_kActorDescriptors.GetAt(ui);
        if (pkActor)
        {
            pkActor->RegisterStreamables(kStream);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::SaveBinary(NiStream& kStream)
{
    NiMeshPSysData::SaveBinary(kStream);

    // Nothing in PhysX PSys info gets saved
    
    kStream.SaveLinkID(m_spScene);
    
    NiStreamSaveBinary(kStream, NiBool(m_bFillActorPoolsOnLoad));
    NiStreamSaveBinary(kStream, NiBool(m_bKeepsActorMeshes));
    NiStreamSaveBinary(kStream, m_uiDefaultActorPoolSize);

    NiStreamSaveBinary(kStream, m_uiNumGenerations);

    for (NiUInt32 ui = 0; ui < m_uiNumGenerations; ui++)
    {
        NiStreamSaveBinary(kStream, 0);
    }
    m_kActorPoolCenter.SaveBinary(kStream);
    m_kActorPoolDim.SaveBinary(kStream);

    NiUInt32 uiSize = m_kActorDescriptors.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXActorDesc* pkActor = m_kActorDescriptors.GetAt(ui);
        kStream.SaveLinkID(pkActor);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysData::IsEqual(NiObject* pkObject)
{
    if (!NiMeshPSysData::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXMeshPSysData* pkDest = (NiPhysXMeshPSysData*) pkObject;

    if (m_bFillActorPoolsOnLoad != pkDest->m_bFillActorPoolsOnLoad ||
        m_bKeepsActorMeshes != pkDest->m_bKeepsActorMeshes ||
        m_uiDefaultActorPoolSize != pkDest->m_uiDefaultActorPoolSize ||
        m_kActorPoolCenter != pkDest->m_kActorPoolCenter ||
        m_kActorPoolDim != pkDest->m_kActorPoolDim)
    {
        return false;
    }
    
    if (m_kActorDescriptors.GetSize() != pkDest->m_kActorDescriptors.GetSize())
    {
        return false;
    }

    for (NiUInt32 ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActor1 = m_kActorDescriptors.GetAt(ui);
        NiPhysXActorDesc* pkActor2 = pkDest->m_kActorDescriptors.GetAt(ui);
        if ((pkActor1 && !pkActor2) || (!pkActor1 && pkActor2) ||
            (pkActor1 && pkActor2 && !pkActor1->IsEqual(pkActor2)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
