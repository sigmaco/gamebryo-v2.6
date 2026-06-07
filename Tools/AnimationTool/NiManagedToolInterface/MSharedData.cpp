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

#include "stdafx.h"
#include "MSharedData.h"

using namespace NiManagedToolInterface;
using namespace System::Threading;

//---------------------------------------------------------------------------
MSharedData::MSharedData() : m_aScenes(NULL), m_pkActorManager(NULL),
    m_pkKFMTool(NULL), m_uiLockCount(0)
{
    m_aScenes = NiNew NiTPrimitiveArray<NiAVObject*>(2, 2);
}
//---------------------------------------------------------------------------
void MSharedData::Init()
{
    if (!ms_pkThis)
    {
        ms_pkThis = NiExternalNew MSharedData;
    }
}
//---------------------------------------------------------------------------
void MSharedData::Shutdown()
{
    if (ms_pkThis)
    {
        ms_pkThis->Lock();
        ms_pkThis->DeleteContents();
        NiDelete ms_pkThis->m_aScenes;
        ms_pkThis->Unlock();
        ms_pkThis = NULL;
    }
}
//---------------------------------------------------------------------------
void MSharedData::Lock()
{
    if (m_uiLockCount == 0)
    {
        Monitor::Enter(this);
    }
    m_uiLockCount++;
}
//---------------------------------------------------------------------------
void MSharedData::Unlock()
{
    m_uiLockCount--;
    if (m_uiLockCount == 0)
    {
        Monitor::Exit(this);
    }
}
//---------------------------------------------------------------------------
void MSharedData::DeleteContents()
{
    NIASSERT(m_uiLockCount > 0);

    for (unsigned int ui = 0; ui < m_aScenes->GetSize(); ui++)
    {
        SetScene(ui, NULL);
    }

    SetActorManager(NULL);
    SetKFMTool(NULL);
}
//---------------------------------------------------------------------------
unsigned int MSharedData::GetSceneArrayCount()
{
    NIASSERT(m_uiLockCount > 0);

    return m_aScenes->GetSize();
}
//---------------------------------------------------------------------------
unsigned int MSharedData::AddScene(NiAVObject* pkScene)
{
    NIASSERT(m_uiLockCount > 0);

    NIASSERT(pkScene);
    pkScene->Update(0.0f);
    NiMesh::CompleteSceneModifiers(pkScene);
    pkScene->UpdateProperties();
    pkScene->UpdateEffects();
    pkScene->UpdateNodeBound();
    pkScene->IncRefCount();
    unsigned int uiIndex;
    for (uiIndex = USER_INDEX_BASE; uiIndex < m_aScenes->GetSize(); uiIndex++)
    {
        NiAVObject* pkTempObj = m_aScenes->GetAt(uiIndex);
        if (!pkTempObj)
        {
            break;
        }
    }
    m_aScenes->SetAtGrow(uiIndex, pkScene);

    OnSceneAdded(uiIndex, pkScene);
    return uiIndex;
}
//---------------------------------------------------------------------------
void MSharedData::RemoveScene(unsigned int uiIndex)
{
    NIASSERT(m_uiLockCount > 0);

    NiAVObject* pkScene = GetScene(uiIndex);
    if (!pkScene)
    {
        return;
    }
    
    OnSceneRemoved(uiIndex, pkScene);

    if (pkScene)
        pkScene->DecRefCount();

    m_aScenes->SetAt(uiIndex, NULL);
}
//---------------------------------------------------------------------------
NiAVObject* MSharedData::GetScene(unsigned int uiIndex)
{
    NIASSERT(m_uiLockCount > 0);

    if (uiIndex >= m_aScenes->GetSize())
    {
        return NULL;
    }

    return m_aScenes->GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void MSharedData::SetScene(unsigned int uiIndex, NiAVObject* pkScene)
{
    NIASSERT(m_uiLockCount > 0);

    if (pkScene)
    {
        pkScene->Update(0.0f);
        NiMesh::CompleteSceneModifiers(pkScene);
        pkScene->UpdateProperties();
        pkScene->UpdateEffects();
        pkScene->UpdateNodeBound();
    }

    NiAVObject* pkOldScene = GetScene(uiIndex);
    m_aScenes->SetAtGrow(uiIndex, pkScene);
    if (pkScene)
    {
        pkScene->IncRefCount();
    }

    if (pkOldScene || pkScene)
    {
        OnSceneChanged(uiIndex, pkOldScene, pkScene);
    }

    if (pkOldScene)
    {
        pkOldScene->DecRefCount();
    }
}
//---------------------------------------------------------------------------
void MSharedData::CompactSceneArray()
{
    NIASSERT(m_uiLockCount > 0);

    m_aScenes->Compact();
    OnSceneArrayCompacted();
}
//---------------------------------------------------------------------------
void MSharedData::SetActorManager(NiActorManager* pkActorManager)
{
    NIASSERT(m_uiLockCount > 0);

    OnActorManagerChanged(m_pkActorManager, pkActorManager);
    if (pkActorManager)
    {
        pkActorManager->IncRefCount();
    }
    if (m_pkActorManager)
    {
        m_pkActorManager->DecRefCount();
    }
    m_pkActorManager = pkActorManager;
}
//---------------------------------------------------------------------------
NiActorManager* MSharedData::GetActorManager()
{
    NIASSERT(m_uiLockCount > 0);

    return m_pkActorManager;
}
//---------------------------------------------------------------------------
void MSharedData::SetKFMTool(NiKFMTool* pkKFMTool)
{
    NIASSERT(m_uiLockCount > 0);

    OnKFMToolChanged(m_pkKFMTool, pkKFMTool);
    if (pkKFMTool)
    {
        pkKFMTool->IncRefCount();
    }
    if (m_pkKFMTool)
    {
        m_pkKFMTool->DecRefCount();
    }
    m_pkKFMTool = pkKFMTool;
}
//---------------------------------------------------------------------------
NiKFMTool* MSharedData::GetKFMTool()
{
    NIASSERT(m_uiLockCount > 0);

    return m_pkKFMTool;
}

//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
MSharedData* MSharedData::get_Instance()
{
    NIASSERT(ms_pkThis);
    return ms_pkThis;
}
//---------------------------------------------------------------------------
