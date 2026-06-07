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

#include "NiPSParticleSystem.h"
#include "NiTransform.h"

#include "NiPhysXPSParticleSystemDest.h"

#include "NiPhysXPSParticleSystem.h"
#include "NiPhysXPSMeshParticleSystem.h"

NiImplementRTTI(NiPhysXPSParticleSystemDest, NiPhysXDest);

//---------------------------------------------------------------------------
NiPhysXPSParticleSystemDest::NiPhysXPSParticleSystemDest(
    NiPSParticleSystem* pkTarget) : m_pkTarget(pkTarget)
{
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystemDest::NiPhysXPSParticleSystemDest() : m_pkTarget(0)
{
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystemDest::~NiPhysXPSParticleSystemDest()
{
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemDest::UpdateFromActors(const float,
    const NiTransform& kRootTransform, const float fScalePToW,
    const bool)
{
    if (!m_bActive)
        return;

    NiTransform kPSysXform = m_pkTarget->GetWorldTransform();
    NiTransform kInvPSysXform;
    kPSysXform.Invert(kInvPSysXform);
    NiTransform kPhysXToPSys = kInvPSysXform * kRootTransform;

    if(NiIsKindOf(NiPhysXPSParticleSystem, m_pkTarget))
    {
        NiPhysXPSParticleSystem* pkTarget = 
            (NiPhysXPSParticleSystem*)m_pkTarget;

        pkTarget->SetPhysXToPSysTransform(kPhysXToPSys);
        pkTarget->SetScalePhysXToWorld(fScalePToW);

        // Inform the target that it needs to do some work.
        pkTarget->SetNeedsUpdate(true);
    }
    else if (NiIsKindOf(NiPhysXPSMeshParticleSystem, m_pkTarget))
    {
        NiPhysXPSMeshParticleSystem* pkTarget = 
            (NiPhysXPSMeshParticleSystem*)m_pkTarget;

        pkTarget->SetPhysXToPSysTransform(kPhysXToPSys);
        pkTarget->SetScalePhysXToWorld(fScalePToW);

        // Inform the target that it needs to do some work.
        pkTarget->SetNeedsUpdate(true);
    }
    else
    {
        NIASSERT(false && "NiPhysXPSParticleSystemDest: Wrong target type\n");
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPSParticleSystemDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXDest::RegisterStreamables(kStream))
        return false;

    m_pkTarget->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemDest::SaveBinary(NiStream& kStream)
{
    NiPhysXDest::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkTarget);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSParticleSystemDest);
//---------------------------------------------------------------------------
bool NiPhysXPSParticleSystemDest::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemDest::LoadBinary(NiStream& kStream)
{
    NiPhysXDest::LoadBinary(kStream);

    m_pkTarget = (NiPSParticleSystem*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemDest::LinkObject(NiStream& kStream)
{
    NiPhysXDest::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXPSParticleSystemDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXPSParticleSystemDest, pkObject));
    if(!NiPhysXDest::IsEqual(pkObject))
        return false;
        
    NiPhysXPSParticleSystemDest* pkNewObject = 
        (NiPhysXPSParticleSystemDest*)pkObject;

    if (!m_pkTarget->IsEqual(pkNewObject->m_pkTarget))
        return false;

    return true;
}
//---------------------------------------------------------------------------
