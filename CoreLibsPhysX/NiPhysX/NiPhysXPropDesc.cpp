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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

NiImplementRTTI(NiPhysXPropDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXPropDesc::NiPhysXPropDesc(NiUInt32 uiMaterialHashSize,
    NiUInt32 uiStateMapSize) : m_kActors(0,5), m_kJoints(0,5), m_kClothes(0,5),
    m_kMaterials(uiMaterialHashSize), m_kStateNameMap(uiStateMapSize)
{
    m_uiRBCompartmentID = 0;
    m_uiClothCompartmentID = 0;
    m_uiFluidCompartmentID = 0;
    m_uiNumStates = 0;
    m_uFlags = KEEP_DEFAULT_STATE_MASK;
}
//---------------------------------------------------------------------------
NiPhysXPropDesc::~NiPhysXPropDesc()
{
    m_kActors.RemoveAll();
    m_kJoints.RemoveAll();
    m_kClothes.RemoveAll();
    m_kMaterials.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiPhysXPropDesc::CreateInScene(NxScene* pkScene, const NxMat34& kXform,
    NxCompartment* pkRBCompartment, NxCompartment* pkClothCompartment,
    NxCompartment*, const bool bKeepMeshes,
    const NiUInt32 uiIndex)
{
    NIASSERT(uiIndex < m_uiNumStates);
    
    // Set all the materials.
    NiTMapIterator kIter = m_kMaterials.GetFirstPos();
    while (kIter)
    {
        // Get the material from our descriptors.
        NxMaterialIndex usMaterialIndex = 0;
        NiPhysXMaterialDescPtr spMaterial = 0;
        m_kMaterials.GetNext(kIter, usMaterialIndex, spMaterial);
        
        if (usMaterialIndex == 0)
        {
            // This is the default material, so do nothing.
            continue;
        }
        
        // Make sure PhysX has the required index. Just keep asking for
        // new materials until one comes back with the right index.
        NxMaterialDesc kMaterialDesc;
        NxMaterial* pkMaterial =
            pkScene->getMaterialFromIndex(usMaterialIndex);
        while (pkMaterial->getMaterialIndex() == 0)
        {
            pkScene->createMaterial(kMaterialDesc);
            pkMaterial = pkScene->getMaterialFromIndex(usMaterialIndex);
        }
        
        // Save our material into the PhysX material.
        spMaterial->ToMaterialDesc(kMaterialDesc, uiIndex);
        pkMaterial->loadFromDesc(kMaterialDesc);
    }
    
    // Add all the actors
    for (NiUInt32 i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->CreateActor(pkScene, pkRBCompartment, uiIndex, kXform,
            bKeepMeshes);
        if (!spActor->GetActor())
        {
            RemoveFromScene(pkScene);
            return false;
        }
    }    
    
    // Add all the joints
    for (NiUInt32 i = 0; i < m_kJoints.GetSize(); i++)
    {
        NiPhysXJointDescPtr spJoint = m_kJoints.GetAt(i);
        if (spJoint)
        {
            NiPhysXD6JointDescPtr spD6Joint =
                NiDynamicCast(NiPhysXD6JointDesc, spJoint);
            spD6Joint->CreateJoint(pkScene, kXform);
            if (!spD6Joint->GetJoint())
            {
                RemoveFromScene(pkScene);
                return false;
            }
        }
    }
    
    // Add all the cloth
    for (NiUInt32 i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->CreateCloth(pkScene, pkClothCompartment, uiIndex, kXform,
            bKeepMeshes);
        if (!spCloth->GetCloth())
        {
            RemoveFromScene(pkScene);
            return false;
        }
    }
        
    PushToSrcDest();
    
    RemoveStates();

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::RemoveFromScene(NxScene* pkScene)
{
    // We don't free materials because they are shared and someone else
    // may be using them.
    
    // Remove all the cloth, which may reference actors
    for (NiUInt32 i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->RemoveCloth(pkScene);
    }    

    // Remove all joints. They reference actors.
    for (NiUInt32 i = 0; i < m_kJoints.GetSize(); i++)
    {
        NiPhysXJointDescPtr spJoint = m_kJoints.GetAt(i);
        if (spJoint)
        {
            NiPhysXD6JointDescPtr spD6Joint =
                NiDynamicCast(NiPhysXD6JointDesc, spJoint);
            spD6Joint->RemoveJoint(pkScene);
        }
    }

    // Remove all the actors
    for (NiUInt32 i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->RemoveActor(pkScene);
    }    
}
//---------------------------------------------------------------------------
NiUInt32 NiPhysXPropDesc::AddState(NxScene* pkPhysXScene,
    const NxMat34& kXform, NiFixedString& kName)
{
    for (NiUInt32 i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->AddState(kXform);
    }
    
    for (NiUInt32 i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->AddState(kXform);
    }
    
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        NIASSERT(spMaterialDesc != 0);
        NxMaterial* pkNxMaterial = pkPhysXScene->getMaterialFromIndex(usKey);
        spMaterialDesc->FromMaterial(pkNxMaterial, m_uiNumStates);
    }
    
    if (kName.Exists())
        m_kStateNameMap.SetAt(kName, m_uiNumStates);
    
    m_uiNumStates++;
    
    return m_uiNumStates;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::RestoreState(NxScene* pkScene,
    const NiUInt32 uiIndex, const NxMat34& kXform, const bool bKeepMeshes)
{
    // Remove all the cloth. Cloth does not allow reset of torn vertices to
    // an untorn state, so our solution is to remove the cloth and re-add it.
    NxCompartment* pkClothCompartment = 0;
    for (NiUInt32 i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        pkClothCompartment = spCloth->GetCloth()->getCompartment();
        spCloth->RemoveCloth(pkScene);
    }

    for (NiUInt32 i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->RestoreState(uiIndex, kXform);
    }
    
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        NIASSERT(spMaterialDesc != 0);
        NxMaterial* pkNxMaterial = pkScene->getMaterialFromIndex(usKey);
        spMaterialDesc->ToMaterial(pkNxMaterial, uiIndex);
    }
    
    // Add all the cloth back in
    for (NiUInt32 i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->CreateCloth(pkScene, pkClothCompartment, uiIndex, kXform,
            bKeepMeshes);
    }

    // Here we could restore broken joints
}
//---------------------------------------------------------------------------
NxMaterialIndex NiPhysXPropDesc::AddMaterial(NxMaterialDesc& kMaterial,
    const NxMaterialIndex usBaseIndex)
{
    NiUInt16 usMaxIndex = 0;
    
    // Check if it's already there
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NiUInt16 usIndex;
        NiPhysXMaterialDescPtr spMaterial;
        m_kMaterials.GetNext(iter, usIndex, spMaterial);
        
        if (spMaterial->IsDataEqual(kMaterial))
            return usIndex;
            
        if (usIndex > usMaxIndex)
            usMaxIndex = usIndex;
    }
    
    usMaxIndex++;
    if (usMaxIndex < usBaseIndex)
        usMaxIndex = usBaseIndex;
    
    NiPhysXMaterialDesc* pkNewMaterial = NiNew NiPhysXMaterialDesc(usMaxIndex);
    pkNewMaterial->FromMaterialDesc(kMaterial);
    
    m_kMaterials.SetAt(usMaxIndex, pkNewMaterial);
    
    return usMaxIndex;
}
//---------------------------------------------------------------------------
NiPhysXMaterialDescPtr NiPhysXPropDesc::SetMaterialAt(
    NxMaterialDesc& kMaterial, const NxMaterialIndex usIndex)
{
    NiPhysXMaterialDescPtr spExisting;
    
    if (!m_kMaterials.GetAt(usIndex, spExisting))
        spExisting = 0;
    
    NiPhysXMaterialDesc* pkNewMaterial = NiNew NiPhysXMaterialDesc(usIndex);
    pkNewMaterial->FromMaterialDesc(kMaterial);
    m_kMaterials.SetAt(usIndex, pkNewMaterial);
    
    return spExisting;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::PushToSrcDest() const
{
    for (NiUInt32 i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        if (spActor->GetSource())
            spActor->GetSource()->SetTarget(spActor->GetActor());
        if (spActor->GetDest())
        {
            spActor->GetDest()->SetActor(spActor->GetActor());
            if (spActor->GetActorParent())
            {
                spActor->GetDest()->SetActorParent(
                    spActor->GetActorParent()->GetActor());
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::RemoveStates()
{
    if (GetBit(KEEP_ALL_STATES_MASK))
    {
        return;
    }

    NiUInt32 uiState = 0;
    if (GetBit(KEEP_DEFAULT_STATE_MASK))
        uiState = 1;

    for (NiUInt32 i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->RemoveStatesBeyond(uiState);
    }

    for (NiUInt32 i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->RemoveStatesBeyond(uiState);
    }
    
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        NIASSERT(spMaterialDesc != 0);
        spMaterialDesc->RemoveStatesBeyond(uiState);
    }
    
    // Remove entries in state name map
    m_kStateNameMap.RemoveAll();
    if (GetBit(KEEP_DEFAULT_STATE_MASK))
    {
        NiFixedString kDefault("Default");
        m_kStateNameMap.SetAt(kDefault, 0);
    }

    m_uiNumStates = uiState;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::RemoveState(NiUInt32 uiState)
{
    if (uiState >= m_uiNumStates)
        return;
        
    for (NiUInt32 i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->RemoveState(uiState);
    }

    for (NiUInt32 i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->RemoveState(uiState);
    }
    
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        NIASSERT(spMaterialDesc != 0);
        spMaterialDesc->RemoveState(uiState);
    }

    // Update entries in state name map
    iter = m_kStateNameMap.GetFirstPos();
    NiFixedString kNameToRemove;
    while (iter)
    {
        NiFixedString kName;
        NiUInt32 uiIndex;
        m_kStateNameMap.GetNext(iter, kName, uiIndex);
        
        if (uiIndex == uiState)
        {
            kNameToRemove = kName;
        }
        else if (uiIndex > uiState)
        {
            m_kStateNameMap.SetAt(kName, uiIndex - 1);
        }
    }
    m_kStateNameMap.RemoveAt(kNameToRemove);
    
    m_uiNumStates--;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPropDesc);     
//---------------------------------------------------------------------------
void NiPhysXPropDesc::CopyMembers(NiPhysXPropDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    // Clone the actors
    pkDest->m_kActors.SetSize(m_kActors.GetSize());
    for (NiUInt32 ui = 0; ui < m_kActors.GetSize(); ui++)
    {
        NiPhysXActorDescPtr spActorClone = (NiPhysXActorDesc*)
            m_kActors.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kActors.SetAt(ui, spActorClone);
    }

    // Clone the joints
    pkDest->m_kJoints.SetSize(m_kJoints.GetSize());
    for (NiUInt32 ui = 0; ui < m_kJoints.GetSize(); ui++)
    {
        NiPhysXJointDescPtr spJointClone = (NiPhysXJointDesc*)
            m_kJoints.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kJoints.SetAt(ui, spJointClone);
    }
    
    // Clone the clothes
    pkDest->m_kClothes.SetSize(m_kClothes.GetSize());
    for (NiUInt32 ui = 0; ui < m_kClothes.GetSize(); ui++)
    {
        NiPhysXClothDescPtr spClothClone = (NiPhysXClothDesc*)
            m_kClothes.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kClothes.SetAt(ui, spClothClone);
    }

    // Clone materials. Clones use the same index, which is what we want.
    // If two clones are merged in the same scene, their indexes will be
    // the same, and so they continue to have the same material.
    pkDest->m_kMaterials.Resize(
        NiTMap<NiUInt32, NiUInt32>::NextPrime(m_kMaterials.GetCount()));
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        
        NiPhysXMaterialDesc* spMaterialClone = (NiPhysXMaterialDesc*)
            spMaterialDesc->CreateSharedClone(kCloning);
        pkDest->m_kMaterials.SetAt(usKey, spMaterialClone);
    }
    
    // Clone the map of state names
    pkDest->m_uiNumStates = m_uiNumStates;
    pkDest->m_kStateNameMap.Resize(
        NiTMap<NiUInt32, NiUInt32>::NextPrime(m_uiNumStates));
    iter = m_kStateNameMap.GetFirstPos();
    while (iter)
    {
        NiFixedString kName;
        NiUInt32 uiIndex;
        m_kStateNameMap.GetNext(iter, kName, uiIndex);
        pkDest->m_kStateNameMap.SetAt(kName, uiIndex);
    }
    
    pkDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);
    
    // Process the actors
    for (NiUInt32 ui = 0; ui < m_kActors.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXActorDesc* pkActor = m_kActors.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkActor, bJunk))
            pkActor->ProcessClone(kCloning);
    }

    // Process the joints
    for (NiUInt32 ui = 0; ui < m_kJoints.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXJointDesc* pkJoint = m_kJoints.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkJoint, bJunk))
            pkJoint->ProcessClone(kCloning);
    }
    
    // Process the clothes
    for (NiUInt32 ui = 0; ui < m_kClothes.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXClothDesc* pkCloth = m_kClothes.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkCloth, bJunk))
            pkCloth->ProcessClone(kCloning);
    }

    // Process materials.
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        
        bool bJunk;
        if (!kCloning.m_pkProcessMap->GetAt(spMaterialDesc, bJunk))
            spMaterialDesc->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPropDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    for (NiUInt32 i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->RegisterStreamables(kStream);
    }
    for (NiUInt32 i = 0; i < m_kJoints.GetSize(); i++)
    {
        NiPhysXJointDescPtr spJoint = m_kJoints.GetAt(i);
        NIASSERT(spJoint != 0);
        spJoint->RegisterStreamables(kStream);
    }
    for (NiUInt32 i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->RegisterStreamables(kStream);
    }
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NiUInt16 usKey;
        NiPhysXMaterialDescPtr spMaterial;
        m_kMaterials.GetNext(iter, usKey, spMaterial);
        spMaterial->RegisterStreamables(kStream);
    }
    
    iter = m_kStateNameMap.GetFirstPos();
    while (iter)
    {
        NiFixedString kName;
        NiUInt32 uiIndex;
        m_kStateNameMap.GetNext(iter, kName, uiIndex);
        kStream.RegisterFixedString(kName);
    }
     
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    NiStreamSaveBinary(kStream, m_kActors.GetEffectiveSize());
    for (NiUInt32 i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        kStream.SaveLinkID(spActor);
    }
    NiStreamSaveBinary(kStream, m_kJoints.GetEffectiveSize());
    for (NiUInt32 i = 0; i < m_kJoints.GetSize(); i++)
    {
        NiPhysXJointDescPtr spJoint = m_kJoints.GetAt(i);
        NIASSERT(spJoint != 0);
        kStream.SaveLinkID(spJoint);
    }
    NiStreamSaveBinary(kStream, m_kClothes.GetEffectiveSize());
    for (NiUInt32 i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        kStream.SaveLinkID(spCloth);
    }
    NiStreamSaveBinary(kStream, m_kMaterials.GetCount());
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NiUInt16 usKey;
        NiPhysXMaterialDescPtr spMaterial;
        m_kMaterials.GetNext(iter, usKey, spMaterial);
        NiStreamSaveBinary(kStream, usKey);
        kStream.SaveLinkID(spMaterial);
    }

    NiStreamSaveBinary(kStream, m_uiNumStates);

    NiStreamSaveBinary(kStream, m_kStateNameMap.GetCount());
    iter = m_kStateNameMap.GetFirstPos();
    while (iter)
    {
        NiFixedString kName;
        NiUInt32 uiIndex;
        m_kStateNameMap.GetNext(iter, kName, uiIndex);
        kStream.SaveFixedString(kName);
        NiStreamSaveBinary(kStream, uiIndex);
    }
    
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPropDesc);
//---------------------------------------------------------------------------
void NiPhysXPropDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
    
    NiUInt32 uiSize;

    NiStreamLoadBinary(kStream, uiSize);
    m_kActors.SetSize(uiSize);
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        m_kActors.SetAt(i, (NiPhysXActorDesc*)kStream.ResolveLinkID());
    }

    NiStreamLoadBinary(kStream, uiSize);
    m_kJoints.SetSize(uiSize);
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        m_kJoints.SetAt(i, (NiPhysXJointDesc*)kStream.ResolveLinkID());
    }
    
    if (kStream.GetFileVersion() >= kStream.GetVersion(20, 3, 0, 5))
    {
        NiStreamLoadBinary(kStream, uiSize);
        m_kClothes.SetSize(uiSize);
        for (NiUInt32 i = 0; i < uiSize; i++)
        {
            m_kClothes.SetAt(i, (NiPhysXClothDesc*)kStream.ResolveLinkID());
        }
    }

    NiStreamLoadBinary(kStream, uiSize);
    m_kMaterials.Resize(NiTMap<NiUInt32, NiUInt32>::NextPrime(uiSize));
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        NiUInt16 usKey;
        NiStreamLoadBinary(kStream, usKey);
        NiPhysXMaterialDescPtr spMat =
            (NiPhysXMaterialDesc*)kStream.ResolveLinkID();
        m_kMaterials.SetAt(usKey, spMat);
    }
    
    NiStreamLoadBinary(kStream, m_uiNumStates);

    if (kStream.GetFileVersion() >= kStream.GetVersion(20, 4, 0, 0))
    {
        NiUInt32 uiNumStateNames;
        NiStreamLoadBinary(kStream, uiNumStateNames);
        
        m_kStateNameMap.Resize(
            NiTMap<NiUInt32, NiUInt32>::NextPrime(uiNumStateNames));
        for (NiUInt32 ui = 0; ui < uiNumStateNames; ui++)
        {
            NiFixedString kName;
            kStream.LoadFixedString(kName);
            NiUInt32 uiIndex;
            NiStreamLoadBinary(kStream, uiIndex);
            m_kStateNameMap.SetAt(kName, uiIndex);
        }
    }
    else
    {
        m_kMaterials.Resize(3);
        NiFixedString kName("Default");
        m_kStateNameMap.SetAt(kName, 0);
    }

    if (kStream.GetFileVersion() >= kStream.GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_uFlags);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPropDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXPropDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXPropDesc* pkNewObject = (NiPhysXPropDesc*)pkObject;

    for (NiUInt32 i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NiPhysXActorDescPtr spNewActor = pkNewObject->m_kActors.GetAt(i);
        if (spActor)
        {
            if (!spNewActor)
                return false;
            if (!spActor->IsEqual(spNewActor))
                return false;
        }
        else
        {
            if (spNewActor)
                return false;
        }
    }

    for (NiUInt32 i = 0; i < m_kJoints.GetSize(); i++)
    {
        NiPhysXJointDescPtr spJoint = m_kJoints.GetAt(i);
        NiPhysXJointDescPtr spNewJoint = pkNewObject->m_kJoints.GetAt(i);
        if (spJoint)
        {
            if (!spNewJoint)
                return false;
            if (!spJoint->IsEqual(spNewJoint))
                return false;
        }
        else
        {
            if (spNewJoint)
                return false;
        }
    }

    for (NiUInt32 i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NiPhysXClothDescPtr spNewCloth = pkNewObject->m_kClothes.GetAt(i);
        if (spCloth)
        {
            if (!spNewCloth)
                return false;
            if (!spCloth->IsEqual(spNewCloth))
                return false;
        }
        else
        {
            if (spNewCloth)
                return false;
        }
    }

    if (m_kMaterials.GetCount() != pkNewObject->m_kMaterials.GetCount())
        return false;

    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NiUInt16 usKey;
        NiPhysXMaterialDescPtr spMaterial;
        m_kMaterials.GetNext(iter, usKey, spMaterial);
        
        NiPhysXMaterialDescPtr spNewMat;
        if (!pkNewObject->m_kMaterials.GetAt(usKey, spNewMat))
            return false;
        if (!spMaterial->IsEqual(spNewMat))
            return false;
    }

    if (m_uiNumStates != pkNewObject->m_uiNumStates)
        return false;

    if (m_kStateNameMap.GetCount() != pkNewObject->m_kStateNameMap.GetCount())
        return false;
        
    iter = m_kStateNameMap.GetFirstPos();
    while (iter)
    {
        NiFixedString kName;
        NiUInt32 uiIndex;
        m_kStateNameMap.GetNext(iter, kName, uiIndex);
        
        NiUInt32 uiNewIndex;
        if (!pkNewObject->m_kStateNameMap.GetAt(kName, uiNewIndex))
            return false;
        if (uiNewIndex != uiIndex)
            return false;
    }

    if (m_uFlags != pkNewObject->m_uFlags)
        return false;

    return true;
}
//---------------------------------------------------------------------------
