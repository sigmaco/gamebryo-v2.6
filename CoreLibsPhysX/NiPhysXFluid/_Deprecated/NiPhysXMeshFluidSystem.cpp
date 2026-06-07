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
#include "NiPhysXFluidPCH.h"

#include "NiPhysXMeshFluidSystem.h"
#include "NiPhysXFluidDest.h"

NiImplementRTTI(NiPhysXMeshFluidSystem, NiMeshParticleSystem);

//---------------------------------------------------------------------------
NiPhysXMeshFluidSystem::NiPhysXMeshFluidSystem()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshFluidSystem);
//---------------------------------------------------------------------------
void NiPhysXMeshFluidSystem::LoadBinary(NiStream& kStream)
{
    NiMeshParticleSystem::LoadBinary(kStream);

    m_spDestination = (NiPhysXFluidDest*)kStream.ResolveLinkID();

    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bPhysXSpace = ( bVal != 0 );
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidSystem::LinkObject(NiStream& kStream)
{
    NiMeshParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshFluidSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spDestination)
        m_spDestination->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidSystem::SaveBinary(NiStream& kStream)
{
    NiMeshParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_spDestination);

    NiStreamSaveBinary(kStream, NiBool(m_bPhysXSpace));
}
//---------------------------------------------------------------------------
bool NiPhysXMeshFluidSystem::IsEqual(NiObject* pkObject)
{
    if (!NiMeshParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXMeshFluidSystem* pkNewObject = (NiPhysXMeshFluidSystem*)pkObject;
    
    if (m_bPhysXSpace != pkNewObject->m_bPhysXSpace)
        return false;

    return true;
}
//---------------------------------------------------------------------------
