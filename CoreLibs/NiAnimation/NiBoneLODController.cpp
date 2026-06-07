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

#include "NiBoneLODController.h"
#include "NiSkinningLODController.h"
#include "NiGeometryConverter.h"
#include <NiCloningProcess.h>
#include <NiTriBasedGeom.h>
#include <NiStringExtraData.h>
#include <NiSystem.h>

NiImplementRTTI(NiBoneLODController, NiTimeController);

int NiBoneLODController::ms_iGlobalLOD = -1;
//---------------------------------------------------------------------------
void NiBoneLODController::CleanUp()
{
    unsigned int ui;

    for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
    {
        NiTPrimitiveSet<NiNode*>* pkSet = 
            m_kBoneArray.GetAt(ui);
        NiDelete pkSet;
    }
    m_kBoneArray.RemoveAll();

    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        NiTPrimitiveSet<SkinInfo*>* pkSet = 
            m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                NiDelete pkInfo;
            }
        }
        NiDelete pkSet;
    }
    m_kSkinArray.RemoveAll();

    m_kSkinSet.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiBoneLODController::SetBoneLOD(int iLOD)
{
    if (m_uiNumLODs > 0 && m_iLOD != iLOD &&
        (iLOD == -1 || (iLOD > -1 && (unsigned int) iLOD < m_uiNumLODs)))
    {
        // Set bone update flags and activate the appropriate skins.

        unsigned int ui, uj;

        // Set bone update flags.
        bool bUpdate = false;
        for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
        {
            NiTPrimitiveSet<NiNode*>* pkSet = 
                m_kBoneArray.GetAt(ui);
            if (pkSet)
            {
                if ((int) ui == iLOD)
                {
                    bUpdate = true;
                }
                for (uj = 0; uj < pkSet->GetSize(); uj++)
                {
                    pkSet->GetAt(uj)->SetSelectiveUpdate(bUpdate);
                }
            }
        }

        // Disable all skins.
        for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
        {
            NiTriBasedGeom* pkSkinGeom = m_kSkinSet.GetAt(ui);
            if (pkSkinGeom)
            {
                pkSkinGeom->SetAppCulled(true);
                pkSkinGeom->SetSelectiveUpdate(false);
            }
        }

        // Activate skins and set appropriate NiSkinInstance objects.
        if (iLOD != -1)
        {
            NiTPrimitiveSet<SkinInfo*>* pkSet = 
                m_kSkinArray.GetAt(iLOD);
            if (pkSet)
            {
                for (ui = 0; ui < pkSet->GetSize(); ui++)
                {
                    NiTriBasedGeom* pkSkinGeom = pkSet->GetAt(ui)
                        ->m_pkSkinGeom;
                    if (pkSkinGeom)
                    {
                        pkSkinGeom->SetAppCulled(false);
                        pkSkinGeom->SetSelectiveUpdate(true);
                        pkSkinGeom->SetSkinInstance(pkSet->GetAt(ui)
                            ->m_spSkinInst);
                    }
                }
            }
        }
        
        m_iLOD = iLOD;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiBoneLODController::RemoveSkinFromAllLODs(
    NiTriBasedGeom* pkSkinToRemove)
{
    unsigned int ui;

    // Search m_kSkinArray.
    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            bool bRemovedItem;
            do
            {
                bRemovedItem = false;
                for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
                {
                    SkinInfo* pkInfo = pkSet->GetAt(uj);
                    if (pkInfo->m_pkSkinGeom == pkSkinToRemove)
                    {
                        pkSet->RemoveAt(uj);
                        NiDelete pkInfo;
                        bRemovedItem = true;
                        break;
                    }
                }
            } while (bRemovedItem);

            if (pkSet->GetSize() == 0)
            {
                m_kSkinArray.RemoveAt(ui);
                NiDelete pkSet;
            }
        }
    }

    // Search m_kSkinSet.
    bool bRemovedItem;
    do
    {
        bRemovedItem = false;
        for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
        {
            NiTriBasedGeom* pkSkin = m_kSkinSet.GetAt(ui);
            if (pkSkin == pkSkinToRemove)
            {
                m_kSkinSet.RemoveAt(ui);
                bRemovedItem = true;
                break;
            }
        }
    } while (bRemovedItem);
}
//---------------------------------------------------------------------------
void NiBoneLODController::GetSkinData(NiTriBasedGeomSet& kGeoms,
    NiSkinInstanceSet& kInstances)
{
    kGeoms.RemoveAll();
    kInstances.RemoveAll();

    for (unsigned int ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                kGeoms.Add(pkInfo->m_pkSkinGeom);
                kInstances.Add(pkInfo->m_spSkinInst);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::ReplaceSkin(NiTriBasedGeom* pkOldSkin,
    NiTriBasedGeom* pkNewSkin)
{
    unsigned int ui;

    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                if (pkInfo->m_pkSkinGeom == pkOldSkin)
                {
                    pkInfo->m_pkSkinGeom = pkNewSkin;
                }
            }
        }
    }

    for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        if (m_kSkinSet.GetAt(ui) == pkOldSkin)
        {
            m_kSkinSet.ReplaceAt(ui, pkNewSkin);
        }
    }
}
//---------------------------------------------------------------------------
bool NiBoneLODController::FindGeom(NiTriBasedGeom* pkGeom)
{
    for (unsigned int ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        if (pkGeom == m_kSkinSet.GetAt(ui))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBoneLODController);
//---------------------------------------------------------------------------
void NiBoneLODController::CopyMembers(NiBoneLODController* pkDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pkDest, kCloning);

    pkDest->m_iLOD = m_iLOD;
    pkDest->m_uiNumLODs = m_uiNumLODs;
}
//---------------------------------------------------------------------------
void NiBoneLODController::ProcessClone(NiCloningProcess& kCloning)
{
    NiTimeController::ProcessClone(kCloning);

    NiObject* pkDestObj = 0;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkDestObj));
    NiBoneLODController* pkDest = (NiBoneLODController*)pkDestObj;

    unsigned int ui;

    for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
    {
        NiNodeSet* pkOldSet = m_kBoneArray.GetAt(ui);
        if (pkOldSet)
        {
            NiNodeSet* pkNewSet = NiNew NiNodeSet;
            pkDest->m_kBoneArray.SetAtGrow(ui, pkNewSet);

            for (unsigned int uj = 0; uj < pkOldSet->GetSize(); uj++)
            {
                NiNode* pkOldBone = pkOldSet->GetAt(uj);

                NiObject* pkNewBoneObj = 0;
                bool bCloned = kCloning.m_pkCloneMap->GetAt(pkOldBone,
                    pkNewBoneObj);
                NiNode* pkNewBone = (NiNode*)pkNewBoneObj;

                if (bCloned)
                {
                    pkNewSet->Add(pkNewBone);
                }
                else
                {
                    pkNewSet->Add(pkOldBone);
                }
            }
        }
        else
        {
            pkDest->m_kBoneArray.SetAtGrow(ui, NULL);
        }
    }

    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkOldSet = m_kSkinArray.GetAt(ui);
        if (pkOldSet)
        {
            SkinInfoSet* pkNewSet = NiNew SkinInfoSet;
            pkDest->m_kSkinArray.SetAtGrow(ui, pkNewSet);

            for (unsigned int uj = 0; uj < pkOldSet->GetSize(); uj++)
            {
                SkinInfo* pkOldInfo = pkOldSet->GetAt(uj);
                SkinInfo* pkNewInfo = NiNew SkinInfo;
                pkNewSet->Add(pkNewInfo);

                NiObject* pkNewSkinGeom = 0;
                bool bCloned = kCloning.m_pkCloneMap->
                    GetAt(pkOldInfo->m_pkSkinGeom, pkNewSkinGeom);
                if (bCloned)
                {
                    pkNewInfo->m_pkSkinGeom = (NiTriBasedGeom*)pkNewSkinGeom;
                }
                else
                {
                    pkNewInfo->m_pkSkinGeom = pkOldInfo->m_pkSkinGeom;
                }

                pkNewInfo->m_spSkinInst =
                    (NiSkinInstance*) pkOldInfo->m_spSkinInst->CreateClone(
                        kCloning);
                pkOldInfo->m_spSkinInst->ProcessClone(kCloning);
            }
        }
        else
        {
            pkDest->m_kSkinArray.SetAtGrow(ui, NULL);
        }
    }

    for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        NiTriBasedGeom* pkOldSkinGeom = m_kSkinSet.GetAt(ui);

        NiObject* pkNewSkinGeomObj = 0;
        bool bCloned = kCloning.m_pkCloneMap->GetAt(pkOldSkinGeom,
            pkNewSkinGeomObj);
        NiTriBasedGeom* pkNewSkinGeom = (NiTriBasedGeom*)pkNewSkinGeomObj;
        if (bCloned)
        {
            pkDest->m_kSkinSet.Add(pkNewSkinGeom);
        }
        else
        {
            pkDest->m_kSkinSet.Add(pkOldSkinGeom);
        }
    }

    int iLOD = pkDest->m_iLOD;
    pkDest->m_iLOD = -1;
    pkDest->SetBoneLOD(iLOD);
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
bool NiBoneLODController::RegisterStreamables(NiStream& kStream)
{
    if (!NiTimeController::RegisterStreamables(kStream))
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                if (pkInfo->m_spSkinInst)
                {
                    pkInfo->m_spSkinInst->RegisterStreamables(kStream);
                }
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBoneLODController::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_iLOD);
    NiStreamSaveBinary(kStream, m_uiNumLODs);

    unsigned int ui;

    NiStreamSaveBinary(kStream, m_kBoneArray.GetSize());
    for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
    {
        NiNodeSet* pkSet = m_kBoneArray.GetAt(ui);
        if (pkSet)
        {
            NiStreamSaveBinary(kStream, pkSet->GetSize());
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                NiNode* pkBone = pkSet->GetAt(uj);
                kStream.SaveLinkID(pkBone);
            }
        }
        else
        {
            NiStreamSaveBinary(kStream, 0);
        }
    }

    NiStreamSaveBinary(kStream, m_kSkinArray.GetSize());
    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            NiStreamSaveBinary(kStream, pkSet->GetSize());
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                kStream.SaveLinkID(pkInfo->m_pkSkinGeom);
                kStream.SaveLinkID(pkInfo->m_spSkinInst);
            }
        }
        else
        {
            NiStreamSaveBinary(kStream, 0);
        }
    }

    NiStreamSaveBinary(kStream, m_kSkinSet.GetSize());
    for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        NiTriBasedGeom* pkGeom = m_kSkinSet.GetAt(ui);
        kStream.SaveLinkID(pkGeom);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBoneLODController);

