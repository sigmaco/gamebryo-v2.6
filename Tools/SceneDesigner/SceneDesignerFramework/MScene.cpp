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

#include "MScene.h"
#include "MEntityFactory.h"
#include "MSelectionSetFactory.h"
#include "MEventManager.h"
#include "ServiceProvider.h"
#include "MUtility.h"
#include "MAddRemoveEntityCommand.h"
#include "MAddRemoveSelectionSetCommand.h"
#include "MFramework.h"
#include "MLayerManager.h"
#include "MPrefabManager.h"
#include "MPrefab.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MScene::MScene(NiScene* pkScene) :
    m_pkScene(pkScene),
    m_bDirtyBit(false)
{
    MInitRefObject(m_pkScene);

    m_pkErrors = NiNew NiDefaultErrorHandler();
    MInitRefObject(m_pkErrors);
    
    __hook(&MEventManager::EntityComponentAdded, MEventManager::Instance,
        &MScene::OnEntityComponentAdded);
    __hook(&MEventManager::EntityComponentRemoved, MEventManager::Instance,
        &MScene::OnEntityComponentRemoved);
    __hook(&MEventManager::EntityRemovedFromScene, MEventManager::Instance,
        &MScene::OnEntityRemovedFromScene);
    __hook(&MEventManager::EntityPropertyChanged, MEventManager::Instance,
        &MScene::OnEntityPropertyChanged);
    __hook(&MEventManager::ComponentPropertyRemoved, MEventManager::Instance,
        &MScene::OnComponentPropertyRemoved);
}
//---------------------------------------------------------------------------
void MScene::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        const unsigned int uiEntityCount = m_pkScene->GetEntityCount();
        for (unsigned int ui = 0; ui < uiEntityCount; ui++)
        {
            MEntityFactory::Instance->Remove(m_pkScene->GetEntityAt(ui));
        }

        const unsigned int uiSelectionSetCount =
            m_pkScene->GetSelectionSetCount();
        for (unsigned int ui = 0; ui < uiSelectionSetCount; ui++)
        {
            MSelectionSetFactory::Instance->Remove(
                m_pkScene->GetSelectionSetAt(ui));
        }

        MDisposeRefObject(m_pkErrors);
        MDisposeRefObject(m_pkScene);

        __unhook(&MEventManager::EntityComponentAdded,
            MEventManager::Instance, &MScene::OnEntityComponentAdded);
        __unhook(&MEventManager::EntityComponentRemoved,
            MEventManager::Instance, &MScene::OnEntityComponentRemoved);
        __unhook(&MEventManager::EntityRemovedFromScene,
            MEventManager::Instance, &MScene::OnEntityRemovedFromScene);
        __unhook(&MEventManager::EntityPropertyChanged,
            MEventManager::Instance, &MScene::OnEntityPropertyChanged);
        __unhook(&MEventManager::ComponentPropertyRemoved, 
            MEventManager::Instance, &MScene::OnComponentPropertyRemoved);
    }
}
//---------------------------------------------------------------------------
void MScene::OnEntityComponentAdded(MEntity* pmEntity, MComponent* pmComponent)
{
    MVerifyValidInstance;

    // Only handle this event if the scene is a prefab or the main scene
    if (MFramework::Instance->Scene != this)
    {
        bool isPrefab = false;
        MPrefab* prefabs[] = MFramework::Instance->PrefabManager->GetPrefabs();
        for (int i = 0; i < prefabs->Length; i++)
        {
            if (prefabs[i]->Scene == this)
            {
                isPrefab = true;
                break;
            }
        }

        if (!isPrefab)
            return;
    }
    
    unsigned int uiEntityCount = m_pkScene->GetEntityCount();
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        MEntity* pmEntityInScene = MEntityFactory::Instance->Get(
            m_pkScene->GetEntityAt(ui));
        if (pmEntityInScene->RootMasterEntity == pmEntity)
        {
            ResolveAddedComponentDependencies(pmEntityInScene,
                pmComponent->TemplateID);
        }
    }
}

