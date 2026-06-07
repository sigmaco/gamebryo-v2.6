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

#ifndef NISCENEGRAPHOPTIMIZATIONDEFINES_H
#define NISCENEGRAPHOPTIMIZATIONDEFINES_H

#define OPT_REMOVE_DUP_PROPERTIES               "RemoveDupProperties"
#define OPT_REMOVE_CHILDLESS_NODES              "RemoveChildlessNodes"
#define OPT_REMOVE_MULTIMTL_NODES               "RemoveMultiMtlNodes"
#define OPT_REMOVE_SINGLE_CHILD_NODES           "RemoveSingleChildNodes"
#define OPT_MERGE_SIBLING_NODES                 "MergeSiblingNodes"
#define OPT_OPTIMIZE_TRISHAPES                  "OptimizeTriShapes"
#define OPT_OPTIMIZE_MESH_SIZE_MODIFIER         "OptimizeMeshSizeModifier"
#define OPT_OPTIMIZE_MESH_NORMAL_MODIFIER       "OptimizeMeshNormalModifier"
#define OPT_OPTIMIZE_MESH_POSITION_MODIFIER     "OptimizeMeshPositionModifier"
#define OPT_OPTIMIZE_DX9_MESH_VERTEX_CACHE_SIZE \
    "OptimizeDX9MeshVertexCacheSize"
#define OPT_OPTIMIZE_D3D10_MESH_VERTEX_CACHE_SIZE \
    "OptimizeD3D10MeshVertexCacheSize"
#define OPT_OPTIMIZE_XENON_MESH_VERTEX_CACHE_SIZE \
    "OptimizeXenonMeshVertexCacheSize"
#define OPT_OPTIMIZE_PS3_MESH_VERTEX_CACHE_SIZE \
    "OptimizePS3MeshVertexCacheSize"
#define OPT_OPTIMIZE_WII_MESH_VERTEX_CACHE_SIZE \
    "OptimizeWiiMeshVertexCacheSize"
#define OPT_MERGE_SIBLING_TRISHAPES             "MergeSiblingTriShapes"
#define OPT_NO_MERGE_ON_ALPHA                   "NoMergeOnAlpha"
#define OPT_REMOVE_EXTRA_UVSETS                 "RemoveExtraUVSets"
#define OPT_REMOVE_UNNECESSARY_NORMALS          "RemoveUnnecessaryNormals"
#define OPT_REMOVE_UNNECESSARY_VISCONTROLLERS \
    "RemoveUnnecessaryVisControllers"
#define OPT_REMOVE_NAMES                        "RemoveNames"
#define OPT_NAME_UNNAMED_OBJECTS                "NameUnnamedObjects"
#define OPT_REMOVE_BAD_TEXTURING_PROPS          "RemoveBadTexturingProps"
#define OPT_REMOVE_UNNECESSARY_EXTRA_DATA       "RemoveUnnecessaryExtraData"
#define OPT_REMOVE_SGOKEEP_TAGS                 "RemoveSgoKeepTags"
#define OPT_REMOVE_UNNECESSARY_LOOKATCONTROLLERS \
    "RemoveUnnecessaryLookAtControllers"
#define OPT_REMOVE_HIDDEN_BONE_MESH             "RemoveHiddenBoneGeometry"
#define OPT_REDUCE_ANIMATION_KEYS               "ReduceAnimationKeys"
#define OPT_ANIMATION_KEY_REDUCTION_TOLERANCE \
    "AnimationKeyReductionTolerance"
#define OPT_SET_SELECTIVE_UPDATE                "SetSelectiveUpdate"
#define OPT_SORT_BY_TEXTURING_PROP              "SortByTexturingProp"
#define OPT_WELD_SKIN                           "WeldSkin"
#define OPT_EXPORT_SOFTWARE_SKIN                "ExportSoftwareSkin"
#define OPT_EXPORT_HARDWARE_SKIN                "ExportHardwareSkin"
#define OPT_BONES_PER_PARTITION                 "BonesPerPartition"

#endif
