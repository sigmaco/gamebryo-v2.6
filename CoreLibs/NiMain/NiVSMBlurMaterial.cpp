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

#include "NiCommonSemantics.h"
#include "NiRenderObjectMaterialOption.h"
#include "NiRenderer.h"
#include "NiShadowTechnique.h"
#include "NiVSMShadowTechnique.h"
#include "NiVSMBlurMaterial.h"
#include "NiShadowGenerator.h"
#include "NiVSMBlurMaterialDescriptor.h"
#include "NiVSMBlurPixelProgramDescriptor.h"
#include "NiVSMBlurVertexProgramDescriptor.h"



#define NIVSMBLURMATERIAL_VERTEX_VERSION   3
#define NIVSMBLURMATERIAL_PIXEL_VERSION    1

NiImplementRTTI(NiVSMBlurMaterial, NiFragmentMaterial);

//---------------------------------------------------------------------------
NiVSMBlurMaterial::NiVSMBlurMaterial(
    NiMaterialNodeLibrary* pkLibrary, bool bAutoCreateCaches) : 
    NiFragmentMaterial(pkLibrary, "NiVSMBlurMaterial", 
    NIVSMBLURMATERIAL_VERTEX_VERSION, 0, 
    NIVSMBLURMATERIAL_PIXEL_VERSION, bAutoCreateCaches),
    m_kDescriptorName("NiVSMBlurMaterialDescriptor")
{
    /* */
}
//---------------------------------------------------------------------------
bool NiVSMBlurMaterial::GenerateDescriptor(const NiRenderObject* pkMesh, 
    const NiPropertyState* pkPropState, 
    const NiDynamicEffectState*,
    NiMaterialDescriptor& kMaterialDesc)
{    
    if (!pkPropState)
    {
        NIASSERT(!"Could not find property state! Try calling"
            " UpdateProperties.\n");
        return false;
    }

    NiVSMBlurMaterialDescriptor* pkDesc = (NiVSMBlurMaterialDescriptor*) 
        &kMaterialDesc;
    pkDesc->m_kIdentifier = m_kDescriptorName;

    // Handle transform
    if (pkMesh->RequiresMaterialOption(
        NiRenderObjectMaterialOption::TRANSFORM_SKINNED()))
    {
        pkDesc->SetTRANSFORM(TRANSFORM_SKINNED);
    }
    //Insert code to set instanced transform bit here:
    //else if (...determine if instanced...)
    //{
    //    pkDesc->SetTRANSFORM(NiStandardMaterial::TRANSFORM_INSTANCED);
    //}
    else
    {
        pkDesc->SetTRANSFORM(TRANSFORM_DEFAULT);
    }

    if (pkPropState)
    {
        NiTexturingProperty* pkTexProp = pkPropState->GetTexturing();
        if (pkTexProp)
        {
            // Used to define horizontal or vertical blur
            switch (pkTexProp->GetApplyMode())
            {
            case NiTexturingProperty::APPLY_REPLACE:
                pkDesc->SetBLURDIRECTION(false);
                break;
            default:
            case NiTexturingProperty::APPLY_MODULATE:
                pkDesc->SetBLURDIRECTION(true);
                break;
            }

            if (pkTexProp->GetBaseTextureTransform())
            {
                pkDesc->SetTextureUsage(0, 0, 
                    NiVSMBlurMaterial::TEX_OUT_TRANSFORM);
            }
            else
            {
                pkDesc->SetTextureUsage(0, 0, 
                    NiVSMBlurMaterial::TEX_OUT_PASSTHRU);
            }

        }
    }

    // Encode kernel size in the POINTLIGHTCOUNT since it is unused by this 
    // material.
    unsigned int uiKernelSize = 4;
    NiVSMShadowTechnique* pkVSMTechnique = (NiVSMShadowTechnique*)
        NiShadowManager::GetKnownShadowTechnique("NiVSMShadowTechnique");
    if (pkVSMTechnique)
    {
        uiKernelSize = pkVSMTechnique->GetBlurKernelSize();
    }
    pkDesc->SetVSMBLURKERNEL(uiKernelSize-1);

    return true;
}
//---------------------------------------------------------------------------
bool NiVSMBlurMaterial::GeneratePixelShadeTree(Context& kContext, 
    NiGPUProgramDescriptor* pkDesc)
{
    NIASSERT(pkDesc->m_kIdentifier == "NiVSMBlurPixelProgramDescriptor");
    NiVSMBlurPixelProgramDescriptor* pkPixelDesc = 
        (NiVSMBlurPixelProgramDescriptor*)pkDesc;

    // Add constant map elements
    kContext.m_spUniforms = NiNew NiMaterialResourceProducerNode("Uniforms",
        "Uniform");
    kContext.m_spConfigurator->AddNode(kContext.m_spUniforms);

    // Create statics
    kContext.m_spStatics = NiNew NiMaterialResourceProducerNode("Statics", 
        "Static");
    kContext.m_spConfigurator->AddNode(kContext.m_spStatics);

    // Create pixel in
    kContext.m_spInputs = NiNew 
        NiMaterialResourceProducerNode("PixelIn", "Vertex");
    // This is necessary for D3D10 support.
    kContext.m_spInputs->AddOutputResource("float4", "Position",
        "World",  "PosProjected");
    
    kContext.m_spConfigurator->AddNode(kContext.m_spInputs);

    // Create pixel out
    kContext.m_spOutputs = NiNew NiMaterialResourceConsumerNode("PixelOut", 
        "Pixel");
    NiMaterialResource* pkPixelOutColor = 
        kContext.m_spOutputs->AddInputResource("float4", "Color", "", "Color");
    kContext.m_spConfigurator->AddNode(kContext.m_spOutputs);

    NiMaterialResource* pkTexDiffuseAccum = NULL;
    NiMaterialResource* pkUVSet = 
        kContext.m_spInputs->AddOutputResource(
        "float2", "TexCoord", "", "UVSet0");

    NiMaterialNode* pkMatNode = NULL;
    NiFixedString kBlurSampler;
    if (pkPixelDesc->GetBLURDIRECTION() == false)
    {
        kBlurSampler = "HorzBlurSample";
    }
    else
    {
        kBlurSampler = "VertBlurSample";
    }

    // The KernelSize is encoded in the POINTLIGHTCOUNT count since it unused
    // by this material.
    unsigned int uiKernelSize = pkPixelDesc->GetVSMBLURKERNEL() + 1;

    if (!InsertTexture(kContext, pkUVSet, pkTexDiffuseAccum, 
        pkMatNode, kBlurSampler, uiKernelSize))
    {
        return false;
    }


    // Final color output
    kContext.m_spConfigurator->AddBinding("ColorOut", pkMatNode, 
        pkPixelOutColor);

    return true;
}
//---------------------------------------------------------------------------
bool NiVSMBlurMaterial::GenerateVertexShadeTree(Context& kContext, 
    NiGPUProgramDescriptor* pkDesc)
{
    NIASSERT(pkDesc->m_kIdentifier == "NiVSMBlurVertexProgramDescriptor");
    NiVSMBlurVertexProgramDescriptor* pkVertexDesc = 
        (NiVSMBlurVertexProgramDescriptor*)pkDesc;

    // Create vertex in
    kContext.m_spInputs = NiNew NiMaterialResourceProducerNode("VertexIn",
        "Vertex");  
    kContext.m_spConfigurator->AddNode(kContext.m_spInputs);

    // Add constant map elements
    kContext.m_spUniforms = NiNew NiMaterialResourceProducerNode("Uniforms",
        "Uniform");
    kContext.m_spConfigurator->AddNode(kContext.m_spUniforms);

    kContext.m_spStatics = NiNew NiMaterialResourceProducerNode("Statics",
        "Static");
    kContext.m_spConfigurator->AddNode(kContext.m_spStatics);

    // Create vertex out
    kContext.m_spOutputs = NiNew NiMaterialResourceConsumerNode("VertexOut", 
        "Vertex");
    NiMaterialResource* pkVertOutProjPos = 
        kContext.m_spOutputs->AddInputResource("float4", "Position", "World", 
        "PosProjected");
    kContext.m_spConfigurator->AddNode(kContext.m_spOutputs);

    NiMaterialResource* pkVertTransformTexCoord = NULL;
    NiMaterialResource* pkVertInTexCoord = 
        kContext.m_spInputs->AddOutputResource("float2", "TexCoord", 
        "", "UVSet0");

    NiMaterialResource* pkVertOutTexCoord = 
        kContext.m_spOutputs->AddInputResource("float2", "TexCoord", 
        "", "UVSet0");

   
    unsigned int uiUVInput;
    NiVSMBlurMaterial::TexGenOutput eTexGenOutput;
    pkVertexDesc->GetOutputUVUsage(0, uiUVInput, eTexGenOutput);
    HandleTexGen(kContext, pkVertInTexCoord, pkVertTransformTexCoord, "Base", 
        0, eTexGenOutput);

    kContext.m_spConfigurator->AddBinding(pkVertTransformTexCoord,
        pkVertOutTexCoord);


    // Handle transform pipeline
    NiMaterialResource* pkWorldPos = NULL;
    NiMaterialResource* pkWorldNormal = NULL;
    NiMaterialResource* pkWorldView = NULL;
    NiMaterialResource* pkViewPos = NULL;
    NiMaterialResource* pkProjectedPos = NULL;

    if (!SetupTransformPipeline(kContext, pkVertOutProjPos, pkVertexDesc,
        true, false, pkWorldPos, pkViewPos, pkProjectedPos, pkWorldNormal, 
        pkWorldView))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiVSMBlurMaterial::VerifyShaderPrograms(
    NiGPUProgram* pkVertexShader, NiGPUProgram*, 
    NiGPUProgram* pkPixelShader)
{
    unsigned int uiReturnCode = RC_SUCCESS;
    if (pkVertexShader == NULL)
        uiReturnCode |= RC_COMPILE_FAILURE_VERTEX;
    if (pkPixelShader == NULL)
        uiReturnCode |= RC_COMPILE_FAILURE_PIXEL;
    // No need to check geometry shader

    return uiReturnCode;
}
//---------------------------------------------------------------------------
bool NiVSMBlurMaterial::SetupTransformPipeline(Context& kContext, 
   NiMaterialResource* pkOutProjPos,
   NiVSMBlurVertexProgramDescriptor* pkVertexDesc, bool,
   bool bForceViewPos, NiMaterialResource*& pkWorldPos, 
   NiMaterialResource*& pkViewPos, NiMaterialResource*& pkProjectedPos,
   NiMaterialResource*& pkWorldNormal, NiMaterialResource*& pkWorldView)
{
    pkWorldPos = NULL;
    pkWorldNormal = NULL;
    pkWorldView = NULL;

    NiMaterialResource* pkWorldMatrix = NULL;

    if (!HandlePositionFragment(kContext, 
        (TransformType)pkVertexDesc->GetTRANSFORM(),
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
    /*
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
    */
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
    NiVSMBlurMaterial::GenerateShaderDescArray(
    NiMaterialDescriptor* pkMaterialDescriptor,
    RenderPassDescriptor* pkRenderPasses,
    unsigned int uiMaxCount,
    unsigned int& uiCountAdded)
{
    NI_UNUSED_ARG(uiMaxCount);
    NIASSERT(uiMaxCount != 0);
    uiCountAdded = 0;

    if (pkMaterialDescriptor->m_kIdentifier != 
        "NiVSMBlurMaterialDescriptor")
        return RC_INVALID_MATERIAL;

    // Make sure that we're using the Gamebryo render state on the first pass.
    // Also ensure that no alpha blending is used as this is not needed for 
    // rendering to shadow maps.
    pkRenderPasses[0].m_bUsesNiRenderState = true;
    pkRenderPasses[0].m_bAlphaOverride = true;
    pkRenderPasses[0].m_bAlphaBlend = false;

    // Reset all object offsets for the first pass.
    pkRenderPasses[0].m_bResetObjectOffsets = true;

    NiVSMBlurMaterialDescriptor* pkMatlDesc = 
        (NiVSMBlurMaterialDescriptor*) pkMaterialDescriptor;

    // Uncomment these lines to get a human-readable version of the 
    // material description
    // char acBuffer[2048];
    // pkMatlDesc->ToString(acBuffer, 2048);

    NiVSMBlurVertexProgramDescriptor* pkVertexDesc = 
        (NiVSMBlurVertexProgramDescriptor*) &pkRenderPasses[0].m_kVertexDesc;
    pkVertexDesc->m_kIdentifier = "NiVSMBlurVertexProgramDescriptor";

    NiVSMBlurPixelProgramDescriptor* pkPixelDesc = 
        (NiVSMBlurPixelProgramDescriptor*) &pkRenderPasses[0].m_kPixelDesc;
    pkPixelDesc->m_kIdentifier = "NiVSMBlurPixelProgramDescriptor";

    // Vertex Desc
    unsigned int uiTransform = pkMatlDesc->GetTRANSFORM();
    pkVertexDesc->SetTRANSFORM(uiTransform);
    
    unsigned int uiUVInput;
    NiVSMBlurMaterial::TexGenOutput eTexGenOutput;
    pkMatlDesc->GetTextureUsage(0, uiUVInput, eTexGenOutput);
    pkVertexDesc->SetOutputUVUsage(0, uiUVInput, eTexGenOutput);

    
    // Pixel Desc
    pkPixelDesc->SetBLURDIRECTION(pkMatlDesc->GetBLURDIRECTION());

    pkPixelDesc->SetVSMBLURKERNEL(pkMatlDesc->GetVSMBLURKERNEL());

    pkPixelDesc->SetUVSetForMap(0, 0);

    unsigned int uiShadowTechniqueSlot = pkMatlDesc->GetSHADOWTECHNIQUE();
    NiShadowTechnique* pkShadowTechnique = 
        NiShadowManager::GetActiveShadowTechnique(
        (unsigned short)uiShadowTechniqueSlot);
        
    pkPixelDesc->SetSHADOWTECHNIQUE(pkShadowTechnique->GetTechniqueID());

    uiCountAdded++;
    return RC_SUCCESS;
}
//---------------------------------------------------------------------------
NiShader* NiVSMBlurMaterial::CreateShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer == NULL)
        return false;

    return pkRenderer->GetShadowWriteShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
bool NiVSMBlurMaterial::SetupPackingRequirements(NiShader* pkShader,
    NiMaterialDescriptor* pkMaterialDescriptor,
    RenderPassDescriptor*, unsigned int)
{
    NiShaderDeclarationPtr spShaderDecl = NiShaderDeclaration::Create(15, 1);

    if (!spShaderDecl)
    {
        NIASSERT(!"Invalid shader declaration");
        return false;
    }

    NiVSMBlurMaterialDescriptor* pkRealMaterialDesc = 
        (NiVSMBlurMaterialDescriptor*) pkMaterialDescriptor;

    unsigned int uiEntryCount = 0;
    spShaderDecl->SetEntry(uiEntryCount++,
        NiShaderDeclaration::SHADERPARAM_NI_POSITION0,
        NiShaderDeclaration::SPTYPE_FLOAT3);

    if (pkRealMaterialDesc->GetTRANSFORM() == TRANSFORM_SKINNED)
    {
        spShaderDecl->SetEntry(uiEntryCount++,
            NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT,
            NiShaderDeclaration::SPTYPE_FLOAT4);
        spShaderDecl->SetEntry(uiEntryCount++,
            NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES,
            NiShaderDeclaration::SPTYPE_UBYTE4);
    }

    spShaderDecl->SetEntry(uiEntryCount++, 
        NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0,
        NiShaderDeclaration::SPTYPE_FLOAT2);

    pkShader->SetSemanticAdapterTableFromShaderDeclaration(spShaderDecl);

    return true;
}
//---------------------------------------------------------------------------
bool NiVSMBlurMaterial::HandlePositionFragment(Context& kContext, 
    TransformType eTransType, NiMaterialResource*& pkVertWorldPos,
    NiMaterialResource*& pkWorldMatrix)
{
    pkVertWorldPos = NULL;
    pkWorldMatrix = NULL;

    if (eTransType == TRANSFORM_DEFAULT)
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "TransformPosition");
        kContext.m_spConfigurator->AddNode(pkProjFrag);

        // Add Per-Vertex Elements
        kContext.m_spInputs->AddOutputResource("float3", "Position", "Local", 
            "Position", 1);

        // Add constant map elements
        pkWorldMatrix = AddOutputPredefined(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_DEF_WORLD, 4);

        // Bind projection
        kContext.m_spConfigurator->AddBinding("Position", kContext.m_spInputs, 
            "Position", pkProjFrag);

        kContext.m_spConfigurator->AddBinding(pkWorldMatrix,
            "World", pkProjFrag);

        pkVertWorldPos = pkProjFrag->GetOutputResourceByVariableName(
            "WorldPos");
    }
    else if (eTransType == TRANSFORM_SKINNED)
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "TransformSkinnedPosition");
        kContext.m_spConfigurator->AddNode(pkProjFrag);

        // Add Per-Vertex Elements
        kContext.m_spInputs->AddOutputResource("float3", "Position", 
            "Local", "Position");
        kContext.m_spInputs->AddOutputResource("float3", "BlendWeight", 
            "Local", "BlendWeights");
        kContext.m_spInputs->AddOutputResource("int4", "BlendIndices", 
            "Local", "BlendIndices");

        // Add constant map elements
        NiMaterialResource* pkBoneMatrix = AddOutputPredefined(
            kContext.m_spUniforms,
            NiShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_3,
            3, 30);

        // Bind projection
        kContext.m_spConfigurator->AddBinding("Position", kContext.m_spInputs, 
            "Position", pkProjFrag);
        kContext.m_spConfigurator->AddBinding("BlendWeights",
            kContext.m_spInputs, "BlendWeights", pkProjFrag);
        kContext.m_spConfigurator->AddBinding("BlendIndices", 
            kContext.m_spInputs, "BlendIndices", pkProjFrag);       

        kContext.m_spConfigurator->AddBinding(pkBoneMatrix,
            "Bones", pkProjFrag);

        pkVertWorldPos = pkProjFrag->GetOutputResourceByVariableName(
            "WorldPos");

        pkWorldMatrix = 
            pkProjFrag->GetOutputResourceByVariableName("SkinBoneTransform");
    }
    else
    {
        NIASSERT(false && "Unhandled transform type!");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiVSMBlurMaterial::HandleViewProjectionFragment(
    Context& kContext, 
    bool bForceViewPos, NiMaterialResource* pkVertWorldPos, 
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

    return true;
}
//---------------------------------------------------------------------------
bool NiVSMBlurMaterial::HandleTexGen(Context& kContext, 
    NiMaterialResource* pkInputUVResource, 
    NiMaterialResource*& pkOutputUVResource, const NiFixedString& kMapName, 
    unsigned int, NiVSMBlurMaterial::TexGenOutput eOutputType)
{
    NiFixedString kOutTexCoordType = NULL;
    NiMaterialResource* pkTexCoordRes = NULL;

    pkOutputUVResource = pkTexCoordRes = pkInputUVResource;

    if (!pkTexCoordRes)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kOutTexCoordType = "float2";
    NIASSERT(kOutTexCoordType != NULL);

    NiFixedString kVarName = kMapName;

    if (eOutputType == NiVSMBlurMaterial::TEX_OUT_TRANSFORM)
    {
        NiMaterialNode* pkNode = 
            GetAttachableNodeFromLibrary("TexTransformApply");
        kContext.m_spConfigurator->AddNode(pkNode);
        kContext.m_spConfigurator->AddBinding(pkTexCoordRes, 
            pkNode->GetInputResourceByVariableName("TexCoord"));

        char acName[32];
        NiSprintf(acName, 32, "%s_TexTransform", (const char*)
            kMapName);

        NiMaterialResource* pkTexTransform = 
        AddOutputPredefined(kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_DEF_TEXTRANSFORMBASE);

        kContext.m_spConfigurator->AddBinding(pkTexTransform, 
            pkNode->GetInputResourceByVariableName("TexTransform"));

        pkOutputUVResource = 
            pkNode->GetOutputResourceByVariableName("TexCoordOut"); 
    }
    else if (eOutputType != NiVSMBlurMaterial::TEX_OUT_PASSTHRU)
    {
        NIASSERT(!"Unsupported TexGenOutput type.");
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiVSMBlurMaterial::InsertTexture(Context& kContext, 
    NiMaterialResource* pkUV,
    NiMaterialResource*& pkOutputColor,
    NiMaterialNode*& pkNode,
    NiFixedString& kString,
    unsigned int uiBlurKernel)
{
    pkNode = GetAttachableNodeFromLibrary(kString);

    NiMaterialResource* pkSamplerRes = InsertTextureSampler(
        kContext, "Base", TEXTURE_SAMPLER_2D, 0);

    if (!pkSamplerRes)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    bool bSuccess = true;
    kContext.m_spConfigurator->AddNode(pkNode);
    bSuccess &= kContext.m_spConfigurator->AddBinding(
        pkUV, pkNode->GetInputResourceByVariableName("TexCoord"));
    bSuccess &= kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
        pkNode->GetInputResourceByVariableName("Sampler"));
    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("bool", "(false)"), 
        "Saturate", pkNode);

    NiMaterialResource* pkMapSize = AddOutputPredefined(kContext.m_spUniforms, 
        NiShaderConstantMap::SCM_DEF_TEXSIZEBASE);
    kContext.m_spConfigurator->AddBinding(pkMapSize, "MapSize", pkNode);

    char acKernelString[256];
    NiSprintf(acKernelString, 256, "(%d.0)", uiBlurKernel);

    kContext.m_spConfigurator->AddBinding(
        kContext.m_spStatics->AddOutputConstant("float", acKernelString),
        pkNode->GetInputResourceByVariableName("KernelSize"));



    if (!bSuccess)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    pkOutputColor = 
        pkNode->GetOutputResourceByVariableName("ColorOut");

    return bSuccess;
}
//---------------------------------------------------------------------------
