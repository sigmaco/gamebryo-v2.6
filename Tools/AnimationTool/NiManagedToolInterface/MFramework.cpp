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

#include "stdafx.h"

#include "MFramework.h"
#include "MSharedData.h"

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MFramework::MFramework() : m_bUpdating(false)
{
    m_pkMessageManager = NiExternalNew MMessageManager();

    m_pkSceneGraph = NiExternalNew MSceneGraph;
    m_pkRenderer = NiExternalNew MRenderer;
    m_pkTimeManager = NiExternalNew MTimeManager();
    m_pkAnimation = NiExternalNew MAnimation(m_pkTimeManager);
    m_pkPhysics = NiExternalNew MPhysics();
    m_pkStatManager = NiExternalNew MStatisticsManager();
    m_pkUIManager = NiExternalNew MUIManager();
    m_pkUIManager->SetCamera(m_pkRenderer->ActiveCamera);
    m_strAppStartupPath = NULL;
}
//---------------------------------------------------------------------------
void MFramework::Init(String* strAppStartupPath)
{
    if (!ms_pkThis)
    {
        NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(
#ifdef NI_MEMORY_DEBUGGER
            NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(),
            false)
#else
            NiExternalNew NiStandardAllocator()
#endif
            );

        NiInit(pkInitOptions);
        
        MSharedData::Init();
        ms_pkThis = NiExternalNew MFramework;
        ms_pkThis->m_strAppStartupPath = strAppStartupPath;
        
        // Seed the start time of the application
        MUtility::GetCurrentTimeInSec();
        NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);
        ms_pkThis->m_pkRenderer->Init(strAppStartupPath, 
            ms_pkThis->m_pkStatManager);
        // Moved to post-render ms_pkThis->m_pkUIManager->Init();
#if defined(EE_PHYSX_BUILD)
        ms_pkThis->m_pkPhysics->Init();
#endif

    }
}
//---------------------------------------------------------------------------
void MFramework::Shutdown()
{
    if (ms_pkThis)
    {
        ms_pkThis->m_pkUIManager->Shutdown();
        ms_pkThis->m_pkStatManager->ResetRoots();
        ms_pkThis->m_pkRenderer->Shutdown();
        ms_pkThis->m_pkAnimation->Shutdown();
#if defined(EE_PHYSX_BUILD)
        ms_pkThis->m_pkPhysics->Shutdown();
#endif
        ms_pkThis->DeleteContents();

        MSharedData::Shutdown();
        ms_pkThis = NULL;
        NiImageConverter::SetImageConverter(NULL);

        const NiInitOptions* pkInitOptions = NiStaticDataManager
            ::GetInitOptions();
        NiShutdown();
        NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
        NiExternalDelete pkInitOptions;
        NiExternalDelete pkAllocator;
    }
}
//---------------------------------------------------------------------------
bool MFramework::InstanceIsValid()
{
    return (ms_pkThis != NULL);
}
//---------------------------------------------------------------------------
void MFramework::DeleteContents()
{
    m_pkUIManager->DeleteContents();
#if defined(EE_PHYSX_BUILD)
    m_pkPhysics->DeleteContents();
#endif
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    pkData->DeleteContents();
    pkData->Unlock();
    m_pkUIManager->DeleteContents();
    m_pkTimeManager = NULL;
    m_pkRenderer->DeleteContents();

}
//---------------------------------------------------------------------------
void MFramework::Update()
{
    if (m_bUpdating)
    {
        return;
    }

    m_bUpdating = true;

    m_pkTimeManager->UpdateTime();
    float fTime = m_pkTimeManager->CurrentTime;
    m_pkAnimation->Update(fTime, false);
    m_pkUIManager->Update();
    m_pkRenderer->ClearClickSwap();

    m_bUpdating = false;
}
//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
MFramework* MFramework::get_Instance()
{
    NIASSERT(ms_pkThis != NULL);
    return ms_pkThis;
}
//---------------------------------------------------------------------------
MSceneGraph* MFramework::get_SceneGraph()
{
    return m_pkSceneGraph;
}
//---------------------------------------------------------------------------
MAnimation* MFramework::get_Animation()
{
    return m_pkAnimation;
}
//---------------------------------------------------------------------------
MPhysics* MFramework::get_Physics()
{
    return m_pkPhysics;
}
//---------------------------------------------------------------------------
MRenderer* MFramework::get_Renderer()
{
    return m_pkRenderer;
}
//---------------------------------------------------------------------------
MTimeManager* MFramework::get_Clock()
{
    return m_pkTimeManager;
}
//---------------------------------------------------------------------------
MMessageManager* MFramework::get_Output()
{
    return m_pkMessageManager;
}
//---------------------------------------------------------------------------
MUIManager* MFramework::get_Input()
{
    return m_pkUIManager;
}
//---------------------------------------------------------------------------
String* MFramework::get_AppStartupPath()
{
    return m_strAppStartupPath;
}
//---------------------------------------------------------------------------
