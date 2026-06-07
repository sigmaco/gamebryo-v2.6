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
#include "resource.h"
#include "NiNormalVisualizerPlugin.h"
#include <NiPSParticleSystem.h>

NiImplementRTTI(NiNormalVisualizerPlugin, NiPlugin);

float NiNormalVisualizerPlugin::ms_fNormalScale = 0.1f;
float NiNormalVisualizerPlugin::ms_fBoundScale = 1.0f;
//---------------------------------------------------------------------------
NiNormalVisualizerPlugin::NiNormalVisualizerPlugin() :
    NiPlugin("Normal Visualizer", "2.0", 
        "Creates geometry representing the normals of an object.",
        "Searches the scene graph for geometry and creates geometry "
        "representing the normals of an object")
{
}
//---------------------------------------------------------------------------
bool NiNormalVisualizerPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiNormalVisualizerPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiNormalVisualizerPlugin::FindGeometry(NiAVObject* pkRoot, 
    NiTList<NiMesh*>& kGeoms)
{
    if(NiIsKindOf(NiNode, pkRoot))
    {
        NiNode* pkNode = (NiNode*) pkRoot;

        for(unsigned int ui=0; ui < pkNode->GetArrayCount(); ui++)
        {
            if(pkNode->GetAt(ui))
            {
                FindGeometry(pkNode->GetAt(ui), kGeoms);
            }
        }
    }
    else if (NiIsKindOf(NiMesh, pkRoot) &&
        !NiIsKindOf(NiPSParticleSystem, pkRoot))
    {
        NiMesh* pkGeom = (NiMesh*)pkRoot;
        kGeoms.AddTail(pkGeom);
    }
}

//---------------------------------------------------------------------------
NiExecutionResultPtr NiNormalVisualizerPlugin::Execute(
    const NiPluginInfo*)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    NiTList<NiMesh*> kGeoms;
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);
        
        if (spScene)
            ms_fBoundScale = spScene->GetWorldBound().GetRadius() * 2.0f;
        
        FindGeometry(spScene, kGeoms);
    }

    NiTListIterator kIter = kGeoms.GetHeadPos();
    while(kIter)
    {
        CreateNormals(kGeoms.GetNext(kIter));
    }
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
void NiNormalVisualizerPlugin::CreateNormals(NiMesh* pkMesh)
{
    const NiUInt32 uiNumInputVerts = pkMesh->GetVertexCount();
    
    // Get the verts iterator
    NiDataStreamElementLock kLockVerts(pkMesh,
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_UNKNOWN,
        NiDataStream::LOCK_TOOL_READ);
    if (!kLockVerts.IsLocked())
        return;
    NiTStridedRandomAccessIterator<NiPoint3> kVertIter =
        kLockVerts.begin<NiPoint3>();
        
    // Get the normals iterator
    NiDataStreamElementLock kLockNorms(pkMesh,
        NiCommonSemantics::NORMAL(), 0, NiDataStreamElement::F_UNKNOWN,
        NiDataStream::LOCK_TOOL_READ);
    if (!kLockNorms.IsLocked())
        return;
    NiTStridedRandomAccessIterator<NiPoint3> kNormIter =
        kLockNorms.begin<NiPoint3>();
    
    NiNode* pkParent = pkMesh->GetParent();

    if (pkParent == NULL)
        return;

    // Create the buffers
    NiUInt32 uiNumOutputVerts = 2 * uiNumInputVerts;
    NiUInt32* puiIndices = 0;
    puiIndices = NiAlloc(NiUInt32, uiNumOutputVerts);
    for (NiUInt32 ui = 0; ui < uiNumOutputVerts; ui++)
    {
        puiIndices[ui] = ui;
    }

    NiPoint3* pkVertex = NiNew NiPoint3[uiNumOutputVerts]; 
    NiColorA* pkColor = NiNew NiColorA[uiNumOutputVerts]; 

    // Fill the buffers
    float fObjScale = pkMesh->GetScale();
    NiUInt32 uj = 0;
    for (NiUInt32 ui = 0; ui < uiNumInputVerts; ui++)
    {
        NiPoint3 kCurrentNormal = kNormIter[ui];
        NiPoint3 kBeginLinePos = kVertIter[ui];
        kCurrentNormal.Unitize();
        NiPoint3 kEndLinePos = kBeginLinePos + ((ms_fNormalScale * 
            ms_fBoundScale * 1.0f/fObjScale) * kCurrentNormal);
        pkVertex[uj] = kBeginLinePos;
        pkColor[uj] = NiColorA(1.0f, 0.0f, 0.0f, 1.0f);
        uj++;
        pkVertex[uj] = kEndLinePos;
        pkColor[uj] = NiColorA(1.0f, 0.0f, 0.0f, 1.0f);
        uj++;
    }

    // Build the mesh
    NiMeshPtr spLinesMesh = NiNew NiMesh();
    spLinesMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);

    NiUInt8 uiAccessMask = NiDataStream::ACCESS_CPU_WRITE_STATIC |
        NiDataStream::ACCESS_GPU_READ;

    spLinesMesh->AddStream(
        NiCommonSemantics::INDEX(), 0, NiDataStreamElement::F_UINT32_1, 
        uiNumOutputVerts, uiAccessMask, NiDataStream::USAGE_VERTEX_INDEX,
        puiIndices, true, true);
    NiFree(puiIndices);

    spLinesMesh->AddStream(
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
        uiNumOutputVerts, uiAccessMask, NiDataStream::USAGE_VERTEX,
        pkVertex, true, true);

    spLinesMesh->AddStream(
        NiCommonSemantics::COLOR(), 0, NiDataStreamElement::F_FLOAT32_4,
        uiNumOutputVerts, uiAccessMask, NiDataStream::USAGE_VERTEX,
        pkColor, true, true);

    NiBound kBound;
    kBound.ComputeFromData(uiNumInputVerts, pkVertex);
    spLinesMesh->SetModelBound(kBound);

    spLinesMesh->SetTranslate(pkMesh->GetTranslate());
    spLinesMesh->SetRotate(pkMesh->GetRotate());
    spLinesMesh->SetScale(pkMesh->GetScale());

    pkParent->AttachChild(spLinesMesh);
    
    NiDelete[] pkVertex;
    NiDelete[] pkColor;
}
//---------------------------------------------------------------------------
NiPluginInfo* NiNormalVisualizerPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiNormalVisualizerPlugin");
    pkPluginInfo->SetType("PROCESS");
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiNormalVisualizerPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool NiNormalVisualizerPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
