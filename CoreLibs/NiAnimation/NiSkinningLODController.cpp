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
#include "NiAnimationPCH.h"

#include <NiCommonSemantics.h>
#include <NiMesh.h>
#include "NiSkinningLODController.h"

NiImplementRTTI(NiSkinningLODController, NiTimeController);

//---------------------------------------------------------------------------
NiSkinningLODController* NiSkinningLODController::FindSkinningLODController(
    NiAVObject* pkObject)
{
    NIASSERT(pkObject);

    NiSkinningLODController* pkLODController = NiGetController(
        NiSkinningLODController, pkObject);
    if (pkLODController)
    {
        return pkLODController;
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                pkLODController = FindSkinningLODController(pkChild);
                if (pkLODController)
                {
                    return pkLODController;
                }
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiSkinningLODController::~NiSkinningLODController()
{
    for (NiUInt32 ui = 0; ui < m_kLODs.GetSize(); ui++)
    {
        LODInfo* pkLOD = m_kLODs.GetAt(ui);
        if (pkLOD)
            NiDelete pkLOD;
    }
}
//---------------------------------------------------------------------------
void NiSkinningLODController::Update(float)
{
    if (m_uiNextLOD == m_uiCurrentLOD)
        return;

    if (m_uiNextLOD > m_kLODs.GetSize())
        return;

    m_uiCurrentLOD = m_uiNextLOD;

    // Get bone/skin LOD levels for this skinning LOD
    const LODInfo* pkLOD = m_kLODs.GetAt(m_uiCurrentLOD);
    NiUInt32 uiNumBonesActive = pkLOD->m_uiNumBones;

    // Cull all skins.
    for (NiUInt32 uiS = 0; uiS < m_kSkins.GetSize(); uiS++)
    {
        NiMesh* pkSkin = m_kSkins.GetAt(uiS);
        pkSkin->SetAppCulled(true);
        pkSkin->SetSelectiveUpdate(false);
    }

    // Iterate through active skins and uncull them...
    for (NiUInt32 uiA = 0; uiA < pkLOD->m_kActiveSkins.GetSize(); uiA++)
    {
        NiMesh* pkSkin = m_kSkins.GetAt(pkLOD->m_kActiveSkins.GetAt(uiA));
        pkSkin->SetAppCulled(false);
        pkSkin->SetSelectiveUpdate(true);
    }

    // Set selective update flags on bones
    for (NiUInt32 uiB = 0; uiB < m_kBones.GetSize(); uiB++)
    {
        bool bUpdate = (uiB < uiNumBonesActive);
        m_kBones.GetAt(uiB)->SetSelectiveUpdate(bUpdate);
    }
}
//---------------------------------------------------------------------------
void NiSkinningLODController::SetBoneLOD(NiUInt32 uiLOD)
{
    m_uiNextLOD = uiLOD;
}
//---------------------------------------------------------------------------
void NiSkinningLODController::SetBones(NiUInt32 uiNumBones, NiNode** ppkBones)
{
    m_kBones.SetSize(uiNumBones);
    for (NiUInt32 uiB = 0; uiB < uiNumBones; uiB++)
    {
        m_kBones.SetAt(uiB, ppkBones[uiB]);
    }
}
//---------------------------------------------------------------------------
void NiSkinningLODController::SetMaxBoneLODs(NiUInt32 uiMaxBoneLODs)
{
    m_kLODs.SetGrowBy(uiMaxBoneLODs);
}
//---------------------------------------------------------------------------
void NiSkinningLODController::AddBoneLOD(NiUInt32 uiBoneLOD, 
    NiUInt32 uiNumBones, NiUInt32 uiActiveSkins)
{
    LODInfo* pkNewInfo = NiNew LODInfo;
    m_kLODs.SetAtGrow(uiBoneLOD, pkNewInfo);

    pkNewInfo->m_kActiveSkins.SetSize(uiActiveSkins);
    pkNewInfo->m_uiNumBones = uiNumBones;
}
//---------------------------------------------------------------------------
void NiSkinningLODController::AddSkin(NiUInt32 uiBoneLOD, NiMesh* pkSkin)
{
    NiUInt32 uiIdx = m_kSkins.Find(pkSkin);
    if (uiIdx == NiUInt32(~0))
        uiIdx = m_kSkins.Add(pkSkin);
    m_kLODs.GetAt(uiBoneLOD)->m_kActiveSkins.Add(uiIdx);
}
//---------------------------------------------------------------------------
void NiSkinningLODController::InitializeNewController()
{
    m_uiCurrentLOD = (NiUInt32) ~0;
    SetBoneLOD(0);
    NiSkinningLODController::Update(0.0f);
}
//---------------------------------------------------------------------------
NiUInt32 NiSkinningLODController::GetLODForNode(
    const NiAVObject* pkObject) const
{
    NIASSERT(pkObject);

    NiUInt32 uiNumLODLevels = m_kLODs.GetSize();

    // Search for the node in the LOD bone list.
    NiUInt32 uiNumBones = m_kBones.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumBones; ui++)
    {
        NiNode* pkNode = m_kBones.GetAt(ui);
        if (pkObject == pkNode)
        {
            // Find the matching level of detail.
            NiUInt32 uj = 1;
            while (uj < uiNumLODLevels)
            {
                const LODInfo* pkLOD = m_kLODs.GetAt(uj);
                if (ui >= pkLOD->m_uiNumBones)
                {
                    break;
                }
                uj++;
            }
            return uj - 1;
        }
    }

    // The node was never assigned a level of detail.
    // Return uiNumLODLevels - 1 to indicate the node should never be culled.
    if (uiNumLODLevels > 0)
    {
        uiNumLODLevels--;
    }
    return uiNumLODLevels;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiSkinningLODController);
//---------------------------------------------------------------------------
void NiSkinningLODController::CopyMembers(NiSkinningLODController* pkDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pkDest, kCloning);

    pkDest->m_uiCurrentLOD = m_uiCurrentLOD;
    pkDest->m_uiNextLOD = m_uiNextLOD;

    pkDest->m_kLODs.SetSize(m_kLODs.GetSize());
    for (NiUInt32 ui = 0; ui < m_kLODs.GetSize(); ui++)
    {
        LODInfo* pkNewInfo = NiNew LODInfo;
        *pkNewInfo = *m_kLODs.GetAt(ui);
        pkDest->m_kLODs.SetAt(ui, pkNewInfo);
    }
}
//---------------------------------------------------------------------------
void NiSkinningLODController::ProcessClone(NiCloningProcess& kCloning)
{
    NiTimeController::ProcessClone(kCloning);

    NiObject* pkDestObj = 0;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkDestObj));
    NiSkinningLODController* pkDest = (NiSkinningLODController*)pkDestObj;

    NiUInt32 ui;
    pkDest->m_kBones.SetSize(m_kBones.GetSize());
    for (ui = 0; ui < m_kBones.GetSize(); ui++)
    {
        NiObject* pkBone = m_kBones.GetAt(ui);
        kCloning.m_pkCloneMap->GetAt(pkBone, pkBone);
        pkDest->m_kBones.SetAt(ui, (NiNode*)pkBone);
    }

    pkDest->m_kSkins.SetSize(m_kSkins.GetSize());
    for (ui = 0; ui < m_kSkins.GetSize(); ui++)
    {
        NiObject* pkSkin = m_kSkins.GetAt(ui);
        kCloning.m_pkCloneMap->GetAt(pkSkin, pkSkin);
        pkDest->m_kSkins.SetAt(ui, (NiMesh*)pkSkin);
    }
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
bool NiSkinningLODController::RegisterStreamables(NiStream& kStream)
{
    if (!NiTimeController::RegisterStreamables(kStream))
    {
        return false;
    }

    for (NiUInt32 uiB = 0; uiB < m_kBones.GetSize(); uiB++)
    {
        m_kBones.GetAt(uiB)->RegisterStreamables(kStream);
    }

    for (NiUInt32 uiS = 0; uiS < m_kSkins.GetSize(); uiS++)
    {
        m_kSkins.GetAt(uiS)->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSkinningLODController::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiCurrentLOD);

    NiUInt32 ui;

    NiUInt32 uiNumBones = m_kBones.GetSize();
    NiStreamSaveBinary(kStream, uiNumBones);
    for (ui = 0; ui < uiNumBones; ui++)
    {
        kStream.SaveLinkID(m_kBones.GetAt(ui));
    }

    NiUInt32 uiNumSkins = m_kSkins.GetSize();
    NiStreamSaveBinary(kStream, uiNumSkins);
    for (ui = 0; ui < uiNumSkins; ui++)
    {
        kStream.SaveLinkID(m_kSkins.GetAt(ui));
    }

    NiUInt32 uiNumLODLevels = m_kLODs.GetSize();
    NiStreamSaveBinary(kStream, uiNumLODLevels);
    for (ui = 0; ui < uiNumLODLevels; ui++)
    {
        const LODInfo* pkLOD = m_kLODs.GetAt(ui);
        NiStreamSaveBinary(kStream, pkLOD->m_uiNumBones);
        NiUInt32 uiNumActiveSkins = pkLOD->m_kActiveSkins.GetSize();
        NiStreamSaveBinary(kStream, uiNumActiveSkins);
        for (NiUInt32 j = 0; j < uiNumActiveSkins; j++)
        {
            NiStreamSaveBinary(kStream, pkLOD->m_kActiveSkins.GetAt(j));
        }
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSkinningLODController);

//---------------------------------------------------------------------------
void NiSkinningLODController::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiCurrentLOD);

    NiUInt32 ui;

    NiUInt32 uiNumBones;
    NiStreamLoadBinary(kStream, uiNumBones);
    m_kBones.SetSize(uiNumBones);
    for (ui = 0; ui < uiNumBones; ui++)
    {
        kStream.ReadLinkID();
        m_kBones.SetAt(ui, NULL);
    }

    NiUInt32 uiNumSkins;
    NiStreamLoadBinary(kStream, uiNumSkins);
    m_kSkins.SetSize(uiNumSkins);
    for (ui = 0; ui < uiNumSkins; ui++)
    {
        kStream.ReadLinkID();
        m_kSkins.SetAt(ui, NULL);
    }

    NiUInt32 uiNumLODLevels;
    NiStreamLoadBinary(kStream, uiNumLODLevels);
    m_kLODs.SetSize(uiNumLODLevels);
    
    for (ui = 0; ui < uiNumLODLevels; ui++)
    {
        LODInfo* pkLOD = NiNew LODInfo;
        m_kLODs.SetAt(ui, pkLOD);

        NiStreamLoadBinary(kStream, pkLOD->m_uiNumBones);

        NiUInt32 uiNumActiveSkins;
        NiStreamLoadBinary(kStream, uiNumActiveSkins);
        pkLOD->m_kActiveSkins.SetSize(uiNumActiveSkins);
        for (NiUInt32 j = 0; j < uiNumActiveSkins; j++)
        {
            NiUInt32 uiSkinIdx;
            NiStreamLoadBinary(kStream, uiSkinIdx);
            pkLOD->m_kActiveSkins.SetAt(j, uiSkinIdx);
        }
    }
}
//---------------------------------------------------------------------------
void NiSkinningLODController::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);

    NiUInt32 ui;
    for (ui = 0; ui < m_kBones.GetSize(); ui++)
    {
        m_kBones.SetAt(ui, (NiNode*)kStream.GetObjectFromLinkID());
    }

    for (ui = 0; ui < m_kSkins.GetSize(); ui++)
    {
        m_kSkins.SetAt(ui, (NiMesh*)kStream.GetObjectFromLinkID());
    }
}
//---------------------------------------------------------------------------
void NiSkinningLODController::PostLinkObject(NiStream& kStream)
{
    NiTimeController::PostLinkObject(kStream);

    InitializeNewController();
}
//---------------------------------------------------------------------------
bool NiSkinningLODController::IsEqual(NiObject* pkObject)
{
    if (!NiTimeController::IsEqual(pkObject))
    {
        return false;
    }

    NiSkinningLODController* pkThat = NiDynamicCast(NiSkinningLODController,
        pkObject);
    if (!pkThat)
        return false;

    if (m_uiCurrentLOD != pkThat->m_uiCurrentLOD)
        return false;

    if (m_kBones.GetSize() != pkThat->m_kBones.GetSize())
        return false;

    if (m_kLODs.GetSize() != pkThat->m_kLODs.GetSize())
        return false;

    for (NiUInt32 ui = 0; ui < m_kLODs.GetSize(); ui++)
    {
        if (*m_kLODs.GetAt(ui) != *pkThat->m_kLODs.GetAt(ui))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSkinningLODController::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    pkStrings->Add(NiGetViewerString(
        NiSkinningLODController::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("m_uiCurrentLOD", m_uiCurrentLOD));
    pkStrings->Add(NiGetViewerString("GetNumberOfBoneLODs()", 
        GetNumberOfBoneLODs()));
}
//---------------------------------------------------------------------------
