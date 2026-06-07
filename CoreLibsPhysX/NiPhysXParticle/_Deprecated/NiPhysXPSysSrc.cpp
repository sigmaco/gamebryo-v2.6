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

#include "NiPhysXSrc.h"
#include "NiPhysXPSysSrc.h"

NiImplementRTTI(NiPhysXPSysSrc, NiPhysXSrc);

//---------------------------------------------------------------------------
NiPhysXPSysSrc::NiPhysXPSysSrc() : m_pkSource(0)
{
}
//---------------------------------------------------------------------------
NiPhysXPSysSrc::~NiPhysXPSysSrc()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPSysSrc::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXSrc::RegisterStreamables(kStream))
        return false;

    m_pkSource->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysSrc::SaveBinary(NiStream& kStream)
{
    NiPhysXSrc::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkSource);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSysSrc);
//---------------------------------------------------------------------------
bool NiPhysXPSysSrc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysSrc::LoadBinary(NiStream& kStream)
{
    NiPhysXSrc::LoadBinary(kStream);

    m_pkSource = (NiParticleSystem*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXPSysSrc::LinkObject(NiStream& kStream)
{
    NiPhysXSrc::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXPSysSrc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXPSysSrc, pkObject));
    if(!NiPhysXSrc::IsEqual(pkObject))
        return false;
        
    NiPhysXPSysSrc* pkNewObject = (NiPhysXPSysSrc*)pkObject;

    if (!m_pkSource->IsEqual(pkNewObject->m_pkSource))
        return false;

    return true;
}
//---------------------------------------------------------------------------
