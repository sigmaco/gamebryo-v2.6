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

#ifndef NISTANDARDMATERIAL_H
#define NISTANDARDMATERIAL_H

#include "NiRenderObject.h"
#include "NiShaderConstantMap.h"
#include "NiTSet.h"
#include "NiMaterialDescriptor.h"
#include "NiGPUProgramDescriptor.h"
#include "NiMaterialConfigurator.h"
#include "NiMaterialResourceProducerNode.h"
#include "NiMaterialResourceConsumerNode.h"
#include "NiMaterialFragmentNode.h"
#include "NiMaterialNodeLibrary.h"
#include "NiFragmentMaterial.h"

class NiStandardVertexProgramDescriptor;
class NiStandardPixelProgramDescriptor;
class NiStandardMaterialShader;

class NIMAIN_ENTRY NiStandardMaterial : public NiFragmentMaterial
{
    NiDeclareRTTI;
public:
    // Note: This structure should be kept in sync with the one in
    // NiVSMBlurMaterial

    // This enumeration determines how this material deals with 
    // transformations.
    enum TransformType
    {
        // Traditional vertex transformations.
        TRANSFORM_DEFAULT = 0,
        // Skinned vertex transformations wherein bones are chosen from a 
        // palette of transforms using per-vertex indices.
        TRANSFORM_SKINNED, 
        // This value is reserved and should not be used.
        TRANSFORM_RESERVED,
        // Instanced vertex transformations.
        TRANSFORM_INSTANCED,
        // Maximum transform enum
        TRANSFORM_MAX
    };

    // This enumeration determines how this material deals with normals.
    enum NormalType
    {
        // No per-vertex normals.
        NORMAL_NONE = 0,
        // Per-vertex normals.
        NORMAL_ONLY,
        // Per-vertex normal, binormal, and tangent vectors.
        NORMAL_NBT,
        // Maximum normal enum
        NORMAL_MAX
    };

    // This enumeration determines how this material deals with fog.
    enum Fog
    {
        // No per-vertex fog.
        FOG_NONE = 0,
        // Fog linearly increases based off of camera depth
        FOG_LINEAR,
        // Fog increases exponentially based off of camera depth.
        FOG_SQUARED,
        // Maximum fog enum
        FOG_MAX
    };

    // This enumeration determines how this material deals with texture 
    // coordinate generation.
    enum TexGenInput
    {
        // Sourced from an input vertex texture coordinate
        TEX_IN_TEXCOORD = 0,
        // Sourced from the world position.
        TEX_IN_WORLD_POS,
        // Sourced from the world normal.
        TEX_IN_WORLD_NORM,
        // Sourced from the camera reflection vector in world space.
        TEX_IN_WORLD_REFLECT,
        // Maximum in enum
        TEX_IN_MAX
    };

    // This enumeration determines how this material deals with texture 
    // coordinate transformation..
    enum TexGenOutput
    {
        // Coordinate used as-is.
        TEX_OUT_PASSTHRU = 0,
        // Coordinate has texture transform applied.
        TEX_OUT_TRANSFORM,
        // Coordinate has parallel projection applied.
        TEX_OUT_PARALLEL_PROJECT,
        // Coordinate has perspective projection applied.
        TEX_OUT_PERSPECTIVE_PROJECT,
        // Maximum out enum
        TEX_OUT_MAX
    };

    // This enumeration determines how this material deals with vertex colors.
    enum VertexColorType
    {
        // Material does not have vertex colors.
        VC_NONE = 0, 
        // Material does have vertex colors.
        VC_EXISTS,
        // Maximum VC enum
        VC_MAX
    };

    // This enumeration determines how this material deals with source of 
    // ambient/emissive/diffuse color.
    enum AmbDiffEmissiveEnum
    {
        // Material diffuse, ambient, and emissive come from 
        // NiMaterialProperty.
        ADE_IGNORE = 0,
        // Material emissive comes from vertex colors. Diffuse and ambient
        // come from NiMaterialProperty.
        ADE_EMISSIVE,
        // Material emissive comes from NiMaterialProperty. Diffuse and
        // ambient come from vertex colors.
        ADE_AMB_DIFF,
        // Maximum enum
        ADE_MAX
    };

    // This enumeration determines what lighting model the material uses.
    enum LightingModeEnum
    {
        // Only the emissive portion of lighting is used.
        LIGHTING_E = 0,
        // Lighting uses emissive, ambient, and diffuse portions.
        LIGHTING_E_A_D,
        // Maximum enum
        LIGHTING_MAX
    };

    // This enumeration determines how this material deals with how the 
    // material responds to lighting.
    enum ApplyMode
    {
        // Lighting is ignored.
        APPLY_REPLACE = 0,
        // Lighting is multiplied with texture.
        APPLY_MODULATE,
        // Maximum enum
        APPLY_MAX
    };

    // This enumeration determines how this material deals with normal maps.
    enum NormalMapType
    {
        // Standard RGB normal maps.
        NORMAL_MAP_STANDARD = 0,
        // ATI's compressed normal map format. (Not implemented)
        NORMAL_MAP_DXN,
        // Compressed DXT5 format. R and A are the input vectors. 
        // (Not implemented)
        NORMAL_MAP_DXT5,
        // Maximum enumm
        NORMAL_MAP_MAX
    };

    // This enumeration lists the types of lights.
    enum LightType
    {
        // Directional light.
        LIGHT_DIR = 0,
        // Point light
        LIGHT_POINT,
        // Spot light
        LIGHT_SPOT,
        LIGHT_MAX
    };

    // This enumeration determines how this material deals with texture 
    // effects.
    enum TexEffectType
    {
        // No texture effect applied.
        TEXEFFECT_NONE = 0,
        // World position multiplied by parallel projection matrix.
        TEXEFFECT_WORLD_PARALLEL,
        // World position multiplied by perspective projection matrix.
        TEXEFFECT_WORLD_PERSPECTIVE,
        // Spherical environment map using world reflection as lookup vector.
        TEXEFFECT_SPHERICAL,
        // Diffuse cubic environment map using world normal as lookup vector.
        TEXEFFECT_DIFFUSE_CUBE,
        // Specular cubic environment map using world refflection as lookup 
        // vector.
        TEXEFFECT_SPECULAR_CUBE,
        // Maximum enum
        TEXEFFECT_MAX
    };


