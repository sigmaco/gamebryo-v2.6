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

#include "NiPhysXPSysData.h"
#include <NiBool.h>

NiImplementRTTI(NiPhysXPSysData, NiPSysData);

//---------------------------------------------------------------------------
NiPhysXPSysData::NiPhysXPSysData()
{
}
//---------------------------------------------------------------------------
NiPhysXPSysData::~NiPhysXPSysData()
{
    m_spScene = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSysData);
//---------------------------------------------------------------------------
void NiPhysXPSysData::LoadBinary(NiStream& kStream)
{
    NiPSysData::LoadBinary(kStream);

    m_spScene = (NiPhysXScene*)kStream.ResolveLinkID();

    NiBool bVal;
    
    // Load Fill Pools on Load
    NiStreamLoadBinary(kStream, bVal);
    m_bFillActorPoolOnLoad = (bVal != 0);

    // Load actor meshes on load
    NiStreamLoadBinary(kStream, bVal);
    m_bKeepsActorMeshes = (bVal != 0);

    // Load the Default Pool Size
    NiStreamLoadBinary(kStream, m_uiDefaultActorPoolSize);

    NiUInt32 uiGenerationPoolSize;
    NiStreamLoadBinary(kStream, uiGenerationPoolSize);
    m_kActorPoolCenter.LoadBinary(kStream);
    m_kActorPoolDim.LoadBinary(kStream);

    m_spActorDescriptor =(NiPhysXActorDesc*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::LinkObject(NiStream& kStream)
{
    NiPSysData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysData::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysData::RegisterStreamables(kStream))
        return false;
    
    if (m_spScene) 
        m_spScene->RegisterStreamables(kStream);
    
    if (m_spActorDescriptor)
        m_spActorDescriptor->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::SaveBinary(NiStream& kStream)
{
    NiPSysData::SaveBinary(kStream);

    kStream.SaveLinkID(m_spScene);
    
    NiStreamSaveBinary(kStream, NiBool(m_bFillActorPoolOnLoad));
    NiStreamSaveBinary(kStream, NiBool(m_bKeepsActorMeshes));
    NiStreamSaveBinary(kStream, m_uiDefaultActorPoolSize);

    NiStreamSaveBinary(kStream, 0);
    m_kActorPoolCenter.SaveBinary(kStream);
    m_kActorPoolDim.SaveBinary(kStream);

    kStream.SaveLinkID(m_spActorDescriptor);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysData::IsEqual(NiObject* pkObject)
{
    if (!NiPSysData::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXPSysData* pkDest = (NiPhysXPSysData*) pkObject;

    if (m_bFillActorPoolOnLoad != pkDest->m_bFillActorPoolOnLoad ||
        m_bKeepsActorMeshes != pkDest->m_bKeepsActorMeshes ||
        m_uiDefaultActorPoolSize != pkDest->m_uiDefaultActorPoolSize ||
        m_kActorPoolCenter != pkDest->m_kActorPoolCenter ||
        m_kActorPoolDim != pkDest->m_kActorPoolDim)
    {
        return false;
    }
                    
    if (!m_spActorDescriptor->IsEqual(pkDest->m_spActorDescriptor))
        return false;

    return true;
}
//---------------------------------------------------------------------------

