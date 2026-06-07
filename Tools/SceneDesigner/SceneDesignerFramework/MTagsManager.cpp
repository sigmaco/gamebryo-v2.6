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
#include "SceneDesignerFrameworkPCH.h"

#include "MTagsManager.h"
#include "MFramework.h"
#include "MLayer.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MTagsManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MTagsManager();
    }
}
//---------------------------------------------------------------------------
void MTagsManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MTagsManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MTagsManager* MTagsManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MTagsManager::MTagsManager()
{
    __hook(&MEventManager::SceneClosing, MEventManager::Instance,
        &MTagsManager::OnSceneClosing);
    __hook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
        &MTagsManager::OnNewSceneLoaded);
    __hook(&MEventManager::LayerAdded, MEventManager::Instance,
        &MTagsManager::OnLayerAdded);
    __hook(&MEventManager::EntityAddedToScene, MEventManager::Instance,
        &MTagsManager::OnEntityAddedToScene);
    __hook(&MEventManager::EntityRemovedFromScene, MEventManager::Instance,
        &MTagsManager::OnEntityRemovedFromScene);
    __hook(&MEventManager::EntityTagsChanged, MEventManager::Instance,
        &MTagsManager::OnEntityTagsChanged);

    m_pmTagToEntityMap = new Dictionary<String*, List<MEntity*>*>();
}
//---------------------------------------------------------------------------
void MTagsManager::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        __unhook(&MEventManager::SceneClosing, MEventManager::Instance,
        &MTagsManager::OnSceneClosing);
        __unhook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
            &MTagsManager::OnNewSceneLoaded);
        __unhook(&MEventManager::LayerAdded, MEventManager::Instance,
            &MTagsManager::OnLayerAdded);
        __unhook(&MEventManager::EntityAddedToScene, MEventManager::Instance,
            &MTagsManager::OnEntityAddedToScene);
        __unhook(&MEventManager::EntityRemovedFromScene,
            MEventManager::Instance, &MTagsManager::OnEntityRemovedFromScene);
        __unhook(&MEventManager::EntityTagsChanged, MEventManager::Instance,
            &MTagsManager::OnEntityTagsChanged);
    }
}
//---------------------------------------------------------------------------
MEntity* MTagsManager::GetEntitiesForTag(String* strTag)[]
{
    MVerifyValidInstance;

    if (m_pmTagToEntityMap->ContainsKey(strTag))
    {
        return m_pmTagToEntityMap->Item[strTag]->ToArray();
    }
    else
    {
        return new MEntity*[0];
    }
}
//---------------------------------------------------------------------------
String* MTagsManager::GetTags()[]
{
    MVerifyValidInstance;

    List<String*>* pmTags = new List<String*>();
    Dictionary<String*, List<MEntity*>*>::Enumerator mEnum =
        m_pmTagToEntityMap->GetEnumerator();
    while (mEnum.MoveNext())
    {
        pmTags->Add(mEnum.Current.Key);
    }

    return pmTags->ToArray();
}
//---------------------------------------------------------------------------
void MTagsManager::OnSceneClosing(MScene*)
{
    MVerifyValidInstance;

    m_pmTagToEntityMap->Clear();
    MFramework::Instance->EventManager->RaiseTagsManagerUpdated();
}
//---------------------------------------------------------------------------
void MTagsManager::OnNewSceneLoaded(MScene* pmScene)
{
    MVerifyValidInstance;

    m_pmTagToEntityMap->Clear();
    AddEntitiesToMap(pmScene->GetEntities());
    MFramework::Instance->EventManager->RaiseTagsManagerUpdated();
}
//---------------------------------------------------------------------------
void MTagsManager::OnLayerAdded(MLayer* pmLayer, MLayer*)
{
    MVerifyValidInstance;

    AddEntitiesToMap(pmLayer->GetEntities());
    MFramework::Instance->EventManager->RaiseTagsManagerUpdated();
}
//---------------------------------------------------------------------------
void MTagsManager::OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmScene == MFramework::Instance->Scene)
    {
        AddEntitiesToMap(new MEntity*[] { pmEntity });
        MFramework::Instance->EventManager->RaiseTagsManagerUpdated();
    }
}
//---------------------------------------------------------------------------
void MTagsManager::OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmScene == MFramework::Instance->Scene)
    {
        RemoveEntitiesFromMap(new MEntity*[] { pmEntity });
        MFramework::Instance->EventManager->RaiseTagsManagerUpdated();
    }
}
//---------------------------------------------------------------------------
void MTagsManager::OnEntityTagsChanged(MEntity* pmEntity, String* strOldTags)
{
    MVerifyValidInstance;

    String* astrOldTags[] = strOldTags->Split(MEntity::TagDelimiterString
        ->ToCharArray());

    if (MFramework::Instance->Scene->IsEntityInScene(pmEntity))
    {
        // The entity is in the scene, so remove it from the old tags and add
        // it to the new tags.
        RemoveEntityFromTags(astrOldTags, pmEntity);
        AddEntitiesToMap(new MEntity*[] { pmEntity });
    }
    else
    {
        // Search through all entries in the scene, identifying entities that
        // inherit tags from this entity and queueing them up to be modified.
        List<MEntity*>* pmEntitiesToProcess = new List<MEntity*>();
        MEntity* amSceneEntities[] = MFramework::Instance->Scene
            ->GetEntities();
        for (int iEntity = 0; iEntity < amSceneEntities->Length; ++iEntity)
        {
            MEntity* pmSceneEntity = amSceneEntities[iEntity];
            MEntity* pmMasterEntity = pmSceneEntity->MasterEntity;
            while (pmMasterEntity != NULL)
            {
                if (pmMasterEntity == pmEntity)
                {
                    // The scene entity inherits tags from the changed
                    // entity. Add the scene entity to a list to be
                    // processed later.
                    pmEntitiesToProcess->Add(pmSceneEntity);
                    break;
                }

                pmMasterEntity = pmMasterEntity->MasterEntity;
            }
        }

        // Now that all affected entities in the scene have been identified,
        // remove them from the old tags and add them to the new tags.
        MEntity* amEntitiesToProcess[] = pmEntitiesToProcess->ToArray();
        for (int iEntity = 0; iEntity < amEntitiesToProcess->Length; ++iEntity)
        {
            RemoveEntityFromTags(astrOldTags, amEntitiesToProcess[iEntity]);
        }
        AddEntitiesToMap(amEntitiesToProcess);
    }

    MFramework::Instance->EventManager->RaiseTagsManagerUpdated();
}
//---------------------------------------------------------------------------
void MTagsManager::AddEntitiesToMap(MEntity* amEntities[])
{
    MVerifyValidInstance;

    for (int iEntity = 0; iEntity < amEntities->Length; ++iEntity)
    {
        MEntity* pmEntityToAdd = amEntities[iEntity];
        MEntity* pmEntity = pmEntityToAdd;
        while (pmEntity != NULL)
        {
            AddEntityToTags(pmEntity->TagsArray, pmEntityToAdd);
            pmEntity = pmEntity->MasterEntity;
        }
    }
}
//---------------------------------------------------------------------------
void MTagsManager::RemoveEntitiesFromMap(MEntity* amEntities[])
{
    MVerifyValidInstance;

    for (int iEntity = 0; iEntity < amEntities->Length; ++iEntity)
    {
        MEntity* pmEntityToRemove = amEntities[iEntity];
        MEntity* pmEntity = pmEntityToRemove;
        while (pmEntity != NULL)
        {
            RemoveEntityFromTags(pmEntity->TagsArray, pmEntityToRemove);
            pmEntity = pmEntity->MasterEntity;
        }
    }
}
//---------------------------------------------------------------------------
void MTagsManager::AddEntityToTags(String* astrTags[], MEntity* pmEntity)
{
    MVerifyValidInstance;

    for (int iTag = 0; iTag < astrTags->Length; ++iTag)
    {
        String* strTag = astrTags[iTag];
        if (String::IsNullOrEmpty(strTag))
        {
            continue;
        }
        List<MEntity*>* pmTagEntities = NULL;
        if (m_pmTagToEntityMap->ContainsKey(strTag))
        {
            pmTagEntities = m_pmTagToEntityMap->Item[strTag];
        }
        if (pmTagEntities == NULL)
        {
            pmTagEntities = new List<MEntity*>();
            m_pmTagToEntityMap->Item[strTag] = pmTagEntities;
        }
        if (!pmTagEntities->Contains(pmEntity))
        {
            pmTagEntities->Add(pmEntity);
        }
    }
}
//---------------------------------------------------------------------------
void MTagsManager::RemoveEntityFromTags(
    String* astrTags[],
    MEntity* pmEntity)
{
    MVerifyValidInstance;

    for (int iTag = 0; iTag < astrTags->Length; ++iTag)
    {
        String* strTag = astrTags[iTag];
        if (String::IsNullOrEmpty(strTag))
        {
            continue;
        }
        if (m_pmTagToEntityMap->ContainsKey(strTag))
        {
            List<MEntity*>* pmTagEntities = m_pmTagToEntityMap->Item[
                strTag];
            if (pmTagEntities != NULL)
            {
                pmTagEntities->Remove(pmEntity);
                if (pmTagEntities->Count == 0)
                {
                    m_pmTagToEntityMap->Remove(strTag);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
