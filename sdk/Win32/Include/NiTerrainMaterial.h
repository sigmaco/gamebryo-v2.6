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

#ifndef NITERRAINMATERIAL_H
#define NITERRAINMATERIAL_H

#include <NiFragmentMaterial.h>
#include <NiTexturingProperty.h>

#include "NiTerrainLibType.h"

class NiTerrainMaterialVertexDescriptor;
class NiTerrainMaterialPixelDescriptor;
class NiTerrainMaterialDescriptor;

/**
    This class implements the default terrain material. The terrain material 
    currently supports up to 4 layers, in which each layer can have a base 
    map, detail map, normal map, and parallax map. The material generates a 
    unique shader depending on how many layers are present on the terrain, 
    what lights are affecting the terrain, fogging, etc.
    
    Note that this material uses a parallax offset of 0.05. The parallax offset
    is used to determine precisely how high the parallax mapping effect makes
    the texture appear. In general, this value should be computed based off the
    physical parameters of the surface. Since the height map represents both 
    positive and negative displacements of the surface in tangent space, 
    we can determine the maximum degree of those displacements. For example, 
    say that the height field for our texture represents a displacement of
    1 inch either up or down in the sample file. If the span of the texture 
    without tiling represents 500 inches, then we should set an offset scale of
    1/500 or 0.05. This means the height of a parallax map for terrain will
    always allow for a maximum displacement of 1/500th of the texture tile 
    size. Ensure that the height values stored in the parallax channel are 
    scaled appropriately.         
*/
class NITERRAIN_ENTRY NiTerrainMaterial : public NiFragmentMaterial
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    /// @endcond

