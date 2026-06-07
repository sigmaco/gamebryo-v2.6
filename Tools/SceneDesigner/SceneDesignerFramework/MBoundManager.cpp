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

#include "MFramework.h"
#include "MBoundManager.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MBoundManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MBoundManager();
    }
}
//---------------------------------------------------------------------------
void MBoundManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MBoundManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MBoundManager* MBoundManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MBoundManager::MBoundManager() : m_uiUpdatesBetweenRecalculation(1),
    m_uiUpdateCounter(0), m_fOrbitDistance(100.0f), m_pkToolBounds(NULL)
{
    m_pkSceneBound = NiNew NiBound();
}
//---------------------------------------------------------------------------
void MBoundManager::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        NiDelete m_pkSceneBound;
        if (m_pkToolBounds)
        {
            m_pkToolBounds->RemoveAll();
        }
        NiDelete m_pkToolBounds;
        m_pkToolBounds = NULL;
    }
}
//---------------------------------------------------------------------------
void MBoundManager::Startup()
{
    MVerifyValidInstance;

    // figure out how many tool scenes we need to consider
    int iViewCount = MFramework::Instance->ViewportManager->ViewportCount;
    m_pkToolBounds = NiNew NiTObjectSet<NiBound>(iViewCount);
    m_amViewports = new MViewport*[iViewCount];
    for (int i = 0; i < iViewCount; i++)
    {
        // store a pointer to the viewport object in an array
        m_amViewports[i] = 
            MFramework::Instance->ViewportManager->GetViewport(i);
        NiBound kDefaultBound;
        kDefaultBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
        m_pkToolBounds->Add(kDefaultBound);
    }

    String* strName = "Frames Per Bound Calculation";
    String* strOptionCategory = "Performance.Frames Per Bound Calculation";
    String* strDescription = "The scene bounds will be recalculated once per "
        "this number of updates";
    SettingsCategory eCategory = SettingsCategory::PerUser;
    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MBoundManager::OnSettingsChanged);

    // register settings for num updates between bounds calculation
    // first register the setting with the service if it doesn't already exist
    SettingsService->RegisterSettingsObject(strName, 
        __box(m_uiUpdatesBetweenRecalculation), eCategory);
    // set the event handler for when that setting changes
    if (pmHandler != NULL)
    {
        SettingsService->SetChangedSettingHandler(strName, eCategory, 
            pmHandler);
    }
    // get the value of the setting in case it existed before and cache it
    Object* pmObj;
    pmObj = SettingsService->GetSettingsObject(strName, eCategory);
    __box unsigned int* puiVal = dynamic_cast<__box unsigned int*>(pmObj);
    if (puiVal != NULL)
    {
        m_uiUpdatesBetweenRecalculation = *puiVal;
    }
    OptionsService->AddOption(strOptionCategory, eCategory, strName); 
    OptionsService->SetHelpDescription(strOptionCategory, 
        strDescription);

    strName = "Orbit Distance";
    eCategory = SettingsCategory::PerUser;

    // first register the setting with the service if it doesn't already exist
    SettingsService->RegisterSettingsObject(strName, 
        __box(m_fOrbitDistance), eCategory);
    // set the event handler for when that setting changes
    if (pmHandler != NULL)
    {
        SettingsService->SetChangedSettingHandler(strName, eCategory, 
            pmHandler);
    }
    // get the value of the setting in case it existed before and cache it
    pmObj = SettingsService->GetSettingsObject(strName, eCategory);
    __box float* pfVal = dynamic_cast<__box float*>(pmObj);
    if (pfVal != NULL)
    {
        m_fOrbitDistance = *pfVal;
    }

    // do an initial update to set the scene bounds
    m_uiUpdateCounter = UINT_MAX - 1;
    Update(0.0f);
}
//---------------------------------------------------------------------------
void MBoundManager::OnSettingsChanged(Object*,
    SettingChangedEventArgs* pmEventArgs)
{
    MVerifyValidInstance;

    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    if ((strSetting->Equals("Frames Per Bound Calculation")) && 
        (eCategory == SettingsCategory::PerUser))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box int* piVal = dynamic_cast<__box int*>(pmObj);
        if (piVal != NULL)
        {
            m_uiUpdatesBetweenRecalculation = (unsigned int)*piVal;
        }
    }
}
//---------------------------------------------------------------------------
void MBoundManager::Update(float)
{
    MVerifyValidInstance;

    m_uiUpdateCounter++;
    if (m_uiUpdateCounter >= m_uiUpdatesBetweenRecalculation)
    {
        MFramework::Instance->Scene->GetBound(m_pkSceneBound);
        if (m_pkSceneBound->GetRadius() <= 0.0f)
        {
            m_pkSceneBound->SetRadius(m_fOrbitDistance);
        }
        for (int i = 0; i < m_amViewports->Count; i++)
        {
            // recalculate all bounds
            NiBound kSceneBound;
            kSceneBound = *m_pkSceneBound;
            NiTPrimitiveArray<const NiBound*> kBoundArray;
            kBoundArray.Add(&kSceneBound);
            NiBound kProxyBound;
            MFramework::Instance->ProxyManager->ProxyScene->GetBound(
                &kProxyBound);
            kBoundArray.Add(&kProxyBound);
            MScene* pmToolScene = m_amViewports[i]->ToolScene;
            MEntity* amEntities[] = pmToolScene->GetEntities();
            for (int j = 0; j < amEntities->Count; j++)
            {
                if (!MFramework::Instance->CameraManager->
                    EntityIsCamera(amEntities[j]))
                {
                    NiAVObject* pkScene;
                    pkScene = amEntities[j]->GetSceneRootPointer(0);
                    if (pkScene)
                    {
                        kBoundArray.Add(&(pkScene->GetWorldBound()));
                    }
                }
            }
            kSceneBound.ComputeMinimalBound(kBoundArray);
            if (kSceneBound.GetRadius() <= 0.0f)
            {
                kSceneBound.SetRadius(m_fOrbitDistance);
            }
            m_pkToolBounds->ReplaceAt(i, kSceneBound);
        }
        m_uiUpdateCounter = 0;
    }
}
//---------------------------------------------------------------------------
NiBound* MBoundManager::GetSceneBound()
{
    MVerifyValidInstance;

    return m_pkSceneBound;
}
//---------------------------------------------------------------------------
NiBound* MBoundManager::GetToolSceneBound(MViewport* pmViewport)
{
    MVerifyValidInstance;

    for (unsigned int ui = 0; ui < m_pkToolBounds->GetSize(); ui++)
    {
        if (pmViewport == m_amViewports[ui])
        {
            NiBound* pkBound = &(m_pkToolBounds->GetAt(ui));
            return pkBound;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
ISettingsService* MBoundManager::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found!");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MBoundManager::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not found!");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
