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

#pragma once

#include "MDisposable.h"
#include "IMessageService.h"
#include "MEntity.h"
#include "MScene.h"
#include "MSelectionSet.h"
#include "MBatchCommand.h"
#include "MPalette.h"
#include "MViewport.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MLayer;
    public __gc class MPrefab;

    public __gc class MEventManager : public MDisposable
    {
    public:
        // Events definitions.
        __event void EntityPropertyAdded(MEntity* pmEntity,
            String* strPropertyName, bool bInBatch);
        __event void EntityPropertyRemoved(MEntity* pmEntity,
            String* strPropertyName, bool bInBatch);
        __event void EntityPropertyChanging(MEntity* pmEntity, 
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        __event void EntityPropertyChanged(MEntity* pmEntity,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        __event void ComponentPropertyAdded(MComponent* pmComponent,
            String* strPropertyName, bool bInBatch);
        __event void ComponentPropertyRemoved(MComponent* pmComponent,
            String* strPropertyName, bool bInBatch);
        __event void ComponentPropertyChanging(MComponent* pmComponent,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        __event void ComponentPropertyChanged(MComponent* pmComponent,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        __event void EntityComponentAdded(MEntity* pmEntity,
            MComponent* pmComponent);
        __event void EntityComponentRemoved(MEntity* pmEntity,
            MComponent* pmComponent);
        __event void SelectedEntitiesChanged();
        __event void MainSceneSaving(String* strFilename, bool bAutoSave);
        __event void MainSceneSaved(String* strFilename, bool bAutoSave);
        __event void SceneClosing(MScene* pmScene);
        __event void NewSceneLoaded(MScene* pmScene);
        __event void DeletedAutoSave(String* strFilename);
        __event void RequestAddCloneToScene(MScene* pmScene,
            MEntity* pmSourceEntity, bool __gc* bDoNotAdd);
        __event void EntityAddedToScene(MScene* pmScene, MEntity* pmEntity);
        __event void EntityRemovedFromScene(MScene* pmScene,
            MEntity* pmEntity);
        __event void SelectionSetAddedToScene(MScene* pmScene,
            MSelectionSet* pmSelectionSet);
        __event void SelectionSetRemovedFromScene(MScene* pmScene,
            MSelectionSet* pmSelectionSet);
        __event void EntityNameChanging(MEntity* pmEntity, String* strNewName,
            bool bInBatch);
        __event void EntityNameChanged(MEntity* pmEntity, String* strOldName,
            bool bInBatch);
        __event void PaletteAdded(MPalette* pmPalette);
        __event void PaletteRemoved(MPalette* pmPalette);
        __event void PaletteActiveEntityChanged(MPalette* pmPalette,
            MEntity* pmOldActiveEntity);
        __event void ActivePaletteChanged(MPalette* pmActivePalette,
            MPalette* pmOldActivePalette);
        __event void SelectionSetNameChanging(MSelectionSet* pmSelectionSet,
            String* strNewName, bool bInBatch);
        __event void SelectionSetNameChanged(MSelectionSet* pmSelectionSet,
            String* strOldName, bool bInBatch);
        __event void ViewportCameraChanged(MViewport* pmViewport, 
            MEntity* pmCamera);
        __event void ViewportRenderingModeChanged(MViewport* pmViewport,
            IRenderingMode* pmRenderingMode);
        __event void ActiveViewportChanged(MViewport* pmOldViewport,
            MViewport* pmNewViewport);
        __event void ExclusiveViewportChanged(MViewport* pmOldViewport,
            MViewport* pmNewViewport);
        __event void CameraAddedToManager(MEntity* pmCamera);
        __event void CameraRemovedFromManager(MEntity* pmCamera);
        __event void EntityHiddenStateChanged(MEntity* pmEntity,
            bool bHidden);
        __event void EntityFrozenStateChanged(MEntity* pmEntity,
            bool bFrozen);
        __event void EntityTagsChanged(MEntity* pmEntity, String* strOldTags);
        __event void ComponentServiceChanged(MComponent* pmComponent);
        __event void LongOperationStarted();
        __event void LongOperationCompleted();
        __event void ApplicationClosing();
        __event void LayerAdded(MLayer* pmLayer, MLayer* pmParent);
        __event void LayerDeleted(MLayer* pmLayer, bool bFileDeleted);
        __event void LayerRenamed(MLayer* pmLayer, String* strOldFilename);
        __event void LayerSaving(MLayer* pmLayer, bool bAutoSave);
        __event void LayerSaved(MLayer* pmLayer, bool bAutoSave);
        __event void LayerFilenameAdded(MLayer* pmLayer, String* strFilename);
        __event void LayerFilenameRemoved(MLayer* pmLayer, String* strFilename);
        __event void LayerFilenameRenamed(MLayer* pmLayer, 
            String* strOldFilename, String* strNewFilename);
        __event void LayerStatusChanged(MLayer* pmLayer);
        __event void ActiveLayerChanged(MLayer* pmActiveLayer, 
            MLayer* pmOldActiveLayer);
        __event void EntitiesMovedToNewLayer(MLayer* pmSrcLayer, 
            MLayer* pmDstLayer, MEntity* amEntities[]);
        __event void PrefabAdded(MPrefab* pmPrefab);
        __event void PrefabRemoved(MPrefab* pmPrefab);
        __event void TagsManagerUpdated();
        __event void FilterEnabledChanged();
        __event void FilterOptionsChanged();
        __event void PaletteWritableStatusChanged(MPalette* pmPalette);

        // Raising functions.
        void RaiseEntityPropertyAdded(MEntity* pmEntity, 
            String* strPropertyName, bool bInBatch);
        void RaiseEntityPropertyRemoved(MEntity* pmEntity, 
            String* strPropertyName, bool bInBatch);
        void RaiseEntityPropertyChanging(MEntity* pmEntity, 
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void RaiseEntityPropertyChanged(MEntity* pmEntity,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void RaiseComponentPropertyAdded(MComponent* pmComponent,
            String* strPropertyName, bool bInBatch);
        void RaiseComponentPropertyRemoved(MComponent* pmComponent,
            String* strPropertyName, bool bInBatch);
        void RaiseComponentPropertyChanging(MComponent* pmComponent,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void RaiseComponentPropertyChanged(MComponent* pmComponent,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void RaiseEntityComponentAdded(MEntity* pmEntity,
            MComponent* pmComponent);
        void RaiseEntityComponentRemoved(MEntity* pmEntity,
            MComponent* pmComponent);
        void RaiseSelectedEntitiesChanged();
        void RaiseMainSceneSaving(String* strFilename, bool bAutoSave);
        void RaiseMainSceneSaved(String* strFilename, bool bAutoSave);
        void RaiseSceneClosing(MScene* pmScene);
        void RaiseNewSceneLoaded(MScene* pmScene);
        void RaiseDeletedAutoSave(String* strFilename);
        void RaiseRequestAddCloneToScene(MScene* pmScene,
            MEntity* pmSourceEntity, bool __gc* bDoNotAdd);
        void RaiseEntityAddedToScene(MScene* pmScene, MEntity* pmEntity);
        void RaiseEntityRemovedFromScene(MScene* pmScene,
            MEntity* pmEntity);
        void RaiseSelectionSetAddedToScene(MScene* pmScene,
            MSelectionSet* pmSelectionSet);
        void RaiseSelectionSetRemovedFromScene(MScene* pmScene,
            MSelectionSet* pmSelectionSet);
        void RaiseEntityNameChanging(MEntity* pmEntity, String* strNewName,
            bool bInBatch);
        void RaiseEntityNameChanged(MEntity* pmEntity, String* strOldName,
            bool bInBatch);
        void RaisePaletteAdded(MPalette* pmPalette);
        void RaisePaletteRemoved(MPalette* pmPalette);
        void RaisePaletteActiveEntityChanged(MPalette* pmPalette,
            MEntity* pmOldActiveEntity);
        void RaiseActivePaletteChanged(MPalette* pmActivePalette,
            MPalette* pmOldActivePalette);       
        void RaiseSelectionSetNameChanging(MSelectionSet* pmSelectionSet,
            String* strNewName, bool bInBatch);
        void RaiseSelectionSetNameChanged(MSelectionSet* pmSelectionSet,
            String* strOldName, bool bInBatch);
        void RaiseViewportCameraChanged(MViewport* pmViewport,
            MEntity* pmCamera);
        void RaiseViewportRenderingModeChanged(MViewport* pmViewport,
            IRenderingMode* pmRenderingMode);
        void RaiseActiveViewportChanged(MViewport* pmOldViewport,
            MViewport* pmNewViewport);
        void RaiseExclusiveViewportChanged(MViewport* pmOldViewport,
            MViewport* pmNewViewport);
        void RaiseCameraAddedToManager(MEntity* pmCamera);
        void RaiseCameraRemovedFromManager(MEntity* pmCamera);
        void RaiseEntityHiddenStateChanged(MEntity* pmEntity, bool bHidden);
        void RaiseEntityFrozenStateChanged(MEntity* pmEntity, bool bFrozen);
        void RaiseEntityTagsChanged(MEntity* pmEntity, String* strOldTags);
        void RaiseComponentServiceChanged(MComponent* pmComponent);
        void RaiseLongOperationStarted();
        void RaiseLongOperationCompleted();
        void RaiseApplicationClosing();
        void RaiseLayerAdded(MLayer* pmLayer, MLayer* pmParent);
        void RaiseLayerDeleted(MLayer* pmLayer, bool bFileDeleted);
        void RaiseLayerRenamed(MLayer* pmLayer, String* strOldFilename);
        void RaiseLayerSaving(MLayer* pmLayer, bool bAutoSave);
        void RaiseLayerSaved(MLayer* pmLayer, bool bAutoSave);
        void RaiseLayerFilenameAdded(MLayer* pmLayer, String* strFilename);
        void RaiseLayerFilenameRemoved(MLayer* pmLayer, String* strFilename);
        void RaiseLayerFilenameRenamed(MLayer* pmLayer, 
            String* strOldFilename, String* strNewFilename);
        void RaiseLayerStatusChanged(MLayer* pmLayer);
        void RaiseActiveLayerChanged(MLayer* pmActiveLayer,
            MLayer* pmOldActiveLayer);       
        void RaiseEntitiesMovedToNewLayer(MLayer* pmSrcLayer, 
            MLayer* pmDstLayer, MEntity* amEntities[]);
        void RaisePrefabAdded(MPrefab* pmPrefab);
        void RaisePrefabRemoved(MPrefab* pmPrefab);
        void RaiseTagsManagerUpdated();
        void RaiseFilterEnabledChanged();
        void RaiseFilterOptionsChanged();
        void RaisePaletteWritableStatusChanged(MPalette* pmPalette);

    private:
        static IMessageService* ms_pmMessageService;
        __property static IMessageService* get_MessageService();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MEventManager* get_Instance();
    private:
        static MEventManager* ms_pmThis = NULL;
        MEventManager();
    };
}}}}
