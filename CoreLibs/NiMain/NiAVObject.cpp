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

#include "NiAVObject.h"
#include "NiBool.h"
#include "NiCloningProcess.h"
#include "NiDynamicEffectState.h"
#include "NiMainMetrics.h"
#include "NiNode.h"
#include "NiRenderer.h"
#include "NiTimeController.h"
#include "NiRenderObject.h"
#include "NiCollisionObject.h" 

NiImplementRTTI(NiAVObject, NiObjectNET);

//---------------------------------------------------------------------------
NiUpdateProcessPtr NiAVObject::ms_spDefaultUpdateProcess;

//---------------------------------------------------------------------------
// Construction and Destruction
//---------------------------------------------------------------------------
NiAVObject::NiAVObject()
{
    m_uFlags = 0;
    m_pkParent = 0;
    SetAppCulled(false);
    m_kLocal.MakeIdentity();
    m_kWorld.MakeIdentity();
    m_kWorldBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);

    SetSelectiveUpdate(false);
    SetSelectiveUpdateTransforms(false);
    SetSelectiveUpdatePropertyControllers(false);
    SetSelectiveUpdateRigid(true);
    m_spCollisionObject = 0;
}
//---------------------------------------------------------------------------
NiAVObject::~NiAVObject()
{
    DetachAllProperties();

    m_spCollisionObject = 0;
}
//---------------------------------------------------------------------------
void NiAVObject::AttachParent(NiNode* pkParent)
{
    NiNode* pkOldParent = GetParent();
    if (pkOldParent) // node currently has a parent, detach from it
        pkOldParent->DetachChild(this);

    m_pkParent = pkParent;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// properties
//---------------------------------------------------------------------------
NiProperty* NiAVObject::GetProperty(int iType) const
{
    NIASSERT(iType >= 0 && iType < NiProperty::MAX_TYPES);

    NiTListIterator kPos = m_kPropertyList.GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProperty = m_kPropertyList.GetNext(kPos);
        if (pkProperty && pkProperty->Type() == iType)
            return pkProperty;
    }

    return 0;
}
//---------------------------------------------------------------------------
NiPropertyPtr NiAVObject::RemoveProperty(int iType)
{
    NIASSERT(iType >= 0 && iType < NiProperty::MAX_TYPES);

    NiTListIterator kPos = m_kPropertyList.GetHeadPos();
    while (kPos)
    {
        NiPropertyPtr spProperty = m_kPropertyList.GetNext(kPos);
        if (spProperty && spProperty->Type() == iType)
        {
            m_kPropertyList.Remove(spProperty);
            return spProperty;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Scene Graph Updating
//---------------------------------------------------------------------------
void NiAVObject::Update(NiUpdateProcess& kUpdate)
{
    kUpdate.PreUpdate(this);

    UpdateDownwardPass(kUpdate);

    if (m_pkParent)
        m_pkParent->UpdateUpwardPass();

    kUpdate.PostUpdate(this);
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateSelected(NiUpdateProcess& kUpdate)
{
    kUpdate.PreUpdate(this);

    UpdateSelectedDownwardPass(kUpdate);

    if (m_pkParent)
        m_pkParent->UpdateUpwardPass();

    kUpdate.PostUpdate(this);
}
//---------------------------------------------------------------------------
void NiAVObject::Update(float fTime, bool bUpdateControllers)
{
    //NILOG("Deprecated function\n");
    if (ms_spDefaultUpdateProcess)
    {
        ms_spDefaultUpdateProcess->SetTime(fTime);
        ms_spDefaultUpdateProcess->SetUpdateControllers(bUpdateControllers);
        ms_spDefaultUpdateProcess->SetSubmitModifiers(bUpdateControllers);
        Update(*ms_spDefaultUpdateProcess);
    }
    else
    {
        NIASSERT(false && "No update process available!");
    }
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateSelected(float fTime)
{
    //NILOG("Deprecated function\n");
    if (ms_spDefaultUpdateProcess)
    {
        ms_spDefaultUpdateProcess->SetTime(fTime);
        ms_spDefaultUpdateProcess->SetUpdateControllers(true);
        ms_spDefaultUpdateProcess->SetSubmitModifiers(true);
        UpdateSelected(*ms_spDefaultUpdateProcess);
    }
    else
    {
        NIASSERT(false && "No update process available!");
    }
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.

    NIMETRICS_MAIN_INCREMENTUPDATES();

#ifdef _PS3
    if (__builtin_expect(kUpdate.GetUpdateControllers(), false))
#else
    if (kUpdate.GetUpdateControllers())
#endif
        UpdateObjectControllers(kUpdate.GetTime());

    UpdateWorldData();
    UpdateWorldBound();
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.

    NIMETRICS_MAIN_INCREMENTUPDATES();

    UpdateObjectControllers(kUpdate.GetTime(),
        GetSelectiveUpdatePropertyControllers());

    if (GetSelectiveUpdateTransforms())
    {
        UpdateWorldData();
        UpdateWorldBound();
    }
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.

    NIMETRICS_MAIN_INCREMENTUPDATES();

    UpdateObjectControllers(kUpdate.GetTime(),
        GetSelectiveUpdatePropertyControllers());

    if (GetSelectiveUpdateTransforms())
    {
        UpdateWorldData();
        UpdateWorldBound();
    }
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateNodeBound()
{
    // default does nothing. overridden by NiNode.
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateControllers(float fTime)
{
    UpdateObjectControllers(fTime);
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateProperties()
{
    // get the property state incident upon the object's parent, or the empty 
    // state if the object is a root
    NiPropertyStatePtr spParentState;
    
    // Avoid use of ?: operator here to avoid ee-gcc compile error.
    if (m_pkParent)
        spParentState = m_pkParent->UpdatePropertiesUpward();
    else
        spParentState = NiNew NiPropertyState;

    // Compute the local state and pass it down the subtree
    UpdatePropertiesDownward(spParentState);
}
//---------------------------------------------------------------------------
void NiAVObject::SetSelectiveUpdateFlags(bool& bSelectiveUpdate, 
    bool bSelectiveUpdateTransforms, bool& bRigid)
{
    // Sets the SelectiveUpdate flag by checking to see if the 
    // object is animated.

    // Sets the SelectiveUpdateTransforms flag if the object has any
    // ancestors with animated transforms.

    // Sets the SelectiveUpdatePropertyControllers flag if the object has
    // any animated property controllers.

    // Sets the SelectiveUpdateRigid flag to true, but returns bRigid 
    // as false if the object has animated transforms.

    bRigid = ! (HasTransformController() ||
        GetBit(SELECTIVE_XFORMS_OVERRIDE_MASK));

    if (!bSelectiveUpdateTransforms)
    {
        // set SelectiveUpdateTransforms flag
        // if there are any transform controllers.
        bSelectiveUpdateTransforms = !bRigid;
    }

    // check for any controllers on the properties
    bool bSelectiveUpdatePropertyControllers = HasPropertyController();
    
    if (bSelectiveUpdatePropertyControllers || bSelectiveUpdateTransforms ||
        GetControllers() != NULL)
        bSelectiveUpdate = true;
    else
        bSelectiveUpdate = false;

    SetSelectiveUpdate(bSelectiveUpdate);
    SetSelectiveUpdateTransforms(bSelectiveUpdateTransforms);
    SetSelectiveUpdatePropertyControllers(
        bSelectiveUpdatePropertyControllers);
    SetSelectiveUpdateRigid(true);
}
//---------------------------------------------------------------------------
bool NiAVObject::HasTransformController() const
{
    NiTimeController* pkCtrl;
    for (pkCtrl = GetControllers(); pkCtrl != NULL;
         pkCtrl = pkCtrl->GetNext())
    {
        if (pkCtrl->IsTransformController())
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiAVObject::HasPropertyController() const
{
    NiTListIterator kPos = m_kPropertyList.GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProperty = m_kPropertyList.GetNext(kPos);
        if (pkProperty && pkProperty->GetControllers())
        {
            return true;
        }        
    }
    return false;
}
//--------------------------------------------------------------------------- 
void NiAVObject::UpdatePropertiesDownward(NiPropertyState*)
{
    /* */
}
//---------------------------------------------------------------------------
NiPropertyStatePtr NiAVObject::PushLocalProperties(
    NiPropertyState* pkParentState, bool bCopyOnChange)
{
    if(GetPropertyList().IsEmpty())
    {
        return pkParentState;
    }
    else
    {
        NiPropertyStatePtr spState;

        // because there are local properties, we must copy the state before
        // pushing the new properties if the "copy on change" flag is set.
        // otherwise, we are free to change the input state
        if(bCopyOnChange)
            spState = NiNew NiPropertyState(*pkParentState);
        else
            spState = pkParentState;

        NiTListIterator kPos = m_kPropertyList.GetHeadPos();
        while (kPos)
        {
            spState->SetProperty(m_kPropertyList.GetNext(kPos));
        }

        return spState;
    }   
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateEffects()
{
    // get the effect state incident upon the object's parent, or NULL (the
    // empty state) if the object is a root
    NiDynamicEffectStatePtr spParentState = NULL;
    if (m_pkParent)
        spParentState = m_pkParent->UpdateEffectsUpward();

    // Compute the local state and pass it down the subtree
    UpdateEffectsDownward(spParentState);
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateEffectsDownward(NiDynamicEffectState*)
{
    /* */
}
//---------------------------------------------------------------------------
void NiAVObject::OnVisible(NiCullingProcess&)
{
    // stub for derived classes   
}
//---------------------------------------------------------------------------
void NiAVObject::Cull(NiCullingProcess& kCuller)
{
    if (!GetAppCulled())
        kCuller.Process(this);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// name search
//---------------------------------------------------------------------------
NiAVObject* NiAVObject::GetObjectByName(const NiFixedString& kName)
{
    if (!kName.Exists())
        return 0;

    if (GetName() == kName)
        return this;

    return 0;
}
//---------------------------------------------------------------------------
// type search
//---------------------------------------------------------------------------
void NiAVObject::GetObjectsByType(const NiRTTI* pkRTTI, 
    NiTPointerList<NiAVObject*>& kObjects)
{
    if (!pkRTTI)
        return;

    if (GetRTTI() == pkRTTI)
        kObjects.AddTail(this);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// transforms
//---------------------------------------------------------------------------
void NiAVObject::CopyTransforms(const NiAVObject* pkSrc)
{
    m_kLocal = pkSrc->m_kLocal;
    m_kWorld = pkSrc->m_kWorld;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// renderer data
//---------------------------------------------------------------------------
void NiAVObject::PurgeRendererData(NiRenderer* pkRenderer)
{
    NiProperty* pkProperty = GetProperty(NiProperty::MATERIAL);

    if (pkProperty != NULL)
        pkRenderer->PurgeMaterial((NiMaterialProperty*) pkProperty);
}
//---------------------------------------------------------------------------
#ifndef __SPU__
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiAVObject* NiAVObject::CloneInPlace(NiAVObject* pkDest, 
    NiCloningProcess& kCloning)
{
    NIASSERT(pkDest != NULL);

    // pkDest should share properties with this NiAVObject rather than
    // cloning them.  We can do this by adding them to the clone map
    // explicitly.
    NiTListIterator kPos = m_kPropertyList.GetTailPos();
    while (kPos)
    {
        NiProperty* pkProperty = m_kPropertyList.GetPrev(kPos);
        kCloning.m_pkCloneMap->SetAt(pkProperty, pkProperty);
    }

    CopyMembers(pkDest, kCloning);
    NiAVObject::ProcessClone(kCloning);
    return pkDest;
}
//---------------------------------------------------------------------------
void NiAVObject::CloneProperties(NiAVObject* pkDest, NiPropertyList* pkList,
    NiCloningProcess& kCloning)
{
    // iterate in reverse order so that new list 
    // will come out in the correct order
    NiTListIterator kPos = pkList->GetTailPos();
    while (kPos)
    {
        NiProperty* pkProperty = pkList->GetPrev(kPos);
        
        pkDest->AttachProperty((NiProperty*)pkProperty->CreateSharedClone(
            kCloning));
    }
}
//---------------------------------------------------------------------------
void NiAVObject::ProcessPropertyClones(NiPropertyList* pkList,
    NiCloningProcess& kCloning)
{
    NiTListIterator kPos = pkList->GetTailPos();
    while (kPos)
    {
        NiProperty* pkProperty = pkList->GetPrev(kPos);
        bool bTemp;
        bool bProcessed = kCloning.m_pkProcessMap->GetAt(pkProperty, bTemp);
        if (!bProcessed)
        {
            pkProperty->ProcessClone(kCloning);
        }
    }
}
//---------------------------------------------------------------------------
void NiAVObject::CopyMembers(NiAVObject* pkDest,
    NiCloningProcess& kCloning)
{
    NiObjectNET::CopyMembers(pkDest, kCloning);

    // copy flags and model data
    pkDest->m_uFlags = m_uFlags;
    pkDest->m_kLocal = m_kLocal;

    // clone properties
    if (!m_kPropertyList.IsEmpty())
    {
        CloneProperties(pkDest, &m_kPropertyList, kCloning);
    }

    // clone Collision object.
    if (m_spCollisionObject)
    {
        NiCollisionObject* pColObj =
            NiSmartPointerCast(NiCollisionObject,m_spCollisionObject);

        pkDest->SetCollisionObject(
            (NiCollisionObject*)pColObj->CreateClone(kCloning));
    }

    // note: world data is not copied.  Clone must be added to scene graph 
    // and Updated to generate this data
}
//---------------------------------------------------------------------------
void NiAVObject::ProcessClone(NiCloningProcess& kCloning)
{
    NiObjectNET::ProcessClone(kCloning);

    if (!m_kPropertyList.IsEmpty())
    {
        ProcessPropertyClones(&m_kPropertyList, kCloning);
    }

    if (m_spCollisionObject)
    {
        m_spCollisionObject->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiAVObject::LoadBinary(NiStream& kStream)
{
    NiObjectNET::LoadBinary(kStream);

    // read native type members
    NiStreamLoadBinary(kStream, m_uFlags);

    m_kLocal.m_Translate.LoadBinary(kStream);
    m_kLocal.m_Rotate.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_kLocal.m_fScale);

    // read property pointers
    kStream.ReadMultipleLinkIDs();  // m_kPropertyList
    kStream.ReadLinkID(); // m_pkCollisionObject

// --- Begin NIF conversion code
    // All files older than 20.0.0.4 have no DISABLE_SORTING flag, and thus
    // we should clear the bit to be safe.
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 0, 0, 4))
    {
        SetBit(false, DISABLE_SORTING);
    }
// --- End NIF conversion code
}
//---------------------------------------------------------------------------
void NiAVObject::LinkObject(NiStream& kStream)
{
    NiObjectNET::LinkObject(kStream);

    // link the properties
    int uiListSize = kStream.GetNumberOfLinkIDs();

    while (uiListSize--)
        m_kPropertyList.AddHead(
            (NiProperty*) kStream.GetObjectFromLinkID());

    
    m_spCollisionObject = 
        (NiCollisionObject*) kStream.GetObjectFromLinkID();

    if (m_spCollisionObject)
        m_spCollisionObject->Convert(kStream.GetFileVersion(), NULL);
}
//---------------------------------------------------------------------------
bool NiAVObject::RegisterStreamables(NiStream& kStream)
{
    if (!NiObjectNET::RegisterStreamables(kStream))
        return false;

    // register properties
    NiTListIterator kPos = m_kPropertyList.GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProperty = m_kPropertyList.GetNext(kPos);
        if (pkProperty)
            pkProperty->RegisterStreamables(kStream);
    }
    if (m_spCollisionObject)
        m_spCollisionObject->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiAVObject::SaveBinary(NiStream& kStream)
{
    NiObjectNET::SaveBinary(kStream);

    // save native type members
    NiStreamSaveBinary(kStream, m_uFlags);
    m_kLocal.m_Translate.SaveBinary(kStream);
    m_kLocal.m_Rotate.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_kLocal.m_fScale);

    // save property pointers
    int iListSize = m_kPropertyList.GetSize();
    NiStreamSaveBinary(kStream, iListSize);
    if (iListSize > 0)
    {
        // save in reverse order because properties will link faster that way

        NiProperty** ppkProperty = NiAlloc(NiProperty*, iListSize);
        int i = 0;
        NiTListIterator kPos = m_kPropertyList.GetHeadPos();
        while (kPos)
        {
            ppkProperty[i++] = m_kPropertyList.GetNext(kPos);
        }

        for (i = iListSize - 1; i >= 0; i--)
            kStream.SaveLinkID(ppkProperty[i]);
        NiFree(ppkProperty);
    }

    kStream.SaveLinkID(m_spCollisionObject);
}
//---------------------------------------------------------------------------
bool NiAVObject::IsEqual(NiObject* pkObject)
{
    if (!NiObjectNET::IsEqual(pkObject))
        return false;

    NiAVObject* pkAVObject = (NiAVObject*) pkObject;

    if (GetAppCulled() != pkAVObject->GetAppCulled())
        return false;

    if (m_kLocal != pkAVObject->m_kLocal)
        return false;

    unsigned int uiCount0 = m_kPropertyList.GetSize();
    unsigned int uiCount1 = pkAVObject->m_kPropertyList.GetSize();
    if (uiCount0 != uiCount1)
        return false;

    if (uiCount0 > 0)
    {
        NiTListIterator kPos0 = m_kPropertyList.GetHeadPos();
        NiTListIterator kPos1 = pkAVObject->m_kPropertyList.GetHeadPos();
        while (kPos0)
        {
            NiProperty* pkProperty0 = m_kPropertyList.GetNext(kPos0);
            NiProperty* pkProperty1 = 
                pkAVObject->m_kPropertyList.GetNext(kPos1);
            if ((pkProperty0 && !pkProperty1) || 
                (!pkProperty0 && pkProperty1))
            {
                return false;
            }

            if (pkProperty0 && !pkProperty0->IsEqual(pkProperty1))
                return false;
        }
    }

    NiCollisionObject* pkCO1 = GetCollisionObject();
    NiCollisionObject* pkCO2 = pkAVObject->GetCollisionObject();

    if (pkCO1 && pkCO2)
    {
        // The collision objects are involved.
        if (pkCO1->IsEqual(pkCO2) == false)
            return false;
    }
    else
    {
        // If equal, they are NULL
        if (pkCO1 != pkCO2)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiAVObject::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObjectNET::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiAVObject::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_bAppCulled", GetAppCulled()));
    pkStrings->Add(m_kLocal.m_Translate.GetViewerString("m_localTranslate"));
    pkStrings->Add(m_kLocal.m_Rotate.GetViewerString("m_localRotate"));
    pkStrings->Add(NiGetViewerString("m_fLocalScale", m_kLocal.m_fScale));

    pkStrings->Add(m_kWorld.m_Translate.GetViewerString("m_worldTranslate"));
    pkStrings->Add(m_kWorld.m_Rotate.GetViewerString("m_worldRotate"));
    pkStrings->Add(NiGetViewerString("m_worldScale", m_kWorld.m_fScale));
    pkStrings->Add(m_kWorldBound.GetViewerString("m_kWorldBound"));

    pkStrings->Add(NiGetViewerString("SelUpdate", GetSelectiveUpdate()));
    pkStrings->Add(NiGetViewerString("SelUpdateTransforms", 
        GetSelectiveUpdateTransforms()));
    pkStrings->Add(NiGetViewerString("SelUpdateRigid", 
        GetSelectiveUpdateRigid()));
    pkStrings->Add(NiGetViewerString("SelUpdatePropControllers", 
        GetSelectiveUpdatePropertyControllers()));

    if (!m_kPropertyList.IsEmpty())
    {
        NiTListIterator kPos = m_kPropertyList.GetHeadPos();
        while (kPos)
        {
            NiProperty* pkProperty = m_kPropertyList.GetNext(kPos);
            pkStrings->Add(NiGetViewerString(
                pkProperty->GetRTTI()->GetName(), pkProperty));
        }
    }

    if (m_spCollisionObject)
        pkStrings->Add(NiGetViewerString("m_spCollisionObject", 
            m_spCollisionObject));
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
void NiAVObject::RegisterDefaultUpdateProcess(NiUpdateProcess* pkUpdateProcess)
{
    ms_spDefaultUpdateProcess = pkUpdateProcess;
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateDownwardPass(float, bool)
{
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateSelectedDownwardPass(float)
{
}
//---------------------------------------------------------------------------
void NiAVObject::UpdateRigidDownwardPass(float)
{
}
//---------------------------------------------------------------------------
