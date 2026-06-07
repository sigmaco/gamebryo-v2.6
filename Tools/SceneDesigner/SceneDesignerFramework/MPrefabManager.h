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
#include "MPoint3.h"
#include "ICommandService.h"
#include "ISelectionService.h"
#include "IMessageService.h"

using namespace System::Collections::Generic;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MPrefab;
    public __gc class MEntity;
    public __gc class MScene;
    public __gc class MLayer;

    public __gc class MPrefabManager : public MDisposable
    {
    public:
        void RemoveAllPrefabs();

        MPrefab* LoadPrefab(String* filename);

        String* BuildSubcategories(String* path)[];
        MPrefab* GetPrefabsInCategory(String* category)[];

        MPrefab* GetPrefabByName(String* category, String* prefab);

        MPrefab* GetPrefabByFilename(String* fullpath);

        // Return the prefab folder (same as palette folder)
        __property String* get_PrefabFolder();

        __property MPrefab* get_ActivePrefab();
        __property void set_ActivePrefab(MPrefab* newCurrent);

        // Determines whether picking a prefab allows picking of the
        // individual entities in the prefab (true) or only the root (false)
        __property bool get_PickPrefabEntities();
        __property void set_PickPrefabEntities(bool bPickPrefabEntities);

        // List of prefabs held by the PrefabManager
        MPrefab* GetPrefabs()[];

        MPrefab* CreatePrefabFromSelection(String* strFilename, 
            bool bSubstitute);

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MPrefabManager* get_Instance();

    private:
        static MPrefabManager* ms_pmThis = NULL;
        static String* ms_strTranslationName = "Translation";
        static ICommandService* ms_pmCommandService;
        __property static ICommandService* get_CommandService();
        static ISelectionService* ms_pmSelectionService;
        __property static ISelectionService* get_SelectionService();

        MPrefabManager();

        String* PrepPath(String* path);
        bool AddPrefab(MPrefab* pmPrefab);

        void OnEntitiesMovedToNewLayer(MLayer* pmSrcLayer, MLayer* pmDstLayer,
            MEntity* amEntities[]);
        void OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity);
        void OnSceneClosing(MScene* pmScene);

        typedef List<MPrefab*> Prefabs;

        Prefabs* m_mPrefabs;          // all open layers
        MPrefab* m_pmActive;
        bool     m_bPickPrefabEntities;

        static IMessageService* ms_pmMessageService = NULL;
        __property static IMessageService* get_MessageService();
    };
}}}}
