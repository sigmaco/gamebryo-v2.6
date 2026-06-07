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

#ifndef NIPHYSXMANAGER_H
#define NIPHYSXMANAGER_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXAllocator.h"
#include "NiPhysXClothMesh.h"
#include "NiPhysXConvexMesh.h"
#include "NiPhysXTriangleMesh.h"
#include "NiPhysXTypes.h"
#include "NiPhysXUserOutput.h"
#include <NiPhysXTypes.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

/**
    NiPhysXManager is a singleton class for managing global Gamebryo-PhysX
    state: the PhysX SDK pointer and collision meshes.
    
    Applications cannot create an instance of this class - a single instance
    is created by the Gamebryo-PhysX static data manager when the NiPhysX
    library is loaded (be sure to include NiPhysX.h in at least one file).
*/
class NIPHYSX_ENTRY NiPhysXManager : public NiMemObject
{
public:
    /// Obtain a pointer to the singleton manager.
    static NiPhysXManager* GetPhysXManager();
    
    /**
        Initialize the PhysX SDK.
        
        This function must be called before any PhysX or Gamebryo-PhysX
        related calls. This function creates the PhysX SDK using the given
        allocator and user output stream. If not specified, a NiPhysXAllocator
        object and a NiPhysXUserOutput object are created and used. The SDK
        descriptor argument is passed directly to the PhysX SDK creation
        function. Its default is a constant initialized with the defalt
        constructor for the NxPhysicsSDKDesc class.

        The PhysX SDK is a reference counted singleton object, so if another
        thread has already initialized, then this call will increment the
        reference count. In that situation, the allocator and SDK descriptor
        are not used but the output stream is used. Every call to Initialize
        must be matched with a call to Shutdown.
    */
    bool Initialize(NxUserAllocator* pkAllocator = NULL,
        NxUserOutputStream* pkOutputStream = NULL,
        const NxPhysicsSDKDesc& kSDKDesc =
        NiPhysXTypes::NXPHYSICSSDKDESC_DEFAULT,
        NxSDKCreateError* peErrorCode = 0);

    /**
        Releases the PhysX SDK and other resources such as the mesh tables.
        
        Shutdown must not be called until all pointers into PhysX related
        things have been freed, including smart pointers, because destructors
        for some Gamebryo-PhysX objects require the PhysX SDK. Note that if
        another thread has a handle on the SDK then it will not be released
        and any resources you may still have, such as scenes or meshes, will
        not be released.
    */
    void Shutdown();
    
    /**
        @name PhysX SDK Locking Functions
        
        The PhysX SDK object is not thread safe. These functions control
        access to any critical sections that touch the PhysX SDK, and have
        critical section semantics (see NiCriticalSection). In a multi-threaded
        environment, an application should call WaitSDKLock() before any call
        into the PhysX SDK, and call ReleaseSDKLock() when done. 
    */
    /// @{

    /// Wait on te lock controlling access to the PhysX SDK object.
    void WaitSDKLock();
    
    /// Release the lock on the PhysX SDK object.
    void ReleaseSDKLock();

    /// @}

    /**
        @name Collision Mesh Management Functions

        The PhysX SDK allows the sharing of meshes. NiPhysXManager implements
        sharing from the Gamebryo side, particularly the sharing of meshes
        across scenes that are created at different times or from streams.
        Three hash tables of meshes are maintained, one for NxConvexMesh
        meshes, one for NxTriangleMesh meshes and one for NxClothMesh objects.
        The tables are indexed on the name for each mesh. 

        When a NIF file is streamed in, any meshes in the file are added to the
        manager, as are any in external files referenced by the NIF file.
        Applications may also add their own meshes. 
    */
    /// @{

    /**
        Get the platform specific directory in which to search for meshes.
        
        PhysX meshes may have different cooked formats on different platforms,
        and these functions allow applications to more easily manage loading
        the appropriate resources. This is modeled after the NiImageConverter
        class's methods for accessing textures, and functions in the same way.
        It is used when loading a NIF file that contains references to
        externally stored meshes. 
    */
    static const char* GetPlatformSpecificSubdirectory();

    /// Set the platform specific directory in which to search for meshes.
    ///
    /// The string is copied.
    static void SetPlatformSpecificSubdirectory(const char* pcSubDir);

    /**
        Convert a given file name in a platform specific file.
    
        This is modeled after the NiImageConverter class's methods for
        accessing textures, and functions in the same way. 
    */
    static char* ConvertFilenameToPlatformSpecific(const char* pcPath);
 
    /**
        Convert a given file name in a platform specific file.
    
        This is modeled after the NiImageConverter class's methods for
        accessing textures, and functions in the same way. 
    */
    static void ConvertFilenameToPlatformSpecific(const NiFixedString& kPath,
        NiFixedString& kPlatformSpecificPath);
    

    /// Get a cloth mesh by name.
    ///
    /// The return value is a smart pointer and should not be freed.
    NiPhysXClothMesh* GetClothMesh(const char* pcPath);

