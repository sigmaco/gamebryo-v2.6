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
#include "NormalUnifierPlugin.h"
#include <NiPSParticleSystem.h>

// RTTI implementation macro.
NiImplementRTTI(NormalUnifierPlugin, NiPlugin);

//---------------------------------------------------------------------------
NormalUnifierPlugin::NormalUnifierPlugin() : NiPlugin(
    "Unify Normals Plug-in",                // name
    "2.0",                                          // version
    "A plug-in to unify normals amongst separate pieces of mesh",
    // short description
    "Averages normals of separate meshes where edges line up, removing "
    "seams.\nAll objects being unified must have \"NiUnifyNormals\" added "
    "to their user-defined properties.")
    // long description
{
}
//---------------------------------------------------------------------------
NiPluginInfo* NormalUnifierPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NormalUnifierPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NormalUnifierPlugin::HasManagementDialog()
{
    // Since we have parameters that can be changed, we provide a management
    // dialog, otherwise we would return false.
    return false;
}
//---------------------------------------------------------------------------
bool NormalUnifierPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
bool NormalUnifierPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    // Verify that the NiPluginInfo object can actually be executed by
    // this plug-in. If we were to support previous versions, this is where
    // we would agree to handle the NiPluginInfo. Conversion would come in the 
    // actual Execute call.

    // Other useful values to check would be:
    // - the name of the plug-in,
    // - the application name if your plug-in is application-dependent 
    //   (which you can get from NiFramework)
    // - if any required parameters exist in this NiPluginInfo

    if (pkInfo->GetClassName() == "NormalUnifierPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NormalUnifierPlugin::Execute(
    const NiPluginInfo*)
{
    // Coming into this method, CanExecute has already been called.
    // Therefore, we know that we can actually execute pkInfo.

    // We'll likely want to use the scene graph in this process plug-in,
    // so we'll go ahead and get the scene graph shared data.
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        GetSharedData(NiGetSharedDataType(NiSceneGraphSharedData));

    // If we do not find scene graph shared data, we cannot complete the
    // processing of the scene graph. Return failure.
    if (!pkSGSharedData)
    {
        // If we want to, we can pop up an NiMessageBox giving a more
        // useful warning if not running in silent mode.
        NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
        NIASSERT(pkDataList);
        NiExporterOptionsSharedData* pkExporterSharedData = 
            (NiExporterOptionsSharedData*) 
            pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
        bool bWriteResultsToLog = false;
        NiXMLLogger* pkLogger = NULL;

        if (pkExporterSharedData)
        {
            bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
            pkLogger = pkExporterSharedData->GetXMLLogger();
        }

        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("NormalUnifierError", "The Normal Unifier "
                "Process Plug-in could not find the NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The Normal Unifier Process Plug-in could not find "
                "the NiSceneGraphSharedData!","Scene Graph Shared Data "
                "Missing");
        }

        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }


    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (NiUInt32 ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNode* pkRoot = pkSGSharedData->GetRootAt(ui);
        pkRoot->Update(0.0f);
        NiMesh::CompleteSceneModifiers(pkRoot);
        
        // Populates the array of meshes.
        CollectMesh(pkRoot);
    }
    
    PrepareVertexMap();
    CollectVertices();

    ProcessClusters();

    CleanMap();

    // Presumably, we have correctly executed our actions on the scene
    // graph. We return success in this case.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
