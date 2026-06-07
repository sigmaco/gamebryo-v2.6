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

#include <stdafx.h>
#include "NiSceneGraphOptimizationPlugin.h"
#include "NiSceneGraphOptimizationDialog.h"
#include "NiSceneGraphOptimizationDefines.h"
#include <NiBoneLODController.h>

NiImplementRTTI(NiSceneGraphOptimizationPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiSceneGraphOptimizationPlugin::NiSceneGraphOptimizationPlugin() :
    NiPlugin("Scene Graph Optimization", "3.0", "Optimize scene graph for "
    "speed and size.", "Optimizes scene, removes childless nodes, duplicates "
    "properties, and much, much more.")
{
    m_bShowProgress = true;
}
//---------------------------------------------------------------------------
bool NiSceneGraphOptimizationPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiSceneGraphOptimizationPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiSceneGraphOptimizationPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    bool bViewerOptimize = false;
    bool bViewerRun = false;
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));

    if (pkExporterSharedData)
    {
        bViewerRun = pkExporterSharedData->GetViewerRun();
        bViewerOptimize = pkExporterSharedData->GetOptimizeMeshForViewer();
    }

    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    NiString strTrue = NiString::FromBool(true);

    // Get script version.
    unsigned int uiScriptVersion = NiPluginHelpers::GetVersionFromString(
        pkInfo->GetVersion());
    
    // Read the Parameters
    m_bRemoveDupProperties = pkInfo->GetBool(OPT_REMOVE_DUP_PROPERTIES);
    m_bRemoveChildlessNodes = pkInfo->GetBool(OPT_REMOVE_CHILDLESS_NODES);
    m_bRemoveMultiMtlNodes = pkInfo->GetBool(OPT_REMOVE_MULTIMTL_NODES);
    m_bRemoveSingleChildNodes = pkInfo->GetBool(OPT_REMOVE_SINGLE_CHILD_NODES);
    m_bMergeSiblingNodes = pkInfo->GetBool(OPT_MERGE_SIBLING_NODES);

    // If we are exporting out to a viewer we may not want to optimize because
    // it takes to long.
    m_bOptimizeTriShapes = (!bViewerRun || bViewerOptimize) ?
        pkInfo->GetBool(OPT_OPTIMIZE_TRISHAPES) : false;

    pkInfo->GetValue(OPT_OPTIMIZE_MESH_SIZE_MODIFIER).ToFloat(
        m_fOptimizeMeshSizeModifier);
    pkInfo->GetValue(OPT_OPTIMIZE_MESH_NORMAL_MODIFIER).ToFloat(
        m_fOptimizeMeshNormalModifier);
    pkInfo->GetValue(OPT_OPTIMIZE_MESH_POSITION_MODIFIER).ToFloat(
        m_fOptimizeMeshPositionModifier);

    int iTemp = (int)m_uiOptimizeMeshVertexCacheSize;
    switch (NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
    {
    case NiSystemDesc::RENDERER_DX9:
    case NiSystemDesc::RENDERER_GENERIC:
        pkInfo->GetValue(OPT_OPTIMIZE_DX9_MESH_VERTEX_CACHE_SIZE).ToInt(
            iTemp);
        break;
    case NiSystemDesc::RENDERER_D3D10:
        pkInfo->GetValue(OPT_OPTIMIZE_D3D10_MESH_VERTEX_CACHE_SIZE).ToInt(
            iTemp);
        break;
    case NiSystemDesc::RENDERER_WII:
        pkInfo->GetValue(OPT_OPTIMIZE_WII_MESH_VERTEX_CACHE_SIZE).ToInt(
            iTemp);
        break;
    case NiSystemDesc::RENDERER_XENON:
        pkInfo->GetValue(OPT_OPTIMIZE_XENON_MESH_VERTEX_CACHE_SIZE).ToInt(
            iTemp);
        break;
    case NiSystemDesc::RENDERER_PS3:
        pkInfo->GetValue(OPT_OPTIMIZE_PS3_MESH_VERTEX_CACHE_SIZE).ToInt(
            iTemp);
    }
    m_uiOptimizeMeshVertexCacheSize = (NiUInt32)iTemp;

    m_bMergeSiblingMeshes =pkInfo->GetBool(OPT_MERGE_SIBLING_TRISHAPES);
    m_bNoMergeOnAlpha = pkInfo->GetBool(OPT_NO_MERGE_ON_ALPHA);
    m_bRemoveExtraUVSets = pkInfo->GetBool(OPT_REMOVE_EXTRA_UVSETS);
    m_bRemoveUnnecessaryNormals = pkInfo->GetBool(
        OPT_REMOVE_UNNECESSARY_NORMALS);
    m_bRemoveUnnecessaryVisControllers = pkInfo->GetBool(
        OPT_REMOVE_UNNECESSARY_VISCONTROLLERS);
    m_bRemoveNames = pkInfo->GetBool(OPT_REMOVE_NAMES);
    m_bNameUnnamedObjects = pkInfo->GetBool(OPT_NAME_UNNAMED_OBJECTS);
    m_bRemoveBadTexturingProps = pkInfo->GetBool(
        OPT_REMOVE_BAD_TEXTURING_PROPS);
    m_bRemoveUnnecessaryExtraData = pkInfo->GetBool(
        OPT_REMOVE_UNNECESSARY_EXTRA_DATA);
    m_bRemoveSgoKeepTags = pkInfo->GetBool(OPT_REMOVE_SGOKEEP_TAGS);
    m_bRemoveUnnecessaryLookAtControllers = 
        pkInfo->GetBool(OPT_REMOVE_UNNECESSARY_LOOKATCONTROLLERS);
    m_bRemoveHiddenBoneMesh = 
        pkInfo->GetBool(OPT_REMOVE_HIDDEN_BONE_MESH);
    m_bReduceAnimationKeys = 
        pkInfo->GetBool(OPT_REDUCE_ANIMATION_KEYS);

    if(!pkInfo->GetValue(OPT_ANIMATION_KEY_REDUCTION_TOLERANCE).ToFloat(
        m_fAnimationKeyReductionTolerance))
    {
        // This case should give a warning
        m_bReduceAnimationKeys = false;
    }

    m_bSortByTexturingProp = pkInfo->GetBool(OPT_SORT_BY_TEXTURING_PROP);

    if (CompareVersions(uiScriptVersion, "1.2"))
    {
        m_bWeldSkin = true;
    }
    else
    {
        m_bWeldSkin = pkInfo->GetBool(OPT_WELD_SKIN);
    }

    int iBones = 4;
    if(!pkInfo->GetValue(OPT_BONES_PER_PARTITION).ToInt(iBones))
    {
        // This case should give a warning
    }

    NiNodePtr spRoot = pkSGSharedData->GetFullSceneGraph();
    for (unsigned int ui = 0; ui < spRoot->GetArrayCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);
        m_uiBonesPerPartition = (unsigned int)iBones;

        // Force software skinning on Wii
        if (NiSystemDesc::GetSystemDesc().GetToolModeRendererID() == 
            NiSystemDesc::RENDERER_WII)
        {
            m_bExportSoftwareSkin = true;
            m_bExportHardwareSkin = false;
        }
        else
        {
            m_bExportSoftwareSkin = pkInfo->GetBool(OPT_EXPORT_SOFTWARE_SKIN);
            m_bExportHardwareSkin = pkInfo->GetBool(OPT_EXPORT_HARDWARE_SKIN);
        }

        if (m_bExportHardwareSkin)
            m_bExportSoftwareSkin = false;

        if (NiSystemDesc::GetSystemDesc().GetToolModeRendererID() == 
            NiSystemDesc::RENDERER_XENON)
        {
            // Force bones per partition to 30 on Xenon since that's what the
            // default pipeline supports. Shaders can override during the
            // optimize pass. This value must be kept in synch with the 
            // value in NDL_VSFragments::TransformSkinnedPosition and the
            // value in NiXenonRenderer::m_uiHWBones.
            m_uiBonesPerPartition = 30;
            m_bExportHardwareSkin = true;
        }

        Optimize(spScene);
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiSceneGraphOptimizationPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiSceneGraphOptimizationPlugin");
    pkPluginInfo->SetType("PROCESS");

    pkPluginInfo->SetFloat(OPT_ANIMATION_KEY_REDUCTION_TOLERANCE, 0.0004f);
    pkPluginInfo->SetBool(OPT_REMOVE_DUP_PROPERTIES, true);
    pkPluginInfo->SetBool(OPT_REMOVE_CHILDLESS_NODES, true);
    pkPluginInfo->SetBool(OPT_REMOVE_MULTIMTL_NODES, true);
    pkPluginInfo->SetBool(OPT_REMOVE_SINGLE_CHILD_NODES, true);
    pkPluginInfo->SetBool(OPT_MERGE_SIBLING_NODES, true);
    pkPluginInfo->SetBool(OPT_OPTIMIZE_TRISHAPES,true);
    pkPluginInfo->SetFloat(OPT_OPTIMIZE_MESH_SIZE_MODIFIER, 1.0f);
    pkPluginInfo->SetFloat(OPT_OPTIMIZE_MESH_NORMAL_MODIFIER, 1.0f);
    pkPluginInfo->SetFloat(OPT_OPTIMIZE_MESH_POSITION_MODIFIER, 1.0f);
    pkPluginInfo->SetInt(OPT_OPTIMIZE_DX9_MESH_VERTEX_CACHE_SIZE, 16);
    pkPluginInfo->SetInt(OPT_OPTIMIZE_D3D10_MESH_VERTEX_CACHE_SIZE, 16);
    pkPluginInfo->SetInt(OPT_OPTIMIZE_XENON_MESH_VERTEX_CACHE_SIZE, 15);
    pkPluginInfo->SetInt(OPT_OPTIMIZE_PS3_MESH_VERTEX_CACHE_SIZE, 24);
    pkPluginInfo->SetInt(OPT_OPTIMIZE_WII_MESH_VERTEX_CACHE_SIZE, 8);
    pkPluginInfo->SetBool(OPT_MERGE_SIBLING_TRISHAPES, true);
    pkPluginInfo->SetBool(OPT_NO_MERGE_ON_ALPHA,true);
    pkPluginInfo->SetBool(OPT_REMOVE_EXTRA_UVSETS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_UNNECESSARY_NORMALS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_UNNECESSARY_VISCONTROLLERS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_NAMES, false);
    pkPluginInfo->SetBool(OPT_NAME_UNNAMED_OBJECTS, false);
    pkPluginInfo->SetBool(OPT_REMOVE_BAD_TEXTURING_PROPS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_UNNECESSARY_EXTRA_DATA, true);
    pkPluginInfo->SetBool(OPT_REMOVE_SGOKEEP_TAGS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_UNNECESSARY_LOOKATCONTROLLERS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_HIDDEN_BONE_MESH,true);
    pkPluginInfo->SetBool(OPT_REDUCE_ANIMATION_KEYS,true);
    pkPluginInfo->SetBool(OPT_SORT_BY_TEXTURING_PROP,false);
    pkPluginInfo->SetBool(OPT_WELD_SKIN, true);
    pkPluginInfo->SetBool(OPT_EXPORT_SOFTWARE_SKIN, false);
    pkPluginInfo->SetBool(OPT_EXPORT_HARDWARE_SKIN, true);
    pkPluginInfo->SetInt(OPT_BONES_PER_PARTITION,30);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiSceneGraphOptimizationPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiSceneGraphOptimizationPlugin::DoManagementDialog(
    NiPluginInfo* pkInitialInfo, NiWindowRef hWndParent)
{
    // Convert old version script, if necessary.
    bool bConvertedOldScript = false;
    unsigned int uiScriptVersion = NiPluginHelpers::GetVersionFromString(
        pkInitialInfo->GetVersion());
    if (CompareVersions(uiScriptVersion, m_strVersion))
    {
        if (CompareVersions(uiScriptVersion, "1.2"))
        {
            pkInitialInfo->SetBool(OPT_WELD_SKIN, true);
        }

        pkInitialInfo->SetVersion(m_strVersion);
        bConvertedOldScript = true;
    }

    // We only support software or hardware skinning currently.
    if (pkInitialInfo->GetBool(OPT_EXPORT_SOFTWARE_SKIN) &&
        pkInitialInfo->GetBool(OPT_EXPORT_HARDWARE_SKIN))
    {
        pkInitialInfo->SetBool(OPT_EXPORT_SOFTWARE_SKIN, false);
        bConvertedOldScript = true;
    }

    // Make sure all the new options are present
    NiPluginInfoPtr spDefaultInfo = GetDefaultPluginInfo();
    if (spDefaultInfo)
    {
        NiUInt32 uiDefaultParamCount = spDefaultInfo->GetParameterCount();
        NiUInt32 uiInitParamCount = pkInitialInfo->GetParameterCount();
        NI_UNUSED_ARG(uiInitParamCount);
        for (NiUInt32 ui=0; ui < uiDefaultParamCount; ui++)
        {
            NiParameterInfo* pkInfo = spDefaultInfo->GetParameterAt(ui);
            NiUInt32 uiIndex = pkInitialInfo->GetParameterIndex(
                pkInfo->GetKey());
            if (uiIndex == NIPT_INVALID_INDEX)
            {
                pkInitialInfo->AddParameter(pkInfo);
            }
        }
    }

    NiSceneGraphOptimizationDialog kDialog(m_hModule, hWndParent,
        pkInitialInfo);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
        return true;
        break;
    case IDCANCEL:
        break;
    default:
        NIASSERT(false);
    }

    return bConvertedOldScript;
}


