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

#include "MLayer.h"
#include "MLayerManager.h"
#include "MFramework.h"
#include "MEventManager.h"
#include "MPrefabManager.h"
#include "MLayerManager.h"
#include "ICommandService.h"
#include "MUtility.h"
#include "MMoveEntitiesToLayerCommand.h"
#include "MChangeLayerStateCommand.h"
#include "MRemoveLayerFilenameCommand.h"
#include "MAddLayerCommand.h"

#include "NiEntity.h"

using namespace System::IO;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MLayer::MLayer(MScene* pmScene, NiScene* uberScene)
: m_sentinel(new FileSystemWatcher)
, m_scene(pmScene)
, m_closing(false)
, m_writable(true)
, m_bVisible(true)
, m_bLocked(false)
, m_overrideCount(0)
{
    CreateFileWatcher();

    // we need to mirror our entities into the uber scene but we
    // want to bypass the MEntity/MScene mechanism because we don't
    // want to issue callbacks.
    const unsigned numEntities = pmScene->EntityCount;

    pmScene->Layer = this;
    // When we add the entities to the uber scene it will change the source
    // filename to be the same as it's name. Temporarily set the name of the
    // scene to the same as our layer's scene and then set it back later
    NiScene* pkScene = pmScene->GetNiScene();
    uberScene->SetSourceFilename(pkScene->GetSourceFilename(), false);
    for (unsigned int ui = 0; ui < numEntities; ++ui)
    {
        NiEntityInterface* pkEntity = pkScene->GetEntityAt(ui);
        uberScene->AddEntity(pkEntity);
        MEntity* pmEntity = MEntityFactory::Instance->Get(pkEntity);
        pmEntity->Layer = this;
    }
    uberScene->SetSourceFilename("", false);
    MFramework* fw = MFramework::Instance;
    if (fw)
    {
        m_scene->Update(0.0, fw->ExternalAssetManager);
    }
}
//---------------------------------------------------------------------------
MLayer::MLayer(String* strName)
: m_sentinel(new FileSystemWatcher)
, m_scene(0)
, m_closing(false)
, m_bVisible(true)
, m_bLocked(false)
{
    const char* pcName = MStringToCharPointer(strName);
    NiScene* pkScene = NiNew NiScene("");
    pkScene->SetSourceFilename(pcName);
    MFreeCharPointer(pcName);
    m_scene = MSceneFactory::Instance->Get(pkScene);
    m_scene->Layer = this;
    m_scene->Update(0.0, MFramework::Instance->ExternalAssetManager);

    CreateFileWatcher();
}
//---------------------------------------------------------------------------
namespace
{
    bool DeleteIfExists(String* filename)
    {
        FileInfo* fi = new FileInfo(filename);
        if (fi->Exists)
        {
            fi->IsReadOnly = false;
            fi->Delete();
            return true;
        }
        return false;
    }
//---------------------------------------------------------------------------

