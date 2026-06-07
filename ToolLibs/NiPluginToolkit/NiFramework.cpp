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

#include "NiFramework.h"
#include <NiStaticDataManager.h>
#include "NiSharedDataList.h"
#include "NiPluginManager.h"
#include <NiVersion.h>
#include "NiScriptTemplateManager.h"
#include "NiDefaultScriptReader.h"
#include "NiDefaultScriptWriter.h"
#include "NiPluginToolkitSDM.h"
#include <NiMemTracker.h>
#include <NiStandardAllocator.h>

static NiPluginToolkitSDM NiPluginToolkitSDMObject;

NiImplementRootRTTI(NiFramework);

NiFramework* NiFramework::ms_pkThis = NULL;

//---------------------------------------------------------------------------
NiFramework& NiFramework::GetFramework()
{
    NIASSERT(ms_pkThis != NULL);
    return *ms_pkThis;
}
//---------------------------------------------------------------------------
void NiFramework::InitFramework(const char* pcAppPath, const char* pcAppName,
    const char* pcAppVersion, bool bFloodgateParallelExecution)
{
    NIASSERT(ms_pkThis == NULL);

    NiInitOptions* pkOptions = NiExternalNew NiInitOptions(
#ifdef NI_MEMORY_DEBUGGER
        NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(), false)
#else
        NiExternalNew NiStandardAllocator()
#endif
    );

    pkOptions->SetFloodgateParallelExecution(bFloodgateParallelExecution);

    NiInit(pkOptions);
    NiSharedDataList::CreateInstance();
    NiPluginManager::CreateInstance();
    NiScriptTemplateManager::CreateInstance();
    NiScriptTemplateManager* pkScriptManager = 
        NiScriptTemplateManager::GetInstance();
    pkScriptManager->AddScriptReader(NiNew NiDefaultScriptReader());
    pkScriptManager->AddScriptWriter(NiNew NiDefaultScriptWriter());

    ms_pkThis = NiNew NiFramework(pcAppPath, pcAppName, pcAppVersion);
}
//---------------------------------------------------------------------------
void NiFramework::ShutdownFramework()
{
    NIASSERT(ms_pkThis != NULL);

    NiDelete ms_pkThis;
    ms_pkThis = NULL;

    NiScriptTemplateManager::DestroyInstance();
    NiSharedDataList::DestroyInstance();
    NiPluginManager::DestroyInstance();
    const NiInitOptions* pkOptions = NiStaticDataManager::GetInitOptions();
    NiShutdown();
    NiAllocator* pkAlloc = pkOptions->GetAllocator();
    NiExternalDelete pkOptions;
    NiExternalDelete pkAlloc;
}
//---------------------------------------------------------------------------
NiFramework::NiFramework(const char* pcAppPath, const char* pcAppName,
    const char* pcAppVersion)
{
    m_strAppPath = pcAppPath;
    m_strAppName = pcAppName;
    m_strAppVersion = pcAppVersion;
}
//---------------------------------------------------------------------------
const NiString& NiFramework::GetAppPath()
{
    return m_strAppPath;
}
//---------------------------------------------------------------------------
const NiString& NiFramework::GetAppName()
{
    return m_strAppName;
}
//---------------------------------------------------------------------------
const NiString& NiFramework::GetAppVersion()
{
    return m_strAppVersion;
}
//---------------------------------------------------------------------------
const char* NiFramework::GetNiVersion()
{
    return GAMEBRYO_SDK_VERSION_STRING;
}
//---------------------------------------------------------------------------
NiSharedDataList& NiFramework::GetSharedDataList() const
{
    NiSharedDataList* pkSharedDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkSharedDataList);
    return *pkSharedDataList;
}
//---------------------------------------------------------------------------
NiBatchExecutionResultPtr NiFramework::ExecuteScript(NiScriptInfo* pkScript)
{
    NiPluginManager* pkPluginManager = NiPluginManager::GetInstance();
    NIASSERT(pkPluginManager);
    return pkPluginManager->ExecuteScript(pkScript);
}
//---------------------------------------------------------------------------
