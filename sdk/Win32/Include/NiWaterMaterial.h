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

#ifndef NIWATERMATERIAL_H
#define NIWATERMATERIAL_H


#include <NiFragmentMaterial.h>
#include <NiTexturingProperty.h>

#include "NiTerrainLibType.h"

class NiWaterMaterialVertexDescriptor;
class NiWaterMaterialPixelDescriptor;
class NiWaterMaterialDescriptor;

/**
     This is an NiFragmentMaterial based material used for rendering
     a NiWaterComponent. This material will automatically generate the required
     shader based on the specified parameters.
 */
class NITERRAIN_ENTRY NiWaterMaterial : public NiFragmentMaterial
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    /// @endcond

public:

    /// Constants declared for the global shader constants used by the 
    /// material.
    
    /// Reflection constant
    static const char* REFLECTION_SHADER_CONSTANT;

    /// Specularity constant
    static const char* SPECULARITY_SHADER_CONSTANT;

    /// Water disturbance constant
    static const char* DISTURBANCE_SHADER_CONSTANT;

    /// Fog density constant
    static const char* FOGDENSITY_SHADER_CONSTANT;

    /// Fog Distance constant
    static const char* FOGDISTANCE_SHADER_CONSTANT;

    /// Fog fall off constant
    static const char* FOGFALLOFF_SHADER_CONSTANT;

    /// Normal map offset constant
    static const char* NORMALMAPOFFSET_SHADER_CONSTANT;

    /// Used for tracking the version of the terrain material.
    enum
    {
        /// Specifies the vertex shader version.
        VERTEX_VERSION = 1,

        /// Specifies the pixel shader version.
        PIXEL_VERSION = 1,

        /// Specifies the geometry shader version.
        GEOMETRY_VERSION = 0,

        /// Specifies the maximum number of layers per block.
        MAX_LAYERS_PER_BLOCK = 8,
    };

    enum ReflectionMode
    {
        /// Indicates no reflection.
        REFLECTION_MODE_NONE                = 0,

        /// Indicates to use cube map for the reflection mode.
        REFLECTION_MODE_CUBE_MAP            = 1,

        /// Indicates to use planar projection for reflection mode.
        REFLECTION_MODE_PLANAR_PROJECTION   = 2
    };

    /// Allows the water to have distance fog.
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
    
    virtual ~NiWaterMaterial();

    static NiWaterMaterial* Create();

    /**
        Inherited from NiFragmentMaterial. Creates the default fallback 
        functions.
    */
    virtual void AddDefaultFallbacks();
    
    /**
        This method switches environment fogging from per-pixel to 
        vertex-based fogging.
    */
    static bool DropPerPixelFogging(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

protected:

    NiWaterMaterial(NiMaterialNodeLibrary* pkMaterialNodeLib,
        bool bAutoCreateCaches = true, const NiFixedString &name=
        "NiWaterMaterial");

    virtual NiShader* CreateShader(NiMaterialDescriptor* pkDesc);

    virtual bool SetupPackingRequirements(NiShader* pkShader,
        NiMaterialDescriptor* pkMaterialDescriptor,
        RenderPassDescriptor* pkRenderPasses, NiUInt32 uiCount);

    virtual bool GenerateDescriptor(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkPropState, 
        const NiDynamicEffectState* pkEffects,
        NiMaterialDescriptor& kMaterialDesc);

    virtual ReturnCode GenerateShaderDescArray(
        NiMaterialDescriptor* pkMaterialDescriptor,
        RenderPassDescriptor* pkRenderPasses, 
        NiUInt32 uiMaxCount,
        NiUInt32& uiCountAdded);

    virtual bool GenerateVertexShadeTree(Context& kContext, 
        NiGPUProgramDescriptor* pkDesc);

    virtual bool GenerateGeometryShadeTree(Context& kContext, 
        NiGPUProgramDescriptor* pkDesc);

    virtual bool GeneratePixelShadeTree(Context& kContext,
        NiGPUProgramDescriptor* pkDesc);
        
    virtual NiUInt32 VerifyShaderPrograms(NiGPUProgram* pkVertexShader,
        NiGPUProgram* pkGeometryShader, NiGPUProgram* pkPixelShader);

    bool SetupTransformPipeline(Context& kContext,
        NiMaterialResource*& pkVertOutProjPos,
        NiWaterMaterialVertexDescriptor* pkVertDesc, 
        NiMaterialResource*& pkWorldPos, 
        NiMaterialResource*& pkWorldView,
        NiMaterialResource*& pkViewDistance);

    bool SetupTexture(Context& kContext, 
        NiWaterMaterialVertexDescriptor* pkVertexDesc, 
        NiMaterialResource* pkWorldPos);

    bool HandlePositionFragment(Context& kContext, 
        NiMaterialResource*& pkVertWorldPos,
        NiMaterialResource*& pkWorldMatrix);

    bool HandleNormalFragment(Context& kContext, 
        bool bHasTangent, NiMaterialResource* pkWorldMatrix);

    bool HandleViewProjectionFragment(Context& kContext, 
        NiMaterialResource* pkVertWorldPos, 
        NiMaterialResource* pkVertOutProjectedPos);

    bool HandleTextureCoordinates(Context& kContext);

    bool HandleProjectiveCoordinates(Context& kContext,
        NiMaterialResource* pkVertexWorldPosition);

    bool SetupTextureSampling(Context& kContext, 
        NiWaterMaterialPixelDescriptor* pkPixelDesc, 
        NiMaterialResource* pkPixelBaseUV, 
        NiMaterialResource* pkPixelNormalUV, 
        NiMaterialResource*& pkPixelBaseMap, 
        NiMaterialResource*& pkPixelNormalMap);

    bool SetupLighting(Context& kContext, 
        NiMaterialResource* pkPixelNormalMap,
        NiMaterialResource* pkPixelWorldNorm,
        NiMaterialResource* pkPixelWorldTangent, 
        NiMaterialResource* pkPixelWorldBinormal,
        NiMaterialResource*& pkPixelCalculatedNormal,
        NiMaterialResource*& pkPixelLightedColor);

    bool SetupReflectionCalculation(Context& kContext, 
        NiWaterMaterialPixelDescriptor* pkPixelDesc, 
        NiMaterialResource* pkPixelCalculatedNormal,
        NiMaterialResource* pkPixelViewVector, 
        NiMaterialResource* pkPixelPlanarTexCoord, 
        NiMaterialResource*& pkPixelSpecularColor,
        NiMaterialResource*& pkPixelPlanarReflectedColor, 
        NiMaterialResource*& pkPixelCubeMapReflectedColor,
        NiMaterialResource*& pkPixelRefractedColor,         
        NiMaterialResource*& pkPixelReflectionVector);

    bool SetupFinalColor(Context& kContext, 
        NiWaterMaterialPixelDescriptor* pkDesc,
        NiMaterialResource* pkPixelOutColor,
        NiMaterialResource* pkPixelBaseMap, 
        NiMaterialResource* pkPixelLightedColor, 
        NiMaterialResource* pkPixelSpecularColor, 
        NiMaterialResource* pkPixelPlanarReflectedColor, 
        NiMaterialResource* pkPixelCubeMapReflectedColor, 
        NiMaterialResource* pkPixelRefractedColor,
        NiMaterialResource* pkPixelWorldPos,
        NiMaterialResource* pkPixelViewDistance, 
        NiMaterialResource* pkPixelViewVector, 
        NiMaterialResource* pkPixelReflectionVector, 
        NiMaterialResource* pkPixelWorldNorm, 
        NiMaterialResource* pkPixelCalculatedNormal);

    bool HandleBaseMap(Context& kContext, 
        NiMaterialResource* pkPixelBaseUV,
        NiMaterialResource*& pkPixelBaseMap);

    bool HandleNormalMap(Context& kContext, 
        NiMaterialResource* pkPixelNormalUV,
        NiMaterialResource*& pkPixelNormalMap);

    bool HandleNormalCalculation(Context& kContext, 
        NiMaterialResource*& pkWorldNormal,
        NiMaterialResource* pkPixelWorldTangent,
        NiMaterialResource* pkPixelWorldBinormal, 
        NiMaterialResource* pkPixelNormalMap);

    bool HandleLighting(Context& kContext, 
        NiMaterialResource* pkWorldNormal, 
        NiMaterialResource*& pkPixelLightedColor);

    void HandleViewVector(Context& kContext,
        NiMaterialResource* pkWorldPos,
        NiMaterialResource*& pkViewVector,
        NiMaterialResource*& pkViewDistance);

    bool HandlePlanarReflection(Context& kContext, 
        NiMaterialResource* pkPixelCalculatedNormal, 
        NiMaterialResource* pkPixelPlanarTexCoord, 
        NiMaterialResource*& pkPixelPlanarReflectedColor);

    bool HandleRefraction(Context& kContext, 
        NiMaterialResource* pkPixelCalculatedNormal, 
        NiMaterialResource* pkPixelPlanarTexCoord, 
        NiMaterialResource*& pkPixelRefractedColor);

    bool HandleReflectionDirectionCalculation(Context& kContext,
        NiMaterialResource* pkPixelCalculatedNormal,
        NiMaterialResource* pkPixelViewVector,
        NiMaterialResource*& pkReflectionDirection);  

    bool HandleCubeMapReflection(Context& kContext, 
        NiMaterialResource* pkReflectionDirection, 
        NiMaterialResource*& pkPixelCubeMapReflectedColor);

    bool HandleSpecularCalculation(Context& kContext, 
        NiMaterialResource* pkReflectionDirection,
        NiMaterialResource* pkPixelViewVector, 
        NiMaterialResource*& pkPixelSpecularColor);

    bool HandleAccumulateColors(Context& kContext, 
        NiMaterialResource*& pkTempOutput, 
        NiMaterialResource* pkPixelLightedColor);

    bool HandleModulateColors(Context& kContext, 
        NiMaterialResource*& pkTempOutput, 
        NiMaterialResource* pkPixelLightedColor);

    bool HandleLerpColors(Context& kContext, 
        NiMaterialResource*& pkTempOutput, 
        NiMaterialResource* pkColor,
        NiMaterialResource* pkLerpValue);

    bool HandleRetrieveColorAlpha(Context& kContext, 
        NiMaterialResource* pkInputColor,
        NiMaterialResource*& pkDiffuseColor, 
        NiMaterialResource*& pkDepth);

    bool HandleCalculateFogFactor(Context& kContext, 
        NiMaterialResource*& pkFogFactor, 
        NiMaterialResource* pkPixelViewDistance,
        NiMaterialResource* pkPixelViewVector, 
        NiMaterialResource* pkPixelWorldNorm, 
        NiMaterialResource* pkDepth);

    bool HandleCalculateFresnelFactor(Context& kContext, 
        NiMaterialResource*& pkFresnelFactor, 
        NiMaterialResource* pkPixelReflectionVector, 
        NiMaterialResource* pkPixelCalculatedNormal);

    bool HandleLerpScalar(Context& kContext, 
        NiMaterialResource* pkScalar1,
        NiMaterialResource* pkScalar2,
        NiMaterialResource* pkLerpValue,
        NiMaterialResource*& pkResult);

    bool HandleMultiplyScalar(Context& kContext, 
        NiMaterialResource* pkScalar1,
        NiMaterialResource* pkScalar2,
        NiMaterialResource*& pkResult);

    void HandleCalculateFog(Context& kContext, 
        NiMaterialResource* pkViewPos, NiMaterialResource*& pkFogValue, 
        Fog eFogType);

    bool HandleComposeColorAndAlpha(Context& kContext,
        NiMaterialResource* pkColor, 
        NiMaterialResource* pkAlpha,
        NiMaterialResource*& pkOutColor);

    bool HandleCalculateAndApplyFog(Context& kContext, 
        NiWaterMaterialPixelDescriptor* pkPixelDesc,
        NiMaterialResource* pkViewPos,
        NiMaterialResource* pkUnfoggedColor, 
        NiMaterialResource*& pkFogOutput);

    virtual bool HandleFinalVertexOutputs(Context &kContext, 
        NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldView,
        NiMaterialResource* pkViewDistance);

private:

    NiFixedString GenerateSamplerName(const NiFixedString& baseName, 
        NiUInt32 uiIndex);
    
    NiFixedString m_kMaterialDescriptorName;
    NiFixedString m_kVertexShaderDescriptorName;
    NiFixedString m_kPixelShaderDescriptorName;   

};

NiSmartPointer(NiWaterMaterial);

#endif
