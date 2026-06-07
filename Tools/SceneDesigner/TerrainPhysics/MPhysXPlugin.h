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

// UI COMMAND NAMES
// Note: Must be the same in the Terrain Plugin
#define PING_TERRAIN_PHYSX_PLUGIN "TerrainPlugin.PingPhysX"
#define PONG_TERRAIN_PHYSX_PLUGIN "TerrainPlugin.PongPhysX"
#define COOK_TERRAIN_PHYSX_DATA "TerrainPlugin.CookPhysxData"

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace PhysXPlugin 
{

    /** 
        The terrain plug-in class interacts withe scene designer to create and
            modify a terrain.
        
     */
    public __gc class MPhysXPlugin : public MDisposable, public IPlugin
    {

    public:
        //===================================================================
        // Constructor
        //===================================================================

        /** 
            Default constructor
            
         */
        MPhysXPlugin();

    private:

        static MPhysXPlugin *ms_pmInstance = 0;             

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

        static System::ComponentModel::ComponentResourceManager* 
            GetResourceManager();

        static MPhysXPlugin *GetInstance();

    private:

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

        [UICommandHandlerAttribute(PING_TERRAIN_PHYSX_PLUGIN)]
        void OnPingPhysXPlugin(System::Object* pmSender, 
            EventArgs* pmArgs);

        [UICommandHandlerAttribute(COOK_TERRAIN_PHYSX_DATA)]
        void OnCookPhysXData(System::Object* pmSender, 
            EventArgs* pmArgs);

        //===================================================================
        // Plug in event registration and handlers
        //===================================================================

        void RegisterEvents();

    };

}}}}
