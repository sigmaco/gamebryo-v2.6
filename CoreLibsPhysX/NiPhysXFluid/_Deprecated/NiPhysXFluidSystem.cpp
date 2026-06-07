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

#include "NiPhysXFluidSystem.h"
#include "NiPhysXFluidDest.h"

NiImplementRTTI(NiPhysXFluidSystem, NiParticleSystem);

//---------------------------------------------------------------------------
NiPhysXFluidSystem::NiPhysXFluidSystem()
{
    m_spDestination = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidSystem);
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::LoadBinary(NiStream& kStream)
{
    NiParticleSystem::LoadBinary(kStream);

    m_spDestination = (NiPhysXFluidDest*)kStream.ResolveLinkID();

    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bPhysXSpace = ( bVal != 0 );
}
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::LinkObject(NiStream& kStream)
{
    NiParticleSystem::LinkObject(kStream);

}
//---------------------------------------------------------------------------
bool NiPhysXFluidSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spDestination)
        m_spDestination->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::SaveBinary(NiStream& kStream)
{
    NiParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_spDestination);

    NiStreamSaveBinary(kStream, NiBool(m_bPhysXSpace));
}
//---------------------------------------------------------------------------
bool NiPhysXFluidSystem::IsEqual(NiObject* pkObject)
{
    if (!NiParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXFluidSystem* pkNewObject = (NiPhysXFluidSystem*)pkObject;
    
    if (m_bPhysXSpace != pkNewObject->m_bPhysXSpace)
        return false;

    return true;
}
//---------------------------------------------------------------------------
