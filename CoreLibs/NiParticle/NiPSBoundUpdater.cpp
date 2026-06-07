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

#include "NiPSBoundUpdater.h"
#include "NiPSMeshParticleSystem.h"
#include "NiPSCommonSemantics.h"

NiImplementRTTI(NiPSBoundUpdater, NiObject);

//---------------------------------------------------------------------------
NiPSBoundUpdater::NiPSBoundUpdater(NiInt16 sUpdateSkip) :
    m_sUpdateSkip(0),
    m_usUpdateCount(0),
    m_pkSkipBounds(NULL)
{
    SetUpdateSkip(sUpdateSkip);
}
//---------------------------------------------------------------------------
NiPSBoundUpdater::~NiPSBoundUpdater()
{
    NiDelete[] m_pkSkipBounds;
}
//---------------------------------------------------------------------------
void NiPSBoundUpdater::UpdateBound(NiPSParticleSystem* pkParticleSystem)
{
    // Get particle counts.
    NiUInt32 uiNumParticles = pkParticleSystem->GetNumParticles();
    NiUInt32 uiMaxNumParticles = pkParticleSystem->GetMaxNumParticles();

    // Check for mesh particle system.
    NiPSMeshParticleSystem* pkMeshParticleSystem = NiDynamicCast(
        NiPSMeshParticleSystem, pkParticleSystem);

    // Check for no particles.
    if (uiNumParticles == 0)
    {
        // Set the bound to identity if no particles.
        NiBound kBound;
        kBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
        if (pkMeshParticleSystem)
        {
            pkMeshParticleSystem->GetParticleContainer()->SetWorldBound(
                kBound);
        }
        else
        {
            pkParticleSystem->SetModelBound(kBound);
        }

        // Reset the update count.
        m_usUpdateCount = 0;

        return;
    }

    // Select the appropriate skip divisor.
    NiUInt8 ucSkipDivisor = pkMeshParticleSystem ? 20 : 50;

    // Check for auto setting the update skip.
    if (m_sUpdateSkip == AUTO_SKIP_UPDATE)
    {
        SetUpdateSkip((NiInt16) ((uiMaxNumParticles / ucSkipDivisor) + 1));
    }

    // Set the computed skip based on the number of particles.
    NiInt16 sComputedSkip = (NiInt16)(NiMin(m_sUpdateSkip, (uiNumParticles /
        ucSkipDivisor) + 1));

    // Ensure a minimum of 1.
    sComputedSkip = (NiInt16)(NiMax(sComputedSkip, 1));

    // Compute the skip bounds.
    NiBound kSkipBound;
    if (pkMeshParticleSystem)
    {
        // Get the particle container.
        NiNode* pkParticleContainer =
            pkMeshParticleSystem->GetParticleContainer();

        // Compute the bounding volume containing all mesh particles.
        kSkipBound = pkParticleContainer->GetAt(0)->GetWorldBound();
        for (NiUInt16 us = m_usUpdateCount; us < uiNumParticles;
            us = (NiUInt16)(us + sComputedSkip))
        {
            kSkipBound.Merge(&pkParticleContainer->GetAt(us)->GetWorldBound());
        }
    }
    else
    {
        NiPoint3* pkPositions = pkParticleSystem->GetPositions();
        float* pfRadii = pkParticleSystem->GetRadii();
        float* pfSizes = pkParticleSystem->GetSizes();

        // Compute the axis-aligned box containing the data.
        NiPoint3 kPosition = pkPositions[0];
        float fMinX = kPosition.x;
        float fMinY = kPosition.y;
        float fMinZ = kPosition.z;
        float fMaxX = fMinX;
        float fMaxY = fMinY;
        float fMaxZ = fMinZ;
        for (NiUInt16 us = m_usUpdateCount; us < uiNumParticles;
            us = (NiUInt16)(us + sComputedSkip)) 
        {
            kPosition = pkPositions[us];
            float fFinalSize = pfRadii[us] * pfSizes[us];
            
            fMinX = NiMin(fMinX, kPosition.x - fFinalSize);
            fMaxX = NiMax(fMaxX, kPosition.x + fFinalSize);
            
            fMinY = NiMin(fMinY, kPosition.y - fFinalSize);
            fMaxY = NiMax(fMaxY, kPosition.y + fFinalSize);
            
            fMinZ = NiMin(fMinZ, kPosition.z - fFinalSize);
            fMaxZ = NiMax(fMaxZ, kPosition.z + fFinalSize);
        }
        NiPoint3 kMin(fMinX, fMinY, fMinZ);
        NiPoint3 kMax(fMaxX, fMaxY, fMaxZ);

        kSkipBound.SetCenterAndRadius(0.5f * (kMin + kMax),
            (kMax - kMin).Length() / 2.0f);
    }

    // Update the skip bounds.
    m_pkSkipBounds[m_usUpdateCount] = kSkipBound;
    
    // Zero the unused bounds.
    for (NiInt16 s = sComputedSkip; s < m_sUpdateSkip; ++s)
    {
        m_pkSkipBounds[s].SetCenter(NiPoint3::ZERO);
        m_pkSkipBounds[s].SetRadius(0.0f);
    }

    // Merge the skip bounds.
    NiBound kTotalBound = m_pkSkipBounds[m_usUpdateCount];
    for (NiInt16 s = 1; s < sComputedSkip; ++s)
    {
        if (m_pkSkipBounds[s].GetRadius() != 0.0f)
        {
            kTotalBound.Merge(&m_pkSkipBounds[s]);
        }
    }

    // Guarantee we have some type of bound.
    if (kTotalBound.GetRadius() == 0.0f)
    {
        kTotalBound.SetRadius(pkParticleSystem->GetRadii()[0] *
            pkParticleSystem->GetSizes()[0]);
    }

    // Reset the update count.
    if (++m_usUpdateCount >= sComputedSkip)
    {
        m_usUpdateCount = 0;
    }

    // Set the bound on the particle system object.
    if (pkMeshParticleSystem)
    {
        pkMeshParticleSystem->GetParticleContainer()->SetWorldBound(
            kTotalBound);
    }
    else
    {
        pkParticleSystem->SetModelBound(kTotalBound);
    }
}
//---------------------------------------------------------------------------
void NiPSBoundUpdater::SetUpdateSkip(NiInt16 sUpdateSkip)
{
    m_sUpdateSkip = sUpdateSkip;

    NiDelete[] m_pkSkipBounds;

    // Always have at least 1.
    sUpdateSkip = (NiInt16)(NiMax(sUpdateSkip, 1));

    m_pkSkipBounds = NiNew NiBound[sUpdateSkip];

    // Initialize the bounds.
    for (NiInt16 s = 0; s < sUpdateSkip; ++s)
    {
        m_pkSkipBounds[s].SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
    }
}
//---------------------------------------------------------------------------
void NiPSBoundUpdater::ResetUpdateSkipBounds()
{
    NiInt16 sUpdateSkip = (NiInt16)(NiMax(m_sUpdateSkip, 1));

    // Initialize the bounds
    for (NiInt16 s = 0; s < sUpdateSkip; ++s)
    {
        m_pkSkipBounds[s].SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
    }
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSBoundUpdater);
//---------------------------------------------------------------------------
void NiPSBoundUpdater::CopyMembers(
    NiPSBoundUpdater* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->SetUpdateSkip(m_sUpdateSkip);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSBoundUpdater);
//---------------------------------------------------------------------------
void NiPSBoundUpdater::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiInt16 sUpdateSkip;
    NiStreamLoadBinary(kStream, sUpdateSkip);
    SetUpdateSkip(sUpdateSkip);
}
//---------------------------------------------------------------------------
void NiPSBoundUpdater::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSBoundUpdater::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSBoundUpdater::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_sUpdateSkip);
}
//---------------------------------------------------------------------------
bool NiPSBoundUpdater::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPSBoundUpdater* pkDest = (NiPSBoundUpdater*) pkObject;

    if (pkDest->m_sUpdateSkip != m_sUpdateSkip)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSBoundUpdater::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSBoundUpdater::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("UpdateSkip", m_sUpdateSkip));
}
//---------------------------------------------------------------------------
