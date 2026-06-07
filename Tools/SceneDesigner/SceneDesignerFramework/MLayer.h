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
#include "MScene.h"
#include "ICommandService.h"
#include "ISelectionService.h"

using namespace System::IO;
using namespace System::Collections::Generic;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MEntity;
    public __gc class MScene;
    public __gc class MSelectionSet;

    public __gc class MLayer : public MDisposable
    {
    public:
        // Create a group from an existing scene, all 
        // entities are added to uberscene
        MLayer(MScene* scene, NiScene* uberScene);
        // Create an empty group from a filename
        MLayer(String* strName);

        MLayer* GetSubLayers()[];

        // Returns the scene's sourcefilename
        __property String* get_Name();
        __property void set_Name(String* strFilename);

        // Returns the filename (without path) of the source layer file.
        __property String* get_ShortName();

        __property bool get_IsDefaultLayer();

        __value enum TriState
        {
            False,
            True,
            Indeterminate
        };

        __property bool get_Writable();
        __property void set_Writable(bool w);

        __property bool get_Visible();
        __property void set_Visible(bool bVisible);
        __property TriState get_VisibleState();

        __property bool get_Locked();
        __property void set_Locked(bool bLocked);
        __property TriState get_LockedState();

        __property bool get_Dirty();
        __property void set_Dirty(bool dirty);

        // Gets the file format string for this layer (not valid for groups).
        __property String* get_Format();

        void RemoveEntitiesFromMainScene(bool undoable);

        __property unsigned get_EntityCount();
        MEntity* GetEntities()[];

        void AddEntity(MEntity* pmEntity, bool enableCallbacks, bool undoable);
        void RemoveEntity(MEntity* pmEntity, bool enableCallbacks, bool undoable);

        // Selection set functions
        __property unsigned int get_SelectionSetCount();
        MSelectionSet* GetSelectionSets()[];
        MSelectionSet* GetSelectionSetByName(String* strName);
        bool AddSelectionSet(MSelectionSet* pmSelectionSet);
        void RemoveSelectionSet(MSelectionSet* pmSelectionSet);
        void RemoveAllSelectionSets();

        bool IsEntityReferenced(MEntity* pmEntity);

        bool Save(bool bAutoSave);

        void SelectAllEntities();
        // Move the entities from various layers into this one
        void MoveEntitiesToLayer(MEntity* amEntities[]);
        // Move the entities from various layers into this one
        // optionally filter out prefab entitites
        void MoveEntitiesToLayer(MEntity* amEntities[], 
            bool filterPrefabEntities);
        void MoveSelectedEntities();

        void RemoveSceneFromStreamingObject();

        MEntity* FindEntityByName(String* strName);

        void ForceVisible(bool bVisible, bool bUndoable);
        void ForceLocked(bool bLocked, bool bUndoable);

        bool IsExplicitSublayer(MLayer* pmSubLayer);
        void AddExplicitLayer(MLayer* layer);
        void RemoveExplicitLayer(MLayer* layer);

        void Rename(String* strFilename, bool bDeleteOldFile);

        static MLayer* FindLayer(MEntity* pmEntity);

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    private public:
        // Callable only by Add/RemoveLayerCommand
        void DoAddExplicitLayer(MLayer* layer);
        void DoRemoveExplicitLayer(MLayer* layer);
        // Callable only by the ChangeLayerStatusCommand
        void DoChangeVisibleStatus(bool bVisible);
        // Callable only by the ChangeLayerStatusCommand
        void DoChangeLockedStatus(bool bLocked);

        // These functions are used to temporarily mark the layer as writable.
        void OverrideWritableFlag();
        void RemoveWritableFlagOverride();

        void BuildExplicitList(Dictionary<String*, bool>* explicitLayers);

    private:
        FileSystemWatcher* m_sentinel;
        MScene* m_scene;
        unsigned int m_overrideCount;
        bool m_closing;     // layer is closing
        bool m_writable;    // layer (file) is writable
        bool m_bVisible;
        bool m_bLocked;

        bool Do_Save(String* strFilename, bool bAutoSave);

        void CreateFileWatcher();
        void OnFileStatusChanged(Object* /*source*/, FileSystemEventArgs* e);

        static ICommandService* ms_pmCommandService;
        __property static ICommandService* get_CommandService();

        static ISelectionService* ms_pmSelectionService;
        __property static ISelectionService* get_SelectionService();

        static IMessageService* ms_pmMessageService;
        __property static IMessageService* get_MessageService();
    };
}}}}