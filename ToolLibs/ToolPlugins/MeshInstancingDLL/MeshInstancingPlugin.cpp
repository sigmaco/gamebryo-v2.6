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
#include "MeshInstancingPlugin.h"
#include "MeshInstancingDialog.h"
#include "MeshInstancingDLLDefines.h"

#include <NiToolDataStream.h>
#include <NiMesh.h>
#include <NiMeshHWInstance.h>
#include <NiTransformController.h>

NiImplementRTTI(MeshInstancingPlugin, NiPlugin);
MeshInstancingPlugin* 
    MeshInstancingPlugin::ms_pkThis = NULL;

//---------------------------------------------------------------------------
MeshInstancingPlugin::MeshInstancingPlugin() : NiPlugin(
    "Mesh Instancing Plug-in",                // name
    "1.0",                                    // version
    "A plug-in to find meshes in the scene graph that satisfy the hardware "
    "instancing requirements.",
    // short description
    "Iterates through all the meshes in the scene. It compares data streams, "
    "materials, and other properties that are attached to a mesh object. If "
    "all of these things are equal it will replace all meshes except one with "
    "the hardware instancing node.")
    // long description
{
    m_uiNumInstancedObjects = 10;
    m_uiNumInstancedVerts = 4000;
}
//---------------------------------------------------------------------------
NiPluginInfo* MeshInstancingPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("MeshInstancingPlugin");
    pkPluginInfo->SetType("PROCESS");
    pkPluginInfo->SetInt(OPT_MESH_INSTANCING_NUM_INSTANCED_OBJECTS, 10);
    pkPluginInfo->SetInt(OPT_MESH_INSTANCING_DX9_NUM_INSTANCED_VERTS, 4000);
    pkPluginInfo->SetInt(OPT_MESH_INSTANCING_D3D10_NUM_INSTANCED_VERTS, 4000);
    pkPluginInfo->SetInt(OPT_MESH_INSTANCING_XENON_NUM_INSTANCED_VERTS, 10000);
    pkPluginInfo->SetInt(OPT_MESH_INSTANCING_PS3_NUM_INSTANCED_VERTS, 8000);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool MeshInstancingPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "MeshInstancingPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
bool MeshInstancingPlugin::ShouldCreateInstances(
    const NiUInt32 uiInstanceCount, const NiUInt32 uiVertexCount)
{
    if (uiInstanceCount > m_uiNumInstancedObjects)
        return true;

    if ((uiInstanceCount * uiVertexCount) > m_uiNumInstancedVerts)
        return true;

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr MeshInstancingPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    // Coming into this method, CanExecute has already been called.
    // Therefore, we know that we can actually execute pkInfo.

    // If this is a Wii specific export then return with success since
    // hardware instancing isn't supported
    if (NiSystemDesc::GetSystemDesc().GetToolModeRendererID() == 
        NiSystemDesc::RENDERER_WII)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
    }

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
            pkLogger->LogElement("MeshInstancingError", "The Mesh Instancing "
                "Plug-in could not find the NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The Mesh Instancing Plug-in could not find "
                "the NiSceneGraphSharedData!","Scene Graph Shared Data "
                "Missing");
        }

        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Get the platform
#ifdef NIDEBUG
    NiSharedDataList* pkDataList = 
