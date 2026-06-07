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

#include <NiMetaDataStore.h>

#include "TerrainCommandPanel.h"
#include "SurfaceCommandPanel.h"
#include "MBrush.h"
#include "MTerrainPickPolicy.h"

#define TOGGLE_BRUSH_UI_COMMAND_NAME "ToggleTerrainBrush"
#define ENABLE_BRUSH_UI_COMMAND_NAME "EnableTerrainBrush"
#define MENU_TERRAIN_OPEN_UI_COMMAND_NAME "MenuTerrainOpen"
#define REBUILD_TERRAIN_LIGHT_COMMAND_NAME "RebuildTerrainLights"
#define MOVE_TO_TERRAIN_UI_COMMAND_NAME "MoveSelectedToTerrain"

// Note: Must be the same in the TerrainPhysX Plugin
#define PING_TERRAIN_PHYSX_PLUGIN "TerrainPlugin.PingPhysX"
#define PONG_TERRAIN_PHYSX_PLUGIN "TerrainPlugin.PongPhysX"
#define COOK_TERRAIN_PHYSX_DATA "TerrainPlugin.CookPhysxData"

#define MENU_TERRAIN "File##Terrain##"

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{
    /** 
        The terrain plug-in class interacts withe scene designer to create and
            modify a terrain.
        
     */
    public __gc class MTerrainPlugin : public MDisposable, public IPlugin
    {

    public:
        //===================================================================
        // Constructor
        //===================================================================

        /** 
            Default constructor
            
         */
        MTerrainPlugin();

        //===================================================================
        // Static properties
        //===================================================================
        __property static String* get_ActivateBrushUICommandName()
        {
            return TOGGLE_BRUSH_UI_COMMAND_NAME;
        }
        __property static String* get_MenuTerrainOpenUICommandName()
        {
            return MENU_TERRAIN_OPEN_UI_COMMAND_NAME;
        }

        __property static String* get_RebuildLightCommandName()
        {
            return REBUILD_TERRAIN_LIGHT_COMMAND_NAME;
        }

        __property static String* get_MoveToTerrainUICommandName()
        {
            return MOVE_TO_TERRAIN_UI_COMMAND_NAME;
        }

    private:
        /// The terrain command panel
        TerrainCommandPanel* m_pmCommandPanel;    
        
        /// the surface command panel
        SurfaceCommandPanel* m_pmSurfaceCommandPanel;

        /// Flag indicating true if the PhysX plugin has loaded successfully
        bool m_bPhysXPluginLoaded;

        static MTerrainPlugin *ms_pmInstance = 0;

        /// The interaction service
        IInteractionModeService* m_pmInteractionModeService;
        
        /// previous active interaction mode
        IInteractionMode* m_pmPrevActiveMode;

        /// We use as a singleton
        NiMetaDataStore* m_pkMetaDataStore;                     

        System::ComponentModel::ComponentResourceManager* m_pmResourceManager;

    protected:
        //===================================================================
        // MDisposable member
        //===================================================================
        virtual void Do_Dispose(bool bDisposing);


    public:
        //==================================================================
        //IPlugin implementation
        //==================================================================

        __property String* get_Name();

        __property System::Version* get_Version();

        __property System::Version* get_ExpectedVersion();

        virtual void Load(int iToolMajorVersion, int iToolMinorVersion);

        virtual IService* GetProvidedServices()[];

        virtual void Start();

        /** 
            Static function that gives access to the brush at anytime
            
            @return returns the active brush
         */
        MBrush* GetBrush();
        TerrainCommandPanel* GetTerrainCommandPanel();
        SurfaceCommandPanel* GetSurfaceCommandPanel();

        static System::ComponentModel::ComponentResourceManager* 
            GetResourceManager();

        void GetTerrainEntity(MEntity*& pmEntity);
        void SetTerrainEntity(MEntity* pmEntity);
        void SelectClosestTerrain();

        static MTerrainPlugin *GetInstance();

    private:
        //===================================================================
        // Static member variables
        //===================================================================

        /// The terrain brush
        MBrush* m_pmBrush;

        /// The selected terrain entity
        MEntity* m_pmSelectedTerrain;

        /// The terrain picking policy
        MTerrainPickPolicy* m_pmTerrainPickPolicy;

        //===================================================================
        // Dll management
        //===================================================================

        /** 
            Initialises DLLs            
         */
        [DllInit]
        static void InitStatics();

        /** 
            Closes DLLs            
         */
        [DllShutdown]
        static void ShutdownStatics();

        //===================================================================
        // Widget creation functions
        //===================================================================

        /** 
            Adds the toolbar buttons
            
            @param pmUICommandService Service to use for adding the widget
         */
        void AddToolBarButtons(IUICommandService* pmUICommandService);

        //===================================================================
        // ToolBar button handlers
        //===================================================================
       

        [UICommandHandlerAttribute(PONG_TERRAIN_PHYSX_PLUGIN)]
        void OnPongPhysXPlugin(System::Object* pmSender, 
            EventArgs* pmArgs);        

        /** 
            Toolbar handler (detects clicks on the button)
            
            @param pmSender Object sending the request
            @param pmArgs Arguments given for the request
         */
        [UICommandHandlerAttribute(TOGGLE_BRUSH_UI_COMMAND_NAME)]
        void OnToolBarBtnTerrainToggle(System::Object* pmSender, 
            EventArgs* pmArgs);

        /** 
            Toolbar handler (detects clicks on the button)
            
            @param pmSender Object sending the request
            @param pmArgs Arguments given for the request
         */
        [UICommandHandlerAttribute(REBUILD_TERRAIN_LIGHT_COMMAND_NAME)]
        void OnToolBarBtnRebuildLights(System::Object* pmSender, 
            EventArgs* pmArgs);

        /** 
            Toolbar handler (detects clicks on the button)
            
            @param pmSender Object sending the request
            @param pmArgs Arguments given for the request
         */
        [UICommandHandlerAttribute(MOVE_TO_TERRAIN_UI_COMMAND_NAME)]
        void OnToolBarBtnMoveToTerrain(System::Object* pmSender, 
            EventArgs* pmArgs);

        /** 
            Events to display the terrain window
            
            @param pmSender Object sending the request
            @param pmArgs Arguments given for the request
         */
        [UICommandValidatorAttribute(TOGGLE_BRUSH_UI_COMMAND_NAME)]
        void OnValidateTerrainToggle(System::Object* pmSender, 
            UIState* pmState);

        [UICommandValidatorAttribute(REBUILD_TERRAIN_LIGHT_COMMAND_NAME)]
        void OnValidateRebuildLights(System::Object* pmSender, 
            UIState* pmState);

        [UICommandValidatorAttribute(MOVE_TO_TERRAIN_UI_COMMAND_NAME)]
        void OnValidateMoveToTerrain(System::Object* pmSender, 
            UIState* pmState);

        [UICommandHandlerAttribute(ENABLE_BRUSH_UI_COMMAND_NAME)]
        void ActivateInteractionMode(System::Object* pmSender,
            EventArgs* pmArgs);

        //===================================================================
        // Plug in event registration and handlers
        //===================================================================
        void RegisterEvents();

        void SceneClosing(MScene* pmScene);
        void ApplicationClosing();
        void OnLayerRenamed(MLayer* pmLayer, String* strOldFilename);
        void OnLoadingScene(MScene* pmScene);
        void OnLayerAdded(MLayer* pmLayer, MLayer* pmParent);
        void OnEntityPropertyChanged(MEntity* pmEntity, 
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void OnRequestAddCloneToScene(MScene* pmScene, MEntity* pmSourceEntity,
            bool __gc* bDoNotAdd);
        void OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity);
        void OnLayerSaving(MLayer* pmLayer, bool bAutoSave);
        void OnLayerSaved(MLayer* pmLayer, bool bAutoSave);
        void OnDeletedAutoSave(String* pmFilename);
        void OnPaletteAdded(MPalette* pmPalette);
        bool EnsureValidSceneForEntity(MEntity* pmEntity);
        void InitialiseNewSector(NiTerrain* pkTerrain, NiInt16 iIndexX, 
            NiInt16 iIndexY);

        System::Void EntitySelected(MEntity* pmEntity);

        /// @cond EMERGENT_INTERNAL
        /**
            This function checks if the given entity uses a 
            NiTerrainSectorComponent. If it does it will remove it from the 
            entity. It will then ask the user whether this change should be 
            saved or not. It returns the user's answer.
        */
        bool HandleComponentDeprecation(MEntity* pmEntity);
        /// @endcond

        //===================================================================
        // Save Management
        //===================================================================

        /** 
            Call this function on a component to delete any backup folders
            that the terrain may have created. 
            
            @param component the component to clean up. if the parameter is 0
                then all components will have their backup files deleted
         */
        void DeleteBackupFiles(NiTerrainComponent* pkComponent);

        /** 
            Call this function to check the validity of the given archive path.
            It checks for the existence of an archive. If found it will prompt
            the user to replace.

            @param pcArchiveDirectory The full path name to check for the
                existence of an archive.
        */
        bool ValidateArchiveDirectory(const char* pcArchiveDirectory);

        //===================================================================
        // Terrain Loading
        //===================================================================
        /**
            Call this function on a list of entities to check the entities
            for terrain.  If terrain is found, check for deprecated components
            and load the terrain archive.
        */
        void LoadTerrainArchives(MEntity* pkEntities[]);
    };

}}}}
