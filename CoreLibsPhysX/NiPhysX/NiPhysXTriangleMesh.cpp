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

NiImplementRTTI(NiPhysXTriangleMesh, NiObject);

//---------------------------------------------------------------------------
NiPhysXTriangleMesh::NiPhysXTriangleMesh() : m_kName(NULL), m_pkMesh(0)
{
    m_bStreamInline = true;
    m_pkMeshDesc = 0;
}
//---------------------------------------------------------------------------
NiPhysXTriangleMesh::~NiPhysXTriangleMesh()
{
    // The PhysX SDK owns the mesh and deletes it.
}
//---------------------------------------------------------------------------
NiPhysXTriangleMesh* NiPhysXTriangleMesh::Create(NiPhysXMeshDesc* pkDesc,
    const NiFixedString& kFilename)
{
    NiPhysXTriangleMesh* pkResult = NiNew NiPhysXTriangleMesh;
    NIASSERT(pkResult);
 
    pkResult->m_kName = kFilename;
    NiStandardizeFilePath(pkResult->m_kName);

    NiPhysXManager::ConvertFilenameToPlatformSpecific(
        pkResult->m_kName, pkResult->m_kName);

    if (!pkResult->CreateMesh())
    {
        NiDelete pkResult;
        return 0;
    }
    else
    {
        pkResult->m_pkMeshDesc = pkDesc;
        return pkResult;
    }
}
//---------------------------------------------------------------------------
NiPhysXTriangleMesh* NiPhysXTriangleMesh::Create(NiPhysXMeshDesc* pkDesc,
    const NiPhysXMemStream& kStream, const NiFixedString& kName)
{
    NiPhysXTriangleMesh* pkResult = NiNew NiPhysXTriangleMesh;
    NIASSERT(pkResult);
 
    pkResult->m_kName = kName;

    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    
    pkManager->WaitSDKLock();
    pkResult->m_pkMesh = pkManager->m_pkPhysXSDK->createTriangleMesh(kStream);
    pkManager->ReleaseSDKLock();
    
    if (!pkResult->m_pkMesh)
    {
        NiDelete pkResult;
        return NULL;
    }
    else
    {
        pkResult->m_pkMeshDesc = pkDesc;
        return pkResult;
    }
}
//---------------------------------------------------------------------------
bool NiPhysXTriangleMesh::CreateMesh()
{
    NiPhysXFileStream* pkStream =
        NiNew NiPhysXFileStream(m_kName, NiFile::READ_ONLY);
    if (!pkStream->Ready())
    {
        size_t stSize = strlen((const char *)m_kName) + 256;
        char* pcMsg = (char*)NiMalloc(sizeof(char) * stSize);
        NiSprintf(pcMsg, stSize,
            "NiPhysXTriangleMesh::CreateMesh cannot open file %s\n",
            pcMsg);
        NiOutputDebugString(pcMsg);
        NiFree(pcMsg);
        NiDelete pkStream;
        return false;
    }
    
    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    
    pkManager->WaitSDKLock();
    m_pkMesh = pkManager->m_pkPhysXSDK->createTriangleMesh(*pkStream);
    pkManager->ReleaseSDKLock();
    
    if (!m_pkMesh)
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
