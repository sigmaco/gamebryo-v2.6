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
#include <NiParticlePCH.h>

#include "NiMeshPSysData.h"
#include <NiBool.h>

NiImplementRTTI(NiMeshPSysData, NiPSysData);

//---------------------------------------------------------------------------
NiMeshPSysData::NiMeshPSysData() :
    m_bFillPoolsOnLoad(false),
    m_uiDefaultPoolSize(0),
    m_uiNumGenerations(1)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMeshPSysData);
//---------------------------------------------------------------------------
void NiMeshPSysData::LoadBinary(NiStream& kStream)
{
    NiPSysData::LoadBinary(kStream);

    if(kStream.GetFileVersion() <= NiStream::GetVersion(10,1,0,0))
    {
        // Assume a pool is equal to the max number of particles
        m_uiDefaultPoolSize = GetMaxNumParticles();

        // Assume one generation
        m_uiNumGenerations = 1;
    }
    else
    {
        // Load the Default Pool Size
        NiStreamLoadBinary(kStream, m_uiDefaultPoolSize);

        // Load Fill Pools on Load
        NiBool bFill;
        NiStreamLoadBinary(kStream, bFill);
        m_bFillPoolsOnLoad = NIBOOL_IS_TRUE(bFill);

        NiStreamLoadBinary(kStream, m_uiNumGenerations);

        for (unsigned int uiLoop = 0; uiLoop < m_uiNumGenerations; uiLoop++)
        {
            unsigned int uiGenerationPoolSize;
            NiStreamLoadBinary(kStream, uiGenerationPoolSize);
        }
    }

    kStream.ReadLinkID();   // m_spParticleMeshes
}
//---------------------------------------------------------------------------
void NiMeshPSysData::LinkObject(NiStream& kStream)
{
    NiPSysData::LinkObject(kStream);

    m_spParticleMeshes = (NiNode*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiMeshPSysData::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysData::RegisterStreamables(kStream))
    {
        return false;
    }

    m_spParticleMeshes->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiMeshPSysData::SaveBinary(NiStream& kStream)
{
    NiPSysData::SaveBinary(kStream);

    // Save the Default Pool Size
    NiStreamSaveBinary(kStream, m_uiDefaultPoolSize);

    // Save Fill Pools on Load
    NiStreamSaveBinary(kStream, (NiBool)m_bFillPoolsOnLoad);

    // Save the number of pools
    NiStreamSaveBinary(kStream, m_uiNumGenerations);

    // Save the size of each pool
    for (unsigned int uiLoop = 0; uiLoop < m_uiNumGenerations; uiLoop++)
    {
        NiStreamSaveBinary(kStream, 0);
    }

    kStream.SaveLinkID(m_spParticleMeshes);
}
//---------------------------------------------------------------------------
bool NiMeshPSysData::IsEqual(NiObject* pkObject)
{
    if (!NiPSysData::IsEqual(pkObject))
    {
        return false;
    }

    NiMeshPSysData* pkDest = (NiMeshPSysData*) pkObject;

    if (!m_spParticleMeshes->IsEqual(pkDest->m_spParticleMeshes))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