//---------------------------------------------------------------------------
void MScene::ResolveAddedComponentDependencies(MEntity* pmEntity, 
    Guid mTemplateID)
{
    MVerifyValidInstance;

    if (pmEntity == NULL)
        return;

    // The entity is resolved if it already has the component or has no master
    MComponent* pmComponent = pmEntity->GetComponentByTemplateID(mTemplateID);
    MEntity* pmMasterEntity = pmEntity->MasterEntity;
    if (pmComponent != NULL || pmMasterEntity == NULL)
        return;

    // Resolve the master before you move to the current entity
    ResolveAddedComponentDependencies(pmMasterEntity, mTemplateID);
    pmComponent = pmMasterEntity->GetComponentByTemplateID(mTemplateID);
    if (pmComponent)
        pmEntity->AddComponent(pmComponent->Clone(true), false, true);
}
//---------------------------------------------------------------------------
void MScene::OnEntityComponentRemoved(MEntity* pmEntity,
    MComponent* pmComponent)
{
    MVerifyValidInstance;

    // Only handle this event if the scene is a prefab or the main scene
    if (MFramework::Instance->Scene != this)
    {
        bool isPrefab = false;
        MPrefab* prefabs[] = MFramework::Instance->PrefabManager->GetPrefabs();
        for (int i = 0; i < prefabs->Length; i++)
        {
            if (prefabs[i]->Scene == this)
            {
                isPrefab = true;
                break;
            }
        }

        if (!isPrefab)
            return;
    }

    unsigned int uiEntityCount = m_pkScene->GetEntityCount();
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        MEntity* pmEntityInScene = MEntityFactory::Instance->Get(
            m_pkScene->GetEntityAt(ui));
        if (pmEntityInScene->RootMasterEntity == pmEntity)
        {
            ResolveRemovedComponentDependencies(pmEntityInScene,
                pmComponent->TemplateID);
        }
    }
}
//---------------------------------------------------------------------------
void MScene::ResolveRemovedComponentDependencies(MEntity* pmEntity,
    Guid mTemplateID)
{
    MVerifyValidInstance;

    while(pmEntity != NULL)
    {
        MComponent* pmComponent = pmEntity->GetComponentByTemplateID(mTemplateID);
        if (pmComponent != NULL)
        {
            pmEntity->RemoveComponent(pmComponent, false, true);
            pmEntity = pmEntity->MasterEntity;
        }
        else
        {
            pmEntity = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void MScene::OnEntityRemovedFromScene(MScene*, MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (!CommandService->BeginUndoFrame(String::Format("Remove all references "
        "to \"{0}\" entity from \"{1}\" scene", pmEntity->Name, this->Name)))
    {
        return;
    }

    for (unsigned int ui = 0; ui < m_pkScene->GetEntityCount(); ui++)
    {
        MEntity* pmEntityInScene = MEntityFactory::Instance->Get(
            m_pkScene->GetEntityAt(ui));
        ResolveRemovedEntityDependencies(pmEntityInScene, pmEntity);
    }

    for (unsigned int ui = 0; ui < m_pkScene->GetSelectionSetCount(); ui++)
    {
        MSelectionSet* pmSelectionSet = MSelectionSetFactory::Instance->Get(
            m_pkScene->GetSelectionSetAt(ui));
        ResolveRemovedEntityDependencies(pmSelectionSet, pmEntity);
    }

    CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
void MScene::ResolveRemovedEntityDependencies(MEntity* pmEntity,
    MEntity* pmRemovedEntity)
{
    MVerifyValidInstance;

    String* astrPropertyNames[] = pmEntity->GetPropertyNames();
    for (int i = 0; i < astrPropertyNames->Length; i++)
    {
        String* strPropertyName = astrPropertyNames[i];
        PropertyType* pmPropertyType = pmEntity->GetPropertyType(
            strPropertyName);
        if (pmPropertyType != NULL && pmPropertyType->PrimitiveType->Equals(
            NiEntityPropertyInterface::PT_ENTITYPOINTER))
        {
            for (unsigned int ui = 0;
                ui < pmEntity->GetElementCount(strPropertyName); ui++)
            {
                MEntity* pmEntityProperty = dynamic_cast<MEntity*>(
                    pmEntity->GetPropertyData(strPropertyName, ui));
                if (pmEntityProperty != NULL)
                {
                    if (pmEntityProperty == pmRemovedEntity)
                    {
                        pmEntity->SetPropertyData(strPropertyName, NULL, ui,
                            true);
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void MScene::ResolveRemovedEntityDependencies(MSelectionSet* pmSelectionSet,
    MEntity* pmRemovedEntity)
{
    MVerifyValidInstance;

    MEntity* amEntities[] = pmSelectionSet->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        if (pmEntity == pmRemovedEntity)
        {
            pmSelectionSet->RemoveEntity(pmEntity);
            break;
        }
    }

    if (pmSelectionSet->EntityCount == 0)
    {
        RemoveSelectionSet(pmSelectionSet);
    }
}
//---------------------------------------------------------------------------
void MScene::OnEntityPropertyChanged(MEntity* pmEntity,
    String* strPropertyName, unsigned int uiPropertyIndex, bool )
{
    MVerifyValidInstance;

    if (pmEntity->IsExternalAssetPath(strPropertyName, uiPropertyIndex))
    {
        if (MFramework::Instance->Scene->IsEntityInScene(pmEntity))
        {
            RefreshEntityReferences(pmEntity);
        }
        else
        {
            MEntity* amDependentEntities[] = MFramework::Instance->Scene
                ->GetDependentEntities(pmEntity);
            for (int i = 0; i < amDependentEntities->Length; i++)
            {
                MEntity* pmDependentEntity = amDependentEntities[i];
                if (MFramework::Instance->Scene->IsEntityInScene(
                    pmDependentEntity))
                {
                    RefreshEntityReferences(pmDependentEntity);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void MScene::RefreshEntityReferences(MEntity* pmChangedEntity)
{
    const unsigned int uiEntityCount = m_pkScene->GetEntityCount();
    for (unsigned int uiEntityIdx = 0; uiEntityIdx < uiEntityCount;
        ++uiEntityIdx)
    {
        MEntity* pmEntity = MEntityFactory::Instance->Get(
            m_pkScene->GetEntityAt(uiEntityIdx));
        String* astrPropertyNames[] = pmEntity->GetPropertyNames();
        for (int iPropNameIdx = 0; iPropNameIdx < astrPropertyNames->Length;
            ++iPropNameIdx)
        {
            String* strPropertyName = astrPropertyNames[iPropNameIdx];
            PropertyType* pmPropertyType = pmEntity->GetPropertyType(
                strPropertyName);
            if (pmPropertyType != NULL &&
                pmPropertyType->PrimitiveType->Equals(
                    NiEntityPropertyInterface::PT_ENTITYPOINTER))
            {
                const unsigned int uiElementCount =
                    pmEntity->GetElementCount(strPropertyName);
                for (unsigned int uiElementIdx = 0;
                    uiElementIdx < uiElementCount; ++uiElementIdx)
                {
                    MEntity* pmEntityProperty = dynamic_cast<MEntity*>(
                        pmEntity->GetPropertyData(strPropertyName,
                        uiElementIdx));
                    if (pmEntityProperty != NULL &&
                        pmEntityProperty == pmChangedEntity)
                    {
                        pmEntity->SetPropertyData(strPropertyName, NULL,
                            uiElementIdx, true);
                        pmEntity->SetPropertyData(strPropertyName,
                            pmEntityProperty, uiElementIdx, true);
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void MScene::OnComponentPropertyRemoved(MComponent* pmComponent, 
    String* strPropertyName, bool /*bInBatch*/)
{
    MVerifyValidInstance;

    Guid kTemplateID = pmComponent->TemplateID;
    unsigned int uiEntityCount = m_pkScene->GetEntityCount();
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        MEntity* pmEntity = MEntityFactory::Instance->Get(
            m_pkScene->GetEntityAt(ui));
        if (pmEntity->GetComponentByTemplateID(kTemplateID) &&
            pmEntity->HasProperty(strPropertyName))
        {
            // Make sure the component being updated is linked to the 
            // component on the entity
            MComponent* pmEntityComponent = 
                pmEntity->GetComponentByTemplateID(kTemplateID);
            while (pmEntityComponent != NULL && pmEntityComponent != pmComponent)
                 pmEntityComponent = pmEntityComponent->MasterComponent;
            if (pmEntityComponent != NULL)
            {
                ResolveRemovedPropertyDependencies(pmEntity, pmComponent, 
                   strPropertyName);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MScene::ResolveRemovedPropertyDependencies(MEntity* pmEntity,
    MComponent* pmComponent, String* strPropertyName)
{
    if (pmEntity == NULL)
    {
        return;
    }

    MComponent* pmEntityComponent = 
        pmEntity->GetComponentByTemplateID(pmComponent->TemplateID);
    if (pmEntityComponent != pmComponent)
    {
        ResolveRemovedPropertyDependencies(pmEntity->MasterEntity, 
            pmComponent, strPropertyName);
    }

    if (pmEntity->HasProperty(strPropertyName))
    {
        pmEntityComponent->RemoveProperty(strPropertyName, true);
    }    
}

//---------------------------------------------------------------------------
NiScene* MScene::GetNiScene()
{
    MVerifyValidInstance;

    return m_pkScene;
}
//---------------------------------------------------------------------------
String* MScene::get_Name()
{
    MVerifyValidInstance;

    return m_pkScene->GetName();
}
//---------------------------------------------------------------------------
void MScene::set_Name(String* strName)
{
    MVerifyValidInstance;

    this->Dirty = true;

    const char* pcName = MStringToCharPointer(strName);
    m_pkScene->SetName(pcName);
    MFreeCharPointer(pcName);
}
//---------------------------------------------------------------------------
unsigned int MScene::get_EntityCount()
{
    MVerifyValidInstance;

    return m_pkScene->GetEntityCount();
}
//---------------------------------------------------------------------------
bool MScene::get_Dirty()
{
    MVerifyValidInstance;

    // we don't check our entities anymore because they now tell 
    // us when they are dirty
    return m_bDirtyBit;
}
//---------------------------------------------------------------------------
void MScene::set_Dirty(bool bDirty)
{
    MVerifyValidInstance;

    MAssert(!bDirty || this->Writable, "Object is read-only and cannot be "
        "modified!");

    m_bDirtyBit = bDirty;

    // Clear dirty flags for all entities in this scene.
    if (!bDirty)
    {
        MEntity* amEntities[] = GetEntities();
        for (int i = 0; i < amEntities->Length; ++i)
        {
            amEntities[i]->Dirty = false;
        }
    }
}
//---------------------------------------------------------------------------
MEntity* MScene::GetEntities()[]
{
    MVerifyValidInstance;

    unsigned int uiEntityCount = m_pkScene->GetEntityCount();

    MEntity* amEntities[] = new MEntity*[uiEntityCount];
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        amEntities[ui] = MEntityFactory::Instance->Get(
            m_pkScene->GetEntityAt(ui));
    }

    return amEntities;
}

//---------------------------------------------------------------------------
MEntity* MScene::GetEntityByName(String* strName)
{
    MVerifyValidInstance;

    const char* pcName = MStringToCharPointer(strName);
    NiEntityInterface* pkEntity = m_pkScene->GetEntityByName(pcName);
    MFreeCharPointer(pcName);

    if (pkEntity != NULL)
    {
        return MEntityFactory::Instance->Get(pkEntity);
    }

    return NULL;
}
//---------------------------------------------------------------------------
MEntity* MScene::GetEntityByID(Guid mID)
{
    MVerifyValidInstance;

    NiUniqueID kID;
    MUtility::GuidToID(mID, kID);
    NiEntityInterface* pkEntity = m_pkScene->GetEntityByID(kID);
    if (pkEntity)
    {
        return MEntityFactory::Instance->Get(pkEntity);
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool MScene::AddEntity(MEntity* pmEntity, bool bUndoable)
{
    MVerifyValidInstance;

    this->Dirty = true;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    if (!IsEntityInScene(pmEntity))
    {
        CommandService->ExecuteCommand(new MAddRemoveEntityCommand(
            NiNew NiAddRemoveEntityCommand(m_pkScene,
            pmEntity->GetNiEntityInterface(), true)), bUndoable);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void MScene::RemoveEntity(MEntity* pmEntity, bool bUndoable)
{
    MVerifyValidInstance;

    if (pmEntity->PrefabRoot != NULL)
    {
        String* strMsg = String::Format("The \"{0}\" entity cannot be removed "
            "from the scene because it is part of a prefab entity. To remove "
            "the sub-entity you should change its visibility.", pmEntity->Name, 
            this->Name);
        String* strDetails = String::Format("The parent entity is \"{0}\".", 
            pmEntity->PrefabRoot->Name);
        this->MessageService->AddMessage(MessageChannelType::Errors,
            new Emergent::Gamebryo::SceneDesigner::PluginAPI::Message(
            strMsg, strDetails, pmEntity));
        return;
    }
    MLayer* amDependentLayers[] = MFramework::Instance->LayerManager
        ->SearchReadOnlyLayersForEntityReference(pmEntity);
    if (amDependentLayers->Length > 0)
    {
        String* strMsg = String::Format("The \"{0}\" entity cannot be removed "
            "from the scene because it is referenced by one or more "
            "read-only layers.", pmEntity->Name, this->Name);
        String* strDetails = String::Format("The following read-only layers "
            "depend on the \"{0}\" entity:\n", pmEntity->Name);
        for (int i = 0; i < amDependentLayers->Length; ++i)
        {
            strDetails = String::Concat(strDetails, String::Format("\t{0}\n",
                amDependentLayers[i]->Name));
        }

        this->MessageService->AddMessage(MessageChannelType::Errors,
            new Emergent::Gamebryo::SceneDesigner::PluginAPI::Message(
            strMsg, strDetails, pmEntity));
        return;
    }

    this->Dirty = true;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    CommandService->ExecuteCommand(new MAddRemoveEntityCommand(
        NiNew NiAddRemoveEntityCommand(m_pkScene,
        pmEntity->GetNiEntityInterface(), false)), bUndoable);
}
//---------------------------------------------------------------------------
void MScene::RemoveAllEntities(bool bUndoable)
{
    MVerifyValidInstance;

    MEntity* amEntities[] = GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        RemoveEntity(amEntities[i], bUndoable);
    }
}
//---------------------------------------------------------------------------
bool MScene::IsEntityInScene(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");
    return NIBOOL_IS_TRUE(
        m_pkScene->IsEntityInScene(pmEntity->GetNiEntityInterface()));
}
//---------------------------------------------------------------------------
MEntity* MScene::GetDependentEntities(MEntity* pmEntity)[]
{
    MEntity* allEntities[] = GetEntities();
    ArrayList* retVal = new ArrayList();
    for (int i = 0; i < allEntities->Count; i++)
    {
        MEntity* pmCurrentEntity = allEntities[i];//->get_Item(i);
        while (pmCurrentEntity != NULL)
        {
            if (pmEntity == pmCurrentEntity->MasterEntity)
            {
                retVal->Add(allEntities[i]);
                break;
            }
            pmCurrentEntity = pmCurrentEntity->MasterEntity;
        }
    }
    return dynamic_cast<MEntity*[]>(retVal->ToArray(__typeof(MEntity)));
}
//---------------------------------------------------------------------------
unsigned int MScene::get_SelectionSetCount()
{
    MVerifyValidInstance;

    return m_pkScene->GetSelectionSetCount();
}
//---------------------------------------------------------------------------
MSelectionSet* MScene::GetSelectionSets()[]
{
    MVerifyValidInstance;

    unsigned int uiSelectionSetCount = m_pkScene->GetSelectionSetCount();

    MSelectionSet* amSelectionSets[] = new MSelectionSet*[
        uiSelectionSetCount];
        for (unsigned int ui = 0; ui < uiSelectionSetCount; ui++)
        {
            amSelectionSets[ui] = MSelectionSetFactory::Instance->Get(
                m_pkScene->GetSelectionSetAt(ui));
        }

        return amSelectionSets;
}
//---------------------------------------------------------------------------
MSelectionSet* MScene::GetSelectionSetByName(String* strName)
{
    MVerifyValidInstance;

    const char* pcName = MStringToCharPointer(strName);
    NiEntitySelectionSet* pkSelectionSet = m_pkScene->GetSelectionSetByName(
        pcName);
    MFreeCharPointer(pcName);

    if (pkSelectionSet != NULL)
    {
        return MSelectionSetFactory::Instance->Get(pkSelectionSet);
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool MScene::AddSelectionSet(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    this->Dirty = true;

    MAssert(pmSelectionSet != NULL, "Null selection set passed to function!");

    if (GetSelectionSetByName(pmSelectionSet->Name) == NULL)
    {
        CommandService->ExecuteCommand(new MAddRemoveSelectionSetCommand(
            NiNew NiAddRemoveSelectionSetCommand(m_pkScene,
            pmSelectionSet->GetNiEntitySelectionSet(), true)), true);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void MScene::RemoveSelectionSet(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    this->Dirty = true;

    MAssert(pmSelectionSet != NULL, "Null selection set passed to function!");

    if (GetSelectionSetByName(pmSelectionSet->Name) != NULL)
    {
        CommandService->ExecuteCommand(new MAddRemoveSelectionSetCommand(
            NiNew NiAddRemoveSelectionSetCommand(m_pkScene,
            pmSelectionSet->GetNiEntitySelectionSet(), false)), true);
    }
}
//---------------------------------------------------------------------------
void MScene::RemoveAllSelectionSets()
{
    MVerifyValidInstance;

    MSelectionSet* amSelectionSets[] = GetSelectionSets();
    for (int i = 0; i < amSelectionSets->Length; i++)
    {
        RemoveSelectionSet(amSelectionSets[i]);
    }
}
//---------------------------------------------------------------------------
bool MScene::IsSelectionSetInScene(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    MAssert(pmSelectionSet != NULL, "Null selection set provided to "
        "function!");

    for (unsigned int ui = 0; ui < m_pkScene->GetSelectionSetCount(); ui++)
    {
        if (pmSelectionSet->GetNiEntitySelectionSet() ==
            m_pkScene->GetSelectionSetAt(ui))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
unsigned int MScene::get_LayerFilenameCount()
{
    MVerifyValidInstance;

    return m_pkScene->GetLayerFilenameCount();
}
//---------------------------------------------------------------------------
String* MScene::GetLayerFilenames()[]
{
    MVerifyValidInstance;

    const unsigned int uiLayerFilenameCount =
        m_pkScene->GetLayerFilenameCount();
    String* amLayerFilenames[] = new String*[uiLayerFilenameCount];
    for (unsigned int ui = 0; ui < uiLayerFilenameCount; ++ui)
    {
        amLayerFilenames[ui] = m_pkScene->GetLayerFilenameAt(ui);
    }

    return amLayerFilenames;
}
//---------------------------------------------------------------------------
bool MScene::AddLayerFilename(String* strLayerFilename)
{
    MVerifyValidInstance;

    const char* pcLayerFilename = MStringToCharPointer(strLayerFilename);
    bool bSuccess = NIBOOL_IS_TRUE(m_pkScene->AddLayerFilename(
        pcLayerFilename));
    MFreeCharPointer(pcLayerFilename);

    // if AddLayerFilename failed it means the file was already there
    // and the scene was not changed.
    if (bSuccess)
    {
        Dirty = true;
        if (m_pmLayer)
        {
            MEventManager::Instance->RaiseLayerFilenameAdded(m_pmLayer, 
                strLayerFilename);
        }
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
bool MScene::RenameLayerFilename(String* strOldLayerFilename,
                                 String* strNewLayerFilename)
{
    MVerifyValidInstance;

    const char* pcOldLayerFilename = MStringToCharPointer(strOldLayerFilename);
    const char* pcNewLayerFilename = MStringToCharPointer(strNewLayerFilename);
    bool bSuccess = NIBOOL_IS_TRUE(m_pkScene->RenameLayerFilename(
        pcOldLayerFilename, pcNewLayerFilename));
    MFreeCharPointer(pcOldLayerFilename);
    MFreeCharPointer(pcNewLayerFilename);

    // if RemoveLayerFilename failed it means the file was not there
    // and the scene was not changed.
    if (bSuccess)
    {
        Dirty = true;
        if (m_pmLayer)
        {
            MEventManager::Instance->RaiseLayerFilenameRenamed(m_pmLayer, 
                strOldLayerFilename, strNewLayerFilename);
        }
    }
    return bSuccess;
}
//---------------------------------------------------------------------------
void MScene::RemoveLayerFilename(String* strLayerFilename)
{
    MVerifyValidInstance;

    const char* pcLayerFilename = MStringToCharPointer(strLayerFilename);
    m_pkScene->RemoveLayerFilename(pcLayerFilename);
    MFreeCharPointer(pcLayerFilename);
    Dirty = true;
    if (m_pmLayer)
    {
        MEventManager::Instance->RaiseLayerFilenameRemoved(m_pmLayer, 
            strLayerFilename);
    }
}
//---------------------------------------------------------------------------
void MScene::RemoveAllLayerFilenames()
{
    MVerifyValidInstance;

    m_pkScene->RemoveAllLayerFilenames();
    Dirty = true;
}
//---------------------------------------------------------------------------
bool MScene::get_Writable()
{
    MVerifyValidInstance;

    if (m_pmLayer != NULL)
    {
        return m_pmLayer->Writable;
    }

    return true;
}
//---------------------------------------------------------------------------
String* MScene::get_SourceFilename()
{
    MVerifyValidInstance;

    return m_pkScene->GetSourceFilename();
}
//---------------------------------------------------------------------------
void MScene::set_SourceFilename(String* strFilename)
{
    MVerifyValidInstance;

    const char* pcFilename = MStringToCharPointer(strFilename);
    m_pkScene->SetSourceFilename(pcFilename);
    MFreeCharPointer(pcFilename);
}
//---------------------------------------------------------------------------
void MScene::Update(float fTime, NiExternalAssetManager* pkAssetManager)
{
    MVerifyValidInstance;

    // Clear out errors in handler.
    m_pkErrors->ClearErrors();
    pkAssetManager->SetErrorHandler(m_pkErrors);

    // Update the scene.
    m_pkScene->Update(fTime, m_pkErrors, pkAssetManager);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
        m_pkErrors);
}
//---------------------------------------------------------------------------
void MScene::UpdateEffects()
{
    MVerifyValidInstance;

    MEntity* amEntities[] = GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        UpdateEffects(amEntities[i]);
    }
}
//---------------------------------------------------------------------------
void MScene::UpdateEffects(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    for (unsigned int ui = 0; ui < pmEntity->GetSceneRootPointerCount();
        ui++)
    {
        NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
        if (pkSceneRoot)
        {
            pkSceneRoot->UpdateEffects();
        }
    }
}
//---------------------------------------------------------------------------
String* MScene::GetUniqueEntityName(String* strProposedName)
{
    MVerifyValidInstance;

    if (GetEntityByName(strProposedName) == NULL)
    {
        return strProposedName;
    }

    String* strBaseName = strProposedName;

    // Search for last space character in proposed name.
    int iIndex = strProposedName->LastIndexOf(' ');
    if (iIndex == strProposedName->Length - 1)
    {
        iIndex = strProposedName->LastIndexOf(' ', iIndex - 1);
    }
    if (iIndex > -1)
    {
        String* strSuffix = strProposedName->Substring(iIndex + 1);
        try
        {
            Int32::Parse(strSuffix);
            strBaseName = strProposedName->Substring(0, iIndex);
        }
        catch (FormatException*)
        {
            // The suffix is not an integer; set the base name to the
            // proposed name.
            strBaseName = strProposedName;
        }

    }

    int iSuffix = 1;
    String* strUniqueName;
    do
    {
        strUniqueName = String::Format("{0} {1:D02}", strBaseName,
            __box(iSuffix++));
    }
    while (GetEntityByName(strUniqueName) != NULL);

    return strUniqueName;
}
//---------------------------------------------------------------------------
String* MScene::GetUniqueSelectionSetName(String* strProposedName)
{
    MVerifyValidInstance;

    if (GetSelectionSetByName(strProposedName) == NULL)
    {
        return strProposedName;
    }

    String* strBaseName = strProposedName;

    // Search for last space character in proposed name.
    int iIndex = strProposedName->LastIndexOf(' ');
    if (iIndex == strProposedName->Length - 1)
    {
        iIndex = strProposedName->LastIndexOf(' ', iIndex - 1);
    }
    if (iIndex > -1)
    {
        String* strSuffix = strProposedName->Substring(iIndex + 1);
        try
        {
            Int32::Parse(strSuffix);
            strBaseName = strProposedName->Substring(0, iIndex);
        }
        catch (FormatException*)
        {
            // The suffix is not an integer; set the base name to the
            // proposed name.
            strBaseName = strProposedName;
        }

    }

    int iSuffix = 1;
    String* strUniqueName;
    do
    {
        strUniqueName = String::Format("{0} {1:D02}", strBaseName,
            __box(iSuffix++));
    }
    while (GetSelectionSetByName(strUniqueName) != NULL);

    return strUniqueName;
}
//---------------------------------------------------------------------------
void MScene::GetBound(NiBound* pkBound)
{
    MVerifyValidInstance;

    m_pkScene->GetBound(*pkBound);
}
//---------------------------------------------------------------------------
IMessageService* MScene::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
ICommandService* MScene::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
MLayer* MScene::get_Layer()
{
    return m_pmLayer;
}
//---------------------------------------------------------------------------
void MScene::set_Layer(MLayer* pmLayer)
{
    m_pmLayer = pmLayer;
}
//---------------------------------------------------------------------------
