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

#include "MProxyManager.h"
#include "MFramework.h"
#include "MUtility.h"
#include "ServiceProvider.h"
#include "NiProxyComponent.h"
#include "NiLightProxyComponent.h"
#include "MLayer.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MProxyManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MProxyManager();
    }
}
//---------------------------------------------------------------------------
void MProxyManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MProxyManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MProxyManager* MProxyManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MProxyManager::MProxyManager()
{
    m_pmProxyHandlers = new ArrayList();
}
//---------------------------------------------------------------------------
void MProxyManager::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        m_pmProxyHandlers->Clear();

        if (m_pmProxyScene != NULL)
        {
            MSceneFactory::Instance->Remove(m_pmProxyScene);
            m_pmProxyScene = NULL;
        }


        // For some reason, __unhook causes bogus compilation errors here.
        // Thus, the events are unhooked manually.
        MEventManager::Instance->remove_SceneClosing(new
            MEventManager::__Delegate_SceneClosing(this,
            &MProxyManager::OnSceneClosing));
        MEventManager::Instance->remove_NewSceneLoaded(new
            MEventManager::__Delegate_NewSceneLoaded(this,
            &MProxyManager::OnNewSceneLoaded));
        __unhook(&MEventManager::LayerAdded, MEventManager::Instance,
            &MProxyManager::OnLayerAdded);
        MEventManager::Instance->remove_EntityAddedToScene(new
            MEventManager::__Delegate_EntityAddedToScene(this,
            &MProxyManager::OnEntityAddedToScene));
        MEventManager::Instance->remove_EntityRemovedFromScene(new
            MEventManager::__Delegate_EntityRemovedFromScene(this,
            &MProxyManager::OnEntityRemovedFromScene));
        MEventManager::Instance->remove_EntityComponentAdded(new
            MEventManager::__Delegate_EntityComponentAdded(this,
            &MProxyManager::OnEntityComponentAdded));
        MEventManager::Instance->remove_EntityComponentRemoved(new
            MEventManager::__Delegate_EntityComponentRemoved(this,
            &MProxyManager::OnEntityComponentRemoved));
        MEventManager::Instance->remove_EntityHiddenStateChanged(new
            MEventManager::__Delegate_EntityHiddenStateChanged(this,
            &MProxyManager::OnEntityHiddenStateChanged));
        MEventManager::Instance->remove_EntityFrozenStateChanged(new
            MEventManager::__Delegate_EntityFrozenStateChanged(this,
            &MProxyManager::OnEntityFrozenStateChanged));
    }

}
//---------------------------------------------------------------------------
void MProxyManager::Startup()
{
    MVerifyValidInstance;

    InitProxies();

    __hook(&MEventManager::SceneClosing, MEventManager::Instance,
        &MProxyManager::OnSceneClosing);
    __hook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
        &MProxyManager::OnNewSceneLoaded);
    __hook(&MEventManager::LayerAdded, MEventManager::Instance,
        &MProxyManager::OnLayerAdded);
    __hook(&MEventManager::EntityAddedToScene, MEventManager::Instance,
        &MProxyManager::OnEntityAddedToScene);
    __hook(&MEventManager::EntityRemovedFromScene, MEventManager::Instance,
        &MProxyManager::OnEntityRemovedFromScene);
    __hook(&MEventManager::EntityComponentAdded, MEventManager::Instance,
        &MProxyManager::OnEntityComponentAdded);
    __hook(&MEventManager::EntityComponentRemoved, MEventManager::Instance,
        &MProxyManager::OnEntityComponentRemoved);
    __hook(&MEventManager::EntityHiddenStateChanged, MEventManager::Instance,
        &MProxyManager::OnEntityHiddenStateChanged);
    __hook(&MEventManager::EntityFrozenStateChanged, MEventManager::Instance,
        &MProxyManager::OnEntityFrozenStateChanged);
}
//---------------------------------------------------------------------------
void MProxyManager::InitProxies()
{
    MVerifyValidInstance;

    m_pmEntityToProxy = new Hashtable();
    m_pmProxyScene = MSceneFactory::Instance->Get(NiNew NiScene("Proxy Scene",
        10));
}
//---------------------------------------------------------------------------
void MProxyManager::BuildProxyScene(MEntity* amSceneEntities[])
{
    MVerifyValidInstance;

    CommandService->BeginUndoFrame("Rebuild scene proxies");
    for (int i = 0; i < amSceneEntities->Length; i++)
    {
        CreateAndAddProxy(amSceneEntities[i]);
    }
    CommandService->EndUndoFrame(false);
}
//---------------------------------------------------------------------------
void MProxyManager::OnSceneClosing(MScene* pmScene)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene == pmScene)
    {
        MFramework::Instance->SceneFactory->Remove(m_pmProxyScene);
        InitProxies();
    }
}
//---------------------------------------------------------------------------
void MProxyManager::OnNewSceneLoaded(MScene* pmScene)
{
    MVerifyValidInstance;

    if (pmScene == MFramework::Instance->Scene)
    {
        BuildProxyScene(pmScene->GetEntities());
    }
}
//---------------------------------------------------------------------------
void MProxyManager::OnLayerAdded(MLayer* pmLayer, MLayer*)
{
    MVerifyValidInstance;
    BuildProxyScene(pmLayer->GetEntities());
}
//---------------------------------------------------------------------------
void MProxyManager::OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmScene == MFramework::Instance->Scene)
    {
        CreateAndAddProxy(pmEntity);
    }
}
//---------------------------------------------------------------------------
void MProxyManager::OnEntityRemovedFromScene(MScene* pmScene,
    MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmScene == MFramework::Instance->Scene)
    {
        RemoveProxy(pmEntity);
    }
}
//---------------------------------------------------------------------------
void MProxyManager::OnEntityComponentAdded(MEntity* pmEntity,
    MComponent*)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene->IsEntityInScene(pmEntity))
    {
        RemoveProxy(pmEntity);
        CreateAndAddProxy(pmEntity);
    }
}
//---------------------------------------------------------------------------
void MProxyManager::OnEntityComponentRemoved(MEntity* pmEntity,
    MComponent*)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene->IsEntityInScene(pmEntity))
    {
        RemoveProxy(pmEntity);
        CreateAndAddProxy(pmEntity);
    }
}
//---------------------------------------------------------------------------
void MProxyManager::OnEntityHiddenStateChanged(MEntity* pmEntity,
    bool bHidden)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene->IsEntityInScene(pmEntity))
    {
        ProcessHiddenStateChange(pmEntity, bHidden);
    }
}
//---------------------------------------------------------------------------
void MProxyManager::OnEntityFrozenStateChanged(MEntity* pmEntity,
    bool bFrozen)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene->IsEntityInScene(pmEntity))
    {
        ProcessFrozenStateChange(pmEntity, bFrozen);
    }
}
//---------------------------------------------------------------------------
void MProxyManager::ProcessHiddenStateChange(MEntity* pmEntity, bool)
{
    MVerifyValidInstance;

    MEntity* pmProxy = GetProxyForEntity(pmEntity);
    if (pmProxy != NULL)
    {
        pmProxy->Hidden = pmEntity->Hidden;
    }
}
//---------------------------------------------------------------------------
void MProxyManager::ProcessFrozenStateChange(MEntity* pmEntity, bool)
{
    MVerifyValidInstance;

    MEntity* pmProxy = GetProxyForEntity(pmEntity);
    if (pmProxy != NULL)
    {
        pmProxy->Frozen = pmEntity->Frozen;
    }
}
//---------------------------------------------------------------------------
void MProxyManager::UpdateProxyScales(MViewport* pmViewport)
{
    NiCamera* pkCamera = pmViewport->GetNiCamera();
    // iterate through all known proxy components and call UpdateScale
    MEntity* amProxies[] = m_pmProxyScene->GetEntities();
    for (int i = 0; i < amProxies->Count; i++)
    {
        MComponent* amComponents[] = amProxies[i]->GetComponents();
        for (int j = 0; j < amComponents->Count; j++)
        {
            NiEntityComponentInterface* pkComponent =  
                amComponents[j]->GetNiEntityComponentInterface();
            if ((pkComponent->GetClassName() == NiProxyComponent::ClassName())
                || (pkComponent->GetClassName() == 
                NiLightProxyComponent::ClassName()))
            {
                NiProxyComponent* pkProxy = (NiProxyComponent*)pkComponent;
                if (pkProxy)
                {
                    pkProxy->UpdateScale(pkCamera);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
MEntity* MProxyManager::CreateProxy(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    MEntity* pmMasterProxy;
    for (int i = 0; i < m_pmProxyHandlers->get_Count(); i++)
    {
        IProxyHandler* pkHandler = dynamic_cast<IProxyHandler*>(
            m_pmProxyHandlers->get_Item(i));
        if (pkHandler)
        {
            pmMasterProxy = pkHandler->GetMasterProxyEntity(pmEntity);
            if (pmMasterProxy)
                break;
        }
    }

    MEntity* pmProxy;
    if (pmMasterProxy != NULL)
    {
        String* strProxyName = m_pmProxyScene->GetUniqueEntityName(
            String::Format("{0} - {1}", pmMasterProxy->Name, pmEntity->Name));
        MEntity* amClones[] = pmMasterProxy->Clone(strProxyName, false);
        NIASSERT(amClones->Count == 1);
        pmProxy = amClones[0];
        pmProxy->SetPropertyData(ms_strSourceEntityName, pmEntity, false);
        pmProxy->SetHidden(pmEntity->Hidden, false);
        pmProxy->SetFrozen(pmEntity->Frozen, false);
    }

    return pmProxy;
}
//---------------------------------------------------------------------------
void MProxyManager::AddProxy(MEntity* pmProxy, MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmProxy != NULL, "Null proxy provided to function!");
    MAssert(pmEntity != NULL, "Null entity provided to function!");

    if (!m_pmEntityToProxy->Contains(pmEntity))
    {
        // Add proxy to proxy scene.
        bool bSuccess = m_pmProxyScene->AddEntity(pmProxy, false);
        MAssert(bSuccess, "Add to proxy scene failed!");

        // Add proxy to hashtable.
        m_pmEntityToProxy->Add(pmEntity, pmProxy);
    }
}
//---------------------------------------------------------------------------
void MProxyManager::CreateAndAddProxy(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    MEntity* pmProxy = CreateProxy(pmEntity);
    if (pmProxy != NULL)
    {
        AddProxy(pmProxy, pmEntity);
    }
}
//---------------------------------------------------------------------------
void MProxyManager::RemoveProxy(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    if (m_pmEntityToProxy->Contains(pmEntity))
    {
        MEntity* pmProxy = dynamic_cast<MEntity*>(m_pmEntityToProxy->Item[
            pmEntity]);
        MAssert(pmProxy != NULL, "Proxy not found in hashtable!");

        // Remove proxy from proxy scene.
        m_pmProxyScene->RemoveEntity(pmProxy, false);

        // Remove proxy from hashtable.
        m_pmEntityToProxy->Remove(pmEntity);
    }
}
//---------------------------------------------------------------------------
MEntity* MProxyManager::GetProxyForEntity(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    return dynamic_cast<MEntity*>(m_pmEntityToProxy->Item[pmEntity]);
}
//---------------------------------------------------------------------------
MScene* MProxyManager::get_ProxyScene()
{
    MVerifyValidInstance;

    return m_pmProxyScene;
}
//---------------------------------------------------------------------------
void MProxyManager::AddProxyHandler(IProxyHandler* pkHandler)
{
    MVerifyValidInstance;

    m_pmProxyHandlers->Add(pkHandler);
}
//---------------------------------------------------------------------------
void MProxyManager::RemoveProxyHandler(IProxyHandler* pkHandler)
{
    MVerifyValidInstance;

    m_pmProxyHandlers->Remove(pkHandler);
}
//---------------------------------------------------------------------------
IProxyHandler* MProxyManager::GetProxyArray()[]
{
    MVerifyValidInstance;

    return dynamic_cast<IProxyHandler*[]>(
        m_pmProxyHandlers->ToArray(__typeof(IProxyHandler)));
}
//---------------------------------------------------------------------------
MEntity* MProxyManager::CreateGenericMasterProxyEntity(
    String* pmFilename, String* pmEntityName, 
    bool bSizeRelativeToCamera)
{
    if (!pmFilename || !pmEntityName)
        return NULL;

    MEntity* pmMasterProxy = NULL;

    NiDefaultErrorHandlerPtr spErrors;

    NiUniqueID kTemplateID;
    
    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiEntityInterfaceIPtr spEntity = NULL;
    bool bSuccess = false;

    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    
    const char* pcEntityName = MStringToCharPointer(pmEntityName);
    spEntity = NiNew NiGeneralEntity(pcEntityName, kTemplateID, 2);
    MFreeCharPointer(pcEntityName);

    if (bSizeRelativeToCamera)
    {
        bSuccess = NIBOOL_IS_TRUE(spEntity->AddComponent(NiNew 
            NiProxyComponent));
    }
    else
    {
        bSuccess = NIBOOL_IS_TRUE(spEntity->AddComponent(NiNew 
            NiTransformationComponent));
    }

    if (bSuccess)
    {
        const char* pcFilename = 
            MStringToCharPointer(pmFilename);

        if (!pcFilename)
            return false;

        char acAbsGeomPath[NI_MAX_PATH];       
        if (!NiPath::IsUniqueAbsolute(pcFilename))
        {
            const char* pcAppStartupPath = 
                MStringToCharPointer(MFramework::Instance->AppStartupPath);

            size_t stSize = 
                NiPath::ConvertToAbsolute(acAbsGeomPath, 
                NI_MAX_PATH, pcFilename, pcAppStartupPath);
            NI_UNUSED_ARG(stSize);
            NIASSERT(stSize > 0);
            
            MFreeCharPointer(pcAppStartupPath);
        }
        else
        {
            NiStrcpy(acAbsGeomPath, NI_MAX_PATH, pcFilename);
        }
    
        MFreeCharPointer(pcFilename);

        bSuccess = NIBOOL_IS_TRUE(spEntity->AddComponent(NiNew
            NiSceneGraphComponent(acAbsGeomPath)));
        if (bSuccess)
        {
            pmMasterProxy = MEntityFactory::Instance->Get(spEntity);
            pmMasterProxy->SetPropertyData("Source Entity", NULL,
                false);
        }
        else
        {
            if (!spErrors)
            {
                spErrors = NiNew NiDefaultErrorHandler(1);
            }
            spErrors->ReportError("Error creating camera proxy entity; "
                "camera proxy geometry will not be shown.", NULL, NULL, 
                NULL);
        }
    }
    else
    {
        if (!spErrors)
        {
            spErrors = NiNew NiDefaultErrorHandler(1);
        }
        spErrors->ReportError("Error creating camera proxy entity; camera "
            "proxy geometry will not be shown.", NULL, NULL, NULL);
    }

    if (spErrors)
    {
        MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
            spErrors);
    }

    return pmMasterProxy;
}
//---------------------------------------------------------------------------
ICommandService* MProxyManager::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------