    // This enumeration determines how this material deals with texture maps.
    enum TextureMap
    {
        MAP_PARALLAX = 0,
        MAP_BASE,
        MAP_NORMAL,
        MAP_DARK,
        MAP_DETAIL,
        MAP_BUMP,
        MAP_GLOSS,
        MAP_GLOW,
        MAP_CUSTOM00,
        MAP_CUSTOM01,
        MAP_CUSTOM02,
        MAP_CUSTOM03,
        MAP_CUSTOM04,
        MAP_DECAL00,
        MAP_DECAL01,
        MAP_DECAL02,
        MAP_ENV,
        MAP_PROJECTED_LIGHT00,
        MAP_PROJECTED_LIGHT01,
        MAP_PROJECTED_LIGHT02,
        MAP_PROJECTED_SHADOW00,
        MAP_PROJECTED_SHADOW01,
        MAP_PROJECTED_SHADOW02,
        MAP_DIRSHADOW00,
        MAP_DIRSHADOW01,
        MAP_DIRSHADOW02,
        MAP_DIRSHADOW03,
        MAP_DIRSHADOW04,
        MAP_DIRSHADOW05,
        MAP_DIRSHADOW06,
        MAP_DIRSHADOW07,
        MAP_POINTSHADOW00,
        MAP_POINTSHADOW01,
        MAP_POINTSHADOW02,
        MAP_POINTSHADOW03,
        MAP_POINTSHADOW04,
        MAP_POINTSHADOW05,
        MAP_POINTSHADOW06,
        MAP_POINTSHADOW07,
        MAP_SPOTSHADOW00,
        MAP_SPOTSHADOW01,
        MAP_SPOTSHADOW02,
        MAP_SPOTSHADOW03,
        MAP_SPOTSHADOW04,
        MAP_SPOTSHADOW05,
        MAP_SPOTSHADOW06,
        MAP_SPOTSHADOW07,
        MAP_MAX
    };

    // This enumeration determines how a texture resource will be sampled for
    // this material.
    enum TextureMapSampleType
    {
        // Sample the red, green, blue, and alpha channels of this texture in 
        // 2D.
        TEXTURE_SAMPLE_RGBA = 0,
        // Sample the red, green, and blue channels of this texture in 2D.
        TEXTURE_SAMPLE_RGB,
        // Sample the red, green, and blue channels of this texture using a
        // projected texture lookup in 3D.
        TEXTURE_SAMPLE_PROJ_RGB,
        // Sample the red, green, and blue channels of this texture using a 
        // cube-map lookup in 3D.
        TEXTURE_SAMPLE_CUBE_RGB,
        // Sample the alpha channel of this texture in 2D.
        TEXTURE_SAMPLE_A,
        // Maximum enum
        TEXTURE_SAMPLE_MAX
    };

    // This enumeration determines how a texture resource will be applied for
    // this material.
    enum TextureMapApplyType
    {
        // Multiply the sampled color by the input color.
        TEXTURE_RGB_APPLY_MULTIPLY = 0,
        // Add the sampled color to the input color.
        TEXTURE_RGB_APPLY_ADD,
        // Linearly interpolate between the sampled color and the input color.
        TEXTURE_RGB_APPLY_LERP,
        // Pass the input color through.
        TEXTURE_RGB_APPLY_NOOP,
        // Maximum enum
        TEXTURE_MAX
    };

    enum 
    {
        // The maximum number of UVs available for textures
        STANDARD_PIPE_MAX_UVS_FOR_TEXTURES = 12,
        // The standard pipeline maximum number of textures
        STANDARD_PIPE_MAX_TEXTURE_MAPS  = 16,
        // The standard pipeline maximum number of decal maps
        STANDARD_PIPE_MAX_DECAL_MAPS = 3,
        // The standard pipeline maximum number of shader maps
        STANDARD_PIPE_MAX_SHADER_MAPS = 5,
        // The standard pipeline maximum number of lights
        STANDARD_PIPE_MAX_LIGHTS = 8,
        // The maximum number of custom maps
        STANDARD_PIPE_CUSTOM_MAPS = 5,
        // The hash bucket size
        MATERIAL_HASH_BUCKET_COUNT = 37,
        // The current vertex version. Adjusting this invalidates the vertex
        // cache and forces new shaders to be generated.
        VERTEX_VERSION = 26,
        // The current geometry version. Adjusting this invalidates the 
        // geometry cache and forces new shaders to be generated.
        GEOMETRY_VERSION = 0,
        // The current pixel version. Adjusting this invalidates the pixel
        // cache and forces new shaders to be generated.
        PIXEL_VERSION = 31
    };

    NiStandardMaterial(NiMaterialNodeLibrary* pkLibrary, 
        bool bAutoCreateCaches = true);

    // Get and set whether or not the lighting is done per-pixel.
    void SetForcePerPixelLighting(bool bForce);
    bool GetForcePerPixelLighting() const;

    // Get and set whether or not the lighting restults are saturated.
    void SetSaturateShading(bool bSaturate);
    bool GetSaturateShading() const;

    // Get and set whether or not textures are saturated whenever an 
    // operation is performed.
    void SetSaturateTextures(bool bSaturate);
    bool GetSaturateTextures() const;

    // Inherited from NiFragmentMaterial. Creates the default fallback 
    // functions.
    virtual void AddDefaultFallbacks();

    // Fallback techniques

