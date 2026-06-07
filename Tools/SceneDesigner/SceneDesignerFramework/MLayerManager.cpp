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

#include "MLayerManager.h"
#include "MEventManager.h"
#include "MFramework.h"
#include "MAddLayerCommand.h"
#include "MSetActiveLayerCommand.h"
#include "ICommandService.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MLayerManager::MLayerManager(MEventManager* evmgr)
{
    m_mLayers = new Layers;
    m_pmActiveLayer = 0;

    __hook(&MEventManager::EntityAddedToScene, evmgr,
        &MLayerManager::OnEntityAddedToScene);
    __hook(&MEventManager::EntityRemovedFromScene, evmgr,
        &MLayerManager::OnEntityRemovedFromScene);
    __hook(&MEventManager::LayerStatusChanged, evmgr,
        &MLayerManager::OnLayerStatusChanged);
}
//---------------------------------------------------------------------------
MLayerManager::~MLayerManager()
{
}
//---------------------------------------------------------------------------
void MLayerManager::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        MEventManager* evmgr = MEventManager::Instance;

        // For some reason, __unhook causes bogus compilation errors here.
        // Thus, the events are unhooked manually.
        MEventManager::Instance->remove_EntityAddedToScene(new
            MEventManager::__Delegate_EntityAddedToScene(this,
            &MLayerManager::OnEntityAddedToScene));
        __unhook(&MEventManager::EntityRemovedFromScene, evmgr, 
            &MLayerManager::OnEntityRemovedFromScene);
        MEventManager::Instance->remove_LayerStatusChanged(new
            MEventManager::__Delegate_LayerStatusChanged(this,
            &MLayerManager::OnLayerStatusChanged));
    }
}
//---------------------------------------------------------------------------
unsigned int MLayerManager::get_Count()
{
    MVerifyValidInstance;

    return m_mLayers->Count;
}
//---------------------------------------------------------------------------
void MLayerManager::AddEmptyLayer(String* filename, bool undoable)
{
    CommandService->ExecuteCommand(new MAddLayerCommand(filename, ActiveLayer),
        undoable);
}
//---------------------------------------------------------------------------
void MLayerManager::ImportLayer(String* strFilename, String*)
{
    CommandService->ExecuteCommand(new MAddLayerCommand(strFilename, 
        ActiveLayer), true);
}
//---------------------------------------------------------------------------
void MLayerManager::AddLayerFromSelectedEntities(String* strFilename)
{
    // Start an undo frame so it all gets undone at the same time
    CommandService->BeginUndoFrame(String::Format(
        "Create new layer \"{0}\" from selected entities", strFilename));

    AddEmptyLayer(strFilename, true);

    // the layer is the last one in the list
    MLayer* newlayer = m_mLayers->Item[m_mLayers->Count - 1];
    newlayer->MoveSelectedEntities();

    CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
bool MLayerManager::AddLayer(MLayer* pmLayer, MLayer* pmParent,
                             CallbackStatus events)
{
    MVerifyValidInstance;

    MVerifyValidInstance;

    if (m_mLayers->Contains(pmLayer))
    {
        return false;
    }

    m_mLayers->Add(pmLayer);

    if (EnableEvents == events)
    {
        MEventManager::Instance->RaiseLayerAdded(pmLayer, pmParent);
    }

    if (m_pmActiveLayer == NULL && pmLayer->Writable)
    {
        DoSetActiveLayer(pmLayer, EnableEvents == events);
    }

    return true;
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::GetLayerByName(String* strFilename)
{
    MVerifyValidInstance;

    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        MLayer* pmLayer = en.Current;
        if (strFilename->Equals(pmLayer->Name))
        {
            return pmLayer;
        }
    }

    return NULL;
}
//--------------------------------------------------------------------------
MLayer* MLayerManager::GetLayer(unsigned i)
{
    MVerifyValidInstance;

    return m_mLayers->Item[i];
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::GetLayers()[]
{
    MVerifyValidInstance;

    return m_mLayers->ToArray();
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::GetTopLevelLayers()[]
{
    MVerifyValidInstance;

    if (DefaultLayer == NULL)
    {
        return new MLayer*[0];
    } else
    {
        return DefaultLayer->GetSubLayers();
    }
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::GetImplicitLayers()[]
{
    MVerifyValidInstance;

    Layers* implicitLayers = new Layers();

    MLayer* defLayer = DefaultLayer;
    if (defLayer)
    {
        Dictionary<String*, bool>* explicitLayers = 
            new Dictionary<String*, bool>();

        defLayer->BuildExplicitList(explicitLayers);

        Layers::Enumerator en = m_mLayers->GetEnumerator();
        while (en.MoveNext())
        {
            MLayer* pmLayer = en.Current;
            if (!pmLayer->IsDefaultLayer && 
                !explicitLayers->ContainsKey(pmLayer->Name))
            {
                implicitLayers->Add(pmLayer);
            }
        }
    }
    return implicitLayers->ToArray();
}
//---------------------------------------------------------------------------
bool MLayerManager::IsImplicit(MLayer* pmLayer)
{
    NIASSERT(DefaultLayer != NULL);

    Dictionary<String*, bool>* explicitLayers = 
        new Dictionary<String*, bool>();

    DefaultLayer->BuildExplicitList(explicitLayers);
    return !explicitLayers->ContainsKey(pmLayer->Name);
}
//---------------------------------------------------------------------------
void MLayerManager::RemoveLayer(MLayer* pmLayer, bool bDeleteFile)
{
    MVerifyValidInstance;

    NIASSERT(pmLayer != DefaultLayer);

    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        MLayer* pmParent = en.Current;
        if (pmParent->IsExplicitSublayer(pmLayer))
        {
            pmParent->Dirty = true;
            pmParent->DoRemoveExplicitLayer(pmLayer);
        }
    }

    pmLayer->RemoveEntitiesFromMainScene(false);
    pmLayer->RemoveSceneFromStreamingObject();

    if (bDeleteFile)
    {
        FileInfo* fi = new FileInfo(pmLayer->Name);
        if (fi->IsReadOnly)
        {
            fi->IsReadOnly = false;
        }
        fi->Delete();
    }

    m_mLayers->Remove(pmLayer);
    MEventManager::Instance->RaiseLayerDeleted(pmLayer, bDeleteFile);

    // If we deleted the active layer we need to find a new one
    if (ActiveLayer == pmLayer)
    {
        ActiveLayer = FindActiveLayer();
    }
    pmLayer->Dispose();

    CommandService->ClearUndoAndRedoStacks();
}
//---------------------------------------------------------------------------
void MLayerManager::RemoveAllLayers()
{
    MVerifyValidInstance;

    // this operation results in a flurry of entities being removed from the scene
    // ignore all of them since the entire scene is being deleted.
    MEventManager* evmgr = MEventManager::Instance;
    __unhook(&MEventManager::EntityRemovedFromScene, evmgr, 
        &MLayerManager::OnEntityRemovedFromScene);

    DoSetActiveLayer(NULL, true);

    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        MLayer* pmLayer = en.Current;
        // don't remove it from the streaming object because eventually
        // the streaming object's Reset function will be called.
        pmLayer->Dispose();
    }
    m_mLayers->Clear();

    __hook(&MEventManager::EntityRemovedFromScene, evmgr, 
        &MLayerManager::OnEntityRemovedFromScene);
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::get_DefaultLayer()
{
    return m_mLayers->Count > 0 ? m_mLayers->Item[0] : NULL;
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::get_ActiveLayer()
{
    MVerifyValidInstance;

    return m_pmActiveLayer;
}
//---------------------------------------------------------------------------
void MLayerManager::set_ActiveLayer(MLayer* pmActiveLayer)
{
    MVerifyValidInstance;

    SetActiveLayer(pmActiveLayer, true);
}
//---------------------------------------------------------------------------
void MLayerManager::SetActiveLayer(MLayer* pmActiveLayer, bool bUndoable)
{
    MVerifyValidInstance;

    CommandService->ExecuteCommand(new MSetActiveLayerCommand(m_pmActiveLayer,
        pmActiveLayer), bUndoable);
}
//---------------------------------------------------------------------------
void MLayerManager::DoSetActiveLayer(MLayer* pmActiveLayer,
    bool enableCallbacks)
{
    if (pmActiveLayer != m_pmActiveLayer)
    {
        MLayer* pmOldActiveLayer = m_pmActiveLayer;
        m_pmActiveLayer = pmActiveLayer;
        if (enableCallbacks)
        {
            MEventManager::Instance->
                RaiseActiveLayerChanged(m_pmActiveLayer, pmOldActiveLayer);
            }
    }
}
//---------------------------------------------------------------------------
void MLayerManager::Init(MEventManager* evmgr)
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MLayerManager(evmgr);
    }
}
//---------------------------------------------------------------------------
void MLayerManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MLayerManager::InstanceIsValid()
{
    return (ms_pmThis != 0);
}
//---------------------------------------------------------------------------
MLayerManager* MLayerManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
void MLayerManager::OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmScene == MFramework::Instance->Scene)
    {
        if (m_pmActiveLayer)
        {
            m_pmActiveLayer->AddEntity(pmEntity, true, false);
        }
    }
}
//---------------------------------------------------------------------------
void MLayerManager::OnEntityRemovedFromScene(MScene* pmScene,
    MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmScene == MFramework::Instance->Scene)
    {
        MLayer* pmLayer = MLayer::FindLayer(pmEntity);
        if (pmLayer)
        {
            pmLayer->RemoveEntity(pmEntity, true, false);
        }
    }
}
//---------------------------------------------------------------------------
namespace
{
    List<MLayer*>* SearchReadOnlyLayersForEntityReference(MEntity* pmEntity, 
        List<MLayer*>* layers)
    {
        List<MLayer*>* dependents = new List<MLayer*>();
        List<MLayer*>::Enumerator en = layers->GetEnumerator();
        while (en.MoveNext())
        {
            MLayer* pmLayer = en.Current;
            MAssert(pmLayer != NULL, "Null layer encountered!");
            if (!pmLayer->Writable && pmLayer->IsEntityReferenced(pmEntity))
            {
                dependents->Add(pmLayer);            
            }
        }
        return dependents;
    }
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::FindDependentReadOnlyLayers(MLayer* pmLayer)[]
{
    MVerifyValidInstance;

    // make a list of all candidates to search
    List<MLayer*>* candidates = new List<MLayer*>();
    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        MAssert(en.Current != NULL, "Null layer encountered!");
        if (!en.Current->Writable && en.Current != pmLayer)
        {
            candidates->Add(en.Current);
        }
    }
    if (pmLayer->IsDefaultLayer)
    {
        return candidates->ToArray();
    }

