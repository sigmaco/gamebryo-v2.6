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

#include "MEventManager.h"
#include "ServiceProvider.h"
#include "MLayer.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
void MEventManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MEventManager();
    }
}
//---------------------------------------------------------------------------
void MEventManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MEventManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MEventManager* MEventManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MEventManager::MEventManager()
{
}
//---------------------------------------------------------------------------
void MEventManager::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
IMessageService* MEventManager::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPropertyAdded(MEntity* pmEntity, 
    String* strPropertyName, bool bInBatch)
{
    MVerifyValidInstance;

    EntityPropertyAdded(pmEntity, strPropertyName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPropertyRemoved(MEntity* pmEntity, 
    String* strPropertyName, bool bInBatch)
{
    MVerifyValidInstance;

    EntityPropertyRemoved(pmEntity, strPropertyName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPropertyChanging(MEntity* pmEntity, 
    String* strPropertyName, unsigned int uiPropertyIndex,
    bool bInBatch)
{
    MVerifyValidInstance;

    EntityPropertyChanging(pmEntity, strPropertyName, uiPropertyIndex,
        bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPropertyChanged(MEntity* pmEntity,
    String* strPropertyName, unsigned int uiPropertyIndex,
    bool bInBatch)
{
    MVerifyValidInstance;

    EntityPropertyChanged(pmEntity, strPropertyName, uiPropertyIndex,
        bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseComponentPropertyAdded(MComponent* pmComponent,
    String* strPropertyName, bool bInBatch)
{
    MVerifyValidInstance;

    ComponentPropertyAdded(pmComponent, strPropertyName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseComponentPropertyRemoved(MComponent* pmComponent,
    String* strPropertyName, bool bInBatch)
{
    MVerifyValidInstance;

    ComponentPropertyRemoved(pmComponent, strPropertyName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseComponentPropertyChanging(MComponent* pmComponent,
    String* strPropertyName, unsigned int uiPropertyIndex,
    bool bInBatch)
{
    MVerifyValidInstance;

    ComponentPropertyChanging(pmComponent, strPropertyName, uiPropertyIndex,
        bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseComponentPropertyChanged(MComponent* pmComponent,
    String* strPropertyName, unsigned int uiPropertyIndex,
    bool bInBatch)
{
    MVerifyValidInstance;

    ComponentPropertyChanged(pmComponent, strPropertyName, uiPropertyIndex,
        bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityComponentAdded(MEntity* pmEntity,
    MComponent* pmComponent)
{
    MVerifyValidInstance;

    EntityComponentAdded(pmEntity, pmComponent);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityComponentRemoved(MEntity* pmEntity,
    MComponent* pmComponent)
{
    MVerifyValidInstance;

    EntityComponentRemoved(pmEntity, pmComponent);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSelectedEntitiesChanged()
{
    MVerifyValidInstance;

    SelectedEntitiesChanged();
}
//---------------------------------------------------------------------------
void MEventManager::RaiseMainSceneSaving(String* strFilename, bool bAutoSave)
{
    MVerifyValidInstance;

    MainSceneSaving(strFilename, bAutoSave);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseMainSceneSaved(String* strFilename, bool bAutoSave)
{
    MVerifyValidInstance;

    MainSceneSaved(strFilename, bAutoSave);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSceneClosing(MScene* pmScene)
{
    MVerifyValidInstance;

    SceneClosing(pmScene);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseNewSceneLoaded(MScene* pmScene)
{
    MVerifyValidInstance;

    NewSceneLoaded(pmScene);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseDeletedAutoSave(String* strFilename)
{
    MVerifyValidInstance;

    DeletedAutoSave(strFilename);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseRequestAddCloneToScene(MScene* pmScene,
    MEntity* pmSourceEntity, bool __gc* bDoNotAdd)
{
    MVerifyValidInstance;

    RequestAddCloneToScene(pmScene, pmSourceEntity, bDoNotAdd);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityAddedToScene(MScene* pmScene,
    MEntity* pmEntity)
{
    MVerifyValidInstance;

    EntityAddedToScene(pmScene, pmEntity);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityRemovedFromScene(MScene* pmScene,
    MEntity* pmEntity)
{
    MVerifyValidInstance;

    EntityRemovedFromScene(pmScene, pmEntity);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSelectionSetAddedToScene(MScene* pmScene,
    MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    SelectionSetAddedToScene(pmScene, pmSelectionSet);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSelectionSetRemovedFromScene(MScene* pmScene,
    MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    SelectionSetRemovedFromScene(pmScene, pmSelectionSet);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityNameChanging(MEntity* pmEntity,
    String* strNewName, bool bInBatch)
{
    MVerifyValidInstance;

    EntityNameChanging(pmEntity, strNewName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityNameChanged(MEntity* pmEntity,
    String* strOldName, bool bInBatch)
{
    MVerifyValidInstance;

    EntityNameChanged(pmEntity, strOldName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaisePaletteAdded(MPalette* pmPalette)
{
    MVerifyValidInstance;

    PaletteAdded(pmPalette);
}
//---------------------------------------------------------------------------
void MEventManager::RaisePaletteRemoved(MPalette* pmPalette)
{
    MVerifyValidInstance;

    PaletteRemoved(pmPalette);
}
//---------------------------------------------------------------------------
void MEventManager::RaisePaletteActiveEntityChanged(MPalette* pmPalette,
    MEntity* pmOldActiveEntity)
{
    MVerifyValidInstance;

    PaletteActiveEntityChanged(pmPalette, pmOldActiveEntity);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseActivePaletteChanged(MPalette* pmActivePalette,
    MPalette* pmOldActivePalette)
{
    MVerifyValidInstance;

    ActivePaletteChanged(pmActivePalette, pmOldActivePalette);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSelectionSetNameChanging(
    MSelectionSet* pmSelectionSet, String* strNewName, bool bInBatch)
{
    MVerifyValidInstance;

    SelectionSetNameChanging(pmSelectionSet, strNewName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSelectionSetNameChanged(
    MSelectionSet* pmSelectionSet, String* strOldName, bool bInBatch)
{
    MVerifyValidInstance;

    SelectionSetNameChanged(pmSelectionSet, strOldName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseViewportCameraChanged(MViewport* pmViewport,
    MEntity* pmCamera)
{
    MVerifyValidInstance;

    ViewportCameraChanged(pmViewport, pmCamera);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseViewportRenderingModeChanged(MViewport* pmViewport,
    IRenderingMode* pmRenderingMode)
{
    MVerifyValidInstance;

    ViewportRenderingModeChanged(pmViewport, pmRenderingMode);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseActiveViewportChanged(MViewport* pmOldViewport,
    MViewport* pmNewViewport)
{
    MVerifyValidInstance;

    ActiveViewportChanged(pmOldViewport, pmNewViewport);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseExclusiveViewportChanged(MViewport* pmOldViewport,
    MViewport* pmNewViewport)
{
    MVerifyValidInstance;

    ExclusiveViewportChanged(pmOldViewport, pmNewViewport);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseCameraAddedToManager(MEntity* pmCamera)
{
    MVerifyValidInstance;

    CameraAddedToManager(pmCamera);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseCameraRemovedFromManager(MEntity* pmCamera)
{
    MVerifyValidInstance;

    CameraRemovedFromManager(pmCamera);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityHiddenStateChanged(MEntity* pmEntity,
    bool bHidden)
{
    MVerifyValidInstance;

    EntityHiddenStateChanged(pmEntity, bHidden);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityFrozenStateChanged(MEntity* pmEntity,
    bool bFrozen)
{
    MVerifyValidInstance;

    EntityFrozenStateChanged(pmEntity, bFrozen);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityTagsChanged(MEntity* pmEntity,
    String* strOldTags)
{
    MVerifyValidInstance;

    EntityTagsChanged(pmEntity, strOldTags);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseComponentServiceChanged(MComponent* pmComponent)
{
    MVerifyValidInstance;

    ComponentServiceChanged(pmComponent);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLongOperationStarted()
{
    MVerifyValidInstance;

    LongOperationStarted();
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLongOperationCompleted()
{
    MVerifyValidInstance;

    LongOperationCompleted();
}
//---------------------------------------------------------------------------
void MEventManager::RaiseApplicationClosing()
{
    MVerifyValidInstance;

    ApplicationClosing();
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLayerAdded(MLayer* pmLayer, MLayer* pmParent)
{
    MVerifyValidInstance;

    LayerAdded(pmLayer, pmParent);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLayerDeleted(MLayer* pmLayer, bool bFileDeleted)
{
    MVerifyValidInstance;

    LayerDeleted(pmLayer, bFileDeleted);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLayerRenamed(MLayer* pmLayer, String* strOldFilename)
{
    MVerifyValidInstance;

    LayerRenamed(pmLayer, strOldFilename);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLayerSaving(MLayer* pmLayer, bool bAutoSave)
{
    MVerifyValidInstance;

    LayerSaving(pmLayer, bAutoSave);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLayerSaved(MLayer* pmLayer, bool bAutoSave)
{
    MVerifyValidInstance;

    LayerSaved(pmLayer, bAutoSave);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLayerFilenameAdded(MLayer* pmLayer, 
    String* strFilename)
{
    MVerifyValidInstance;

    LayerFilenameAdded(pmLayer, strFilename);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLayerFilenameRemoved(MLayer* pmLayer, 
    String* strFilename)
{
    MVerifyValidInstance;

    LayerFilenameRemoved(pmLayer, strFilename);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLayerFilenameRenamed(MLayer* pmLayer, 
    String* strOldFilename, String* strNewFilename)
{
    MVerifyValidInstance;

    LayerFilenameRenamed(pmLayer, strOldFilename, strNewFilename);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLayerStatusChanged(MLayer* pmLayer)
{
    MVerifyValidInstance;

    LayerStatusChanged(pmLayer);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseActiveLayerChanged(MLayer* pmActiveLayer,
    MLayer* pmOldActiveLayer)
{
    MVerifyValidInstance;

    ActiveLayerChanged(pmActiveLayer, pmOldActiveLayer);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntitiesMovedToNewLayer(MLayer* pmSrcLayer, 
    MLayer* pmDstLayer, MEntity* amEntities[])
{
    MVerifyValidInstance;

    EntitiesMovedToNewLayer(pmSrcLayer, pmDstLayer, amEntities);
}
//---------------------------------------------------------------------------
void MEventManager::RaisePrefabAdded(MPrefab* pmPrefab)
{
    MVerifyValidInstance;

    PrefabAdded(pmPrefab);
}
//---------------------------------------------------------------------------
void MEventManager::RaisePrefabRemoved(MPrefab* pmPrefab)
{
    MVerifyValidInstance;

    PrefabRemoved(pmPrefab);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseTagsManagerUpdated()
{
    MVerifyValidInstance;

    TagsManagerUpdated();
}
//---------------------------------------------------------------------------
void MEventManager::RaiseFilterEnabledChanged()
{
    MVerifyValidInstance;

    FilterEnabledChanged();
}
//---------------------------------------------------------------------------
void MEventManager::RaiseFilterOptionsChanged()
{
    MVerifyValidInstance;

    FilterOptionsChanged();
}
//---------------------------------------------------------------------------
void MEventManager::RaisePaletteWritableStatusChanged(MPalette* pmPalette)
{
    MVerifyValidInstance;

    PaletteWritableStatusChanged(pmPalette);
}
//---------------------------------------------------------------------------
