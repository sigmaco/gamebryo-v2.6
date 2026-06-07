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

#include "NiTerrainPCH.h"

#include "NiWaterMaterial.h"
#include "NiWaterMaterialDescriptor.h"
#include "NiWaterMaterialPixelDescriptor.h"
#include "NiWaterMaterialVertexDescriptor.h"
#include "NiWaterMaterialNodeLibrary.h"

#include <NiCommonSemantics.h>
#include <NiFogProperty.h>
#include <NiRenderer.h>
#include <NiRenderObject.h>
#include <NiRenderObjectMaterialOption.h>
#include <NiShaderFactory.h>
#include <NiIntegerExtraData.h>

NiImplementRTTI(NiWaterMaterial, NiFragmentMaterial);

const char* NiWaterMaterial::REFLECTION_SHADER_CONSTANT = 
    "Reflection";

const char* NiWaterMaterial::SPECULARITY_SHADER_CONSTANT =
    "Specularity";

const char* NiWaterMaterial::DISTURBANCE_SHADER_CONSTANT = 
    "Disturbance";

const char* NiWaterMaterial::FOGDENSITY_SHADER_CONSTANT = 
    "FogDensity";

const char* NiWaterMaterial::FOGDISTANCE_SHADER_CONSTANT = 
    "FogDistance";

const char* NiWaterMaterial::FOGFALLOFF_SHADER_CONSTANT = 
    "FogFallOff";

const char* NiWaterMaterial::NORMALMAPOFFSET_SHADER_CONSTANT = 
    "NormalMapModifier";