    /// Get a cloth mesh by the underlying PhysX mesh object.
    ///
    /// The return value is a smart pointer and should not be freed.
    NiPhysXClothMesh* GetClothMesh(const NxClothMesh* pkMesh);

    /**
        Set a cloth mesh using the name in the NiPhysXClothMesh object.
        
        The argument is converted to a smart pointer for storage, so
        applications must not explicitly free the given pointer.
    */
    void SetClothMesh(NiPhysXClothMesh* pkMesh);

    /// Remove the given mesh from the table.
    bool RemoveClothMesh(NiPhysXClothMesh* pkMesh);

    /// Get a convex mesh by name.
    ///
    /// The return value is a smart pointer and should not be freed.
    NiPhysXConvexMesh* GetConvexMesh(const char* pcPath);

    /// Get a convex mesh by the underlying PhysX mesh object.
    ///
    /// The return value is a smart pointer and should not be freed.
    NiPhysXConvexMesh* GetConvexMesh(const NxConvexMesh* pkMesh);

    /**
        Set a convex mesh using the name in the NiPhysXConvexhMesh object.
        
        The argument is converted to a smart pointer for storage, so
        applications must not explicitly free the given pointer.
    */
    void SetConvexMesh(NiPhysXConvexMesh* pkMesh);

    /// Remove the given mesh from the table.
    bool RemoveConvexMesh(NiPhysXConvexMesh* pkMesh);

    /// Get a triangle mesh by name.
    ///
    /// The return value is really a smart pointer and should not be freed.
    NiPhysXTriangleMesh* GetTriangleMesh(const char* pcPath);

    /// Get a triangle mesh by the underlying PhysX mesh object.
    ///
    /// The return value is really a smart pointer and should not be freed.
    NiPhysXTriangleMesh* GetTriangleMesh(const NxTriangleMesh* pkMesh);

    /**
        Set a triangle mesh using the name in the NiPhysXTrianglehMesh object.
        
        The argument is converted to a smart pointer for storage, so
        applications must not explicitly free the given pointer.
    */
    void SetTriangleMesh(NiPhysXTriangleMesh* pkMesh);

    /// Remove the given mesh from the table.
    bool RemoveTriangleMesh(NiPhysXTriangleMesh* pkMesh);

    /// Remove all cached meshes with a reference count of 0.
    void RemoveUnusedMeshes();

    /// @}

    /**
        The SDK object that is created.
        
        Not valid until Initialize has been called, nor after Shutdown has
        been called.
    */
    NxPhysicsSDK* m_pkPhysXSDK;

    /**
        Create a PhysX scene in a thread on a particular core.
        
        The PhysX SDK creates the simulation thread at the time createScene
        is called. This function assists in targetting that thread to another
        core if necessary. Behavior is platform specific ...

        Windows: createScene is called directly. Windows will move the thread
            across processors.

        PS3: createScene is called directly. The PS3 OS will move the thread
            to either of the hardware threads as needed.

        Xbox 360: The current thread of execution is moved to the ideal
            processor specified in kAffinity before createScene is called.
            This has the effect of running the thread on that processor. The
            current thread is restored to its original processor after
            createScene is called.
    */
    NxScene* CreateSceneOnDifferentCore(const NxSceneDesc& kDesc, 
        const NiProcessorAffinity& kAffinity);
        
    /// Provide a string corresponding to the SDK creation error codes.
    static const char* GetSDKCreateErrorString(NxSDKCreateError* peErrorCode);

protected:
    
    // Cannot create one of these directly - only through SDM
    NiPhysXManager();
    
    // And only destructed through the SDM
    ~NiPhysXManager();
    
    // Locking functions for the mesh tables.
    void WaitMeshLock();
    void ReleaseMeshLock();

    // Some platforms require specific initialization or shutdown. These
    // functions are implemented in platform specific files.
    bool PlatformSpecificPostSDKInit();
    void PlatformSpecificSDKShutdown();
    
    // Only non-NULL if the user did not provide their own versions to
    // Initialize().
    NiPhysXUserOutput* m_pkOutputStream;
    NiPhysXAllocator* m_pkAllocator;

    // The one and only instance of this class
    static NiPhysXManager* ms_pPhysXManager;
    
    // For managing the locks
    NiCriticalSection m_kSDKCritSect;
    NiCriticalSection m_kMeshCritSect;
    
    // Mesh directory.
    static char* ms_pcPlatformSubDir;
    
    // Meshes
    NiTStringMap<NiPhysXClothMeshPtr> m_kClothMeshes;
    NiTStringMap<NiPhysXConvexMeshPtr> m_kConvexMeshes;
    NiTStringMap<NiPhysXTriangleMeshPtr> m_kTriangleMeshes;

    friend class NiPhysXSDM;
};

#include "NiPhysXManager.inl"

#endif  // #ifndef NIPHYSXMANAGER_H

