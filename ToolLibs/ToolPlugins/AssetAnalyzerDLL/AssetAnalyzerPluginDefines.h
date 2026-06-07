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

#ifndef ASSETANALYZERPLUGINDEFINES_H
#define ASSETANALYZERPLUGINDEFINES_H

/// The following definitions are for constant character strings
/// used by AssetAnalyzerPlugin, its corresponding NiPluginInfo object,
/// and its management NiDialog. Using #defines is useful in avoiding
/// string typo mistakes that can be costly to track down when debugging.
#define ASSET_ANALYZER_USE_MAX_TRIANGLE_COUNT \
    "AssetAnalyzerUseMaxTriangleCount"
#define ASSET_ANALYZER_MAX_TRIANGLE_COUNT \
    "AssetAnalyzerMaxTriangleCount"

#define ASSET_ANALYZER_USE_MAX_TEXTURE_SIZE \
    "AssetAnalyzerUseMaxTextureSize"
#define ASSET_ANALYZER_MAX_TEXTURE_SIZE \
    "AssetAnalyzerMaxTextureSize"

#define ASSET_ANALYZER_USE_MAX_SCENE_MEMORY_SIZE \
    "AssetAnalyzerUseMaxSceneMemorySize"
#define ASSET_ANALYZER_MAX_SCENE_MEMORY_SIZE \
    "AssetAnalyzerMaxSceneMemorySize"

#define ASSET_ANALYZER_USE_MAX_TOTAL_TEXTURE_SIZE \
    "AssetAnalyzerUseMaxTotalTextureSize"
#define ASSET_ANALYZER_MAX_TOTAL_TEXTURE_SIZE \
    "AssetAnalyzerMaxTotalTextureSize"

#define ASSET_ANALYZER_USE_MAX_OBJECT_COUNT \
    "AssetAnalyzerUseMaxObjectCount"
#define ASSET_ANALYZER_MAX_OBJECT_COUNT \
    "AssetAnalyzerMaxObjectCount"

// "TrishapeRatio" is in this string (instead of "MeshRatio") to
// maintain backward compatibility for this plug-in's persisted config
// settings, when this feature was called "Minimum Triangle to Trishape
// Ratio".
#define ASSET_ANALYZER_USE_MINIMUM_TRIANGLE_TO_MESH_RATIO \
    "AssetAnalyzerUseMinimumTriangleToTrishapeRation"
#define ASSET_ANALYZER_MINIMUM_TRIANGLE_TO_MESH_RATIO \
    "AssetAnalyzerMinimumTriangleToTrishapeRation"

#define ASSET_ANALYZER_USE_MULTI_SUB_OBJECT_WARNINGS \
    "AssetAnalyzerUseMultiSubObjectWarnings"
#define ASSET_ANALYZER_MULTI_SUB_OBJECT_WARNINGS \
    "AssetAnalyzerMultiSubObjectWarnings"

#define ASSET_ANALYZER_USE_MAXIMUM_MORPHING_VERTEX_PER_OBJECT \
    "AssetAnalyzerUseMaximumMorphingVertexPerObject"
#define ASSET_ANALYZER_MAXIMUM_MORPHING_VERTEX_PER_OBJECT \
    "AssetAnalyzerMaximumMorphingVertexPerObject"

#define ASSET_ANALYZER_USE_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT \
    "AssetAnalyzerUseMaximumTriangleCountPerObject"
#define ASSET_ANALYZER_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT \
    "AssetAnalyzerMaximumTriangleCountPerObject"

#define ASSET_ANALYZER_USE_MAXIMUM_SCENE_GRAPH_DEPTH \
    "AssetAnalyzerUseMaximumSceneGraphDepth"
#define ASSET_ANALYZER_MAXIMUM_SCENE_GRAPH_DEPTH \
    "AssetAnalyzerMaximumSceneGraphDepth"

#define ASSET_ANALYZER_USE_MAXIMUM_LIGHTS_PER_OBJECT \
    "AssetAnalyzerUseMaximumLightsPerObject"
#define ASSET_ANALYZER_MAXIMUM_LIGHTS_PER_OBJECT \
    "AssetAnalyzerMaximumLightsPerObject"

#define ASSET_ANALYZER_USE_REQUIRED_OBJECTS \
    "AssetAnalyzerUseRequiredObjects"
#define ASSET_ANALYZER_NUMBER_OF_REQUIRED_OBJECTS \
    "AssetAnalyzerNumberOfRequiredObjects"
#define ASSET_ANALYZER_REQUIRED_OBJECTS \
    "AssetAnalyzerRequiredObjects"

#define ASSET_ANALYZER_NO_SPOT_LIGHTS \
    "AssetAnalyzerNoSpotLights"
#define ASSET_ANALYZER_NO_POINT_LIGHTS \
    "AssetAnalyzerNoPointLights"
#define ASSET_ANALYZER_NO_DIRECTIONAL_LIGHTS \
    "AssetAnalyzerNoDirectionalLights"
#define ASSET_ANALYZER_NO_AMBIENT_LIGHTS \
    "AssetAnalyzerNoAmbientLights"

#endif  // #ifndef ASSETANALYZERPLUGINDEFINES_H
