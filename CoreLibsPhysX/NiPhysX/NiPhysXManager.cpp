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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#include <NiFilename.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

NiPhysXManager* NiPhysXManager::ms_pPhysXManager;
char* NiPhysXManager::ms_pcPlatformSubDir;

//---------------------------------------------------------------------------
NiPhysXManager::NiPhysXManager()
{
    m_pkAllocator = 0;
    m_pkOutputStream = 0;
    m_pkPhysXSDK = 0;
    ms_pcPlatformSubDir = 0;
}
//---------------------------------------------------------------------------
NiPhysXManager::~NiPhysXManager()
{
    if (ms_pcPlatformSubDir)
    {
        NiFree(ms_pcPlatformSubDir);
    }
    ms_pcPlatformSubDir = 0;
}
//---------------------------------------------------------------------------
bool NiPhysXManager::Initialize(NxUserAllocator* pkAllocator,
        NxUserOutputStream* pkOutputStream, const NxPhysicsSDKDesc& kSDKDesc,
        NxSDKCreateError* eErrorCode)
{
    // If allocator or output stream are NULL, then allocate and use the
    // defaults. If someone is already using an SDK, our suggestions will be
    // ignored, but that should only happen in art tools.
    if (!pkAllocator)
    {
        m_pkAllocator = NiNew NiPhysXAllocator;
        pkAllocator = m_pkAllocator;
    }
    if (!pkOutputStream)
    {
        m_pkOutputStream = NiNew NiPhysXUserOutput;
        pkOutputStream = m_pkOutputStream;
    }
    
    WaitSDKLock();
    m_pkPhysXSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, pkAllocator,
        pkOutputStream, kSDKDesc, eErrorCode);
    ReleaseSDKLock();
    
    if (m_pkPhysXSDK == 0)
    {
        NiOutputDebugString("NiPhysXManager: Failed to create PhysX SDK\n");
        return false;
    }
    
    if (!PlatformSpecificPostSDKInit())
    {
        Shutdown();
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXManager::Shutdown()
{
    if (m_pkPhysXSDK)
    {    
        // Clean up the SDK
        WaitSDKLock();
        NxReleasePhysicsSDK(m_pkPhysXSDK);
        m_pkPhysXSDK = 0;
        ReleaseSDKLock();
    }
    
    // Clean up the allocator and output stream
    NiDelete m_pkOutputStream;
    m_pkOutputStream = 0;
    NiDelete m_pkAllocator;
    m_pkAllocator = 0;
    
    // Clear out the meshes
    WaitMeshLock();
    m_kClothMeshes.RemoveAll();
    m_kConvexMeshes.RemoveAll();
    m_kTriangleMeshes.RemoveAll();
    ReleaseMeshLock();

    PlatformSpecificSDKShutdown();
}
//---------------------------------------------------------------------------
const char* NiPhysXManager::GetSDKCreateErrorString(
    NxSDKCreateError* peErrorCode)
{
    if (!peErrorCode)
        return NULL;
        
    switch (*peErrorCode)
    {
        case NXCE_NO_ERROR:
            return "No errors occurred when creating the Physics SDK.";
            
        case NXCE_PHYSX_NOT_FOUND:
            return "Unable to find the PhysX libraries. The PhysX drivers are "
                "not installed correctly.";
            
        case NXCE_WRONG_VERSION:
            return "The application supplied a version number that does not "
                "match with the library's.";
                  
        case NXCE_DESCRIPTOR_INVALID:
            return "The supplied SDK descriptor is invalid.";
            
        case NXCE_CONNECTION_ERROR:
            return "A PhysX card was found, but there are problems when "
                "communicating with the card.";
                
        case NXCE_RESET_ERROR:
            return "A PhysX card was found, but it did not reset (or "
                "initialize) properly.";
                
        case NXCE_IN_USE_ERROR:
            return "A PhysX card was found, but it is already in use by "
                "another application.";

        case NXCE_BUNDLE_ERROR:
            return "A PhysX card was found, but there are issues with loading "
                "the firmware.";
                
        default:
            NIASSERT(false && "Unknown PhysX SDK Create error code");
    }
    
    return NULL;
}
//---------------------------------------------------------------------------
const char* NiPhysXManager::GetPlatformSpecificSubdirectory() 
{
    return ms_pcPlatformSubDir;
}
//---------------------------------------------------------------------------
void NiPhysXManager::SetPlatformSpecificSubdirectory(const char* pcSubDir)
{
    if (ms_pcPlatformSubDir)
    {
        NiFree(ms_pcPlatformSubDir);
    }

    if (pcSubDir)
    {
        size_t stLen = strlen(pcSubDir) + 1;
        ms_pcPlatformSubDir = NiAlloc(char, stLen);
        NiStrcpy(ms_pcPlatformSubDir, stLen, pcSubDir);
        NiPath::Standardize(ms_pcPlatformSubDir);
    }
    else
    {
        ms_pcPlatformSubDir = NULL;
    }
}
//---------------------------------------------------------------------------
char* NiPhysXManager::ConvertFilenameToPlatformSpecific(const char* pcPath)
{
    char* pcPlatformPath = NiAlloc(char, 260);
    if (ms_pcPlatformSubDir)
    {
        NiFilename kPlatform(pcPath);
        kPlatform.SetPlatformSubDir(ms_pcPlatformSubDir);
        kPlatform.GetFullPath(pcPlatformPath, 260);
    }
    else
    {
        NiStrncpy(pcPlatformPath, 260, pcPath, strlen(pcPath));
    }
    return pcPlatformPath;
}
//---------------------------------------------------------------------------
void NiPhysXManager::ConvertFilenameToPlatformSpecific(
    const NiFixedString& kPath, NiFixedString& kPlatformSpecificPath)
{
    if (ms_pcPlatformSubDir)
    {
        char acPlatformPath[260];
        NiFilename kPlatform((const char*)kPath);
        kPlatform.SetPlatformSubDir(ms_pcPlatformSubDir);
        kPlatform.GetFullPath(acPlatformPath, 260);
        kPlatformSpecificPath = acPlatformPath;
    }
    else
    {
        kPlatformSpecificPath = kPath;
    }
}
//---------------------------------------------------------------------------
NiPhysXClothMesh* NiPhysXManager::GetClothMesh(const NxClothMesh* pkMesh)
{
    WaitMeshLock();
    NiTMapIterator iter = m_kClothMeshes.GetFirstPos();
    while (iter)
    {
        const char* pcKey;
        NiPhysXClothMeshPtr spThisMesh;
        m_kClothMeshes.GetNext(iter, pcKey, spThisMesh);
        if (spThisMesh->GetMesh() == pkMesh)
        {
            ReleaseMeshLock();    
            return spThisMesh;
        }
    }
    
    ReleaseMeshLock();
    return NULL;
}
//---------------------------------------------------------------------------
NiPhysXConvexMesh* NiPhysXManager::GetConvexMesh(const NxConvexMesh* pkMesh)
{
    WaitMeshLock();
    NiTMapIterator iter = m_kConvexMeshes.GetFirstPos();
    while (iter)
    {
        const char* pcKey;
        NiPhysXConvexMeshPtr spThisMesh;
        m_kConvexMeshes.GetNext(iter, pcKey, spThisMesh);
        if (spThisMesh->GetMesh() == pkMesh)
        {
            ReleaseMeshLock();    
            return spThisMesh;
        }
    }
    
    ReleaseMeshLock();
    return NULL;
}
//---------------------------------------------------------------------------
NiPhysXTriangleMesh* NiPhysXManager::GetTriangleMesh(
    const NxTriangleMesh* pkMesh)
{
    WaitMeshLock();
    NiTMapIterator iter = m_kTriangleMeshes.GetFirstPos();
    while (iter)
    {
        const char* pcKey;
        NiPhysXTriangleMeshPtr spThisMesh;
        m_kTriangleMeshes.GetNext(iter, pcKey, spThisMesh);
        if (spThisMesh->GetMesh() == pkMesh)
        {
            ReleaseMeshLock();    
            return spThisMesh;
        }
    }
    
    ReleaseMeshLock();    
    return NULL;
}
//---------------------------------------------------------------------------
bool NiPhysXManager::RemoveClothMesh(NiPhysXClothMesh* pkMesh)
{
    NxClothMesh* pkNxMesh = pkMesh->GetMesh();
    if (!pkNxMesh)
        return false;
    
    WaitMeshLock();

    if (pkNxMesh->getReferenceCount() != 0)
    {
        ReleaseMeshLock();    
        return false;
    }
    
    m_pkPhysXSDK->releaseClothMesh(*pkNxMesh);
    
    m_kClothMeshes.RemoveAt(pkMesh->GetName());
        
    ReleaseMeshLock();    

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXManager::RemoveConvexMesh(NiPhysXConvexMesh* pkMesh)
{
    NxConvexMesh* pkNxMesh = pkMesh->GetMesh();
    if (!pkNxMesh)
        return false;
    
    WaitMeshLock();

    if (pkNxMesh->getReferenceCount() != 0)
    {
        ReleaseMeshLock();    
        return false;
    }
    
    m_pkPhysXSDK->releaseConvexMesh(*pkNxMesh);
    
    m_kConvexMeshes.RemoveAt(pkMesh->GetName());
        
    ReleaseMeshLock();    

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXManager::RemoveTriangleMesh(NiPhysXTriangleMesh* pkMesh)
{
    NxTriangleMesh* pkNxMesh = pkMesh->GetMesh();
    if (!pkNxMesh)
        return false;
    
    WaitMeshLock();

    if (pkNxMesh->getReferenceCount() != 0)
    {
        ReleaseMeshLock();    
        return false;
    }
    
    m_pkPhysXSDK->releaseTriangleMesh(*pkNxMesh);
    
    m_kTriangleMeshes.RemoveAt(pkMesh->GetName());
        
    ReleaseMeshLock();    

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXManager::RemoveUnusedMeshes()
{
    NiTPrimitiveArray<NiPhysXClothMesh*> kClothMeshesToRemove;
    NiTPrimitiveArray<NiPhysXConvexMesh*> kConvexMeshesToRemove;
    NiTPrimitiveArray<NiPhysXTriangleMesh*> kTriMeshesToRemove;
    
    NiTMapIterator kClothIter = m_kClothMeshes.GetFirstPos();
    while (kClothIter)
    {
        const char* pcName;
        NiPhysXClothMeshPtr spMesh;
        m_kClothMeshes.GetNext(kClothIter, pcName, spMesh);
        
        if (spMesh->GetMesh() && spMesh->GetMesh()->getReferenceCount() == 0)
            kClothMeshesToRemove.Add(spMesh);
    }
    for (NiUInt32 ui = 0; ui < kClothMeshesToRemove.GetSize(); ++ui)
        RemoveClothMesh(kClothMeshesToRemove.GetAt(ui));
    kClothMeshesToRemove.RemoveAll();
    
    NiTMapIterator kConvexIter = m_kConvexMeshes.GetFirstPos();
    while (kConvexIter)
    {
        const char* pcName;
        NiPhysXConvexMeshPtr spMesh;
        m_kConvexMeshes.GetNext(kConvexIter, pcName, spMesh);
        
        if (spMesh->GetMesh() && spMesh->GetMesh()->getReferenceCount() == 0)
            kConvexMeshesToRemove.Add(spMesh);
    }
    for (NiUInt32 ui = 0; ui < kConvexMeshesToRemove.GetSize(); ++ui)
        RemoveConvexMesh(kConvexMeshesToRemove.GetAt(ui));
    kConvexMeshesToRemove.RemoveAll();
    
    NiTMapIterator kTriangleIter = m_kTriangleMeshes.GetFirstPos();
    while (kTriangleIter)
    {
        const char* pcName;
        NiPhysXTriangleMeshPtr spMesh;
        m_kTriangleMeshes.GetNext(kTriangleIter, pcName, spMesh);
        
        if (spMesh->GetMesh() && spMesh->GetMesh()->getReferenceCount() == 0)
            kTriMeshesToRemove.Add(spMesh);
    }
    for (NiUInt32 ui = 0; ui < kTriMeshesToRemove.GetSize(); ++ui)
        RemoveTriangleMesh(kTriMeshesToRemove.GetAt(ui));
    kTriMeshesToRemove.RemoveAll();
}
//---------------------------------------------------------------------------