    List<MLayer*>* allDependents = new List<MLayer*>();

    MEntity* entities[] = pmLayer->GetEntities();
    for (int i = 0; i < entities->Length; ++i)
    {
        MEntity* pmEntity = entities[i];
        MAssert(pmEntity != NULL, "Null entity encountered!");
        List<MLayer*>* dependents = 
            ::SearchReadOnlyLayersForEntityReference(pmEntity, candidates);
        Layers::Enumerator en = dependents->GetEnumerator();
        while (en.MoveNext())
        {
            candidates->Remove(en.Current);
            allDependents->Add(en.Current);
        }
    }
    return allDependents->ToArray();
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::SearchReadOnlyLayersForEntityReference(
    MEntity* pmEntity)[]
{
    MVerifyValidInstance;

    List<MLayer*>* pmDependentLayers = 
        ::SearchReadOnlyLayersForEntityReference(pmEntity, m_mLayers);
    return pmDependentLayers->ToArray();
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::FindReferencingLayers(MLayer* pmLayer)[]
{
    List<MLayer*>* candidates = new List<MLayer*>();
    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        MAssert(en.Current != NULL, "Null layer encountered!");
        if (en.Current != pmLayer && en.Current->IsExplicitSublayer(pmLayer))
        {
            candidates->Add(en.Current);
        }
    }
    return candidates->ToArray();
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::FindActiveLayer()
{
    MVerifyValidInstance;

    // Find the first writable layer
    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        MLayer* pmCandidate = en.Current;
        if (pmCandidate != m_pmActiveLayer && pmCandidate->Writable)
        {
            return pmCandidate;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
void MLayerManager::OnLayerStatusChanged(MLayer* pmLayer)
{
    MVerifyValidInstance;

    // If the active layer became unwritable then we need a new active layer
    if (pmLayer == m_pmActiveLayer && !pmLayer->Writable)
    {
        MLayer* pmNewCurrent = FindActiveLayer();
        SetActiveLayer(pmNewCurrent, false);
    } else if (m_pmActiveLayer == NULL)
    {
        // we don't have an active layer, see if this one works
        if (pmLayer->Writable)
        {
            SetActiveLayer(pmLayer, false);
        }
    }
}
//---------------------------------------------------------------------------
ICommandService* MLayerManager::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
bool MLayerManager::SaveAll(String* strFilename)
{
    return SaveAll(strFilename, false);
}
//---------------------------------------------------------------------------
bool MLayerManager::SaveAll(String* strFilename, bool bAutoSave)
{
    MEventManager::Instance->RaiseMainSceneSaving(strFilename, bAutoSave);

    bool bSuccess = true;
    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        MLayer* layer = en.Current;

        // The name of the default layer may be changing.
        if (layer->IsDefaultLayer && !layer->Name->Equals(strFilename))
        {
            MAssert(!bAutoSave, "AutoSave specified when changing the "
                "name of the default layer!");
            layer->Name = strFilename;
        }

        if (layer->Writable)
        {
            bSuccess = bSuccess && layer->Save(bAutoSave);
        }
    }

    MEventManager::Instance->RaiseMainSceneSaved(strFilename, bAutoSave);

    return bSuccess;
}
//---------------------------------------------------------------------------
MLayer* MLayerManager::FindLayerByName(String* name)
{
    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        MLayer* layer = en.Current;
        if (layer->Name->Equals(name))
        {
            return layer;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
MEntity* MLayerManager::FindEntityByName(String* name)
{
    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        MLayer* layer = en.Current;
        MEntity* entity = layer->FindEntityByName(name);
        if (entity != NULL)
        {
            return entity;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool MLayerManager::AreAnyLayersDirty()
{
    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        if (en.Current->Dirty)
        {
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void MLayerManager::OverrideWritableFlag()
{
    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        en.Current->OverrideWritableFlag();
    }
}
//---------------------------------------------------------------------------
void MLayerManager::RemoveWritableFlagOverride()
{
    Layers::Enumerator en = m_mLayers->GetEnumerator();
    while (en.MoveNext())
    {
        en.Current->RemoveWritableFlagOverride();
    }
}
//---------------------------------------------------------------------------
