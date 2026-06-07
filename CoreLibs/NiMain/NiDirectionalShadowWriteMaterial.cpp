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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiMainLibType.h"
#include "StandardMaterialNodeLibrary/NiStandardMaterialNodeLibrary.h"
#include "NiCommonSemantics.h"
#include "NiDirectionalShadowWriteMaterial.h"
#include "NiShadowGenerator.h"
#include "NiStandardMaterialDescriptor.h"
#include "NiStandardPixelProgramDescriptor.h"
#include "NiStandardVertexProgramDescriptor.h"
#include "NiShadowTechnique.h"
#include "NiRenderer.h"
#include "NiRenderObjectMaterialOption.h"

#define NIDIRECTIONALSHADOWWRITEMATERIAL_VERTEX_VERSION   6
#define NIDIRECTIONALSHADOWWRITEMATERIAL_PIXEL_VERSION    4

NiImplementRTTI(NiDirectionalShadowWriteMaterial, NiFragmentMaterial);

//---------------------------------------------------------------------------
NiDirectionalShadowWriteMaterial::NiDirectionalShadowWriteMaterial(
    NiMaterialNodeLibrary* pkLibrary, bool bAutoCreateCaches) : 
    NiStandardMaterial("NiDirShadowWriteMat", pkLibrary, 
    NIDIRECTIONALSHADOWWRITEMATERIAL_VERTEX_VERSION, 0, 
    NIDIRECTIONALSHADOWWRITEMATERIAL_PIXEL_VERSION, bAutoCreateCaches),
    m_kDescriptorName("NiStandardMaterialDescriptor")
{
    // Explicitly add the NiStandardMaterial node library.
    NiMaterialNodeLibrary* pkLib = 
        NiStandardMaterialNodeLibrary::CreateMaterialNodeLibrary();
    m_kLibraries.Add(pkLib);
}
//---------------------------------------------------------------------------
NiDirectionalShadowWriteMaterial::NiDirectionalShadowWriteMaterial(
    const NiFixedString& kName, NiMaterialNodeLibrary* pkLibrary, 
    bool bAutoCreateCaches) : 
    NiStandardMaterial(kName, pkLibrary, 
    NIDIRECTIONALSHADOWWRITEMATERIAL_VERTEX_VERSION, 0, 
    NIDIRECTIONALSHADOWWRITEMATERIAL_PIXEL_VERSION, bAutoCreateCaches), 
    m_kDescriptorName("NiStandardMaterialDescriptor")
{
    // Explicitly add the NiStandardMaterial node library.
    NiMaterialNodeLibrary* pkLib = 
        NiStandardMaterialNodeLibrary::CreateMaterialNodeLibrary();
    m_kLibraries.Add(pkLib);
}
//---------------------------------------------------------------------------
NiDirectionalShadowWriteMaterial::NiDirectionalShadowWriteMaterial(
    const NiFixedString& kName, NiMaterialNodeLibrary* pkLibrary, 
    unsigned int uiVertexVersion, unsigned int uiGeometryVersion, 
    unsigned int uiPixelVersion, bool bAutoCreateCaches) : 
    NiStandardMaterial(kName, pkLibrary, uiVertexVersion, uiGeometryVersion,
    uiPixelVersion, bAutoCreateCaches), 
    m_kDescriptorName("NiStandardMaterialDescriptor")
{
    // Explicitly add the NiStandardMaterial node library.
    NiMaterialNodeLibrary* pkLib = 
        NiStandardMaterialNodeLibrary::CreateMaterialNodeLibrary();
    m_kLibraries.Add(pkLib);
}
//---------------------------------------------------------------------------
bool NiDirectionalShadowWriteMaterial::GenerateDescriptor(
    const NiRenderObject* pkGeom,
    const NiPropertyState* pkPropState, 
    const NiDynamicEffectState* pkEffectState, 
    NiMaterialDescriptor& kMaterialDesc)
{    
    if (!pkPropState)
    {
        NIASSERT(!"Could not find property state! Try calling"
            " UpdateProperties.\n");
        return false;
    }

    NiStandardMaterialDescriptor* pkDesc = (NiStandardMaterialDescriptor*) 
        &kMaterialDesc;
    pkDesc->m_kIdentifier = m_kDescriptorName;

    bool bHardwareSkinned;
    pkDesc->SetTransformDescriptor(pkGeom, bHardwareSkinned);


    // Handle TexGen data and UV sets
    if (pkPropState)
    {
        NiTexturingProperty* pkTexProp = pkPropState->GetTexturing();
        NiAlphaProperty* pkAlphaProp = pkPropState->GetAlpha();

        pkDesc->SetAlphaPropertyDescriptor(pkGeom, pkPropState);

        // Only reference texturing property if there is alpha information.
        if (pkTexProp && pkAlphaProp && pkDesc->GetALPHATEST())
        {
            // Handle adding textures to the shader description.
            // Note that the order in which these textures are handled MUST
            // match the order in which they were declared in the descriptor.
            // This will guarantee that the UVSets for Map## will correspond 
            // to the correct semantic texture map later on.

            const NiTexturingProperty::Map* apkActiveMaps[
                STANDARD_PIPE_MAX_TEXTURE_MAPS];
            memset(apkActiveMaps, 0, sizeof(NiTexturingProperty::Map*) * 
                STANDARD_PIPE_MAX_TEXTURE_MAPS);

            // Count number of texture sets
            NiUInt32 uiNumTextureSets = pkGeom->GetSemanticCount(
                NiCommonSemantics::TEXCOORD());

            switch (pkTexProp->GetApplyMode())
            {
            case NiTexturingProperty::APPLY_REPLACE:
                pkDesc->SetAPPLYMODE(APPLY_REPLACE);
                break;
            default:
            case NiTexturingProperty::APPLY_MODULATE:
                pkDesc->SetAPPLYMODE(APPLY_MODULATE);
                break;
            }

            unsigned int uiMapIdx = 0;

            // It is customary in Gamebryo to skip all textures if there
            // are no texture coordinates in the geometry.
            if (uiNumTextureSets != 0)
            {
                pkDesc->SetBaseMapDescriptor(pkTexProp, apkActiveMaps, 
                    uiMapIdx);

                pkDesc->SetTextureMapUVsDescriptor(pkGeom, apkActiveMaps, 
                    STANDARD_PIPE_MAX_TEXTURE_MAPS, uiNumTextureSets, 
                    uiMapIdx);
            }
        }
    }

    pkDesc->SetShadowTechniqueDescriptor(pkGeom, pkEffectState);

    return true;
}
//---------------------------------------------------------------------------
bool NiDirectionalShadowWriteMaterial::GeneratePixelShadeTree(
    Context& kContext, NiGPUProgramDescriptor* pkDesc)
{
    NIASSERT(pkDesc->m_kIdentifier == "NiStandardPixelProgramDescriptor");
    NiStandardPixelProgramDescriptor* pkPixelDesc = 
        (NiStandardPixelProgramDescriptor*)pkDesc;

    // Add pixel in, pixel out, constants, and uniforms
    if (!AddDefaultMaterialNodes(kContext, pkDesc, 
        NiGPUProgram::PROGRAM_PIXEL))
    {
        return false;
    }
    // This is necessary for D3D10 support.
    kContext.m_spInputs->AddOutputResource("float4", "Position",
        "World",  "PosProjected");

    NiMaterialResource* pkWorldProjPos = 
        kContext.m_spInputs->AddOutputResource("float4", "TexCoord",
        "World",  "WorldPosProjected");
    
    NiMaterialResource* pkWorldViewVect = 
        kContext.m_spInputs->AddOutputResource("float3", "TexCoord",
        "World",  "WorldViewVector");

    NiMaterialResource* apkUVSets[STANDARD_PIPE_MAX_UVS_FOR_TEXTURES];

    unsigned int uiTexIndex = 0;
    for (unsigned int ui = 0; ui < pkPixelDesc->GetInputUVCount(); 
        ui++)
    {
        apkUVSets[uiTexIndex] = kContext.m_spInputs->AddOutputResource(
            "float2", "TexCoord", "", GenerateUVSetName(uiTexIndex));
        uiTexIndex++;
    }

    // Wire up shader to sample the base map and perfrom an alpha test based on
    // the base map's alpha component.
    if (pkPixelDesc->GetALPHATEST() != 0 && 
        pkPixelDesc->GetBASEMAPCOUNT() == 1)
    {
        NiMaterialResource* pkDiffuseTexAccum = NULL;
        NiMaterialResource* pkOpacityTexAccum = NULL;

        if (!HandleBaseMap(kContext, 
            FindUVSetIndexForTextureEnum( MAP_BASE, 
            pkPixelDesc, apkUVSets), pkDiffuseTexAccum, pkOpacityTexAccum, 
            true))
        {
            return false;
        }

        NiMaterialNode* pkAlphaTestNode = 
            GetAttachableNodeFromLibrary("ApplyAlphaTest");
        kContext.m_spConfigurator->AddNode(pkAlphaTestNode);

        NiMaterialResource* pkTestFunction = AddOutputPredefined(
            kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_DEF_ALPHA_TEST_FUNC);

        kContext.m_spConfigurator->AddBinding(pkTestFunction, 
            pkAlphaTestNode->GetInputResourceByVariableName(
            "AlphaTestFunction"));

        NiMaterialResource* pkTestRef = AddOutputPredefined(
            kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_DEF_ALPHA_TEST_REF);
        kContext.m_spConfigurator->AddBinding(pkTestRef, 
            pkAlphaTestNode->GetInputResourceByVariableName(
            "AlphaTestRef"));

        kContext.m_spConfigurator->AddBinding(pkOpacityTexAccum,
            pkAlphaTestNode->GetInputResourceByVariableName("AlphaTestValue"));
    }

    // Create pixel out
    kContext.m_spOutputs = NiNew NiMaterialResourceConsumerNode("PixelOut", 
        "Pixel");
    NiMaterialResource* pkPixelOutColor = 
        kContext.m_spOutputs->AddInputResource("float4", "Color", "", "Color");
    kContext.m_spConfigurator->AddNode(kContext.m_spOutputs);

    // Connect pixel shader node to the assigned ShadowTechnique.
    unsigned int uiShadowTechniqueID = pkPixelDesc->GetSHADOWTECHNIQUE();
    NiShadowTechnique* pkShadowTechnique =
        NiShadowManager::GetKnownShadowTechnique(
        (unsigned short)uiShadowTechniqueID);

    // If this assert is hit it means the NiShadowTechnique referenced by
    // is not one of the active NiShadowTechnique objects. A NiShadowTechnique
    // can only be used by the render if it is one of the active techniques.
    NIASSERT(pkShadowTechnique->GetActiveTechniqueSlot() < 
        NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES);

    NiMaterialNode* pkDepthNode = GetAttachableNodeFromLibrary(
        pkShadowTechnique->GetWriteFragmentName(
        NiStandardMaterial::LIGHT_DIR));
    kContext.m_spConfigurator->AddNode(pkDepthNode);

    // ViewVector Input
    kContext.m_spConfigurator->AddBinding(pkWorldViewVect, 
        pkDepthNode->GetInputResourceByVariableName("WorldViewVector"));

    // WorldPos Input
    kContext.m_spConfigurator->AddBinding(pkWorldProjPos, 
        pkDepthNode->GetInputResourceByVariableName("WorldPosProjected"));

    // Final color output
    kContext.m_spConfigurator->AddBinding("OutputColor", pkDepthNode, 
        pkPixelOutColor);

    return true;
}
//---------------------------------------------------------------------------
bool NiDirectionalShadowWriteMaterial::GenerateVertexShadeTree(
    Context& kContext, NiGPUProgramDescriptor* pkDesc)
{
    NIASSERT(pkDesc->m_kIdentifier == "NiStandardVertexProgramDescriptor");
    NiStandardVertexProgramDescriptor* pkVertexDesc = 
        (NiStandardVertexProgramDescriptor*)pkDesc;

    // Add vert in, vert out, constants, and uniforms
    if (!AddDefaultMaterialNodes(kContext, pkDesc, 
        NiGPUProgram::PROGRAM_VERTEX))
    {
        return false;
    }

    NiMaterialResource* pkVertOutProjPos = 
        kContext.m_spOutputs->AddInputResource("float4", "Position", "World", 
        "PosProjected");

    // Handle transform pipeline
    NiMaterialResource* pkWorldPos = NULL;
    NiMaterialResource* pkWorldNormal = NULL;
    NiMaterialResource* pkWorldView = NULL;
    NiMaterialResource* pkViewPos = NULL;
    NiMaterialResource* pkProjPos = NULL;

    if (!SetupTransformPipeline(kContext, pkVertOutProjPos, pkVertexDesc,
        true, false, pkWorldPos, pkViewPos, pkProjPos, pkWorldNormal, 
        pkWorldView))
    {
        return false;
    }

    unsigned int uiNextUVSet = 0;
    NiMaterialResource* apkOutputUVs[STANDARD_PIPE_MAX_UVS_FOR_TEXTURES];
    memset(apkOutputUVs, 0, STANDARD_PIPE_MAX_UVS_FOR_TEXTURES * 
        sizeof(NiMaterialResource*));

    if (!HandleTextureUVSets(kContext, pkVertexDesc, apkOutputUVs,
        STANDARD_PIPE_MAX_UVS_FOR_TEXTURES, uiNextUVSet))
    {
        return false;
    }

    // Bind the output texture array to output resources.
    for (unsigned int ui = 0; ui < uiNextUVSet; ui++)
    {
        NIASSERT(apkOutputUVs[ui] != NULL);
        NiMaterialResource* pkVertOutTexCoord = 
            kContext.m_spOutputs->AddInputResource(
            apkOutputUVs[ui]->GetType(), "TexCoord",
            "", GenerateUVSetName(ui));

        kContext.m_spConfigurator->AddBinding(apkOutputUVs[ui],
            pkVertOutTexCoord);
    }

#ifdef _XENON
    // If mesh instancing is used we need to manually perform a vfetch on all
    // vertex input data members and populate the input structure.
    if (pkVertexDesc->GetTRANSFORM() == TRANSFORM_INSTANCED)
    {
        kContext.m_spConfigurator->SetProgramDataInputType(
            NiMaterialConfigurator::INPUT_XENON_INDEX);
    }
#endif
    return true;
}
//---------------------------------------------------------------------------
bool NiDirectionalShadowWriteMaterial::SetupTransformPipeline(
    Context& kContext, NiMaterialResource* pkOutProjPos,
    NiStandardVertexProgramDescriptor* pkVertexDesc, bool bForceWorldView,
    bool bForceViewPos, NiMaterialResource*& pkWorldPos, 
    NiMaterialResource*& pkViewPos, NiMaterialResource*& pkProjectedPos, 
    NiMaterialResource*& pkWorldNormal, NiMaterialResource*& pkWorldView)
{
    pkWorldPos = NULL;
    pkWorldNormal = NULL;
    pkWorldView = NULL;

    NiMaterialResource* pkWorldMatrix = NULL;

    if (!HandlePositionFragment(kContext, 
        (NiStandardMaterial::TransformType)pkVertexDesc->GetTRANSFORM(),
        pkWorldPos, pkWorldMatrix))
    {
        return false;
    }

    if (!HandleViewProjectionFragment(kContext, bForceViewPos, 
        pkWorldPos, pkProjectedPos, pkViewPos))
    {
        return false;
    }

    if (!pkProjectedPos || !pkOutProjPos)
        return false;

    if (bForceWorldView)
    {
        NiMaterialResource* pkTangentView = NULL;

        if (!HandleViewVectorFragment(kContext,
            pkWorldPos, pkWorldNormal, false, NULL,
            false, pkWorldView, pkTangentView))
        {
            return false;
        }

        //if (pkWorldView)
        //{
        //    // Insert view vector
        //    NiMaterialResource* pkWorldTexCoord = 
        //        kContext.m_spOutputs->AddInputResource("float3",
        //        "TexCoord", "", "ViewVector");
        //    kContext.m_spConfigurator->AddBinding(pkWorldView, 
        //        pkWorldTexCoord);
        //}
    }

    kContext.m_spConfigurator->AddBinding(pkProjectedPos, 
        pkOutProjPos); 


    if (pkVertexDesc->GetOUTPUTWORLDPOS() == 1)
    {
        // Insert world position
        NiMaterialResource* pkWorldTexCoord = 
            kContext.m_spOutputs->AddInputResource("float4",
            "TexCoord", "", "WorldPos");
        kContext.m_spOutputs->AddOutputResource("float4",
            "TexCoord", "", "WorldPos");
        kContext.m_spConfigurator->AddBinding(pkWorldPos, 
            pkWorldTexCoord);
    }

    return true;
}
//---------------------------------------------------------------------------
NiFragmentMaterial::ReturnCode 
    NiDirectionalShadowWriteMaterial::GenerateShaderDescArray(
    NiMaterialDescriptor* pkMaterialDescriptor,
    RenderPassDescriptor* pkRenderPasses,
    unsigned int uiMaxCount, 
    unsigned int& uiCountAdded)
{
    NI_UNUSED_ARG(uiMaxCount);
    NIASSERT(uiMaxCount != 0);
    uiCountAdded = 0;

    if (pkMaterialDescriptor->m_kIdentifier != 
        "NiStandardMaterialDescriptor")
        return RC_INVALID_MATERIAL;

    // Make sure that we're using the Gamebryo render state on the first pass.
    // Also ensure that no alpha blending is used as this is not needed for 
    // rendering to shadow maps.
    pkRenderPasses[0].m_bUsesNiRenderState = true;
    pkRenderPasses[0].m_bAlphaOverride = true;
    pkRenderPasses[0].m_bAlphaBlend = false;

    // Reset all object offsets for the first pass.
    pkRenderPasses[0].m_bResetObjectOffsets = true;

    NiStandardMaterialDescriptor* pkMatlDesc = 
        (NiStandardMaterialDescriptor*) pkMaterialDescriptor;

    // Uncomment these lines to get a human-readable version of the 
    // material description
    // char acBuffer[2048];
    // pkMatlDesc->ToString(acBuffer, 2048);

    NiStandardVertexProgramDescriptor* pkVertexDesc = 
        (NiStandardVertexProgramDescriptor*) &pkRenderPasses[0].m_kVertexDesc;
    pkVertexDesc->m_kIdentifier = "NiStandardVertexProgramDescriptor";

    NiStandardPixelProgramDescriptor* pkPixelDesc = 
        (NiStandardPixelProgramDescriptor*) &pkRenderPasses[0].m_kPixelDesc;
    pkPixelDesc->m_kIdentifier = "NiStandardPixelProgramDescriptor";

    // Pixel Desc
    unsigned int uiApplyMode = pkMatlDesc->GetAPPLYMODE();
    pkPixelDesc->SetAPPLYMODE(uiApplyMode);

    unsigned int uiBaseCount = pkMatlDesc->GetBASEMAPCOUNT();
    pkPixelDesc->SetBASEMAPCOUNT(uiBaseCount);

    unsigned int uiAlphaTest = pkMatlDesc->GetALPHATEST();
    pkPixelDesc->SetALPHATEST(uiAlphaTest);

    unsigned int uiShadowTechniqueSlot = pkMatlDesc->GetSHADOWTECHNIQUE();
    NiShadowTechnique* pkShadowTechnique = 
        NiShadowManager::GetActiveShadowTechnique(
        (unsigned short)uiShadowTechniqueSlot);

    pkPixelDesc->SetSHADOWTECHNIQUE(pkShadowTechnique->GetTechniqueID());

    // Vertex Desc
    unsigned int uiTransform = pkMatlDesc->GetTRANSFORM();
    pkVertexDesc->SetTRANSFORM(uiTransform);

    unsigned int auiUVSets[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(auiUVSets, UINT_MAX, sizeof(auiUVSets));

    TexGenOutput aeTexGenOutputs[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(aeTexGenOutputs, 0, sizeof(aeTexGenOutputs));

    unsigned int uiTextureCount = pkMatlDesc->GetStandardTextureCount();
    NIASSERT(uiTextureCount <= STANDARD_PIPE_MAX_TEXTURE_MAPS);

    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        pkMatlDesc->GetTextureUsage(ui, auiUVSets[ui], aeTexGenOutputs[ui]);
    }

    AssignTextureCoordinates(auiUVSets, aeTexGenOutputs, uiTextureCount, 
        pkVertexDesc, pkPixelDesc);


    uiCountAdded++;
    return RC_SUCCESS;
}
//---------------------------------------------------------------------------
NiShader* NiDirectionalShadowWriteMaterial::CreateShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer == NULL)
        return false;

    return pkRenderer->GetShadowWriteShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
