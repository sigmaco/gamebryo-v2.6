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

// Pre-compiled header
#include "NiTerrainPCH.h"

#include "NiTerrainMaterial.h"
#include "NiTerrainMaterialDescriptor.h"
#include "NiTerrainMaterialNodeLibrary.h"
#include "NiTerrainMaterialPixelDescriptor.h"
#include "NiTerrainMaterialVertexDescriptor.h"
#include "NiTerrainSectorData.h"

#include <NiIntegersExtraData.h>
#include <NiCommonSemantics.h>
#include <NiFogProperty.h>
#include <NiRenderer.h>
#include <NiRenderObject.h>
#include <NiRenderObjectMaterialOption.h>
#include <NiShaderFactory.h>
#include <NiSourceTexture.h>
#include <NiIntegerExtraData.h>
#include <NiBooleanExtraData.h>


NiImplementRTTI(NiTerrainMaterial, NiFragmentMaterial);

const char* NiTerrainMaterial::LODTHRESHOLD_SHADER_CONSTANT = 
    "g_LODThresholdDistance";

const char* NiTerrainMaterial::LODMORPHDISTANCE_SHADER_CONSTANT = 
    "g_LODMorphDistance";

const char* NiTerrainMaterial::UVBLENDOFFSET_SHADER_CONSTANT = 
    "g_UVOffsetBlend";

const char* NiTerrainMaterial::UVBLENDSCALE_SHADER_CONSTANT = 
    "g_UVScaleBlend";

const char* NiTerrainMaterial::UVSURFACEOFFSET_SHADER_CONSTANT = 
    "g_UVOffsetSurface";

const char* NiTerrainMaterial::UVSURFACESCALE_SHADER_CONSTANT = 
    "g_UVScaleSurface";

const char* NiTerrainMaterial::UVSURFACEUVMODIFIERARRAY_SHADER_CONSTANT = 
    "g_SurfaceUVModifierArray";

const char* NiTerrainMaterial::MORPHMODE_SHADER_CONSTANT = 
    "g_MorphMode";

const char* NiTerrainMaterial::STITCHINGINFO_SHADER_CONSTANT = 
    "g_StitchingInfo";

const char* NiTerrainMaterial::DISTMASKSTR_SHADER_CONSTANT =
    "g_DistMaskStrength";