    // Create a new pass for each pass with lighting. The lights are evenly 
    // divided from the original pass into the new passes. Note that this only
    // affects per-pixel lights. These passes will be additively blended.
    static bool SplitPerPixelLights(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

    // This technique will remove the shadow maps from the input passes.
    static bool DropShadowMaps(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

    // This technique is a combination of DropShadowMaps and 
    // SplitPerPixelLights.
    static bool DropShadowMapsThenSplitPerPixelLights(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

    // Create a new pass for each pass with lighting. The lights are evenly 
    // divided from the original pass into the new passes. Note that this only
    // affects per-vertex lighting. These passes will be additively blended.
    static bool SplitPerVertexLights(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

    // Split texture accesses into two passes. The first pass will include the
    // standard textures and lighting minus the textures in the second pass. 
    // The second pass will only have the glow, environment, bump, projected 
    // lights, and projected shadows with no lighting. These two passes will 
    // be additively blended.
    static bool SplitTextureMaps(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

    // This technique is a combination of DropShadowMaps and SplitTextureMaps.
    static bool DropShadowMapsThenSplitTextureMaps(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

    // This technique will remove the parallax map from the input passes.
    static bool DropParallaxMap(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

    // This technique is a combination of DropParallaxMap and 
    // SplitPerPixelLights.
    static bool DropParallaxMapThenSplitLights(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

    // This technique is a combination of DropShadowMaps, DropParallaxMap,
    // and SplitPerPixelLights..
    static bool DropShadowMapsThenDropParallaxMapThenSplitLights(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

protected:
    // Derived classes should call this constructor to register their material
    // name
    NiStandardMaterial(const NiFixedString& kName, 
        NiMaterialNodeLibrary* pkLibrary, unsigned int uiVertexVersion,
        unsigned int uiGeometryVersion, unsigned int uiPixelVersion, 
        bool bAutoCreateCaches);
    NiStandardMaterial(const NiFixedString& kName, 
        NiMaterialNodeLibrary* pkLibrary, bool bAutoCreateCaches);
        
    // Inherited from NiFragmentMaterial. Creates the NiShader that will be 
    // filled in by later methods.
    virtual NiShader* CreateShader(NiMaterialDescriptor* pkDesc);

    // Inherited from NiFragmentMaterial. Configures the shader's packing 
    // structures for the requirements of this instance of the material. 
    // Note that this method is considered deprecated, and 
    // GetVertexInputSemantics should be used instead.
    virtual bool SetupPackingRequirements(NiShader* pkShader,
        NiMaterialDescriptor* pkMaterialDescriptor,
        RenderPassDescriptor* pkRenderPasses, unsigned int uiCount);

    // Inherited from NiFragmentMaterial. Generate the descriptor object that 
    // matches the state of this geometry. This descriptor will be an 
    // NiStandardMaterialDescriptor object.
    virtual bool GenerateDescriptor(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects,
        NiMaterialDescriptor& kMaterialDesc);

    // Inherited from NiFragmentMaterial. This derived implementation adds
    // special case code for texture transforms in the vertex program to 
    // properly map up to predefines for the pixel program.
    virtual bool AddResourceToShaderConstantMap(NiShader* pkShader,
        NiShaderConstantMap* pkMap, NiMaterialResource* pkRes, 
        NiGPUProgramDescriptor* pkVertexDesc, 
        NiGPUProgramDescriptor* pkGeometryDesc, 
        NiGPUProgramDescriptor* pkPixelDesc, 
        const NiTObjectSet<ObjectResourceOffset>& kObjectOffsets, 
        unsigned int uiPass, NiGPUProgram::ProgramType eCurrentType);

    // Inherited from NiFragmentMaterial. This derived implementation controls
    // the wiring of the vertex shader material nodes.
    virtual bool GenerateVertexShadeTree(Context& kContext, 
        NiGPUProgramDescriptor* pkDesc);
    
    // Inherited from NiFragmentMaterial. This derived implementation controls
    // the wiring of the geometry shader material nodes. Since, 
    // NiStandardMaterial does not directly use geometry shaders, this method
    // always returns true.
    virtual bool GenerateGeometryShadeTree(Context& kContext, 
        NiGPUProgramDescriptor* pkDesc);
    
    // Inherited from NiFragmentMaterial. This derived implementation controls
    // the wiring of the pixel shader material nodes.
    virtual bool GeneratePixelShadeTree(Context& kContext, 
        NiGPUProgramDescriptor* pkDesc);

    // Inherited from NiFragmentMaterial. By default this creates one render 
    // pass with an NiStandardPixelProgramDescriptor and an 
    // NiStandardVertexProgramDescriptor.
    virtual ReturnCode GenerateShaderDescArray(
        NiMaterialDescriptor* pkMaterialDescriptor,
        RenderPassDescriptor* pkRenderPasses, unsigned int uiMaxCount,
        unsigned int& uiCountAdded);

    // This method takes the shader programs generated by the shader 
    // generating functions and returns RC_SUCCESS if they were created 
    // successfully.
    virtual unsigned int VerifyShaderPrograms(NiGPUProgram* pkVertexShader,
        NiGPUProgram* pkGeometryShader, NiGPUProgram* pkPixelShader);

    // This method deals with generating the world position fragment using the
    // technique specified in the NiStandardMaterial::TransformType 
    // enumeration. This method is called when constructing the vertex
    // shade tree.
    virtual bool HandlePositionFragment(Context& kContext,
        TransformType eTransType, 
        NiMaterialResource*& pkVertWorldPos,
        NiMaterialResource*& pkWorldMatrix);

    // This method deals with generating the world normals and potentially 
    // binormal and tangent fragment using the technique specified in the
    // NiStandardMaterial::NormalType enumeration. This method is called 
    // when constructing the vertex shade tree.
    virtual bool HandleNormalFragment(Context& kContext,
        NormalType eNormType,
        NiMaterialResource* pkWorldMatrix,
        NiMaterialResource*& pkVertWorldNorm,
        NiMaterialResource*& pkVertWorldBiNorm,
        NiMaterialResource*& pkVertWorldTangent);

    // This method deals with manipulating the position after the world 
    // transform has been computed. This method is here as a hook for 
    // inheritance only. This method is called when constructing the vertex
    // shade tree.
    virtual bool HandlePostWorldTransform(Context& kContext,
        NiGPUProgramDescriptor* pkVertexProgram,
        NiMaterialResource*& pkVertWorldPos,
        NiMaterialResource*& pkVertWorldNorm);

    // This method applies the view projection matrix to the world position. 
    // If requested, this method will generate resources for the computed 
    // view position. This method is called when constructing the vertex
    // shade tree.
    virtual bool HandleViewProjectionFragment(Context& kContext, 
        bool bForceViewPos, NiMaterialResource* pkVertWorldPos,
        NiMaterialResource*& pkVertOutProjectedPos,
        NiMaterialResource*& pkVertOutViewPos);

    // This method generates the view vector and if requested generates the 
    // view vector in a specific tangent space. This method is called when 
    // constructing the vertex shade tree.
    virtual bool HandleViewVectorFragment(Context& kContext,
        NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm,
        NiMaterialResource* pkWorldBinorm, NiMaterialResource* pkWorldTangent,
        bool bComputeTangent, NiMaterialResource*& pkWorldViewVector, 
        NiMaterialResource*& pkTangentViewVector);

    // This method handles computing the world space reflection vector from 
    // the world normal and world view vector. Optionally, the user can cause
    // the world normal to be normalized before calculations are made. 
    // This method is called when constructing the vertex shade tree or the
    // pixel shade tree depending on the stage in which lighting occurs.
    virtual bool HandleReflectionVectorFragment(Context& kContext,
        NiMaterialResource* pkWorldNorm, bool bNormalizeNormal, 
        NiMaterialResource* pkWorldViewVector, 
        NiMaterialResource*& pkWorldReflectionVector);

    // This method encapsulates the vertex position/normal/binormal/tangent 
    // transformation pipeline. If requested, this method will generate the 
    // world view vector. 
    virtual bool SetupTransformPipeline(Context& kContext,
        NiMaterialResource* pkVertOutProjPos,
        NiStandardVertexProgramDescriptor* pkVertDesc, bool bForceView,
        bool bForceViewPos, NiMaterialResource*& pkWorldPos, 
        NiMaterialResource*& pkViewPos, NiMaterialResource*& pkProjectedPos,  
        NiMaterialResource*& pkWorldNormal, NiMaterialResource*& pkWorldView);

    // This method adds the appropriate fragment to initialize the 
    // ambient/diffuse/specular/emissive colors from per-vertex values or 
    // NiMaterialProperty constants for the pixel shader or vertex
    // shader depending on the stage in which lighting occurs.
    virtual bool HandleInitialSpecAmbDiffEmissiveColor(Context& kContext,
        bool bSpecularOn, AmbDiffEmissiveEnum eADF, 
        LightingModeEnum eLightingMode,
        NiMaterialResource*& pkDiffuseColorRes,
        NiMaterialResource*& pkSpecularColorRes,
        NiMaterialResource*& pkSpecularPowerRes,
        NiMaterialResource*& pkAmbientColorRes,
        NiMaterialResource*& pkEmissiveColorRes,
        NiMaterialResource*& pkOpacityRes);

    // This method sets up the fogging pipeline in the vertex shader.
    virtual bool HandleCalculateFog(Context& kContext,
        NiMaterialResource* pkViewPos, Fog eFogType);

    // This method controls the UV coordinate generation pipeline for the 
    // vertex shader. This covers passing through or transforming input UVs as
    // well as procedurally generating UV for projected lights/shadows/etc.
    virtual bool HandleVertexUVSets(Context& kContext,
        NiStandardVertexProgramDescriptor* pkVertDesc, 
        NiMaterialResource* pkWorldPos,
        NiMaterialResource* pkWorldNormal,
        NiMaterialResource* pkWorldReflect);

    // This method controls any additional outputs the user wishes to send
    // from the vertex shader. By default, it calls HandleVertexUVSets and
    // HandleCalculateFog, but a user may override or extend this ability in
    // a derived class.
    virtual bool HandleFinalVertexOutputs(Context& kContext,
        NiStandardVertexProgramDescriptor* pkVertDesc, 
        NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNormal,
        NiMaterialResource* pkWorldReflect, NiMaterialResource* pkViewPos,
        NiMaterialResource* pkProjectedPos);

    // This method generates the UV coordinate array for use by textures in 
    // the vertex shader. As needed, it will apply texture transforms to
    // the input UV set and make sure that the UVs are output in the order 
    // expected by the pixel shader.s
    virtual bool HandleTextureUVSets(Context& kContext,
        NiStandardVertexProgramDescriptor* pkVertexDesc,
        NiMaterialResource** apkOutputUVResources, size_t stOutputUVMax,
        unsigned int& uiNextUVIndex);

    // This method generates the UV coordinates for use by texture effects by 
    // applying a world parallel or perspective transform to world position,
    // normal, or reflection vectors as specified by the type fields.
    virtual bool HandleTextureEffectUVSets(Context& kContext,
        NiMaterialResource** apkOutputUVResources, size_t stOutputUVMax,
        NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm, 
        NiMaterialResource* pkWorldReflection, 
        TexEffectType eEnvironmentMapType, unsigned int uiProjectedLightCount, 
        unsigned int uiProjectedLightTypes, 
        unsigned int uiProjectedShadowCount, 
        unsigned int uiProjectedShadowTypes,
        unsigned int& uiNextUVIndex);
    
    // This is the lowest-level method for creating UVs for generic texture 
    // sampling by either passing-through UV coordinates or applying a 
    // texture transformation. It is called from HandleTextureUVSets above.
    virtual bool HandleTextureUVSetGen(Context& kContext, 
        NiMaterialResource* pkInputUVResource,
        NiMaterialResource*& pkOutputUVResource,
        const NiFixedString& kMapName, unsigned int uiExtra,
        NiStandardMaterial::TexGenOutput eOutputType);

    // This is the lowest-level method for creating UVs for any of the texture
    // effects supported by NiStandardMaterial. 
    // It is called from HandleTextureEffectUVSets above.
    virtual bool HandleTextureEffectUVSetGen(Context& kContext,
        NiMaterialResource* pkWorldPos,
        NiMaterialResource* pkWorldNorm, 
        NiMaterialResource* pkWorldReflection,
        NiShaderAttributeDesc::ObjectType eObjType,
        TexEffectType eEffectType, unsigned int uiOccurance,
        NiMaterialResource*& pkUVResource);
    
    // This method controls the lighting and material property pipeline 
    // for the vertex shader. It controls the application of per-vertex 
    // lighting or alternatively does the per-vertex work needed for per-pixel
    // lighting.
    virtual bool HandleVertexLightingAndMaterials(Context& kContext,
        NiStandardVertexProgramDescriptor* pkVertexDesc,
        NiMaterialResource* pkWorldPos,
        NiMaterialResource* pkWorldNormal,
        NiMaterialResource* pkWorldView);

    // This method controls the input data structure creation to the pixel 
    // shader for any non-UV parameters, including the world position, world
    // normal, world binormal, world tangent, world view, and tangent view
    // vectors, based on the state of the pixel descriptor. Variables are only
    // instantiated if they are requested by the descriptor.
    virtual bool HandlePixelInputs(Context& kContext, 
        NiStandardPixelProgramDescriptor* pkPixelDesc,
        NiMaterialResource*& pkPixelWorldPos,
        NiMaterialResource*& pkPixelWorldNorm,
        NiMaterialResource*& pkPixelWorldBinormal,
        NiMaterialResource*& pkPixelWorldTangent,
        NiMaterialResource*& pkPixelWorldViewVector,
        NiMaterialResource*& pkPixelTangentViewVector);

    // This method is called by GeneratePixelShadeTree to initialize most of 
    // the material variables for the texture and lighting pipelines. This 
    // also includes using the correct per-vertex parameters for per-pixel or
    // per-vertex lighting. Variables are only instantiated if they are 
    // requested by the descriptor.
    virtual bool HandlePixelMaterialInitialValues(Context& kContext,
        NiStandardPixelProgramDescriptor* pkPixelDesc,
        unsigned int uiPixelLightCount, bool bSpecular, 
        LightingModeEnum& eLightingMode,
        NiMaterialResource*& pkMatDiffuse,
        NiMaterialResource*& pkMatSpecular,
        NiMaterialResource*& pkSpecularPower,
        NiMaterialResource*& pkGlossiness,
        NiMaterialResource*& pkMatAmbient,
        NiMaterialResource*& pkMatEmissive,
        NiMaterialResource*& pkTexDiffuseAccum,
        NiMaterialResource*& pkTexSpecularAccum,
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource*& pkSpecularAccum,
        NiMaterialResource*& pkOpacityAccum,
        NiMaterialResource*& pkLightDiffuseAccum,
        NiMaterialResource*& pkLightSpecularAccum,
        NiMaterialResource*& pkLightAmbientAccum);

    // This method is called by GeneratePixelShadeTree to initialize the UV
    // sets output from the vertex shader. The input array is allocated by the
    // calling function, but is filled in by this method. Additionally, the
    // number of input simple UVs and number of input dynamic effect UVs
    // must be returned so that later steps in the texturing pipeline can
    // find their UV sets.
    virtual bool HandlePixelUVSets(Context& kContext,
        NiStandardPixelProgramDescriptor* pkPixelDesc,  
        NiMaterialResource** ppkUVSets, 
        unsigned int uiMaxUVIndex,
        unsigned int& uiNumStandardUVs, 
        unsigned int& uiDynamicEffectCount);

    // This method is called by GeneratePixelShadeTree to sample the following
    // NiStandardMaterial maps prior to any lighting calculations being done:
    // Parallax, Dark, Normal, Projected Lights, Base, Detail, Decal, Custom,
    // Projected Shadow, and Gloss.
    virtual bool HandlePreLightTextureApplication(Context& kContext,
        NiStandardPixelProgramDescriptor* pkPixelDesc, 
        NiMaterialResource*& pkWorldPos,
        NiMaterialResource*& pkWorldNormal,
        NiMaterialResource*& pkWorldBinormal,
        NiMaterialResource*& pkWorldTangent,
        NiMaterialResource*& pkWorldViewVector,
        NiMaterialResource*& pkTangentViewVector,
        NiMaterialResource*& pkMatDiffuseColor,
        NiMaterialResource*& pkMatSpecularColor,
        NiMaterialResource*& pkMatSpecularPower,
        NiMaterialResource*& pkMatGlossiness,
        NiMaterialResource*& pkMatAmbientColor,
        NiMaterialResource*& pkMatEmissiveColor,
        NiMaterialResource*& pkOpacityAccum,
        NiMaterialResource*& pkAmbientLightAccum,
        NiMaterialResource*& pkDiffuseLightAccum,
        NiMaterialResource*& pkSpecularLightAccum,
        NiMaterialResource*& pkDiffuseTexAccum,
        NiMaterialResource*& pkSpecularTexAccum,
        unsigned int& uiTexturesApplied,
        NiMaterialResource** apkUVSets,
        unsigned int uiNumStandardUVs,
        unsigned int uiNumTexEffectUVs);

    // This method combines the input material and lighting values to compute
    // the specular and diffuse coefficients of the shading equation. This 
    // method is called by either the vertex or pixel shader depending on
    // the stage in which lighting is performed.
    virtual bool HandleShadingCoefficients(Context& kContext,
        NiMaterialResource* pkMatEmissive, 
        NiMaterialResource* pkMatDiffuse, NiMaterialResource* pkMatAmbient, 
        NiMaterialResource* pkMatSpecular, 
        NiMaterialResource* pkLightSpecularAccum,
        NiMaterialResource* pkLightDiffuseAccum, 
        NiMaterialResource* pkLightAmbientAccum,
        NiMaterialResource*& pkDiffuseCoeff, 
        NiMaterialResource*& pkSpecularCoeff);

    // This method is called by GeneratePixelShadeTree to sample the following
    // NiStandardMaterial maps after all lighting calculations have been done:
    // Bump, Environment, and Glow
    virtual bool HandlePostLightTextureApplication(Context& kContext,
        NiStandardPixelProgramDescriptor* pkPixelDesc,
        NiMaterialResource*& pkWorldNormal, 
        NiMaterialResource* pkViewVector, 
        NiMaterialResource*& pkMatOpacity,
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource*& pkSpecularAccum,
        NiMaterialResource* pkGlossiness,
        unsigned int& uiTexturesApplied,
        NiMaterialResource** apkUVSets,
        unsigned int uiNumStandardUVs,
        unsigned int uiNumTexEffectUVs);

    // This method applies a parallax map material node to the pixel shade
    // tree.
    virtual bool HandleParallaxMap(Context& kContext, 
        NiMaterialResource*& pkUVSet, NiMaterialResource* pkTangentViewVector);

    // This method applies a base map material node to the pixel shade
    // tree.
    virtual bool HandleBaseMap(Context& kContext, NiMaterialResource* pkUVSet,
        NiMaterialResource*& pkDiffuseColorAccum, 
        NiMaterialResource*& pkOpacity, bool bOpacityOnly);

    // This method applies a normal map material node to the pixel shade
    // tree.
    virtual bool HandleNormalMap(Context& kContext, 
        NiMaterialResource* pkUVSet, NormalMapType eType, 
        NiMaterialResource*& pkWorldNormal, 
        NiMaterialResource* pkWorldBinormal, 
        NiMaterialResource* pkWorldTangent);

    // This method applies a dark map material node to the pixel shade
    // tree.
    virtual bool HandleDarkMap(Context& kContext, NiMaterialResource* pkUVSet,
        NiMaterialResource*& pkDiffuseColorRes, 
        NiMaterialResource*& pkSpecularTexAccum);

    // This method applies a detail map material node to the pixel shade
    // tree.
    virtual bool HandleDetailMap(Context& kContext, 
        NiMaterialResource* pkUVSet, NiMaterialResource*& pkDiffuseColorRes,
        NiMaterialResource*& pkSpecularTexAccum);

    // This method applies a gloss map material node to the pixel shade
    // tree.
    virtual bool HandleGlossMap(Context& kContext, NiMaterialResource* pkUVSet,
        NiMaterialResource*& pkGlossiness);

    // This method applies a bump map material node to the pixel shade
    // tree.
    virtual bool HandleBumpMap(Context& kContext, NiMaterialResource* pkUVSet,
        NiMaterialResource*& pkBumpOffset);

    // This method applies a glow map material node to the pixel shade
    // tree.
    virtual bool HandleGlowMap(Context& kContext, NiMaterialResource* pkUVSet,
        NiMaterialResource*& pkEmissiveColorRes,
        NiMaterialResource*& pkSpecularTexAccum);

    // This method applies custom map material nodes to the pixel shade
    // tree.
    virtual bool HandleCustomMaps(Context& kContext, 
        NiStandardPixelProgramDescriptor* pkPixDesc, 
        unsigned int& uiWhichTexture, NiMaterialResource** apkUVSets,
        unsigned int uiNumStandardUVs, 
        NiMaterialResource*& pkMatDiffuseColor,
        NiMaterialResource*& pkMatSpecularColor,
        NiMaterialResource*& pkMatSpecularPower,
        NiMaterialResource*& pkMatGlossiness,
        NiMaterialResource*& pkMatAmbientColor,
        NiMaterialResource*& pkMatEmissiveColor,
        NiMaterialResource*& pkOpacityAccum,
        NiMaterialResource*& pkAmbientLightAccum,
        NiMaterialResource*& pkDiffuseLightAccum,
        NiMaterialResource*& pkSpecularLightAccum,
        NiMaterialResource*& pkDiffuseTexAccum,
        NiMaterialResource*& pkSpecularTexAccum);

    // This method applies an environment map material node to the pixel shade
    // tree.
    virtual bool HandleEnvMap(Context& kContext, 
        NiStandardMaterial::TexEffectType eTexEffect,
        NiMaterialResource* pkUVResource,
        NiMaterialResource*& pkDiffuseAccum, 
        NiMaterialResource*& pkSpecularAccum, 
        NiMaterialResource* pkBumpOffset,
        NiMaterialResource* pkGlossiness);

    // This method applies a decal map material node to the pixel shade
    // tree.
    virtual bool HandleDecalMap(Context& kContext, NiMaterialResource* pkUVSet,
        unsigned int uiIdx, NiMaterialResource*& pkDiffuseColorRes,
        NiMaterialResource*& pkSpecularTexAccum);

    // This method applies a projected light map node to the pixel shade tree.
    virtual bool HandleProjLightMap(Context& kContext, 
        NiMaterialResource* pkUVSet, unsigned int uiIdx, bool bPerspective,
        bool bClipped, NiMaterialResource* pkWorldPos,
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource*& pkSpecularTexAccum);

    // This method applies a projected shadow map node to the pixel shade
    // tree.
    virtual bool HandleProjShadowMap(Context& kContext,
        NiMaterialResource* pkUVSet,  unsigned int uiIdx, bool bPerspective, 
        bool bClipped, NiMaterialResource* pkWorldPos,
        NiMaterialResource*& pkDiffuseColorRes,
        NiMaterialResource*& pkSpecularTexAccum);

    // This method controls the contribution of lighting to the object.
    // It can be called from either the vertex or pixel shader depending on
    // the context in which lighting is occuring.
    virtual bool HandleLighting(Context& kContext, 
        bool bSpecular, unsigned int uiNumPoint, unsigned int uiNumDirectional,
        unsigned int uiNumSpot, unsigned int uiShadowBitfield,
        unsigned int uiShadowTechnique,
        NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm,
        NiMaterialResource* pkViewVector, 
        NiMaterialResource* pkSpecularPower,
        NiMaterialResource*& pkAmbientAccum,
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource*& pkSpecularAccum);

    // This method computes the multiplier that a shadow applies to a given
    // light. This method is called when generating a pixel shade tree.
    virtual bool HandleShadow(Context& kContext, unsigned int uiLight, 
        LightType eType, unsigned int uiLightByType, 
        NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm,
        unsigned int uiShadowTechnique, NiMaterialResource*& pkShadow);

    // This method controls the contribution of a single light to an object.
    // This method can be called from either the vertex or pixel shade tree
    // depending on where lighting is occurring.
    virtual bool HandleLight(Context& kContext, unsigned int uiLight,
        LightType eType, unsigned int uiLightByType, 
        NiMaterialResource* pkWorldPos, 
        NiMaterialResource* pkWorldNorm, NiMaterialResource* pkViewVector, 
        NiMaterialResource* pkSpecularPower,
        NiMaterialResource*& pkAmbientAccum,
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource*& pkSpecularAccum,
        bool bSpecular, NiMaterialResource* pkShadow);

    // This method controls the application of fog to the pixel shade tree.
    virtual bool HandleApplyFog(Context& kContext, 
        NiStandardPixelProgramDescriptor* pkPixDesc,
        NiMaterialResource* pkUnfoggedColor, 
        NiMaterialResource*& pkFogOutput);

    // This method controls the application of alpha testing to the pixel 
    // shade tree. This is currently only used for D3D10 shade trees.
    virtual bool HandleAlphaTest(Context& kContext,
        NiStandardPixelProgramDescriptor* pkPixDesc,
        NiMaterialResource* pkAlphaTestInput);

    // This method controls the combination of the shading coefficients from
    // a material, textures, and lighting. This also handles the differences 
    // in computation needed for per-pixel and per-vertex lighting.
    virtual bool HandleColorAccumulation(Context& kContext,
        NiStandardPixelProgramDescriptor* pkPixelDesc,
        NiMaterialResource* pkMatEmissive, 
        NiMaterialResource* pkMatDiffuse, 
        NiMaterialResource* pkMatAmbient, 
        NiMaterialResource* pkMatSpecular, 
        NiMaterialResource* pkLightSpecularAccum, 
        NiMaterialResource* pkLightDiffuseAccum, 
        NiMaterialResource* pkLightAmbientAccum,
        NiMaterialResource* pkGlossiness, 
        NiMaterialResource* pkTexDiffuseAccum, 
        NiMaterialResource* pkTexSpecularAccum,
        LightingModeEnum eLightingMode, 
        NiMaterialResource*& pkSpecularAccum,
        NiMaterialResource*& pkDiffuseAccum);

    // This is the final method called when generating a pixel shade tree and
    // controls how the data is finally written to the pixel shader outputs.
    virtual bool HandleFinalPixelOutputs(Context& kContext,
        NiStandardPixelProgramDescriptor* pkPixDesc,
        NiMaterialResource* pkDiffuseAccum,
        NiMaterialResource* pkSpecularAccum,
        NiMaterialResource* pkOpacityAccum);

    // This method handles generating a UV set name by index.
    virtual NiFixedString GenerateUVSetName(unsigned int uiIndex);

    // This is a utility method to add material nodes to normalize a float2,
    // float3, or float4 vector.
    virtual bool NormalizeVector(Context& kContext, 
        NiMaterialResource*& pkVector);

    // This is a utility method to add material nodes to add float, float2,
    // float3, or float4 vectors.
    virtual bool AddVector(Context& kContext, 
        NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
        NiMaterialResource*& pkValue);

    // This is a utility method to add material nodes to multiply float, 
    // float2, float3, or float4 vectors.
    virtual bool MultiplyVector(Context& kContext, 
        NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
        NiMaterialResource*& pkValue);

    // This is a utility method to add material nodes to multiply/add float3 
    // vectors.
    virtual bool MultiplyAddVector(Context& kContext, 
        NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
        NiMaterialResource* pkVector3, NiMaterialResource*& pkValue);

    // This is a utility method to add material nodes to linearly interpolate
    // float, float2, float3, or float4 vectors.
    virtual bool LerpVector(Context& kContext, 
        NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
        NiMaterialResource* pkLerpAmount, NiMaterialResource*& pkValue);

    // This is a utility method to add material nodes to multiply a float, 
    // float2, float3, or float4 vector by a single float.
    virtual bool ScaleVector(Context& kContext, 
        NiMaterialResource* pkVector, NiMaterialResource* pkScale,
        NiMaterialResource*& pkValue);

    // This is a utility method to add material nodes to saturate float, 
    // float2, float3, or float4 vector.
    virtual bool SaturateVector(Context& kContext, 
        NiMaterialResource* pkVector, NiMaterialResource*& pkValue);

    // This is a utility method to add material nodes to multiply a float3 
    // vector by the sum of another float3 vector and a scalar.
    virtual bool MultiplyScalarSatAddVector(Context& kContext, 
        NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
        NiMaterialResource* pkScale, NiMaterialResource*& pkValue);

    // This method adds a texture to the shade-tree by enumeration. This 
    // texture will be sampled according to the TextureMapSampleType, eSample,
    // and input UV set, pkUV. The sampled color has the mathematical 
    // operation defined by TextureMapApplyType argument, using the 
    // pkInputColor. The sampled color can optionally be scaled using the 
    // pkSampledRGBScalar parameter. The final computed RGB color will be 
    // passed by reference using pkOutputColor. If alpha was specified, the 
    // opacity value will be passed by reference using pkOutputOpacity.
    virtual bool InsertTexture(Context& kContext, TextureMap eMap, 
        unsigned int uiOccurance, TextureMapApplyType eApplyType, 
        TextureMapSampleType eSample, NiMaterialResource* pkUV,
        NiMaterialResource* pkInputColor, 
        NiMaterialResource* pkSampledRGBScalar,
        NiMaterialResource*& pkOutputColor,
        NiMaterialResource*& pkOutputOpacity);

    // This method adds a texture to the shade-tree by name. This texture will
    // be sampled according to the TextureMapSampleType, eSample, and input 
    // UV set, pkUV. The sampled color has the mathematical operation defined
    // by TextureMapApplyType argument, using the pkInputColor. The sampled 
    // color can optionally be scaled using the pkSampledRGBScalar parameter. 
    // The final computed RGB color will be passed by reference using 
    // pkOutputColor. If alpha was specified, the opacity value will be 
    // passed by reference using pkOutputOpacity.
    virtual bool InsertTexture(Context& kContext, 
        const NiFixedString& kSamplerName, unsigned int uiOccurance, 
        TextureMapApplyType eApplyType, TextureMapSampleType eSamplerType, 
        NiMaterialResource* pkUV, NiMaterialResource* pkInputColor, 
        NiMaterialResource* pkSampledRGBScalar,
        NiMaterialResource*& pkOutputColor, 
        NiMaterialResource*& pkOutputOpacity);

    // This method will add material nodes to split a float4 vector into 
    // float3 and float vectors.
    virtual bool SplitColorAndOpacity(Context& kContext, 
        NiMaterialResource* pkColorAndOpacity, NiMaterialResource*& pkColor,
        NiMaterialResource*& pkOpacity);

    // This method will add material nodes to split a float4 vector into 
    // float3 and float vectors.
    virtual bool SplitRGBA(Context& kContext, 
        NiMaterialResource* pkColorAndOpacity, NiMaterialResource*& pkRed,
        NiMaterialResource*& pkGreen, NiMaterialResource*& pkBlue,
        NiMaterialResource*& pkAlpha);

    // This utility method will invoke conversion functions when binding 
    // resources with common float count mismatches, i.e. binding a float4
    // to a float3 truncates the fourth float value. When the resources match
    // types, a normal bind occurs.
    virtual bool TypeCastBind(Context& kContext, 
        NiMaterialResource* pkSrc, NiMaterialResource* pkDest);

    // This method finds the UV set array index associated with the 
    // uiTexture-th texture in the descriptor. 
    static unsigned int FindUVSetIndexForTextureIndex(
        unsigned int uiTexture, NiStandardPixelProgramDescriptor* pkPixDesc);

    // This method finds the UV set resource associated with the enumerated 
    // texture in the descriptor.
    static NiMaterialResource* FindUVSetIndexForTextureEnum( 
        TextureMap eMap, NiStandardPixelProgramDescriptor* pkPixDesc,
        NiMaterialResource** apkUVSets);

    // This method finds the UV set index associated with the enumerated 
    // texture in the descriptor.
    static unsigned int FindUVSetIndexForTextureEnum(TextureMap eMap, 
        NiStandardPixelProgramDescriptor* pkPixDesc);

    // This method determines the texture occurrence from the texture 
    // enumeration for this descriptor.
    static int MapIndexFromTextureEnum(TextureMap eMap,
        NiStandardPixelProgramDescriptor* pkPixDesc);

    // Get the name of the texture and its index for that texture name from 
    // the texture enumeration.
    static bool GetTextureNameFromTextureEnum(TextureMap eMap,
        NiFixedString& kString, unsigned int& uiOccurance);

    // Get the texture transform predefined mapping from the texture 
    // enumeration.
    static bool GetTextureTransformMappingFromEnum(TextureMap eMap, 
        NiShaderConstantMap::DefinedMappings& eMapping, 
        unsigned int& uiOccurance);

    // This method removes the specified map from the descriptors for pixel
    // and vertex shade trees.
    static bool RemoveTextureMap(TextureMap eMap, 
        NiStandardVertexProgramDescriptor* pkVertDesc, 
        NiStandardPixelProgramDescriptor* pkPixDesc);

    // This method assists in the remapping of UV coordinates when a map is 
    // removed from the descriptors.
    static bool GenerateVertexOutputUVArrays(
        unsigned int* puiInputUVSets, TexGenOutput* peTexGenOutputs,
        unsigned int* puiOutputUVSetsByTexIndex,
        unsigned int& uiTextureCount, 
        unsigned int& uiOutputUVCount,
        NiStandardVertexProgramDescriptor* pkVertexDesc, 
        NiStandardPixelProgramDescriptor* pkPixelDesc, 
        unsigned int uiMaxTextureCount);

    // Determine if a specific Map enumeration is enabled in a pixel
    // descriptor.
    static bool IsMapEnabled(TextureMap eMap, 
        NiStandardPixelProgramDescriptor* pkPixelDesc);

    // Enable or disable a specific Map enumeration in a pixel descriptor.
    static bool SetMapEnabled(TextureMap eMap, bool bEnabled,
        NiStandardPixelProgramDescriptor* pkPixelDesc);

    // This method generates the mapping from input UV sets to output UV sets
    // so that the vertex and pixel shade trees have compatible inputs and
    // outputs. 
    static bool AssignTextureCoordinates(const unsigned int* puiUVSets,
        const TexGenOutput* peTexGenOutputs, const unsigned int uiTextureCount,
        NiStandardVertexProgramDescriptor* pkVertexDesc, 
        NiStandardPixelProgramDescriptor* pkPixelDesc);

    // The type of the descriptor 
    NiFixedString m_kDescriptorName;

    // Forces per-pixel lighting in all shade trees
    bool m_bForcePerPixelLighting;

    // Forces saturation of shading values
    bool m_bSaturateShading;
    
    // Forces saturation of all texture lookups
    bool m_bSaturateTextures;
};

typedef NiPointer<NiStandardMaterial> NiStandardMaterialPtr;

#endif  //#ifndef NISTANDARDMATERIAL_H
