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

#include "NiPSysSpawnModifier.h"
#include <NiStream.h>

NiImplementRTTI(NiPSysSpawnModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysSpawnModifier::NiPSysSpawnModifier() :
    m_usNumSpawnGenerations(1),
    m_fPercentageSpawned(1.0f),
    m_usMinNumToSpawn(1),
    m_usMaxNumToSpawn(1),
    m_fSpawnSpeedChaos(0.0f),
    m_fSpawnDirChaos(0.0f),
    m_fLifeSpan(0.0f),
    m_fLifeSpanVar(0.0f)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysSpawnModifier);
//---------------------------------------------------------------------------
void NiPSysSpawnModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_usNumSpawnGenerations);
    NiStreamLoadBinary(kStream, m_fPercentageSpawned);
    NiStreamLoadBinary(kStream, m_usMinNumToSpawn);
    NiStreamLoadBinary(kStream, m_usMaxNumToSpawn);
    NiStreamLoadBinary(kStream, m_fSpawnSpeedChaos);
    NiStreamLoadBinary(kStream, m_fSpawnDirChaos);
    NiStreamLoadBinary(kStream, m_fLifeSpan);
    NiStreamLoadBinary(kStream, m_fLifeSpanVar);
}
//---------------------------------------------------------------------------
void NiPSysSpawnModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysSpawnModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysSpawnModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_usNumSpawnGenerations);
    NiStreamSaveBinary(kStream, m_fPercentageSpawned);
    NiStreamSaveBinary(kStream, m_usMinNumToSpawn);
    NiStreamSaveBinary(kStream, m_usMaxNumToSpawn);
    NiStreamSaveBinary(kStream, m_fSpawnSpeedChaos);
    NiStreamSaveBinary(kStream, m_fSpawnDirChaos);
    NiStreamSaveBinary(kStream, m_fLifeSpan);
    NiStreamSaveBinary(kStream, m_fLifeSpanVar);
}
//---------------------------------------------------------------------------
bool NiPSysSpawnModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysSpawnModifier* pkDest = (NiPSysSpawnModifier*) pkObject;

    if (pkDest->m_usNumSpawnGenerations != m_usNumSpawnGenerations ||
        pkDest->m_fPercentageSpawned != m_fPercentageSpawned ||
        pkDest->m_usMinNumToSpawn != m_usMinNumToSpawn ||
        pkDest->m_usMaxNumToSpawn != m_usMaxNumToSpawn ||
        pkDest->m_fSpawnSpeedChaos != m_fSpawnSpeedChaos ||
        pkDest->m_fSpawnDirChaos != m_fSpawnDirChaos ||
        pkDest->m_fLifeSpan != m_fLifeSpan ||
        pkDest->m_fLifeSpanVar != m_fLifeSpanVar)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
