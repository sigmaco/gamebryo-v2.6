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

#include "NiPhysXMeshPSysProp.h"
#include "NiPhysXMeshParticleSystem.h"
#include "NiPhysXParticle.h"

NiImplementRTTI(NiPhysXMeshPSysProp, NiPhysXProp);    
        
//---------------------------------------------------------------------------
NiPhysXMeshPSysProp::NiPhysXMeshPSysProp()
{
}
//---------------------------------------------------------------------------
NiPhysXMeshPSysProp::~NiPhysXMeshPSysProp()
{
    m_kSystems.RemoveAll();
}
//---------------------------------------------------------------------------
NiUInt32 NiPhysXMeshPSysProp::GetSystemCount() const
{
    return m_kSystems.GetSize();
}
//---------------------------------------------------------------------------
NiPhysXMeshParticleSystem* NiPhysXMeshPSysProp::GetSystemAt(
    NiUInt32 uiIndex)
{
    return m_kSystems.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::AddSystem(NiPhysXMeshParticleSystem* pkSystem)
{
    m_kSystems.AddFirstEmpty(pkSystem);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysProp::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXProp::RegisterStreamables(kStream))
        return false;

    NiUInt32 uiSize = m_kSystems.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXMeshParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->RegisterStreamables(kStream);
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::SaveBinary(NiStream& kStream)
{
    NiPhysXProp::SaveBinary(kStream);

    NiUInt32 uiSize = m_kSystems.GetSize();
    NiStreamSaveBinary(kStream, m_kSystems.GetEffectiveSize());
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXMeshParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            kStream.SaveLinkID(pkSystem);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshPSysProp);
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::LoadBinary(NiStream& kStream)
{
    NiPhysXProp::LoadBinary(kStream);

    NiUInt32 uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    m_kSystems.SetSize(uiSize);
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        m_kSystems.SetAt(ui,
            (NiPhysXMeshParticleSystem*)kStream.ResolveLinkID());
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::LinkObject(NiStream& kStream)
{
    NiPhysXProp::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysProp::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysProp::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXMeshPSysProp, pkObject));
    if(!NiPhysXProp::IsEqual(pkObject))
        return false;
        
    NiPhysXMeshPSysProp* pkThatObject = (NiPhysXMeshPSysProp*)pkObject;
        
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
        NiPhysXMeshParticleSystem* pkSystemThis = 
            m_kSystems.GetAt(uiThis);
        while (uiThis < uiSizeThis && !(pkSystemThis))
        {
            uiThis++;
            pkSystemThis = m_kSystems.GetAt(uiThis);
        }

        NiPhysXMeshParticleSystem* pkSystemThat = 
             pkThatObject->m_kSystems.GetAt(uiThat);
        while (uiThat < uiSizeThat && !(pkSystemThat))
        {
            uiThat++;
            pkSystemThat = pkThatObject->m_kSystems.GetAt(uiThat);
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
