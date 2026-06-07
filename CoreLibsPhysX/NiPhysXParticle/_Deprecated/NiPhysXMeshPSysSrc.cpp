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

#include "NiParticle.h"

#include "NiPhysXSrc.h"
#include "NiPhysXPSysSrc.h"
#include "NiPhysXMeshPSysSrc.h"

NiImplementRTTI(NiPhysXMeshPSysSrc, NiPhysXPSysSrc);

//---------------------------------------------------------------------------
NiPhysXMeshPSysSrc::NiPhysXMeshPSysSrc()
{
}
//---------------------------------------------------------------------------
NiPhysXMeshPSysSrc::~NiPhysXMeshPSysSrc()
{
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysSrc::UpdateToActors(const float, const float)
{
    // This function deliberately left blank
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysSrc::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXPSysSrc::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysSrc::SaveBinary(NiStream& kStream)
{
    NiPhysXPSysSrc::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshPSysSrc);
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysSrc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysSrc::LoadBinary(NiStream& kStream)
{
    NiPhysXPSysSrc::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysSrc::LinkObject(NiStream& kStream)
{
    NiPhysXPSysSrc::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXMeshPSysSrc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXMeshPSysSrc, pkObject));
    if(!NiPhysXPSysSrc::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