    // Delete existing autosave files for all layers. Copy
    // backup files to main files, if they exist, and delete
    // them. This leaves the main files in their original
    // state, even if they were restored via an autosave.
    void CleanupAutosaveFiles(String* filename)
    {
        // Delete autosave files.
        FileInfo* fi = new FileInfo(String::Concat(filename, ".autosave"));
        if (fi->Exists)
        {
            fi->IsReadOnly = false;
            fi->Delete();
            MEventManager::Instance->RaiseDeletedAutoSave(filename);
        }
        fi = new FileInfo(
            String::Concat(filename, ".scene.settings.autosave"));
        if (fi->Exists)
        {
            fi->IsReadOnly = false;
            fi->Delete();
        }

        // Restore backup files.
        fi = new FileInfo(String::Concat(filename, ".backup"));
        if (fi->Exists)
        {
            FileInfo* fiOrig = new FileInfo(filename);
            if (fiOrig->Exists)
            {
                fiOrig->IsReadOnly = false;
            }
            fi->IsReadOnly = false;
            fi->CopyTo(filename, true);
            fi->Delete();
        }
        fi = new FileInfo(String::Concat(filename, ".scene.settings.backup"));
        if (fi->Exists)
        {
            FileInfo* fiOrig = 
                new FileInfo(String::Concat(filename, ".scene.settings"));
            if (fiOrig->Exists)
            {
                fiOrig->IsReadOnly = false;
            }
            fi->IsReadOnly = false;
            fi->CopyTo(String::Concat(filename, ".scene.settings"), true);
            fi->Delete();
        }
    }
}
//---------------------------------------------------------------------------
void MLayer::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        m_closing = true;
        // don't watch the file anymore.
        m_sentinel->EnableRaisingEvents = false;
        m_sentinel->Changed -= new 
            FileSystemEventHandler(this, &MLayer::OnFileStatusChanged);

        CleanupAutosaveFiles(Name);
        
        m_sentinel = NULL;

        if (m_scene)
        {
            MScene* pmScene = MSceneFactory::Instance->Get(
                m_scene->GetNiScene());
            MAssert(pmScene == m_scene, "Layer scene does not match "
                "scene returned from factory!");

            // Don't remove it if it is an active layer
            if (!MPrefabManager::Instance->GetPrefabByFilename(Name))
            {
                MSceneFactory::Instance->Remove(pmScene);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MLayer::RemoveEntitiesFromMainScene(bool undoable)
{
    MVerifyValidInstance;

    if (m_scene)
    {
        MFramework::Instance->BeginLongOperation();

        // we are closing this layer so to prevent wierd problems with
        // removing entities let's pretend that this layer is writable.
        m_writable = true;

        m_closing = true;
        const unsigned int TotalEntities = MFramework::Instance->Scene->EntityCount;
        // iterate over the entities and remove them from the 
        // primary scene
        const unsigned int n = m_scene->EntityCount;
        for (unsigned int ui = 0; ui < n; ++ui)
        {
            MEntity* entity = m_scene->GetEntities()[ui];
            MFramework::Instance->Scene->RemoveEntity(entity, undoable);
        }
        NIASSERT(TotalEntities - n == MFramework::Instance->Scene->EntityCount);

        MFramework::Instance->EndLongOperation();
    }
}
//---------------------------------------------------------------------------
bool MLayer::Save(bool bAutoSave)
{
    MVerifyValidInstance;

    return Do_Save(Name, bAutoSave);
}
//---------------------------------------------------------------------------
bool MLayer::Do_Save(String* strFilename, bool bAutoSave)
{
    MVerifyValidInstance;

    if (!m_scene || !m_scene->GetNiScene())
        return false;

    String* oldFilename = Name;
    String* strFormat = MUtility::GetFormatFromFilename(strFilename);

    MAssert(strFilename->Equals(Name), "Changing the name of the file "
       "should be done through the Rename function.");

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

    MEventManager::Instance->RaiseLayerSaving(this, bAutoSave);

    // Create autosave filename.
    String* strAutoSaveFilename = String::Concat(strFilename, ".autosave");

    // If autosaving, use autosave filename.
    if (bAutoSave)
    {
        strFilename = strAutoSaveFilename;
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
    pkEntityStreaming->InsertScene(m_scene->GetNiScene());
    const char* pcFilename = MStringToCharPointer(strFilename);
    bool bSuccess = NIBOOL_IS_TRUE(pkEntityStreaming->Save(pcFilename,
        pkEntityStreaming->GetSceneCount() - 1));
    MFreeCharPointer(pcFilename);

    if (bAutoSave)
    {
        // If autosaving, reset the source filename property for the saved
        // scene to the original filename.
        const char* pcOriginalFilename = MStringToCharPointer(oldFilename);
        m_scene->GetNiScene()->SetSourceFilename(pcOriginalFilename);
        MFreeCharPointer(pcOriginalFilename);
    }

    if (bSuccess && !bAutoSave)
    {
        m_scene->Dirty = false;

        // If save was successful, delete any existing autosave file.
        // Also delete any existing backup file.
        if (DeleteIfExists(String::Concat(strFilename, ".autosave")))
        {
            MEventManager::Instance->RaiseDeletedAutoSave(strFilename);
        }
        DeleteIfExists(String::Concat(strFilename, 
            ".scene.settings.autosave"));
        DeleteIfExists(String::Concat(strFilename, ".backup"));
        DeleteIfExists(String::Concat(strFilename, ".scene.settings.backup"));
    }

    pkEntityStreaming->RemoveAllScenes(); // Because static instance

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

    // Raise a status changed event because our dirty flag has changed
    // and our filename may have changed.
    if (!bAutoSave)
    {
        MEventManager::Instance->RaiseLayerStatusChanged(this);
    }

    MEventManager::Instance->RaiseLayerSaved(this, bAutoSave);

    return bSuccess;
}
//---------------------------------------------------------------------------
void MLayer::SelectAllEntities()
{
    MVerifyValidInstance;

    List<MEntity*>* pmEntities = new List<MEntity*>();
    MEntity* amEntities[] = m_scene->GetEntities();
    for (int i = 0; i != amEntities->Length; ++i)
    {
        if (amEntities[i]->PrefabRoot == NULL)
        {
            pmEntities->Add(amEntities[i]);
        }
    }

    SelectionService->ReplaceSelection(pmEntities->ToArray());
}
//---------------------------------------------------------------------------
void MLayer::MoveEntitiesToLayer(MEntity* amEntities[])
{
    MoveEntitiesToLayer(amEntities, true);
}
//---------------------------------------------------------------------------
void MLayer::MoveEntitiesToLayer(MEntity* amEntities[], 
                                 bool filterPrefabEntities)
{
    MVerifyValidInstance;

    MMoveEntitiesToLayerCommand* cmd = new MMoveEntitiesToLayerCommand(this);

    List<MEntity*>* badEntities = new List<MEntity*>();
    for (int i = 0; i < amEntities->Count; i++)
    {
        if (amEntities[i]->PrefabRoot == NULL ||!filterPrefabEntities)
        {
            cmd->AddEntity(amEntities[i]);
        } else
        {
            badEntities->Add(amEntities[i]);
        }
    }

    if (amEntities->Length > badEntities->Count)
    {
        CommandService->ExecuteCommand(cmd, true);
    }
    if (badEntities->Count > 0)
    {
        String* details = "The affected entities were \n";
        List<MEntity*>::Enumerator en = badEntities->GetEnumerator();
        while (en.MoveNext())
        {
            details = String::Format("{0}\t{1}\n", details, en.Current->Name);
        }
        MessageService->AddMessage(MessageChannelType::General,
            new Emergent::Gamebryo::SceneDesigner::PluginAPI::Message(
            "Could not move some entities that were part of prefab entities",
            details, this));
    }
}
//---------------------------------------------------------------------------
void MLayer::MoveSelectedEntities()
{
    MVerifyValidInstance;

    MEntity* amEntities[] = SelectionService->GetSelectedEntities();
    MoveEntitiesToLayer(amEntities);
}
//---------------------------------------------------------------------------
String* MLayer::get_Name()
{
    MVerifyValidInstance;

    // The group stores it's name locally but the layers
    // actually use their scene's sourcefilename.
    return m_scene->SourceFilename;
}
//---------------------------------------------------------------------------
void MLayer::set_Name(String* strFilename)
{
    Rename(strFilename, false);
}
//---------------------------------------------------------------------------
void MLayer::Rename(String* strFilename, bool bDeleteOldFile)
{
    NIASSERT(m_scene);
    NIASSERT(m_scene->GetNiScene());
    if (strFilename != Name)
    {
        String* oldFilename = Name;

        MLayer* referees[] = MLayerManager::Instance->
            FindReferencingLayers(this);
        for (int i = 0; i != referees->Count; ++i)
        {
            referees[i]->m_scene->
                RenameLayerFilename(oldFilename, strFilename);
            referees[i]->Dirty = true;
        }
        m_scene->SourceFilename = strFilename;
        CreateFileWatcher();

        if (bDeleteOldFile && !String::IsNullOrEmpty(oldFilename))
        {
            DeleteIfExists(oldFilename);
            DeleteIfExists(String::Concat(oldFilename, ".scene.settings"));
        }
        if (DeleteIfExists(String::Concat(oldFilename, ".autosave")))
        {
            MEventManager::Instance->RaiseDeletedAutoSave(oldFilename);
        }
        DeleteIfExists(String::Concat(oldFilename, ".scene.settings.autosave"));
        DeleteIfExists(String::Concat(oldFilename, ".backup"));
        DeleteIfExists(String::Concat(oldFilename, ".scene.settings.backup"));

        Dirty = true;

        MEventManager::Instance->RaiseLayerRenamed(this, oldFilename);
    }
}
//---------------------------------------------------------------------------
String* MLayer::get_ShortName()
{
    MVerifyValidInstance;

    if (IsDefaultLayer)
    {
        return "<Default>";
    }
    if (Name->Equals(String::Empty))
    {
        return "<untitled>";
    }

    String* strName = this->Name;
    return Path::GetFileName(strName);
}
//---------------------------------------------------------------------------
bool MLayer::get_IsDefaultLayer()
{
    MVerifyValidInstance;

    return MLayerManager::Instance->DefaultLayer == this;
}
//---------------------------------------------------------------------------
bool MLayer::get_Writable()
{
    MVerifyValidInstance;

    return (m_overrideCount > 0 || m_writable);
}
//---------------------------------------------------------------------------
void MLayer::set_Writable(bool writable)
{
    MVerifyValidInstance;

    MAssert(!String::IsNullOrEmpty(Name), "SourceFilename is NULL");

    FileInfo* info = new FileInfo(Name);
    info->IsReadOnly = !writable;
    if (m_writable != writable)
    {
        m_writable = writable;
        MEventManager::Instance->RaiseLayerStatusChanged(this);
    }
}
//---------------------------------------------------------------------------
bool MLayer::get_Visible()
{
    MVerifyValidInstance;

    return m_bVisible;
}
//---------------------------------------------------------------------------
void MLayer::set_Visible(bool bVisible)
{
    MVerifyValidInstance;

    ForceVisible(bVisible, true);
}
//---------------------------------------------------------------------------
void MLayer::ForceVisible(bool bVisible, bool bUndoable)
{
    MVerifyValidInstance;

    MChangeLayerStateCommand* cmd = 
        new MChangeLayerStateCommand(this, MChangeLayerStateCommand::Visible, 
            bVisible);
    CommandService->ExecuteCommand(cmd, bUndoable);
}
//---------------------------------------------------------------------------
MLayer::TriState MLayer::get_VisibleState()
{
    MVerifyValidInstance;

    if (!m_bVisible)
    {
        return TriState::False;
    }

    MEntity* amEntities[] = GetEntities();
    for (int i = 0; i < amEntities->Length; ++i)
    {
        if (amEntities[i]->LocalHidden)
        {
            return TriState::Indeterminate;
        }
    }

    return TriState::True;
}
//---------------------------------------------------------------------------
bool MLayer::get_Locked()
{
    MVerifyValidInstance;

    return m_bLocked;
}
//---------------------------------------------------------------------------
void MLayer::set_Locked(bool bLocked)
{
    MVerifyValidInstance;

    ForceLocked(bLocked, true);
}
//---------------------------------------------------------------------------
void MLayer::ForceLocked(bool bLocked, bool bUndoable)
{
    MVerifyValidInstance;

    MChangeLayerStateCommand* cmd = 
        new MChangeLayerStateCommand(this, MChangeLayerStateCommand::Locked, 
            bLocked);
    CommandService->ExecuteCommand(cmd, bUndoable);
}
//---------------------------------------------------------------------------
MLayer::TriState MLayer::get_LockedState()
{
    MVerifyValidInstance;

    if (m_bLocked)
    {
        return TriState::True;
    }

    MEntity* amEntities[] = GetEntities();
    for (int i = 0; i < amEntities->Length; ++i)
    {
        if (amEntities[i]->LocalFrozen)
        {
            return TriState::Indeterminate;
        }
    }

    return TriState::False;
}
//---------------------------------------------------------------------------
bool MLayer::get_Dirty()
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    return m_scene->Dirty;
}
//---------------------------------------------------------------------------
void MLayer::set_Dirty(bool dirty)
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    if (m_scene->Dirty != dirty)
    {
        m_scene->Dirty = dirty;
        MEventManager::Instance->RaiseLayerStatusChanged(this);
    }
}
//---------------------------------------------------------------------------
String* MLayer::get_Format()
{
    MVerifyValidInstance;

    return MUtility::GetFormatFromFilename(Name);
}
//---------------------------------------------------------------------------
unsigned int MLayer::get_SelectionSetCount()
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    return m_scene->SelectionSetCount;
}
//---------------------------------------------------------------------------
MSelectionSet* MLayer::GetSelectionSets()[]
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    return m_scene->GetSelectionSets();
}
//---------------------------------------------------------------------------
MSelectionSet* MLayer::GetSelectionSetByName(String* strName)
{
    MVerifyValidInstance;

    return m_scene->GetSelectionSetByName(strName);
}
//---------------------------------------------------------------------------
bool MLayer::AddSelectionSet(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    m_scene->AddSelectionSet(pmSelectionSet);
    return true;
}
//---------------------------------------------------------------------------
void MLayer::RemoveSelectionSet(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    m_scene->RemoveSelectionSet(pmSelectionSet);
}
//---------------------------------------------------------------------------
void MLayer::RemoveAllSelectionSets()
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    m_scene->RemoveAllSelectionSets();
}
//---------------------------------------------------------------------------
bool MLayer::IsEntityReferenced(MEntity* pmEntity)
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    MEntity* amSceneEntities[] = m_scene->GetEntities();
    for (int iSceneEntity = 0; iSceneEntity < amSceneEntities->Length;
        ++iSceneEntity)
    {
        MEntity* pmSceneEntity = amSceneEntities[iSceneEntity];
        if (pmSceneEntity == pmEntity)
        {
            return true;
        }

        String* amPropertyNames[] = pmSceneEntity->GetPropertyNames();
        for (int iPropertyName = 0; iPropertyName < amPropertyNames->Length;
            ++iPropertyName)
        {
            String* strPropertyName = amPropertyNames[iPropertyName];
            PropertyType* pmPropertyType = pmSceneEntity->GetPropertyType(
                strPropertyName);
            if (pmPropertyType != NULL &&
                pmPropertyType->ManagedType == __typeof(MEntity))
            {
                const unsigned int uiElementCount =
                    pmSceneEntity->GetElementCount(strPropertyName);
                for (unsigned int uiElement = 0; uiElement < uiElementCount;
                    ++uiElement)
                {
                    MEntity* pmEntityProperty = dynamic_cast<MEntity*>(
                        pmSceneEntity->GetPropertyData(strPropertyName,
                        uiElement));
                    if (pmEntityProperty == pmEntity)
                    {
                        return true;
                    }
                }
            }
        }
    }

    MSelectionSet* amSelectionSets[] = m_scene->GetSelectionSets();
    for (int iSelectionSet = 0; iSelectionSet < amSelectionSets->Length;
        ++iSelectionSet)
    {
        MSelectionSet* pmSelectionSet = amSelectionSets[iSelectionSet];
        MEntity* amSelectionSetEntities[] = pmSelectionSet->GetEntities();
        for (int iSelectionSetEntity = 0;
            iSelectionSetEntity < amSelectionSetEntities->Length;
            ++iSelectionSetEntity)
        {
            MEntity* pmSelectionSetEntity = amSelectionSetEntities[
                iSelectionSetEntity];
            if (pmSelectionSetEntity == pmEntity)
            {
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
unsigned MLayer::get_EntityCount()
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    return m_scene->EntityCount;
}
//---------------------------------------------------------------------------
MEntity* MLayer::GetEntities()[]
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    return m_scene->GetEntities();
}
//---------------------------------------------------------------------------
void MLayer::AddEntity(MEntity* pmEntity, bool enableCallbacks, bool)
{
    MVerifyValidInstance;

    m_scene->GetNiScene()->AddEntity(pmEntity->GetNiEntityInterface());
    pmEntity->Layer = this;
    Dirty = true;

    if (enableCallbacks)
    {
        MEventManager::Instance->RaiseLayerStatusChanged(this);
    }
}
//---------------------------------------------------------------------------
void MLayer::RemoveEntity(MEntity* pmEntity, bool enableCallbacks, bool)
{
    MVerifyValidInstance;

    if (m_closing)
    {
        return;
    }
    m_scene->GetNiScene()->RemoveEntity(pmEntity->GetNiEntityInterface());
    pmEntity->Layer = NULL;
    Dirty = true;

    if (enableCallbacks)
    {
        MEventManager::Instance->RaiseLayerStatusChanged(this);
    }
}
//---------------------------------------------------------------------------
MEntity* MLayer::FindEntityByName(String* strName)
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    return m_scene->GetEntityByName(strName);
}
//---------------------------------------------------------------------------
MLayer* MLayer::FindLayer(MEntity* pmEntity)
{
    return pmEntity->Layer;
}
//---------------------------------------------------------------------------
ICommandService* MLayer::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
ISelectionService* MLayer::get_SelectionService()
{
    if (ms_pmSelectionService == NULL)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service not found!");
    }
    return ms_pmSelectionService;
}
//---------------------------------------------------------------------------
IMessageService* MLayer::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
void MLayer::OnFileStatusChanged(Object*, FileSystemEventArgs*)
{
    MVerifyValidInstance;

    // our attributes changed, we need to notify our observers that our
    // writable status may have changed.
    assert(Name != NULL);
    assert(!Name->Equals(String::Empty));

    FileInfo* info = new FileInfo(Name);
    bool writable = !info->IsReadOnly;
    if (m_writable != writable)
    {
        m_writable = writable;
        MEventManager::Instance->RaiseLayerStatusChanged(this);

        // if the layer became read-only then we need to clear the undo
        // stack to prevent someone from undoing an operation that affects
        // this layer, but only if there are commands to undo/redo.
        if (!m_writable && CommandService->UndoStackCount > 0 || 
            CommandService->RedoStackCount > 0)
        {
            CommandService->ClearUndoAndRedoStacks();
            String* strMsg = "Clearing the undo stack because a layer "
                "became read-only.";
            String* strDetails = String::Format("The layer named \"{0}\" "
                "became read-only.", Name);

            MessageService->AddMessage(MessageChannelType::General,
                new Emergent::Gamebryo::SceneDesigner::PluginAPI::Message(
                strMsg, strDetails, this));
        }
    }
}
//---------------------------------------------------------------------------
void MLayer::RemoveSceneFromStreamingObject()
{
    MVerifyValidInstance;

    NIASSERT(m_scene);
    String* strFormat = 
        MUtility::GetFormatFromFilename(m_scene->SourceFilename);
    const char* pcFormat = MStringToCharPointer(strFormat);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    MFreeCharPointer(pcFormat);
    if (pkEntityStreaming)
    {
        pkEntityStreaming->RemoveLoadedScene(m_scene->GetNiScene()
            ->GetSourceFilename());
    }
}
//---------------------------------------------------------------------------
void MLayer::CreateFileWatcher()
{
    MVerifyValidInstance;

    // Assign a synchronizing object (the mainform) to the watcher so
    // it will marshall all events through the same thread that created
    // the UI. This prevents asynchronous access from other threads.
    if (Application::OpenForms->Count > 0)
    {
        m_sentinel->SynchronizingObject = Application::OpenForms->Item[0];
    }

    if (Name != NULL && !Name->Equals(String::Empty))
    {
        // Initialize a FileSystemWatcher to watch this file.
        FileInfo* info = new FileInfo(Name);

        m_sentinel->Path = info->DirectoryName;
        m_sentinel->Filter = info->Name;

        // Watch for changes in attributes
        m_sentinel->NotifyFilter = 
            static_cast<NotifyFilters>(NotifyFilters::Attributes);

        // Add event handlers.
        m_sentinel->Changed += new 
            FileSystemEventHandler(this, &MLayer::OnFileStatusChanged);

        // Begin watching.
        m_sentinel->EnableRaisingEvents = true;

        // Get the writable status
        m_writable = !(info->Exists && info->IsReadOnly);
    } else
    {
        // the default layer on a new file doesn't have a filename
        // make it writable anyway.
        m_writable = true;
    }
}
//---------------------------------------------------------------------------
void MLayer::OverrideWritableFlag()
{
    MVerifyValidInstance;

    m_overrideCount++;
}
//---------------------------------------------------------------------------
void MLayer::RemoveWritableFlagOverride()
{
    MVerifyValidInstance;

    MAssert(m_overrideCount > 0, "Invalid override count!");
    m_overrideCount--;
}
//---------------------------------------------------------------------------
void MLayer::DoChangeVisibleStatus(bool bVisible)
{
    m_bVisible = bVisible;
    MEventManager::Instance->RaiseLayerStatusChanged(this);

    MFramework::Instance->BeginLongOperation();
    MEntity* amEntities[] = GetEntities();
    for (int i = 0; i < amEntities->Length; ++i)
    {
        MEventManager::Instance->RaiseEntityHiddenStateChanged(
            amEntities[i], !bVisible);
    }
    MFramework::Instance->EndLongOperation();
}
//---------------------------------------------------------------------------
void MLayer::DoChangeLockedStatus(bool bLocked)
{
    m_bLocked = bLocked;
    MEventManager::Instance->RaiseLayerStatusChanged(this);

    MFramework::Instance->BeginLongOperation();
    MEntity* amEntities[] = GetEntities();
    for (int i = 0; i < amEntities->Length; ++i)
    {
        MEventManager::Instance->RaiseEntityFrozenStateChanged(
            amEntities[i], bLocked);
    }
    MFramework::Instance->EndLongOperation();
}
//---------------------------------------------------------------------------
MLayer* MLayer::GetSubLayers()[]
{
    MVerifyValidInstance;

    typedef List<MLayer*> Layers;

    Layers* sublayers = new Layers();

    MLayerManager* layerManager = MFramework::Instance->LayerManager;

    // look at all of the filenames loaded by the requested layer
    // and add their matching layers to the list
    String* filenames[] = m_scene->GetLayerFilenames();
    for (int i = 0; i != filenames->Count; ++i)
    {
        MLayer* pmLayer = layerManager->GetLayerByName(filenames[i]);
        if (pmLayer != NULL)
        {
            sublayers->Add(pmLayer);
        }
    }
    return sublayers->ToArray();
}
//---------------------------------------------------------------------------
void MLayer::BuildExplicitList(Dictionary<String*, bool>* explicitLayers)
{
    MVerifyValidInstance;

    NIASSERT(explicitLayers != NULL);
    MLayer* sublayers[] = GetSubLayers();
    for (int i = 0; i != sublayers->Count; ++i)
    {
        if (!explicitLayers->ContainsKey(sublayers[i]->Name))
        {
            explicitLayers->Item[sublayers[i]->Name] = true;
            sublayers[i]->BuildExplicitList(explicitLayers);
        }
    }
}
//---------------------------------------------------------------------------
bool MLayer::IsExplicitSublayer(MLayer* pmSubLayer)
{
    MVerifyValidInstance;

    String* name = pmSubLayer->Name;
    String* sublayers[] = m_scene->GetLayerFilenames();
    for (int i = 0; i != sublayers->Count; ++i)
    {
        if (name->Equals(sublayers[i]))
        {
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void MLayer::AddExplicitLayer(MLayer* layer)
{
    MVerifyValidInstance;

    MAddLayerCommand* cmd = new MAddLayerCommand(layer->Name, this);
    CommandService->ExecuteCommand(cmd, true);
}
//---------------------------------------------------------------------------
void MLayer::DoAddExplicitLayer(MLayer* layer)
{
    MVerifyValidInstance;

    m_scene->AddLayerFilename(layer->Name);
    
    // Raise a status changed event on this layer plus the default layer
    MEventManager::Instance->RaiseLayerStatusChanged(layer);
    MEventManager::Instance->RaiseLayerStatusChanged(this);
}
//---------------------------------------------------------------------------
void MLayer::RemoveExplicitLayer(MLayer* layer)
{
    MVerifyValidInstance;

    CommandService->ExecuteCommand(
        new MRemoveLayerFilenameCommand(layer->Name, this), true);
}
//---------------------------------------------------------------------------
void MLayer::DoRemoveExplicitLayer(MLayer* layer)
{
    MVerifyValidInstance;

    m_scene->RemoveLayerFilename(layer->Name);

    // Raise a status changed event on this layer plus the default layer
    MEventManager::Instance->RaiseLayerStatusChanged(layer);
    MEventManager::Instance->RaiseLayerStatusChanged(this);
}
//---------------------------------------------------------------------------
