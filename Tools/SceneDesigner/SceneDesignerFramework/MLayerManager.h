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
#include "MLayer.h"
#include "ICommandService.h"

using namespace System::Collections::Generic;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MEventManager;

    public __gc class MLayerManager : public MDisposable
    {
    public:
        __property unsigned int get_Count();
        MLayer* GetLayerByName(String* strFilename);
        MLayer* GetLayer(unsigned i);
        MLayer* GetLayers()[];
        MLayer* GetTopLevelLayers()[];
        MLayer* GetImplicitLayers()[];

        void AddEmptyLayer(String* strFilename, bool undoable);
        void ImportLayer(String* strFilename, String* strFormat);
        void AddLayerFromSelectedEntities(String* strFilename);

        void RemoveLayer(MLayer* pMLayer, bool bDeleteFile);

        // Returns the layer for the "main" file
        __property MLayer* get_DefaultLayer();

        __property MLayer* get_ActiveLayer();
        __property void set_ActiveLayer(MLayer* pmActiveLayer);
        void SetActiveLayer(MLayer* pmActiveLayer, bool bUndoable);

        // Return a list of all read-only layers that reference the
        // any entity in the given layer
        MLayer* FindDependentReadOnlyLayers(MLayer* pmLayer)[];
        // Return a list of all read-only layers that reference the
        // given entity
        MLayer* SearchReadOnlyLayersForEntityReference(MEntity* pmEntity)[];
        // Find all layers that reference the given layer as an explicit layer
        MLayer* FindReferencingLayers(MLayer* pmLayer)[];

        bool SaveAll(String* strFilename);
        bool SaveAll(String* strFilename, bool bAutoSave);

        MLayer* FindLayerByName(String* name);
        MEntity* FindEntityByName(String* name);

        bool IsImplicit(MLayer* pmLayer);

        void RemoveAllLayers();

        bool AreAnyLayersDirty();

    private:
        typedef List<MLayer*> Layers;

        Layers* m_mLayers;          // all open layers
        MLayer* m_pmActiveLayer;

    private public:
        // These functions are private to the assembly
        __value enum CallbackStatus { DisableEvents, EnableEvents };

        bool AddLayer(MLayer* pmLayer, MLayer* pmParent, 
            CallbackStatus events);
        // called by SetActiveLayerCommand
        void DoSetActiveLayer(MLayer* pmLayer, bool enableCallbacks);

        // These functions are used to temporarily mark all layers as writable.
        void OverrideWritableFlag();
        void RemoveWritableFlagOverride();

    private:
        static MLayerManager* ms_pmThis = NULL;
        MLayerManager(MEventManager* evmgr);
        ~MLayerManager();

        MLayer* FindActiveLayer();

        void OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity);
        void OnLayerStatusChanged(MLayer* pmLayer);

        static ICommandService* ms_pmCommandService;
        __property static ICommandService* get_CommandService();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init(MEventManager* evmgr);
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MLayerManager* get_Instance();
    };

}}}}