bool NiDirectionalShadowWriteMaterial::SetupPackingRequirements(
    NiShader* pkShader, NiMaterialDescriptor* pkMaterialDescriptor,
    RenderPassDescriptor*, unsigned int)
{
    NiStandardMaterialDescriptor* pkRealMaterialDesc = 
        (NiStandardMaterialDescriptor*) pkMaterialDescriptor;

    unsigned int uiStreamCount = 1;

    // If transform instancing is used create another stream to store the
    // per instance parameters.
    if (pkRealMaterialDesc->GetTRANSFORM() == TRANSFORM_INSTANCED)
    {
        uiStreamCount++;
    }

    NiShaderDeclarationPtr spShaderDecl = 
        NiShaderDeclaration::Create(15, uiStreamCount);

    if (!spShaderDecl)
    {
        NIASSERT(!"Invalid shader declaration");
        return false;
    }

    unsigned int uiEntryCount = 0;
    unsigned int uiInputUVCount = pkRealMaterialDesc->GetINPUTUVCOUNT();
    unsigned int uiUVOffset = 0;
#if !defined(_XENON)
    // If transform instancing is used, the the instance transforms will
    // consume the first three texture coordinates, so offset all other texture
    // coordinates by three.
    if (pkRealMaterialDesc->GetTRANSFORM() == TRANSFORM_INSTANCED)
    {
        uiUVOffset = 3;
        if (uiUVOffset + uiInputUVCount > 8)
        {
            NIASSERT(!"Invalid shader declaration. Too many TexCoords.");
            return false;
        }
    }
#endif //#if !defined(_XENON)

    spShaderDecl->SetEntry(uiEntryCount++,
        NiShaderDeclaration::SHADERPARAM_NI_POSITION0,
        NiShaderDeclaration::SPTYPE_FLOAT3);

    if (pkRealMaterialDesc->GetTRANSFORM() == TRANSFORM_SKINNED)
    {
        spShaderDecl->SetEntry(uiEntryCount++, 
            NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT,
            NiShaderDeclaration::SPTYPE_FLOAT3); // 3, 4th is implicit.
        spShaderDecl->SetEntry(uiEntryCount++,
            NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES,
            NiShaderDeclaration::SPTYPE_UBYTE4);
    }
    else if (pkRealMaterialDesc->GetTRANSFORM() == TRANSFORM_INSTANCED)
    {
        spShaderDecl->SetEntry(0, 
            NiShaderDeclaration::SHADERPARAM_NI_TRANSFORM0,
            NiShaderDeclaration::SPTYPE_FLOAT4, uiStreamCount-1);

        spShaderDecl->SetEntry(1,
            NiShaderDeclaration::SHADERPARAM_NI_TRANSFORM1,
            NiShaderDeclaration::SPTYPE_FLOAT4, uiStreamCount-1);

        spShaderDecl->SetEntry(2, 
            NiShaderDeclaration::SHADERPARAM_NI_TRANSFORM2,
            NiShaderDeclaration::SPTYPE_FLOAT4, uiStreamCount-1);
    }

    for (unsigned int ui = 0; ui < 8 && ui < uiInputUVCount; ui++)
    {
        spShaderDecl->SetEntry(uiEntryCount++, 
            (NiShaderDeclaration::ShaderParameter)
            (NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0 + ui + uiUVOffset),
            NiShaderDeclaration::SPTYPE_FLOAT2);
    }

    pkShader->SetSemanticAdapterTableFromShaderDeclaration(spShaderDecl);

    return true;
}
//---------------------------------------------------------------------------
bool NiDirectionalShadowWriteMaterial::HandleViewProjectionFragment(
    Context& kContext,  bool bForceViewPos, 
    NiMaterialResource* pkVertWorldPos,
    NiMaterialResource*& pkVertOutProjectedPos,
    NiMaterialResource*& pkVertOutViewPos)
{
    if (bForceViewPos)
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "ProjectPositionWorldToViewToProj");
        if (!pkProjFrag)
        {
            NIASSERT(!"Error in fragment");
            return false;
        }

        kContext.m_spConfigurator->AddNode(pkProjFrag);

        NiMaterialResource* pkViewMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_VIEW, 4);
        kContext.m_spConfigurator->AddBinding(pkViewMatrix, 
            pkProjFrag->GetInputResourceByVariableName("ViewTransform"));

        NiMaterialResource* pkProjMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_PROJ, 4);
        kContext.m_spConfigurator->AddBinding(pkProjMatrix, 
            pkProjFrag->GetInputResourceByVariableName("ProjTransform"));

        kContext.m_spConfigurator->AddBinding(pkVertWorldPos, 
            pkProjFrag->GetInputResourceByVariableName("WorldPosition"));

        pkVertOutViewPos = pkProjFrag->GetOutputResourceByVariableName(
            "ViewPos");

        pkVertOutProjectedPos = pkProjFrag->GetOutputResourceByVariableName(
            "ProjPos");
    }
    else
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "ProjectPositionWorldToProj");
        if (!pkProjFrag)
        {
            NIASSERT(!"Error in fragment");
            return false;
        }

        kContext.m_spConfigurator->AddNode(pkProjFrag);

        NiMaterialResource* pkViewProjMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_VIEWPROJ, 4);

        kContext.m_spConfigurator->AddBinding(pkViewProjMatrix, 
            pkProjFrag->GetInputResourceByVariableName("ViewProjection"));
        kContext.m_spConfigurator->AddBinding(pkVertWorldPos, 
            pkProjFrag->GetInputResourceByVariableName("WorldPosition"));

        pkVertOutProjectedPos = pkProjFrag->GetOutputResourceByVariableName(
            "ProjPos");

    }

    // add projected position as a texture coordinate 
    NiMaterialResource* pkVertOutProjTexCoord = 
        kContext.m_spOutputs->AddInputResource("float4", "TexCoord", "World", 
        "PosProjectedPassThrough");

    // Split vertex out projected position into the new texture coordinate
    // as well as output it from this fragment.  This is necessary because
    // (for now) you can't bind two named outputs to a single input.
    NiMaterialNode* pkSplitterNode = GetAttachableNodeFromLibrary(
        "TeeFloat4");
    kContext.m_spConfigurator->AddNode(pkSplitterNode);

    kContext.m_spConfigurator->AddBinding(pkVertOutProjectedPos,
        pkSplitterNode->GetInputResourceByVariableName("Input"));

    kContext.m_spConfigurator->AddBinding(
        pkSplitterNode->GetOutputResourceByVariableName("Output1"),
        pkVertOutProjTexCoord);

    pkVertOutProjectedPos = pkSplitterNode->GetOutputResourceByVariableName(
        "Output2");

    return true;
}
//---------------------------------------------------------------------------
bool NiDirectionalShadowWriteMaterial::HandleViewVectorFragment(
    Context& kContext, NiMaterialResource* pkWorldPos, 
    NiMaterialResource*, NiMaterialResource*, 
    NiMaterialResource*, bool, 
    NiMaterialResource*& pkWorldViewVector, 
    NiMaterialResource*& pkTangentViewVector)
{
    pkWorldViewVector = NULL;
    pkTangentViewVector = NULL;

    NiMaterialNode* pkNode = GetAttachableNodeFromLibrary(
        "CalculateViewVector");
    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkWorldPos, 
        pkNode->GetInputResourceByVariableName("WorldPos"));

    NiMaterialResource* pkCameraPosition = AddOutputPredefined(
        kContext.m_spUniforms,
        NiShaderConstantMap::SCM_DEF_EYE_POS);

    kContext.m_spConfigurator->AddBinding(pkCameraPosition, 
        pkNode->GetInputResourceByVariableName("CameraPos")); 

    // This vector will not be normalized!
    pkWorldViewVector = 
        pkNode->GetOutputResourceByVariableName("WorldViewVector");

    NiMaterialResource* pkVertOutProjTexCoord = 
        kContext.m_spOutputs->AddInputResource("float3", "TexCoord", "World", 
        "WorldViewVector");

    kContext.m_spConfigurator->AddBinding(pkWorldViewVector,
        pkVertOutProjTexCoord);

    return true;
}
//---------------------------------------------------------------------------
