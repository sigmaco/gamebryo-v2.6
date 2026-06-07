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
#include "NiMainPCH.h"

#include "NiCloningProcess.h"
#include "NiSkinInstance.h"
#include "NiAVObject.h"

NiImplementRTTI(NiSkinInstance, NiObject);

#include "NiSystem.h"

//---------------------------------------------------------------------------
void NiSkinInstance::UpdateModelBound(NiBound& kBound)
{
    NiBound kWorldBound;

    NiAVObject* pBone = m_ppkBones[0];
    const NiSkinData::BoneData* pkBoneData = m_spSkinData->GetBoneData();
    kWorldBound.Update(pkBoneData[0].m_kBound, pBone->GetWorldTransform());
    unsigned int uiBones = m_spSkinData->GetBoneCount();

    for (unsigned int i = 1; i < uiBones; i++)
    {
        NiBound kBoneBound;
        pBone = m_ppkBones[i];

        if (pBone->GetSelectiveUpdate())
        {
            kBoneBound.Update(pkBoneData[i].m_kBound, 
                pBone->GetWorldTransform());
            kWorldBound.Merge(&kBoneBound);
        }
    }

    NiTransform kWorldToSkin, kWorldToRootParent;
    m_pkRootParent->GetWorldTransform().Invert(kWorldToRootParent);
    kWorldToSkin = m_spSkinData->GetRootParentToSkin() * kWorldToRootParent;
    kBound.Update(kWorldBound, kWorldToSkin);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiObject* NiSkinInstance::CreateClone(
    NiCloningProcess& kCloning)
{
    NiSkinInstance* pkSkinInstance = NiNew NiSkinInstance;
    CopyMembers(pkSkinInstance, kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(m_pkRootParent, pkClone);
    pkSkinInstance->m_pkRootParent = bCloned ? 
        (NiAVObject*) pkClone : (NiAVObject*) m_pkRootParent;
    
    unsigned int uiBoneCount = m_spSkinData->GetBoneCount();
    
    pkSkinInstance->m_ppkBones = NiAlloc(NiAVObject*, uiBoneCount);
    
    for (unsigned int i = 0; i < uiBoneCount; i++)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_ppkBones[i], pkClone);
        pkSkinInstance->m_ppkBones[i] = bCloned ?
            (NiAVObject*) pkClone : (NiAVObject*) m_ppkBones[i];
    }
    return pkSkinInstance;
}

//---------------------------------------------------------------------------
void NiSkinInstance::CopyMembers(NiSkinInstance* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);
    pkDest->m_spSkinData = m_spSkinData;
    pkDest->m_spSkinPartition = m_spSkinPartition;
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSkinInstance);
//---------------------------------------------------------------------------
void NiSkinInstance::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_spSkinData
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 101))
    {
        kStream.ReadLinkID(); // m_spSkinPartition;
    }
    kStream.ReadLinkID();   // m_spRootParent

    kStream.ReadMultipleLinkIDs();  // m_ppkBones
}

//---------------------------------------------------------------------------
void NiSkinInstance::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    m_spSkinData = (NiSkinData*) kStream.GetObjectFromLinkID();
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 101))
    {
        m_spSkinPartition = (NiSkinPartition*) kStream.GetObjectFromLinkID();
    }
    m_pkRootParent = (NiAVObject*) kStream.GetObjectFromLinkID();
    
    unsigned int uiBoneCount = kStream.GetNumberOfLinkIDs();
    m_ppkBones = NiAlloc(NiAVObject*,uiBoneCount);

    for (unsigned int i = 0; i < uiBoneCount; i++)
    {
        m_ppkBones[i] = (NiAVObject*) kStream.GetObjectFromLinkID();
    }
}

//---------------------------------------------------------------------------
void NiSkinInstance::PostLinkObject(NiStream& kStream)
{
    // m_spSkinData's skin partition may not be valid in LinkObject
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 101))
    {
        if (m_spSkinData)
        {
            // Move skin partition to NiSkinInstance from NiSkinData, in
            // older NIF files.
            m_spSkinPartition = m_spSkinData->GetSkinPartition(true);
            m_spSkinData->SetSkinPartition(NULL, true);
        }
    }
}
//---------------------------------------------------------------------------
bool NiSkinInstance::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    m_spSkinData->RegisterStreamables(kStream);
    if (m_spSkinPartition)
        m_spSkinPartition->RegisterStreamables(kStream);

    m_pkRootParent->RegisterStreamables(kStream);

    unsigned int i, uiBoneCount = m_spSkinData->GetBoneCount();

    for (i = 0; i < uiBoneCount; i++)
    {
        m_ppkBones[i]->RegisterStreamables(kStream);
    }

    return true;
}

//---------------------------------------------------------------------------
void NiSkinInstance::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    kStream.SaveLinkID(m_spSkinData);
    kStream.SaveLinkID(m_spSkinPartition);
    kStream.SaveLinkID(m_pkRootParent);

    unsigned int i, uiBoneCount = m_spSkinData->GetBoneCount();
    
    NiStreamSaveBinary(kStream, uiBoneCount);

    for (i = 0; i < uiBoneCount; i++)
    {
        kStream.SaveLinkID(m_ppkBones[i]);
    }
}