//---------------------------------------------------------------------------
NiWaterMaterial::NiWaterMaterial(NiMaterialNodeLibrary* pkMaterialNodeLib,
    bool bAutoCreateCaches, const NiFixedString &name) : NiFragmentMaterial(
    pkMaterialNodeLib, 
    name, 
    VERTEX_VERSION, GEOMETRY_VERSION, PIXEL_VERSION, 
    bAutoCreateCaches),
    m_kMaterialDescriptorName("NiWaterMaterialDescriptor"),
    m_kVertexShaderDescriptorName("NiWaterMaterialVertexDescriptor"),
    m_kPixelShaderDescriptorName("NiWaterMaterialPixelDescriptor")
{   
}
NiWaterMaterial::~NiWaterMaterial()
{
}
//---------------------------------------------------------------------------
NiWaterMaterial* NiWaterMaterial::Create()
{
    // Get the material if it exist already
    NiWaterMaterial* pkMaterial = NiDynamicCast(NiWaterMaterial, 
        NiMaterial::GetMaterial("NiWaterMaterial"));

    if (!pkMaterial)
    {
        // Create a new material if we didn't find it
        NiMaterialNodeLibrary* pkWaterNodeLib = 
            NiWaterMaterialNodeLibrary::CreateMaterialNodeLibrary();

        pkMaterial = NiNew NiWaterMaterial(pkWaterNodeLib);
        pkMaterial->AddDefaultFallbacks();
    }

    return pkMaterial;
}
//---------------------------------------------------------------------------
NiShader* NiWaterMaterial::CreateShader(NiMaterialDescriptor* pkDesc)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer == NULL)
        return false;

    return pkRenderer->GetFragmentShader(pkDesc);
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::SetupPackingRequirements(NiShader* pkShader,
    NiMaterialDescriptor* pkMaterialDescriptor,
    RenderPassDescriptor*, NiUInt32)
{
    NiWaterMaterialDescriptor* pkMaterialDesc = 
        (NiWaterMaterialDescriptor*) pkMaterialDescriptor;

    NiUInt32 uiStreamCount = 1;    
    NiShaderDeclarationPtr spShaderDecl = 
        NiShaderDeclaration::Create(15, uiStreamCount);

    if (!spShaderDecl)
    {
        NIASSERT(!"Invalid shader declaration");
        return false;
    }
    
    NiUInt32 uiEntryCount = 0;
    
    // Handle position and normal streams.
    spShaderDecl->SetEntry(uiEntryCount++, 
        NiShaderDeclaration::SHADERPARAM_NI_POSITION0,
        NiShaderDeclaration::SPTYPE_FLOAT3);

    spShaderDecl->SetEntry(uiEntryCount++,
        NiShaderDeclaration::SHADERPARAM_NI_NORMAL,
        NiShaderDeclaration::SPTYPE_FLOAT3);

    // Check for per-vertex tangents and add a stream if they are present.
    if (pkMaterialDesc->GetTANGENTS())
    {
        spShaderDecl->SetEntry(uiEntryCount++, 
            NiShaderDeclaration::SHADERPARAM_NI_TANGENT,
            NiShaderDeclaration::SPTYPE_FLOAT3);
    }

    // Add streams for each texture coordinate set that the material 
    // requires.
    NiUInt32 uiTexCoordSetCount = pkMaterialDesc->GetNUM_TEXCOORDS();
    for (NiUInt32 ui = 0; ui < uiTexCoordSetCount; ui++)
    {
        spShaderDecl->SetEntry(uiEntryCount++, 
            (NiShaderDeclaration::ShaderParameter)
            (NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0 + ui),
            NiShaderDeclaration::SPTYPE_FLOAT2);
    }

    pkShader->SetSemanticAdapterTableFromShaderDeclaration(spShaderDecl);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::GenerateDescriptor(const NiRenderObject* pkGeometry, 
    const NiPropertyState* pkPropState, 
    const NiDynamicEffectState* pkEffects,
    NiMaterialDescriptor& kMaterialDesc)
{
    if (!pkPropState)
    {
        NIASSERT(!"Could not find property state! Try calling"
            " UpdateProperties.\n");
     
        return false;
    }

    NiWaterMaterialDescriptor* pkDesc = (NiWaterMaterialDescriptor*)
        &kMaterialDesc;
    
    pkDesc->m_kIdentifier = m_kMaterialDescriptorName;

    // Make sure the water material is being applied to the proper geometry.
    if (pkGeometry->RequiresMaterialOption(
        NiRenderObjectMaterialOption::TRANSFORM_SKINNED()))
    {
        NIASSERT(!"Cannot apply water material to skinned geometry.\n");
        return false;
    }

    if (pkGeometry->RequiresMaterialOption(
        NiRenderObjectMaterialOption::TRANSFORM_INSTANCED()))
    {
        NIASSERT(!"Cannot apply water material to instanced geometry.\n");
        return false;
    }

    if (pkGeometry->RequiresMaterialOption(
        NiRenderObjectMaterialOption::MORPHING()))
    {
        NIASSERT(!"Cannot apply water material to morphed geometry.\n");
        return false;
    }
        
    // water geometry must always have at least per-vertex normals and one 
    // set of texture coordinates.
    bool bHasNormals = pkGeometry->ContainsData(NiCommonSemantics::NORMAL());
    if (!bHasNormals)
    {
        NIASSERT(!"water geometry must have per vertex normals.");
        return false;
    }

    // Count up the number of texture coordinate sets on the geometry.
    NiUInt32 uiNumTexCoordSets = pkGeometry->GetSemanticCount(
        NiCommonSemantics::TEXCOORD());
    if (uiNumTexCoordSets == 0)
    {
        NIASSERT(!"Water geometry must have at least one set of texture "
            "coordinates.");
        return false;
    }
    else
    {
        pkDesc->SetNUM_TEXCOORDS(uiNumTexCoordSets);
    }

    // Water geometry may or may not have tangents pre-computed per vertex.
    // If they exist we will use them, else we will not use tangents and 
    // bi-normals 
    bool bHasTangents = false;
    for (NiUInt32 uiSemanticIndex = 0; uiSemanticIndex < 8; uiSemanticIndex++)
    {
        if (pkGeometry->ContainsData(NiCommonSemantics::TANGENT(), 
            uiSemanticIndex))
        {
            bHasTangents = true;
            break;
        }
    }
    pkDesc->SetTANGENTS(bHasTangents); 

    // Handle property states...
    if (pkPropState)
    {
        // Handle texture properties.        
        NiTexturingProperty* pkTexProp = pkPropState->GetTexturing();
        if (pkTexProp)
        {  
            // Determine which maps exist  
            NiTexturingProperty::Map* pkBaseMap = NULL;
            NiTexturingProperty::Map* pkNormalMap = NULL;
            NiTexturingProperty::ShaderMap* pkReflectionMap = NULL;
            NiTexturingProperty::ShaderMap* pkRefractionMap = NULL;
            
            pkBaseMap = pkTexProp->GetBaseMap();
            pkNormalMap = pkTexProp->GetNormalMap();
            pkReflectionMap = pkTexProp->GetShaderMap(0);
            pkRefractionMap = pkTexProp->GetShaderMap(1);
            
            // Sets the appropriate descriptor bits
            pkDesc->SetBASE_MAP(pkBaseMap && pkBaseMap->GetTexture());

            pkDesc->SetNORMAL_MAP(pkNormalMap && pkNormalMap->GetTexture());

            pkDesc->SetREFLECTION_MAP(pkReflectionMap && 
                pkReflectionMap->GetTexture());

            pkDesc->SetREFRACTION_MAP(pkRefractionMap && 
                pkRefractionMap->GetTexture());
        }

        // Handle fog support.
        NiFogProperty* pkFogProp = pkPropState->GetFog();
        if (pkFogProp && pkFogProp->GetFog())
        {
            switch (pkFogProp->GetFogFunction())
            {
            case NiFogProperty::FOG_Z_LINEAR:
                pkDesc->SetFOGTYPE(FOG_LINEAR);
                break;

            case NiFogProperty::FOG_RANGE_SQ:
                pkDesc->SetFOGTYPE(FOG_SQUARED);
                break;

            default:
                pkDesc->SetFOGTYPE(FOG_NONE);
            }
        } 
    }
    
    NiIntegerExtraData* pkExtraMode = (NiIntegerExtraData*)
        pkGeometry->GetExtraData("Mode");
    bool bUsePlanar = false;

    if (pkExtraMode)
    {
        bool bUseCube = pkDesc->GetREFLECTION_MAP() && 
            (pkExtraMode->GetValue() == REFLECTION_MODE_CUBE_MAP);
        pkDesc->SetUSE_CUBE_MAP_REFLECTION(bUseCube);

        bUsePlanar = pkDesc->GetREFLECTION_MAP() && (
            pkExtraMode->GetValue() == REFLECTION_MODE_PLANAR_PROJECTION);
    }

    if (pkEffects)
    {
        pkDesc->SetENABLE_SUNLIGHT(0);

        // Grab the first directional light if it exists and treat it as the 
        // sunlight.
        NiDynEffectStateIter kLightIter = pkEffects->GetLightHeadPos();
        while (kLightIter != NULL)
        {
            NiLight* pkLight = pkEffects->GetNextLight(kLightIter);
            if (pkLight && 
                (pkLight->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                 pkLight->GetEffectType() == NiDynamicEffect::SHADOWDIR_LIGHT))
            {
                pkDesc->SetENABLE_SUNLIGHT(1);
                break;
            }
        }

        // Get texture effect for planar reflection
        NiTextureEffect* pkTextureEffect = pkEffects->GetEnvironmentMap();
        
        // If the effect doesn't exist, we do not do the planar reflection
        bool bUsePlanarReflection = pkTextureEffect != NULL && bUsePlanar;
        pkDesc->SetUSE_PLANAR_REFLECTION(bUsePlanarReflection);

    }

    return true;
}
//---------------------------------------------------------------------------
NiFragmentMaterial::ReturnCode NiWaterMaterial::GenerateShaderDescArray(
    NiMaterialDescriptor* pkMaterialDescriptor,
    RenderPassDescriptor* pkRenderPasses,
    NiUInt32 uiMaxCount,
    NiUInt32& uiCountAdded)
{
    NI_UNUSED_ARG(uiMaxCount);
    NIASSERT(uiMaxCount != 0);
    uiCountAdded = 0;

    if (pkMaterialDescriptor->m_kIdentifier != m_kMaterialDescriptorName)
        return RC_INVALID_MATERIAL;

    // Setup the first pass.
    pkRenderPasses[0].m_bUsesNiRenderState = true;
    pkRenderPasses[0].m_bResetObjectOffsets = true;

    NiWaterMaterialDescriptor* pkMaterialDesc = (NiWaterMaterialDescriptor*)
        pkMaterialDescriptor;

    NiWaterMaterialVertexDescriptor* pkVertexDesc =
        (NiWaterMaterialVertexDescriptor*)&pkRenderPasses[0].m_kVertexDesc;
    pkVertexDesc->m_kIdentifier = m_kVertexShaderDescriptorName;

    NiWaterMaterialPixelDescriptor* pkPixelDesc = 
        (NiWaterMaterialPixelDescriptor*)&pkRenderPasses[0].m_kPixelDesc;
    pkPixelDesc->m_kIdentifier = m_kPixelShaderDescriptorName;

    // Set the vertex descriptor
    pkVertexDesc->SetTANGENTS(pkMaterialDesc->GetTANGENTS());
    pkVertexDesc->SetNUM_TEXCOORDS(pkMaterialDesc->GetNUM_TEXCOORDS());
    pkVertexDesc->SetUSE_CUBE_MAP_REFLECTION(
        pkMaterialDesc->GetUSE_CUBE_MAP_REFLECTION());
    pkVertexDesc->SetUSE_PLANAR_REFLECTION(
        pkMaterialDesc->GetUSE_PLANAR_REFLECTION());

    // Set pixel descriptor
    pkPixelDesc->SetENABLE_SUNLIGHT(pkMaterialDesc->GetENABLE_SUNLIGHT());
    pkPixelDesc->SetBASE_MAP(pkMaterialDesc->GetBASE_MAP());
    pkPixelDesc->SetNORMAL_MAP(pkMaterialDesc->GetNORMAL_MAP());
    pkPixelDesc->SetNUM_TEXCOORDS(pkMaterialDesc->GetNUM_TEXCOORDS());
    pkPixelDesc->SetREFLECTION_MAP(pkMaterialDesc->GetREFLECTION_MAP());
    pkPixelDesc->SetREFRACTION_MAP(pkMaterialDesc->GetREFRACTION_MAP());
    pkPixelDesc->SetTANGENTS(pkMaterialDesc->GetTANGENTS());
    pkPixelDesc->SetUSE_CUBE_MAP_REFLECTION(
        pkMaterialDesc->GetUSE_CUBE_MAP_REFLECTION());
    pkPixelDesc->SetUSE_PLANAR_REFLECTION(
        pkMaterialDesc->GetUSE_PLANAR_REFLECTION());

    // First attempt to perform per-pixel fogging in the pixel shader, if
    // the shader model can't support it, fallback to vertex shader fog.
    pkVertexDesc->SetFOGTYPE(FOG_NONE);
    pkPixelDesc->SetFOGTYPE(pkMaterialDesc->GetFOGTYPE());

    uiCountAdded++;
    return NiFragmentMaterial::RC_SUCCESS;
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::GenerateVertexShadeTree(Context& kContext, 
    NiGPUProgramDescriptor* pkDesc)
{
    NIASSERT(pkDesc->m_kIdentifier == "NiWaterMaterialVertexDescriptor");
    NiWaterMaterialVertexDescriptor* pkVertexDesc = 
        (NiWaterMaterialVertexDescriptor*)pkDesc;

    kContext.m_spConfigurator->SetDescription(pkVertexDesc->ToString());

    // Add vertex in, vertex out, constants, and uniforms
    if (!AddDefaultMaterialNodes(kContext, pkDesc, 
        NiGPUProgram::PROGRAM_VERTEX))
    {
        return false;
    }

    // Create the main vertex shader output
    NiMaterialResource* pkVertOutProjPos = 
        kContext.m_spOutputs->AddInputResource("float4", "Position",
        "World", "WorldProj");
    
    // Work out transformation pipeline
    NiMaterialResource* pkWorldPos = NULL;
    NiMaterialResource* pkWorldView = NULL;
    NiMaterialResource* pkViewDistance = NULL;
    SetupTransformPipeline(kContext, pkVertOutProjPos, pkVertexDesc, 
        pkWorldPos, pkWorldView, pkViewDistance);

    // If wer are calculating fog in the vertex shader instead of the
    // pixel shader, add FogOut output and calculate fog.
    if (pkVertexDesc->GetFOGTYPE() != FOG_NONE)
    {
        NiMaterialResource* pkFogOut = kContext.m_spOutputs->AddInputResource(
            "float", "Fog", "", "FogOut");

        NiMaterialResource* pkFogValue = NULL;
        HandleCalculateFog(kContext, pkViewDistance, pkFogValue, 
            static_cast<Fog>(pkVertexDesc->GetFOGTYPE()));

        kContext.m_spConfigurator->AddBinding(pkFogValue, pkFogOut);
    }

    // Work out the texture uv
    SetupTexture(kContext, pkVertexDesc, pkWorldPos);

    // Handles the final vertex outputs
    HandleFinalVertexOutputs(kContext, pkWorldPos, pkWorldView,
        pkViewDistance);

    return true;
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::GenerateGeometryShadeTree(Context&, 
    NiGPUProgramDescriptor*)
{
    // a geometry shader isn't needed as yet
    return true;
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::GeneratePixelShadeTree(Context& kContext,
    NiGPUProgramDescriptor* pkDesc)
{
    NIASSERT(pkDesc->m_kIdentifier == "NiWaterMaterialPixelDescriptor");
    NiWaterMaterialPixelDescriptor* pkPixelDesc = 
        (NiWaterMaterialPixelDescriptor*)pkDesc;

    kContext.m_spConfigurator->SetDescription(pkPixelDesc->ToString());

    // Add vertex in, vertex out, constants, and uniforms
    if (!AddDefaultMaterialNodes(kContext, pkPixelDesc, 
        NiGPUProgram::PROGRAM_PIXEL))
    {
        return false;
    }
      
    // Create the final pixel out color.
    NiMaterialResource* pkPixelOutColor = 
        kContext.m_spOutputs->AddInputResource("float4", "Color", "", 
        "Color0"); 
    
    // Setup pixel shader inputs
    NiMaterialResource* pkPixelWorldNorm = NULL;
    NiMaterialResource* pkPixelViewVector = NULL;
    NiMaterialResource* pkPixelViewDistance = NULL;
    NiMaterialResource* pkPixelWorldBinormal = NULL;
    NiMaterialResource* pkPixelWorldTangent = NULL;
    NiMaterialResource* pkPixelWorldPosition = NULL;
    NiMaterialResource* pkPixelPosProj = NULL;
    NiMaterialResource* pkPixelBaseUV = NULL;
    NiMaterialResource* pkPixelNormalUV = NULL;
    NiMaterialResource* pkPixelCalculatedNormal = NULL;
    NiMaterialResource* pkPixelPlanarTexCoord = NULL;
    
    pkPixelPosProj = kContext.m_spInputs->AddOutputResource("float4", 
        "Position", "World",  "PosProjected");    
    pkPixelWorldPosition = kContext.m_spInputs->AddOutputResource("float4",
        "TexCoord", "World", "WorldPosition");

    pkPixelViewVector = kContext.m_spInputs->AddOutputResource("float3",
        "TexCoord", "World", "WorldView");

    pkPixelViewDistance = kContext.m_spInputs->AddOutputResource("float",
        "TexCoord", "World", "ViewDistance");

    pkPixelWorldNorm = kContext.m_spInputs->AddOutputResource("float3",
        "TexCoord", "World", "WorldNormal");

    if (pkPixelDesc->GetTANGENTS())
    {
        pkPixelWorldTangent = kContext.m_spInputs->AddOutputResource(
            "float3", "TexCoord", "World", "WorldTangent");
        pkPixelWorldBinormal = kContext.m_spInputs->AddOutputResource(
            "float3", "TexCoord", "World", "WorldBinormal");
    }

    if (pkPixelDesc->GetNUM_TEXCOORDS() != 0)
    {
        pkPixelBaseUV = kContext.m_spInputs->AddOutputResource(
            "float2", "TexCoord", "", "BaseUV");
        pkPixelNormalUV = kContext.m_spInputs->AddOutputResource(
            "float2", "TexCoord", "", "NormalUV");
    }

    if (pkPixelDesc->GetUSE_PLANAR_REFLECTION() != 0)
    {
        pkPixelPlanarTexCoord = kContext.m_spInputs->AddOutputResource(
            "float4", "TexCoord", "", "PlanarTex");
    }

    // Get the texture map colors
    NiMaterialResource* pkPixelBaseMap = NULL;
    NiMaterialResource* pkPixelNormalMap = NULL;
    SetupTextureSampling(kContext, pkPixelDesc, pkPixelBaseUV, 
        pkPixelNormalUV, pkPixelBaseMap, pkPixelNormalMap);
    
    // Get the lighting color
    NiMaterialResource* pkPixelLightedColor = NULL;
    if (pkPixelDesc->GetENABLE_SUNLIGHT() != 0)
    {
        SetupLighting(kContext, pkPixelNormalMap, pkPixelWorldNorm,
            pkPixelWorldTangent, pkPixelWorldBinormal, 
            pkPixelCalculatedNormal, pkPixelLightedColor);
    }

    // Get the reflection, refraction and specular color
    NiMaterialResource* pkPixelSpecularColor = NULL;
    NiMaterialResource* pkPixelCubeMapReflectedColor = NULL;
    NiMaterialResource* pkPixelPlanarReflectedColor = NULL;
    NiMaterialResource* pkPixelRefractedColor = NULL;
    NiMaterialResource* pkPixelReflectionVector = NULL;    
    SetupReflectionCalculation(kContext, pkPixelDesc, pkPixelCalculatedNormal,
        pkPixelViewVector, pkPixelPlanarTexCoord, 
        pkPixelSpecularColor, pkPixelPlanarReflectedColor, 
        pkPixelCubeMapReflectedColor, pkPixelRefractedColor, 
        pkPixelReflectionVector);

    // Compute the final color
    SetupFinalColor(kContext, pkPixelDesc, pkPixelOutColor, pkPixelBaseMap, 
        pkPixelLightedColor, pkPixelSpecularColor, pkPixelPlanarReflectedColor,
        pkPixelCubeMapReflectedColor, pkPixelRefractedColor,
        pkPixelWorldPosition, pkPixelViewDistance, pkPixelViewVector, 
        pkPixelReflectionVector, pkPixelWorldNorm, pkPixelCalculatedNormal);
    
    
    return true;
}
//---------------------------------------------------------------------------
NiUInt32 NiWaterMaterial::VerifyShaderPrograms(NiGPUProgram* pkVertexShader,
    NiGPUProgram*, NiGPUProgram* pkPixelShader)
{
    NiUInt32 uiReturnCode = RC_SUCCESS;
    if (pkVertexShader == NULL)
        uiReturnCode |= RC_COMPILE_FAILURE_VERTEX;
    if (pkPixelShader == NULL)
        uiReturnCode |= RC_COMPILE_FAILURE_PIXEL;
    // No need to check geometry shader (2nd argument).

    return uiReturnCode;
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::SetupTransformPipeline(Context& kContext,
    NiMaterialResource*& pkVertOutProjPos,
    NiWaterMaterialVertexDescriptor* pkVertDesc, 
    NiMaterialResource*& pkWorldPos, NiMaterialResource*& pkWorldView,
    NiMaterialResource*& pkViewDistance)
{
    pkWorldPos = NULL;
    pkWorldView = NULL;

    NiMaterialResource* pkWorldMatrix = NULL;

    // Get the world position
    if (!HandlePositionFragment(kContext, pkWorldPos, pkWorldMatrix))
    {
        return false;
    }
   
    // Calculate WorldView and ViewDistance in vertex shader
    HandleViewVector(kContext, pkWorldPos, pkWorldView, pkViewDistance);

    // Get the world normals
    bool bTangent = pkVertDesc->GetTANGENTS() != 0;
    if (!HandleNormalFragment(kContext, bTangent, pkWorldMatrix))
    {
        return false;
    }

    // Get the projected position
    if (!HandleViewProjectionFragment(kContext, pkWorldPos, pkVertOutProjPos))
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::SetupTexture(Context& kContext, 
    NiWaterMaterialVertexDescriptor* pkVertexDesc, 
    NiMaterialResource* pkWorldPos)
{
    // The water material only handles one texture stream that holds both 
    // normals uvs and base map uvs
    if (pkVertexDesc->GetNUM_TEXCOORDS() == 1)
    {
        HandleTextureCoordinates(kContext);        
    }

    if (pkVertexDesc->GetUSE_PLANAR_REFLECTION() || 
        pkVertexDesc->GetREFRACTION_MAP())
    {
        HandleProjectiveCoordinates(kContext, pkWorldPos);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiWaterMaterial::HandleCalculateFog(Context& kContext, 
    NiMaterialResource* pkViewDistance, NiMaterialResource*& pkFogValue, 
    Fog eFogType)
{
    if (eFogType == FOG_NONE)
        return;

    NIASSERT(pkViewDistance);

    // Bind fog resources
    NiMaterialNode* pkFogNode = GetAttachableNodeFromLibrary(
        "CalculateFog");
    kContext.m_spConfigurator->AddNode(pkFogNode);

    NiUInt32 uiFogType = 0; 
    bool bFogRange = false;
    if (eFogType == FOG_LINEAR)
    {
        // NiFogProperty::FOG_Z_LINEAR
        uiFogType = 3; // 3 == linear
        bFogRange = false;
    }
    else if (eFogType == FOG_SQUARED)
    {
        // NiFogProperty::FOG_RANGE_SQ
        uiFogType = 3; // 3 == linear
        bFogRange = true;
    }

    char acValue[32];
    NiSprintf(acValue, 32, "(%d)", uiFogType);
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("int", acValue),
        pkFogNode->GetInputResourceByVariableName("FogType"));

    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("bool", 
        bFogRange ? "(true)" : "(false)"),
        pkFogNode->GetInputResourceByVariableName("FogRange"));

    NiMaterialResource* pkFogDensity = AddOutputPredefined(
        kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_FOG_DENSITY); 
    kContext.m_spConfigurator->AddBinding(pkFogDensity, 
        pkFogNode->GetInputResourceByVariableName("FogDensity"));

    NiMaterialResource* pkFogNearFar = AddOutputPredefined(
        kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_FOG_NEARFAR); 
    kContext.m_spConfigurator->AddBinding(pkFogNearFar, 
        pkFogNode->GetInputResourceByVariableName("FogStartEnd"));

    kContext.m_spConfigurator->AddBinding(pkViewDistance, 
        pkFogNode->GetInputResourceByVariableName("ViewDistance"));

    pkFogValue = pkFogNode->GetOutputResourceByVariableName("FogOut");
    NIASSERT(pkFogValue);
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::HandlePositionFragment(Context& kContext, 
    NiMaterialResource*& pkVertWorldPos,
    NiMaterialResource*& pkWorldMatrix)
{
    pkVertWorldPos = NULL;
    pkWorldMatrix = NULL;
    
    NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
        "TransformPositionToWorld");
    kContext.m_spConfigurator->AddNode(pkProjFrag);

    // Add Per-Vertex Elements
    kContext.m_spInputs->AddOutputResource("float3", "Position", "Local", 
        "Position", 1);

    // Add constant map elements
    pkWorldMatrix = AddOutputPredefined(kContext.m_spUniforms,
        NiShaderConstantMap::SCM_DEF_WORLD, 4);

    // Add output
    NiMaterialResource* pkVertOutWorldPosition = 
        kContext.m_spOutputs->AddInputResource("float4", "TexCoord", 
        "World", "WorldPosition");
    
    // Bind projection
    kContext.m_spConfigurator->AddBinding("Position", kContext.m_spInputs, 
        "Position", pkProjFrag);

    kContext.m_spConfigurator->AddBinding(pkWorldMatrix,
        "World", pkProjFrag);

    kContext.m_spConfigurator->AddBinding("WorldPos", pkProjFrag,
            pkVertOutWorldPosition);
    pkVertOutWorldPosition = pkProjFrag->GetOutputResourceByVariableName(
        "WorldPos");

    pkVertWorldPos = pkVertOutWorldPosition;


    return true;
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::HandleNormalFragment(Context& kContext, 
    bool bHasTangent, NiMaterialResource* pkWorldMatrix)
{
    // Handle the normals
    if (bHasTangent)
    {
        // Add required inputs
        NiMaterialResource* pkNorm = 
            kContext.m_spInputs->AddOutputResource("float3", "Normal",
            "Local", "Normal");
        NiMaterialResource* pkTangent = 
            kContext.m_spInputs->AddOutputResource("float3", "Tangent",
            "Local", "Tangent");
        
        // Add required outputs
        NiMaterialResource* pkVertOutWorldNormal = 
            kContext.m_spOutputs->AddInputResource("float3", "TexCoord", 
            "World", "WorldNormal");

        NiMaterialResource* pkVertOutWorldTangent = 
            kContext.m_spOutputs->AddInputResource("float3", "TexCoord", 
            "World", "WorldTangent");

        NiMaterialResource* pkVertOutWorldBinormal = 
            kContext.m_spOutputs->AddInputResource("float3", "TexCoord", 
            "World", "WorldBinormal");
        
        // Transform the tangent and normal
        NiMaterialNode* pkNormFrag = 
            GetAttachableNodeFromLibrary("TransformNT");
        kContext.m_spConfigurator->AddNode(pkNormFrag);
        kContext.m_spConfigurator->AddBinding(pkNorm, 
            pkNormFrag->GetInputResourceByVariableName("Normal"));
        kContext.m_spConfigurator->AddBinding(pkTangent, 
            pkNormFrag->GetInputResourceByVariableName("Tangent"));
        kContext.m_spConfigurator->AddBinding(pkWorldMatrix, 
            pkNormFrag->GetInputResourceByVariableName("World"));

        kContext.m_spConfigurator->AddBinding("WorldNrm", pkNormFrag,
            pkVertOutWorldNormal);
        kContext.m_spConfigurator->AddBinding("WorldTangent", pkNormFrag,
            pkVertOutWorldTangent);

        pkVertOutWorldNormal = pkNormFrag->GetOutputResourceByVariableName(
            "WorldNrm");
        pkVertOutWorldTangent = pkNormFrag->GetOutputResourceByVariableName(
            "WorldTangent");
 
        // Add the compute binormals fragment
        NiMaterialNode* pkBiNormFrag = 
            GetAttachableNodeFromLibrary("CalculateBinormal");
        kContext.m_spConfigurator->AddNode(pkBiNormFrag);
        kContext.m_spConfigurator->AddBinding(pkVertOutWorldNormal, 
            pkBiNormFrag->GetInputResourceByVariableName("WorldNormal"));
        kContext.m_spConfigurator->AddBinding(pkVertOutWorldTangent, 
            pkBiNormFrag->GetInputResourceByVariableName("WorldTangent"));
                
        kContext.m_spConfigurator->AddBinding("WorldBinormal", pkBiNormFrag,
            pkVertOutWorldBinormal);
        pkVertOutWorldBinormal = pkBiNormFrag->GetOutputResourceByVariableName(
            "WorldBinormal");          
        
        return true;
    }

    // Add required input
    NiMaterialResource* pkLocalNorm = 
        kContext.m_spInputs->AddOutputResource("float3", "Normal",
        "Local", "Normal");
    
    // Add required output
    NiMaterialResource* pkVertOutWorldNormal = 
        kContext.m_spOutputs->AddInputResource("float3", "TexCoord", 
        "World", "WorldNormal");

    NiMaterialNode* pkNormFrag = 
        GetAttachableNodeFromLibrary("TransformNormal");
    kContext.m_spConfigurator->AddNode(pkNormFrag);
    kContext.m_spConfigurator->AddBinding(pkLocalNorm, 
        pkNormFrag->GetInputResourceByVariableName("Normal"));
    kContext.m_spConfigurator->AddBinding(pkWorldMatrix, 
        pkNormFrag->GetInputResourceByVariableName("World"));
    
    kContext.m_spConfigurator->AddBinding("WorldNrm", pkNormFrag,
        pkVertOutWorldNormal);
    pkVertOutWorldNormal = pkNormFrag->GetOutputResourceByVariableName(
        "WorldNrm");

    return true;
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::HandleViewProjectionFragment(Context& kContext, 
    NiMaterialResource* pkVertWorldPos, 
    NiMaterialResource* pkVertOutProjectedPos)
{
    // create the fragment node    
    NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
        "ProjectPositionWorldToProj");
    if (!pkProjFrag)
    {
        NIASSERT(!"Error world to proj in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkProjFrag);

    // Add the constant matrix
    NiMaterialResource* pkViewProjMatrix = AddOutputPredefined(
        kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_VIEWPROJ, 4);

    // Bind the inputs and outputs
    kContext.m_spConfigurator->AddBinding(pkViewProjMatrix, 
        pkProjFrag->GetInputResourceByVariableName("ViewProjMatrix"));
    kContext.m_spConfigurator->AddBinding(pkVertWorldPos, 
        pkProjFrag->GetInputResourceByVariableName("WorldPosition"));  

    kContext.m_spConfigurator->AddBinding(
        pkProjFrag->GetOutputResourceByVariableName("WorldProj"),
        pkVertOutProjectedPos);

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleTextureCoordinates(Context& kContext)
{
    NiMaterialNode* pkTextureFrag = GetAttachableNodeFromLibrary(
        "SplitTextureCoordinates");

    if (!pkTextureFrag)
    {
        NIASSERT(!"Error in texture fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkTextureFrag);

    // Add constant map elements
    NiMaterialResource* pkNormalTransform = 
        AddOutputPredefined(kContext.m_spUniforms,
        NiShaderConstantMap::SCM_DEF_TEXTRANSFORMNORMAL, 4);

    // Add normal map modifier constant
    NiMaterialResource* pkNormalCoordMod = AddOutputAttribute(
        kContext.m_spUniforms, NORMALMAPOFFSET_SHADER_CONSTANT,
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT2);

    // Add required inputs
    NiMaterialResource* pkTexcoord = 
        kContext.m_spInputs->AddOutputResource("float2", "TexCoord",
        "", "TexCoord");
    
    // Add required outputs
    NiMaterialResource* pkVertOutBaseUV = 
        kContext.m_spOutputs->AddInputResource("float2", "TexCoord", 
        "World", "BaseUV");
    NiMaterialResource* pkVertOutNormalUV = 
        kContext.m_spOutputs->AddInputResource("float2", "TexCoord", 
        "World", "NormalUV");

    // create binding
    kContext.m_spConfigurator->AddBinding(pkTexcoord,
        pkTextureFrag->GetInputResourceByVariableName("TexCoord"));
    kContext.m_spConfigurator->AddBinding(pkNormalTransform, 
        pkTextureFrag->GetInputResourceByVariableName("TexTransform"));
    kContext.m_spConfigurator->AddBinding(pkNormalCoordMod, 
        pkTextureFrag->GetInputResourceByVariableName("NormalModifier"));
    
    kContext.m_spConfigurator->AddBinding("BaseUV", pkTextureFrag,
        pkVertOutBaseUV);
    pkVertOutBaseUV = pkTextureFrag->GetOutputResourceByVariableName(
        "BaseUV");
    kContext.m_spConfigurator->AddBinding("NormalUV", pkTextureFrag,
        pkVertOutNormalUV);
    pkVertOutNormalUV = pkTextureFrag->GetOutputResourceByVariableName(
        "NormalUV");


    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleProjectiveCoordinates(Context& kContext, 
    NiMaterialResource* pkVertexWorldPosition)
{
    NiMaterialNode* pkTextureFrag = GetAttachableNodeFromLibrary(
        "ComputeProjectiveCoordinates");

    if (!pkTextureFrag)
    {
        NIASSERT(!"Error in texture fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkTextureFrag);

    // Add required outputs
    NiMaterialResource* pkVertOutPlanarTexture = 
        kContext.m_spOutputs->AddInputResource("float4", "TexCoord", 
        "", "PlanarTex");

    // Add constant map elements
    NiMaterialResource* pkProjectiveMatrix = 
        AddOutputObject(kContext.m_spUniforms, 
        NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSFORM, 
        NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP, 0);

    // create binding
    kContext.m_spConfigurator->AddBinding(pkVertexWorldPosition,
        pkTextureFrag->GetInputResourceByVariableName("WorldPos"));
    kContext.m_spConfigurator->AddBinding(pkProjectiveMatrix, 
        pkTextureFrag->GetInputResourceByVariableName("ProjectiveMatrix"));

    kContext.m_spConfigurator->AddBinding("PlanarTex", pkTextureFrag,
        pkVertOutPlanarTexture);
    pkVertOutPlanarTexture = pkTextureFrag->GetOutputResourceByVariableName(
        "PlanarTex");

    return true;
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::SetupTextureSampling(Context& kContext, 
    NiWaterMaterialPixelDescriptor* pkPixelDesc, 
    NiMaterialResource* pkPixelBaseUV, 
    NiMaterialResource* pkPixelNormalUV, 
    NiMaterialResource*& pkPixelBaseMap, 
    NiMaterialResource*& pkPixelNormalMap)
{
    // Get the base texture map color this will be used as the water diffuse
    // color
    if (pkPixelDesc->GetBASE_MAP())
    {
        HandleBaseMap(kContext, pkPixelBaseUV, pkPixelBaseMap);
    }
    else
    {
        // Light blue color if no texture is available (e.g. when viewed
        // in Scene Designer in "No Texture" rendering mode).
        pkPixelBaseMap = kContext.m_spStatics->AddOutputConstant("float4", 
            "(0.31372,0.94117,1.0,0.75)");
    }

    if (pkPixelDesc->GetNORMAL_MAP())
    {
        // If we have a normal map get the RGB value 
        HandleNormalMap(kContext, pkPixelNormalUV, pkPixelNormalMap);
    }

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::SetupLighting(Context& kContext, 
    NiMaterialResource* pkPixelNormalMap,
    NiMaterialResource* pkPixelWorldNorm,
    NiMaterialResource* pkPixelWorldTangent, 
    NiMaterialResource* pkPixelWorldBinormal, 
    NiMaterialResource*& pkPixelCalculatedNormal,
    NiMaterialResource*& pkPixelLightedColor)
{
    pkPixelCalculatedNormal = pkPixelWorldNorm;

    if (pkPixelNormalMap && pkPixelWorldTangent)
    {
        // Only compute the normal if we have tangent and normal map
        HandleNormalCalculation(kContext, pkPixelCalculatedNormal, 
            pkPixelWorldTangent,pkPixelWorldBinormal, pkPixelNormalMap);
    }

    // calculate lighted color
    HandleLighting(kContext, pkPixelCalculatedNormal, pkPixelLightedColor);

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::SetupReflectionCalculation(Context& kContext, 
        NiWaterMaterialPixelDescriptor* pkPixelDesc, 
        NiMaterialResource* pkPixelCalculatedNormal,
        NiMaterialResource* pkPixelViewVector, 
        NiMaterialResource* pkPixelPlanarTexCoord, 
        NiMaterialResource*& pkPixelSpecularColor,
        NiMaterialResource*& pkPixelPlanarReflectedColor, 
        NiMaterialResource*& pkPixelCubeMapReflectedColor,
        NiMaterialResource*& pkPixelRefractedColor,         
        NiMaterialResource*& pkPixelReflectionVector)
{
    // if we do not have normal we will not get this far
    if (!pkPixelCalculatedNormal)
    {
        return false;
    }    
    
    // select the appropriate mode of reflection
    if (pkPixelDesc->GetUSE_PLANAR_REFLECTION())
    {
        HandlePlanarReflection(kContext, pkPixelCalculatedNormal, 
            pkPixelPlanarTexCoord, pkPixelPlanarReflectedColor);
    }
    
    if (pkPixelDesc->GetREFRACTION_MAP())
    {
        HandleRefraction(kContext, pkPixelCalculatedNormal, 
            pkPixelPlanarTexCoord, pkPixelRefractedColor);
    }

    if (pkPixelViewVector)
    {
        // The we need the reflection direction
        HandleReflectionDirectionCalculation(kContext,
            pkPixelCalculatedNormal, pkPixelViewVector,
            pkPixelReflectionVector);    

        if (pkPixelDesc->GetUSE_CUBE_MAP_REFLECTION())
        {
            HandleCubeMapReflection(kContext, pkPixelReflectionVector, 
                pkPixelCubeMapReflectedColor);
        }                

        if (pkPixelDesc->GetENABLE_SUNLIGHT())
        {
            HandleSpecularCalculation(kContext, pkPixelReflectionVector,
                pkPixelViewVector, pkPixelSpecularColor);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::SetupFinalColor(Context& kContext, 
    NiWaterMaterialPixelDescriptor* pkDesc,
    NiMaterialResource* pkPixelOutColor,
    NiMaterialResource* pkPixelBaseMap, 
    NiMaterialResource* pkPixelLightedColor, 
    NiMaterialResource* pkPixelSpecularColor, 
    NiMaterialResource* pkPixelPlanarReflectedColor, 
    NiMaterialResource* pkPixelCubeMapReflectedColor, 
    NiMaterialResource* pkPixelRefractedColor,
    NiMaterialResource* /*pkPixelWorldPos*/,
    NiMaterialResource* pkPixelViewDistance, 
    NiMaterialResource* pkPixelViewVector, 
    NiMaterialResource* pkPixelReflectionVector, 
    NiMaterialResource* pkPixelWorldNorm, 
    NiMaterialResource* pkPixelCalculatedNormal)
{
    NiMaterialResource* pkTempOutput = NULL;
    NiMaterialResource* pkDepth = NULL;
    
    HandleRetrieveColorAlpha(kContext, pkPixelBaseMap, pkTempOutput, pkDepth);

    bool bSpecularAdded = false;
    bool bAddAlpha = true;
    
    if (pkPixelLightedColor)
    {
        HandleModulateColors(kContext, pkTempOutput, pkPixelLightedColor);
    }
    
    // Calculate fog factor 
    NiMaterialResource* pkFogFactor = NULL;
    if (pkPixelViewVector)
    {
        HandleCalculateFogFactor(kContext, pkFogFactor, pkPixelViewDistance,
            pkPixelViewVector, pkPixelWorldNorm, pkDepth);
    }

    if (pkPixelRefractedColor)
    {
        bAddAlpha = false;
               
        HandleLerpColors(kContext, pkPixelRefractedColor, 
          pkTempOutput , pkFogFactor);

        pkTempOutput = pkPixelRefractedColor;
    }
    
    NiMaterialResource* pkFresnelFactor = NULL;
    
    if (pkPixelCubeMapReflectedColor || pkPixelPlanarReflectedColor)
    {
        // Calculate the fresnel factor 
        
        HandleCalculateFresnelFactor(kContext, pkFresnelFactor, 
            pkPixelReflectionVector, pkPixelCalculatedNormal);       

        if (pkPixelCubeMapReflectedColor && pkFresnelFactor)
        {
            // Add the specular sunlight to the reflected value
            if (pkPixelSpecularColor)
            {
                bSpecularAdded = true;
                HandleAccumulateColors(kContext, pkPixelCubeMapReflectedColor, 
                    pkPixelSpecularColor);
            }

            HandleLerpColors(kContext, pkTempOutput, 
              pkPixelCubeMapReflectedColor, pkFresnelFactor);
        }
        else if (pkPixelPlanarReflectedColor && pkFresnelFactor)
        {
            // Add the sunlight to the reflected value
            if (pkPixelSpecularColor)
            {
                bSpecularAdded = true;
                HandleAccumulateColors(kContext, pkPixelPlanarReflectedColor, 
                    pkPixelSpecularColor);
            }

            HandleLerpColors(kContext, pkTempOutput, 
                pkPixelPlanarReflectedColor, pkFresnelFactor);

        } 
    }

    if (!bSpecularAdded && pkPixelSpecularColor)
    {  
        // If the specular color was not taken in account in the reflection
        // section we will do it here
        HandleAccumulateColors(kContext, pkTempOutput, 
                    pkPixelSpecularColor);
    }   

    NiMaterialResource* pkPreFoggedColor = NULL;
    if (bAddAlpha)
    {
        // If the alpha is not being done through refraction, we calculate it 
        // here
        if (pkFogFactor && pkFresnelFactor)
        {
            HandleLerpScalar(kContext,pkFogFactor,
               NULL, pkFresnelFactor, pkDepth);            
        }

        HandleComposeColorAndAlpha(kContext, pkTempOutput, pkDepth,
            pkPreFoggedColor);
    }
    else
    {
        // Alpha value will be defaulted to 1.0
        HandleComposeColorAndAlpha(kContext, pkTempOutput, NULL,
            pkPreFoggedColor);
    }
        

    NiMaterialResource* pkFoggedColor = NULL;
    if (!HandleCalculateAndApplyFog(kContext, pkDesc, pkPixelViewDistance,
        pkPreFoggedColor, pkFoggedColor))
        return false;

    NIASSERT(pkFoggedColor);    
    kContext.m_spConfigurator->AddBinding(pkFoggedColor, pkPixelOutColor);

    return true;
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::HandleBaseMap(Context& kContext, 
        NiMaterialResource* pkPixelBaseUV,
        NiMaterialResource*& pkPixelBaseMap)
{
    NiFixedString kSamplerName = NiTexturingProperty::GetMapNameFromID(
        NiTexturingProperty::BASE_INDEX);

    // Set the base map texture sampler resource
    NiMaterialResource* pkSamplerRes = InsertTextureSampler(kContext,
        kSamplerName, TEXTURE_SAMPLER_2D, 0);

    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("TextureRGBASample");
    
    if (!pkNode)
    {
        NIASSERT(!"Error in base map sampling");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // Create bindings
    kContext.m_spConfigurator->AddBinding(
        pkPixelBaseUV, pkNode->GetInputResourceByVariableName("TexCoord"));
    kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
        pkNode->GetInputResourceByVariableName("Sampler"));
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("bool", "(false)"), 
        "Saturate", pkNode);

    // Output
    pkPixelBaseMap = pkNode->GetOutputResourceByVariableName("ColorOut");
    if (!pkPixelBaseMap)
    {
        NIASSERT(!"Error in material");
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleNormalMap(Context& kContext, 
    NiMaterialResource* pkPixelNormalUV,
    NiMaterialResource*& pkPixelNormalMap)
{
    NiFixedString kSamplerName = NiTexturingProperty::GetMapNameFromID(
            NiTexturingProperty::NORMAL_INDEX);

    // Set the base map texture sampler resource
    NiMaterialResource* pkSamplerRes = InsertTextureSampler(kContext,
        kSamplerName, TEXTURE_SAMPLER_2D, 0);

    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("TextureRGBASample");

    if (!pkNode)
    {
        NIASSERT(!"Error in normal map sampling");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // create bindings
    kContext.m_spConfigurator->AddBinding(
        pkPixelNormalUV, pkNode->GetInputResourceByVariableName("TexCoord"));
    kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
        pkNode->GetInputResourceByVariableName("Sampler"));
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("bool", "(false)"), 
        "Saturate", pkNode);

    // output
    pkPixelNormalMap = pkNode->GetOutputResourceByVariableName("ColorOut");
    if (!pkPixelNormalMap)
    {
        NIASSERT(!"Error in material");
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleNormalCalculation(Context& kContext, 
    NiMaterialResource*& pkWorldNormal,
    NiMaterialResource* pkPixelWorldTangent,
    NiMaterialResource* pkPixelWorldBinormal, 
    NiMaterialResource* pkPixelNormalMap)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("CalculateWorldNormal");

    if (!pkNode)
    {
        NIASSERT(!"Error in normal map calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // create bindings
    kContext.m_spConfigurator->AddBinding(pkWorldNormal, 
        pkNode->GetInputResourceByVariableName("Normal"));
    kContext.m_spConfigurator->AddBinding(pkPixelWorldTangent, 
        pkNode->GetInputResourceByVariableName("Tangent"));
    kContext.m_spConfigurator->AddBinding(pkPixelWorldBinormal,
        pkNode->GetInputResourceByVariableName("Binormal"));
    kContext.m_spConfigurator->AddBinding(pkPixelNormalMap, 
        pkNode->GetInputResourceByVariableName("NormalMap"));

    // output
    pkWorldNormal = pkNode->GetOutputResourceByVariableName("WorldNormalOut");
    
    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleLighting(Context& kContext, 
    NiMaterialResource* pkWorldNormal, 
    NiMaterialResource*& pkPixelLightedColor)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("CalculateDiffuseColor");

    if (!pkNode)
    {
        NIASSERT(!"Error in lighting calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // Add constant map elements
    NiMaterialResource* pkLightDirection = 
        AddOutputObject(kContext.m_spUniforms, 
        NiShaderConstantMap::SCM_OBJ_WORLDDIRECTION, 
        NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT, 0, "Light");
    NiMaterialResource* pkLightDiffuse = 
        AddOutputObject(kContext.m_spUniforms, 
        NiShaderConstantMap::SCM_OBJ_DIFFUSE,
        NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT, 0, "Light");

    // creat bindings
    kContext.m_spConfigurator->AddBinding(pkLightDirection,
        pkNode->GetInputResourceByVariableName("LightDirection"));
    kContext.m_spConfigurator->AddBinding(pkLightDiffuse,
        pkNode->GetInputResourceByVariableName("LightDiffuse"));
    kContext.m_spConfigurator->AddBinding(pkWorldNormal, 
        pkNode->GetInputResourceByVariableName("Normal"));
    
    pkPixelLightedColor = pkNode->GetOutputResourceByVariableName(
        "DiffuseOut");

    return true;
}
//----------------------------------------------------------------------------
void NiWaterMaterial::HandleViewVector(Context& kContext,
    NiMaterialResource* pkWorldPos, NiMaterialResource*& pkViewVector,
    NiMaterialResource*& pkViewDistance)
{
    NIASSERT(pkWorldPos);

    NiMaterialNode* pkCalcViewVectorFrag = GetAttachableNodeFromLibrary(
        "CalculateViewVectorDistance");
    kContext.m_spConfigurator->AddNode(pkCalcViewVectorFrag);
    NIASSERT(pkCalcViewVectorFrag);

    NiMaterialResource* pkCameraPos = AddOutputPredefined(
        kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_EYE_POS);
    NIASSERT(pkCameraPos);

    kContext.m_spConfigurator->AddBinding(pkCameraPos,
        pkCalcViewVectorFrag->GetInputResourceByVariableName("CameraPos"));

    kContext.m_spConfigurator->AddBinding(pkWorldPos,
        pkCalcViewVectorFrag->GetInputResourceByVariableName("WorldPos"));

    pkViewVector = pkCalcViewVectorFrag->GetOutputResourceByVariableName(
        "WorldViewVector");
    NIASSERT(pkViewVector);

    pkViewDistance = pkCalcViewVectorFrag->GetOutputResourceByVariableName(
        "ViewDistance");
    NIASSERT(pkViewDistance);

    // Bind the world view vector as an output to the pixel shader.
    NiMaterialResource* pkVertOutWorldView = 
        kContext.m_spOutputs->AddInputResource("float3", "TexCoord", 
        "World", "WorldView");
    kContext.m_spConfigurator->AddBinding(pkViewVector, pkVertOutWorldView);

    // Bind the view distance as an output to the pixel shader.
    NiMaterialResource* pkVertOutViewDistance = 
        kContext.m_spOutputs->AddInputResource("float", "TexCoord", 
        "World", "ViewDistance");
    kContext.m_spConfigurator->AddBinding(pkViewDistance, 
        pkVertOutViewDistance);
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandlePlanarReflection(Context& kContext, 
    NiMaterialResource* pkPixelCalculatedNormal, 
    NiMaterialResource* pkPixelPlanarTexCoord, 
    NiMaterialResource*& pkPixelPlanarReflectedColor)
{

    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("CalculatePlanarReflection");

    if (!pkNode)
    {
        NIASSERT(!"Error in planar reflection calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    
    NiFixedString kSamplerName = NiTexturingProperty::GetMapNameFromID(
        NiTexturingProperty::SHADER_BASE);

    // Set the cube map texture sampler resource
    NiMaterialResource* pkSamplerRes = InsertTextureSampler(kContext,
        kSamplerName, TEXTURE_SAMPLER_2D, 0);

    // Retrieve constants
    NiMaterialResource* pkDisturbanceFactor = AddOutputAttribute(
        kContext.m_spUniforms, DISTURBANCE_SHADER_CONSTANT,
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);
    
    // Create bindings
    kContext.m_spConfigurator->AddBinding(pkPixelPlanarTexCoord,
        pkNode->GetInputResourceByVariableName("PlanarTex"));
    kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
        pkNode->GetInputResourceByVariableName("Sampler_Env"));
    kContext.m_spConfigurator->AddBinding(pkPixelCalculatedNormal, 
        pkNode->GetInputResourceByVariableName("Normal"));
    kContext.m_spConfigurator->AddBinding(pkDisturbanceFactor,
        pkNode->GetInputResourceByVariableName("Disturbance"));
    
    // output
    pkPixelPlanarReflectedColor = 
        pkNode->GetOutputResourceByVariableName("ReflectOut");



    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleRefraction(Context& kContext, 
    NiMaterialResource* pkPixelCalculatedNormal, 
    NiMaterialResource* pkPixelPlanarTexCoord, 
    NiMaterialResource*& pkPixelRefractedColor)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("CalculateRefraction");

    if (!pkNode)
    {
        NIASSERT(!"Error in refraction calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    NiFixedString kSamplerName = NiTexturingProperty::GetMapNameFromID(
        NiTexturingProperty::SHADER_BASE);
    

    // Set the cube map texture sampler resource
    NiMaterialResource* pkSamplerRes = InsertTextureSampler(kContext,
        kSamplerName, TEXTURE_SAMPLER_2D, 1);
    
    // Retrieve constant value
    NiMaterialResource* pkDisturbanceFactor = AddOutputAttribute(
        kContext.m_spUniforms, DISTURBANCE_SHADER_CONSTANT,
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);
    
    // create bindings
    kContext.m_spConfigurator->AddBinding(pkPixelPlanarTexCoord,
        pkNode->GetInputResourceByVariableName("PlanarTex"));
    kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
        pkNode->GetInputResourceByVariableName("Sampler_Env"));
    kContext.m_spConfigurator->AddBinding(pkPixelCalculatedNormal, 
        pkNode->GetInputResourceByVariableName("Normal"));
    kContext.m_spConfigurator->AddBinding(pkDisturbanceFactor,
        pkNode->GetInputResourceByVariableName("Disturbance"));
    
    // output
    pkPixelRefractedColor = 
        pkNode->GetOutputResourceByVariableName("RefractedOut");
    
    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleReflectionDirectionCalculation(Context& kContext,
    NiMaterialResource* pkPixelCalculatedNormal,
    NiMaterialResource* pkPixelViewVector,
    NiMaterialResource*& pkReflectionDirection)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("CalculateReflectionVector");

    if (!pkNode)
    {
        NIASSERT(!"Error in reflection vector calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // create bindings
    kContext.m_spConfigurator->AddBinding(pkPixelCalculatedNormal,
        pkNode->GetInputResourceByVariableName("Normal"));
    kContext.m_spConfigurator->AddBinding(pkPixelViewVector, 
        pkNode->GetInputResourceByVariableName("ViewVector"));
    
    pkReflectionDirection = 
        pkNode->GetOutputResourceByVariableName("ReflectionVector");


    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleCubeMapReflection(Context& kContext, 
    NiMaterialResource* pkReflectionDirection, 
    NiMaterialResource*& pkPixelCubeMapReflectedColor)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("CalculateCubeMapReflection");

    if (!pkNode)
    {
        NIASSERT(!"Error in cube map reflection calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    NiFixedString kSamplerName = NiTexturingProperty::GetMapNameFromID(
        NiTexturingProperty::SHADER_BASE);

    // Set the cube map texture sampler resource
    NiMaterialResource* pkSamplerRes = InsertTextureSampler(kContext,
        kSamplerName, TEXTURE_SAMPLER_CUBE, 0);

    // create bindings
    kContext.m_spConfigurator->AddBinding(pkReflectionDirection,
        pkNode->GetInputResourceByVariableName("ReflectionVector"));
    kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
        pkNode->GetInputResourceByVariableName("Sampler"));
    
    pkPixelCubeMapReflectedColor = 
        pkNode->GetOutputResourceByVariableName("ReflectedOut");


    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleSpecularCalculation(Context& kContext, 
    NiMaterialResource* pkReflectionDirection,
    NiMaterialResource*, 
    NiMaterialResource*& pkPixelSpecularColor)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("CalculateSpecularColor");

    if (!pkNode)
    {
        NIASSERT(!"Error in specular lighting calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // Add constant map elements
    NiMaterialResource* pkLightDirection = 
        AddOutputObject(kContext.m_spUniforms, 
        NiShaderConstantMap::SCM_OBJ_WORLDDIRECTION, 
        NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT, 0, "Light");
    NiMaterialResource* pkLightSpecular = 
        AddOutputObject(kContext.m_spUniforms, 
        NiShaderConstantMap::SCM_OBJ_SPECULAR,
        NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT, 0, "Light");
    NiMaterialResource* pkSpecularityFactor = AddOutputAttribute(
        kContext.m_spUniforms, SPECULARITY_SHADER_CONSTANT,
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);

   
    // create bindings
    kContext.m_spConfigurator->AddBinding(pkReflectionDirection,
        pkNode->GetInputResourceByVariableName("ReflectionVector")); 
    kContext.m_spConfigurator->AddBinding(pkLightDirection,
        pkNode->GetInputResourceByVariableName("LightDirection"));
    kContext.m_spConfigurator->AddBinding(pkLightSpecular,
        pkNode->GetInputResourceByVariableName("LightSpecular"));
    kContext.m_spConfigurator->AddBinding(pkSpecularityFactor,
        pkNode->GetInputResourceByVariableName("Specularity"));
    
    
    pkPixelSpecularColor = pkNode->GetOutputResourceByVariableName("ColorOut");

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleAccumulateColors(Context& kContext, 
    NiMaterialResource*& pkTempOutput, 
    NiMaterialResource* pkPixelLightedColor)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("AccumulateColors");

    if (!pkNode)
    {
        NIASSERT(!"Error in lighting calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // create bindings
    kContext.m_spConfigurator->AddBinding(pkPixelLightedColor,
        pkNode->GetInputResourceByVariableName("LightDiffuse"));
    kContext.m_spConfigurator->AddBinding(pkTempOutput, 
        pkNode->GetInputResourceByVariableName("MaterialColor"));
    
    pkTempOutput = pkNode->GetOutputResourceByVariableName("DiffuseOut");

    return true;

}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleModulateColors(Context& kContext, 
    NiMaterialResource*& pkTempOutput, 
    NiMaterialResource* pkPixelLightedColor)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("ModulateColors");

    if (!pkNode)
    {
        NIASSERT(!"Error in lighting calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // create bindings
    kContext.m_spConfigurator->AddBinding(pkPixelLightedColor,
        pkNode->GetInputResourceByVariableName("LightDiffuse"));
    kContext.m_spConfigurator->AddBinding(pkTempOutput, 
        pkNode->GetInputResourceByVariableName("MaterialColor"));
    
    pkTempOutput = pkNode->GetOutputResourceByVariableName("DiffuseOut");

    return true;

}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleLerpColors(Context& kContext, 
    NiMaterialResource*& pkTempOutput, 
    NiMaterialResource* pkColor,
    NiMaterialResource* pkLerpValue)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("LerpColors");

    if (!pkNode)
    {
        NIASSERT(!"Error in Lerp calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // create bindings
    kContext.m_spConfigurator->AddBinding(pkColor,
        pkNode->GetInputResourceByVariableName("Color1"));
    kContext.m_spConfigurator->AddBinding(pkTempOutput, 
        pkNode->GetInputResourceByVariableName("MaterialColor"));
    kContext.m_spConfigurator->AddBinding(pkLerpValue, 
        pkNode->GetInputResourceByVariableName("LerpValue"));
    
    pkTempOutput = pkNode->GetOutputResourceByVariableName("LerpOut");

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleRetrieveColorAlpha(Context& kContext, 
    NiMaterialResource* pkInputColor,
    NiMaterialResource*& pkDiffuseColor, 
    NiMaterialResource*& pkAlpha)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("RetrieveAlpha");

    if (!pkNode)
    {
        NIASSERT(!"Error in Lerp calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // create bindings
    kContext.m_spConfigurator->AddBinding(pkInputColor,
        pkNode->GetInputResourceByVariableName("Color"));

    pkAlpha = pkNode->GetOutputResourceByVariableName("AlphaOut");
    pkDiffuseColor = pkNode->GetOutputResourceByVariableName("ColorOut");

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleCalculateFogFactor(Context& kContext, 
    NiMaterialResource*& pkFogFactor, 
    NiMaterialResource* pkPixelViewDistance,
    NiMaterialResource* pkPixelViewVector, 
    NiMaterialResource* pkPixelWorldNorm, 
    NiMaterialResource* pkDepth)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("ComputeFogFactor");

    if (!pkNode)
    {
        NIASSERT(!"Error in Lerp calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // Retrieve constants
    NiMaterialResource* pkFogDensity = AddOutputAttribute(
        kContext.m_spUniforms, FOGDENSITY_SHADER_CONSTANT,
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);
    NiMaterialResource* pkFogDistance = AddOutputAttribute(
        kContext.m_spUniforms, FOGDISTANCE_SHADER_CONSTANT,
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);
    NiMaterialResource* pkFogFallOff = AddOutputAttribute(
        kContext.m_spUniforms, FOGFALLOFF_SHADER_CONSTANT,
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);

    // create bindings
    kContext.m_spConfigurator->AddBinding(pkPixelViewDistance,
        pkNode->GetInputResourceByVariableName("ViewDistance"));
    kContext.m_spConfigurator->AddBinding(pkPixelViewVector,
        pkNode->GetInputResourceByVariableName("ViewVector"));
    kContext.m_spConfigurator->AddBinding(pkPixelWorldNorm,
        pkNode->GetInputResourceByVariableName("WorldNormal"));
    kContext.m_spConfigurator->AddBinding(pkDepth,
        pkNode->GetInputResourceByVariableName("Depth"));
    kContext.m_spConfigurator->AddBinding(pkFogDensity,
        pkNode->GetInputResourceByVariableName("FogDensity"));
    kContext.m_spConfigurator->AddBinding(pkFogDistance,
        pkNode->GetInputResourceByVariableName("FogDistance"));
    kContext.m_spConfigurator->AddBinding(pkFogFallOff,
        pkNode->GetInputResourceByVariableName("FogFallOff"));

    pkFogFactor = pkNode->GetOutputResourceByVariableName("FogFactor");


    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleCalculateFresnelFactor(Context& kContext, 
    NiMaterialResource*& pkFresnelFactor, 
    NiMaterialResource* pkPixelReflectionVector, 
    NiMaterialResource* pkPixelCalculatedNormal)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("ComputeFresnelFactor");

    if (!pkNode)
    {
        NIASSERT(!"Error in Lerp calculation");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    // reatrieve constants
    NiMaterialResource* pkReflectionFactor = AddOutputAttribute(
        kContext.m_spUniforms, REFLECTION_SHADER_CONSTANT,
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);

    // create bindings
    kContext.m_spConfigurator->AddBinding(pkPixelReflectionVector,
        pkNode->GetInputResourceByVariableName("ReflectionVector"));
    kContext.m_spConfigurator->AddBinding(pkPixelCalculatedNormal,
        pkNode->GetInputResourceByVariableName("WorldNormal"));
    kContext.m_spConfigurator->AddBinding(pkReflectionFactor,
        pkNode->GetInputResourceByVariableName("ReflectionFactor"));

    pkFresnelFactor = pkNode->GetOutputResourceByVariableName("FresnelFactor");

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleMultiplyScalar(Context& kContext, 
    NiMaterialResource* pkScalar1,
    NiMaterialResource* pkScalar2,
    NiMaterialResource*& pkResult)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("MultiplyScalars");

    if (!pkNode)
    {
        NIASSERT(!"Error in scalar multiplication");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);
    
    // create bindings
    kContext.m_spConfigurator->AddBinding(pkScalar1,
        pkNode->GetInputResourceByVariableName("Scalar1"));
    kContext.m_spConfigurator->AddBinding(pkScalar2,
        pkNode->GetInputResourceByVariableName("Scalar2"));

    pkResult = pkNode->GetOutputResourceByVariableName("Result");

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleLerpScalar(Context& kContext, 
    NiMaterialResource* pkScalar1,
    NiMaterialResource* pkScalar2,
    NiMaterialResource* pkLerpValue,
    NiMaterialResource*& pkResult)
{
    // Create and add the material node
    NiMaterialNode* pkNode = 
        GetAttachableNodeFromLibrary("LerpScalars");

    if (!pkNode)
    {
        NIASSERT(!"Error in scalar lerp");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);
    
    if (pkScalar1)
    {
        // use data value instead of default
        kContext.m_spConfigurator->AddBinding(pkScalar1,
            pkNode->GetInputResourceByVariableName("Scalar1"));
    }
    
    if (pkScalar2)
    {
        // use data value instead of default
        kContext.m_spConfigurator->AddBinding(pkScalar2,
            pkNode->GetInputResourceByVariableName("Scalar2"));
    }

    kContext.m_spConfigurator->AddBinding(pkLerpValue,
        pkNode->GetInputResourceByVariableName("LerpValue"));

    pkResult = pkNode->GetOutputResourceByVariableName("Result");

    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleComposeColorAndAlpha(Context& kContext,
    NiMaterialResource* pkColor, 
    NiMaterialResource* pkAlpha,
    NiMaterialResource*& pkOutColor)
{
    NiMaterialNode* pkNode = GetAttachableNodeFromLibrary(
        "SetColorAndAlpha");    
    kContext.m_spConfigurator->AddNode(pkNode);
  
    kContext.m_spConfigurator->AddBinding(pkColor, "Color", 
        pkNode);

    if (pkAlpha)    
        kContext.m_spConfigurator->AddBinding(pkAlpha, "Alpha", pkNode);
    
    pkOutColor = pkNode->GetOutputResourceByVariableName("OutputColor");
    
    return true;
}
//----------------------------------------------------------------------------
bool NiWaterMaterial::HandleCalculateAndApplyFog(Context& kContext, 
    NiWaterMaterialPixelDescriptor* pkPixelDesc,
    NiMaterialResource* pkViewDistance,
    NiMaterialResource* pkUnfoggedColor, 
    NiMaterialResource*& pkFogOutput)
{
    // At this time, all other platforms require this fog calculation.
    if (pkPixelDesc->GetFOGTYPE() != FOG_NONE)
    {
        if (kContext.m_spConfigurator->GetPlatformString() == "DX9")
        {
            // DX9 uses HLSL with varying shader targets
            // Fog should only be applied by the pixel shader in SM 3.0 or
            // greater
            NiGPUProgramCache* pkCache = 
                m_aspProgramCaches[NiGPUProgram::PROGRAM_PIXEL];
            NiFixedString kShaderTarget = pkCache->GetShaderProfile();
        
            if (strstr(kShaderTarget, "ps_2_"))
            {
                return false;
            }
        }

        NiMaterialResource* pkFogValue = NULL;
        HandleCalculateFog(kContext, pkViewDistance, pkFogValue, 
            static_cast<Fog>(pkPixelDesc->GetFOGTYPE()));

        NiMaterialNode* pkFogNode = 
            GetAttachableNodeFromLibrary("ApplyFog");
        kContext.m_spConfigurator->AddNode(pkFogNode);

        kContext.m_spConfigurator->AddBinding(
            pkFogValue, 
            pkFogNode->GetInputResourceByVariableName("FogAmount"));

        kContext.m_spConfigurator->AddBinding(pkUnfoggedColor,
            pkFogNode->GetInputResourceByVariableName("UnfoggedColor"));

        NiMaterialResource* pkFogColor = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_FOG_COLOR); 
        kContext.m_spConfigurator->AddBinding(pkFogColor, 
            pkFogNode->GetInputResourceByVariableName("FogColor"));

        pkFogOutput = 
            pkFogNode->GetOutputResourceByVariableName("FoggedColor"); 
    }
    else
    {
        pkFogOutput = pkUnfoggedColor;
    }

    return true;
}
//----------------------------------------------------------------------------
NiFixedString NiWaterMaterial::GenerateSamplerName(
    const NiFixedString& baseName, NiUInt32 uiIndex)
{
    char acResult[32];    
    NiSprintf(acResult, 32, "%s%d", (const char*)baseName, uiIndex);
    return acResult;
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::DropPerPixelFogging(
    NiMaterialDescriptor*, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int /*uiMaxCount*/, unsigned int& /*uiCount*/, 
    unsigned int& /*uiFunctionData*/)
{
    // This function can only deal with failed pixel or vertex shader compiles
    if ((eFailedRC & RC_COMPILE_FAILURE_PIXEL) == 0 &&
        (eFailedRC & RC_COMPILE_FAILURE_VERTEX) == 0)
    {
        return false;
    }

    NiWaterMaterialPixelDescriptor* pkInvalidPixelDesc = 
        reinterpret_cast<NiWaterMaterialPixelDescriptor*>(
        &pkRenderPasses[uiFailedPass].m_kPixelDesc);

    NiWaterMaterialVertexDescriptor* pkInvalidVertexDesc = 
        reinterpret_cast<NiWaterMaterialVertexDescriptor*>(
        &pkRenderPasses[uiFailedPass].m_kVertexDesc);

    // Is fogging enabled in pixel shader? (Shader Model 3.0)
    if (pkInvalidPixelDesc->GetFOGTYPE() != FOG_NONE)
    {            
        NILOG("NiWaterMaterial: Drop per pixel fogging in favor or vertex "
            "fogging.\n");

        pkInvalidVertexDesc->SetFOGTYPE(pkInvalidPixelDesc->GetFOGTYPE());
        pkInvalidPixelDesc->SetFOGTYPE(FOG_NONE);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiWaterMaterial::AddDefaultFallbacks()
{
    AddShaderFallbackFunc(DropPerPixelFogging);

    NiFragmentMaterial::AddDefaultFallbacks();
}
//---------------------------------------------------------------------------
bool NiWaterMaterial::HandleFinalVertexOutputs(Context& kContext,
    NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldView,
    NiMaterialResource* pkViewDistance)
{
    NI_UNUSED_ARG(kContext);
    NI_UNUSED_ARG(pkWorldPos);
    NI_UNUSED_ARG(pkWorldView);
    NI_UNUSED_ARG(pkViewDistance);

    return true;
}
//---------------------------------------------------------------------------

