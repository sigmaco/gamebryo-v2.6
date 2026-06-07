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

// NiPhysXManager inline functions

//---------------------------------------------------------------------------
inline NiPhysXManager* NiPhysXManager::GetPhysXManager()
{
    return ms_pPhysXManager;
}
//---------------------------------------------------------------------------
inline void NiPhysXManager::WaitSDKLock()
{
    m_kSDKCritSect.Lock();
}
//---------------------------------------------------------------------------
inline void NiPhysXManager::ReleaseSDKLock()
{
    m_kSDKCritSect.Unlock();
}
//---------------------------------------------------------------------------
inline void NiPhysXManager::WaitMeshLock()
{
    m_kMeshCritSect.Lock();
}
//---------------------------------------------------------------------------
inline void NiPhysXManager::ReleaseMeshLock()
{
    m_kMeshCritSect.Unlock();
}
//---------------------------------------------------------------------------
inline NiPhysXClothMesh* NiPhysXManager::GetClothMesh(const char* pcPath)
{
    NiPhysXClothMeshPtr spMesh = NULL;

    WaitMeshLock();
    if (m_kClothMeshes.GetAt(pcPath, spMesh))
    {
        ReleaseMeshLock();
        return spMesh;
    }
    
    ReleaseMeshLock();
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiPhysXManager::SetClothMesh(NiPhysXClothMesh* pkMesh)
{
    WaitMeshLock();
    m_kClothMeshes.SetAt(pkMesh->GetName(), pkMesh);
    ReleaseMeshLock();
}
//---------------------------------------------------------------------------
inline NiPhysXConvexMesh* NiPhysXManager::GetConvexMesh(const char* pcPath)
{
    NiPhysXConvexMeshPtr spMesh = NULL;

    WaitMeshLock();
    if (m_kConvexMeshes.GetAt(pcPath, spMesh))
    {
        ReleaseMeshLock();
        return spMesh;
    }
    
    ReleaseMeshLock();
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiPhysXManager::SetConvexMesh(NiPhysXConvexMesh* pkMesh)
{
    WaitMeshLock();
    m_kConvexMeshes.SetAt(pkMesh->GetName(), pkMesh);
    ReleaseMeshLock();
}
//---------------------------------------------------------------------------
inline NiPhysXTriangleMesh* NiPhysXManager::GetTriangleMesh(const char* pcPath)
{
    NiPhysXTriangleMeshPtr spMesh = NULL;
    WaitMeshLock();
    if (m_kTriangleMeshes.GetAt(pcPath, spMesh))
    {
        ReleaseMeshLock();
        return spMesh;
    }    
    
    ReleaseMeshLock();
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiPhysXManager::SetTriangleMesh(NiPhysXTriangleMesh* pkMesh)
{
    WaitMeshLock();
    m_kTriangleMeshes.SetAt(pkMesh->GetName(), pkMesh);
    ReleaseMeshLock();
}
//---------------------------------------------------------------------------
