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
#include "ISettingsService.h"
#include "IOptionsService.h"
#include "IMessageService.h"
#include "MProxyManager.h"

using namespace System::Collections;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI
    ::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MLightManager : public MDisposable, public IProxyHandler
    {
    private public:
        void Startup();

    public:
        static bool EntityIsLight(MEntity* pmEntity);
        MEntity* GetSceneLights()[];

        void AddEntityToDefaultLights(MEntity* pmEntity);
        void AddEntitiesToDefaultLights(MEntity* amEntities[]);
        void RemoveEntityFromDefaultLights(MEntity* pmEntity);
        void RemoveEntitiesFromDefaultLights(MEntity* amEntities[]);

        void Update(float fTime);

        // From IProxyHandler
        virtual MEntity* GetMasterProxyEntity(MEntity* pmEntity);

    private:
        __property bool get_UseDefaultLights();
        __property void set_UseDefaultLights(bool bUseDefaultLights);
        void CreateLightProxyHandler();

        void OnSceneClosing(MScene* pmScene);
        void OnNewSceneLoaded(MScene* pmScene);
        void OnLayerAdded(MLayer* pmLayer, MLayer* pmParent);
        void OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityPropertyChanged(MEntity* pmEntity,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void OnEntityComponentAdded(MEntity* pmEntity,
            MComponent* pmComponent);
        void OnEntityComponentRemoved(MEntity* pmEntity,
            MComponent* pmComponent);

        void RefreshLightAffectedEntities();

        void CreateDefaultLights();
        void DisposeDefaultLights();
        void ClearDefaultLightAffectedEntities();

        void RegisterForDefaultLightsSetting();
        void RegisterForAddEntitiesToLightsSetting();
        void OnDefaultLightsSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);
        void OnAddEntitiesToLightsSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        void CollectSceneLights(MEntity* amAllEntitiesInScene[]);
        void ProcessAddedEntity(MScene* pmScene, MEntity* pmEntity);
        void ProcessRemovedEntity(MScene* pmScene, MEntity* pmEntity);

        ArrayList* m_pmLights;
        MEntity* m_pmMasterProxy;
        MEntity* m_amDefaultLights[];
        bool m_bUseDefaultLights;
        bool m_bAddEntitiesToLights;
        bool m_bUpdateDefaultLights;

        static String* ms_strAffectedEntitiesName = "Affected Entities";
        static String* ms_strCastShadowsName = "Cast Shadows";

        static String* ms_strOptionCategoryName = "Light Settings";

        static String* ms_strDefaultLightsSettingName = "Use Default Lights";
        static String* ms_strDefaultLightsOptionName =
            String::Concat(ms_strOptionCategoryName, ".",
            ms_strDefaultLightsSettingName);

        static String* ms_strAddEntitiesToLightsSettingName =
            "Automatically Add Entities to Lights";
        static String* ms_strAddEntitiesToLightsOptionName =
            String::Concat(ms_strOptionCategoryName, ".",
            ms_strAddEntitiesToLightsSettingName);

        static ISettingsService* ms_pmSettingsService;
        __property static ISettingsService* get_SettingsService();

        static IOptionsService* ms_pmOptionsService;
        __property static IOptionsService* get_OptionsService();

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
        __property static MLightManager* get_Instance();
    private:
        static MLightManager* ms_pmThis = NULL;
        MLightManager();
    };
}}}}