void NormalUnifierPlugin::CollectMesh(NiAVObject* pkObject, bool bAutoAdd)
{
    if (pkObject == NULL)
        return;
    
    if (CheckForExtraDataTags(pkObject))
        bAutoAdd = true;

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;
        for (NiUInt32 ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            CollectMesh(pkNode->GetAt(ui), bAutoAdd);
        }
    }
    else if (NiIsKindOf(NiMesh, pkObject) &&
        !NiIsKindOf(NiPSParticleSystem, pkObject) &&
        bAutoAdd == true)
    {
        m_kMeshArray.Add((NiMesh*)pkObject);
    }
}
//---------------------------------------------------------------------------
void NormalUnifierPlugin::PrepareVertexMap()
{
    // Count the number of vertices, being certain not to overflow
    NiUInt32 uiNumMeshes = m_kMeshArray.GetSize();
    NiUInt32 uiNumVertices = 0;
    NiUInt32 uiMaxTableSize = NiTMap<NiPoint3*, VertexCluster*,
        NiUnifyHashFunctor, NiUnifyEqualsFunctor>::PRIMES[NiTMap<NiPoint3*,
        VertexCluster*, NiUnifyHashFunctor, NiUnifyEqualsFunctor>::NUM_PRIMES
        - 1];
    for (NiUInt32 ui = 0; ui < uiNumMeshes; ui++)
    {
        NiMesh* pkMesh = m_kMeshArray.GetAt(ui);
        NiUInt64 uiNewSum = (NiUInt64)uiNumVertices +
            (NiUInt64)pkMesh->GetVertexCount();
        if (uiNewSum >= uiMaxTableSize)
        {
            uiNumVertices = uiMaxTableSize;
            break;
        }
        uiNumVertices += pkMesh->GetVertexCount();
    }

    // Find a decent prime
    NiUInt32 uiNumVertices32 =
        uiNumVertices > 0xFFFFFFFF ? 0xFFFFFFFF : uiNumVertices;
    NiUInt32 uiTableSize = NiTMap<NiPoint3*, VertexCluster*,
        NiUnifyHashFunctor, NiUnifyEqualsFunctor>::NextPrime(uiNumVertices32);
    
    // Allocate the table
    m_pkVertexMap = NiNew NiTMap<NiPoint3*, VertexCluster*,
        NiUnifyHashFunctor, NiUnifyEqualsFunctor>(uiTableSize);
}
//---------------------------------------------------------------------------
void NormalUnifierPlugin::CollectVertices()
{
    // Every vertex is inserted into a map, with vertices in the same
    // location mapped to the same entry. Each entry in the map contains
    // the information necessary to relocate the vertex and its normal,
    // so that all co-located vertices can have their normal merged.

    NiUInt32 uiNumMeshes = m_kMeshArray.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumMeshes; ui++)
    {
        NiMesh* pkMesh = m_kMeshArray.GetAt(ui);

        // Get the verts iterator
        NiDataStreamElementLock kLockVerts(pkMesh,
            GetPositionSemantic(pkMesh), 0, NiDataStreamElement::F_UNKNOWN,
            NiDataStream::LOCK_TOOL_READ);
        if (!kLockVerts.IsLocked())
            return;
        NiTStridedRandomAccessIterator<NiPoint3> kVertIter =
            kLockVerts.begin<NiPoint3>();
            
        // Get the normals iterator
        NiDataStreamElementLock kLockNorms(pkMesh,
            GetNormalSemantic(pkMesh), 0, NiDataStreamElement::F_UNKNOWN,
            NiDataStream::LOCK_TOOL_READ);
        if (!kLockNorms.IsLocked())
            return;
        NiTStridedRandomAccessIterator<NiPoint3> kNormIter =
            kLockNorms.begin<NiPoint3>();
            
        // Get the bound and set the tolerance appropriately
        NiUnifyEqualsFunctor::SetTolerance(
            1.0e-4f * pkMesh->GetWorldBound().GetRadius());
            
        NiUInt32 uiVertCount = kLockVerts.count();
        for (NiUInt32 ui = 0; ui < uiVertCount; ui++)
            AddVertex(pkMesh, ui, kVertIter[ui], kNormIter[ui]);
    }
}
//---------------------------------------------------------------------------
const NiFixedString& NormalUnifierPlugin::GetNormalSemantic(NiMesh* pkMesh)
{
    NiSkinningMeshModifier* pkSkin = 
        NiGetModifier(NiSkinningMeshModifier, pkMesh);
    
    if (pkSkin)
    {
        return NiCommonSemantics::NORMAL_BP();
    }

    return NiCommonSemantics::NORMAL();
}
//---------------------------------------------------------------------------
const NiFixedString& NormalUnifierPlugin::GetPositionSemantic(NiMesh* pkMesh)
{
    NiSkinningMeshModifier* pkSkin = 
        NiGetModifier(NiSkinningMeshModifier, pkMesh);
    
    if (pkSkin)
    {
        return NiCommonSemantics::POSITION_BP();
    }

    return NiCommonSemantics::POSITION();
}
//---------------------------------------------------------------------------
void NormalUnifierPlugin::AddVertex(NiMesh* pkMesh, NiUInt32 uiIndex,
    NiPoint3& kPoint, NiPoint3& kNormal)
{
    // We cannot store a pointer to the point we are given because it
    // is not certain to survive for the life of processing. But we can
    // use the pointer to look for the existence of the vertex.
    
    VertexCluster* pkCluster = 0;
    if (m_pkVertexMap->GetAt(&kPoint, pkCluster))
    {
        pkCluster->AddVertex(pkMesh, uiIndex, kNormal);
    }
    else
    {
        pkCluster = NiNew VertexCluster;
        pkCluster->AddVertex(pkMesh, uiIndex, kNormal);
        
        NiPoint3* pkKey = NiNew NiPoint3(kPoint.x, kPoint.y, kPoint.z);
        m_pkVertexMap->SetAt(pkKey, pkCluster);
    }
}
//---------------------------------------------------------------------------
void NormalUnifierPlugin::ProcessClusters()
{
    NiTMapIterator kIter = m_pkVertexMap->GetFirstPos();
    while (kIter)
    {
        VertexCluster* pkCluster;
        NiPoint3* pkPoint;
        m_pkVertexMap->GetNext(kIter, pkPoint, pkCluster);

        if (pkCluster->m_kReferences.GetSize() > 1)
        {
            UnifyNormals(pkCluster);
        }
    }
}
//---------------------------------------------------------------------------
void NormalUnifierPlugin::UnifyNormals(VertexCluster* pkCluster)
{
    NiUInt32 uiNumPts = pkCluster->m_kReferences.GetSize();
    NiPoint3::UnitizeVector(pkCluster->m_kNormal);
    for (NiUInt32 ui = 0; ui < uiNumPts; ui++)
    {
        NiMesh* pkMesh = pkCluster->m_kReferences.GetAt(ui)->m_pkMesh;
        NiUInt32 uiIndex = pkCluster->m_kReferences.GetAt(ui)->m_uiIndex;

        // Get the normals iterator
        NiDataStreamElementLock kLockNorms(pkMesh,
            GetNormalSemantic(pkMesh), 0, NiDataStreamElement::F_UNKNOWN,
            NiDataStream::LOCK_TOOL_WRITE);
        if (!kLockNorms.IsLocked())
            return;
        NiTStridedRandomAccessIterator<NiPoint3> kNormIter =
            kLockNorms.begin<NiPoint3>();
            
        kNormIter[uiIndex] = pkCluster->m_kNormal;
    }    
}
//---------------------------------------------------------------------------
void NormalUnifierPlugin::CleanMap()
{
    NiTMapIterator kIter = m_pkVertexMap->GetFirstPos();
    while (kIter)
    {
        VertexCluster* pkCluster;
        NiPoint3* pkPoint;
        m_pkVertexMap->GetNext(kIter, pkPoint, pkCluster);
        NiDelete pkPoint;
        NiDelete pkCluster;
    }
    NiDelete m_pkVertexMap;
    m_pkVertexMap = 0;
    
    m_kMeshArray.RemoveAll();
}
//---------------------------------------------------------------------------
bool NormalUnifierPlugin::CheckForExtraDataTags(NiAVObject* pkObject)
{
    // THIS WAS CUT AND PASTE DIRECTLY FROM NIOPTIMIZE

    // Checks for extra data strings that indicate an object should not
    // be removed.

    // Returns: true, if the specified extra data tags exist.
    //          false, otherwise.

    const char* ppcTags[1] = {"NiUnifyNormals"};

    return CheckForExtraDataTags(pkObject, ppcTags, 1);
}
//---------------------------------------------------------------------------
bool NormalUnifierPlugin::CheckForExtraDataTags(NiAVObject* pkObject, 
    const char* ppcTags[], NiUInt32 uiNumTags)
{
    // THIS WAS CUT AND PASTE DIRECTLY FROM NIOPTIMIZE

    // Checks for the string extra data tags in ppcTags

    // Returns: true, if the specified extra data tags exist.
    //          false, otherwise.

    for (NiUInt32 i=0; i < pkObject->GetExtraDataSize(); i++)
    {
        NiExtraData* pkExtraData = pkObject->GetExtraDataAt((unsigned short)i);

        NiStringExtraData* pkStrExData = NiDynamicCast(NiStringExtraData,
                pkExtraData);
        if (pkStrExData)
        {
            const char* pcString = pkStrExData->GetValue();
            if (pcString)
            {
                for(NiUInt32 uiLoop = 0; uiLoop < uiNumTags; uiLoop++)
                {
                    if (strstr(pcString, ppcTags[uiLoop]))
                    {
                        return true;   
                    }
                }
            }
        }
    }

    return false;
}

