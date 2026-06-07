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
#include "MViewport.h"
#include "ICommandService.h"

using namespace System::Collections;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc  __interface IProxyHandler 
    {
    public:
        MEntity* GetMasterProxyEntity(MEntity* pmEntity);
    };

    public __gc class MProxyManager : public MDisposable
    {
    private public:
        void Startup();

    public:
        // Sets the scale of each proxy and updates them based off the 
        // active camera in the viewport argument
        void UpdateProxyScales(MViewport* pmViewport);

        // Creates a proxy for the specified entity. This function may return
        // NULL if no proxy could be created for the entity.
        MEntity* CreateProxy(MEntity* pmEntity);

        // Adds the specified proxy for the specified entity. The proxy should
        // have been previously created with CreateProxy.
        void AddProxy(MEntity* pmProxy, MEntity* pmEntity);

        // Creates a proxy for the entity and adds it to the proxy scene.
        void CreateAndAddProxy(MEntity* pmEntity);

        // Removes the proxy for the entity, if one exists.
        void RemoveProxy(MEntity* pmEntity);

        // Returns the proxy for the specified entity, if one exists. This
        // function will return NULL if no proxy exists for the entity.
        MEntity* GetProxyForEntity(MEntity* pmEntity);

        // Returns a pointer to a scene containing all the proxies.
        __property MScene* get_ProxyScene();

        // Add a proxy handler interface to the list of handlers
        void AddProxyHandler(IProxyHandler* pHandler);
        void RemoveProxyHandler(IProxyHandler* pHandler);
        IProxyHandler* GetProxyArray()[];

        // Used by external classes to create a proxy entity
        // that has a scene graph
        static MEntity* CreateGenericMasterProxyEntity(
            String* pmFilename, String* pmEntityName, 
            bool bSizeRelativeToCamera);

    private:
        void InitProxies();
        void BuildProxyScene(MEntity* amSceneEntities[]);

        void OnSceneClosing(MScene* pmScene);
        void OnNewSceneLoaded(MScene* pmScene);
        void OnLayerAdded(MLayer* pmLayer, MLayer* pmParent);
        void OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityComponentAdded(MEntity* pmEntity,
            MComponent* pmComponent);
        void OnEntityComponentRemoved(MEntity* pmEntity,
            MComponent* pmComponent);
        void OnEntityHiddenStateChanged(MEntity* pmEntity, bool bHidden);
        void OnEntityFrozenStateChanged(MEntity* pmEntity, bool bFrozen);

        void ProcessHiddenStateChange(MEntity* pmEntity, bool bHidden);
        void ProcessFrozenStateChange(MEntity* pmEntity, bool bFrozen);

        Hashtable* m_pmEntityToProxy;
        ArrayList* m_pmProxyHandlers;
        MScene* m_pmProxyScene;

        static String* ms_strSourceEntityName = "Source Entity";

        static ICommandService* ms_pmCommandService;
        __property static ICommandService* get_CommandService();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MProxyManager* get_Instance();
    private:
        static MProxyManager* ms_pmThis = NULL;
        MProxyManager();
    };
}}}}
