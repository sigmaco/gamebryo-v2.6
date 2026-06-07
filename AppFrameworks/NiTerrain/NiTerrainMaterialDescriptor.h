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

#ifndef NITERRAINMATERIALDESCRIPTOR_H
#define NITERRAINMATERIALDESCRIPTOR_H

#include <NiMaterialDescriptor.h>
#include <NiBitfield.h>

#include "NiTerrainLibType.h"

/**
    This class manages the bit field for the terrain material.
    
    This bit field is then used to construct the individual descriptors for
    both the vertex and pixel shader. The bit field encodes the layers
    affecting the terrain, lights affecting the terrain, and various other
    attributes.
*/
class NITERRAIN_ENTRY NiTerrainMaterialDescriptor :
    public NiMaterialDescriptor
{
public:

    /// Constructor
    NiTerrainMaterialDescriptor();

    /**
        Helper method that sets the appropriate bits in the bit field based on
        the specified layer parameters.

        @param uiLayerIndex Layer to operate on.
        @param uiUVIndex UV set to use when addressing the textures for the 
            given layer.
        @param bEnableDetailMap True if a detail map for the specified layer 
            exists.
        @param bEnableNormalMap True if a normal map for the specified layer
            exists.
        @param bEnableParallaxMap True if a parallax map for the specified 
            layer exists.
        @param bEnableDistributeMask True if a distribution mask for the specified 
            layer exists.
    */        
    void SetLayerCaps(NiUInt32 uiLayerIndex, NiUInt32 uiUVIndex, 
        bool bEnableDetailMap, bool bEnableNormalMap, bool bEnableParallaxMap, 
        bool bEnableDistributeMask);

    /**
        Determines if a view vector should be calculated by analyzing the bits
        set on the material descriptor.

        @note A view vector is required if any layer has a parallax map or a 
            specular map.
    */
    bool RequiresViewVector();

    /// @cond EMERGENT_INTERNAL
    NiBeginDefaultBitfieldEnum()

    // First byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(TANGENTS, 1, BITFIELDSTART, 0)
    NiDeclareDefaultIndexedBitfieldEntry(FOGTYPE, 2, TANGENTS, 0)
    NiDeclareDefaultIndexedBitfieldEntry(BLENDMAP_COUNT, 2, FOGTYPE, 0);
    NiDeclareDefaultIndexedBitfieldEntry(ENABLE_SURFACEUVMODIFIERS, 1, 
        BLENDMAP_COUNT, 0);

    // One directional light is supported on the terrain which represents the 
    // sun.
    NiDeclareDefaultIndexedBitfieldEntry(ENABLE_SUNLIGHT, 1,
        ENABLE_SURFACEUVMODIFIERS, 0);

    // 3 bytes left.
        
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

    // Third byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(MORPH_MODE, 2, 
        BLENDMAP1_UVINDEX, 3)

    NiDeclareDefaultIndexedBitfieldEntry(NUM_DIST_MAPS, 2, 
        MORPH_MODE, 3);

    NiEndDefaultBitfieldEnum();
    /// @endcond

};

#endif
