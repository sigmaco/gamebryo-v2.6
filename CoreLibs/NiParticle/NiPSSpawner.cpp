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

#include "NiPSSpawner.h"
#include "NiPSParticleSystem.h"
#include "NiPSFlagsHelpers.h"
#include "NiPSKernelDefinitions.h"

NiImplementRTTI(NiPSSpawner, NiObject);

const NiUInt16 NiPSSpawner::INVALID_ID = 0xFFF;

//---------------------------------------------------------------------------
NiPSSpawner::NiPSSpawner(
    NiUInt16 usNumSpawnGenerations,
    float fPercentageSpawned,
    NiUInt32 uiMinNumToSpawn,
    NiUInt32 uiMaxNumToSpawn,
    float fSpawnSpeedChaos,
    float fSpawnDirChaos,
    float fLifeSpan,
    float fLifeSpanVar) :
    m_pkMasterPSystem(NULL),
    m_fPercentageSpawned(fPercentageSpawned),
    m_fSpawnSpeedChaos(fSpawnSpeedChaos),
    m_fSpawnDirChaos(fSpawnDirChaos),
    m_fLifeSpan(fLifeSpan),
    m_fLifeSpanVar(fLifeSpanVar),
    m_uiMinNumToSpawn(uiMinNumToSpawn),
    m_uiMaxNumToSpawn(uiMaxNumToSpawn),
    m_usNumSpawnGenerations(usNumSpawnGenerations)
{
}
//---------------------------------------------------------------------------
void NiPSSpawner::SpawnParticles(
    float fCurrentTime,
    float fSpawnTime,
    NiUInt32 uiOldIndex,
    NiPSParticleSystem* pkOldPSystem)
{
    // Do not spawn if exceeded number of spawn generations or percentage
    // spawned.
    NiUInt32 uiFlags = pkOldPSystem->GetFlags()[uiOldIndex];
    NiUInt16 usOldGeneration = NiPSFlagsHelpers::GetGeneration(uiFlags);
    if (usOldGeneration >= m_usNumSpawnGenerations ||
        NiUnitRandom() > m_fPercentageSpawned)
    {
        return;
    }

    // Determine number to spawn.
    NIASSERT(m_uiMaxNumToSpawn >= m_uiMinNumToSpawn);
    NiUInt32 uiVariation = 0;
    if (m_uiMaxNumToSpawn != m_uiMinNumToSpawn)
    {
        float fVariation = NiUnitRandom() * (m_uiMaxNumToSpawn -
            m_uiMinNumToSpawn);
        uiVariation = (NiUInt32) fVariation;
        if (NiFmod(fVariation, 1.0f) > 0.5f)
        {
            uiVariation++;
        }
    }
    NiUInt32 uiNumToSpawn = m_uiMinNumToSpawn + uiVariation;
    if (uiNumToSpawn == 0)
    {
        uiNumToSpawn = 1;
    }

    // Spawn particles.
    for (NiUInt32 ui = 0; ui < uiNumToSpawn; ++ui)
    {
        SpawnParticle(fCurrentTime, fSpawnTime, uiOldIndex, pkOldPSystem);
    }
}
//---------------------------------------------------------------------------
void NiPSSpawner::SpawnParticle(
    float fCurrentTime,
    float fSpawnTime,
    NiUInt32 uiOldIndex,
    NiPSParticleSystem* pkOldPSystem)
{
    NIASSERT(m_pkMasterPSystem);

    // Transform old position and velocity.
    NiPoint3 kOldPosition = pkOldPSystem->GetPositions()[uiOldIndex];
    NiPoint3 kOldVelocity = pkOldPSystem->GetVelocities()[uiOldIndex];
    if (pkOldPSystem != m_pkMasterPSystem)
    {
        NiTransform kOldXForm = pkOldPSystem->GetWorldTransform();
        kOldPosition = kOldXForm * kOldPosition;
        kOldVelocity = kOldXForm.m_Rotate * kOldVelocity;

        NiTransform kNewXForm = m_pkMasterPSystem->GetWorldTransform();
        NiTransform kInvNewXForm;
        kNewXForm.Invert(kInvNewXForm);
        kOldPosition = kInvNewXForm * kOldPosition;
        kOldVelocity = kInvNewXForm.m_Rotate * kOldVelocity;
    }

    // Get pointer to new particle data.
    NiUInt32 uiNewIndex = m_pkMasterPSystem->AddParticle();
    if (uiNewIndex == NiPSParticleSystem::INVALID_PARTICLE)
    {
        return;
    }

    // Calculate age based on spawn time.
    float fNewAge = (fCurrentTime - fSpawnTime);
    m_pkMasterPSystem->GetAges()[uiNewIndex] = fNewAge;

    // Calculate new velocity based on original and speed, direction chaos.
    float fOrigSpeed = kOldVelocity.Length();

    // Add or subtract based on speed up/slow down factor.
    float fNewSpeed = fOrigSpeed;
    if (m_fSpawnSpeedChaos != 0.0f)
    {
        fNewSpeed *= 1.0f + (m_fSpawnSpeedChaos * NiUnitRandom());
    }

    // Calculate directional chaos.
    NiPoint3 kDirChaos(0.0f, 0.0f, 1.0f);
    if (m_fSpawnDirChaos != 0.0f)
    {
        float fDecChaos = NiUnitRandom() * m_fSpawnDirChaos * NI_PI;
        float fPlanChaos = NiUnitRandom() * NI_TWO_PI;

        float fSinDecChaos = NiSin(fDecChaos);
        kDirChaos = NiPoint3(fSinDecChaos * NiCos(fPlanChaos),
            fSinDecChaos * NiSin(fPlanChaos), NiCos(fDecChaos));
    }

    // Rotate directional chaos to line up with original direction.
    // Original direction = (x,y,z), length = l
    // Rotation: angle = acos(z/l), axis = (y,-x,0)
    float fX = kOldVelocity.x;
    float fY = kOldVelocity.y;
    NiMatrix3 kS(NiPoint3(0.0f,0.0f,fX),NiPoint3(0.0f,0.0f,fY),
        NiPoint3(-fX,-fY,0.0f));

    float fTestVal = (kDirChaos.Cross(kOldVelocity))
        * NiPoint3(fY, -fX, 0.0f);
    NiMatrix3 kRot = NiMatrix3::IDENTITY;
    if (fTestVal > NIPSKERNEL_EPSILON)
    {
        // fX or fY != 0
        kRot = kRot + kS * (kRot + kS *
            ((fOrigSpeed - kOldVelocity.z) / 
            (fX * fX + fY * fY))) * (1.0f / fOrigSpeed);
    }
    else if (-fTestVal > NIPSKERNEL_EPSILON)
    {
        // fX or fY != 0
        kRot = kRot - kS * (kRot - kS *
            ((fOrigSpeed - kOldVelocity.z) / 
            (fX * fX + fY * fY))) * (1.0f / fOrigSpeed);
    }
    else // fX and fY == 0.0f
    {
        if (kOldVelocity.z < 0.0f)
        {
            kRot = NiMatrix3(NiPoint3(-1.0f, 0.0f, 0.0f),
                NiPoint3(0.0f, -1.0f, 0.0f), NiPoint3(0.0f, 0.0f, -1.0f));
        }
    }

    NiMatrix3 kTempMat = kRot * fNewSpeed;
    m_pkMasterPSystem->GetVelocities()[uiNewIndex] = kTempMat * kDirChaos;

    // Calculate and set life span.
    float fLifeSpan = m_fLifeSpan;
    if (m_fLifeSpanVar != 0.0f)
    {
        fLifeSpan += m_fLifeSpanVar * (NiUnitRandom() - 0.5f);
    }
    m_pkMasterPSystem->GetLifeSpans()[uiNewIndex] = fLifeSpan;

    // Set new generation value.
    NiUInt32 uiOldFlags = pkOldPSystem->GetFlags()[uiOldIndex];
    NiUInt16 usOldGeneration = NiPSFlagsHelpers::GetGeneration(uiOldFlags);
    NiUInt32 uiNewFlags = 0;
    NiPSFlagsHelpers::SetGeneration(uiNewFlags, usOldGeneration + 1);
    m_pkMasterPSystem->GetFlags()[uiNewIndex] = uiNewFlags;

    // Propagate particle position.
    m_pkMasterPSystem->GetPositions()[uiNewIndex] = kOldPosition;

    // Propagate particle color.
    NiRGBA kOldColor = NiRGBA::WHITE;
    if (pkOldPSystem->HasColors())
    {
        kOldColor = pkOldPSystem->GetColors()[uiOldIndex];
    }
    if (m_pkMasterPSystem->HasColors())
    {
        m_pkMasterPSystem->GetColors()[uiNewIndex] = kOldColor;
    }

    // Propagate particle radius.
    m_pkMasterPSystem->GetRadii()[uiNewIndex] =
        pkOldPSystem->GetRadii()[uiOldIndex];

    // Propagate particle size.
    m_pkMasterPSystem->GetSizes()[uiNewIndex] =
        pkOldPSystem->GetSizes()[uiOldIndex];

    // Propagate particle rotation angle.
    float fOldRotationAngle = 0.0f;
    float fOldRotationSpeed = 0.0f;
    if (pkOldPSystem->HasRotations())
    {
        fOldRotationAngle = pkOldPSystem->GetRotationAngles()[uiOldIndex];
        fOldRotationSpeed = pkOldPSystem->GetRotationSpeeds()[uiOldIndex];
    }
    if (m_pkMasterPSystem->HasRotations())
    {
        m_pkMasterPSystem->GetRotationAngles()[uiNewIndex] = fOldRotationAngle;
        m_pkMasterPSystem->GetRotationSpeeds()[uiNewIndex] = fOldRotationSpeed;
    }

    // Propagate particle rotation axis.
    NiPoint3 kOldRotationAxis = NiPoint3::UNIT_X;
    if (pkOldPSystem->HasRotationAxes())
    {
        kOldRotationAxis = pkOldPSystem->GetRotationAxes()[uiOldIndex];
    }
    if (m_pkMasterPSystem->HasRotationAxes())
    {
        m_pkMasterPSystem->GetRotationAxes()[uiNewIndex] = kOldRotationAxis;
    }

    // Set new last update time.
    m_pkMasterPSystem->GetLastUpdateTimes()[uiNewIndex] = fCurrentTime -
        fNewAge;

    m_pkMasterPSystem->InitializeParticle(uiNewIndex);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSSpawner);
