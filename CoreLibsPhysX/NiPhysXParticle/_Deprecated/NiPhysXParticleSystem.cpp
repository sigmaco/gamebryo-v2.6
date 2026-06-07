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

#include "NiPhysXParticleSystem.h"
#include <NiPSysPositionModifier.h>

#include "NiPhysXParticle.h"

NiImplementRTTI(NiPhysXParticleSystem, NiParticleSystem);

//---------------------------------------------------------------------------
NiPhysXParticleSystem::NiPhysXParticleSystem()
{
    m_spProp = 0;
    m_spDestination = 0;
    m_spSource = 0;
    m_bPhysXSpace = false;
}
//---------------------------------------------------------------------------
NiPhysXParticleSystem::~NiPhysXParticleSystem()
{
    m_spDestination = 0;
    m_spSource = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXParticleSystem);
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::LoadBinary(NiStream& kStream)
{
    NiParticleSystem::LoadBinary(kStream);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 2))
    {
        m_spProp = (NiPhysXPSysProp*)kStream.ResolveLinkID();
    }

    m_spDestination = (NiPhysXPSysDest*)kStream.ResolveLinkID();
    m_spSource = (NiPhysXPSysSrc*)kStream.ResolveLinkID();

    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bPhysXSpace = ( bVal != 0 );
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::LinkObject(NiStream& kStream)
{
    NiParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::PostLinkObject(NiStream& kStream)
{
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 2))
    {
        // Create a prop and add it to the scene, if it exists
        NiPhysXPSysData* pkPhysXData = (NiPhysXPSysData*)GetModelData();
        if (!pkPhysXData)
            return;

        NiPhysXScene* pkScene = pkPhysXData->GetScene();
        if (!pkScene)
            return;
            
        // Remove the sources and destinations from their current prop, or
        // from the scene. It depends on the order in which these post-link
        // functions get called.
        NiPhysXProp* pkRBProp = 0;
        for (unsigned int ui = 0; ui < pkScene->GetPropCount(); ui++)
        {
            NiPhysXProp* pkProp = pkScene->GetPropAt(ui);
            if (NiIsExactKindOf(NiPhysXProp, pkProp))
            {
                pkRBProp = pkProp;
                break;
            }
        }
        if (pkRBProp)
        {
            // Look for the regular prop
            pkRBProp->DeleteDestination(m_spDestination);
            pkRBProp->DeleteSource(m_spSource);
        }
        else
        {
            pkScene->DeleteDestination(m_spDestination);
            pkScene->DeleteSource(m_spSource);
        }
        
        m_spProp = 0;
        for (unsigned int ui = 0; ui < pkScene->GetPropCount(); ui++)
        {
            NiPhysXProp* pkProp = pkScene->GetPropAt(ui);
            if (NiIsKindOf(NiPhysXPSysProp, pkProp))
            {
                m_spProp = (NiPhysXPSysProp*)pkProp;
                break;
            }
        }
        if (!m_spProp)
        {
            m_spProp = NiNew NiPhysXPSysProp();
            pkScene->AddProp(m_spProp);
        }
        m_spProp->AddSystem(this);
        m_spProp->AddDestination(m_spDestination);
        m_spProp->AddSource(m_spSource);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXParticleSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }
    
    if (m_spProp)
        m_spProp->RegisterStreamables(kStream);
    if (m_spDestination)
        m_spDestination->RegisterStreamables(kStream);
    if (m_spSource)
        m_spSource->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::SaveBinary(NiStream& kStream)
{
    NiParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_spProp);
    kStream.SaveLinkID(m_spDestination);
    kStream.SaveLinkID(m_spSource);

    NiStreamSaveBinary(kStream, NiBool(m_bPhysXSpace));
}
//---------------------------------------------------------------------------
bool NiPhysXParticleSystem::IsEqual(NiObject* pkObject)
{
    if (!NiParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXParticleSystem* pkNewObject = (NiPhysXParticleSystem*)pkObject;
    
    if (m_bPhysXSpace != pkNewObject->m_bPhysXSpace)
        return false;

    return true;
}
//---------------------------------------------------------------------------

