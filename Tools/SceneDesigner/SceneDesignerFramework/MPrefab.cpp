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

#include "MPrefab.h"
#include "MScene.h"
#include "MSceneFactory.h"
#include "ServiceProvider.h"
#include "MFramework.h"
#include "MUtility.h"
#include "MLayerManager.h"

using namespace System::IO;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MPrefab::MPrefab(MScene* pmScene)
    : m_pmScene(pmScene)
{
}
//---------------------------------------------------------------------------
void MPrefab::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        if (m_pmScene)
        {
            MScene* pmScene = MSceneFactory::Instance->Get(
                m_pmScene->GetNiScene());
            MAssert(pmScene == m_pmScene, "Prefab scene does not match "
                "scene returned from factory!");
            // Don't remove it if it is an active layer
            if (!MLayerManager::Instance->GetLayerByName(SourceFilename))
            {
                MSceneFactory::Instance->Remove(pmScene);
            }
        }
    }
}
//---------------------------------------------------------------------------
ICommandService* MPrefab::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
String* MPrefab::get_SourceFilename()
{
    MVerifyValidInstance;

    assert(m_pmScene);
    String* fn = m_pmScene->SourceFilename;
    // make sure all directory separators are the same direction
    fn = fn->Replace('/', '\\');
    return fn;
}
//---------------------------------------------------------------------------
String* MPrefab::get_ShortName()
{
    MVerifyValidInstance;

    assert(m_pmScene);
    return Path::GetFileNameWithoutExtension (m_pmScene->SourceFilename);
}
//---------------------------------------------------------------------------
String* MPrefab::get_Filename()
{
    MVerifyValidInstance;

    assert(m_pmScene);
    return Path::GetFileName(m_pmScene->SourceFilename);
}
//---------------------------------------------------------------------------
String* MPrefab::get_Path()
{
    String* dir = SourceFilename;
    int slash = dir->LastIndexOf("\\");
    if (slash == -1)
    {
        return dir;
    }
    // make it always end with a trailing slash
    dir = dir->Substring(0, slash + 1);
    NIASSERT(dir->EndsWith("\\"));
    return dir;
}
//---------------------------------------------------------------------------
MScene* MPrefab::get_Scene()
{
    MVerifyValidInstance;

    return m_pmScene;
}
//---------------------------------------------------------------------------
bool MPrefab::get_IsInstantiated()
{
    MScene* uberscene = MFramework::Instance->Scene;
    MEntity* amEntitiesInScene[] = uberscene->GetEntities();
    for (int i = 0; i != amEntitiesInScene->Count; ++i)
    {
        if (amEntitiesInScene[i]->HasProperty("Prefab Path"))
        {
            String* prefab_name = reinterpret_cast<String*>(
                amEntitiesInScene[i]->GetPropertyData("Prefab Path"));
            if (prefab_name->Equals(SourceFilename))
            {
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
MEntity* MPrefab::Instantiate()[]
{
    // clone all of the entities in the prefab
    String* strPrefabName = MFramework::Instance->Scene->
        GetUniqueEntityName(String::Concat(ShortName, " 01"));

    // Create a root
    MEntity* pmPrefabRoot = CreateRoot(strPrefabName);

    List<MEntity*>* pmNewEntities = new List<MEntity*>();

    pmNewEntities->Add(pmPrefabRoot);

    MEntity* entities[] = m_pmScene->GetEntities();

    for (int i = 0; i != entities->Count; ++i)
    {
        MEntity* entity = entities[i];

        // If there is no prefab root then this is a top-level
        // entity and should be cloned
        if (entity->PrefabRoot == NULL)
        {
            MEntity* amClones[] = entity->Clone(entity->Name, true);
            amClones[0]->PrefabRoot = pmPrefabRoot;

            // check all clones entities for references
            for (int j = 0; j != amClones->Count; ++j)
            {
                MEntity* pmClone = amClones[j];
                pmNewEntities->Add(pmClone);

                // Update the source entity to point to the 
                // original prefab entity
                if (pmClone->HasProperty(
                    NiTransformationComponent::PROP_SOURCE_ENTITY()))
                {
                    System::Object* val = pmClone->GetPropertyData(
                        NiTransformationComponent::PROP_SOURCE_ENTITY(), 0);
                    if (val == NULL)
                    {
                        NiEntityInterface* pkEntity = 
                            pmClone->GetNiEntityInterface();
                        pmClone->SetPropertyData(
                            NiTransformationComponent::PROP_SOURCE_ENTITY(), 
                            pmPrefabRoot, 0, false);
                        // we need to make the inherit transform properties
                        // true and unique
                        bool isUnique;
                        pkEntity->MakePropertyUnique(NiTransformationComponent
                            ::PROP_INHERIT_TRANSLATION(), isUnique);
                        pkEntity->SetPropertyData(NiTransformationComponent
                            ::PROP_INHERIT_TRANSLATION(), true, 0);
                        pkEntity->MakePropertyUnique(
                            NiTransformationComponent::PROP_INHERIT_ROTATION(),
                            isUnique);
                        pkEntity->SetPropertyData(
                            NiTransformationComponent::PROP_INHERIT_ROTATION(),
                            true, 0);
                        pkEntity->MakePropertyUnique(
                            NiTransformationComponent::PROP_INHERIT_SCALE(),
                            isUnique);
                        pkEntity->SetPropertyData(
                            NiTransformationComponent::PROP_INHERIT_SCALE(),
                            true, 0);
                    }
                }
                pmClone->Update(MFramework::Instance->TimeManager->
                    CurrentTime, MFramework::Instance->ExternalAssetManager);
            }
        }
    }
    return pmNewEntities->ToArray();
}
//---------------------------------------------------------------------------
bool MPrefab::get_Dirty()
{
    MVerifyValidInstance;

    // Search all entities in the scene for dirty flags.
    bool bDirty = m_pmScene->Dirty;
    if (!bDirty)
    {
        MEntity* amEntities[] = m_pmScene->GetEntities();
        for (int i = 0; i < amEntities->Length; ++i)
        {
            if (amEntities[i]->Dirty)
            {
                bDirty = true;
                break;
            }
        }
    }

    return bDirty;
}
//---------------------------------------------------------------------------
void MPrefab::set_Dirty(bool bDirty)
{
    MVerifyValidInstance;

    m_pmScene->Dirty = bDirty;
}
//---------------------------------------------------------------------------
bool MPrefab::get_Writable()
{
    MVerifyValidInstance;

    FileInfo* pInfo = new FileInfo(m_pmScene->SourceFilename);
    if (pInfo->Exists)
        return !pInfo->IsReadOnly;

    return true;;
}
//---------------------------------------------------------------------------
MEntity* MPrefab::CreateRoot(String* strPrefabName)
{
    MEntity* pmPrefabRoot = MEntity::CreateGeneralEntity(strPrefabName);

    const char* pcPrefabName = MStringToCharPointer(SourceFilename);
    NiPrefabComponent* pmPrefComponent = NiNew NiPrefabComponent(pcPrefabName);
    MFreeCharPointer(pcPrefabName);

    pmPrefabRoot->GetNiEntityInterface()->
        AddComponent(pmPrefComponent, true);
    pmPrefabRoot->GetNiEntityInterface()->
        AddComponent(NiNew NiTransformationComponent);
    return pmPrefabRoot;
}
//---------------------------------------------------------------------------
bool MPrefab::Save()
{
    MVerifyValidInstance;

    if (!m_pmScene || !m_pmScene->GetNiScene())
        return false;

    String* strFilename = m_pmScene->get_SourceFilename();
    String* strFormat = MUtility::GetFormatFromFilename(strFilename);

    FileInfo* file = new FileInfo(strFilename);
    if (file->Exists)
    {
        if (file->IsReadOnly)
        {
            return false;
        }
        else if (!Dirty)
        {
            return true;
        }
    }

    // Create the appropriate file format handler based on strFormat
    const char* pcFormat = MStringToCharPointer(strFormat);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    MFreeCharPointer(pcFormat);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

    pkEntityStreaming->RemoveAllScenes();
    pkEntityStreaming->InsertScene(m_pmScene->GetNiScene());
    const char* pcFilename = MStringToCharPointer(strFilename);
    bool bSuccess = NIBOOL_IS_TRUE(pkEntityStreaming->Save(pcFilename,
        pkEntityStreaming->GetSceneCount() - 1));
    MFreeCharPointer(pcFilename);

    if (bSuccess)
    {
        m_pmScene->Dirty = false;

        // If save was successful, delete any existing autosave file.
        String* strAutoSaveFilename = String::Concat(strFilename, ".autosave");
        FileInfo* file = new FileInfo(strAutoSaveFilename);
        if (file->Exists)
        {
            file->IsReadOnly = false;
            file->Delete();
            MEventManager::Instance->RaiseDeletedAutoSave(strFilename);
        }

        // Also delete any existing backup file.
        file = new FileInfo(String::Concat(strFilename, ".backup"));
        if (file->Exists)
        {
            file->IsReadOnly = false;
            file->Delete();
        }
    }

    pkEntityStreaming->RemoveAllScenes(); // Because static instance

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

    return bSuccess;
}
//---------------------------------------------------------------------------