//---------------------------------------------------------------------------
void NiBoneLODController::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_iLOD);
    NiStreamLoadBinary(kStream, m_uiNumLODs);

    unsigned int ui;

    unsigned int uiBoneArraySize = 0;
    NiStreamLoadBinary(kStream, uiBoneArraySize);
    for (ui = 0; ui < uiBoneArraySize; ui++)
    {
        NiNodeSet* pkSet = NiNew NiNodeSet;
        m_kBoneArray.SetAtGrow(ui, pkSet);

        unsigned int uiSetSize = 0;
        NiStreamLoadBinary(kStream, uiSetSize);
        for (unsigned int uj = 0; uj < uiSetSize; uj++)
        {
            pkSet->Add((NiNode*) this);
            kStream.ReadLinkID();   // pkBone
        }
    }

    unsigned int uiSkinArraySize = 0;
    NiStreamLoadBinary(kStream, uiSkinArraySize);
    for (ui = 0; ui < uiSkinArraySize; ui++)
    {
        SkinInfoSet* pkSet = NiNew SkinInfoSet;
        m_kSkinArray.SetAtGrow(ui, pkSet);

        unsigned int uiSetSize = 0;
        NiStreamLoadBinary(kStream, uiSetSize);
        for (unsigned int uj = 0; uj < uiSetSize; uj++)
        {
            SkinInfo* pkInfo = NiNew SkinInfo;
            pkSet->Add(pkInfo);

            kStream.ReadLinkID();   // pkInfo->m_pkSkinGeom
            kStream.ReadLinkID();   // pkInfo->m_spSkinInst
        }
    }

    unsigned int uiSkinSetSize = 0;
    NiStreamLoadBinary(kStream, uiSkinSetSize);
    for (ui = 0; ui < uiSkinSetSize; ui++)
    {
        m_kSkinSet.Add((NiTriBasedGeom*) this);
        kStream.ReadLinkID();   // pkGeom
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);

    unsigned int ui;

    for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
    {
        NiNodeSet* pkSet = m_kBoneArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                pkSet->ReplaceAt(uj,
                    (NiNode*) kStream.GetObjectFromLinkID());
            }
        }
    }

    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                pkInfo->m_pkSkinGeom =
                    (NiTriBasedGeom*) kStream.GetObjectFromLinkID();
                pkInfo->m_spSkinInst =
                    (NiSkinInstance*) kStream.GetObjectFromLinkID();
            }
        }
    }

    for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        m_kSkinSet.ReplaceAt(ui, (NiTriBasedGeom*)
            kStream.GetObjectFromLinkID());
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::PostLinkObject(NiStream& kStream)
{
    NiTimeController::PostLinkObject(kStream);

    int iLOD = m_iLOD;
    m_iLOD = -1;
    SetBoneLOD(iLOD);
}
//---------------------------------------------------------------------------
bool NiBoneLODController::IsEqual(NiObject* pkObject)
{
    if (!NiTimeController::IsEqual(pkObject))
    {
        return false;
    }

    NiBoneLODController* pkCtlr = (NiBoneLODController*) pkObject;

    if (m_iLOD != pkCtlr->m_iLOD ||
        m_uiNumLODs != pkCtlr->m_uiNumLODs ||
        m_kBoneArray.GetSize() != pkCtlr->m_kBoneArray.GetSize() ||
        m_kSkinArray.GetSize() != pkCtlr->m_kSkinArray.GetSize() ||
        m_kSkinSet.GetSize() != pkCtlr->m_kSkinSet.GetSize())
    {
        return false;
    }

    unsigned int ui;

    for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
    {
        NiNodeSet* pkSet0 = m_kBoneArray.GetAt(ui);
        NiNodeSet* pkSet1 = pkCtlr->m_kBoneArray.GetAt(ui);

        if ((pkSet0 && !pkSet1) || (!pkSet0 && pkSet1))
        {
            return false;
        }

        if (pkSet0 && pkSet1)
        {
            if (pkSet0->GetSize() != pkSet1->GetSize())
            {
                return false;
            }

            for (unsigned int uj = 0; uj < pkSet0->GetSize(); uj++)
            {
                if (!pkSet0->GetAt(uj)->IsEqual(pkSet1->GetAt(uj)))
                {
                    return false;
                }
            }
        }
    }

    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet0 = m_kSkinArray.GetAt(ui);
        SkinInfoSet* pkSet1 = pkCtlr->m_kSkinArray.GetAt(ui);

        if ((pkSet0 && !pkSet1) || (!pkSet0 && pkSet1))
        {
            return false;
        }

        if (pkSet0 && pkSet1)
        {
            if (pkSet0->GetSize() != pkSet1->GetSize())
            {
                return false;
            }

            for (unsigned int uj = 0; uj < pkSet0->GetSize(); uj++)
            {
                SkinInfo* pkInfo0 = pkSet0->GetAt(uj);
                SkinInfo* pkInfo1 = pkSet1->GetAt(uj);
                if (!pkInfo0->m_pkSkinGeom->IsEqual(pkInfo1->m_pkSkinGeom) ||
                    !pkInfo0->m_spSkinInst->IsEqual(pkInfo1->m_spSkinInst))
                {
                    return false;
                }
            }
        }
    }

    for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        if (!m_kSkinSet.GetAt(ui)->IsEqual(pkCtlr->m_kSkinSet.GetAt(ui)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBoneLODController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    pkStrings->Add(NiGetViewerString(
        NiBoneLODController::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("m_iLOD", m_iLOD));
    pkStrings->Add(NiGetViewerString("m_uiNumLODs", m_uiNumLODs));
}
//---------------------------------------------------------------------------
void NiBoneLODController::Convert(NiStream& kStream, 
    NiTLargeObjectArray<NiObjectPtr>& kTopObjects)
{
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 10))
        return;

    for (unsigned int i = 0; i < kTopObjects.GetSize(); i++)
    {
        NiObject* pkObject = kTopObjects.GetAt(i);
        RecurseScene(kStream, pkObject);
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::RecurseScene(NiStream& kStream, NiObject* pkObject)
{
    if (pkObject == NULL)
        return;

    if (NiIsKindOf(NiNode, pkObject))
    {
        // If this is an NiNode, we need to convert its children
        NiNode* pkNode = NiVerifyStaticCast(NiNode, pkObject);
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
            RecurseScene(kStream, pkNode->GetAt(ui));

        // Iterate over all time controllers, converting as necessary
        NiTimeController* pkController = pkNode->GetControllers();
        NiTimeController* pkNextController = NULL;
        while (pkController != NULL)
        {
            // Hold on to the next pointer, as it might change in the
            // conversion process
            pkNextController = pkController->GetNext();
            NiBoneLODControllerPtr spBoneLODController =
                NiDynamicCast(NiBoneLODController, pkController);

            if (spBoneLODController)
            {
                ConvertBoneLODController(kStream, spBoneLODController);
                pkNode->RemoveController(pkController);
            }
            
            pkController = pkNextController;
        }
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::ConvertBoneLODController(NiStream& kStream,
    NiBoneLODController* pkBoneLOD)
{
    NiObjectNET* pkTarget = pkBoneLOD->GetTarget();
    NIASSERT(pkTarget);
    NIASSERT(NiIsKindOf(NiNode, pkTarget));
    NiNode* pkNodeTarget = NiDynamicCast(NiNode, pkTarget);
    if (!pkNodeTarget)
        return;

    if (pkBoneLOD->m_kBoneArray.GetSize() <= 0)
        return;

    NiTPrimitiveArray<NiNode*> kBoneList;
    kBoneList.SetSize(pkBoneLOD->m_kBoneArray.GetAt(0)->GetSize());

    // Create new controller
    NiSkinningLODController* pkNewCont = NiNew NiSkinningLODController();

    // Detach old controller, attach new controller
    pkNodeTarget->RemoveController(pkBoneLOD);
    pkNewCont->SetTarget(pkNodeTarget);

    pkNewCont->SetMaxBoneLODs(pkBoneLOD->GetNumberOfBoneLODs());

    NiTLargeObjectArray<NiObjectPtr> kConvertList;

    NiUInt32 i;
    for (i = pkBoneLOD->m_kBoneArray.GetSize() - 1; 
        i < pkBoneLOD->m_kBoneArray.GetSize(); i--)
    {
        NiBoneLODController::NiNodeSet* pkSet = 
            pkBoneLOD->m_kBoneArray.GetAt(i);
        for (NiUInt32 j = 0; j < pkSet->GetSize(); j++)
        {
            NiNode* pkBone = pkSet->GetAt(j);
            if (kBoneList.Find(pkBone) > kBoneList.GetSize())
            {
                kBoneList.Add(pkBone);
            }
        }
        
        NiUInt32 uiBones = kBoneList.GetSize();
        NiUInt32 uiSkins = pkBoneLOD->m_kSkinArray.GetAt(i)->GetSize();
        pkNewCont->AddBoneLOD(i, uiBones, uiSkins);
    }

    // Make each trishape unique.
    for (i = 0; i < pkBoneLOD->m_kSkinArray.GetSize(); i++)
    {
        NiBoneLODController::SkinInfoSet* pkSet = 
            pkBoneLOD->m_kSkinArray.GetAt(i);
        for (NiUInt32 j = 0; j < pkSet->GetSize(); j++)
        {
            NiTriBasedGeom* pkGeom = pkSet->GetAt(j)->m_pkSkinGeom;
            NiSkinInstance* pkInstance = pkSet->GetAt(j)->m_spSkinInst;

            // Has this geom been seen before in this BL controller?
            bool bFound = false;
            for (NiUInt32 k = 0; k < kConvertList.GetSize() && !bFound; k++)
            {
                bFound |= (kConvertList.GetAt(k) == pkGeom);
            }
            if (bFound)
            {
                // NOTE: If there is no parent for this geometry, the most
                // likely culprit is that the NiGeometry was already converted
                // by the NiGeometryConverter and removed from its parent.
                NiNode* pkParent = pkGeom->GetParent();
                NIASSERT(pkParent);

                NiObjectPtr spObj = pkGeom->Clone();
                NiTriBasedGeom* pkNewGeom = (NiTriBasedGeom*)(NiObject*)spObj;
                pkParent->AttachChild(pkNewGeom);

                // Update the bone LOD controller
                pkBoneLOD->Update(0.0f);

                pkSet->GetAt(j)->m_pkSkinGeom = pkNewGeom;
                pkGeom = pkNewGeom;
            }

            // Assign the skin instance for conversion
            pkGeom->SetSkinInstance(pkInstance);

            kConvertList.Add(pkGeom);
        }
    }

    // Convert all geometry to meshes
    NiGeometryConverter::ConvertToNiMesh(kStream, kConvertList);

    // Now, add these meshes to the new controller
    for (i = 0; i < pkBoneLOD->m_kSkinArray.GetSize(); i++)
    {
        NiBoneLODController::SkinInfoSet* pkSet = 
            pkBoneLOD->m_kSkinArray.GetAt(i);
        for (NiUInt32 j = 0; j < pkSet->GetSize(); j++)
        {
            NiTriBasedGeom* pkGeom = pkSet->GetAt(j)->m_pkSkinGeom;
            NiAVObject* pkObj;
            if (!kStream.GetConversionMap().GetAt(pkGeom, pkObj))
            {
                // No need for a warning here.  If the conversion map 
                // does not contain an object, the geometry converter will
                // warn about a failed conversion.
                continue;
            }

            NiMesh* pkMesh = NiDynamicCast(NiMesh, pkObj);

            if (pkMesh)
                pkNewCont->AddSkin(i, pkMesh);
        }
    }

    pkNewCont->SetBones(kBoneList.GetSize(), &kBoneList.GetAt(0));
    pkNewCont->InitializeNewController();
}
//---------------------------------------------------------------------------
