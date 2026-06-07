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

#include "NiPhysXFluidPCH.h"

#include "NiPhysXFluid.h"

NiImplementRTTI(NiPhysXFluidPropDesc, NiPhysXPropDesc);

//---------------------------------------------------------------------------
NiPhysXFluidPropDesc::NiPhysXFluidPropDesc(NiUInt32 uiMaterialHashSize)
    : NiPhysXPropDesc(uiMaterialHashSize)
{
}
//---------------------------------------------------------------------------
NiPhysXFluidPropDesc::~NiPhysXFluidPropDesc()
{
    m_kFluids.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiPhysXFluidPropDesc::CreateInScene(NxScene* pkScene,
    const NxMat34& kXform, NxCompartment* pkRBCompartment,
    NxCompartment* pkClothCompartment, NxCompartment* pkFluidCompartment,
    const bool bKeepMeshes, const NiUInt32 uiIndex)
{
    NIASSERT(pkScene != 0);

    // Create everything else first
    if (!NiPhysXPropDesc::CreateInScene(pkScene, kXform, pkRBCompartment,
        pkClothCompartment, pkFluidCompartment, bKeepMeshes, uiIndex))
    {
        return false;
    }
    
    for (NiUInt32 ui = 0; ui < m_kFluids.GetSize(); ui++)
    {
        if (!m_kFluids.GetAt(ui)->CreateFluid(
            pkScene, pkFluidCompartment, kXform))
        {
            return false;
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidPropDesc::RemoveFromScene(NxScene* pkScene)
{
    NiPhysXPropDesc::RemoveFromScene(pkScene);
    
    for (NiUInt32 ui = 0; ui < m_kFluids.GetSize(); ui++)
    {
        m_kFluids.GetAt(ui)->RemoveFluid(pkScene);
    }
}
//---------------------------------------------------------------------------
void NiPhysXFluidPropDesc::RestoreState(NxScene* pkScene,
    const NiUInt32 uiIndex, const NxMat34& kXform, const bool bKeepMeshes)
{
    if (NiSystemDesc::GetSystemDesc().GetPlatformID() ==
        NiSystemDesc::NI_WIN32)    
    {
        for (NiUInt32 ui = 0; ui < m_kFluids.GetSize(); ui++)
        {
            NiPhysXFluidDesc* pkFluidDesc = m_kFluids.GetAt(ui);
            if (pkFluidDesc->GetFluid())
            {
                pkFluidDesc->GetFluid()->removeAllParticles();
                pkFluidDesc->GetParticleSystem()->ResetParticleSystem();
            }
        }
    }

    NiPhysXPropDesc::RestoreState(pkScene, uiIndex, kXform, bKeepMeshes);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXFluidPropDesc);
//---------------------------------------------------------------------------
void NiPhysXFluidPropDesc::CopyMembers(NiPhysXFluidPropDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXPropDesc::CopyMembers(pkDest, kCloning);

    // Clone the fluids
    pkDest->m_kFluids.SetSize(m_kFluids.GetSize());
    for (NiUInt32 ui = 0; ui < m_kFluids.GetSize(); ui++)
    {
        NiPhysXFluidDescPtr spFluidClone = (NiPhysXFluidDesc*)
            m_kFluids.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kFluids.SetAt(ui, spFluidClone);
    }
}
//---------------------------------------------------------------------------
void NiPhysXFluidPropDesc::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXPropDesc::ProcessClone(kCloning);

    for (NiUInt32 ui = 0; ui < m_kFluids.GetSize(); ui++)
    {
        m_kFluids.GetAt(ui)->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXFluidPropDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXPropDesc::RegisterStreamables(kStream))
        return false;

    for (NiUInt32 i = 0; i < m_kFluids.GetSize(); i++)
    {
        NiPhysXFluidDescPtr spFluid = m_kFluids.GetAt(i);
        NIASSERT(spFluid);
        spFluid->RegisterStreamables(kStream);
    }
     
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidPropDesc::SaveBinary(NiStream& kStream)
{
    NiPhysXPropDesc::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_kFluids.GetEffectiveSize());
    for (NiUInt32 i = 0; i < m_kFluids.GetSize(); i++)
    {
        NiPhysXFluidDescPtr spFluid = m_kFluids.GetAt(i);
        NIASSERT(spFluid);
        kStream.SaveLinkID(spFluid);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidPropDesc);
//---------------------------------------------------------------------------
void NiPhysXFluidPropDesc::LoadBinary(NiStream& kStream)
{
    NiPhysXPropDesc::LoadBinary(kStream);

    NiUInt32 uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    m_kFluids.SetSize(uiSize);
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        m_kFluids.SetAt(i, (NiPhysXFluidDesc*)kStream.ResolveLinkID());
    }
}
//---------------------------------------------------------------------------
void NiPhysXFluidPropDesc::LinkObject(NiStream& kStream)
{
    NiPhysXPropDesc::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXFluidPropDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXFluidPropDesc, pkObject));
    if(!NiPhysXPropDesc::IsEqual(pkObject))
        return false;
        
    NiPhysXFluidPropDesc* pkNewObject = (NiPhysXFluidPropDesc*)pkObject;

    for (NiUInt32 i = 0; i < m_kFluids.GetSize(); i++)
    {
        NiPhysXFluidDescPtr spFluid = m_kFluids.GetAt(i);
        NiPhysXFluidDescPtr spNewFluid = pkNewObject->m_kFluids.GetAt(i);
        if (spFluid)
        {
            if (!spNewFluid)
                return false;
            if (!spFluid->IsEqual(spNewFluid))
                return false;
        }
        else
        {
            if (spNewFluid)
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
