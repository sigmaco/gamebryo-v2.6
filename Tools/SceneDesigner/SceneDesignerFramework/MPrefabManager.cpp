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

#include "MPrefabManager.h"
#include "MPaletteManager.h"
#include "MEventManager.h"
#include "MFramework.h"
#include "MUtility.h"
#include "MPrefab.h"
#include "MLayer.h"
#include "MEntityCloneHelper.h"
#include "MSetMasterEntityCommand.h"
#include "MSetPrefabRootCommand.h"
#include "MMakePropertyUniqueCommand.h"
#include "NiMakePropertyUniqueCommand.h"

using namespace System::IO;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MPrefabManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MPrefabManager();
    }
}
//---------------------------------------------------------------------------
void MPrefabManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MPrefabManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MPrefabManager* MPrefabManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MPrefabManager::MPrefabManager()
    : m_mPrefabs(new Prefabs)
    , m_pmActive(NULL)
    , m_bPickPrefabEntities(false)
{
    MEventManager* evmgr = MEventManager::Instance;
    __hook(&MEventManager::EntitiesMovedToNewLayer, evmgr, 
        &MPrefabManager::OnEntitiesMovedToNewLayer);
    __hook(&MEventManager::EntityRemovedFromScene, evmgr,
        &MPrefabManager::OnEntityRemovedFromScene);
    __hook(&MEventManager::SceneClosing, evmgr,
        &MPrefabManager::OnSceneClosing);
}
//---------------------------------------------------------------------------
void MPrefabManager::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        MEventManager* evmgr = MEventManager::Instance;
        __unhook(&MEventManager::EntitiesMovedToNewLayer, evmgr,
            &MPrefabManager::OnEntitiesMovedToNewLayer);
        __unhook(&MEventManager::EntityRemovedFromScene, evmgr, 
            &MPrefabManager::OnEntityRemovedFromScene);
        __unhook(&MEventManager::SceneClosing, evmgr,
        &MPrefabManager::OnSceneClosing);
    }
}
//---------------------------------------------------------------------------
void MPrefabManager::RemoveAllPrefabs()
{
    MVerifyValidInstance;

    Prefabs::Enumerator mEnum = m_mPrefabs->GetEnumerator();
    while (mEnum.MoveNext())
    {
        mEnum.Current->Dispose();
    }
    m_mPrefabs->Clear();
}
//---------------------------------------------------------------------------
MPrefab* MPrefabManager::LoadPrefab(String* filename)
{
    MVerifyValidInstance;

    FileInfo* pmFileInfo = new FileInfo(filename);
    String* strFileName = pmFileInfo->Name;
    String* strPaletteName = 
        Path::GetFileNameWithoutExtension(strFileName);

    String* format = MUtility::GetFormatFromFilename(filename);
    const char* pcFormat = MStringToCharPointer(format);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    MFreeCharPointer(pcFormat);

    if (pkEntityStreaming == NULL)
    {
        // we couldn't find a streaming object for the format
        return NULL;
    }
    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

    // Load the scene
    const char* pcFilename = MStringToCharPointer(filename);
    bool bSuccess = NIBOOL_IS_TRUE(pkEntityStreaming->Load(pcFilename, false, 
        true));

    // Report errors.
    MUtility::AddErrorInterfaceMessages(spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

    if (!bSuccess)
        return NULL;
    
    if (pkEntityStreaming->GetSceneCount() != 1)
    {
        MessageService->AddMessage(MessageChannelType::Errors, new Emergent
            ::Gamebryo::SceneDesigner::PluginAPI::Message(String::Format(
            "\"{0}\" not loaded as a prefab.", filename), "Prefab files must "
            "contain one and only one layer.", NULL));
        return NULL;
    }

    NiScene* pkScene = pkEntityStreaming->GetSceneAt(0);
    const unsigned int uiEntityCount = pkScene->GetEntityCount();
    NiFixedString kTerrainPropName("Terrain Archive");
    for (unsigned int ui = 0; ui < uiEntityCount; ++ui)
    {
        NiEntityInterface* pkEntity = pkScene->GetEntityAt(ui);
        NiFixedString kDisplayName;
        if (pkEntity->GetDisplayName(kTerrainPropName, kDisplayName))
        {
            MessageService->AddMessage(MessageChannelType::Errors, new Emergent
                ::Gamebryo::SceneDesigner::PluginAPI::Message(String::Format(
                "\"{0}\" not loaded as a prefab.", filename), "Prefab files "
                "cannot contain terrain entities.", NULL));
            return NULL;
        }
    }

    MScene* pmScene = MSceneFactory::Instance->Get(pkScene);

    pmScene->Name = strPaletteName;

    MPrefab* pmPrefab = new MPrefab(pmScene);
    
    // Mark prefab scene and entities as clean after being converted.
    pmPrefab->Scene->Dirty = false;

    // If the prefab file was converted from an old file or modified at 
    // load time, mark it as dirty.
    if (pkEntityStreaming->HasBeenConverted(pmScene->GetNiScene()) ||
        pkEntityStreaming->HasBeenModified(pmScene->GetNiScene()))
    {
        pmPrefab->Scene->Dirty = true;
    }

    //pkEntityStreaming->RemoveLoadedScene(pcFilename);
    MFreeCharPointer(pcFilename);
    pkEntityStreaming->RemoveAllScenes(); // Because static instance

    AddPrefab(pmPrefab);

    return pmPrefab;
}
//---------------------------------------------------------------------------
bool MPrefabManager::AddPrefab(MPrefab* pmPrefab)
{
    MVerifyValidInstance;

    if (m_mPrefabs->Contains(pmPrefab))
    {
        return false;
    }

    m_mPrefabs->Add(pmPrefab);
    MEventManager::Instance->RaisePrefabAdded(pmPrefab);

    return true;
}
//---------------------------------------------------------------------------
String* MPrefabManager::PrepPath(String* path)
{
    MVerifyValidInstance;
    path = path->Replace('/', '\\');
    //contains the category entries for all entities in palette
    String* root = Path::Combine(PrefabFolder, path)->Replace('/', '\\');
    if (!root->EndsWith("\\"))
    {
        root = String::Concat(root, "\\");
    }
    return root;
}
//---------------------------------------------------------------------------
String* MPrefabManager::BuildSubcategories(String* path)[]
{
    MVerifyValidInstance;
    path = PrepPath(path);
    List<String*>* categories = new List<String*>();
    Prefabs::Enumerator mEnum = m_mPrefabs->GetEnumerator();
    while (mEnum.MoveNext())
    {
        MPrefab* pmPrefab = mEnum.Current;
        String* dir = pmPrefab->Path;
        // Strip off the root folder
        if (dir->StartsWith(path) && dir->Length > path->Length)
        {
            int sep = dir->IndexOf('\\', path->Length + 1);
            NIASSERT(sep != -1);
            dir = dir->Substring(path->Length, sep - path->Length);
            if (!categories->Contains(dir))
            {
                categories->Add(dir);
            }
        }
    }
    return categories->ToArray();
}
//---------------------------------------------------------------------------
MPrefab* MPrefabManager::GetPrefabsInCategory(String* category)[]
{
    MVerifyValidInstance;
    category = PrepPath(category);

    //contains the category entries for all entities in palette
    List<MPrefab*>* prefabs = new List<MPrefab*>();
    Prefabs::Enumerator mEnum = m_mPrefabs->GetEnumerator();
    while (mEnum.MoveNext())
    {
        MPrefab* pmPrefab = mEnum.Current;
        String* dir = pmPrefab->Path;
        if (dir->StartsWith(category) && dir->Length == category->Length)
        {
            prefabs->Add(pmPrefab);
        }
    }
    return prefabs->ToArray();
}
//---------------------------------------------------------------------------
MPrefab* MPrefabManager::GetPrefabByName(String* category, String* prefab)
{
    MVerifyValidInstance;
    String* fullpath = String::Concat(PrepPath(category), prefab);
    return GetPrefabByFilename(fullpath);
}
//---------------------------------------------------------------------------
MPrefab* MPrefabManager::GetPrefabByFilename(String* fullpath)
{
    MVerifyValidInstance;

    fullpath = fullpath->Replace('/', '\\');
    Prefabs::Enumerator mEnum = m_mPrefabs->GetEnumerator();
    while (mEnum.MoveNext())
    {
        String* currentname = mEnum.Current->SourceFilename;
        if (currentname->Equals(fullpath))
        {
            return mEnum.Current;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
String* MPrefabManager::get_PrefabFolder()
{
    MVerifyValidInstance;
    return MPaletteManager::Instance->PaletteFolder;
}
//---------------------------------------------------------------------------
MPrefab* MPrefabManager::get_ActivePrefab()
{
    MVerifyValidInstance;
    return m_pmActive;
}
//---------------------------------------------------------------------------
void MPrefabManager::set_ActivePrefab(MPrefab* newCurrent)
{
    MVerifyValidInstance;
    m_pmActive = newCurrent;
}
//---------------------------------------------------------------------------
bool MPrefabManager::get_PickPrefabEntities()
{
    MVerifyValidInstance;

    return m_bPickPrefabEntities;
}
//---------------------------------------------------------------------------
void MPrefabManager::set_PickPrefabEntities(bool bPickPrefabEntities)
{
    MVerifyValidInstance;

    m_bPickPrefabEntities = bPickPrefabEntities;
}
//---------------------------------------------------------------------------
void MPrefabManager::OnEntitiesMovedToNewLayer(MLayer*, 
    MLayer* pmDstLayer, MEntity* amEntities[])
{
    MVerifyValidInstance;

    for (int i = 0; i != amEntities->Length; ++i)
    {
        if (amEntities[i]->IsPrefabRoot)
        {
            pmDstLayer->MoveEntitiesToLayer(amEntities[i]->
                GetPrefabEntities(), false);
        }
    }
}
//---------------------------------------------------------------------------
void MPrefabManager::OnEntityRemovedFromScene(MScene* pmScene, 
                                              MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmScene == MFramework::Instance->Scene)
    {
        // handle deleting prefabs
        MEntity* pmPrefabEntities[] = pmEntity->GetPrefabEntities();
        for (int i = 0; i != pmPrefabEntities->Length; ++i)
        {
            pmPrefabEntities[i]->PrefabRoot = NULL;
            pmScene->RemoveEntity(pmPrefabEntities[i], true);
        }
    }
}
//---------------------------------------------------------------------------
void MPrefabManager::OnSceneClosing(MScene*)
{
    MVerifyValidInstance;

    RemoveAllPrefabs();
}
//---------------------------------------------------------------------------
namespace
{
    void SaveScene(MScene* pmScene)
    {
        String* strFilename = pmScene->SourceFilename;
        String* strFormat = MUtility::GetFormatFromFilename(strFilename);

        // Create the appropriate file format handler based on strFormat
        const char* pcFormat = MStringToCharPointer(strFormat);
        NiEntityStreaming* pkEntityStreaming =
            NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
        MFreeCharPointer(pcFormat);

        // Create error handler.
        NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
        pkEntityStreaming->SetErrorHandler(spErrors);

        pkEntityStreaming->RemoveAllScenes();
        pkEntityStreaming->InsertScene(pmScene->GetNiScene());
        const char* pcFilename = MStringToCharPointer(strFilename);
        bool bSuccess = NIBOOL_IS_TRUE(pkEntityStreaming->Save(pcFilename,
            pkEntityStreaming->GetSceneCount() - 1));
        MFreeCharPointer(pcFilename);

        if (bSuccess)
        {
            pmScene->Dirty = false;
        }

        pkEntityStreaming->RemoveAllScenes(); // Because static instance

        // Report errors.
        MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
        pkEntityStreaming->SetErrorHandler(NULL);

    }
}
//---------------------------------------------------------------------------
MPrefab* MPrefabManager::CreatePrefabFromSelection(String* strFilename, 
    bool bSubstitute)
{
    MEntity* amEntities[] = SelectionService->GetSelectedEntities();
    MPoint3* pmCenter = SelectionService->SelectionCenter;

    // Create a scene for the prefab
    const char* pcName = MStringToCharPointer(strFilename);
    NiScene* pkScene = NiNew NiScene("Main Scene");
    pkScene->SetSourceFilename(pcName);
    MFreeCharPointer(pcName);
    MScene* pmScene = MSceneFactory::Instance->Get(pkScene);
    pmScene->Update(0.0, MFramework::Instance->ExternalAssetManager);

    MEntityCloneHelper* cloner = new MEntityCloneHelper;

    // Clone the entities
    for (int i = 0; i < amEntities->Length; ++i)
    {
        MEntity* pmEntity = amEntities[i];
        cloner->Clone(pmEntity, pmEntity->Name, false);
    }

    // Remap references between clones
    // i.e. fixup internal pointers
    cloner->Fixup(true);

    // recenter cloned entities (subtract out selection center) and
    // add them to the prefab's scene
    MEntity* amClones[] = cloner->GetEntities();
    for (int j = 0; j != amClones->Count; ++j)
    {
        MEntity* pmClone = amClones[j];
        pmScene->AddEntity(pmClone, false);

        // Recenter cloned entities
        if (pmClone->PrefabRoot == NULL &&
            pmClone->HasProperty(ms_strTranslationName) &&
            pmClone->GetPropertyData(
                NiTransformationComponent::PROP_SOURCE_ENTITY()) == NULL)
        {
            MPoint3* pmEntityTranslation = dynamic_cast<MPoint3*>(pmClone->
                GetPropertyData(ms_strTranslationName));
            pmEntityTranslation = 
                MPoint3::Subtract(pmEntityTranslation, pmCenter);
            pmClone->SetPropertyData(ms_strTranslationName, 
                pmEntityTranslation, false);
        }
    }

    // Save prefab to disk and add it to prefab manager
    SaveScene(pmScene);

    MPrefab* pmPrefab = new MPrefab(pmScene);
    AddPrefab(pmPrefab);

    // Now, convert the selected entities into an instance of the new prefab
    if (bSubstitute)
    {
        CommandService->BeginUndoFrame(String::Format(
            "Replacing entities with \"{0}\" prefab", pmPrefab->ShortName));

        // Create the prefab root
        MEntity* pmPrefabRoot = pmPrefab->CreateRoot(String::Concat(
            pmPrefab->ShortName, " 01"));
        // Change the selection to point to the new entity
        MEntity* amSelection[] = new MEntity*[1];
        // Set the translation for the root
        pmPrefabRoot->SetPropertyData(ms_strTranslationName, pmCenter, true);

        // Force the master entity on the original to point to it's clone
        // in the prefab.
        MSetMasterEntityCommand* cmdSetMaster = new MSetMasterEntityCommand();

        // Set the prefab root on the original entities to point to our
        // new root so that it looks like they were instantiated
        MSetPrefabRootCommand* cmdSetPrefabRoot = new MSetPrefabRootCommand();

        for (int i = 0; i < amEntities->Length; ++i)
        {
            MEntity* pmEntity = amEntities[i];
            cmdSetPrefabRoot->Add(pmEntity, pmPrefabRoot);
            cmdSetMaster->Add(pmEntity, cloner->Lookup(pmEntity));

            // Since the new master entities are clones of the existing
            // entities, their properties are all the same. Reset all
            // properties on the entities in the scene so that they are
            // inherited from the new master.
            String* amPropertyNames[] = pmEntity->GetPropertyNames();
            for (int i = 0; i < amPropertyNames->Length; ++i)
            {
                if (pmEntity->CanResetProperty(amPropertyNames[i]))
                {
                    pmEntity->ResetProperty(amPropertyNames[i]);
                }
            }

            // Set source entity properties on original entities to root
            // and force it's inherit transformation properties to be unique
            // and true.
            // Update the source entity to point to the original prefab entity
            if (pmEntity->HasProperty(
                NiTransformationComponent::PROP_SOURCE_ENTITY()))
            {
                Object* val = pmEntity->GetPropertyData(
                    NiTransformationComponent::PROP_SOURCE_ENTITY(), 0);
                if (val == NULL)
                {
                    NiEntityInterface* pkEntity =
                        pmEntity->GetNiEntityInterface();
                    pmEntity->SetPropertyData(
                        NiTransformationComponent::PROP_SOURCE_ENTITY(), 
                        pmPrefabRoot, true);
                    // we need to make the inherit transform properties
                    // true and unique
                    CommandService->ExecuteCommand(
                        new MMakePropertyUniqueCommand(
                        NiNew NiMakePropertyUniqueCommand(pkEntity, 
                        NiTransformationComponent::PROP_INHERIT_TRANSLATION()),
                        pmEntity), true);
                    CommandService->ExecuteCommand(
                        new MMakePropertyUniqueCommand(
                        NiNew NiMakePropertyUniqueCommand(pkEntity, 
                        NiTransformationComponent::PROP_INHERIT_ROTATION()),
                        pmEntity), true);
                    CommandService->ExecuteCommand(
                        new MMakePropertyUniqueCommand(
                        NiNew NiMakePropertyUniqueCommand(pkEntity, 
                        NiTransformationComponent::PROP_INHERIT_SCALE()),
                        pmEntity), true);
                    pmEntity->SetPropertyData(
                        NiTransformationComponent::PROP_INHERIT_TRANSLATION(),
                        __box(true), true);
                    pmEntity->SetPropertyData(
                        NiTransformationComponent::PROP_INHERIT_ROTATION(),
                        __box(true), true);
                    pmEntity->SetPropertyData(
                        NiTransformationComponent::PROP_INHERIT_SCALE(),
                        __box(true), true);
                }
            }
        }
        CommandService->ExecuteCommand(cmdSetPrefabRoot, true);
        CommandService->ExecuteCommand(cmdSetMaster, true);

        // Add the prefab root to the scene
        MFramework::Instance->Scene->AddEntity(pmPrefabRoot, true);
        amSelection[0] = pmPrefabRoot;
        SelectionService->ReplaceSelection(amSelection);

        CommandService->EndUndoFrame(true);
    }

    cloner->Dispose();

    return pmPrefab;
}
//---------------------------------------------------------------------------
ICommandService* MPrefabManager::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
ISelectionService* MPrefabManager::get_SelectionService()
{
    if (ms_pmSelectionService == NULL)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service not found!");
    }
    return ms_pmSelectionService;
}
//---------------------------------------------------------------------------
MPrefab* MPrefabManager::GetPrefabs()[]
{
    MVerifyValidInstance;

    return m_mPrefabs->ToArray();
}
//---------------------------------------------------------------------------
IMessageService* MPrefabManager::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message Service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
