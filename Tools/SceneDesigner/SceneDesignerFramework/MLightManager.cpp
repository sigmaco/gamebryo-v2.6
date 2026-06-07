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

#include "MLightManager.h"
#include "MFramework.h"
#include "MUtility.h"
#include "ServiceProvider.h"
#include "NiLightProxyComponent.h"
#include "MLayer.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MLightManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MLightManager();
    }
}
//---------------------------------------------------------------------------
void MLightManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MLightManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MLightManager* MLightManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MLightManager::MLightManager() : m_bUseDefaultLights(true),
    m_bAddEntitiesToLights(true), m_bUpdateDefaultLights(false)
{
    m_pmLights = new ArrayList();

    __hook(&MEventManager::SceneClosing, MEventManager::Instance,
        &MLightManager::OnSceneClosing);
    __hook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
        &MLightManager::OnNewSceneLoaded);
    __hook(&MEventManager::LayerAdded, MEventManager::Instance,
        &MLightManager::OnLayerAdded);
    __hook(&MEventManager::EntityAddedToScene, MEventManager::Instance,
        &MLightManager::OnEntityAddedToScene);
    __hook(&MEventManager::EntityRemovedFromScene, MEventManager::Instance,
        &MLightManager::OnEntityRemovedFromScene);
    __hook(&MEventManager::EntityPropertyChanged, MEventManager::Instance,
        &MLightManager::OnEntityPropertyChanged);
    __hook(&MEventManager::EntityComponentAdded, MEventManager::Instance,
        &MLightManager::OnEntityComponentAdded);
    __hook(&MEventManager::EntityComponentRemoved, MEventManager::Instance,
        &MLightManager::OnEntityComponentRemoved);
}
//---------------------------------------------------------------------------
void MLightManager::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        // For some reason, __unhook causes bogus compilation errors here.
        // Thus, some events are unhooked manually.
        __unhook(&MEventManager::SceneClosing, MEventManager::Instance,
            &MLightManager::OnSceneClosing);
        MEventManager::Instance->remove_NewSceneLoaded(new
            MEventManager::__Delegate_NewSceneLoaded(this,
            &MLightManager::OnNewSceneLoaded));
        __unhook(&MEventManager::LayerAdded, MEventManager::Instance,
            &MLightManager::OnLayerAdded);
        MEventManager::Instance->remove_EntityAddedToScene(new
            MEventManager::__Delegate_EntityAddedToScene(this,
            &MLightManager::OnEntityAddedToScene));
        MEventManager::Instance->remove_EntityRemovedFromScene(new
            MEventManager::__Delegate_EntityRemovedFromScene(this,
            &MLightManager::OnEntityRemovedFromScene));
        __unhook(&MEventManager::EntityPropertyChanged,
            MEventManager::Instance, &MLightManager::OnEntityPropertyChanged);
        __unhook(&MEventManager::EntityComponentAdded, MEventManager::Instance,
            &MLightManager::OnEntityComponentAdded);
        MEventManager::Instance->remove_EntityComponentRemoved(new
            MEventManager::__Delegate_EntityComponentRemoved(this,
            &MLightManager::OnEntityComponentRemoved));

        if (m_amDefaultLights != NULL)
        {
            DisposeDefaultLights();
        }
    }
}
//---------------------------------------------------------------------------
void MLightManager::Startup()
{
    MVerifyValidInstance;

    CreateLightProxyHandler();
    MProxyManager::Instance->AddProxyHandler(this);

    CreateDefaultLights();
    RegisterForDefaultLightsSetting();
    RegisterForAddEntitiesToLightsSetting();
}
//---------------------------------------------------------------------------
MEntity* MLightManager::GetMasterProxyEntity(MEntity* pmEntity)
{
    if (MLightManager::EntityIsLight(pmEntity))
    {
        return m_pmMasterProxy;
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MLightManager::RegisterForDefaultLightsSetting()
{
    MVerifyValidInstance;

    SettingsService->RegisterSettingsObject(ms_strDefaultLightsSettingName,
        __box(UseDefaultLights), SettingsCategory::PerUser);
    SettingsService->SetChangedSettingHandler(ms_strDefaultLightsSettingName,
        SettingsCategory::PerUser, new SettingChangedHandler(this,
        &MLightManager::OnDefaultLightsSettingChanged));
    OnDefaultLightsSettingChanged(NULL, NULL);

    OptionsService->AddOption(ms_strDefaultLightsOptionName,
        SettingsCategory::PerUser, ms_strDefaultLightsSettingName);
    OptionsService->SetHelpDescription(ms_strDefaultLightsOptionName,
        "Indicates whether or not default lights will affect scene entities "
        "when no other lights exist in the scene.");
}
//---------------------------------------------------------------------------
void MLightManager::RegisterForAddEntitiesToLightsSetting()
{
    MVerifyValidInstance;

    SettingsService->RegisterSettingsObject(
        ms_strAddEntitiesToLightsSettingName, __box(m_bAddEntitiesToLights),
        SettingsCategory::PerUser);
    SettingsService->SetChangedSettingHandler(
        ms_strAddEntitiesToLightsSettingName, SettingsCategory::PerUser,
        new SettingChangedHandler(this,
        &MLightManager::OnAddEntitiesToLightsSettingChanged));
    OnAddEntitiesToLightsSettingChanged(NULL, NULL);

    OptionsService->AddOption(ms_strAddEntitiesToLightsOptionName,
        SettingsCategory::PerUser, ms_strAddEntitiesToLightsSettingName);
    OptionsService->SetHelpDescription(ms_strAddEntitiesToLightsOptionName,
        "Indicates whether or not entities added to the scene will be "
        "automatically affected by all lights in the scene.");
}
//---------------------------------------------------------------------------
void MLightManager::OnDefaultLightsSettingChanged(Object*,
    SettingChangedEventArgs*)
{
    MVerifyValidInstance;

    __box bool* pbUseDefaultLights = dynamic_cast<__box bool*>(
        SettingsService->GetSettingsObject(ms_strDefaultLightsSettingName,
        SettingsCategory::PerUser));
    if (pbUseDefaultLights != NULL)
    {
        UseDefaultLights = *pbUseDefaultLights;
    }
}
//---------------------------------------------------------------------------
void MLightManager::OnAddEntitiesToLightsSettingChanged(Object*,
    SettingChangedEventArgs*)
{
    MVerifyValidInstance;

    __box bool* pbAddEntitiesToLights = dynamic_cast<__box bool*>(
        SettingsService->GetSettingsObject(
        ms_strAddEntitiesToLightsSettingName, SettingsCategory::PerUser));
    if (pbAddEntitiesToLights != NULL)
    {
        m_bAddEntitiesToLights = *pbAddEntitiesToLights;
    }
}
//---------------------------------------------------------------------------
void MLightManager::CreateLightProxyHandler()
{
    m_pmMasterProxy = NULL;

    NiDefaultErrorHandlerPtr spErrors;

    NiUniqueID kTemplateID;
    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiEntityInterfaceIPtr spDummyEntity = NULL;

    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiEntityInterfaceIPtr spEntity = NiNew NiGeneralEntity("Light Proxy",
        kTemplateID, 2);
    bool bSuccess = NIBOOL_IS_TRUE(spEntity->AddComponent(NiNew 
        NiLightProxyComponent(spDummyEntity, NULL)));
    if (bSuccess)
    {
        const char* pcAppStartupPath = 
            MStringToCharPointer(MFramework::Instance->AppStartupPath);

        char acAbsGeomPath[NI_MAX_PATH];
        size_t stSize = 
            NiPath::ConvertToAbsolute(acAbsGeomPath, NI_MAX_PATH, 
            "..\\..\\Data\\light.nif", pcAppStartupPath);
        NI_UNUSED_ARG(stSize);
        NIASSERT(stSize > 0);

        bSuccess = NIBOOL_IS_TRUE(spEntity->AddComponent(NiNew
            NiSceneGraphComponent(acAbsGeomPath)));
        if (bSuccess)
        {
            m_pmMasterProxy = MEntityFactory::Instance->Get(spEntity);
            m_pmMasterProxy->SetPropertyData("Source Entity", NULL,
                false);
        }
        else
        {
            if (!spErrors)
            {
                spErrors = NiNew NiDefaultErrorHandler(2);
            }
            spErrors->ReportError("Error creating light proxy entity;"
                " light proxy geometry will not be shown.", NULL, NULL, 
                NULL);
        }
        MFreeCharPointer(pcAppStartupPath);
    }
    else
    {
        if (!spErrors)
        {
            spErrors = NiNew NiDefaultErrorHandler(2);
        }
        spErrors->ReportError("Error creating light proxy entity; light "
            "proxy geometry will not be shown.", NULL, NULL, NULL);
    }

    if (spErrors)
    {
        MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
            spErrors);
    }
}
//---------------------------------------------------------------------------
bool MLightManager::EntityIsLight(MEntity* pmEntity)
{
    MAssert(pmEntity != NULL, "Null entity provided to function!");

    NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(0);
    if (pkSceneRoot != NULL && NiIsKindOf(NiLight, pkSceneRoot))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
MEntity* MLightManager::GetSceneLights()[]
{
    return static_cast<MEntity*[]>(m_pmLights->ToArray(__typeof(MEntity)));
}
//---------------------------------------------------------------------------
void MLightManager::OnSceneClosing(MScene* pmScene)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene == pmScene)
    {
        m_pmLights->Clear();
        ClearDefaultLightAffectedEntities();
    }
}
//---------------------------------------------------------------------------
void MLightManager::OnNewSceneLoaded(MScene* pmScene)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene == pmScene)
    {
        CollectSceneLights(pmScene->GetEntities());
        if (m_pmLights->Count == 0 && UseDefaultLights)
        {
            // If there are no lights in the scene, make all scene entities
            // get affected by the default lights.
            AddEntitiesToDefaultLights(pmScene->GetEntities());
        }
    }
}
//---------------------------------------------------------------------------
void MLightManager::OnLayerAdded(MLayer* pmLayer, MLayer*)
{
    MVerifyValidInstance;

    CollectSceneLights(pmLayer->GetEntities());
    if (m_pmLights->Count == 0 && UseDefaultLights)
    {
        // If there are no lights in the scene, make all scene entities
        // get affected by the default lights.
        AddEntitiesToDefaultLights(pmLayer->GetEntities());
    }
}
//---------------------------------------------------------------------------
void MLightManager::OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene == pmScene)
    {
        ProcessAddedEntity(pmScene, pmEntity);
    }
}
//---------------------------------------------------------------------------
void MLightManager::OnEntityRemovedFromScene(MScene* pmScene,
    MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene == pmScene)
    {
        ProcessRemovedEntity(pmScene, pmEntity);
    }
}
//---------------------------------------------------------------------------
void MLightManager::OnEntityPropertyChanged(MEntity* pmEntity,
    String* strPropertyName, unsigned int uiPropertyIndex, bool)
{
    MVerifyValidInstance;

    if (pmEntity->IsExternalAssetPath(strPropertyName, uiPropertyIndex))
    {
        // Perform an initial update here to force the entity to load
        // the external asset.
        if (MFramework::Instance->Scene->IsEntityInScene(pmEntity))
        {
            pmEntity->Update(MTimeManager::Instance->CurrentTime,
                MFramework::Instance->ExternalAssetManager);
        }
        else
        {
            MEntity* amDependentEntities[] = MFramework::Instance->Scene
                ->GetDependentEntities(pmEntity);
            for (int i = 0; i < amDependentEntities->Length; i++)
            {
                MEntity* pmDependentEntity = amDependentEntities[i];
                if (MFramework::Instance->Scene->IsEntityInScene(
                    pmDependentEntity))
                {
                    pmDependentEntity->Update(
                        MTimeManager::Instance->CurrentTime,
                        MFramework::Instance->ExternalAssetManager);
                }
            }
        }

        // Refresh each light's affected entities list so that it will
        // affect the new external asset.
        RefreshLightAffectedEntities();
    }
}
//---------------------------------------------------------------------------
void MLightManager::OnEntityComponentAdded(MEntity* pmEntity,
    MComponent* pmComponent)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene->IsEntityInScene(pmEntity))
    {
        if (pmComponent->HasProperty(ms_strAffectedEntitiesName))
        {
            // Ensure that entity has been updated with new component.
            pmEntity->Update(MTimeManager::Instance->CurrentTime,
                MFramework::Instance->ExternalAssetManager);

            // Process the entity as if it has just been added to the scene.
            ProcessAddedEntity(MFramework::Instance->Scene, pmEntity);
        }
        else if (pmComponent->HasProperty(ms_strCastShadowsName))
        {
            // Force the shadow generator to recreate itself (to properly
            // support undo functionality).
            Object* pbActive = pmComponent->GetPropertyData(
                ms_strCastShadowsName);
            pmComponent->SetPropertyData(ms_strCastShadowsName, __box(false),
                true);
            pmComponent->SetPropertyData(ms_strCastShadowsName, pbActive,
                true);
        }
    }
}
//---------------------------------------------------------------------------
void MLightManager::OnEntityComponentRemoved(MEntity* pmEntity,
    MComponent* pmComponent)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene->IsEntityInScene(pmEntity))
    {
        if (pmComponent->HasProperty(ms_strAffectedEntitiesName))
        {
            // Remove entity from light array.
            m_pmLights->Remove(pmEntity);

            // Remove all affected entities for the light.
            for (unsigned int ui = 0; ui < pmComponent->GetElementCount(
                ms_strAffectedEntitiesName); ui++)
            {
                pmComponent->SetPropertyData(ms_strAffectedEntitiesName, NULL,
                    ui, true);
            }

            if (m_pmLights->Count == 0 && UseDefaultLights)
            {
                // If this light was the last light in the scene, make all
                // scene entities get affected by the default lights.
                AddEntitiesToDefaultLights(
                    MFramework::Instance->Scene->GetEntities());
            }
        }
        else if (pmComponent->HasProperty(ms_strCastShadowsName))
        {
            // Disable shadow generator.
            pmComponent->SetPropertyData(ms_strCastShadowsName, __box(false),
                true);
        }
    }
}
//---------------------------------------------------------------------------
void MLightManager::RefreshLightAffectedEntities()
{
    MVerifyValidInstance;

    if (m_bUseDefaultLights && m_pmLights->Count == 0)
    {
        for (int i = 0; i < m_amDefaultLights->Length; i++)
        {
            MEntity* pmDefaultLight = m_amDefaultLights[i];

            unsigned int uiAffectedEntitiesCount = pmDefaultLight
                ->GetElementCount(ms_strAffectedEntitiesName);
            for (unsigned int ui = 0; ui < uiAffectedEntitiesCount; ui++)
            {
                MEntity* pkAffectedEntity = dynamic_cast<MEntity*>(
                    pmDefaultLight->GetPropertyData(
                    ms_strAffectedEntitiesName, ui));
                if (pkAffectedEntity != NULL)
                {
                    pmDefaultLight->SetPropertyData(
                        ms_strAffectedEntitiesName, NULL, ui, false);
                    pmDefaultLight->SetPropertyData(
                        ms_strAffectedEntitiesName, pkAffectedEntity, ui,
                        false);
                    break;
                }
            }
        }
        m_bUpdateDefaultLights = true;
    }
    else
    {
        for (int i = 0; i < m_pmLights->Count; i++)
        {
            MEntity* pmLight = dynamic_cast<MEntity*>(m_pmLights->Item[i]);
            MAssert(pmLight != NULL, "Invalid light array!");

            // Refresh affected entities.
            unsigned int uiAffectedEntitiesCount = pmLight->GetElementCount(
                ms_strAffectedEntitiesName);
            for (unsigned int ui = 0; ui < uiAffectedEntitiesCount; ui++)
            {
                MEntity* pkAffectedEntity = dynamic_cast<MEntity*>(
                    pmLight->GetPropertyData(ms_strAffectedEntitiesName, ui));
                if (pkAffectedEntity != NULL)
                {
                    pmLight->SetPropertyData(ms_strAffectedEntitiesName, NULL,
                        ui, false);
                    pmLight->SetPropertyData(ms_strAffectedEntitiesName,
                        pkAffectedEntity, ui, false);
                    break;
                }
            }

            // Force the shadow generator to recreate itself, if the light
            // has one.
            if (pmLight->HasProperty(ms_strCastShadowsName))
            {
                Object* pbActive = pmLight->GetPropertyData(
                    ms_strCastShadowsName);
                pmLight->SetPropertyData(ms_strCastShadowsName, __box(false),
                    true);
                pmLight->SetPropertyData(ms_strCastShadowsName, pbActive,
                    true);
            }

            pmLight->Update(0.0f, MFramework::Instance->ExternalAssetManager);
        }
    }
}
//---------------------------------------------------------------------------
bool MLightManager::get_UseDefaultLights()
{
    MVerifyValidInstance;

    return m_bUseDefaultLights;
}
//---------------------------------------------------------------------------
void MLightManager::set_UseDefaultLights(bool bUseDefaultLights)
{
    MVerifyValidInstance;

    if (m_bUseDefaultLights != bUseDefaultLights)
    {
        m_bUseDefaultLights = bUseDefaultLights;
        if (m_bUseDefaultLights)
        {
            if (m_pmLights->Count == 0)
            {
                // If there are no lights in the scene, make all scene
                // entities get affected by the default lights.
                AddEntitiesToDefaultLights(MFramework::Instance->Scene
                    ->GetEntities());
            }
        }
        else
        {
            ClearDefaultLightAffectedEntities();
        }
    }
}
//---------------------------------------------------------------------------
void MLightManager::CreateDefaultLights()
{
    MVerifyValidInstance;

    // MEntityFactory is explicitly not used to create default light entities
    // because they are entirely internal and will be disposed of manually.

    NiFixedString kDefaultLightName = "Default Light";
    NiFixedString kRotationName = "Rotation";

    NiUniqueID kTemplateID;
    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiEntityInterface* pkDefaultLight = NiNew NiGeneralEntity(
        kDefaultLightName, kTemplateID, 2);
    pkDefaultLight->AddComponent(NiNew NiTransformationComponent());
    pkDefaultLight->AddComponent(NiNew NiLightComponent(
        NiLightComponent::LT_DIRECTIONAL, 1.0f, NiColor::BLACK,
        NiColor::WHITE, NiColor::WHITE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 100,
        100));

    m_amDefaultLights = new MEntity*[2];
    int iIndex = 0;

    NiMatrix3 kRotation(
        NiPoint3(0.44663f, 0.496292f, -0.744438f),
        NiPoint3(0.0f, -0.83205f, -0.5547f),
        NiPoint3(-0.894703f, 0.247764f, -0.371646f));
    pkDefaultLight->SetPropertyData(kRotationName, kRotation, 0);
    m_amDefaultLights[iIndex++] = MEntityFactory::Instance->Get(
        pkDefaultLight);
    
    NiEntityInterface* pkDefaultLightClone = pkDefaultLight->Clone(
        kDefaultLightName, false);
    kRotation.SetCol(0, NiPoint3(-0.44663f, -0.496292f, 0.744438f));
    kRotation.SetCol(1, NiPoint3(0.0f, 0.83205f, 0.5547f));
    kRotation.SetCol(2, NiPoint3(-0.894703f, 0.247764f, -0.371646f));
    pkDefaultLightClone->SetPropertyData(kRotationName, kRotation, 0);
    m_amDefaultLights[iIndex++] = MEntityFactory::Instance->Get(
        pkDefaultLightClone);

    m_bUpdateDefaultLights = true;
}
//---------------------------------------------------------------------------
void MLightManager::DisposeDefaultLights()
{
    MVerifyValidInstance;

    MAssert(m_amDefaultLights != NULL, "Null default lights array!");

    for (int i = 0; i < m_amDefaultLights->Length; i++)
    {
        m_amDefaultLights[i]->Dispose();
    }
}
//---------------------------------------------------------------------------
void MLightManager::ClearDefaultLightAffectedEntities()
{
    MVerifyValidInstance;

    MAssert(m_amDefaultLights != NULL, "Null default lights array!");

    for (int i = 0; i < m_amDefaultLights->Length; i++)
    {
        MEntity* pmDefaultLight = m_amDefaultLights[i];

        unsigned int uiAffectedEntitiesCount = pmDefaultLight
            ->GetElementCount(ms_strAffectedEntitiesName);
        for (unsigned int ui = 0; ui < uiAffectedEntitiesCount; ui++)
        {
            pmDefaultLight->SetPropertyData(ms_strAffectedEntitiesName,
                NULL, ui, false);
        }
    }
    m_bUpdateDefaultLights = true;
}
//---------------------------------------------------------------------------
void MLightManager::AddEntityToDefaultLights(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");
    MAssert(m_amDefaultLights != NULL, "Null default lights array!");

    for (int i = 0; i < m_amDefaultLights->Length; i++)
    {
        MEntity* pmDefaultLight = m_amDefaultLights[i];

        unsigned int uiAffectedEntitiesCount = pmDefaultLight
            ->GetElementCount(ms_strAffectedEntitiesName);
        pmDefaultLight->SetPropertyData(ms_strAffectedEntitiesName,
            pmEntity, uiAffectedEntitiesCount, false);
    }
    m_bUpdateDefaultLights = true;
}
//---------------------------------------------------------------------------
void MLightManager::AddEntitiesToDefaultLights(MEntity* amEntities[])
{
    MVerifyValidInstance;

    MAssert(amEntities != NULL, "Null array provided to function!");
    MAssert(m_amDefaultLights != NULL, "Null default lights array!");

    for (int i = 0; i < m_amDefaultLights->Length; i++)
    {
        MEntity* pmDefaultLight = m_amDefaultLights[i];

        unsigned int uiIndex = pmDefaultLight->GetElementCount(
            ms_strAffectedEntitiesName);
        for (int j = 0; j < amEntities->Length; j++)
        {
            MEntity* pmEntity = amEntities[j];
            if (!MCameraManager::EntityIsCamera(pmEntity))
            {
                pmDefaultLight->SetPropertyData(ms_strAffectedEntitiesName,
                    pmEntity, uiIndex++, false);
            }
        }
    }
    m_bUpdateDefaultLights = true;
}
//---------------------------------------------------------------------------
void MLightManager::RemoveEntityFromDefaultLights(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");
    MAssert(m_amDefaultLights != NULL, "Null default lights array!");

    for (int i = 0; i < m_amDefaultLights->Length; i++)
    {
        MEntity* pmDefaultLight = m_amDefaultLights[i];

        unsigned int uiAffectedEntitiesCount = pmDefaultLight
            ->GetElementCount(ms_strAffectedEntitiesName);
        for (unsigned int ui = 0; ui < uiAffectedEntitiesCount; ui++)
        {
            if (pmDefaultLight->GetPropertyData(ms_strAffectedEntitiesName,
                ui) == pmEntity)
            {
                pmDefaultLight->SetPropertyData(ms_strAffectedEntitiesName,
                    NULL, ui, false);
                break;
            }
        }
    }
    m_bUpdateDefaultLights = true;
}
//---------------------------------------------------------------------------
void MLightManager::RemoveEntitiesFromDefaultLights(MEntity* amEntities[])
{
    MVerifyValidInstance;

    MAssert(amEntities != NULL, "Null array provided to function!");
    MAssert(m_amDefaultLights != NULL, "Null default lights array!");

    for (int i = 0; i < m_amDefaultLights->Length; i++)
    {
        MEntity* pmDefaultLight = m_amDefaultLights[i];

        for (int j = 0; j < amEntities->Length; j++)
        {
            MEntity* pmEntity = amEntities[j];
            unsigned int uiAffectedEntitiesCount = pmDefaultLight
                ->GetElementCount(ms_strAffectedEntitiesName);
            for (unsigned int ui = 0; ui < uiAffectedEntitiesCount; ui++)
            {
                if (pmDefaultLight->GetPropertyData(ms_strAffectedEntitiesName,
                    ui) == pmEntity)
                {
                    pmDefaultLight->SetPropertyData(ms_strAffectedEntitiesName,
                        NULL, ui, false);
                    break;
                }
            }
        }
    }
    m_bUpdateDefaultLights = true;
}
//---------------------------------------------------------------------------
void MLightManager::Update(float fTime)
{
    MVerifyValidInstance;

    if (m_bUpdateDefaultLights)
    {
        for (int i = 0; i < m_amDefaultLights->Length; i++)
        {
            m_amDefaultLights[i]->Update(fTime, MFramework::Instance
                ->ExternalAssetManager);
        }
        m_bUpdateDefaultLights = false;
    }
}
//---------------------------------------------------------------------------
void MLightManager::CollectSceneLights(MEntity* amAllEntitiesInScene[])
{
    MVerifyValidInstance;

    for (int i = 0; i < amAllEntitiesInScene->Length; i++)
    {
        MEntity* pmEntity = amAllEntitiesInScene[i];
        if (EntityIsLight(pmEntity))
        {
            m_pmLights->Add(pmEntity);
        }
    }
}
//---------------------------------------------------------------------------
void MLightManager::ProcessAddedEntity(MScene* pmScene, MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmScene != NULL, "Null scene provided to function!");
    MAssert(pmEntity != NULL, "Null entity provided to function!");

    if (EntityIsLight(pmEntity))
    {
        if (m_pmLights->Count == 0 && UseDefaultLights)
        {
            // If there are currently no lights, remove all affected entities
            // from the default lights.
            ClearDefaultLightAffectedEntities();
        }

        // Add entity to light array.
        m_pmLights->Add(pmEntity);

        if (m_bAddEntitiesToLights)
        {
            MAssert(pmEntity->HasProperty(ms_strAffectedEntitiesName),
                "Light does not have affected entities property!");

            // Remove all affected entities for the light.
            unsigned int uiAffectedEntitiesCount = pmEntity->GetElementCount(
                ms_strAffectedEntitiesName);
            for (unsigned int ui = 0; ui < uiAffectedEntitiesCount; ui++)
            {
                pmEntity->SetPropertyData(ms_strAffectedEntitiesName, NULL,
                    ui, true);
            }

            // Add all scene entities as affected entities for the light.
            MEntity* amSceneEntities[] = pmScene->GetEntities();
            unsigned int uiIndex = 0;
            for (int i = 0; i < amSceneEntities->Length; i++)
            {
                MEntity* pmSceneEntity = amSceneEntities[i];
                if (!EntityIsLight(pmSceneEntity) &&
                    !MCameraManager::EntityIsCamera(pmSceneEntity))
                {
                    pmEntity->SetPropertyData(ms_strAffectedEntitiesName,
                        pmSceneEntity, uiIndex++, true);
                }
            }
        }
    }
    else if (!MCameraManager::EntityIsCamera(pmEntity))
    {
        if (m_pmLights->Count == 0 && UseDefaultLights)
        {
            // If there are no lights in the scene, the default lights should
            // affect the entity.
            AddEntityToDefaultLights(pmEntity);
        }
        else if (m_bAddEntitiesToLights)
        {
            if (pmEntity->Writable)
            {
                // If lights exist in the scene, add this entity to the
                // affected entities lists for each light.
                for (int i = 0; i < m_pmLights->Count; i++)
                {
                    MEntity* pmLight = dynamic_cast<MEntity*>(
                        m_pmLights->Item[i]);
                    MAssert(pmLight != NULL, "Invalid light in array!");

                    if (!pmLight->Writable)
                    {
                        MessageService->AddMessage(MessageChannelType::Errors,
                            new Emergent::Gamebryo::SceneDesigner::PluginAPI
                            ::Message("Cannot add entity to light.",
                            String::Format("The \"{0}\" light is not writable,"
                            " so it cannot be set to affect the \"{1}\" "
                            "entity.", pmLight->Name, pmEntity->Name),
                            pmLight));
                        continue;
                    }

                    unsigned int uiAffectedEntitiesCount = pmLight
                        ->GetElementCount(ms_strAffectedEntitiesName);
                    unsigned int uiIndex;
                    for (uiIndex = 0; uiIndex < uiAffectedEntitiesCount;
                        uiIndex++)
                    {
                        if (pmLight->GetPropertyData(
                            ms_strAffectedEntitiesName, uiIndex) == NULL)
                        {
                            break;
                        }
                    }
                    pmLight->SetPropertyData(ms_strAffectedEntitiesName,
                        pmEntity, uiIndex, true);
                }
            }
            else
            {
                MessageService->AddMessage(MessageChannelType::Errors,
                    new Emergent::Gamebryo::SceneDesigner::PluginAPI::Message(
                    "Cannot add entity to lights.", String::Format("The "
                    "\"{0}\" entity is not writable and cannot be added to "
                    "the lights in the scene.", pmEntity->Name), pmEntity));
            }
        }
    }
}
//---------------------------------------------------------------------------
void MLightManager::ProcessRemovedEntity(MScene* pmScene, MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (EntityIsLight(pmEntity))
    {
        // Remove from light array.
        m_pmLights->Remove(pmEntity);

        MAssert(pmEntity->HasProperty(ms_strAffectedEntitiesName),
            "Light does not have affected entities property!");

        // Remove all affected entities for the light.
        for (unsigned int ui = 0; ui < pmEntity->GetElementCount(
            ms_strAffectedEntitiesName); ui++)
        {
            pmEntity->SetPropertyData(ms_strAffectedEntitiesName, NULL,
                ui, true);
        }

        // Disable the shadow generator, if there is one.
        if (pmEntity->HasProperty(ms_strCastShadowsName))
        {
            pmEntity->SetPropertyData(ms_strCastShadowsName, __box(false),
                true);
        }

        if (m_pmLights->Count == 0 && UseDefaultLights)
        {
            // If this light was the last light in the scene, make all scene
            // entities get affected by the default lights.
            AddEntitiesToDefaultLights(pmScene->GetEntities());
        }
    }
    else if (!MCameraManager::EntityIsCamera(pmEntity) &&
        m_pmLights->Count == 0 && UseDefaultLights)
    {
        // If there are no lights in the scene, remove the entity from the
        // default lights' affected entities lists.
        RemoveEntityFromDefaultLights(pmEntity);
    }

    // Update removed entity to ensure that affected nodes are updated.
    pmEntity->Update(MTimeManager::Instance->CurrentTime,
        MFramework::Instance->ExternalAssetManager);
}
//---------------------------------------------------------------------------
ISettingsService* MLightManager::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found!");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MLightManager::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not found!");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
IMessageService* MLightManager::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
