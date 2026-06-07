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

#include "NiPhysXPSParticleSystemProp.h"
#include "NiPhysXPSParticleSystem.h"
#include "NiPhysXParticle.h"

NiImplementRTTI(NiPhysXPSParticleSystemProp, NiPhysXProp);    
        
//---------------------------------------------------------------------------
NiPhysXPSParticleSystemProp::NiPhysXPSParticleSystemProp()
{
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystemProp::~NiPhysXPSParticleSystemProp()
{
    for (NiUInt32 ui = m_kSystems.GetSize(); ui > 0 ; --ui)
    {
        NiPhysXPSParticleSystemPtr spPSys = m_kSystems.GetAt(ui - 1);
        m_kSystems.GetAt(ui - 1)->DetachFromProp();
        spPSys = 0;
    }
}
//---------------------------------------------------------------------------
NiUInt32 NiPhysXPSParticleSystemProp::GetSystemCount() const
{
    return m_kSystems.GetSize();
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystem* NiPhysXPSParticleSystemProp::GetSystemAt(
    NiUInt32 uiIndex)
{
    return m_kSystems.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemProp::AddSystem(NiPhysXPSParticleSystem* pkSystem)
{
    m_kSystems.AddFirstEmpty(pkSystem);
    AddModifiedMesh(pkSystem);
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemProp::RemoveSystem(
    NiPhysXPSParticleSystem* pkSystem)
{
    m_kSystems.Remove(pkSystem);
    DeleteModifiedMesh(pkSystem);
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemProp::AttachSceneCallback(NiPhysXScene* pkScene)
{
    NiPhysXProp::AttachSceneCallback(pkScene);

    NiUInt32 uiSize = m_kSystems.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXPSParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->AttachToScene(pkScene);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemProp::DetachSceneCallback(NiPhysXScene* pkScene)
{
    NiUInt32 uiSize = m_kSystems.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXPSParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
        {
            pkSystem->DetachFromScene();
        }
    }

    NiPhysXProp::DetachSceneCallback(pkScene);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSParticleSystemProp);
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemProp::CopyMembers(
    NiPhysXPSParticleSystemProp* pkDest, NiCloningProcess& kCloning)
{
    NiPhysXProp::CopyMembers(pkDest, kCloning);

    NiUInt32 uiSize = m_kSystems.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXPSParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
        {
            NiPhysXPSParticleSystem* pkNewSystem =
                (NiPhysXPSParticleSystem*)
                pkSystem->CreateSharedClone(kCloning);
            pkDest->AddSystem(pkNewSystem);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemProp::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXProp::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;

    NiUInt32 uiSize = m_kSystems.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXPSParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPSParticleSystemProp::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXProp::RegisterStreamables(kStream))
        return false;

    NiUInt32 uiSize = m_kSystems.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXPSParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->RegisterStreamables(kStream);
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemProp::SaveBinary(NiStream& kStream)
{
    NiPhysXProp::SaveBinary(kStream);

    NiUInt32 uiSize = m_kSystems.GetSize();
    NiStreamSaveBinary(kStream, m_kSystems.GetEffectiveSize());
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXPSParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            kStream.SaveLinkID(pkSystem);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSParticleSystemProp);
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemProp::LoadBinary(NiStream& kStream)
{
    NiPhysXProp::LoadBinary(kStream);

    NiUInt32 uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    m_kSystems.SetSize(uiSize);
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        m_kSystems.SetAt(ui, 
            (NiPhysXPSParticleSystem*)kStream.ResolveLinkID());
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystemProp::LinkObject(NiStream& kStream)
{
    NiPhysXProp::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSParticleSystemProp::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXPSParticleSystemProp::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXPSParticleSystemProp, pkObject));
    if(!NiPhysXProp::IsEqual(pkObject))
        return false;
        
    NiPhysXPSParticleSystemProp* pkThatObject = 
        (NiPhysXPSParticleSystemProp*)pkObject;
        
    NiUInt32 uiEffectiveThis = m_kSystems.GetEffectiveSize();
    NiUInt32 uiEffectiveThat = pkThatObject->m_kSystems.GetEffectiveSize();
    if (uiEffectiveThis != uiEffectiveThat)
        return false;

    NiUInt32 uiSizeThis = m_kSystems.GetSize(); 
    NiUInt32 uiSizeThat = pkThatObject->m_kSystems.GetSize();
    NiUInt32 uiThis = 0;
    NiUInt32 uiThat = 0;
    bool bDone = false;
    do
    {
        NiPhysXPSParticleSystem* pkSystemThis = NULL;
        while (uiThis < uiSizeThis)
        {
            pkSystemThis = m_kSystems.GetAt(uiThis);
            if (pkSystemThis)
            {
                break;
            }
            uiThis++;
        }

        NiPhysXPSParticleSystem* pkSystemThat = NULL;
        while (uiThat < uiSizeThat)
        {
            pkSystemThat = pkThatObject->m_kSystems.GetAt(uiThat);
            if (pkSystemThat)
            {
                break;
            }
            uiThat++;
        }

        if (uiThis == uiSizeThis && uiThat == uiSizeThat)
            bDone = true; // All is good
        else if (uiThis == uiSizeThis && uiThat != uiSizeThat)
            return false; // More systems in That than in This
        else if (uiThis != uiSizeThis && uiThat == uiSizeThat)
            return false; // More systems in This than in That
        else if (!pkSystemThis->IsEqual(pkSystemThat))
            return false;
            
        uiThis++;
        uiThat++;
    } while (!bDone);

    return true;
}
//---------------------------------------------------------------------------