//---------------------------------------------------------------------------
NiTerrainMaterial::NiTerrainMaterial(NiMaterialNodeLibrary* pkMaterialNodeLib,
    bool bAutoCreateCaches,const NiFixedString &name) : NiFragmentMaterial(
    pkMaterialNodeLib,
    name, 
    VERTEX_VERSION, GEOMETRY_VERSION, PIXEL_VERSION, 
    bAutoCreateCaches),
    m_kMaterialDescriptorName("NiTerrainMaterialDescriptor"),
    m_kVertexShaderDescriptorName("NiTerrainMaterialVertexDescriptor"),
    m_kPixelShaderDescriptorName("NiTerrainMaterialPixelDescriptor")
{
    // Get the shader version we are currently running on. This is used during
    // shade tree construction to drop certain maps (normal and parallax) if we
    // are running on any 2_x shader model.

    NiGPUProgramCache* pkCache = 
        m_aspProgramCaches[NiGPUProgram::PROGRAM_PIXEL];
    const NiFixedString& kShaderModel = pkCache->GetShaderProfile();

    // If we are on an SM2_x card do not allow normal and parallax maps even if 
    // the profile has enough instructions to support them. This is due to 
    // performance issues with SM2_x when normal and parallax maps are enabled for
    // the terrain.
    if (strstr((const char*)kShaderModel, "ps_2_"))
        m_bEnableNPOnSM2 = false;
    else
        m_bEnableNPOnSM2 = true;
}
//---------------------------------------------------------------------------
NiTerrainMaterial::~NiTerrainMaterial()
{
}
//---------------------------------------------------------------------------
NiTerrainMaterial* NiTerrainMaterial::Create()
{
    NiTerrainMaterial* pkMaterial = NiDynamicCast(NiTerrainMaterial, 
        NiMaterial::GetMaterial("NiTerrainMaterial"));

    if (!pkMaterial)
    {
        NiMaterialNodeLibrary* pkTerrainNodeLib = 
            NiTerrainMaterialNodeLibrary::CreateMaterialNodeLibrary();

        pkMaterial = NiNew NiTerrainMaterial(pkTerrainNodeLib);
        pkMaterial->AddDefaultFallbacks();
    }

    return pkMaterial;
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::SetForceEnableNormalParallaxMaps(bool bEnable)
{
    m_bEnableNPOnSM2 = bEnable;
}
//---------------------------------------------------------------------------
NiShader* NiTerrainMaterial::CreateShader(NiMaterialDescriptor* pkDesc)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer == NULL)
        return false;

    return pkRenderer->GetFragmentShader(pkDesc);
}
//---------------------------------------------------------------------------
bool NiTerrainMaterial::SetupPackingRequirements(NiShader* pkShader,
    NiMaterialDescriptor* pkMaterialDescriptor,
    NiFragmentMaterial::RenderPassDescriptor*, 
    NiUInt32)
{
    NiTerrainMaterialDescriptor* pkMaterialDesc = 
        (NiTerrainMaterialDescriptor*)pkMaterialDescriptor;

    NiUInt32 uiStreamCount = 1;
    NiShaderDeclarationPtr spShaderDecl = 
        NiShaderDeclaration::Create(15, uiStreamCount);

    if (!spShaderDecl)
    {
        NIASSERT(!"Invalid shader declaration.");
        return false;
    }

    NiUInt32 uiEntryCount = 0;
    
    // Handle position and normal streams.
    spShaderDecl->SetEntry(uiEntryCount++, 
        NiShaderDeclaration::SHADERPARAM_NI_POSITION0,
        NiShaderDeclaration::SPTYPE_FLOAT4);

    spShaderDecl->SetEntry(uiEntryCount++,
        NiShaderDeclaration::SHADERPARAM_NI_NORMAL,
        NiShaderDeclaration::SPTYPE_FLOAT4);

    // Check for per-vertex tangents and add a stream if they are present.
    if (pkMaterialDesc->GetTANGENTS() && m_bEnableNPOnSM2)
    {
        spShaderDecl->SetEntry(uiEntryCount++, 
            NiShaderDeclaration::SHADERPARAM_NI_TANGENT,
            NiShaderDeclaration::SPTYPE_FLOAT4);
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
bool NiTerrainMaterial::GenerateDescriptor(const NiRenderObject* pkGeometry, 
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

    NiTerrainMaterialDescriptor* pkDesc = (NiTerrainMaterialDescriptor*)
        &kMaterialDesc;
    
    pkDesc->m_kIdentifier = m_kMaterialDescriptorName;
    
    // Make sure the terrain material is being applied to the proper geometry.
    if (pkGeometry->RequiresMaterialOption(
        NiRenderObjectMaterialOption::TRANSFORM_SKINNED()))
    {
        NIASSERT(!"Cannot apply terrain material to skinned geometry.\n");
        return false;
    }

    if (pkGeometry->RequiresMaterialOption(
        NiRenderObjectMaterialOption::TRANSFORM_INSTANCED()))
    {
        NIASSERT(!"Cannot apply terrain material to instanced geometry.\n");
        return false;
    }

    if (pkGeometry->RequiresMaterialOption(
        NiRenderObjectMaterialOption::MORPHING()))
    {
        NIASSERT(!"Cannot apply terrain material to morphed geometry.\n");
        return false;
    }
        
    // Terrain geometry must always have at least per-vertex normals and one 
    // set of texture coordinates.
    bool bHasNormals = pkGeometry->ContainsData(NiCommonSemantics::NORMAL());
    if (!bHasNormals)
    {
        NIASSERT(!"Terrain geometry must have per vertex normals.");
        return false;
    }

    // Count up the number of texture coordinate sets on the geometry.
    NiUInt32 uiNumTexCoordSets = pkGeometry->GetSemanticCount(
        NiCommonSemantics::TEXCOORD());
    if (uiNumTexCoordSets == 0)
    {
        NIASSERT(!"Terrain geometry must have at least one set of texture "
            "coordinates.");
        return false;
    }
    else
    {
        pkDesc->SetNUM_TEXCOORDS(uiNumTexCoordSets);
    }
    
    // Figure out if the surface's coordinates are being modified:
    if (pkGeometry->GetExtraData(UVSURFACEUVMODIFIERARRAY_SHADER_CONSTANT))
    {
        pkDesc->SetENABLE_SURFACEUVMODIFIERS(1);
    }
    else
    {
        pkDesc->SetENABLE_SURFACEUVMODIFIERS(0);
    }

    // Figure out if we are doing 2D or 3D morphing:
    NiIntegerExtraData* pkMorphMode = NiDynamicCast(NiIntegerExtraData,
        pkGeometry->GetExtraData(MORPHMODE_SHADER_CONSTANT));
    pkDesc->SetMORPH_MODE(0);
    if (pkMorphMode)
    {
        NiUInt32 uiMode = pkMorphMode->GetValue();
        if (uiMode & NiTerrainSectorData::LOD_MORPH_ENABLE)
        {
            pkDesc->SetMORPH_MODE(
                (uiMode & (~NiTerrainSectorData::LOD_MORPH_ENABLE))
                + 1);
        }
    }
    
    // Terrain geometry may or may not have tangents pre-computed per vertex.
    // If they exist we will use them, else a complete tangent frame will be 
    // computed in the pixel shader.
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
    pkDesc->SetTANGENTS(bHasTangents && m_bEnableNPOnSM2);    

    // Handle property states...
    if (pkPropState)
    {
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

        // Handle texture properties.        
        NiTexturingProperty* pkTexProp = pkPropState->GetTexturing();
        if (pkTexProp && pkTexProp->GetShaderMapCount())
        {  
            // Determine which maps exist for each layer and what UV set to use
            // for each of them.            
            NiTPrimitiveArray<NiTexturingProperty::ShaderMap*> kBaseMaps;
            NiTPrimitiveArray<NiTexturingProperty::ShaderMap*> kNormalMaps;
            NiTPrimitiveArray<NiTexturingProperty::ShaderMap*> kDistributionMaps;
            NiTPrimitiveArray<NiTexturingProperty::ShaderMap*> kBlendMaps;
            BuildTextureArrays(pkTexProp, kBaseMaps, kNormalMaps, 
                kDistributionMaps, kBlendMaps);
            
            // Now that we have each texture assigned to the geometry bucketed
            // by type, map them to a layer. It is assumed that the shader maps
            // were added such that the ordering of each map determines the 
            // layer the map belongs on.
            BuildLayerDescription(pkDesc, kBaseMaps, kNormalMaps, 
                kDistributionMaps, kBlendMaps);
        }
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
            if (pkLight && (pkLight->GetEffectType() == 
                NiDynamicEffect::DIR_LIGHT || pkLight->GetEffectType() == 
                NiDynamicEffect::SHADOWDIR_LIGHT))
            {  
                pkDesc->SetENABLE_SUNLIGHT(1);
                break;
            }             
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::BuildLayerDescription(
    NiTerrainMaterialDescriptor* pkDesc, 
    const NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kBaseMaps,
    const NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kNormalMaps,
    const NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kDistributionMaps,
    const NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kBlendMaps)
{
    NIASSERT(pkDesc);
    
    // Handle the blend maps first.
    pkDesc->SetBLENDMAP_COUNT(kBlendMaps.GetSize());

    if (kBlendMaps.GetSize())
    {
        pkDesc->SetBLENDMAP0_UVINDEX(kBlendMaps[0]->GetTextureIndex());
        if (kBlendMaps.GetSize() == 2)    
            pkDesc->SetBLENDMAP1_UVINDEX(kBlendMaps[1]->GetTextureIndex());
    }
    
    // Now enable the various layers based on the maps available. The number of
    // possible layers is based on the number of blend maps available and the 
    // existence of a base map for the layer. For example, there could be two 
    // blend maps, but only the first channel of the second blend map is 
    // actually used. This results in 5 layers even though there could have 
    // been 8.
    NiUInt32 uiCurrLayerIndex = 0;
    NiUInt32 uiMaxLayers = kBlendMaps.GetSize() * 4;
    NIASSERT(uiMaxLayers <= MAX_LAYERS_PER_BLOCK);

    NiTexturingProperty::ShaderMap* pkDistMap = NULL;
    if (kDistributionMaps.GetSize())
    {
        pkDesc->SetNUM_DIST_MAPS(1);
        pkDistMap = kDistributionMaps[0];
    }
    else
    {
        pkDesc->SetNUM_DIST_MAPS(0);
    }

    while (uiCurrLayerIndex < uiMaxLayers)
    {
        // Stop constructing layers if we run out of base map
        if (uiCurrLayerIndex == kBaseMaps.GetSize())
            break;
        
        // The UV set used for a layer is determined by the UV set assigned to 
        // the base map.
        NiTexturingProperty::ShaderMap* pkBaseMap = 
            kBaseMaps[uiCurrLayerIndex];

        NiTexturingProperty::ShaderMap* pkNormalMap = 
            uiCurrLayerIndex < kNormalMaps.GetSize() ? 
            kNormalMaps[uiCurrLayerIndex] : NULL;

        NIASSERT(pkBaseMap);
        NIASSERT(pkBaseMap->GetTextureIndex() < pkDesc->GetNUM_TEXCOORDS());
               
        // Detail maps may be packed into the alpha channel of the base map.
        // Parallax maps may be packed into the alpha channel of the normal 
        // map.
        bool bHasDetailMap = false;
        bool bHasParallaxMap = false;

        NiTexture* pkBaseTex = pkBaseMap->GetTexture();
        NIASSERT(pkBaseTex);

        if (pkBaseTex->GetExtraDataSize() > 0)
        {
            NiBooleanExtraData* pkBoolData = NiDynamicCast(NiBooleanExtraData,
                pkBaseTex->GetExtraDataAt(0));
            if (pkBoolData)
            {
                bHasDetailMap = pkBoolData->GetValue();
            }
        }

        if (pkNormalMap && m_bEnableNPOnSM2)
        {
            NiTexture* pkNormalTex = pkNormalMap->GetTexture();
            NIASSERT(pkNormalTex);
            if (pkNormalTex->GetExtraDataSize() > 0)
            {
                NiBooleanExtraData* pkBoolData = 
                    NiDynamicCast(NiBooleanExtraData, 
                    pkNormalTex->GetExtraDataAt(0));
                if (pkBoolData)
                {
                    bHasParallaxMap = pkBoolData->GetValue();
                }
            }
        }

        // Distribution maps are packed in groups of 4 where each map is stored in
        // a separate channel.
        bool bHasDistributionMap = false;
        if (pkDistMap)
        {
            NIASSERT(pkDistMap->GetTexture());

            NiIntegersExtraData* pkDistExtraData = NiDynamicCast(
                NiIntegersExtraData, pkDistMap->GetTexture()->GetExtraDataAt(0));

            NIASSERT(pkDistExtraData);
            NiUInt32 uiSize;
            NiInt32* piEnableFlags = NULL;
            pkDistExtraData->GetArray(uiSize, piEnableFlags);

            NIASSERT(piEnableFlags);
            NIASSERT(uiSize == 4);
            bHasDistributionMap = piEnableFlags[uiCurrLayerIndex] == 1;
        }
        
        pkDesc->SetLayerCaps(uiCurrLayerIndex, pkBaseMap->GetTextureIndex(), 
            bHasDetailMap, pkNormalMap != NULL && m_bEnableNPOnSM2, 
            bHasParallaxMap, bHasDistributionMap);

        uiCurrLayerIndex++;
    }

    pkDesc->SetNUM_LAYERS(uiCurrLayerIndex);
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::BuildTextureArrays(NiTexturingProperty* pkTexProp, 
    NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kBaseMaps,
    NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kNormalMaps,
    NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kDistributionMaps,
    NiTPrimitiveArray<NiTexturingProperty::ShaderMap*>& kBlendMaps)
{       
    NiUInt32 uiShaderMapCount = pkTexProp->GetShaderArrayCount();
    
    // Handle the case where there are no blend maps (e.g. no layers) yet 
    // assigned to the terrain.
    if (pkTexProp->GetShaderMapCount() == 0)
        return;

    // The shader map array should contain up to 2 blend maps in the first two 
    // slots of the array.
    NiTexturingProperty::ShaderMap* pkCurrMap;
    NiUInt32 uiCurrShaderMap = 0;

    pkCurrMap = pkTexProp->GetShaderMap(uiCurrShaderMap++);

    // Must have at least one blend map.
    NIASSERT(pkCurrMap && pkCurrMap->GetID() == BLEND_MAP);
    kBlendMaps.Add(pkCurrMap);
    
    // The next map could be either a base map, a blend map, or a distribution map
    // If there are no more maps then no layers have been assigned to the geometry 
    // meaning we'll just put in a temporary default texture as the base map 
    // for layer 0.
    pkCurrMap = pkTexProp->GetShaderMap(uiCurrShaderMap++);
    if (pkCurrMap)
    {
        if (pkCurrMap->GetID() == BLEND_MAP)
        {
            kBlendMaps.Add(pkCurrMap);
        }
        else if (pkCurrMap->GetID() == COMBINED_DISTRIBUTION_MASK)
        {
            kDistributionMaps.Add(pkCurrMap);
        }
        else
        {
            uiCurrShaderMap--;
        }
    }

    while (uiCurrShaderMap < uiShaderMapCount)
    {
        pkCurrMap = pkTexProp->GetShaderMap(uiCurrShaderMap++);
        
        if (!pkCurrMap || pkCurrMap->GetID() == DISTRIBUTION_MASK)
            continue;

        // Must always have a valid base map.
        NIASSERT(pkCurrMap && pkCurrMap->GetID() == BASE_MAP);        
        kBaseMaps.Add(pkCurrMap);            
        
        pkCurrMap = pkTexProp->GetShaderMap(uiCurrShaderMap++);
        if (pkCurrMap && m_bEnableNPOnSM2)
        {
            NIASSERT(pkCurrMap->GetID() == NORMAL_MAP);
            kNormalMaps.Add(pkCurrMap);
        }
        else
        {
            kNormalMaps.Add(NULL);
        }
    }
}
//---------------------------------------------------------------------------
NiFragmentMaterial::ReturnCode NiTerrainMaterial::GenerateShaderDescArray(
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

    NiTerrainMaterialDescriptor* pkMaterialDesc = 
        (NiTerrainMaterialDescriptor*)pkMaterialDescriptor;

    NiTerrainMaterialVertexDescriptor* pkVertexDesc =
        (NiTerrainMaterialVertexDescriptor*)&pkRenderPasses[0].m_kVertexDesc;
    pkVertexDesc->m_kIdentifier = m_kVertexShaderDescriptorName;

    NiTerrainMaterialPixelDescriptor* pkPixelDesc = 
        (NiTerrainMaterialPixelDescriptor*)&pkRenderPasses[0].m_kPixelDesc;
    pkPixelDesc->m_kIdentifier = m_kPixelShaderDescriptorName;

    bool bHasTangents = (pkMaterialDesc->GetTANGENTS() != 0);    
    pkVertexDesc->SetTANGENTS(bHasTangents);
    pkVertexDesc->SetFOGTYPE(pkMaterialDesc->GetFOGTYPE());
    pkPixelDesc->SetFOGTYPE(pkMaterialDesc->GetFOGTYPE());
    pkPixelDesc->SetENABLE_SURFACEUVMODIFIERS(
        pkMaterialDesc->GetENABLE_SURFACEUVMODIFIERS());

    // Set the number of input texture coordinates to use on the shader.
    if (pkMaterialDesc->GetNUM_LAYERS())
    {
        pkVertexDesc->SetNUM_TEXCOORDS(pkMaterialDesc->GetNUM_TEXCOORDS());
        pkPixelDesc->SetNUM_TEXCOORDS(pkMaterialDesc->GetNUM_TEXCOORDS());    
    }
    else
    {
        pkVertexDesc->SetNUM_TEXCOORDS(0);
        pkPixelDesc->SetNUM_TEXCOORDS(0);
    }    
    
    bool bOutputWorldView = pkMaterialDesc->RequiresViewVector();    
    pkVertexDesc->SetOUTPUT_WORLDVIEW(bOutputWorldView);
    pkPixelDesc->SetWORLD_VIEW(bOutputWorldView);

    // Copy the Morph Mode value:
    pkVertexDesc->SetMORPH_MODE(pkMaterialDesc->GetMORPH_MODE());

    // If there are per-vertex tangents, then we want to output a tangent frame
    // per vertex with the bi-normal computed in the vertex shader. If there 
    // are no tangents, we send across a per-vertex normal and compute the 
    // tangent frame in the pixel shader.    
    if (pkVertexDesc->GetTANGENTS())   
    {
        pkVertexDesc->SetOUTPUTWORLDNBT(1);        
        pkPixelDesc->SetWORLDNBT(1);        
    }
    else
    {
        pkVertexDesc->SetOUTPUTWORLDNBT(0);
        pkPixelDesc->SetWORLDNBT(0);
    }

    // Handle any lights affecting the terrain.
    pkPixelDesc->SetENABLE_SUNLIGHT(pkMaterialDesc->GetENABLE_SUNLIGHT());   

    // Now inspect the material descriptor for information regarding the layers
    // to render for the block.
    NiUInt32 uiNumLayers = pkMaterialDesc->GetNUM_LAYERS();
    pkPixelDesc->SetNUM_LAYERS(uiNumLayers);

    NiUInt32 uiNumBlendMaps = pkMaterialDesc->GetBLENDMAP_COUNT();
    pkPixelDesc->SetBLENDMAP_COUNT(uiNumBlendMaps);

    NiUInt32 uiNumDistMaps = pkMaterialDesc->GetNUM_DIST_MAPS();
    pkPixelDesc->SetNUM_DIST_MAPS(uiNumDistMaps);
  
    for (NiUInt32 ui = 0; ui < uiNumLayers; ui++)
    {
        pkPixelDesc->SetLayerCapabilitiesFromMaterialDescriptor(pkMaterialDesc,
            ui);        
    }
    
    uiCountAdded++;
    return NiFragmentMaterial::RC_SUCCESS;
}
//---------------------------------------------------------------------------
bool NiTerrainMaterial::GenerateVertexShadeTree(Context& kContext, 
    NiGPUProgramDescriptor* pkDesc)
{
    NIASSERT(pkDesc->m_kIdentifier == "NiTerrainMaterialVertexDescriptor");
    NiTerrainMaterialVertexDescriptor* pkVertexDesc = 
        (NiTerrainMaterialVertexDescriptor*)pkDesc;

    kContext.m_spConfigurator->SetDescription(pkVertexDesc->ToString());

    // Add vertex in, vertex out, constants, and uniforms
    if (!AddDefaultMaterialNodes(kContext, pkDesc, 
        NiGPUProgram::PROGRAM_VERTEX))
    {
        return false;
    }

    kContext.m_spInputs->AddOutputResource("float4", "Position", "Local", 
        "vPosition");
    kContext.m_spInputs->AddOutputResource("float4", "Normal", "Local",
        "vNormal");
    
    if (pkVertexDesc->GetTANGENTS())
    {
        kContext.m_spInputs->AddOutputResource("float4", "Tangent", "Local",
            "vTangent");    
    }    

    // Handle the texture coordinates.
    HandleVertexTexCoordSets(kContext, pkVertexDesc);

    // First split the position, normal, and possibly tangent into a source and
    // a destination morph.
    NiMaterialResource* pkPosHigh = NULL;
    NiMaterialResource* pkPosLow = NULL;
    NiMaterialResource* pkNormalHigh = NULL;
    NiMaterialResource* pkNormalLow = NULL;
    NiMaterialResource* pkTangentHigh = NULL;
    NiMaterialResource* pkTangentLow = NULL;
    HandleInputDataSplits(kContext, pkVertexDesc, pkPosHigh, pkPosLow, 
        pkNormalHigh, pkNormalLow, pkTangentHigh, pkTangentLow);

    // Transform the resulting high and low values into world space.
    NiMaterialResource* pkWorldMatrix = AddOutputPredefined(
        kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_WORLD, 4);
    NIASSERT(pkWorldMatrix);

    // Transform our vertex positions into world space.    
    HandleWorldSpaceTransformation(kContext, pkPosHigh, 
        pkPosHigh, pkWorldMatrix);
    HandleWorldSpaceTransformation(kContext, pkPosLow, 
        pkPosLow, pkWorldMatrix);

    // Handle the view vector fragment and connect it to the vertex out if 
    // there are any specular maps available.
    NiMaterialResource* pkViewVector = NULL;
    HandleViewVector(kContext, pkVertexDesc, pkViewVector, 
        pkPosHigh);

    // Calculate the morph value for the vertex attributes.
    NiMaterialResource* pkMorphValue = NULL;
    HandleCalculateMorphValue(kContext, pkVertexDesc, pkMorphValue);

    Fog eFogType = (Fog)pkVertexDesc->GetFOGTYPE();
    bool bOutputViewPos = eFogType != FOG_NONE;

    // Lerp the high and low positions and send them through the view 
    // projection matrix. Also, output the interpolated world position of the 
    // vertex as well.
    NiMaterialResource* pkViewPos = NULL;
    LerpPositionsAndApplyViewProjection(kContext, pkPosHigh, pkPosLow, 
        pkMorphValue, pkViewPos, bOutputViewPos);

    HandleCalculateFog(kContext, pkViewPos, eFogType);

    // Lerp the normals and possible the vertex tangents and send them to the 
    // vertex output. Also generate a bi-normal if tangents are present.
    LerpAndOutputNormalsAndTangents(kContext, pkVertexDesc, pkNormalHigh, 
        pkNormalLow, pkTangentHigh, pkTangentLow, pkMorphValue, pkWorldMatrix);

    // Calls into virtual handler for the final vertex outputs
    HandleFinalVertexOutputs(kContext, pkPosLow, pkPosHigh);

    return true;
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleVertexTexCoordSets(
    NiFragmentMaterial::Context& kContext,
    NiTerrainMaterialVertexDescriptor* pkDesc)
{    
    NiUInt32 uiNumTexCoordSets = pkDesc->GetNUM_TEXCOORDS();

    // We need to add the texture coordinates to the input of the vertex 
    // shader. Then they are transformed and passed on to the pixel shader.    
    for (NiUInt32 ui = 0; ui < uiNumTexCoordSets; ui++)
    {
        // Create a unique name for the texture set.
        char acVarName[32];
        NiSprintf(acVarName, 32, "UVSet%d", ui);        

        // Add to vertex input.
        NiMaterialResource* pkUVIn = kContext.m_spInputs->AddOutputResource(
            "float2", "TexCoord", "", acVarName);        

        // Pass it through a series of fragments.
        NiMaterialNode* pkCalcTexCoord = GetAttachableNodeFromLibrary(
            "CalculateTextureCoordinates");
        kContext.m_spConfigurator->AddNode(pkCalcTexCoord);

        kContext.m_spConfigurator->AddBinding(pkUVIn,
            pkCalcTexCoord->GetInputResourceByVariableName("UVCoord"));

        NiMaterialResource* pkUVBlendScale = AddOutputAttribute(
            kContext.m_spUniforms, UVBLENDSCALE_SHADER_CONSTANT, 
            NiShaderAttributeDesc::ATTRIB_TYPE_POINT2);
        NIASSERT(pkUVBlendScale);

        NiMaterialResource* pkUVBlendOffset = AddOutputAttribute(
            kContext.m_spUniforms, UVBLENDOFFSET_SHADER_CONSTANT, 
            NiShaderAttributeDesc::ATTRIB_TYPE_POINT2);
        NIASSERT(pkUVBlendOffset);

        NiMaterialResource* pkUVScaleSurface = AddOutputAttribute(
            kContext.m_spUniforms, UVSURFACESCALE_SHADER_CONSTANT, 
            NiShaderAttributeDesc::ATTRIB_TYPE_POINT2);
        NIASSERT(pkUVScaleSurface);

        NiMaterialResource* pkUVOffsetSurface = AddOutputAttribute(
            kContext.m_spUniforms, UVSURFACEOFFSET_SHADER_CONSTANT, 
            NiShaderAttributeDesc::ATTRIB_TYPE_POINT2);
        NIASSERT(pkUVOffsetSurface);

        kContext.m_spConfigurator->AddBinding(pkUVBlendScale, 
            pkCalcTexCoord->GetInputResourceByVariableName("UVBlendScale"));

        kContext.m_spConfigurator->AddBinding(pkUVBlendOffset, 
            pkCalcTexCoord->GetInputResourceByVariableName("UVBlendOffset"));

        kContext.m_spConfigurator->AddBinding(pkUVScaleSurface, 
            pkCalcTexCoord->GetInputResourceByVariableName("UVSurfaceScale"));

        kContext.m_spConfigurator->AddBinding(pkUVOffsetSurface, 
            pkCalcTexCoord->GetInputResourceByVariableName("UVSurfaceOffset"));
         
        // Output the result from the vertex shader.        
        NiMaterialResource* pkVertUVOut = 
            kContext.m_spOutputs->AddInputResource("float4", "TexCoord", "",
            acVarName);

        kContext.m_spConfigurator->AddBinding(
            pkCalcTexCoord->GetOutputResourceByVariableName("OutputUVCoord"), 
            pkVertUVOut);
    }
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::LerpAndOutputNormalsAndTangents(
    NiFragmentMaterial::Context& kContext,
    NiTerrainMaterialVertexDescriptor* pkDesc,    
    NiMaterialResource* pkNormalHigh, NiMaterialResource* pkNormalLow,
    NiMaterialResource* pkTangentHigh, NiMaterialResource* pkTangentLow,
    NiMaterialResource* pkMorphValue, NiMaterialResource* pkWorldMatrix)
{
    NIASSERT(pkNormalHigh);
    NIASSERT(pkNormalLow);    
    NIASSERT(pkMorphValue);
    NIASSERT(pkWorldMatrix);

    // First send the normals and possible the tangents through the 
    // interpolator fragment to get the final values.
    NiMaterialNode* pkLerpFloat3 = GetAttachableNodeFromLibrary(
        "LerpFloat3AndNormalize");
    kContext.m_spConfigurator->AddNode(pkLerpFloat3);

    // Handle normals.
    kContext.m_spConfigurator->AddBinding(pkNormalHigh,
        pkLerpFloat3->GetInputResourceByVariableName("InputOne"));
    kContext.m_spConfigurator->AddBinding(pkNormalLow,
        pkLerpFloat3->GetInputResourceByVariableName("InputTwo"));
    kContext.m_spConfigurator->AddBinding(pkMorphValue,
        pkLerpFloat3->GetInputResourceByVariableName("Amount"));

    NiMaterialResource* pkFinalNormal = 
        pkLerpFloat3->GetOutputResourceByVariableName("InterpolatedOutput");
    NIASSERT(pkFinalNormal);

    HandleWorldSpaceRotation(kContext, pkFinalNormal, pkFinalNormal,
        pkWorldMatrix);

    kContext.m_spConfigurator->AddBinding(pkFinalNormal,
        kContext.m_spOutputs->AddInputResource("float3", "TexCoord", "World",
        "WorldNormal"));

    // Handle tangents if present. If we do have tangents generate a per-vertex
    // transposed tangent frame as well.
    if (pkDesc->GetTANGENTS())
    {
        NIASSERT(pkTangentLow);
        NIASSERT(pkTangentHigh);

        pkLerpFloat3 = GetAttachableNodeFromLibrary("LerpFloat3AndNormalize");
        kContext.m_spConfigurator->AddNode(pkLerpFloat3);

        // Interpolate the tangent and put it into world space.
        kContext.m_spConfigurator->AddBinding(pkTangentHigh,
            pkLerpFloat3->GetInputResourceByVariableName("InputOne"));
        kContext.m_spConfigurator->AddBinding(pkTangentLow,
            pkLerpFloat3->GetInputResourceByVariableName("InputTwo"));
        kContext.m_spConfigurator->AddBinding(pkMorphValue,
            pkLerpFloat3->GetInputResourceByVariableName("Amount"));

        NiMaterialResource* pkFinalTangent = 
            pkLerpFloat3->GetOutputResourceByVariableName(
            "InterpolatedOutput");
        NIASSERT(pkFinalTangent);

        HandleWorldSpaceRotation(kContext, pkFinalTangent, pkFinalTangent,
            pkWorldMatrix);

        // Construct the bi-normal to complete the tangent frame.
        NiMaterialNode* pkCalcBinormal = GetAttachableNodeFromLibrary(
            "CalculateBinormal");
        kContext.m_spConfigurator->AddNode(pkCalcBinormal);

        kContext.m_spConfigurator->AddBinding(pkFinalNormal,
            pkCalcBinormal->GetInputResourceByVariableName("WorldNormal"));
        kContext.m_spConfigurator->AddBinding(pkFinalTangent,
            pkCalcBinormal->GetInputResourceByVariableName("WorldTangent"));

        NiMaterialResource* pkBinormal = 
            pkCalcBinormal->GetOutputResourceByVariableName("WorldBinormal");
        
        NIASSERT(pkBinormal);        

        // Send the tangent frame out to the pixel shader.        
        kContext.m_spConfigurator->AddBinding(pkBinormal, 
            kContext.m_spOutputs->AddInputResource("float3", "TexCoord", 
            "World", "WorldBinormal"));
        
        kContext.m_spConfigurator->AddBinding(pkFinalTangent, 
            kContext.m_spOutputs->AddInputResource("float3", "TexCoord", 
            "World", "WorldTangent"));
    }
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleInputDataSplits(
    NiFragmentMaterial::Context& kContext, 
    NiTerrainMaterialVertexDescriptor* pkDesc, 
    NiMaterialResource*& pkPosHigh, NiMaterialResource*& pkPosLow,
    NiMaterialResource*& pkNormalHigh, NiMaterialResource*& pkNormalLow,
    NiMaterialResource*& pkTangentHigh, NiMaterialResource*& pkTangentLow)
{
    NIASSERT(pkDesc);

    // Handle the split positions fragment.
    NiMaterialNode* pkSplitPositionFrag = GetAttachableNodeFromLibrary(
        "SplitPosition");
    kContext.m_spConfigurator->AddNode(pkSplitPositionFrag);        
    
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spInputs->GetOutputResourceByVariableName("vPosition"),
        pkSplitPositionFrag->GetInputResourceByVariableName("CombinedPosition")
    );

    pkPosHigh = pkSplitPositionFrag->GetOutputResourceByVariableName(
        "PositionHigh");
    pkPosLow = pkSplitPositionFrag->GetOutputResourceByVariableName(
        "PositionLow");

    NIASSERT(pkPosHigh && pkPosLow);

    // Handle the split normals fragment.
    NiMaterialNode* pkSplitNormalFrag = GetAttachableNodeFromLibrary(
        "SplitNormal");
    kContext.m_spConfigurator->AddNode(pkSplitNormalFrag);

    kContext.m_spConfigurator->AddBinding(
        kContext.m_spInputs->GetOutputResourceByVariableName("vNormal"),
        pkSplitNormalFrag->GetInputResourceByVariableName("CombinedNormal"));

    pkNormalHigh = pkSplitNormalFrag->GetOutputResourceByVariableName(
        "NormalHigh");
    pkNormalLow = pkSplitNormalFrag->GetOutputResourceByVariableName(
        "NormalLow");

    NIASSERT(pkNormalHigh && pkNormalLow);

    // Handle the split tangents if necessary.
    if (pkDesc->GetTANGENTS())
    {
        NiMaterialNode* pkSplitTangentFrag = GetAttachableNodeFromLibrary(
            "SplitTangent");        
        kContext.m_spConfigurator->AddNode(pkSplitTangentFrag);

        kContext.m_spConfigurator->AddBinding(
            kContext.m_spInputs->GetOutputResourceByVariableName("vTangent"),
            pkSplitTangentFrag->GetInputResourceByVariableName(
                "CombinedTangent"));

        pkTangentHigh = pkSplitTangentFrag->GetOutputResourceByVariableName(
            "TangentHigh");
        pkTangentLow = pkSplitTangentFrag->GetOutputResourceByVariableName(
            "TangentLow");

        NIASSERT(pkTangentHigh && pkTangentLow);
    }    
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleWorldSpaceTransformation(
    NiFragmentMaterial::Context& kContext,         
    NiMaterialResource*& pkWorldPos, NiMaterialResource* pkLocalPos,
    NiMaterialResource* pkWorldMatrix)
{
    NIASSERT(pkWorldMatrix);
    NIASSERT(pkLocalPos);

    NiMaterialNode* pkWorldFrag = GetAttachableNodeFromLibrary(
        "TransformPositionToWorld");

    kContext.m_spConfigurator->AddNode(pkWorldFrag);
    
    kContext.m_spConfigurator->AddBinding(pkLocalPos, 
        pkWorldFrag->GetInputResourceByVariableName("LocalPosition"));

    kContext.m_spConfigurator->AddBinding(pkWorldMatrix,
        pkWorldFrag->GetInputResourceByVariableName("WorldMatrix"));

    pkWorldPos = pkWorldFrag->GetOutputResourceByVariableName("WorldPos");  
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleWorldSpaceRotation(
    NiFragmentMaterial::Context& kContext,         
    NiMaterialResource*& pkWorldRot, NiMaterialResource* pkLocalRot,
    NiMaterialResource* pkWorldMatrix)
{
    NIASSERT(pkWorldMatrix);
    NIASSERT(pkLocalRot);

    NiMaterialNode* pkWorldFrag = GetAttachableNodeFromLibrary(
        "ApplyWorldRotation");

    kContext.m_spConfigurator->AddNode(pkWorldFrag);

    kContext.m_spConfigurator->AddBinding(pkLocalRot, 
        pkWorldFrag->GetInputResourceByVariableName("ValueToRotate"));

    kContext.m_spConfigurator->AddBinding(pkWorldMatrix,
        pkWorldFrag->GetInputResourceByVariableName("WorldMatrix"));

    pkWorldRot = pkWorldFrag->GetOutputResourceByVariableName(
        "WorldRotationOfValue"); 
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleCalculateFog(Context& kContext, 
    NiMaterialResource* pkViewPos, Fog eFogType)
{
    if (eFogType == FOG_NONE)
        return;

    NIASSERT(pkViewPos != NULL);

    // Bind fog resources
    NiMaterialNode* pkFogNode = GetAttachableNodeFromLibrary(
        "CalculateFog");
    kContext.m_spConfigurator->AddNode(pkFogNode);

    NiMaterialResource* pkFogOut = kContext.m_spOutputs->AddInputResource(
        "float", "Fog", "", "FogOut");
    kContext.m_spConfigurator->AddBinding(
        pkFogNode->GetOutputResourceByVariableName("FogOut"), pkFogOut);

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

    kContext.m_spConfigurator->AddBinding(pkViewPos, 
        pkFogNode->GetInputResourceByVariableName("ViewPosition"));
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleViewVector(NiFragmentMaterial::Context& kContext,
    NiTerrainMaterialVertexDescriptor* pkDesc, 
    NiMaterialResource*& pkViewVector, NiMaterialResource* pkWorldPos)
{
    NIASSERT(pkWorldPos);

    NiMaterialNode* pkCalcViewVectorFrag = GetAttachableNodeFromLibrary(
        "CalculateViewVector");
    kContext.m_spConfigurator->AddNode(pkCalcViewVectorFrag);

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

    // Also bind the world view vector as an output to the pixel shader.
    if (pkDesc->GetOUTPUT_WORLDVIEW())
    {
        NiMaterialResource* pkVertOutWorldView = 
            kContext.m_spOutputs->AddInputResource("float3", "TexCoord", 
                "World", "WorldView");
        kContext.m_spConfigurator->AddBinding(pkViewVector, 
            pkVertOutWorldView);
    }    
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleCalculateMorphValue(
    NiFragmentMaterial::Context& kContext,
    NiTerrainMaterialVertexDescriptor* pkDesc, 
    NiMaterialResource*& pkMorphValue)
{
    NiMaterialNode* pkMorphFrag = NULL;
    NiMaterialResource* pkStitchingRes = NULL;
    switch (pkDesc->GetMORPH_MODE())
    {
        case 3: // 3D Morphing
        {
            pkMorphFrag = GetAttachableNodeFromLibrary(
                "Calculate3DVertexMorph");
        }break;
        case 2: // 2.5D Morphing
        {
            pkMorphFrag = GetAttachableNodeFromLibrary(
                "Calculate25DVertexMorph");
        }break;
        case 1: // 2D Morphing
        {
            pkMorphFrag = GetAttachableNodeFromLibrary(
                "Calculate2DVertexMorph");
        }break;
        case 0: // No Morphing
        default:
        {
            pkMorphFrag = GetAttachableNodeFromLibrary(
                "CalculateNoVertexMorph");
            pkStitchingRes = AddOutputAttribute(
                kContext.m_spUniforms, STITCHINGINFO_SHADER_CONSTANT,
                NiShaderAttributeDesc::ATTRIB_TYPE_POINT4);
            NIASSERT(pkStitchingRes);
        }
    }
    NIASSERT(pkMorphFrag);
    kContext.m_spConfigurator->AddNode(pkMorphFrag);

    NiMaterialResource* pkLODThresholdDistance = AddOutputAttribute(
        kContext.m_spUniforms, LODTHRESHOLD_SHADER_CONSTANT,
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);
    NIASSERT(pkLODThresholdDistance);

    NiMaterialResource* pkLODMorphDistance = AddOutputAttribute(
        kContext.m_spUniforms, LODMORPHDISTANCE_SHADER_CONSTANT,
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);
    NIASSERT(pkLODMorphDistance);

    NiMaterialResource* pkCameraPos = AddOutputAttribute(
        kContext.m_spUniforms, "g_AdjustedEyePos",
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT3);
    NIASSERT(pkCameraPos);

    if (pkStitchingRes)
    {
        kContext.m_spConfigurator->AddBinding(pkStitchingRes,
            pkMorphFrag->GetInputResourceByVariableName("StitchingInfo"));
    }
    kContext.m_spConfigurator->AddBinding(pkLODThresholdDistance,
        pkMorphFrag->GetInputResourceByVariableName("LODThresholdDistance"));
    kContext.m_spConfigurator->AddBinding(pkLODMorphDistance,
        pkMorphFrag->GetInputResourceByVariableName("LODMorphDistance"));
    kContext.m_spConfigurator->AddBinding(pkCameraPos,
        pkMorphFrag->GetInputResourceByVariableName("TerrainCameraPos"));
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spInputs->GetOutputResourceByVariableName("vPosition"),
        pkMorphFrag->GetInputResourceByVariableName("LocalPos"));

    pkMorphValue = pkMorphFrag->GetOutputResourceByVariableName(
        "LODMorphValue");
    NIASSERT(pkMorphValue);
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::LerpPositionsAndApplyViewProjection(
    NiFragmentMaterial::Context& kContext,    
    NiMaterialResource* pkWorldPositionHigh, 
    NiMaterialResource* pkWorldPositionLow,
    NiMaterialResource* pkMorphValue,
    NiMaterialResource*& pkViewPos,
    bool bIncludeViewPos)
{    
    NIASSERT(pkMorphValue);
    NIASSERT(pkWorldPositionHigh);
    NIASSERT(pkWorldPositionLow);

    // Transform both the high and low vertex positions from world space to 
    // clip space.
    NiMaterialResource* pkClipSpacePositionHigh = NULL;
    NiMaterialResource* pkClipSpacePositionLow = NULL;    
    NiMaterialResource* pkViewProjMatrix = AddOutputPredefined(
        kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_VIEWPROJ, 4);

    if (bIncludeViewPos)
    {
        NiMaterialResource* pkViewMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_VIEW, 4);

        NiMaterialNode* pkTransformToViewPos = 
            GetAttachableNodeFromLibrary("TransformPositionToView");
        kContext.m_spConfigurator->AddNode(pkTransformToViewPos);

        kContext.m_spConfigurator->AddBinding(pkWorldPositionHigh,
            pkTransformToViewPos->GetInputResourceByVariableName(
            "WorldPosition"));

        kContext.m_spConfigurator->AddBinding(pkViewMatrix,
            pkTransformToViewPos->GetInputResourceByVariableName(
            "ViewMatrix"));

        pkViewPos = 
            pkTransformToViewPos->GetOutputResourceByVariableName("ViewPos");
    }

    NiMaterialNode* pkViewProjFrag = GetAttachableNodeFromLibrary(
        "TransformPositionToClip");
    kContext.m_spConfigurator->AddNode(pkViewProjFrag);

    kContext.m_spConfigurator->AddBinding(pkViewProjMatrix, 
        pkViewProjFrag->GetInputResourceByVariableName("ViewProjMatrix"));
    kContext.m_spConfigurator->AddBinding(pkWorldPositionHigh, 
        pkViewProjFrag->GetInputResourceByVariableName("WorldPosition"));

    pkClipSpacePositionHigh = pkViewProjFrag->GetOutputResourceByVariableName(
        "WorldProj");

    pkViewProjFrag = GetAttachableNodeFromLibrary("TransformPositionToClip");
    kContext.m_spConfigurator->AddNode(pkViewProjFrag);

    kContext.m_spConfigurator->AddBinding(pkViewProjMatrix, 
        pkViewProjFrag->GetInputResourceByVariableName("ViewProjMatrix"));
    kContext.m_spConfigurator->AddBinding(pkWorldPositionLow, 
        pkViewProjFrag->GetInputResourceByVariableName("WorldPosition"));

    pkClipSpacePositionLow = pkViewProjFrag->GetOutputResourceByVariableName(
        "WorldProj");

    NIASSERT(pkClipSpacePositionHigh);
    NIASSERT(pkClipSpacePositionLow);

    // Morph the clip space vertex position by the LOD morph value and add it 
    // to out vertex out structure.
    NiMaterialNode* pkLerp4Frag = GetAttachableNodeFromLibrary("LerpFloat4");
    kContext.m_spConfigurator->AddNode(pkLerp4Frag);

    kContext.m_spConfigurator->AddBinding(pkClipSpacePositionHigh,
        pkLerp4Frag->GetInputResourceByVariableName("InputOne"));

    kContext.m_spConfigurator->AddBinding(pkClipSpacePositionLow,
        pkLerp4Frag->GetInputResourceByVariableName("InputTwo"));

    kContext.m_spConfigurator->AddBinding(pkMorphValue,
        pkLerp4Frag->GetInputResourceByVariableName("Amount"));

    NiMaterialResource* pkMorphedClipSpacePosition = 
        pkLerp4Frag->GetOutputResourceByVariableName("InterpolatedOutput");
    NIASSERT(pkMorphedClipSpacePosition);

    NiMaterialResource* pkProjPos = kContext.m_spOutputs->AddInputResource(
        "float4", "Position", "World", "WorldProj");
    kContext.m_spConfigurator->AddBinding(pkMorphedClipSpacePosition, 
        pkProjPos);

    // Now we need to morph the world space vertex position and add that to the
    // output vertex structure.
    NiMaterialNode* pkLerpFloat4ToFloat3Frag = 
        GetAttachableNodeFromLibrary("LerpFloat4ToFloat3Result");
    kContext.m_spConfigurator->AddNode(pkLerpFloat4ToFloat3Frag);

    kContext.m_spConfigurator->AddBinding(pkWorldPositionHigh,
        pkLerpFloat4ToFloat3Frag->GetInputResourceByVariableName("InputOne"));
    kContext.m_spConfigurator->AddBinding(pkWorldPositionLow,
        pkLerpFloat4ToFloat3Frag->GetInputResourceByVariableName("InputTwo"));
    kContext.m_spConfigurator->AddBinding(pkMorphValue,
        pkLerpFloat4ToFloat3Frag->GetInputResourceByVariableName("Amount"));
}
//---------------------------------------------------------------------------
bool NiTerrainMaterial::GeneratePixelShadeTree(Context& kContext,
    NiGPUProgramDescriptor* pkDesc)
{
    NIASSERT(pkDesc->m_kIdentifier == "NiTerrainMaterialPixelDescriptor");
    NiTerrainMaterialPixelDescriptor* pkPixelDesc = 
        (NiTerrainMaterialPixelDescriptor*)pkDesc;

    kContext.m_spConfigurator->SetDescription(pkPixelDesc->ToString());

    // Add pixel in, pixel out, constants, and uniforms
    if (!AddDefaultMaterialNodes(kContext, pkDesc, 
        NiGPUProgram::PROGRAM_PIXEL))
    {
        return false;
    }
    
    kContext.m_spInputs->AddOutputResource("float4", "Position",
        "World",  "PosProjected");   

    // Create pixel out. Always just contains a single color.    
    NiMaterialResource* pkPixelOutColor = 
        kContext.m_spOutputs->AddInputResource("float4", "Color", "", 
        "Color0");    

    // Add specified texture coordinate sets.
    HandlePixelTexCoordSets(kContext, pkPixelDesc);

    // Vertex shader always sends at a minimum the interpolated world normal 
    // and world position. But we may also need the world view vector.
    NiMaterialResource* pkWorldView = NULL;
    if (pkPixelDesc->GetWORLD_VIEW())
    {
        NiMaterialNode* pkNormalizeVector = GetAttachableNodeFromLibrary(
            "NormalizeVector");
        kContext.m_spConfigurator->AddNode(pkNormalizeVector);
        
        kContext.m_spConfigurator->AddBinding(
            kContext.m_spInputs->AddOutputResource("float3", "TexCoord", 
            "World", "WorldView"), pkNormalizeVector->GetInputResource(0));

        pkWorldView = pkNormalizeVector->GetOutputResource(0);
    }    
    
    NiMaterialNode* pkNormalizeVector = GetAttachableNodeFromLibrary(
        "NormalizeVector");
    kContext.m_spConfigurator->AddNode(pkNormalizeVector);

    kContext.m_spConfigurator->AddBinding(
        kContext.m_spInputs->AddOutputResource("float3", "TexCoord", "World", 
            "WorldNormal"), pkNormalizeVector->GetInputResource(0));

    NiMaterialResource* pkWorldNormal = pkNormalizeVector->GetOutputResource(
        0);

    bool bHasWorldNBT = pkPixelDesc->GetWORLDNBT() == 1;
    if (bHasWorldNBT)
    {
        kContext.m_spInputs->AddOutputResource("float3", 
            "TexCoord", "World", "WorldBinormal");

        kContext.m_spInputs->AddOutputResource("float3", 
            "TexCoord", "World", "WorldTangent");
    }

    // Add the blend maps to the shader.
    NiMaterialResource* pkBlendMaps[MAX_LAYERS_PER_BLOCK / 4][4];
    NiMaterialResource* pkMaskAccum = 
        kContext.m_spStatics->AddOutputConstant("float", "(0)");
    HandleSampleBlendMaps(kContext, pkPixelDesc, pkBlendMaps, pkMaskAccum);
    NIASSERT(pkMaskAccum);

    NiMaterialResource* pkDiffuseAccum = NULL;
    NiMaterialResource* pkNormalAccum = NULL;
    NiUInt32 uiNumLayers = pkPixelDesc->GetNUM_LAYERS();
    if (uiNumLayers)
    {
        ProcessLayers(kContext, pkPixelDesc, pkDiffuseAccum, pkNormalAccum,
            pkBlendMaps, pkMaskAccum, pkWorldNormal, pkWorldView, 
            pkPixelDesc->GetBLENDMAP_COUNT());
    }
    
    // Get the light contribution from the sun.
    NiMaterialResource* pkDiffuseFactor = NULL;
    if (uiNumLayers)
    {
        HandleSunlight(kContext, pkPixelDesc, pkDiffuseFactor, pkNormalAccum, 
            pkWorldView);
    }    
    else
    {
        HandleSunlight(kContext, pkPixelDesc, pkDiffuseFactor, pkWorldNormal,
            pkWorldView);
    }

    // Composite the final color based on the accumulated values gathered 
    // while processing the layers.
    NiMaterialNode* pkFinalColorFrag = GetAttachableNodeFromLibrary(
        "CompositeFinalColor");
    kContext.m_spConfigurator->AddNode(pkFinalColorFrag);

    // Must at least have a base map color.
    if (pkDiffuseAccum)
    {
        kContext.m_spConfigurator->AddBinding(pkDiffuseAccum, 
            pkFinalColorFrag->GetInputResourceByVariableName("DiffuseColor"));
    }
    else
    {    
        kContext.m_spConfigurator->AddBinding(
            kContext.m_spStatics->AddOutputConstant("float3", 
            "(1.0, 1.0, 1.0)"),
            pkFinalColorFrag->GetInputResourceByVariableName("DiffuseColor"));
    }   
  
    if (pkDiffuseFactor)
    {
        kContext.m_spConfigurator->AddBinding(pkDiffuseFactor,
            pkFinalColorFrag->GetInputResourceByVariableName("DiffuseFactor"));
    }
    else
    {
        kContext.m_spConfigurator->AddBinding(
            kContext.m_spStatics->AddOutputConstant("float3", 
            "(1.0, 1.0, 1.0)"),
            pkFinalColorFrag->GetInputResourceByVariableName("DiffuseFactor"));
    }    
    
    NiMaterialResource* pkFinalColor = 
        pkFinalColorFrag->GetOutputResourceByVariableName("OutputColor");

    if (!HandleApplyFog(kContext, pkPixelDesc, pkFinalColor, pkFinalColor))
        return false;
 
    NIASSERT(pkFinalColor);    
    kContext.m_spConfigurator->AddBinding(pkFinalColor, pkPixelOutColor);
   
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainMaterial::HandleApplyFog(Context& kContext, 
    NiTerrainMaterialPixelDescriptor* pkPixelDesc,
    NiMaterialResource* pkUnfoggedColor, 
    NiMaterialResource*& pkFogOutput)
{
    // At this time, all other platforms require this fog calculation.
    if (pkPixelDesc->GetFOGTYPE() != FOG_NONE)
    {
        if (kContext.m_spConfigurator->GetPlatformString() == "DX9")
        {
            // DX9 uses HLSL with varying shader targets
            // Fog should only be applied by the pixel shader in 
            // SM 3.0 or greater
            NiGPUProgramCache* pkCache = 
                m_aspProgramCaches[NiGPUProgram::PROGRAM_PIXEL];
            NiFixedString kShaderTarget = pkCache->GetShaderProfile();
        
            // In SM 2.0, fog is applied automatically by the hardware, 
            // so we simply return true and do not apply fog in the 
            // pixel shader.
            if (strstr(kShaderTarget, "ps_2_"))
            {
                return true;
            }
        }

        NiMaterialNode* pkFogNode = 
            GetAttachableNodeFromLibrary("ApplyFog");
        kContext.m_spConfigurator->AddNode(pkFogNode);

        NiMaterialResource* pkFogResource = 
            kContext.m_spInputs->AddOutputResource("float", "Fog", "", 
            "FogDepth");

        kContext.m_spConfigurator->AddBinding(
            pkFogResource, 
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
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleSunlight(NiFragmentMaterial::Context& kContext, 
    NiTerrainMaterialPixelDescriptor* pkDesc, 
    NiMaterialResource*& pkDiffuseAccum,
    NiMaterialResource* pkPerPixelNormal,
    NiMaterialResource*)
{
    NIASSERT(pkPerPixelNormal);
    
    bool bEnableSunlight = pkDesc->GetENABLE_SUNLIGHT() == 1;    
    if (bEnableSunlight)
    {
        NiMaterialNode* pkLightNode = GetAttachableNodeFromLibrary(
            "AccumulateLight");
        kContext.m_spConfigurator->AddNode(pkLightNode);

        kContext.m_spConfigurator->AddBinding(pkPerPixelNormal,
            pkLightNode->GetInputResourceByVariableName("WorldNormal"));

        if (!pkDiffuseAccum)
        {
            kContext.m_spConfigurator->AddBinding(
                kContext.m_spStatics->AddOutputConstant("float3", 
                "(0.0, 0.0, 0.0)"), 
                pkLightNode->GetInputResourceByVariableName("DiffuseAccum"));
        }
        else
        {
            kContext.m_spConfigurator->AddBinding(pkDiffuseAccum,
                pkLightNode->GetInputResourceByVariableName("DiffuseAccum"));
        }

        NiMaterialResource* pkLightDirection = 
            AddOutputObject(kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_OBJ_WORLDDIRECTION, 
            NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT, 0, "Dir");

        NiMaterialResource* pkLightDiffuse = 
            AddOutputObject(kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_OBJ_DIFFUSE,
            NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT, 0, "Dir");

        NiMaterialResource* pkLightAmbient = 
            AddOutputObject(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_OBJ_AMBIENT,
            NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT, 0, "Dir");
        
        kContext.m_spConfigurator->AddBinding(pkLightDirection,
            pkLightNode->GetInputResourceByVariableName("LightDirection"));

        kContext.m_spConfigurator->AddBinding(pkLightDiffuse,
            pkLightNode->GetInputResourceByVariableName("LightDiffuse"));

        kContext.m_spConfigurator->AddBinding(pkLightAmbient, 
            pkLightNode->GetInputResourceByVariableName("LightAmbient"));

        pkDiffuseAccum = pkLightNode->GetOutputResourceByVariableName(
            "OutputDiffuse");
        NIASSERT(pkDiffuseAccum);
    }
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::ProcessLayers(NiFragmentMaterial::Context& kContext,
    NiTerrainMaterialPixelDescriptor* pkDesc,
    NiMaterialResource*& pkDiffuseAccum, 
    NiMaterialResource*& pkNormalAccum,
    NiMaterialResource* pkBlendMap[MAX_LAYERS_PER_BLOCK / 4][4],
    NiMaterialResource* pkMaskAccum,
    NiMaterialResource* pkInterpolatedWorldNormal,
    NiMaterialResource* pkWorldView,
    NiUInt32 uiSamplerStartIndex)
{
    NIASSERT(pkDesc);
    NIASSERT(pkInterpolatedWorldNormal);
    
    NiUInt32 uiNumLayers = pkDesc->GetNUM_LAYERS();
    NiMaterialResource* pkLayerUVSets[MAX_LAYERS_PER_BLOCK];
    NiMaterialResource* pkBlendMasks[MAX_LAYERS_PER_BLOCK];
    
    if (pkDesc->GetENABLE_SURFACEUVMODIFIERS())
    {
        // Access the UV Modifier Arrays:
        AddOutputAttribute(
            kContext.m_spUniforms, UVSURFACEUVMODIFIERARRAY_SHADER_CONSTANT,
            NiShaderAttributeDesc::ATTRIB_TYPE_POINT4, uiNumLayers);
    }

    NiMaterialResource* pkDistMapSampler = NULL;
    NiMaterialResource* pkDistMapStrength = NULL;
    if (pkDesc->SupportsDistributionMask())
    {
        // Add a sampler for our distribution map.
        NIASSERT(pkDesc->GetNUM_DIST_MAPS() <= 1);

        NiFixedString kSamplerName = GenerateSamplerName("DistributionMap", 0);
        pkDistMapSampler = AddTextureSampler(kContext, kSamplerName, 
            uiSamplerStartIndex);

        // This index pointed to either the distribution map or the first base map
        // in the first layer. If we have a distribution map, sample it and increment 
        // index to point at the first layer's texture.
        uiSamplerStartIndex++;

        pkDistMapStrength = AddOutputAttribute(kContext.m_spUniforms, 
            DISTMASKSTR_SHADER_CONSTANT, 
            NiShaderAttributeDesc::ATTRIB_TYPE_POINT4);
    }

    // First we need to loop through all the layers and calculate uv
    // coordinates for them.
    for (NiUInt32 uiCurrLayer = 0; uiCurrLayer < uiNumLayers; uiCurrLayer++)
    {
        // Make sure the blend map index is less than number of layers
        NIASSERT((uiCurrLayer / MAX_LAYERS_PER_BLOCK) < uiNumLayers);

        // Get our blend mask value.
        pkBlendMasks[uiCurrLayer] = pkBlendMap[0][uiCurrLayer];

        NiMaterialResource* pkUVSet = FindTextureUVSet(kContext,
            pkDesc->GetLayerUVIndex(uiCurrLayer));
        NIASSERT(pkUVSet);
        
        if (pkDesc->GetENABLE_SURFACEUVMODIFIERS())
        {
            // Modify this texture coordinate set according to this surface's 
            // scale and offset:
            NiMaterialResource* pkSurfaceScale = 0;
            NiMaterialResource* pkSurfaceOffset = 0;
            NiMaterialResource* pkOriginalUVSet = pkUVSet;
            HandlePixelAccessSurfaceUVModifiers(kContext, pkDesc, 
                uiCurrLayer, pkSurfaceScale, pkSurfaceOffset);
            NIASSERT(pkSurfaceScale);
            NIASSERT(pkSurfaceOffset);

            HandlePixelSurfaceTexCoordSets(kContext, pkOriginalUVSet, 
                pkSurfaceScale, pkSurfaceOffset, pkUVSet);
        }

        NIASSERT(pkUVSet);
        pkLayerUVSets[uiCurrLayer] = pkUVSet;
    }

    // Before we can sample any layer textures, we need to modulate our global
    // blend mask texture with any distribution maps from each layer.
    if (pkDesc->SupportsDistributionMask())
    {
        NiMaterialNode* pkCombineBlends =
            GetAttachableNodeFromLibrary("ComputeTotalMaskValue");
        kContext.m_spConfigurator->AddNode(pkCombineBlends);

        NIASSERT(pkDistMapStrength);
        kContext.m_spConfigurator->AddBinding(pkDistMapStrength,
            pkCombineBlends->GetInputResourceByVariableName(
            "DistStrength"));

        for (NiUInt32 uiCurrLayer = 0; uiCurrLayer < uiNumLayers; uiCurrLayer++)
        {
            // Get our per layer distribution mask value.
            NiMaterialResource* pkCurrDistMask;
            if (pkDesc->SupportsDistributionMask(uiCurrLayer))
            {
                NiMaterialNode* pkGetDistMaskValue = 
                    GetAttachableNodeFromLibrary("SampleSingleChannel");
                kContext.m_spConfigurator->AddNode(pkGetDistMaskValue);

                kContext.m_spConfigurator->AddBinding(
                    pkLayerUVSets[uiCurrLayer], 
                    pkGetDistMaskValue->GetInputResourceByVariableName(
                    "MapUV"));

                kContext.m_spConfigurator->AddBinding(pkDistMapSampler,
                    pkGetDistMaskValue->GetInputResourceByVariableName(
                    "Sampler"));

                char acIndexStr[64];
                NiSnprintf(acIndexStr, 64, NI_TRUNCATE, "(%d)", uiCurrLayer);
                kContext.m_spConfigurator->AddBinding(
                    kContext.m_spStatics->AddOutputConstant("int", acIndexStr),
                    pkGetDistMaskValue->GetInputResourceByVariableName(
                    "ChannelIndex"));

                pkCurrDistMask = 
                    pkGetDistMaskValue->GetOutputResourceByVariableName(
                    "Output");
                NIASSERT(pkCurrDistMask);
            }
            else
            {
                pkCurrDistMask = NULL;
            }

            char acGMaskName[64];
            char acLMaskName[64];
            char acOutputMaskName[64];

            NiSnprintf(acGMaskName, 64, NI_TRUNCATE, "gMask%d", uiCurrLayer);
            NiSnprintf(acLMaskName, 64, NI_TRUNCATE, "lMask%d", uiCurrLayer);
            NiSnprintf(acOutputMaskName, 64, NI_TRUNCATE, "gMask%d_Out", uiCurrLayer);

            if (pkBlendMasks[uiCurrLayer])
            {
                kContext.m_spConfigurator->AddBinding(pkBlendMasks[uiCurrLayer],
                    pkCombineBlends->GetInputResourceByVariableName(
                    acGMaskName));
            }

            if (pkCurrDistMask)
            {
                kContext.m_spConfigurator->AddBinding(pkCurrDistMask,
                    pkCombineBlends->GetInputResourceByVariableName(
                    acLMaskName));
            }

            pkBlendMasks[uiCurrLayer] = pkBlendMasks[uiCurrLayer] != NULL ? 
                pkCombineBlends->GetOutputResourceByVariableName(
                acOutputMaskName) : NULL;
        }
    }

    // Accumulate any parallax map values.
    NiMaterialResource* pkParallaxAccum = NULL;
    for (NiUInt32 uiCurrLayer = 0; uiCurrLayer < uiNumLayers; uiCurrLayer++)
    {
        NiUInt32 uiLayerBaseSamplerIndex = (uiCurrLayer * 3) + 
            uiSamplerStartIndex;

        // Sample any parallax maps using the original UV coordinates.
        if (pkDesc->SupportsParallaxMap(uiCurrLayer))
        {
            HandleParallaxMap(kContext, pkDesc, pkParallaxAccum, 
                pkBlendMasks[uiCurrLayer], pkLayerUVSets[uiCurrLayer], 
                uiCurrLayer, uiLayerBaseSamplerIndex + 1);
        }
    }

    // Now that we have taken the parallax maps into account, loop through
    // the layers again and sample the remaining textures.
    for (NiUInt32 uiCurrLayer = 0; uiCurrLayer < uiNumLayers; uiCurrLayer++)
    {
        NiUInt32 uiLayerBaseSamplerIndex = (uiCurrLayer * 3) + 
            uiSamplerStartIndex;

        // If this layer supports normal map or we have an accumulated 
        // parallax value we need a tangent frame.
        NiMaterialResource* pkWorldTangent = NULL;
        NiMaterialResource* pkWorldBinormal = NULL;

        ComputeTangentFrame(kContext, pkDesc, pkWorldTangent, pkWorldBinormal,
            pkLayerUVSets[uiCurrLayer], pkInterpolatedWorldNormal);

        // First if we have an accumulated parallax map value use it to perturb
        // the uv set for this layer.
        if (pkParallaxAccum)
        {
            CalculateParallaxOffset(kContext, pkParallaxAccum, pkWorldView, 
                pkWorldTangent, pkWorldBinormal, pkInterpolatedWorldNormal,
                pkLayerUVSets[uiCurrLayer]);
        }

        // Handle the base map and detail map.
        HandleBaseMap(kContext, pkDesc, pkDiffuseAccum, 
            pkBlendMasks[uiCurrLayer], pkLayerUVSets[uiCurrLayer],
            uiCurrLayer, uiLayerBaseSamplerIndex);

        // If we have a normal map, use the normal map, else use the
        // interpolated vertex normal for the normal contribution for the 
        // layer.
        HandleNormalMap(kContext, pkDesc, pkNormalAccum, 
            pkBlendMasks[uiCurrLayer], pkInterpolatedWorldNormal, 
            pkWorldBinormal, pkWorldTangent,
            pkLayerUVSets[uiCurrLayer], uiCurrLayer, 
            uiLayerBaseSamplerIndex + 1);
    }

    NiMaterialResource* pkOneMinusMaskAccum = NULL;
    
    NiMaterialNode* pkInverseRatio = GetAttachableNodeFromLibrary(
        "InverseRatio");
    NIASSERT(pkInverseRatio);    
    kContext.m_spConfigurator->AddNode(pkInverseRatio);
    kContext.m_spConfigurator->AddBinding(pkMaskAccum,
        pkInverseRatio->GetInputResourceByVariableName("Ratio"));
    pkOneMinusMaskAccum = 
        pkInverseRatio->GetOutputResourceByVariableName("Output");
    NIASSERT(pkOneMinusMaskAccum);

    // Blend the leftover diffuse values with white:
    NiMaterialNode* pkBlendWhite = GetAttachableNodeFromLibrary(
        "CalcBlendRGBAndAccumulate");
    NIASSERT(pkBlendWhite);    
    kContext.m_spConfigurator->AddNode(pkBlendWhite);
    
    kContext.m_spConfigurator->AddBinding(pkOneMinusMaskAccum,
        pkBlendWhite->GetInputResourceByVariableName("Mask"));
    kContext.m_spConfigurator->AddBinding(pkDiffuseAccum,
        pkBlendWhite->GetInputResourceByVariableName("AccumColor"));
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("float3", "(1,1,1)"),
        pkBlendWhite->GetInputResourceByVariableName("InputColor"));

    pkDiffuseAccum = 
        pkBlendWhite->GetOutputResourceByVariableName("OutputColor");

    // Blend the leftover normal values with interpolated world normal:
    NiMaterialNode* pkBlendWhiteNormal = GetAttachableNodeFromLibrary(
        "CalcBlendRGBAndAccumulate");
    NIASSERT(pkBlendWhiteNormal);    
    kContext.m_spConfigurator->AddNode(pkBlendWhiteNormal);
    
    kContext.m_spConfigurator->AddBinding(pkOneMinusMaskAccum,
        pkBlendWhiteNormal->GetInputResourceByVariableName("Mask"));
    kContext.m_spConfigurator->AddBinding(pkNormalAccum,
        pkBlendWhiteNormal->GetInputResourceByVariableName("AccumColor"));
    kContext.m_spConfigurator->AddBinding(
        pkInterpolatedWorldNormal,
        pkBlendWhiteNormal->GetInputResourceByVariableName("InputColor"));

    pkNormalAccum = 
        pkBlendWhiteNormal->GetOutputResourceByVariableName("OutputColor");

    // Normalize the resulting accumulated per pixel normal.
    NiMaterialNode* pkNormalizeNode = GetAttachableNodeFromLibrary(
        "NormalizeVector");

    kContext.m_spConfigurator->AddNode(pkNormalizeNode);
    
    kContext.m_spConfigurator->AddBinding(pkNormalAccum,
        pkNormalizeNode->GetInputResourceByVariableName("Input"));
    
    pkNormalAccum = pkNormalizeNode->GetOutputResourceByVariableName("Output");
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::CalculateParallaxOffset(Context& kContext,
    NiMaterialResource* pkParallaxValue, NiMaterialResource* pkWorldView,
    NiMaterialResource* pkWorldTangent, NiMaterialResource* pkWorldBinormal,
    NiMaterialResource* pkWorldNormal, NiMaterialResource*& pkUVSet)
{
    // First put the view vector into tangent space.
    NiMaterialNode* pkWorldToTangent = GetAttachableNodeFromLibrary(
        "WorldToTangent");
    kContext.m_spConfigurator->AddNode(pkWorldToTangent);

    kContext.m_spConfigurator->AddBinding(pkWorldNormal,
    pkWorldToTangent->GetInputResourceByVariableName("WorldNormal"));

    kContext.m_spConfigurator->AddBinding(pkWorldTangent,
    pkWorldToTangent->GetInputResourceByVariableName("WorldTangent"));

    kContext.m_spConfigurator->AddBinding(pkWorldBinormal,
    pkWorldToTangent->GetInputResourceByVariableName("WorldBinormal"));

    kContext.m_spConfigurator->AddBinding(pkWorldView,
    pkWorldToTangent->GetInputResourceByVariableName("VectorIn"));

    NiMaterialResource* pkTangentSpaceWorldView = 
    pkWorldToTangent->GetOutputResourceByVariableName("VectorOut");

    // Now perturb the texture coordinate for this layer using the accumulated
    // height value.
    NiMaterialNode* pkComputeParallaxOffset = NULL;
    
    pkComputeParallaxOffset = GetAttachableNodeFromLibrary(
        "CalculateParallaxOffset");
    kContext.m_spConfigurator->AddNode(pkComputeParallaxOffset);

    kContext.m_spConfigurator->AddBinding(pkUVSet,
    pkComputeParallaxOffset->GetInputResourceByVariableName(
        "TexCoord"));

    kContext.m_spConfigurator->AddBinding(pkTangentSpaceWorldView,
    pkComputeParallaxOffset->GetInputResourceByVariableName(
        "TangentSpaceEyeVec"));

    kContext.m_spConfigurator->AddBinding(pkParallaxValue,
    pkComputeParallaxOffset->GetInputResourceByVariableName("Height"));

    pkUVSet = pkComputeParallaxOffset->GetOutputResourceByVariableName(
        "ParallaxOffsetUV");
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleParallaxMap(Context& kContext,
    NiTerrainMaterialPixelDescriptor*,
    NiMaterialResource*& pkParallaxAccum, 
    NiMaterialResource* pkBlendMask,
    NiMaterialResource* pkUVSet, NiUInt32 uiCurrLayer, NiUInt32 uiSamplerIndex)
{
    NiMaterialResource* pkMapSampler = NULL;
    NiMaterialResource* pkParallaxValue = NULL;

    NiMaterialNode* pkSampleParallaxMap =  
        GetAttachableNodeFromLibrary("SampleLayerTextureAlpha");

    kContext.m_spConfigurator->AddNode(pkSampleParallaxMap);

    // Use the normal map since that's where the parallax map is stored.
    NiFixedString kSamplerName = GenerateSamplerName("NormalMap",
        uiCurrLayer);

    pkMapSampler = AddTextureSampler(kContext, kSamplerName, 
        uiSamplerIndex);

    NIASSERT(pkMapSampler);

    kContext.m_spConfigurator->AddBinding(pkUVSet, 
        pkSampleParallaxMap->GetInputResourceByVariableName("MapUV"));

    kContext.m_spConfigurator->AddBinding(pkMapSampler,
        pkSampleParallaxMap->GetInputResourceByVariableName("Sampler"));

    pkParallaxValue = pkSampleParallaxMap->GetOutputResourceByVariableName(
        "OutputAlpha");
    NIASSERT(pkParallaxValue);

    // Accumulate the parallax value.
    NiMaterialNode* pkBlendNode = GetAttachableNodeFromLibrary(
        "CalcBlendFloatAndAccumulate");
    kContext.m_spConfigurator->AddNode(pkBlendNode);

    if (!pkParallaxAccum)
    {
        kContext.m_spConfigurator->AddBinding(
            kContext.m_spStatics->AddOutputConstant("float", "(0)"),
            pkBlendNode->GetInputResourceByVariableName("AccumValue"));
    }
    else
    {
        kContext.m_spConfigurator->AddBinding(pkParallaxAccum,
            pkBlendNode->GetInputResourceByVariableName("AccumValue"));
    }

    kContext.m_spConfigurator->AddBinding(pkParallaxValue,
        pkBlendNode->GetInputResourceByVariableName("Input"));

    kContext.m_spConfigurator->AddBinding(pkBlendMask,
        pkBlendNode->GetInputResourceByVariableName("Mask"));
        

    pkParallaxAccum = pkBlendNode->GetOutputResourceByVariableName("Output");
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleBaseMap(NiFragmentMaterial::Context& kContext, 
    NiTerrainMaterialPixelDescriptor* pkDesc,
    NiMaterialResource*& pkDiffuseAccum, NiMaterialResource* pkBlendMask,
    NiMaterialResource* pkUVSet, NiUInt32 uiCurrLayer, NiUInt32 uiSamplerIndex)
{
    NIASSERT(pkBlendMask);
    NIASSERT(pkUVSet);
    NIASSERT(pkDesc);

    bool bSupportsDetailMap = pkDesc->SupportsDetailMap(uiCurrLayer);
    NiMaterialResource* pkDetailMapUV = NULL;

    if (bSupportsDetailMap)
    {
        NiMaterialNode* pkScaleUV = GetAttachableNodeFromLibrary(
            "ScaleUVForDetailMap");
        kContext.m_spConfigurator->AddNode(pkScaleUV);

        kContext.m_spConfigurator->AddBinding(pkUVSet,
            pkScaleUV->GetInputResourceByVariableName("MapUV"));

        pkDetailMapUV = pkScaleUV->GetOutputResourceByVariableName(
            "OutputUV");
        NIASSERT(pkDetailMapUV);
    }
    
    // Sample the base map and possibly the detail map for this layer.    
    NiMaterialNode* pkSampleBaseMapTexture = NULL;
    if (bSupportsDetailMap)
    {
        pkSampleBaseMapTexture = GetAttachableNodeFromLibrary(
            "SampleBaseMapWithDetailMap");
    }
    else
    {
        pkSampleBaseMapTexture = GetAttachableNodeFromLibrary(
            "SampleLayerTextureRGB");
    }    
    
    kContext.m_spConfigurator->AddNode(pkSampleBaseMapTexture);

    NiFixedString kSamplerName = GenerateSamplerName("BaseMap", uiCurrLayer);
    NiMaterialResource* pkMapSampler = AddTextureSampler(kContext, 
        kSamplerName, uiSamplerIndex);
    NIASSERT(pkMapSampler);

    kContext.m_spConfigurator->AddBinding(pkUVSet, 
        pkSampleBaseMapTexture->GetInputResourceByVariableName("MapUV"));
    kContext.m_spConfigurator->AddBinding(pkMapSampler, 
        pkSampleBaseMapTexture->GetInputResourceByVariableName("Sampler"));
    
    if (bSupportsDetailMap)
    {
        kContext.m_spConfigurator->AddBinding(pkDetailMapUV, 
            pkSampleBaseMapTexture->GetInputResourceByVariableName(
            "DetailMapUV"));
    }

    NiMaterialResource* pkBaseMapColor = 
        pkSampleBaseMapTexture->GetOutputResourceByVariableName("OutputColor");
    NIASSERT(pkBaseMapColor);

    // Multiply the base map color and possibly the detail map value by the 
    // blend mask to get the value for the current layer.
    NiMaterialNode* pkBlendNode = NULL;
    if (bSupportsDetailMap)
    {
        pkBlendNode = GetAttachableNodeFromLibrary(
            "CalcBlendBaseMapWithDetailMap");
    }
    else
    {
        pkBlendNode = GetAttachableNodeFromLibrary(
            "CalcBlendRGBAndAccumulate");
    }
    
    kContext.m_spConfigurator->AddNode(pkBlendNode);

    kContext.m_spConfigurator->AddBinding(pkBlendMask, 
        pkBlendNode->GetInputResourceByVariableName("Mask"));
    kContext.m_spConfigurator->AddBinding(pkBaseMapColor,
        pkBlendNode->GetInputResourceByVariableName("InputColor"));

    // If this is the first layer then we want set a color of black as the 
    // initial accumulated color, else set it to the last accumulated color.
    if (!pkDiffuseAccum)
    {
        kContext.m_spConfigurator->AddBinding(
            kContext.m_spStatics->AddOutputConstant("float3", "(0,0,0)"),
            pkBlendNode->GetInputResourceByVariableName("AccumColor"));
    }
    else
    {
        kContext.m_spConfigurator->AddBinding(pkDiffuseAccum,
            pkBlendNode->GetInputResourceByVariableName("AccumColor"));
    }

    pkDiffuseAccum = pkBlendNode->GetOutputResourceByVariableName(
        "OutputColor");
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleNormalMap(
    NiFragmentMaterial::Context& kContext,
    NiTerrainMaterialPixelDescriptor* pkDesc,
    NiMaterialResource*& pkNormalAccum,
    NiMaterialResource* pkBlendMask,
    NiMaterialResource* pkWorldNormal,
    NiMaterialResource* pkWorldBinormal,
    NiMaterialResource* pkWorldTangent,
    NiMaterialResource* pkUVSet,
    NiUInt32 uiCurrLayer,
    NiUInt32 uiSamplerIndex)
{
    NIASSERT(uiCurrLayer < MAX_LAYERS_PER_BLOCK);
    
    NiMaterialResource* pkMapSampler = NULL;
    NiMaterialResource* pkTangentSpaceNormal = NULL;
    NiMaterialResource* pkPerPixelNormal = NULL;

    if (pkDesc->SupportsNormalMap(uiCurrLayer))
    {
        NIASSERT(pkWorldNormal);
        NIASSERT(pkWorldTangent);
        NIASSERT(pkWorldBinormal);
        NIASSERT(pkDesc);
        NIASSERT(pkBlendMask);
        NIASSERT(pkUVSet);    

        NiMaterialNode* pkSampleNormalMapTexture = 
            GetAttachableNodeFromLibrary("SampleLayerTextureRGB");

        kContext.m_spConfigurator->AddNode(pkSampleNormalMapTexture);

        NiFixedString kSamplerName = GenerateSamplerName("NormalMap",
            uiCurrLayer);

        pkMapSampler = AddTextureSampler(kContext, kSamplerName, 
            uiSamplerIndex);
        NIASSERT(pkMapSampler);

        kContext.m_spConfigurator->AddBinding(pkUVSet, 
            pkSampleNormalMapTexture->GetInputResourceByVariableName(
            "MapUV"));

        kContext.m_spConfigurator->AddBinding(pkMapSampler,
            pkSampleNormalMapTexture->GetInputResourceByVariableName(
            "Sampler"));

        pkTangentSpaceNormal = 
            pkSampleNormalMapTexture->GetOutputResourceByVariableName(
            "OutputColor");
        NIASSERT(pkTangentSpaceNormal);

        // Put the tangent space normal into world space before we accumulate.
        ConvertTangentSpaceNormalToWorldSpace(kContext, pkDesc, 
            pkPerPixelNormal, pkWorldTangent, pkWorldBinormal, pkWorldNormal,
            pkTangentSpaceNormal);
    }
    else
    {
        pkPerPixelNormal = pkWorldNormal;
    }

    // Multiply the resulting normal  by the blend mask to get the values for the 
    // current layer.
    NiMaterialNode* pkBlendNode = GetAttachableNodeFromLibrary(
        "CalcBlendRGBAndAccumulate");
    kContext.m_spConfigurator->AddNode(pkBlendNode);

    kContext.m_spConfigurator->AddBinding(pkBlendMask, 
        pkBlendNode->GetInputResourceByVariableName("Mask"));
    kContext.m_spConfigurator->AddBinding(pkPerPixelNormal,
        pkBlendNode->GetInputResourceByVariableName("InputColor"));

    if (!pkNormalAccum)
    {
        kContext.m_spConfigurator->AddBinding(
            kContext.m_spStatics->AddOutputConstant("float3", "(0,0,0)"),
            pkBlendNode->GetInputResourceByVariableName("AccumColor"));
    }
    else
    {
        kContext.m_spConfigurator->AddBinding(pkNormalAccum,
            pkBlendNode->GetInputResourceByVariableName("AccumColor"));
    }

    pkNormalAccum = pkBlendNode->GetOutputResourceByVariableName(
        "OutputColor");
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::ComputeTangentFrame(
    NiFragmentMaterial::Context& kContext, 
    NiTerrainMaterialPixelDescriptor* pkDesc, 
    NiMaterialResource*& pkWorldTangent,
    NiMaterialResource*& pkWorldBinormal,    
    NiMaterialResource* pkUVSet,
    NiMaterialResource*)
{
    NI_UNUSED_ARG(pkDesc);
    NI_UNUSED_ARG(pkUVSet);
    NIASSERT(pkDesc);
    NIASSERT(pkUVSet);

    if (!pkDesc->GetWORLDNBT())
        return;
    
    pkWorldTangent = kContext.m_spInputs->GetOutputResourceByVariableName(
        "WorldTangent");

    pkWorldBinormal = kContext.m_spInputs->GetOutputResourceByVariableName(
        "WorldBinormal");

    // Make sure they are normalized.
    NiMaterialNode* pkNormalizeVector = GetAttachableNodeFromLibrary(
        "NormalizeVector");
    kContext.m_spConfigurator->AddNode(pkNormalizeVector);
    
    kContext.m_spConfigurator->AddBinding(pkWorldTangent,
        pkNormalizeVector->GetInputResource(0));
    pkWorldTangent = pkNormalizeVector->GetOutputResource(0);

    pkNormalizeVector = GetAttachableNodeFromLibrary("NormalizeVector");
    kContext.m_spConfigurator->AddNode(pkNormalizeVector);

    kContext.m_spConfigurator->AddBinding(pkWorldBinormal,
        pkNormalizeVector->GetInputResource(0));
    pkWorldBinormal = pkNormalizeVector->GetOutputResource(0);

    NIASSERT(pkWorldTangent);
    NIASSERT(pkWorldBinormal);
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::ConvertTangentSpaceNormalToWorldSpace(
    NiFragmentMaterial::Context& kContext, 
    NiTerrainMaterialPixelDescriptor* pkDesc, 
    NiMaterialResource*& pkWorldSpaceNormalFromTangentSpace,
    NiMaterialResource* pkWorldTangent,
    NiMaterialResource* pkWorldBinormal,
    NiMaterialResource* pkWorldNormal,
    NiMaterialResource* pkTangentSpaceNormal)  
{
    NI_UNUSED_ARG(pkDesc);
    NIASSERT(pkTangentSpaceNormal);    
    NIASSERT(pkWorldBinormal);
    NIASSERT(pkWorldTangent);
    NIASSERT(pkWorldNormal);
    NIASSERT(pkDesc);
    
    NiMaterialNode* pkCalcNormalFromColor = GetAttachableNodeFromLibrary(
        "CalculateNormalFromColor");
    kContext.m_spConfigurator->AddNode(pkCalcNormalFromColor);

    // The tangent space normal is actually in RGB form at this point.
    kContext.m_spConfigurator->AddBinding(pkTangentSpaceNormal,
        pkCalcNormalFromColor->GetInputResourceByVariableName("NormalMap"));

    kContext.m_spConfigurator->AddBinding(pkWorldNormal, 
        pkCalcNormalFromColor->GetInputResourceByVariableName(
        "WorldNormal"));

    kContext.m_spConfigurator->AddBinding(pkWorldTangent,
        pkCalcNormalFromColor->GetInputResourceByVariableName(
        "WorldTangent"));

    kContext.m_spConfigurator->AddBinding(pkWorldBinormal,
        pkCalcNormalFromColor->GetInputResourceByVariableName(
        "WorldBinormal"));

    pkWorldSpaceNormalFromTangentSpace = 
        pkCalcNormalFromColor->GetOutputResourceByVariableName(
        "WorldNormalOut");   
}
//---------------------------------------------------------------------------
NiMaterialResource* NiTerrainMaterial::FindTextureUVSet(
    const NiFragmentMaterial::Context& kContext, NiUInt32 uiUVIndex)
{   
    // Create the name based on the specified UV index.
    char acVarName[32];
    NiSprintf(acVarName, 32, "UVSet%d", uiUVIndex);        

    return kContext.m_spInputs->GetOutputResourceByVariableName(acVarName);
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandleSampleBlendMaps(
    NiFragmentMaterial::Context& kContext,
    NiTerrainMaterialPixelDescriptor* pkDesc,
    NiMaterialResource* pkBlendMapOutputs[MAX_LAYERS_PER_BLOCK / 4][4],
    NiMaterialResource*& pkMaskAccum)
{
    NIASSERT(pkDesc);

    NiUInt32 uiNumBlendMaps = pkDesc->GetBLENDMAP_COUNT();
    NiUInt32 uiNumLayers = pkDesc->GetNUM_LAYERS();

    NIASSERT(uiNumLayers <= MAX_LAYERS_PER_BLOCK);    
    NIASSERT(((uiNumBlendMaps * 4) <= MAX_LAYERS_PER_BLOCK));

    NiUInt32 uiCurrLayer = 0;
    for (NiUInt32 ui = 0; ui < uiNumBlendMaps; ui++)
    {        
        NiMaterialNode* pkSampleBlendMap = GetAttachableNodeFromLibrary(
            "SampleBlendMap");

        kContext.m_spConfigurator->AddNode(pkSampleBlendMap);        
        
        NiMaterialResource* pkUVSet = FindTextureUVSet(kContext, 
            pkDesc->GetBlendMapUVIndex(ui));
        NIASSERT(pkUVSet);

        NiFixedString kSamplerName = GenerateSamplerName("BlendMap", ui);
        NiMaterialResource* pkSampler = AddTextureSampler(kContext, 
            kSamplerName, ui);
        NIASSERT(pkSampler);

        if (pkUVSet)
        {
            kContext.m_spConfigurator->AddBinding(pkUVSet,
                pkSampleBlendMap->GetInputResourceByVariableName("MapUV"));
        }

        kContext.m_spConfigurator->AddBinding(pkSampler,
            pkSampleBlendMap->GetInputResourceByVariableName("Sampler"));

        NiMaterialNode* pkAccumMasks = 
            GetAttachableNodeFromLibrary("CalcBlendFloatAndAccumulate");
        NIASSERT(pkAccumMasks);
        kContext.m_spConfigurator->AddNode(pkAccumMasks); 
        kContext.m_spConfigurator->AddBinding(pkMaskAccum,
            pkAccumMasks->GetInputResourceByVariableName(
                "AccumValue"));
        kContext.m_spConfigurator->AddBinding(
            pkSampleBlendMap->GetOutputResourceByVariableName("TotalMask"),
            pkAccumMasks->GetInputResourceByVariableName(
                "Mask"));
        kContext.m_spConfigurator->AddBinding(
            kContext.m_spStatics->AddOutputConstant("float", "(1)"),
            pkAccumMasks->GetInputResourceByVariableName(
                "Input"));
        pkMaskAccum = pkAccumMasks->GetOutputResourceByVariableName("Output");

        // Map output color to a material resource. One for each channel, but 
        // stop if we have reached the number of layers since there may not be
        // enough layers to match the total number of blend map channels since 
        // each blend map can do up to four layers.
        pkBlendMapOutputs[ui][0] = (uiCurrLayer++ < uiNumLayers) ? 
            pkSampleBlendMap->GetOutputResourceByVariableName("OutputRed") :
                NULL;

        pkBlendMapOutputs[ui][1] = (uiCurrLayer++ < uiNumLayers) ? 
            pkSampleBlendMap->GetOutputResourceByVariableName("OutputGreen") :
                NULL;

        pkBlendMapOutputs[ui][2] = (uiCurrLayer++ < uiNumLayers) ? 
            pkSampleBlendMap->GetOutputResourceByVariableName("OutputBlue") :
                NULL;

        pkBlendMapOutputs[ui][3] = (uiCurrLayer++ < uiNumLayers) ? 
            pkSampleBlendMap->GetOutputResourceByVariableName("OutputAlpha") :
                NULL;
    }
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandlePixelTexCoordSets(
    NiFragmentMaterial::Context& kContext,
    NiTerrainMaterialPixelDescriptor* pkDesc)
{
    NIASSERT(pkDesc);

    NiUInt32 uiUVSets = pkDesc->GetNUM_TEXCOORDS();
    for (NiUInt32 ui = 0; ui < uiUVSets; ui++)
    {
        // Create a unique name for the texture set.
        char acVarName[32];
        NiSprintf(acVarName, 32, "UVSet%d", ui);        

        // Add to vertex input.
        kContext.m_spInputs->AddOutputResource("float4", "TexCoord", "", 
            acVarName);
    }
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandlePixelAccessSurfaceUVModifiers(Context& kContext,
    NiTerrainMaterialPixelDescriptor* pkDesc, NiUInt32 uiCurrLayer, 
    NiMaterialResource*& pkSurfaceScale, NiMaterialResource*& pkSurfaceOffset)
{
    NiMaterialNode* pkAccessUVModifier = NULL;
    if (pkDesc->GetNUM_LAYERS() != 1)
    {
        // Get the node used to access the UV Offset array:
        pkAccessUVModifier = GetAttachableNodeFromLibrary(
            "AccessSurfaceUVModifiersArray");
        kContext.m_spConfigurator->AddNode(pkAccessUVModifier);
        
        // Get the resource for the array:
        NiMaterialResource* pkUVModifierArray = 0;
        pkUVModifierArray = 
            kContext.m_spUniforms->GetOutputResourceByVariableName(
            UVSURFACEUVMODIFIERARRAY_SHADER_CONSTANT);
        NIASSERT(pkUVModifierArray);

        NiMaterialResource* pkIndex = 0;
        NiFixedString kIndexValue = NiFixedString(NiString("(") + 
            NiString::FromUInt(uiCurrLayer)+NiString(")"));

        pkIndex = kContext.m_spStatics->AddOutputConstant(
            "int",
            kIndexValue);
        NIASSERT(pkIndex);

        // Perform the binding:
        NiMaterialResource* pkModifierArrayInput = pkAccessUVModifier->
            GetInputResourceByVariableName("UVModifierArray");
        pkModifierArrayInput->SetCount(pkUVModifierArray->GetCount());
        kContext.m_spConfigurator->AddBinding(pkUVModifierArray, 
            pkModifierArrayInput);

        kContext.m_spConfigurator->AddBinding(pkIndex,
            pkAccessUVModifier->GetInputResourceByVariableName("Index"));
    }
    else
    {
        // Get the node used to access the UV Offset array:
        pkAccessUVModifier = GetAttachableNodeFromLibrary(
            "AccessSurfaceUVModifiers");
        kContext.m_spConfigurator->AddNode(pkAccessUVModifier);

        // Get the resource for the array:
        NiMaterialResource* pkUVModifierArray = 0;
        pkUVModifierArray = 
            kContext.m_spUniforms->GetOutputResourceByVariableName(
            UVSURFACEUVMODIFIERARRAY_SHADER_CONSTANT);
        NIASSERT(pkUVModifierArray);

        // Perform the binding:
        NiMaterialResource* pkModifierArrayInput = pkAccessUVModifier->
            GetInputResourceByVariableName("UVModifierArray");
        pkModifierArrayInput->SetCount(pkUVModifierArray->GetCount());
        kContext.m_spConfigurator->AddBinding(pkUVModifierArray, 
            pkModifierArrayInput);
    }

    // Get the outputs:
    pkSurfaceScale = pkAccessUVModifier->GetOutputResourceByVariableName(
        "OutputScale");
    pkSurfaceOffset = pkAccessUVModifier->GetOutputResourceByVariableName(
        "OutputOffset");
    NIASSERT(pkSurfaceScale);
    NIASSERT(pkSurfaceOffset);
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::HandlePixelSurfaceTexCoordSets(
    NiFragmentMaterial::Context& kContext,
    NiMaterialResource* pkOriginalUV,
    NiMaterialResource* pkScale,
    NiMaterialResource* pkOffset,
    NiMaterialResource*& pkUVSet)
{
    NIASSERT(pkOriginalUV);
    NIASSERT(pkScale);
    NIASSERT(pkOffset);

    // Get the appropriate UV Modifier fragment:
    NiMaterialNode* pkScaleOffsetFrag = GetAttachableNodeFromLibrary(
        "CalcSurfaceUVSet_ScaleAndOffset");
    kContext.m_spConfigurator->AddNode(pkScaleOffsetFrag);

    // Bind the inputs and outputs:
    kContext.m_spConfigurator->AddBinding(pkOriginalUV,
        pkScaleOffsetFrag->GetInputResourceByVariableName("InputUV"));
    kContext.m_spConfigurator->AddBinding(pkScale,
        pkScaleOffsetFrag->GetInputResourceByVariableName("Scale"));
    kContext.m_spConfigurator->AddBinding(pkOffset,
        pkScaleOffsetFrag->GetInputResourceByVariableName("Offset"));

    // Supply the output connector:
    pkUVSet = pkScaleOffsetFrag->GetOutputResourceByVariableName(
        "OutputUV");
    NIASSERT(pkUVSet);
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainMaterial::VerifyShaderPrograms(NiGPUProgram* pkVertexShader,
    NiGPUProgram*, NiGPUProgram* pkPixelShader)
{
    NiUInt32 uiReturnCode = RC_SUCCESS;
    if (pkVertexShader == NULL)
        uiReturnCode |= RC_COMPILE_FAILURE_VERTEX;
    if (pkPixelShader == NULL)
        uiReturnCode |= RC_COMPILE_FAILURE_PIXEL;
    // No need to check geometry shader

    return uiReturnCode;
}
//---------------------------------------------------------------------------
bool NiTerrainMaterial::DropNormalParallaxMap(
    NiMaterialDescriptor*, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int, unsigned int&, unsigned int&)
{
    // This function can only deal with failed pixel or vertex shader compiles
    if ((eFailedRC & RC_COMPILE_FAILURE_PIXEL) == 0 &&
        (eFailedRC & RC_COMPILE_FAILURE_VERTEX) == 0)
    {
        return false;
    }

    NiTerrainMaterialPixelDescriptor* pkInvalidPixelDesc = 
        reinterpret_cast<NiTerrainMaterialPixelDescriptor*>(
        &pkRenderPasses[uiFailedPass].m_kPixelDesc);

    NiTerrainMaterialVertexDescriptor* pkInvalidVertexDesc = 
        reinterpret_cast<NiTerrainMaterialVertexDescriptor*>(
        &pkRenderPasses[uiFailedPass].m_kVertexDesc);

    bool bRemovedMaps = false;

    unsigned int uiNumLayers = pkInvalidPixelDesc->GetNUM_LAYERS();
    for(unsigned int uiCurrentLayer = 0; uiCurrentLayer < uiNumLayers;
        ++uiCurrentLayer)
    {
        if (pkInvalidPixelDesc->SupportsNormalMap(uiCurrentLayer) ||
            pkInvalidPixelDesc->SupportsParallaxMap(uiCurrentLayer))
        {
            pkInvalidPixelDesc->SetNormalEnabled(uiCurrentLayer, false);
            pkInvalidPixelDesc->SetParallaxEnabled(uiCurrentLayer, false);

            bRemovedMaps = true;
        }
    }

    if (bRemovedMaps)
    {            
        NILOG("NiTerrainMaterial: Dropping normal/parallax maps\n");

        // If we have removed all the normal/parallax maps, we no longer
        // need world view or tangents
        NILOG("Remove WorldView from vertex and pixel shaders\n");
        pkInvalidPixelDesc->SetWORLD_VIEW(false);
        pkInvalidVertexDesc->SetOUTPUT_WORLDVIEW(false);

        NILOG("Remove tangents from vertex and pixel shaders\n");
        pkInvalidPixelDesc->SetWORLDNBT(false);
        pkInvalidVertexDesc->SetTANGENTS(false);
    }

    return bRemovedMaps;
}
//---------------------------------------------------------------------------
void NiTerrainMaterial::AddDefaultFallbacks()
{
    AddShaderFallbackFunc(DropNormalParallaxMap);

    NiFragmentMaterial::AddDefaultFallbacks();
}
//---------------------------------------------------------------------------
bool NiTerrainMaterial::HandleFinalVertexOutputs(Context& kContext,
    NiMaterialResource* pkWorldPositionLow,
    NiMaterialResource* pkWorldPositionHigh)
{
    NI_UNUSED_ARG(kContext);
    NI_UNUSED_ARG(pkWorldPositionLow);
    NI_UNUSED_ARG(pkWorldPositionHigh);

    return true;
}
//---------------------------------------------------------------------------