#endif
        NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    int iTemp = (int)m_uiNumInstancedObjects;
    pkInfo->GetValue(OPT_MESH_INSTANCING_NUM_INSTANCED_OBJECTS).ToInt(
        iTemp);
    m_uiNumInstancedObjects = (NiUInt32)iTemp;

    iTemp = (int)m_uiNumInstancedVerts;
    switch (NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
    {
    case NiSystemDesc::RENDERER_DX9:
        pkInfo->GetValue(OPT_MESH_INSTANCING_DX9_NUM_INSTANCED_VERTS).ToInt(
            iTemp);
        break;
    case NiSystemDesc::RENDERER_D3D10:
        pkInfo->GetValue(OPT_MESH_INSTANCING_D3D10_NUM_INSTANCED_VERTS).ToInt(
            iTemp);
        break;
    case NiSystemDesc::RENDERER_XENON:
        pkInfo->GetValue(OPT_MESH_INSTANCING_XENON_NUM_INSTANCED_VERTS).ToInt(
            iTemp);
        break;
    case NiSystemDesc::RENDERER_PS3:
        pkInfo->GetValue(OPT_MESH_INSTANCING_PS3_NUM_INSTANCED_VERTS).ToInt(
            iTemp);
        break;
    }
    m_uiNumInstancedVerts = (NiUInt32)iTemp;

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    NiTPointerList<NiAVObject*> kObjectList;
    for (NiUInt32 ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNode* pkRoot = pkSGSharedData->GetRootAt(ui);
        pkRoot->GetObjectsByType(&NiMesh::ms_RTTI, kObjectList);
    }

    NiTPrimitiveArray<NiMesh*> kMeshObjects;
    NiTListIterator kObject = kObjectList.GetHeadPos();
    while (kObject)
    {
        NiMesh* pkMesh = (NiMesh*)kObjectList.GetNext(kObject);
        kMeshObjects.Add(pkMesh);
    }

    // Make sure we disable the check to see if objects are named the same.
    // The instance plug-in does not care if just the names are different.
    NiObjectNET::EqualityType eDefaultEqualityType = 
        NiObjectNET::GetDefaultEqualityType();
    NiObjectNET::SetDefaultEqualityType(NiObjectNET::EQUAL_NONE);

    // Iterate through all the meshes finding the ones that can be instanced.
    for (NiUInt32 uiMesh = 0; uiMesh < kMeshObjects.GetSize(); uiMesh++)
    {
        NiMesh* pkMesh = kMeshObjects[uiMesh];

        // If this is null we most likely removed it.
        if (!pkMesh)
            continue;

        NiTPrimitiveArray<NiMesh*> kInstanceMeshes;
        kInstanceMeshes.Add(pkMesh);

        for (NiUInt32 ui = uiMesh + 1; ui < kMeshObjects.GetSize(); ui++)
        {
            NiMesh* pkOtherMesh = kMeshObjects[ui];
            if (!pkOtherMesh)
                continue;

            if (pkMesh->IsInstancable(pkOtherMesh))
            {
                kInstanceMeshes.Add(pkOtherMesh);
            }
        }

        // Make sure we want to create the instances.
        NiUInt32 uiInstanceCount = kInstanceMeshes.GetSize();
        if (uiInstanceCount > 1 && 
            ShouldCreateInstances(uiInstanceCount, pkMesh->GetVertexCount()))
        {
            // Prepare the mesh for instancing
            if (NiInstancingUtilities::EnableMeshInstancing(pkMesh, 
                uiInstanceCount))
            {

                // If we are a generic asset set the index buffer to CPU_READ.
                // This is done so the renderer can convert it correctly.
                if (NiSystemDesc::GetSystemDesc().GetToolModeRendererID() == 
                    NiSystemDesc::RENDERER_GENERIC)
                {      
                    NiDataStreamRef* pkRef = pkMesh->FindStreamRef(
                        NiCommonSemantics::INDEX());

                    NIASSERT(pkRef);
                    NiDataStream* pkDS = pkRef->GetDataStream();

                    NIASSERT(NiIsKindOf(NiToolDataStream, pkDS));
                    NiToolDataStream* pkTDS = (NiToolDataStream*)pkDS;
                    pkTDS->MakeReadable();
                }

                for (NiUInt32 ui = 0; ui < uiInstanceCount; ui++)
                {
                    NiMesh* pkInstanceMesh = kInstanceMeshes[ui];

                    // Create the instance and add it to the source mesh.
                    NiMeshHWInstance* pkInstance = 
                        NiNew NiMeshHWInstance(pkMesh);
                    pkInstance->SetLocalTransform(
                        pkInstanceMesh->GetLocalTransform());
                    NiInstancingUtilities::AddMeshInstance(pkMesh, pkInstance);

                    NiNode* pkParent = pkInstanceMesh->GetParent();
                    pkParent->AttachChild(pkInstance);

                    // We only want to remove the non source mesh
                    if (pkInstanceMesh != pkMesh)
                    {
                        // Remove this mesh from all the meshes so it will
                        // not be processed.
                        kMeshObjects.Remove(pkInstanceMesh);

                        pkParent->DetachChild(pkInstanceMesh);
                    }
                    else
                    {
                        // If we are the source make sure we are the identity
                        NiTransform kTransform;
                        kTransform.MakeIdentity();
                        pkMesh->SetLocalTransform(kTransform);
                    }
                }
            }
        }
    }

    // Return the equality type back to default.
    NiObjectNET::SetDefaultEqualityType(eDefaultEqualityType);

    // Presumably, we have correctly executed our actions on the scene
    // graph. We return success in this case.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool MeshInstancingPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool MeshInstancingPlugin::DoManagementDialog(
        NiPluginInfo* pkInitialInfo, NiWindowRef hWndParent)
{
    MeshInstancingDialog kDialog(m_hModule, hWndParent, 
        pkInitialInfo);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
        break;
    case IDCANCEL:
        return false;
        break;
    default:
        NIASSERT(false);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
