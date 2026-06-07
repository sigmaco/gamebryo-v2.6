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

#include "NiParticleSystem.h"

#include "NiPhysXDest.h"
#include "NiPhysXPSysDest.h"

NiImplementRTTI(NiPhysXPSysDest, NiPhysXDest);

//---------------------------------------------------------------------------
NiPhysXPSysDest::NiPhysXPSysDest() : m_pkTarget(0)
{
}
//---------------------------------------------------------------------------
NiPhysXPSysDest::~NiPhysXPSysDest()
{
    m_pkTarget = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPSysDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXDest::RegisterStreamables(kStream))
        return false;

    m_pkTarget->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysDest::SaveBinary(NiStream& kStream)
{
    NiPhysXDest::SaveBinary(kStream);
    
    kStream.SaveLinkID(m_pkTarget);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSysDest);
//---------------------------------------------------------------------------
bool NiPhysXPSysDest::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysDest::LoadBinary(NiStream& kStream)
{
    NiPhysXDest::LoadBinary(kStream);

    m_pkTarget = (NiParticleSystem*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXPSysDest::LinkObject(NiStream& kStream)
{
    NiPhysXDest::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXPSysDest, pkObject));
    if(!NiPhysXDest::IsEqual(pkObject))
        return false;

    NiPhysXPSysDest* pkNewObject = (NiPhysXPSysDest*)pkObject;

    if (!m_pkTarget->IsEqual(pkNewObject->m_pkTarget))
        return false;

    return true;
}
//---------------------------------------------------------------------------
