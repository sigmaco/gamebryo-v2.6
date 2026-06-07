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

#include "NiPhysXPSSimulator.h"

NiImplementRTTI(NiPhysXPSSimulator, NiPSSimulator);

//---------------------------------------------------------------------------
NiPhysXPSSimulator::NiPhysXPSSimulator() : NiPSSimulator()
{
}
//---------------------------------------------------------------------------
NiPhysXPSSimulator::~NiPhysXPSSimulator()
{
}
//---------------------------------------------------------------------------
bool NiPhysXPSSimulator::Attach(NiMesh* pkMesh)
{
    if (NiPSSimulator::Attach(pkMesh))
    {
        AddCompleteSyncPoint(NiSyncArgs::SYNC_PHYSICS_SIMULATE);
    
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------
bool NiPhysXPSSimulator::Detach(NiMesh* pkMesh)
{
    if (NiPSSimulator::Detach(pkMesh))
    {
        RemoveCompleteSyncPoint(NiSyncArgs::SYNC_PHYSICS_SIMULATE);

        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSSimulator);
//---------------------------------------------------------------------------
void NiPhysXPSSimulator::CopyMembers(NiPhysXPSSimulator* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSSimulator::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPSSimulator::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSSimulator::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulator::SaveBinary(NiStream& kStream)
{
    NiPSSimulator::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSSimulator);
//---------------------------------------------------------------------------
void NiPhysXPSSimulator::LoadBinary(NiStream& kStream)
{
    NiPSSimulator::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulator::LinkObject(NiStream& kStream)
{
    NiPSSimulator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSSimulator::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXPSSimulator, pkObject));
    if(!NiPSSimulator::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXPSSimulator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSSimulator::GetViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
