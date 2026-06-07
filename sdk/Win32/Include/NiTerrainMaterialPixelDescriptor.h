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

#ifndef NITERRAINPIXELDESCRIPTOR_H
#define NITERRAINPIXELDESCRIPTOR_H 

#include <NiGPUProgramDescriptor.h>
#include <NiBitfield.h>

#include "NiTerrainLibType.h"

class NiTerrainMaterialDescriptor;

/**
    This class manages the bit fields that are constructed from the terrain 
    material descriptor.
    
    It is used when determining how to construct a shade tree in order to
    generate a pixel shader.
*/
class NITERRAIN_ENTRY NiTerrainMaterialPixelDescriptor : 
    public NiGPUProgramDescriptor
{
public:

    /// Convert the bit field to a human readable string.
    ///
    /// This is useful when debugging the values set in the bit field.
    NiString ToString();

    /// Helper function to retrieve the UV set index used by the given layer.
    NiUInt32 GetLayerUVIndex(NiUInt32 uiLayer);

    /// Helper function to retrieve the UV set index used to access the 
    /// specified blend map.
    NiUInt32 GetBlendMapUVIndex(NiUInt32 uiBlendMap);

    /// Return true if the specified layer supports a detail map.
    bool SupportsDetailMap(NiUInt32 uiLayerIndex);

    /// Return true if the specified layer supports a normal map.
    bool SupportsNormalMap(NiUInt32 uiLayerIndex);

    /// Enable or disable normal map for a given layer.
    void SetNormalEnabled(NiUInt32 uiLayerIndex, bool bEnabled);

    /// Return true if the specified layer supports a parallax map.
    bool SupportsParallaxMap(NiUInt32 uiLayerIndex);

    /// Enable or disable parallax map for a given layer.
    void SetParallaxEnabled(NiUInt32 uiLayerIndex, bool bEnabled);

    /// Return true if the specified layer supports a distribution mask.
    bool SupportsDistributionMask(NiUInt32 uiLayerIndex);

    /// Returns true if a distribution mask is found on any layer.
    bool SupportsDistributionMask();

    /// Enable or disable a distribution mask for a given layer.
    void SetDistributionMaskEnabled(NiUInt32 uiLayerIndex, bool bEnabled);

    /// Analyzes the material descriptor and sets the appropriate bits in the
    /// pixel descriptor's bit field to reflect the necessary state.
    void SetLayerCapabilitiesFromMaterialDescriptor(
        NiTerrainMaterialDescriptor* pkMaterialDesc, NiUInt32 uiLayer);

    /// @cond EMERGENT_INTERNAL
    NiBeginDefaultBitfieldEnum()

    // First byte, index 0
    NiDeclareDefaultIndexedBitfieldEntry(WORLDNBT, 1, BITFIELDSTART, 0)    
    NiDeclareDefaultIndexedBitfieldEntry(FOGTYPE, 2, WORLDNBT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(BLENDMAP_COUNT, 2, FOGTYPE, 0);
    NiDeclareDefaultIndexedBitfieldEntry(ENABLE_SURFACEUVMODIFIERS, 1, 
        BLENDMAP_COUNT, 0);

    NiDeclareDefaultIndexedBitfieldEntry(ENABLE_SUNLIGHT, 1, 
        ENABLE_SURFACEUVMODIFIERS, 0);
    NiDeclareDefaultIndexedBitfieldEntry(WORLD_VIEW, 1, ENABLE_SUNLIGHT, 0);

    // First byte, Index 1    

    /// Update to 8 layers on a single terrain block. Note, this does not limit
    /// the number of surface layers for an entire terrain, just an individual 
    /// block. Each layer may have it's own UV set, but this is not required.
    NiDeclareDefaultIndexedBitfieldEntry(NUM_TEXCOORDS, 4, BITFIELDSTART, 1)
    NiDeclareDefaultIndexedBitfieldEntry(NUM_LAYERS, 4, NUM_TEXCOORDS, 1)

    // Second byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(LAYER0_NORMAL_ENABLED, 1, 
        NUM_LAYERS, 1) 
    NiDeclareDefaultIndexedBitfieldEntry(LAYER0_PARALLAX_ENABLED, 1, 
        LAYER0_NORMAL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER0_DISTMASK_ENABLED, 1, 
        LAYER0_PARALLAX_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER0_DETAIL_ENABLED, 1, 
        LAYER0_DISTMASK_ENABLED, 1)

    NiDeclareDefaultIndexedBitfieldEntry(LAYER1_NORMAL_ENABLED, 1, 
        LAYER0_DETAIL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER1_PARALLAX_ENABLED, 1, 
        LAYER1_NORMAL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER1_DISTMASK_ENABLED, 1, 
        LAYER1_PARALLAX_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER1_DETAIL_ENABLED, 1, 
        LAYER1_DISTMASK_ENABLED, 1)

    // Third byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(LAYER2_NORMAL_ENABLED, 1, 
        LAYER1_DETAIL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER2_PARALLAX_ENABLED, 1, 
        LAYER2_NORMAL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER2_DISTMASK_ENABLED, 1, 
        LAYER2_PARALLAX_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER2_DETAIL_ENABLED, 1, 
        LAYER2_DISTMASK_ENABLED, 1)

    NiDeclareDefaultIndexedBitfieldEntry(LAYER3_NORMAL_ENABLED, 1, 
        LAYER2_DETAIL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER3_PARALLAX_ENABLED, 1, 
        LAYER3_NORMAL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER3_DISTMASK_ENABLED, 1, 
        LAYER3_PARALLAX_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER3_DETAIL_ENABLED, 1, 
        LAYER3_DISTMASK_ENABLED, 1)

    // Fourth byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(LAYER4_NORMAL_ENABLED, 1, 
        LAYER3_DETAIL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER4_PARALLAX_ENABLED, 1, 
        LAYER4_NORMAL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER4_DISTMASK_ENABLED, 1, 
        LAYER4_PARALLAX_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER4_DETAIL_ENABLED, 1, 
        LAYER4_DISTMASK_ENABLED, 1)

    NiDeclareDefaultIndexedBitfieldEntry(LAYER5_NORMAL_ENABLED, 1, 
        LAYER4_DETAIL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER5_PARALLAX_ENABLED, 1, 
        LAYER5_NORMAL_ENABLED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER5_DISTMASK_ENABLED, 1, 
        LAYER5_PARALLAX_ENABLED, 1)
    NiDeclareDefaultLastIndexedBitfieldEntry(LAYER5_DETAIL_ENABLED, 1, 
        LAYER5_DISTMASK_ENABLED, 1)

    // First byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(LAYER6_NORMAL_ENABLED, 1, 
        BITFIELDSTART, 2)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER6_PARALLAX_ENABLED, 1, 
        LAYER6_NORMAL_ENABLED, 2);
    NiDeclareDefaultIndexedBitfieldEntry(LAYER6_DISTMASK_ENABLED, 1, 
        LAYER6_PARALLAX_ENABLED, 2)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER6_DETAIL_ENABLED, 1, 
        LAYER6_DISTMASK_ENABLED, 2)

    NiDeclareDefaultIndexedBitfieldEntry(LAYER7_NORMAL_ENABLED, 1, 
        LAYER6_DETAIL_ENABLED, 2)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER7_PARALLAX_ENABLED, 1, 
        LAYER7_NORMAL_ENABLED, 2)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER7_DISTMASK_ENABLED, 1, 
        LAYER7_PARALLAX_ENABLED, 2)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER7_DETAIL_ENABLED, 1, 
        LAYER7_DISTMASK_ENABLED, 2)

    // Each layer may have a unique UV set so we have a total of 8 possible UV 
    // sets.

    // Second byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(LAYER0_UVINDEX, 4, 
        LAYER7_DETAIL_ENABLED, 2) 
    NiDeclareDefaultIndexedBitfieldEntry(LAYER1_UVINDEX, 4, LAYER0_UVINDEX, 2)

    // Third byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(LAYER2_UVINDEX, 4, LAYER1_UVINDEX, 2)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER3_UVINDEX, 4, LAYER2_UVINDEX, 2)

    // Fourth byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(LAYER4_UVINDEX, 4, LAYER3_UVINDEX, 2)
    NiDeclareDefaultLastIndexedBitfieldEntry(LAYER5_UVINDEX, 4, LAYER4_UVINDEX,
        2)

    // First byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(LAYER6_UVINDEX, 4, BITFIELDSTART, 3)
    NiDeclareDefaultIndexedBitfieldEntry(LAYER7_UVINDEX, 4, LAYER6_UVINDEX, 3)

    // Second byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(BLENDMAP0_UVINDEX, 4, LAYER7_UVINDEX, 
        3)
    NiDeclareDefaultIndexedBitfieldEntry(BLENDMAP1_UVINDEX, 4, 
        BLENDMAP0_UVINDEX, 3)

    NiDeclareDefaultIndexedBitfieldEntry(NUM_DIST_MAPS, 2, 
        BLENDMAP1_UVINDEX, 3);

    NiEndDefaultBitfieldEnum()
    /// @endcond
};

#endif
