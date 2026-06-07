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

#ifndef NISCENEGRAPHOPTIMIZATIONPLUGIN_H
#define NISCENEGRAPHOPTIMIZATIONPLUGIN_H

/// This class implements the scene graph optimization plugin. It will allow 
/// a make a number of optimizations on the scene graph including; Remove 
/// duplicate properties, Remove bad NiTexturing properties, remove 
/// multi-material nodes, remove childless nodes, remove single-child nodes,
/// Merge sibling nodes, merge sibling NiMeshes, Optimize NiMesh
/// objects, Remove extra texture coordinate sets, Remove unnecessary
/// normals, Remvoe Unnecessary NiVisControllers, Remove Unnecessary
/// LookAtControllers, Remove hidden bone mesh, Reduce Animation keys,
/// Remove unnecessary extra data, sort children by NiTexturing property,
/// Removing object names, Skin and Bone Support.
class NiSceneGraphOptimizationPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiSceneGraphOptimizationPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    // Optimizes the scene graph rooted at pkScene using the current set of
    // options selected.
    void Optimize(NiNode* pkScene);

    // Resets all options to their default values.
    void ResetDefaultOptions();

    //***********************************************************************
    // Optimization options.
    //***********************************************************************
    bool m_bRemoveDupProperties;
    bool m_bRemoveChildlessNodes;
    bool m_bRemoveMultiMtlNodes;
    bool m_bRemoveSingleChildNodes;
    bool m_bMergeSiblingNodes;
    bool m_bOptimizeTriShapes;
    float m_fOptimizeMeshSizeModifier;
    float m_fOptimizeMeshNormalModifier;
    float m_fOptimizeMeshPositionModifier;
    NiUInt32 m_uiOptimizeMeshVertexCacheSize;
    bool m_bMergeSiblingMeshes;
    bool m_bNoMergeOnAlpha;
    bool m_bRemoveExtraUVSets;
    bool m_bRemoveUnnecessaryNormals;
    bool m_bRemoveUnnecessaryVisControllers;
    bool m_bRemoveNames;
    bool m_bNameUnnamedObjects;
    bool m_bRemoveBadTexturingProps;
    bool m_bRemoveUnnecessaryExtraData;
    bool m_bRemoveSgoKeepTags;
    bool m_bRemoveUnnecessaryLookAtControllers;
    bool m_bRemoveHiddenBoneMesh;
    bool m_bReduceAnimationKeys;
    float m_fAnimationKeyReductionTolerance;
    bool m_bSortByTexturingProp;
    bool m_bWeldSkin;
    bool m_bExportSoftwareSkin;
    bool m_bExportHardwareSkin;
    unsigned int m_uiBonesPerPartition;
    
    void InitProgressBar();
    void UpdateProgressBar(const char* pcLine, bool bDoOption);
    void CloseProgressBar();

    bool m_bShowProgress;
    NiProgressDialog* m_pkProgressBar;
};

#endif  // #ifndef NISCENEGRAPHOPTIMIZATIONPLUGIN_H
