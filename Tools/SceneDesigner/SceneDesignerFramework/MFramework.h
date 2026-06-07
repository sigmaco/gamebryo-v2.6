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

#include "MRenderer.h"
#include "MEventManager.h"
#include "MTimeManager.h"
#include "MCameraManager.h"
#include "MViewportManager.h"
#include "MPaletteManager.h"
#include "MProxyManager.h"
#include "MLightManager.h"
#include "MBoundManager.h"
#include "MTagsManager.h"
#include "MFilterManager.h"

#include "MSceneFactory.h"
#include "MEntityFactory.h"
#include "MSelectionSetFactory.h"
#include "MComponentFactory.h"

#include "MScene.h"
#include "MPickUtility.h"
#include "IInteractionModeService.h"
#include "IRenderingModeService.h"
#include "ISelectionService.h"
#include "IUICommandService.h"
#include "ISettingsService.h"
#include "IOptionsService.h"

using namespace System::Collections::Generic;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MScene;
    public __gc class MLayerManager;
    public __gc class MPrefabManager;

    public __gc class MFramework : public MDisposable
    {
    public:
        void Startup();

        // Managers.
        __property MRenderer* get_Renderer();
        __property MEventManager* get_EventManager();
        __property MTimeManager* get_TimeManager();
        __property MCameraManager* get_CameraManager();
        __property MViewportManager* get_ViewportManager();
        __property MPaletteManager* get_PaletteManager();
        __property MProxyManager* get_ProxyManager();
        __property MLightManager* get_LightManager();
        __property MBoundManager* get_BoundManager();
        __property MLayerManager* get_LayerManager();
        __property MPrefabManager* get_PrefabManager();
        __property MTagsManager* get_TagsManager();
        __property MFilterManager* get_FilterManager();

        // Factories.
        __property MSceneFactory* get_SceneFactory();
        __property MEntityFactory* get_EntityFactory();
        __property MSelectionSetFactory* get_SelectionSetFactory();
        __property MComponentFactory* get_ComponentFactory();

        __property String* get_AppStartupPath();
        __property MScene* get_Scene();
        __property MPickUtility* get_PickUtility();
        __property NiExternalAssetManager* get_ExternalAssetManager();
        __property String* get_ApplicationName();
        __property String* get_ImageSubfolder();
        __property bool get_PerformingLongOperation();

        __property String* get_CurrentFilename();

        typedef List<MScene*> Scenes;

        Scenes* NewScene(unsigned int uiEntityArraySize);
        Scenes* LoadScene(String* strFilename, String* strFormat);
        void InitNewScene(Scenes* pmNewScenes);
        bool SaveScene(String* strFilename, String* strFormat, bool bAutoSave);

        void Update();

        // *** begin Emergent internal use only ***
        void BeginLongOperation();
        void EndLongOperation();

        void RestartAnimation();
        // *** end Emergent internal use only ***

    private public:
        // Internal helper function for loading a scene.
        Scenes* LoadScene(
            String* strFilename,
            String* strFormat,
            bool bResetStreamingObject);

    private:
        // Helper functions.
        void PerformUpdate();
        void InitNewScene(Scenes* pmNewScenes, bool bRaiseEvent);
        void RegisterSettingsAndOptions();
        void OnImageSubfolderSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        // Service accessors.
        __property static IInteractionModeService*
            get_InteractionModeService();
        static IInteractionModeService* ms_pmInteractionModeService;
        __property static ISelectionService* get_SelectionService();
        static ISelectionService* ms_pmSelectionService;
        __property static ISettingsService* get_SettingsService();
        static ISettingsService* ms_pmSettingsService;
        __property static IOptionsService* get_OptionsService();
        static IOptionsService* ms_pmOptionsService;

        // Member variables.
        MScene* m_pmScene;
        MScene* m_pmToolScene;
        MPickUtility* m_pmPickUtility;
        NiExternalAssetManager* m_pkAssetManager;
        String* m_strImageSubfolder;
        bool m_bDoNotUpdate;
        bool m_bInitSuccessful;
        unsigned int m_uiLongOperationCount;
        String* m_strCurrentFilename;

        static String* ms_strImageSubfolderSettingName = "Image Subfolder";
        static String* ms_strImageSubfolderOptionName = String::Concat(
            "External File Settings.", ms_strImageSubfolderSettingName);

        static void ClearStream();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MFramework* get_Instance();
    private:
        static MFramework* ms_pmThis = NULL;

        MFramework();
    };
}}}}
