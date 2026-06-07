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
#include "NiMeshPCH.h"

#include <NiAVObject.h>
#include <NiSystem.h>
#include <NiRenderObject.h>
#include "NiMesh.h"
#include "NiInstancingUtilities.h"
#include "NiInstancingMeshModifier.h"

NiImplementRTTI(NiMeshHWInstance, NiAVObject);

//---------------------------------------------------------------------------
NiMeshHWInstance::NiMeshHWInstance(NiMesh* pkMesh)
{
    SetAppCulled(true);

    m_pkMasterMesh = pkMesh;
    if (m_pkMasterMesh)
        m_pkMeshModifier = NiGetModifier(NiInstancingMeshModifier, pkMesh);
}
//---------------------------------------------------------------------------
NiMeshHWInstance::NiMeshHWInstance() :
    m_pkMasterMesh(NULL),
    m_pkMeshModifier(NULL)
{
   SetAppCulled(true);
}
//---------------------------------------------------------------------------
NiMeshHWInstance::~NiMeshHWInstance()
{
    NiInstancingUtilities::RemoveMeshInstance(m_pkMasterMesh, this);
    m_pkMasterMesh = 0;
}
//---------------------------------------------------------------------------
NiMesh* NiMeshHWInstance::GetMesh()
{
    return m_pkMasterMesh;
}
//---------------------------------------------------------------------------
void NiMeshHWInstance::SetMesh(NiMesh* pkMesh)
{
    m_pkMasterMesh = pkMesh;

    if (pkMesh)
        m_pkMeshModifier = NiGetModifier(NiInstancingMeshModifier, pkMesh);
    else
        m_pkMeshModifier = NULL;

}
//---------------------------------------------------------------------------
void NiMeshHWInstance::UpdateWorldBound()
{
    if (m_pkMasterMesh)
        m_kWorldBound.Update(m_pkMasterMesh->GetModelBound(), m_kWorld);
}
//---------------------------------------------------------------------------
void NiMeshHWInstance::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{
    NiAVObject::UpdateDownwardPass(kUpdate);

    if (m_pkMeshModifier)
        m_pkMeshModifier->MarkInstanceNodeTranformsDirty();
}
//---------------------------------------------------------------------------
void NiMeshHWInstance::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{
    NiAVObject::UpdateSelectedDownwardPass(kUpdate);    

    if (m_pkMeshModifier)
        m_pkMeshModifier->MarkInstanceNodeTranformsDirty();
}
//---------------------------------------------------------------------------
void NiMeshHWInstance::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    NiAVObject::UpdateRigidDownwardPass(kUpdate);

    if (m_pkMeshModifier)
        m_pkMeshModifier->MarkInstanceNodeTranformsDirty();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiMeshHWInstance);
//---------------------------------------------------------------------------
void NiMeshHWInstance::CopyMembers(NiMeshHWInstance* pkDest, 
    NiCloningProcess& kCloning) 
{
    NiAVObject::CopyMembers(pkDest, kCloning);   
}
//---------------------------------------------------------------------------
void NiMeshHWInstance::ProcessClone(NiCloningProcess& kCloning)
{
    NiAVObject::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned && pkClone->IsKindOf(GetRTTI()));
    if (bCloned && pkClone->IsKindOf(GetRTTI()))
    {
        NiMeshHWInstance* pkMeshInstClone = (NiMeshHWInstance*)pkClone;

        NIVERIFY(kCloning.m_pkCloneMap->GetAt(m_pkMasterMesh, pkClone));
        pkMeshInstClone->m_pkMasterMesh = (NiMesh*)pkClone;

        NIVERIFY(kCloning.m_pkCloneMap->GetAt(m_pkMeshModifier, pkClone));
        pkMeshInstClone->m_pkMeshModifier = (NiInstancingMeshModifier*)pkClone;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMeshHWInstance);
//---------------------------------------------------------------------------
void NiMeshHWInstance::LoadBinary(NiStream& kStream)
{
    NiAVObject::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkMasterMesh
    kStream.ReadLinkID();   // m_pkMeshModifier
}
//---------------------------------------------------------------------------
void NiMeshHWInstance::LinkObject(NiStream& kStream)
{
    NiAVObject::LinkObject(kStream);

    m_pkMasterMesh = (NiMesh*)kStream.GetObjectFromLinkID();
    m_pkMeshModifier = 
        (NiInstancingMeshModifier*)kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiMeshHWInstance::RegisterStreamables(NiStream& kStream)
{
    if (!NiAVObject::RegisterStreamables(kStream))
        return false;    

    return true;
}
//---------------------------------------------------------------------------
void NiMeshHWInstance::SaveBinary(NiStream& kStream)
{
    NiAVObject::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkMasterMesh);
    kStream.SaveLinkID(m_pkMeshModifier);
}
//---------------------------------------------------------------------------
bool NiMeshHWInstance::IsEqual(NiObject* pkObject)
{
    if (!NiAVObject::IsEqual(pkObject))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
void NiMeshHWInstance::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiAVObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiMeshHWInstance::ms_RTTI.GetName()));


    pkStrings->Add(NiGetViewerString("Master Mesh", m_pkMasterMesh));
    pkStrings->Add(NiGetViewerString("Instancing Mesh-modifier", 
        m_pkMeshModifier));

}
//---------------------------------------------------------------------------

