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

#ifndef NISTATICDATAMANAGER_H
#define NISTATICDATAMANAGER_H

#include "NiSystemLibType.h"
#include "NiSDMMacros.h"
#include "NiInitOptions.h"

#define NI_NUM_LIBRARIES 32

//---------------------------------------------------------------------------
class NISYSTEM_ENTRY NiStaticDataManager
{
public:
    static void Init(const NiInitOptions* pkOptions = NULL);
    static void Shutdown();

    typedef void (*InitFunction)(void);
    typedef void (*ShutdownFunction)(void);

    static void AddLibrary(const char* pcLibName, InitFunction pfnInit, 
        ShutdownFunction pfnShutdown, const char* pcDependencies);
    static void RemoveLibrary(const char* pcLibName);

    static const NiInitOptions* GetInitOptions();

    static void ProcessAccumulatedLibraries();

private:
    struct LibraryNode {
        /// The name of the library.
        const char* m_pcName;
        
        /// A white-space separated list of library names on which this
        /// library depends.
        const char* m_pcDependsOn;
        
        /// The initialization function.
        InitFunction m_pfnInitFunction;

        /// The shutdown function.
        ShutdownFunction m_pfnShutdownFunction;

        /// Edges in the graph that is used to find an initialization order.
        NiUInt8* m_puiDependencies;
        NiUInt8 m_uiNumDependencies;
        NiUInt8 m_uiMaxDependencies;

        /// The index into the array of all libraries
        NiUInt8 m_uiIndex;
    };
    
    /// Returns true if the library at the given index has other libraries
    /// that depend on it.
    static bool HasDependents(const NiUInt8 uiLibIndex);
    
    /// Returns true if all of the dependencies of the given library
    /// have been initialized.
    static bool DependenciesInitialized(const NiUInt8 uiLibIndex);
    
    /// Computes an ordering that has all dependents initialized after their
    /// dependencies.
    static bool ComputeProcessOrder();
    
    /// Builds up, for a single node, the dependency graph for computing
    /// the process order.
    static bool ConstructDependencyGraph(const NiUInt8 uiLibIndex,
        NiUInt8* auiUnvisited, NiUInt8& uiNumUnvisited,
        NiUInt8* auiDependencyFree, NiUInt8& uiNumDependencyFree);
    
    /// Adds a single dependency.
    static void AddDependency(const NiUInt8 uiLibIndex,
        const NiUInt8 uiDependentIndex);

    /// Removes all the dependencies on the given index. Used in computing
    /// the processing order.
    static void RemoveDependencies(const NiUInt8 uiLibIndex,
        NiUInt8* auiUnvisited, NiUInt8& uiNumUnvisited,
        NiUInt8* auiDependencyFree, NiUInt8& uiNumDependencyFree);

    static const NiInitOptions* ms_pkInitOptions;

    static LibraryNode ms_akLibraries[NI_NUM_LIBRARIES];
    static NiUInt8 ms_auiProcessOrder[NI_NUM_LIBRARIES];
    static NiUInt8 ms_uiNumLibraries;
    static NiUInt8 ms_uiNumInitializedLibraries;
    static bool ms_bAutoCreatedInitOptions;
    static bool ms_bInitialized;
};

#define NiInit NiStaticDataManager::Init
#define NiShutdown NiStaticDataManager::Shutdown

#include "NiStaticDataManager.inl"

#endif // #ifndef NISTATICDATAMANAGER_H