//---------------------------------------------------------------------------
bool NiSkinInstance::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiSkinInstance* pkSkinInstance = (NiSkinInstance*) pkObject;

    if (!m_spSkinData->IsEqual(pkSkinInstance->m_spSkinData))
        return false;
    
    if (m_spSkinPartition &&
        !m_spSkinPartition->IsEqual(pkSkinInstance->m_spSkinPartition))
    {
        return false;
    }
    else if (!m_spSkinPartition && pkSkinInstance->m_spSkinPartition)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiSkinInstance::~NiSkinInstance()
{
    NiFree(m_ppkBones);
}
//---------------------------------------------------------------------------
NiSkinInstance::NiSkinInstance(NiSkinData* pkSkinData,
    NiAVObject* pkRootParent, NiAVObject** ppkBones) : 
    m_spSkinData(pkSkinData), 
    m_pkRootParent(pkRootParent), 
    m_ppkBones(ppkBones)
{
    NIASSERT(pkSkinData != NULL);
    NIASSERT(pkRootParent != NULL);
    NIASSERT(ppkBones != NULL);
}

//---------------------------------------------------------------------------
NiSkinInstance::NiSkinInstance() : 
    m_spSkinData(0), 
    m_pkRootParent(0), 
    m_ppkBones(0)
{
    // Used for streaming: All members will be initialized by LoadBinary.
    // Used for cloning: All members will be initialized by CopyMembers and
    // ProcessClone.
}

//---------------------------------------------------------------------------
NiSkinData* NiSkinInstance::GetSkinData() const
{
    return m_spSkinData;
}

//---------------------------------------------------------------------------
NiAVObject* NiSkinInstance::GetRootParent() const
{
    return m_pkRootParent;
}

//---------------------------------------------------------------------------
NiAVObject*const* NiSkinInstance::GetBones() const
{
    return m_ppkBones;
}

//---------------------------------------------------------------------------
NiSkinPartition* NiSkinInstance::GetSkinPartition() const
{
    return m_spSkinPartition;
}

//---------------------------------------------------------------------------
void NiSkinInstance::SetRootParent(NiAVObject* pkRootParent)
{
    m_pkRootParent = pkRootParent;
}

//---------------------------------------------------------------------------
void NiSkinInstance::SetSkinData(NiSkinData* pkSkinData)
{
    m_spSkinData = pkSkinData;
}

//---------------------------------------------------------------------------
void NiSkinInstance::SetBone(unsigned int uiIndex, NiAVObject* pkBone)
{
    m_ppkBones[uiIndex] = pkBone;
}

//---------------------------------------------------------------------------
void NiSkinInstance::SetSkinPartition(
    NiSkinPartition* pkSkinPartition)
{
    m_spSkinPartition = pkSkinPartition;
}

//---------------------------------------------------------------------------
void NiSkinInstance::GetWorldToSkinTransform(
    NiTransform& kWorldToSkin) const
{
    NiTransform kWorldToRootParent;

    m_pkRootParent->GetWorldTransform().Invert(kWorldToRootParent);
    kWorldToSkin = m_spSkinData->GetRootParentToSkin() * kWorldToRootParent;
}
//---------------------------------------------------------------------------
void NiSkinInstance::ConcatWorldToSkinTransform(
    const NiTransform& kSourceMat, NiTransform& kDestMat) const
{
    NiTransform kWorldToRootParent;
    NiTransform kTemp;

    m_pkRootParent->GetWorldTransform().Invert(kWorldToRootParent);
    kTemp = kSourceMat * m_spSkinData->GetRootParentToSkin();
    kDestMat = kTemp * kWorldToRootParent;
}
//---------------------------------------------------------------------------
void NiSkinInstance::GetBoneWorldTransform(unsigned int uiBone, 
    NiTransform& kBoneWorld) const
{
    kBoneWorld = m_ppkBones[uiBone]->GetWorldTransform();
}
//---------------------------------------------------------------------------
void NiSkinInstance::ConcatBoneWorldTransform(unsigned int uiBone, 
    const NiTransform& kSourceMat, NiTransform& kDestMat) const
{
    kDestMat = kSourceMat * m_ppkBones[uiBone]->GetWorldTransform();
}
//---------------------------------------------------------------------------
void NiSkinInstance::GetSkinToBoneTransform(unsigned int uiBone, 
    NiTransform& kSkinToBone) const
{
    const NiSkinData::BoneData* pkBoneData = m_spSkinData->GetBoneData();
    kSkinToBone = pkBoneData[uiBone].m_kSkinToBone;
}
//---------------------------------------------------------------------------
void NiSkinInstance::ConcatSkinToBoneTransform(unsigned int uiBone, 
    const NiTransform& kSourceMat, NiTransform& kDestMat) const
{
    const NiSkinData::BoneData* pkBoneData = m_spSkinData->GetBoneData();
    kDestMat = kSourceMat * pkBoneData[uiBone].m_kSkinToBone;
}
//---------------------------------------------------------------------------
