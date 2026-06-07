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

#include "NiPhysXClothMesh.h"
#include "NiPhysXFileStream.h"
#include "NiPhysXManager.h"
#include "NiPhysXMeshDesc.h"

NiImplementRTTI(NiPhysXClothMesh, NiObject);

//---------------------------------------------------------------------------
NiPhysXClothMesh::NiPhysXClothMesh() : m_kName(NULL), m_pkMesh(0)
{
    m_bStreamInline = true;
    m_pkMeshDesc = 0;
    m_fArea = 0.0f;
}
//---------------------------------------------------------------------------
NiPhysXClothMesh::~NiPhysXClothMesh()
{
}
//---------------------------------------------------------------------------
NiPhysXClothMesh* NiPhysXClothMesh::Create(NiPhysXMeshDesc* pkDesc,
    const NiFixedString& kFilename)
{
    NiPhysXClothMesh* pkResult = NiNew NiPhysXClothMesh;
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
NiPhysXClothMesh* NiPhysXClothMesh::Create(NiPhysXMeshDesc* pkDesc,
    NiPhysXMemStream& kStream, const NiFixedString& kName)
{
    NiPhysXClothMesh* pkResult = NiNew NiPhysXClothMesh;
    NIASSERT(pkResult);
 
    pkResult->m_kName = kName;

    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    
    pkManager->WaitSDKLock();
    pkResult->m_pkMesh = pkManager->m_pkPhysXSDK->createClothMesh(kStream);
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
bool NiPhysXClothMesh::CreateMesh()
{
    NiPhysXFileStream* pkStream =
        NiNew NiPhysXFileStream(m_kName, NiFile::READ_ONLY);
    if (!pkStream->Ready())
    {
        size_t stSize = strlen((const char *)m_kName) + 256;
        char* pcMsg = (char*)NiMalloc(sizeof(char) * stSize);
        NiSprintf(pcMsg, stSize,
            "NiPhysXClothMesh::CreateMesh cannot open file %s\n",
            pcMsg);
        NiOutputDebugString(pcMsg);
        NiFree(pcMsg);
        NiDelete pkStream;
        return false;
    }
    
    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    
    pkManager->WaitSDKLock();
    m_pkMesh = pkManager->m_pkPhysXSDK->createClothMesh(*pkStream);
    pkManager->ReleaseSDKLock();
    
    if (!m_pkMesh)
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
float NiPhysXClothMesh::GetArea()
{
    if (m_fArea > 0.0f)
        return m_fArea;
        
    NxClothMeshDesc kClothMeshDesc;
    m_pkMesh->saveToDesc(kClothMeshDesc);

    NIASSERT(kClothMeshDesc.pointStrideBytes == sizeof(NxVec3));

    m_fArea = 0.0f;
    NxVec3* pkVertices = (NxVec3*)kClothMeshDesc.points;
    for (NiUInt32 ui = 0; ui < kClothMeshDesc.numTriangles; ui++)
    {
        // The returned descriptor seems to have 32 bit indices, even though
        // the flags say 16 bit and the mesh was created with 16 bit.

        NiUInt32 uiI0 = *(NiUInt32*)(
                ((char*)kClothMeshDesc.triangles) +
                kClothMeshDesc.triangleStrideBytes * ui);
        NiUInt32 uiI1 = *(NiUInt32*)(
                ((char*)kClothMeshDesc.triangles) +
                kClothMeshDesc.triangleStrideBytes * ui +
                kClothMeshDesc.triangleStrideBytes / 3);
        NiUInt32 uiI2 = *(NiUInt32*)(
                ((char*)kClothMeshDesc.triangles) +
                kClothMeshDesc.triangleStrideBytes * ui +
                2 * kClothMeshDesc.triangleStrideBytes / 3);

        NxVec3 kE1 = pkVertices[uiI1] - pkVertices[uiI0];
        NxVec3 kE2 = pkVertices[uiI2] - pkVertices[uiI0];

        NxVec3 kCross = kE1 ^ kE2;
        m_fArea += 0.5f * kCross.magnitude();
    }

    return m_fArea;
}
//---------------------------------------------------------------------------