//---------------------------------------------------------------------------
void NiPSSpawner::CopyMembers(NiPSSpawner* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_fPercentageSpawned = m_fPercentageSpawned;
    pkDest->m_fSpawnSpeedChaos = m_fSpawnSpeedChaos;
    pkDest->m_fSpawnDirChaos = m_fSpawnDirChaos;
    pkDest->m_fLifeSpan = m_fLifeSpan;
    pkDest->m_fLifeSpanVar = m_fLifeSpanVar;
    pkDest->m_usNumSpawnGenerations = m_usNumSpawnGenerations;
    pkDest->m_uiMinNumToSpawn = m_uiMinNumToSpawn;
    pkDest->m_uiMaxNumToSpawn = m_uiMaxNumToSpawn;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSSpawner);
//---------------------------------------------------------------------------
void NiPSSpawner::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fPercentageSpawned);
    NiStreamLoadBinary(kStream, m_fSpawnSpeedChaos);
    NiStreamLoadBinary(kStream, m_fSpawnDirChaos);
    NiStreamLoadBinary(kStream, m_fLifeSpan);
    NiStreamLoadBinary(kStream, m_fLifeSpanVar);
    NiStreamLoadBinary(kStream, m_usNumSpawnGenerations);
    NiStreamLoadBinary(kStream, m_uiMinNumToSpawn);
    NiStreamLoadBinary(kStream, m_uiMaxNumToSpawn);
}
//---------------------------------------------------------------------------
void NiPSSpawner::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSpawner::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSSpawner::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fPercentageSpawned);
    NiStreamSaveBinary(kStream, m_fSpawnSpeedChaos);
    NiStreamSaveBinary(kStream, m_fSpawnDirChaos);
    NiStreamSaveBinary(kStream, m_fLifeSpan);
    NiStreamSaveBinary(kStream, m_fLifeSpanVar);
    NiStreamSaveBinary(kStream, m_usNumSpawnGenerations);
    NiStreamSaveBinary(kStream, m_uiMinNumToSpawn);
    NiStreamSaveBinary(kStream, m_uiMaxNumToSpawn);
}
//---------------------------------------------------------------------------
bool NiPSSpawner::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPSSpawner* pkDest = (NiPSSpawner*) pkObject;

    if (pkDest->m_fPercentageSpawned != m_fPercentageSpawned ||
        pkDest->m_fSpawnSpeedChaos != m_fSpawnSpeedChaos ||
        pkDest->m_fSpawnDirChaos != m_fSpawnDirChaos ||
        pkDest->m_fLifeSpan != m_fLifeSpan ||
        pkDest->m_fLifeSpanVar != m_fLifeSpanVar ||
        pkDest->m_usNumSpawnGenerations != m_usNumSpawnGenerations ||
        pkDest->m_uiMinNumToSpawn != m_uiMinNumToSpawn ||
        pkDest->m_uiMaxNumToSpawn != m_uiMaxNumToSpawn)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSSpawner::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSSpawner::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("PercentageSpawned",
        m_fPercentageSpawned));
    pkStrings->Add(NiGetViewerString("SpawnSpeedChaos", m_fSpawnSpeedChaos));
    pkStrings->Add(NiGetViewerString("SpawnDirChaos", m_fSpawnDirChaos));
    pkStrings->Add(NiGetViewerString("LifeSpan", m_fLifeSpan));
    pkStrings->Add(NiGetViewerString("LifeSpanVar", m_fLifeSpanVar));
    pkStrings->Add(NiGetViewerString("NumSpawnGenerations",
        m_usNumSpawnGenerations));
    pkStrings->Add(NiGetViewerString("MinNumToSpawn", m_uiMinNumToSpawn));
    pkStrings->Add(NiGetViewerString("MaxNumToSpawn", m_uiMaxNumToSpawn));

    pkStrings->Add(NiGetViewerString("Master Particle System",
        m_pkMasterPSystem));
}
//---------------------------------------------------------------------------