public:

    /** 
        @name Global Shader Constants
        
        These strings are declared for the shader constant parameters
        used by the material. 
     */
    //@{
    static const char* LODTHRESHOLD_SHADER_CONSTANT;
    static const char* LODMORPHDISTANCE_SHADER_CONSTANT;
    static const char* UVBLENDOFFSET_SHADER_CONSTANT;
    static const char* UVBLENDSCALE_SHADER_CONSTANT;
    static const char* UVSURFACEOFFSET_SHADER_CONSTANT;
    static const char* UVSURFACESCALE_SHADER_CONSTANT;
    static const char* UVSURFACEUVMODIFIERARRAY_SHADER_CONSTANT;
    static const char* MORPHMODE_SHADER_CONSTANT;
    static const char* STITCHINGINFO_SHADER_CONSTANT;
    static const char* DISTMASKSTR_SHADER_CONSTANT;
    //@}

    /// Used for tracking the version of the terrain material.
    enum
    {
        /// Specifies the vertex shader version.
        VERTEX_VERSION = 2,

        /// Specifies the pixel shader version.
        PIXEL_VERSION = 3,

        /// Specifies the geometry shader version.
        GEOMETRY_VERSION = 0,

        /// Specifies the maximum number of layers per block.
        MAX_LAYERS_PER_BLOCK = 8,
    };

    /// Allows the terrain to have distance fog.
    enum Fog
    {
        /// Specifies no fog.
        FOG_NONE = 0,

        /// Specifies to use linear fog.
        FOG_LINEAR,

        /// Specifies squared fog
        FOG_SQUARED,

        /// The maximum specifications for fog.
        FOG_MAX
    };

    /**
        Each surface layer on the terrain can have a combination of the 
        following types of texture maps. A blend map controls up to four 
        layers.
        @note If a detail map exists for a specific layer, it is packed in the 
        base map's alpha channel. If a specular map exists, it is packed into 
        the alpha channel of the normal map.
    */
    enum TextureMapType
    {
        /// Indicates the base map.
        BASE_MAP = 0,

        /// Indicates the normal map.
        NORMAL_MAP,

        /// Indicates the specular map.
        SPECULAR_MAP,

        /// Indicates the controlling blend map
        BLEND_MAP,

        /// Used to as a way of getting more blending detail. The blend map 
        /// controls the overall blend of a layer, the distribution mask is 
        /// repeated at the same frequency as the base and normal map providing 
        /// more detail to the blend.
        DISTRIBUTION_MASK,

        /// Combined distribution mask from all layers. This is what is used by 
        /// the shader.
        COMBINED_DISTRIBUTION_MASK,
    };

    /// Destructor
    virtual ~NiTerrainMaterial();

    /**
        Since normal and parallax maps are automatically disabled when running on
        SM2_x cards, this method exists to force the support of normal and 
        parallax maps even if the current shader model is 2_x.
        @param bEnable Enables or disables normal and parallax maps.
    */
    void SetForceEnableNormalParallaxMaps(bool bEnable);

    /**
        Helper function for creating an instance of a terrain material. The 
        function first looks up the material in Gamebryo's material database.
        If it is not found an instance of the NiTerrainMaterial is created.
       
        @note Since materials are basically singletons, the terrain material 
        object is only created once.
    */
    static NiTerrainMaterial* Create();

    /**
        Inherited from NiFragmentMaterial. Creates the default fallback 
        functions.
    */
    virtual void AddDefaultFallbacks();
    
    /**
        This method removes the parallax/normal maps from all the layers
        in the pixel shader. In addition, it disables passing tangents from
        the vertex shader to the pixel shader.
    */
    static bool DropNormalParallaxMap(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

protected:

    /// Constructor
    NiTerrainMaterial(NiMaterialNodeLibrary* pkMaterialNodeLib,
        bool bAutoCreateCaches = true,
        const NiFixedString &name="NiTerrainMaterial");
    
    /// Entry point for constructing a shader based on the specified material
    /// descriptor.
    virtual NiShader* CreateShader(NiMaterialDescriptor* pkDesc);

    /// Configures the streams of data to be sent to the shader based upon the
    /// specified material descriptor.
    virtual bool SetupPackingRequirements(NiShader* pkShader,
        NiMaterialDescriptor* pkMaterialDescriptor,
        RenderPassDescriptor* pkRenderPasses, NiUInt32 uiCount);

    /**
        Analyzes the specified render object (terrain blocks in this case), 
        the properties attached to the geometry, and the dynamic effects 
        attached to the geometry. This method then builds a descriptor object
        which is used to stitch together shader fragments to produce a shader.
    */
    virtual bool GenerateDescriptor(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkPropState, 
        const NiDynamicEffectState* pkEffects,
        NiMaterialDescriptor& kMaterialDesc);

    /**
        If no fall backs are required, a material descriptor encoding the 
        information for a single pass are is generated.
    */
    virtual ReturnCode GenerateShaderDescArray(
        NiMaterialDescriptor* pkMaterialDescriptor,
        RenderPassDescriptor* pkRenderPasses, NiUInt32 uiMaxCount,
        NiUInt32& uiCountAdded);

    /**
        Constructs a shade tree for the vertex shader based on the 
        specified descriptor object. The shade tree is then parsed and used
        to construct the final vertex shader.
    */
    virtual bool GenerateVertexShadeTree(Context& kContext, 
        NiGPUProgramDescriptor* pkDesc);

    /**
        Constructs a shade tree for the pixel shader based on the 
        specified descriptor object. The shade tree is then parsed and used
        to construct the final pixel shader.
    */
    virtual bool GeneratePixelShadeTree(Context& kContext,
        NiGPUProgramDescriptor* pkDesc);

    /**
        This method takes the shader programs generated by the shader 
        generating functions and returns RC_SUCCESS if they were created 
        successfully.
    */
    virtual NiUInt32 VerifyShaderPrograms(NiGPUProgram* pkVertexShader,
        NiGPUProgram* pkGeometryShader, NiGPUProgram* pkPixelShader);

    /**
        Handles the shader fragments that split the incoming packed positions, 
        normals, and possibly tangents into source and target morph targets 
        based on the current level of detail of a terrain block.
    */
    virtual void HandleInputDataSplits(Context& kContext, 
        NiTerrainMaterialVertexDescriptor* pkDesc, 
        NiMaterialResource*& pkPosHigh, NiMaterialResource*& pkPosLow,
        NiMaterialResource*& pkNormalHigh, NiMaterialResource*& pkNormalLow,
        NiMaterialResource*& pkTangentHigh, NiMaterialResource*& pkTangentLow);

    /// Handles the shader fragments for putting a local vertex position into 
    /// world space.
    virtual void HandleWorldSpaceTransformation(Context& kContext,         
        NiMaterialResource*& pkWorldPos, NiMaterialResource* pkLocalPos,
        NiMaterialResource* pkWorldMatrix);

    /// Handles the shader fragments for rotating a vector from local space 
    /// to world space.
    virtual void HandleWorldSpaceRotation(Context& kContext,         
        NiMaterialResource*& pkWorldRot, NiMaterialResource* pkLocalRot,
        NiMaterialResource* pkWorldMatrix);

    /// Handles the shader fragments for computing a view vector for the 
    /// specified world position.
    /// @note The camera position is a shader constant automatically 
    /// registered by this method.
    virtual void HandleViewVector(Context& kContext,
        NiTerrainMaterialVertexDescriptor* pkDesc,
        NiMaterialResource*& pkViewVector, NiMaterialResource* pkWorldPos);

    /// Handles the shader fragments for doing per pixel fog.
    virtual void HandleCalculateFog(Context& kContext, 
        NiMaterialResource* pkViewPos, Fog eFogType);

    /// Handles the shader fragments for computing a morph value based on the 
    /// distance from the camera. This is controlled by several additional 
    /// shader constants that are registered by this method.
    virtual void HandleCalculateMorphValue(Context& kContext,
        NiTerrainMaterialVertexDescriptor* pkDesc, 
        NiMaterialResource*& pkMorphValue);

    /// Handles the shader fragments for morphing the vertex positions and 
    /// applying the view projection transformation.
    virtual void LerpPositionsAndApplyViewProjection(
        Context& kContext,        
        NiMaterialResource* pkWorldPositionHigh, 
        NiMaterialResource* pkWorldPositionLow,
        NiMaterialResource* pkMorphValue, 
        NiMaterialResource*& pkViewPos,
        bool bIncludeViewPos);        

    /// Handles the shader fragments for morphing the normals and possibly 
    /// tangents of the geometry and sending them on to the pixel shader.
    virtual void LerpAndOutputNormalsAndTangents(
        Context& kContext,
        NiTerrainMaterialVertexDescriptor* pkDesc,        
        NiMaterialResource* pkNormalHigh, NiMaterialResource* pkNormalLow,
        NiMaterialResource* pkTangentHigh, NiMaterialResource* pkTangentLow,
        NiMaterialResource* pkMorphValue, NiMaterialResource* pkWorldMatrix);

    /// Adds the appropriate interpolators for texture coordinates based on 
    /// how many texture coordinates sets are used in the descriptor.
    virtual void HandleVertexTexCoordSets(Context& kContext,
        NiTerrainMaterialVertexDescriptor* pkDesc);

    /// Analyzes the specified texture property and extracts any base maps, 
    /// normal maps, distribution maps, and blend maps that may be present. These
    /// maps are identified by the enumeration defined in this class.
    virtual void BuildTextureArrays(NiTexturingProperty* pkTexProp, 
        NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kBaseMaps,
        NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kNormalMaps,
        NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kDistributionMaps,
        NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kBlendMaps);

    /// Analyzes the specified arrays of maps and sets the appropriate bits in 
    /// the terrain material descriptor based on what maps are present.
    virtual void BuildLayerDescription(NiTerrainMaterialDescriptor* pkDesc, 
        const NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kBaseMaps,
        const NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kNormalMaps,
        const NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& 
            kDistributionMaps,
        const NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kBlendMaps);

    /// Adds the necessary texture coordinate interpolators based on how many
    /// texture coordinates sets are specified in the material descriptor.
    virtual void HandlePixelTexCoordSets(Context& kContext,
        NiTerrainMaterialPixelDescriptor* pkDesc);

    /// Retrieves the appropriate scale and offset from the surface
    /// UV Modifier array constant
    virtual void HandlePixelAccessSurfaceUVModifiers(Context& kContext, 
        NiTerrainMaterialPixelDescriptor* pkDesc, NiUInt32 uiCurrLayer, 
        NiMaterialResource*& pkSurfaceScale, 
        NiMaterialResource*& pkSurfaceOffset);

    /// Adjusts the texture coordinates for each surface independent of
    /// the other surfaces. 
    virtual void HandlePixelSurfaceTexCoordSets(Context& kContext,
        NiMaterialResource* pkOriginalUV, NiMaterialResource* pkScale,
        NiMaterialResource* pkOffset, NiMaterialResource*& pkUVSet);

    /// Handles the shader fragments for sampling blend maps for each of the 
    /// layers on the terrain block which is indicated in the material 
    /// descriptor.
    virtual void HandleSampleBlendMaps(Context& kContext,
        NiTerrainMaterialPixelDescriptor* pkDesc,        
        NiMaterialResource* pkBlendMapOutputs[MAX_LAYERS_PER_BLOCK / 4][4],
        NiMaterialResource*& pkMaskAccum);

    /// Processes the layers on a given terrain block by handling all the 
    /// shader fragments necessary to produce an accumulated value based on all
    /// of the layers and maps affecting a given pixel.
    virtual void ProcessLayers(Context& kContext,
        NiTerrainMaterialPixelDescriptor* pkDesc,
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource*& pkNormalAccum,
        NiMaterialResource* pkBlendMap[MAX_LAYERS_PER_BLOCK / 4][4],
        NiMaterialResource* pkMaskAccum,
        NiMaterialResource* pkInterpolatedWorldNormal,
        NiMaterialResource* pkWorldView,
        NiUInt32 uiSamplerStartIndex);

    /// Handles the shader fragments necessary to sample a base map that may 
    /// contain an optional detail map in the alpha channel.
    virtual void HandleBaseMap(Context& kContext, 
        NiTerrainMaterialPixelDescriptor* pkDesc,
        NiMaterialResource*& pkDiffuseAccum, 
        NiMaterialResource* pkBlendMask,
        NiMaterialResource* pkUVSet,
        NiUInt32 uiCurrLayer, 
        NiUInt32 uiSamplerIndex);

    /// Handles the shader fragments necessary to sample a normal map.
    virtual void HandleNormalMap(Context& kContext,
        NiTerrainMaterialPixelDescriptor* pkDesc,
        NiMaterialResource*& pkNormalAccum,
        NiMaterialResource* pkBlendMask,
        NiMaterialResource* pkWorldNormal,
        NiMaterialResource* pkWorldBinormal,
        NiMaterialResource* pkWorldTangent,
        NiMaterialResource* pkUVSet,
        NiUInt32 uiCurrLayer,
        NiUInt32 uiSamplerIndex);

    // Handles the shader fragments necessary to sample a parallax map.
    virtual void HandleParallaxMap(Context& kContext,
        NiTerrainMaterialPixelDescriptor* pkDesc,
        NiMaterialResource*& pkParallaxAccum, 
        NiMaterialResource* pkBlendMask,
        NiMaterialResource* pkUVSet, NiUInt32 uiCurrLayer, 
        NiUInt32 uiSamplerIndex);

    /**
        Using the specified height or parallax value, this method will 
        generate the appropriate shader fragments for transforming the view 
        vector into tangent space and apply an offset to the specified
        uv coordinate.
    */
    virtual void CalculateParallaxOffset(Context& kContext,
        NiMaterialResource* pkParallaxValue,
        NiMaterialResource* pkWorldView,
        NiMaterialResource* pkWorldTangent,
        NiMaterialResource* pkWorldBinormal,
        NiMaterialResource* pkWorldNormal,
        NiMaterialResource*& pkUVSet);

    /**
        Handles the shader fragments for applying sunlight to the terrain. 
        Currently, this method treats the sunlight as a simple directional 
        light. Future versions may include additional calculations for other 
        atmospheric influences due to the interaction of sunlight and the 
        terrain.
    */
    virtual void HandleSunlight(Context& kContext, 
        NiTerrainMaterialPixelDescriptor* pkDesc, 
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource* pkPerPixelNormal,
        NiMaterialResource* pkWorldView);

    /**
        Handles the shader fragments for automatically computing a per-pixel 
        tangent frame if a tangent frame was not specified per vertex and sent
        through to the pixel shader. This method uses the technique found in 
        "ShaderX5 - Normal Mapping without Pre-Computed Tangents by Christian 
        Schueler".
    */
    virtual void ComputeTangentFrame(
        Context& kContext, 
        NiTerrainMaterialPixelDescriptor* pkDesc, 
        NiMaterialResource*& pkWorldTangent,
        NiMaterialResource*& pkWorldBinormal,    
        NiMaterialResource* pkUVSet,
        NiMaterialResource* pkInterpolatedWorldSpaceNormal);

    /// Handles the conversion of a vector in tangent space to world space.
    virtual void ConvertTangentSpaceNormalToWorldSpace(
        Context& kContext, 
        NiTerrainMaterialPixelDescriptor* pkDesc, 
        NiMaterialResource*& pkWorldSpaceNormalFromTangentSpace,
        NiMaterialResource* pkWorldTangent,
        NiMaterialResource* pkWorldBinormal,
        NiMaterialResource* pkWorldNormal,
        NiMaterialResource* pkTangentSpaceNormal);

    /// Inspects the specified shader context for an input resource with the 
    /// specified UV set index value.
    virtual NiMaterialResource* FindTextureUVSet(
        const Context& kContext, NiUInt32 uiUVIndex);

    /// Generates a texture sampler name based on the specified prefix and 
    /// count value.
    inline virtual NiFixedString GenerateSamplerName(
        const NiFixedString& baseName, NiUInt32 uiIndex);

    /// Adds a new texture sampler to the shader context.
    inline virtual NiMaterialResource* AddTextureSampler(Context& kContext,
        const NiFixedString& kSamplerName, NiUInt32 uiOccurance);

    bool HandleApplyFog(Context& kContext, 
        NiTerrainMaterialPixelDescriptor* pkPixDesc,
        NiMaterialResource* pkUnfoggedColor, 
        NiMaterialResource*& pkFogOutput);

    /// Allows handling for final vertex outputs. Default handler simply
    /// returns true.
    virtual bool HandleFinalVertexOutputs(Context& kContext,
        NiMaterialResource* pkWorldPositionLow,
        NiMaterialResource* pkWorldPositionHigh);
private:
    
    NiFixedString m_kMaterialDescriptorName;
    NiFixedString m_kVertexShaderDescriptorName;
    NiFixedString m_kPixelShaderDescriptorName;
    bool m_bEnableNPOnSM2;
};

NiSmartPointer(NiTerrainMaterial);
#include "NiTerrainMaterial.inl"

#endif
