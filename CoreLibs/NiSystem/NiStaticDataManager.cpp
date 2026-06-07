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
#include "NiSystemPCH.h"

#include "NiLog.h"
#include "NiStaticDataManager.h"
#include "NiSystemSDM.h"
#include "NiRTLib.h"
#include "NiInitOptions.h"

NiUInt8 NiStaticDataManager::ms_uiNumLibraries = 0;
NiUInt8 NiStaticDataManager::ms_uiNumInitializedLibraries = 0;
bool NiStaticDataManager::ms_bInitialized = false;
NiStaticDataManager::LibraryNode
    NiStaticDataManager::ms_akLibraries[NI_NUM_LIBRARIES];
NiUInt8 NiStaticDataManager::ms_auiProcessOrder[NI_NUM_LIBRARIES];

const NiInitOptions* NiStaticDataManager::ms_pkInitOptions = NULL;
bool NiStaticDataManager::ms_bAutoCreatedInitOptions = false;

//---------------------------------------------------------------------------
void NiStaticDataManager::Init(const NiInitOptions* pkOptions)
{
    // NiSystem is a required library, so its Init function is called
    // directly instead of being registered.
    if (pkOptions == NULL)
    {
        pkOptions = NiExternalNew NiInitOptions();
        ms_bAutoCreatedInitOptions = true;
    }
    else
    {   
        ms_bAutoCreatedInitOptions = false;
    }
    NIASSERT(pkOptions);
    ms_pkInitOptions = pkOptions;

    NiSystemSDM::Init();

    ms_bInitialized = true;
    ProcessAccumulatedLibraries();
    
    NiOutputDebugString("NiStaticDataManager Initialized\n");
}
//---------------------------------------------------------------------------
void NiStaticDataManager::Shutdown()
{
    // NiSystem is a required library, so its Shutdown functions is called
    // directly instead of being registered.    
    
    // Shutdown in reverse order of initialization
    for (int i = ms_uiNumInitializedLibraries-1; i >= 0 ; i--)
    {
        (*ms_akLibraries[ms_auiProcessOrder[i]].m_pfnShutdownFunction)();
    }
    ms_uiNumInitializedLibraries = 0;

    NiSystemSDM::Shutdown();

    if (ms_bAutoCreatedInitOptions)
        NiExternalDelete ms_pkInitOptions;

    ms_bInitialized = false;

    NiOutputDebugString("NiStaticDataManager Shutdown\n");
}
//---------------------------------------------------------------------------
void NiStaticDataManager::AddLibrary(const char* pcLibName,
    InitFunction pfnInit, ShutdownFunction pfnShutdown,
    const char* pcDependencies)
{
    // You hit this assert if there are more libraries registering than the
    // amount of allocated space. To change the amount of allocated space,
    // modify NI_NUM_LIBRARIES in NiStaticManager.h.
    NIASSERT(ms_uiNumLibraries < NI_NUM_LIBRARIES); 

    // Store all there is to know about the library
    ms_akLibraries[ms_uiNumLibraries].m_pcName = pcLibName;
    ms_akLibraries[ms_uiNumLibraries].m_uiIndex = ms_uiNumLibraries;
    ms_akLibraries[ms_uiNumLibraries].m_pcDependsOn = pcDependencies;
    ms_akLibraries[ms_uiNumLibraries].m_uiNumDependencies = 0;
    ms_akLibraries[ms_uiNumLibraries].m_uiMaxDependencies = 0;
    ms_akLibraries[ms_uiNumLibraries].m_puiDependencies = 0;
    ms_akLibraries[ms_uiNumLibraries].m_pfnInitFunction = pfnInit;
    ms_akLibraries[ms_uiNumLibraries].m_pfnShutdownFunction = pfnShutdown;
    
    // Initially the process order is the order it is added.
    ms_auiProcessOrder[ms_uiNumLibraries] = ms_uiNumLibraries;

    ms_uiNumLibraries++; 
}
//---------------------------------------------------------------------------
void NiStaticDataManager::RemoveLibrary(const char* pcLibName)
{
    NiUInt8 uiRemove = 0;
    for (uiRemove = 0; uiRemove < ms_uiNumLibraries; uiRemove++)
    {
        if (!strcmp(pcLibName, ms_akLibraries[uiRemove].m_pcName))
        {
            break;
        }
    }

    // You hit this assert if you try to remove a library that has not been
    // added.
    NIASSERT(uiRemove != ms_uiNumLibraries);

    // You hit this assert if the library you are trying to remove still has
    // active dependents. Remove the dependents first.
    NIASSERT(!HasDependents(uiRemove));

    // This test is only valid because we do not re-arrange the order
    // of initialization unless we also update ms_uiNumInitializedLibraries.
    if (uiRemove < ms_uiNumInitializedLibraries)
    {
        ms_akLibraries[uiRemove].m_pfnShutdownFunction();
        ms_uiNumInitializedLibraries--;
    }
    
    for (NiUInt32 ui = uiRemove + 1; ui < ms_uiNumLibraries; ui++)
    {
        --ms_akLibraries[ui].m_uiIndex;
        ms_akLibraries[ui - 1] = ms_akLibraries[ui];
    }
    
    // Find it in the process order list, and modify the process order
    // list indices to reflect the shuffle done above.
    NiUInt8 uiOrderRemove = ms_uiNumLibraries;
    for (NiUInt32 ui = 0; ui < ms_uiNumLibraries; ui++)
    {
        if (ms_auiProcessOrder[ui] == uiRemove)
        {
            uiOrderRemove = (NiUInt8)ui;
        }
        else if (ms_auiProcessOrder[ui] > uiRemove)
        {
            ms_auiProcessOrder[ui]--;
        }
    }

    // You hit this assert if the library is not in the process order.
    // Failure indicates an internal problem in this class.
    NIASSERT(uiOrderRemove != ms_uiNumLibraries);

    for (NiUInt32 ui = uiOrderRemove + 1; ui < ms_uiNumLibraries; ui++)
    {
        ms_auiProcessOrder[ui - 1] = ms_auiProcessOrder[ui];
    }

    ms_uiNumLibraries--;
}
//---------------------------------------------------------------------------
void NiStaticDataManager::ProcessAccumulatedLibraries()
{
    if (!ms_bInitialized || !ms_uiNumLibraries)
        return;
        
    if (ms_uiNumInitializedLibraries)
    {
        for (NiUInt32 ui = ms_uiNumInitializedLibraries; 
            ui < ms_uiNumLibraries; ui++)
        {
            NIASSERT(DependenciesInitialized((NiUInt8)ui));
            
            (*ms_akLibraries[ui].m_pfnInitFunction)();
        }        

        ms_uiNumInitializedLibraries = ms_uiNumLibraries;
    }
    else
    {
        NIVERIFY(ComputeProcessOrder());
        
        for (NiUInt32 ui = 0; ui < ms_uiNumLibraries; ui++)
        {
            (*ms_akLibraries[ms_auiProcessOrder[ui]].m_pfnInitFunction)();
        }        

        ms_uiNumInitializedLibraries = ms_uiNumLibraries;
    }
}
//---------------------------------------------------------------------------
bool NiStaticDataManager::HasDependents(const NiUInt8 uiLibIndex)
{
    const char* pcName = ms_akLibraries[uiLibIndex].m_pcName;
    for (NiUInt32 ui = 0; ui < ms_uiNumInitializedLibraries; ui++)
    {
        if (uiLibIndex == ui)
            continue;
            
        if (strstr(ms_akLibraries[ui].m_pcDependsOn, pcName))
           return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------
bool NiStaticDataManager::DependenciesInitialized(const NiUInt8 uiLibIndex)
{
    size_t uiLength = strlen(ms_akLibraries[uiLibIndex].m_pcDependsOn) + 1;
    char* pcDependsOn = (char*)NiExternalMalloc(uiLength);
    NiStrncpy(pcDependsOn, uiLength, ms_akLibraries[uiLibIndex].m_pcDependsOn,
        uiLength - 1);
    char* pcContext = 0;
    char* pcDependency = 0;
    pcDependency = NiStrtok(pcDependsOn, " ", &pcContext);
    while (pcDependency)
    {
        NiUInt32 uiDepIndex = 0;
        for ( ; uiDepIndex < ms_uiNumInitializedLibraries ; uiDepIndex++)
        {
            if (!strcmp(pcDependency, ms_akLibraries[uiDepIndex].m_pcName))
            {
                break;
            }
        }
        if (uiDepIndex == ms_uiNumInitializedLibraries)
            return false;
        pcDependency = NiStrtok(0, " ", &pcContext);
    }

    NiExternalFree(pcDependsOn);

    return true;
}
//---------------------------------------------------------------------------
bool NiStaticDataManager::ComputeProcessOrder()
{
    if (ms_uiNumLibraries == 0)
        return true;

    NiUInt8 auiUnvisited[NI_NUM_LIBRARIES];
    NiUInt8 uiNumUnvisited = 0;
    
    NiUInt8 auiDependencyFree[NI_NUM_LIBRARIES];
    NiUInt8 uiNumDependencyFree = 0;
    
    // Set all the edges in the graph
    for (NiUInt8 ui = 0; ui < ms_uiNumLibraries; ui++)
    {
        if (!ConstructDependencyGraph(ui, auiUnvisited, uiNumUnvisited,
            auiDependencyFree, uiNumDependencyFree))
        {
            return false;
        }
    }
    
    // If you hit this assert, then there is no library without dependencies,
    // which implies a circular dependency structure.
    NIASSERT(uiNumDependencyFree > 0);
    
    NiUInt8 uiNumProcessed = 0;
    while (uiNumDependencyFree > 0)
    {
        NiUInt8 uiNextLib = auiDependencyFree[uiNumDependencyFree - 1];
        --uiNumDependencyFree;
        
        ms_auiProcessOrder[uiNumProcessed] = uiNextLib;
        
        RemoveDependencies(uiNextLib, auiUnvisited, uiNumUnvisited,
            auiDependencyFree, uiNumDependencyFree);
        
        ++uiNumProcessed;
    }
    
    // When done, there should be no unvisited nodes. If there are, there is
    // a circular dependency.
    NIASSERT(uiNumUnvisited == 0);
    NIASSERT(uiNumProcessed == ms_uiNumLibraries);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiStaticDataManager::ConstructDependencyGraph(const NiUInt8 uiLibIndex,
    NiUInt8* auiUnvisited, NiUInt8& uiNumUnvisited,
    NiUInt8* auiDependencyFree, NiUInt8& uiNumDependencyFree)
{
    size_t uiLength = strlen(ms_akLibraries[uiLibIndex].m_pcDependsOn) + 1;
    char* pcDependsOn = (char*)NiExternalMalloc(uiLength);
    NiStrncpy(pcDependsOn, uiLength, ms_akLibraries[uiLibIndex].m_pcDependsOn,
        uiLength - 1);
    char* pcContext = 0;
    char* pcDependency = 0;
    pcDependency = NiStrtok(pcDependsOn, " ", &pcContext);
    
    if (!pcDependency)
    {
        auiDependencyFree[uiNumDependencyFree] = uiLibIndex;
        ++uiNumDependencyFree;
    }
    else
    {
        while (pcDependency)
        {
            NiUInt32 uiDepIndex = 0;
            for ( ; uiDepIndex < ms_uiNumLibraries ; uiDepIndex++)
            {
                if (!strcmp(pcDependency, ms_akLibraries[uiDepIndex].m_pcName))
                {
                    AddDependency(uiLibIndex, (NiUInt8)uiDepIndex);
                    break;
                }
            }
            if (uiDepIndex == ms_uiNumLibraries)
            {
                NiOutputDebugString("------- ERROR -------\n");
                char acMsg[1024];
                NiSprintf(acMsg, 1024, "NiStaticDataManager: Can't find "
                    "library %s that library %s depends on.\n",
                    pcDependency, ms_akLibraries[uiLibIndex].m_pcName);
                NiOutputDebugString(acMsg);
                NiSprintf(acMsg, 1024, "NiStaticDataManager: Did you include "
                    "the top-level header file for %s?\n", pcDependency);
                NiOutputDebugString(acMsg);

                NIASSERT(
                    !"Missing #include library. See debugger output text.");
                return false;
            }
            pcDependency = NiStrtok(0, " ", &pcContext);
        }
        
        auiUnvisited[uiNumUnvisited] = uiLibIndex;
        ++uiNumUnvisited;
    }
    
    NiExternalFree(pcDependsOn);

    return true;
}
//---------------------------------------------------------------------------
void NiStaticDataManager::AddDependency(const NiUInt8 uiLibIndex,
    const NiUInt8 uiDependentIndex)
{
    LibraryNode* pkLib = ms_akLibraries + uiLibIndex;
    if (pkLib->m_uiMaxDependencies == pkLib->m_uiNumDependencies)
    {
        pkLib->m_uiMaxDependencies += 5;
        NiUInt8* puiNewDependencies =
            (NiUInt8*)NiExternalMalloc(pkLib->m_uiMaxDependencies);
        for (NiUInt32 ui = 0; ui < pkLib->m_uiNumDependencies; ui++)
        {
            puiNewDependencies[ui] = pkLib->m_puiDependencies[ui];
        }
        if (pkLib->m_puiDependencies)
        {
            NiExternalFree(pkLib->m_puiDependencies);
        }
        pkLib->m_puiDependencies = puiNewDependencies;
    }
    
    pkLib->m_puiDependencies[pkLib->m_uiNumDependencies] = uiDependentIndex;
    ++pkLib->m_uiNumDependencies;
}
//---------------------------------------------------------------------------
void NiStaticDataManager::RemoveDependencies(const NiUInt8 uiLibIndex,
    NiUInt8* auiUnvisited, NiUInt8& uiNumUnvisited,
    NiUInt8* auiDependencyFree, NiUInt8& uiNumDependencyFree)
{
    NiUInt32 ui = 0;
    while (ui < uiNumUnvisited)
    {
        LibraryNode* pkLib = ms_akLibraries + auiUnvisited[ui];
        bool bRemoved = false;
        
        for (NiUInt32 uj = 0 ; uj < pkLib->m_uiNumDependencies; uj++)
        {
            if (pkLib->m_puiDependencies[uj] == uiLibIndex)
            {
                for (NiUInt32 uk = uj + 1; uk < pkLib->m_uiNumDependencies;
                    uk++)
                {
                    pkLib->m_puiDependencies[uk - 1] =
                        pkLib->m_puiDependencies[uk];
                }
                --pkLib->m_uiNumDependencies;
                if (!pkLib->m_uiNumDependencies)
                {
                    NiExternalFree(pkLib->m_puiDependencies);
                    pkLib->m_puiDependencies = 0;
                    pkLib->m_uiMaxDependencies = 0;
                    
                    auiDependencyFree[uiNumDependencyFree] = auiUnvisited[ui];
                    ++uiNumDependencyFree;
                    
                    for (NiUInt32 uk = ui + 1; uk < uiNumUnvisited; uk++)
                    {
                        auiUnvisited[uk - 1] = auiUnvisited[uk];
                    }
                    --uiNumUnvisited;
                    bRemoved = true;
                }
                break;
            }
        }
        
        if (!bRemoved)
        {
            ui++;
        }
    }
}
//---------------------------------------------------------------------------