//---------------------------------------------------------------------------
NormalUnifierPlugin::VertexCluster::VertexCluster()
    : m_kNormal(0.0f, 0.0f, 0.0f)
{
    m_kReferences.SetGrowBy(3);
}
//---------------------------------------------------------------------------
NormalUnifierPlugin::VertexCluster::~VertexCluster()
{
    for (NiUInt32 ui = 0; ui < m_kReferences.GetSize(); ui++)
    {
        NiFree(m_kReferences.GetAt(ui));
    }
}
//---------------------------------------------------------------------------
void NormalUnifierPlugin::VertexCluster::AddVertex(NiMesh*pkMesh,
    NiUInt32 uiIndex, NiPoint3& kNormal)
{
    Reference* pkRef = NiAlloc(Reference, 1);
    pkRef->m_pkMesh = pkMesh;
    pkRef->m_uiIndex = uiIndex;
    m_kReferences.Add(pkRef);
    
    m_kNormal += kNormal;
}
//---------------------------------------------------------------------------
unsigned int NiUnifyHashFunctor::KeyToHashIndex(NiPoint3* key,
    unsigned int uiTableSize)
{
    return (unsigned int)((unsigned int)key->x | (unsigned int)key->y
        | (unsigned int)key->z) % uiTableSize;
}
//---------------------------------------------------------------------------
float NiUnifyEqualsFunctor::m_fTolerance = 1.0e-4f;
//---------------------------------------------------------------------------
bool NiUnifyEqualsFunctor::IsKeysEqual(NiPoint3* key1, NiPoint3* key2)
{
    // Determines if points are within a box of half-side m_fTolerance.

    // Returns: true, if kP1 is within fEpsilon of kP2.
    //          false, otherwise.

    if (NiAbs(key1->x - key2->x) > m_fTolerance)
    {
        return false;
    }
    if (NiAbs(key1->y - key2->y) > m_fTolerance)
    {
        return false;
    }
    if (NiAbs(key1->z - key2->z) > m_fTolerance)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiUnifyEqualsFunctor::SetTolerance(const float fTolerance)
{
    m_fTolerance = fTolerance;
}
//-----------------------------------------------------------------------