//---------------------------------------------------------------------------
// Progress bar.
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationPlugin::InitProgressBar()
{
    if(m_bShowProgress)
    {
        m_pkProgressBar = NiNew NiProgressDialog("Optimizing Scene Graph");
        m_pkProgressBar->Create();
        m_pkProgressBar->SetRangeSpan(21);
        m_pkProgressBar->SetPosition(0);
        m_pkProgressBar->SetLineOne("Optimization Setting:");
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationPlugin::UpdateProgressBar(const char* pcLine, 
    bool bDoOption)
{

    if(m_bShowProgress && m_pkProgressBar != NULL)
    {
        m_pkProgressBar->StepIt();
        const unsigned int uiStrLen = 512;
        char acString[uiStrLen];
        if (bDoOption)
        {
            NiSprintf(acString, uiStrLen, "%s ...", pcLine);
        }
        else
        {
            NiSprintf(acString, uiStrLen, "%s (Skipping)", pcLine);
        }
        m_pkProgressBar->SetLineTwo(acString);
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationPlugin::CloseProgressBar()
{
    if(m_bShowProgress)
    {
        m_pkProgressBar->Destroy();
        NiDelete m_pkProgressBar;
        m_pkProgressBar = NULL;
    }
}
//---------------------------------------------------------------------------
// General functions.
//---------------------------------------------------------------------------
void OptimizeSkinData(NiToolPipelineCloneHelper& kCloneHelper,
    bool bHardwareSkin, bool bSoftwareSkin, unsigned int uiBonesPerPartition,
    bool bForcePalette)
{
    // Working Assumptions
    //    1. data-streams may be shared or not shared.
    //    2. if any datastreams are shared, then all of those shared data
    //       streams are shared by the same clone-set.
    //       Ie, we can't have Mesh1, Mesh2, and Mesh3 share one data-stream 
    //       and have Mesh1 and Mesh2 share another data-stream.
    //       But, it is ok if Mesh1, Mesh2, and Mesh3 have unshared
    //       data-streams.
    //    3. If data-streams are shared between mesh,then those mesh either
    //       share the same shader, material, and texturingProperties OR
    //       we can treat them as the equivalent.
    //    4. Sharing BONE_PALETTES amoung clones is desired.

    // Method
    //    1. Find all clonesets having more than 1 mesh.
    //       (We can assume that each set has unique mesh)
    //    2. Of the remaining clonesets that only have 1 mesh, only add those
    //       that don't contain mesh of the previous clonesets.
    //    3. Process each cloneset
    NiTObjectSet<NiToolPipelineCloneHelper::CloneSetPtr> kAllCloneSets;
    kCloneHelper.GetAllCloneSets(kAllCloneSets);

    NiTObjectSet<NiToolPipelineCloneHelper::CloneSetPtr> kFilteredCloneSets;

    NiTPointerMap<NiMesh*, bool> kMeshMap;
    // Find clonesets having more than 1 mesh. Hash those sets
    for(NiUInt32 ui=0; ui < kAllCloneSets.GetSize(); ui++)
    {
        NiToolPipelineCloneHelper::CloneSetPtr spCloneSet =
            kAllCloneSets.GetAt(ui);
        if (spCloneSet->GetSize() < 2)
            continue;

        // Hash each mesh in the cloneset and store the cloneset
        kFilteredCloneSets.Add(spCloneSet);
        NiUInt32 uiMeshCnt = spCloneSet->GetSize();
        for(NiUInt32 uiMesh=0; uiMesh < uiMeshCnt; uiMesh++)
        {
            NiMesh* pkMesh = spCloneSet->GetAt(uiMesh);
#ifdef NIDEBUG
            bool bTmp = false;
#endif
            NIASSERT(kMeshMap.GetAt(pkMesh, bTmp) == false);
            kMeshMap.SetAt(pkMesh, true);
        }
    }

    // Find clonsets having exactly 1 mesh. If not in the hash, hash set
    for(NiUInt32 ui=0; ui < kAllCloneSets.GetSize(); ui++)
    {
        NiToolPipelineCloneHelper::CloneSetPtr spCloneSet =
            kAllCloneSets.GetAt(ui);
        if (spCloneSet->GetSize() != 1)
            continue;

        NiMesh* pkMesh = spCloneSet->GetAt(0);

        bool bTmp = false;
        if (kMeshMap.GetAt(pkMesh, bTmp) == true)
            continue; // already have this mesh registered

        kMeshMap.SetAt(pkMesh, true);
        kFilteredCloneSets.Add(spCloneSet);
    }

    // Iterate over each cloneset that we are interested in.
    NiUInt32 uiFilteredCnt = kFilteredCloneSets.GetSize();
    for(NiUInt32 ui=0; ui < uiFilteredCnt; ui++)
    {
        NiToolPipelineCloneHelper::CloneSetPtr spCloneSet =
            kFilteredCloneSets.GetAt(ui);

        // Each mesh in the clone-set is expected to have a skinning mesh
        // modifier.
        NiTPrimitiveSet<NiSkinningMeshModifier*> kSkinModList;
        NiTPrimitiveSet<NiMorphMeshModifier*> kMorphModList;
        NiUInt32 uiMeshCnt = spCloneSet->GetSize();
        for(NiUInt32 uiMesh = 0; uiMesh < uiMeshCnt; uiMesh++)
        {
                NiMesh* pkMesh = spCloneSet->GetAt(uiMesh);
                NIASSERT(pkMesh);

                NiSkinningMeshModifier* pkSkinMod = NiGetModifier(
                    NiSkinningMeshModifier, pkMesh);

                if (pkSkinMod == NULL)
                    break;

                kSkinModList.Add(pkSkinMod);

                // Add the morphing to the list
                NiMorphMeshModifier* pkMorphMod = NiGetModifier(
                    NiMorphMeshModifier, pkMesh);
                if (pkMorphMod)
                {
                    kMorphModList.Add(pkMorphMod);
                }
        }

        // Every mesh must have a skinning modifier and it is expected that
        // they have the same parameters and requirements.
        if (uiMeshCnt != kSkinModList.GetSize())
            continue;

        NiOptimize::OptimizeSkinModifier(kCloneHelper, spCloneSet,kSkinModList,
            kMorphModList, bHardwareSkin, bSoftwareSkin, uiBonesPerPartition,
            bForcePalette);
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationPlugin::Optimize(NiNode* pkScene)
{
    InitProgressBar();

    pkScene->Update(0.0f);
    NiMesh::CompleteSceneModifiers(pkScene);
    pkScene->UpdateProperties();
    pkScene->UpdateEffects();
    
    UpdateProgressBar("Removing extra UV sets", m_bRemoveExtraUVSets); 
    if (m_bRemoveExtraUVSets)
    {
        NiOptimize::RemoveExtraUVSets(pkScene);
    }

    UpdateProgressBar("Removing bad texturing properties", 
        m_bRemoveBadTexturingProps);
    if (m_bRemoveBadTexturingProps)
    {
        NiOptimize::RemoveBadTexturingProps(pkScene);
    }

    UpdateProgressBar("Removing duplicate properties",
        m_bRemoveDupProperties);
    if (m_bRemoveDupProperties)
    {
        NiOptimize::RemoveDupProperties(pkScene);
    }

    UpdateProgressBar("Removing multi-material nodes",
        m_bRemoveMultiMtlNodes);
    if (m_bRemoveMultiMtlNodes)
    {
        NiOptimize::RemoveMultiMtlNodes(pkScene);
    }

    UpdateProgressBar("Removing childless nodes", m_bRemoveChildlessNodes);
    if (m_bRemoveChildlessNodes)
    {
        NiOptimize::RemoveChildlessNodes(pkScene);
    }

    UpdateProgressBar("Removing single child nodes", 
        m_bRemoveSingleChildNodes);
    if (m_bRemoveSingleChildNodes)
    {
        NiOptimize::RemoveSingleChildNodes(pkScene);
    }

    UpdateProgressBar("Merging sibling nodes", m_bMergeSiblingNodes);
    if (m_bMergeSiblingNodes)
    {
        NiOptimize::MergeSiblingNodes(pkScene);
    }

    UpdateProgressBar("Merging sibling meshes",
        m_bMergeSiblingMeshes);
    if (m_bMergeSiblingMeshes)
    {
        NiOptimize::MergeSiblingMeshes(pkScene, m_bNoMergeOnAlpha);
    }

    UpdateProgressBar("Removing unnecessary normals", 
        m_bRemoveUnnecessaryNormals);
    if (m_bRemoveUnnecessaryNormals)
    {
        NiOptimize::RemoveUnnecessaryNormals(pkScene);
    }

    UpdateProgressBar("Removing unnecessary NiVisControllers", 
        m_bRemoveUnnecessaryVisControllers);
    if (m_bRemoveUnnecessaryVisControllers)
    {
        NiOptimize::RemoveUnnecessaryVisControllers(pkScene);
    }

    UpdateProgressBar("Removing unneccessary NiLookAtControllers",
        m_bRemoveUnnecessaryLookAtControllers);
    if (m_bRemoveUnnecessaryLookAtControllers)
    {
        NiOptimize::RemoveUnnecessaryLookAtControllers(pkScene);
    }
    
     UpdateProgressBar("Removing hidden bone mesh", 
         m_bRemoveHiddenBoneMesh);
     if (m_bRemoveHiddenBoneMesh)
     {
         NiOptimize::RemoveHiddenBoneMesh(pkScene);
     }

    UpdateProgressBar("Reducing animation keys", m_bReduceAnimationKeys);
    if (m_bReduceAnimationKeys)
    {
        NiOptimize::ReduceAnimationKeys(pkScene, 
            m_fAnimationKeyReductionTolerance);
    }

    UpdateProgressBar("Cleaning up animation keys", true);
    NiOptimize::CleanUpAnimationKeys(pkScene);

    UpdateProgressBar("Sorting siblings by texturing property", 
        m_bSortByTexturingProp);
    if (m_bSortByTexturingProp)
    {
        NiOptimize::SortByTexturingProp(pkScene);
    }

    // should be done before optimizing skin data
    UpdateProgressBar("Optimizing NiMeshes", m_bOptimizeTriShapes);
    if (m_bOptimizeTriShapes)
    {
        NiOptimize::OptimizeMeshes(pkScene, m_fOptimizeMeshSizeModifier,
            m_fOptimizeMeshNormalModifier, m_fOptimizeMeshPositionModifier,
            m_uiOptimizeMeshVertexCacheSize);
    }

    // should be done after optimizing trishapes but before optimizing
    // skin data
    UpdateProgressBar("Creating skinning LOD controllers", true);
    NiOptimize::CreateSkinningLODControllers(pkScene);

    // should be done before stripify
    // The fifth argument to OptimizeSkinData used to be console-specific,
    // but the cosoles that needed it to be false are no longer supported.
    UpdateProgressBar("Optimizing skin data", true);
    NiToolPipelineCloneHelper kCloneHelper(pkScene);
    OptimizeSkinData(kCloneHelper, m_bExportHardwareSkin, 
        m_bExportSoftwareSkin, m_uiBonesPerPartition, true);

    // must be done after creation of skin partitions
    UpdateProgressBar("Welding skinned objects", m_bWeldSkin);
    if (m_bWeldSkin)
    {
        NiOptimize::WeldSkin(pkScene);
    }

    UpdateProgressBar("Naming unnamed objects", m_bNameUnnamedObjects);
    if (m_bNameUnnamedObjects)
    {
        NiOptimize::NameUnnamedObjects(pkScene);
    }

    UpdateProgressBar("Removing names", m_bRemoveNames);
    if (m_bRemoveNames)
    {
        NiOptimize::RemoveNames(pkScene);
    }

    UpdateProgressBar("Removing unnecessary extra data",
        m_bRemoveUnnecessaryExtraData );
    if (m_bRemoveUnnecessaryExtraData)
    {
        NiOptimize::RemoveUnnecessaryExtraData(pkScene, m_bRemoveSgoKeepTags);
    }

    CloseProgressBar();
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationPlugin::ResetDefaultOptions()
{
    m_bRemoveDupProperties = true;
    m_bRemoveChildlessNodes = true;
    m_bRemoveMultiMtlNodes = true;
    m_bRemoveSingleChildNodes = true;
    m_bMergeSiblingNodes = true;
    m_bOptimizeTriShapes = true;
    m_fOptimizeMeshSizeModifier = 1.0f;
    m_fOptimizeMeshNormalModifier = 1.0f;
    m_fOptimizeMeshPositionModifier = 1.0f;
    m_uiOptimizeMeshVertexCacheSize = 16;
    m_bMergeSiblingMeshes = true;
    m_bNoMergeOnAlpha = true;
    m_bRemoveExtraUVSets = true;
    m_bRemoveUnnecessaryNormals = true;
    m_bRemoveUnnecessaryVisControllers = true;
    m_bRemoveNames = false;
    m_bNameUnnamedObjects = false;
    m_bRemoveBadTexturingProps = true;
    m_bRemoveUnnecessaryExtraData = true;
    m_bRemoveSgoKeepTags = true;
    m_bRemoveUnnecessaryLookAtControllers = true;
    m_bRemoveHiddenBoneMesh = true;
    m_bReduceAnimationKeys = true;
    m_fAnimationKeyReductionTolerance =
        NiOptimize::ms_fEpsilon;
    m_bSortByTexturingProp = false;
    m_bWeldSkin = true;
    m_bExportSoftwareSkin = false;
    m_bExportHardwareSkin = true;
    m_uiBonesPerPartition = 30;
}
