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

//---------------------------------------------------------------------------
// This file has been automatically generated using the
// NiMaterialNodeXMLLibraryParser tool. It should not be directly edited.
//---------------------------------------------------------------------------

#include "NiMainPCH.h"

#include <NiMaterialFragmentNode.h>
#include <NiMaterialNodeLibrary.h>
#include <NiMaterialResource.h>
#include <NiCodeBlock.h>
#include "NiStandardMaterialNodeLibrary.h"

//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment0(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("TransformNormal");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying the world transform "
        "to the\n"
        "    normal.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("Local");
        pkRes->SetVariable("Normal");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetSemantic("WorldMatrix");
        pkRes->SetVariable("World");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldNrm");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    // Transform the normal into world space for lighting\n"
             "    WorldNrm = mul( Normal, (float3x3)World );\n"
             "\n"
             "    // Should not need to normalize here since we will normal"
             "ize in the pixel \n"
             "    // shader due to linear interpolation across triangle not"
             " preserving\n"
             "    // normality.\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment1(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("TransformNBT");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying the world transform "
        "to the\n"
        "    normal, binormal, and tangent.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("Local");
        pkRes->SetVariable("Normal");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Binormal");
        pkRes->SetLabel("Local");
        pkRes->SetVariable("Binormal");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Tangent");
        pkRes->SetLabel("Local");
        pkRes->SetVariable("Tangent");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetSemantic("WorldMatrix");
        pkRes->SetVariable("World");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldNrm");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldBinormal");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldTangent");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    // Transform the normal into world space for lighting\n"
             "    WorldNrm      = mul( Normal, (float3x3)World );\n"
             "    WorldBinormal = mul( Binormal, (float3x3)World );\n"
             "    WorldTangent  = mul( Tangent, (float3x3)World );\n"
             "    \n"
             "    // Should not need to normalize here since we will normal"
             "ize in the pixel \n"
             "    // shader due to linear interpolation across triangle not"
             " preserving\n"
             "    // normality.\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment2(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("TransformPosition");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying the view projection "
        "transform\n"
        "    to the input position. Additionally, this fragment applies the"
        " world \n"
        "    transform to the input position. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Local");
        pkRes->SetVariable("Position");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetSemantic("WorldMatrix");
        pkRes->SetVariable("World");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    // Transform the position into world space for lighting, "
             "and projected \n"
             "    // space for display\n"
             "    WorldPos = mul( float4(Position, 1.0f), World );\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment3(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("WorldToTangent");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for transforming a vector from wo"
        "rld space\n"
        "    to tangent space.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetLabel("World");
        pkRes->SetVariable("VectorIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldNormalIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Binormal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldBinormalIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Tangent");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldTangentIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetLabel("Tangent");
        pkRes->SetVariable("VectorOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    float3x3 xForm = float3x3(WorldTangentIn, WorldBinormalIn"
             ", WorldNormalIn);\n"
             "    VectorOut = mul(xForm, VectorIn.xyz);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment4(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("TransformSkinnedPosition");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying the view projection "
        "and skinning \n"
        "    transform to the input position. Additionally, this fragment a"
        "pplies the \n"
        "    computed world transform to the input position. The weighted w"
        "orld \n"
        "    transform defined by the blendweights is output for use in nor"
        "mals or\n"
        "    other calculations as the new world matrix.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Local");
        pkRes->SetVariable("Position");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int4");
        pkRes->SetSemantic("BlendIndices");
        pkRes->SetVariable("BlendIndices");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("BlendWeight");
        pkRes->SetLabel("Local");
        pkRes->SetVariable("BlendWeights");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("BONEMATRIX_TYPE");
        pkRes->SetCount(30);
        pkRes->SetSemantic("SkinBoneMatrix");
        pkRes->SetLabel("World");
        pkRes->SetVariable("Bones");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetSemantic("WorldMatrix");
        pkRes->SetVariable("SkinBoneTransform");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    // TransformSkinnedPosition *****************************"
             "****************\n"
             "    // Transform the skinned position into world space\n"
             "    // Composite the skinning transform which will take the v"
             "ertex\n"
             "    // and normal to world space.\n"
             "    float fWeight3 = 1.0 - BlendWeights[0] - BlendWeights[1] "
             "- BlendWeights[2];\n"
             "    BONEMATRIX_TYPE ShortSkinBoneTransform;\n"
             "    ShortSkinBoneTransform  = Bones[BlendIndices[0]] * BlendW"
             "eights[0];\n"
             "    ShortSkinBoneTransform += Bones[BlendIndices[1]] * BlendW"
             "eights[1];\n"
             "    ShortSkinBoneTransform += Bones[BlendIndices[2]] * BlendW"
             "eights[2];\n"
             "    ShortSkinBoneTransform += Bones[BlendIndices[3]] * fWeigh"
             "t3;\n"
             "    SkinBoneTransform = float4x4(ShortSkinBoneTransform[0], 0"
             ".0f, \n"
             "        ShortSkinBoneTransform[1], 0.0f, \n"
             "        ShortSkinBoneTransform[2], 0.0f, \n"
             "        ShortSkinBoneTransform[3], 1.0f);\n"
             "\n"
             "    // Transform into world space.\n"
             "    WorldPos.xyz = mul(float4(Position, 1.0), ShortSkinBoneTr"
             "ansform);\n"
             "    WorldPos.w = 1.0f;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("Cg");
        pkBlock->SetPlatform("PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "      // TransformSkinnedPosition ***************************"
             "******************\n"
             "      // Transform the skinned position into world space\n"
             "      // Composite the skinning transform which will take the"
             " vertex\n"
             "      // and normal to world space.\n"
             "      float fWeight3 = 1.0 - BlendWeights[0] - BlendWeights[1"
             "] - BlendWeights[2];\n"
             "      BONEMATRIX_TYPE ShortSkinBoneTransform;\n"
             "      ShortSkinBoneTransform  = Bones[BlendIndices[0]] * Blen"
             "dWeights[0];\n"
             "      ShortSkinBoneTransform += Bones[BlendIndices[1]] * Blen"
             "dWeights[1];\n"
             "      ShortSkinBoneTransform += Bones[BlendIndices[2]] * Blen"
             "dWeights[2];\n"
             "      ShortSkinBoneTransform += Bones[BlendIndices[3]] * fWei"
             "ght3;\n"
             "      SkinBoneTransform = transpose(float4x4(ShortSkinBoneTra"
             "nsform[0],\n"
             "      ShortSkinBoneTransform[1],\n"
             "      ShortSkinBoneTransform[2],\n"
             "      float4(0.0f, 0.0f, 0.0f, 1.0f)));\n"
             "\n"
             "      // Transform into world space.\n"
             "      WorldPos.xyz = mul(ShortSkinBoneTransform, float4(Posit"
             "ion, 1.0));\n"
             "      WorldPos.w = 1.0f;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment5(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("ProjectPositionWorldToProj");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying the view projection "
        "transform\n"
        "    to the input world position.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPosition");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetSemantic("ViewProjMatrix");
        pkRes->SetVariable("ViewProjection");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Proj");
        pkRes->SetVariable("ProjPos");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    ProjPos = mul(WorldPosition, ViewProjection);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment6(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("ProjectPositionWorldToViewToProj");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying the view projection "
        "transform\n"
        "    to the input world position.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPosition");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetSemantic("ViewMatrix");
        pkRes->SetVariable("ViewTransform");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetSemantic("ProjMatrix");
        pkRes->SetVariable("ProjTransform");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("View");
        pkRes->SetVariable("ViewPos");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Proj");
        pkRes->SetVariable("ProjPos");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    ViewPos = mul(WorldPosition, ViewTransform);\n"
             "    ProjPos = mul(ViewPos, ProjTransform);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment7(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("9thOrderSphericalHarmonicLighting");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for generating the diffuse \n"
        "    lighting environment as compressed in spherical harmonics.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldNormal");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("SHCoefficients");
        pkRes->SetVariable("SHCoefficients");
        pkRes->SetCount(9);

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("DiffuseColorOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    DiffuseColorOut  = SHCoefficients[0];\n"
             "    DiffuseColorOut += SHCoefficients[1] * WorldNormal.x;\n"
             "    DiffuseColorOut += SHCoefficients[2] * WorldNormal.y;\n"
             "    DiffuseColorOut += SHCoefficients[3] * WorldNormal.z;\n"
             "    DiffuseColorOut += SHCoefficients[4] * WorldNormal.x * Wo"
             "rldNormal.z;\n"
             "    DiffuseColorOut += SHCoefficients[5] * WorldNormal.y * Wo"
             "rldNormal.z;\n"
             "    DiffuseColorOut += SHCoefficients[6] * WorldNormal.x * Wo"
             "rldNormal.y;\n"
             "    DiffuseColorOut += SHCoefficients[7] * \n"
             "        (3.0 * WorldNormal.z * WorldNormal.z  - 1.0);\n"
             "    DiffuseColorOut += SHCoefficients[8] * \n"
             "        (WorldNormal.x * WorldNormal.x  - WorldNormal.y * Wor"
             "ldNormal.y);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment8(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("CalculateNormalFromColor");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for sampling a normal map to gene"
        "rate the\n"
        "    new world-space normal.\n"
        "    \n"
        "    The normal map type is an enumerated value that indicates the "
        "following:\n"
        "        0 - Standard (rgb = normal/binormal/tangent)\n"
        "        1 - DXN (rg = normal.xy need to calculate z)\n"
        "        2 - DXT5 (ag = normal.xy need to calculate z)\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("NormalMap");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldNormalIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Binormal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldBinormalIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Tangent");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldTangentIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int");
        pkRes->SetSemantic("NormalMapType");
        pkRes->SetVariable("NormalMapType");
        pkRes->SetDefaultValue("(0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldNormalOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    \n"
             "    NormalMap = NormalMap * 2.0 - 1.0;\n"
             "    \n"
             "    // Do nothing extra for Standard\n"
             "    // Handle compressed types:\n"
             "    if (NormalMapType == 1) // DXN\n"
             "    {\n"
             "        NormalMap.rgb = float3(NormalMap.r, NormalMap.g, \n"
             "            sqrt(1 - NormalMap.r * NormalMap.r - NormalMap.g "
             "* NormalMap.g));\n"
             "    }\n"
             "    else if (NormalMapType == 2) // DXT5\n"
             "    {\n"
             "        NormalMap.rg = NormalMap.ag;\n"
             "        NormalMap.b = sqrt(1 - NormalMap.r*NormalMap.r -  \n"
             "            NormalMap.g * NormalMap.g);\n"
             "    }\n"
             "       \n"
             "    float3x3 xForm = float3x3(WorldTangentIn, WorldBinormalIn"
             ", WorldNormalIn);\n"
             "    xForm = transpose(xForm);\n"
             "    WorldNormalOut = mul(xForm, NormalMap.rgb);\n"
             "    \n"
             "    WorldNormalOut = normalize(WorldNormalOut);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment9(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("CalculateParallaxOffset");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating the UV offset to "
        "apply\n"
        "    as a result of a parallax map.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoord");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("Height");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("ParallaxOffsetScale");
        pkRes->SetVariable("OffsetScale");
        pkRes->SetDefaultValue("(0.05)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("ViewVector");
        pkRes->SetLabel("Tangent");
        pkRes->SetVariable("TangentSpaceEyeVec");
        pkRes->SetDefaultValue("(1.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("ParallaxOffsetUV");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    // Calculate offset scaling constant bias.\n"
             "    float2 Bias = float2(OffsetScale, OffsetScale) * -0.5;\n"
             "\n"
             "    // Calculate offset\n"
             "    float2 Offset = Height.rg * OffsetScale + Bias;\n"
             "\n"
             "    // Get texcoord.\n"
             "    ParallaxOffsetUV = TexCoord + Offset * TangentSpaceEyeVec"
             ".xy;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment10(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("CalculateBumpOffset");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating the UV offset to "
        "apply\n"
        "    as a result of a bump map.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("DuDv");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("BumpMatrix");
        pkRes->SetVariable("BumpMatrix");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("UVSet");
        pkRes->SetVariable("BumpOffset");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    BumpOffset.x = DuDv.x * BumpMatrix[0] + DuDv.y * BumpMatr"
             "ix[2];\n"
             "    BumpOffset.y = DuDv.x * BumpMatrix[1] + DuDv.y * BumpMatr"
             "ix[3];\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment11(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("OffsetUVFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying a UV offset to a tex"
        "ture\n"
        "    coordinate set.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoordIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoordOffset");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("UVSet");
        pkRes->SetVariable("TexCoordOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    TexCoordOut = TexCoordIn + TexCoordOffset;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment12(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("OffsetUVFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying a UV offset to a tex"
        "ture\n"
        "    coordinate set.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoordIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoordOffset");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("UVSet");
        pkRes->SetVariable("TexCoordOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    TexCoordOut = TexCoordIn + float3(TexCoordOffset.x, TexCo"
             "ordOffset.y, 0.0);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment13(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("OffsetUVFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying a UV offset to a tex"
        "ture\n"
        "    coordinate set.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoordIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoordOffset");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("UVSet");
        pkRes->SetVariable("TexCoordOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    TexCoordOut = TexCoordIn + \n"
             "        float4(TexCoordOffset.x, TexCoordOffset.y, 0.0, 0.0);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment14(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("TexTransformApply");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying a transform to the i"
        "nput set\n"
        "    of texture coordinates.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoord");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetSemantic("TexTransform");
        pkRes->SetVariable("TexTransform");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoordOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    \n"
             "    TexCoordOut = mul(float4(TexCoord.x, TexCoord.y, 0.0, 1.0"
             "), TexTransform);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment15(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("ProjectTextureCoordinates");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying a projection to the "
        "input set\n"
        "    of texture coordinates.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoord");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetSemantic("TexTransform");
        pkRes->SetVariable("TexTransform");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoordOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "  \n"
             "    TexCoordOut = mul(float4(TexCoord, 1.0), TexTransform);\n"
             "    \n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment16(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("TextureRGBSample");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for sampling a texture and return"
        "ing its value\n"
        "    as a RGB value.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoord");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("sampler2D");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("Sampler");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("bool");
        pkRes->SetVariable("Saturate");
        pkRes->SetDefaultValue("(true)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("ColorOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    ColorOut.rgb = tex2D(Sampler, TexCoord).rgb;\n"
             "    if (Saturate)\n"
             "    {\n"
             "        ColorOut.rgb = saturate(ColorOut.rgb);\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment17(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("TextureRGBASample");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for sampling a texture and return"
        "ing its value\n"
        "    as a RGB value and an A value.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoord");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("sampler2D");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("Sampler");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("bool");
        pkRes->SetVariable("Saturate");
        pkRes->SetDefaultValue("(true)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("ColorOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    ColorOut = tex2D(Sampler, TexCoord);\n"
             "    if (Saturate)\n"
             "    {\n"
             "        ColorOut = saturate(ColorOut);\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment18(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("TextureRGBProjectSample");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for sampling a texture and return"
        "ing its value\n"
        "    as a RGB value and an A value.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoord");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("sampler2D");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("Sampler");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("bool");
        pkRes->SetVariable("Saturate");
        pkRes->SetDefaultValue("(true)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("ColorOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    float4 ProjTexCoord = TexCoord.xyzz;\n"
             "    ColorOut.rgb = tex2Dproj(Sampler, ProjTexCoord).rgb;\n"
             "    if (Saturate)\n"
             "    {\n"
             "        ColorOut.rgb = saturate(ColorOut.rgb);\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment19(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("TextureRGBCubeSample");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for sampling a texture and return"
        "ing its value\n"
        "    as a RGB value.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("TexCoord");
        pkRes->SetVariable("TexCoord");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("samplerCUBE");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("Sampler");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("bool");
        pkRes->SetVariable("Saturate");
        pkRes->SetDefaultValue("(true)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("ColorOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    ColorOut.rgb = texCUBE(Sampler, TexCoord).rgb;\n"
             "    if (Saturate)\n"
             "    {\n"
             "        ColorOut.rgb = saturate(ColorOut.rgb);\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment20(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("CalculateFog");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for handling fogging calculations"
        ".\n"
        "    FogType can be one of 4 values:\n"
        "    \n"
        "        NONE   - 0\n"
        "        EXP    - 1\n"
        "        EXP2   - 2\n"
        "        LINEAR - 3\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int");
        pkRes->SetSemantic("FogType");
        pkRes->SetVariable("FogType");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("FogDensity");
        pkRes->SetVariable("FogDensity");
        pkRes->SetDefaultValue("(1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("View");
        pkRes->SetVariable("ViewPosition");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("bool");
        pkRes->SetSemantic("FogRange");
        pkRes->SetVariable("FogRange");
        pkRes->SetDefaultValue("(false)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("FogStartEnd");
        pkRes->SetVariable("FogStartEnd");
        pkRes->SetDefaultValue("(0.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Fog");
        pkRes->SetVariable("FogOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    float d;\n"
             "    if (FogRange)\n"
             "    {\n"
             "        d = length(ViewPosition);\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        d = ViewPosition.z;\n"
             "    }\n"
             "    \n"
             "    if (FogType == 0) // NONE\n"
             "    {\n"
             "        FogOut = 1.0;\n"
             "    }\n"
             "    else if (FogType == 1) // EXP\n"
             "    {\n"
             "        FogOut = 1.0 / exp( d * FogDensity);\n"
             "    }\n"
             "    else if (FogType == 2) // EXP2\n"
             "    {\n"
             "        FogOut = 1.0 / exp( pow( d * FogDensity, 2));\n"
             "    }\n"
             "    else if (FogType == 3) // LINEAR\n"
             "    {\n"
             "        FogOut = saturate( (FogStartEnd.y - d) / \n"
             "            (FogStartEnd.y - FogStartEnd.x));\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment21(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("WorldReflect");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for computing the reflection vect"
        "or.\n"
        "    The WorldViewVector is negated because the HLSL \"reflect\" fu"
        "nction\n"
        "    expects a world-to-camera vector, rather than a camera-to-worl"
        "d vector.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldNrm");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("ViewVector");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldViewVector");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("bool");
        pkRes->SetSemantic("NormalizeNormal");
        pkRes->SetVariable("NormalizeNormal");
        pkRes->SetDefaultValue("(true)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("WorldReflect");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    if (NormalizeNormal)\n"
             "        WorldNrm = normalize(WorldNrm);\n"
             "    WorldReflect = reflect(-WorldViewVector, WorldNrm);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment22(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("CalculateViewVector");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating the camera view v"
        "ector.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("CameraPosition");
        pkRes->SetLabel("World");
        pkRes->SetVariable("CameraPos");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("ViewVector");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldViewVector");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    WorldViewVector = CameraPos - WorldPos;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment23(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("ClippingPlaneTest");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating whether or not th"
        "e current\n"
        "    position is on the positive or negative side of a clipping pla"
        "ne. If the\n"
        "    point is on the positive side, the return value will be 1.0. I"
        "f the value \n"
        "    is on the negative side, the return value will be 0.0. If the "
        "point is on\n"
        "    the plane, the return value will be 0.0. If InvertClip is true"
        ", then the\n"
        "    fragment returns the opposite of the above.\n"
        "    \n"
        "    A plane is assumed to be the points X satisfying the expressio"
        "n:\n"
        "        X * normal = plane_constant\n"
        "        \n"
        "    The WorldClipPlane must match the form of NiPlane, which is of"
        " the form:\n"
        "        (normal.x, normal.y, normal.z, plane_constant)\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("ClippingPlane");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldClipPlane");
        pkRes->SetDefaultValue("(1.0, 0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("bool");
        pkRes->SetSemantic("InvertClip");
        pkRes->SetVariable("InvertClip");
        pkRes->SetDefaultValue("(false)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Scale");
        pkRes->SetLabel("World");
        pkRes->SetVariable("Scalar");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("       \n"
             "    float fDistance = dot(WorldClipPlane.xyz, WorldPos.xyz) -"
             " WorldClipPlane.w;\n"
             "    if (InvertClip)\n"
             "        Scalar = fDistance > 0.0 ? 0.0 : 1.0;\n"
             "    else\n"
             "        Scalar = fDistance > 0.0 ? 1.0 : 0.0;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment24(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("Light");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for accumulating the effect of a "
        "light\n"
        "    on the current pixel.\n"
        "    \n"
        "    LightType can be one of three values:\n"
        "        0 - Directional\n"
        "        1 - Point \n"
        "        2 - Spot\n"
        "        \n"
        "    Note that the LightType must be a compile-time variable,\n"
        "    not a runtime constant/uniform variable on most Shader Model 2"
        ".0 cards.\n"
        "    \n"
        "    The compiler will optimize out any constants that aren't used.\n"
        "    \n"
        "    Attenuation is defined as (const, linear, quad, range).\n"
        "    Range is not implemented at this time.\n"
        "    \n"
        "    SpotAttenuation is stored as (cos(theta/2), cos(phi/2), fallof"
        "f)\n"
        "    theta is the angle of the inner cone and phi is the angle of t"
        "he outer\n"
        "    cone in the traditional DX manner. Gamebryo only allows settin"
        "g of\n"
        "    phi, so cos(theta/2) will typically be cos(0) or 1. To disable"
        " spot\n"
        "    effects entirely, set cos(theta/2) and cos(phi/2) to -1 or low"
        "er.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Normal");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldNrm");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int");
        pkRes->SetSemantic("LightType");
        pkRes->SetVariable("LightType");
        pkRes->SetDefaultValue("(0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("bool");
        pkRes->SetSemantic("Specularity");
        pkRes->SetVariable("SpecularEnable");
        pkRes->SetDefaultValue("(false)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Shadow");
        pkRes->SetVariable("Shadow");
        pkRes->SetDefaultValue("(1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("ViewVector");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldViewVector");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("LightPos");
        pkRes->SetLabel("World");
        pkRes->SetVariable("LightPos");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("LightAmbient");
        pkRes->SetVariable("LightAmbient");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("LightDiffuse");
        pkRes->SetVariable("LightDiffuse");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("LightSpecular");
        pkRes->SetVariable("LightSpecular");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("LightAttenuation");
        pkRes->SetVariable("LightAttenuation");
        pkRes->SetDefaultValue("(0.0, 1.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("LightSpotAttenuation");
        pkRes->SetVariable("LightSpotAttenuation");
        pkRes->SetDefaultValue("(-1.0, -1.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("LightDirection");
        pkRes->SetVariable("LightDirection");
        pkRes->SetDefaultValue("(1.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Color");
        pkRes->SetLabel("Specular");
        pkRes->SetVariable("SpecularPower");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetLabel("Ambient");
        pkRes->SetVariable("AmbientAccum");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetLabel("Diffuse");
        pkRes->SetVariable("DiffuseAccum");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetLabel("Specular");
        pkRes->SetVariable("SpecularAccum");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetLabel("Ambient");
        pkRes->SetVariable("AmbientAccumOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetLabel("Diffuse");
        pkRes->SetVariable("DiffuseAccumOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetLabel("Specular");
        pkRes->SetVariable("SpecularAccumOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("   \n"
             "    // Get the world space light vector.\n"
             "    float3 LightVector;\n"
             "    float DistanceToLight;\n"
             "    float DistanceToLightSquared;\n"
             "        \n"
             "    if (LightType == 0)\n"
             "    {\n"
             "        LightVector = -LightDirection;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        LightVector = LightPos - WorldPos;\n"
             "        DistanceToLightSquared = dot(LightVector, LightVector"
             ");\n"
             "        DistanceToLight = length(LightVector);\n"
             "        LightVector = normalize(LightVector);\n"
             "    }\n"
             "    \n"
             "    // Take N dot L as intensity.\n"
             "    float LightNDotL = dot(LightVector, WorldNrm);\n"
             "    float LightIntensity = max(0, LightNDotL);\n"
             "\n"
             "    float Attenuate = Shadow;\n"
             "    \n"
             "    if (LightType != 0)\n"
             "    {\n"
             "        // Attenuate Here\n"
             "        Attenuate = LightAttenuation.x +\n"
             "            LightAttenuation.y * DistanceToLight +\n"
             "            LightAttenuation.z * DistanceToLightSquared;\n"
             "        Attenuate = max(1.0, Attenuate);\n"
             "        Attenuate = 1.0 / Attenuate;\n"
             "        Attenuate *= Shadow;\n"
             "\n"
             "        if (LightType == 2)\n"
             "        {\n"
             "            // Get intensity as cosine of light vector and di"
             "rection.\n"
             "            float CosAlpha = dot(-LightVector, LightDirection"
             ");\n"
             "\n"
             "            // Factor in inner and outer cone angles.\n"
             "            float AttenDiff = LightSpotAttenuation.x - LightS"
             "potAttenuation.y;\n"
             "            CosAlpha = saturate((CosAlpha - LightSpotAttenuat"
             "ion.y) / \n"
             "                AttenDiff);\n"
             "\n"
             "            // Power to falloff.\n"
             "            // The pow() here can create a NaN if CosAlpha is"
             " 0 or less.\n"
             "            // On some cards (GeForce 6800), the NaN will pro"
             "pagate through\n"
             "            // a ternary instruction, so we need two to be sa"
             "fe.\n"
             "            float origCosAlpha = CosAlpha;\n"
             "            CosAlpha = origCosAlpha <= 0.0 ? 1.0 : CosAlpha;\n"
             "            CosAlpha = pow(CosAlpha, LightSpotAttenuation.z);\n"
             "            CosAlpha = origCosAlpha <= 0.0 ? 0.0 : CosAlpha;\n"
             "\n"
             "            // Multiply the spot attenuation into the overall"
             " attenuation.\n"
             "            Attenuate *= CosAlpha;\n"
             "        }\n"
             "    }\n"
             "    // Determine the interaction of diffuse color of light an"
             "d material.\n"
             "    // Scale by the attenuated intensity.\n"
             "    DiffuseAccumOut = DiffuseAccum;\n"
             "    DiffuseAccumOut.rgb += LightDiffuse.rgb * LightIntensity "
             "* Attenuate;\n"
             "\n"
             "    // Determine ambient contribution - Is affected by shadow\n"
             "    AmbientAccumOut = AmbientAccum;\n"
             "    AmbientAccumOut.rgb += LightAmbient.rgb * Attenuate;\n"
             "\n"
             "    SpecularAccumOut = SpecularAccum;\n"
             "    if (SpecularEnable)\n"
             "    {\n"
             "        // Get the half vector.\n"
             "        float3 LightHalfVector = LightVector + WorldViewVecto"
             "r;\n"
             "        LightHalfVector = normalize(LightHalfVector);\n"
             "\n"
             "        // Determine specular intensity.\n"
             "        float LightNDotH = max(0, dot(WorldNrm, LightHalfVect"
             "or));\n"
             "        float LightSpecIntensity = pow(LightNDotH, SpecularPo"
             "wer.x);\n"
             "        \n"
             "        //if (LightNDotL < 0.0)\n"
             "        //    LightSpecIntensity = 0.0;\n"
             "        // Must use the code below rather than code above.\n"
             "        // Using previous lines will cause the compiler to ge"
             "nerate incorrect\n"
             "        // output.\n"
             "        float SpecularMultiplier = LightNDotL > 0.0 ? 1.0 : 0"
             ".0;\n"
             "        \n"
             "        // Attenuate Here\n"
             "        LightSpecIntensity = LightSpecIntensity * Attenuate *"
             " \n"
             "            SpecularMultiplier;\n"
             "        \n"
             "        // Determine the interaction of specular color of lig"
             "ht and material.\n"
             "        // Scale by the attenuated intensity.\n"
             "        SpecularAccumOut.rgb += LightSpecIntensity * LightSpe"
             "cular;\n"
             "    }       \n"
             "\n"
             "    \n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment25(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("ApplyFog");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for applying the fog based on the"
        " \n"
        "    calculations in the vertex shader.\n"
        "    \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("UnfoggedColor");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("FogColor");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Fog");
        pkRes->SetVariable("FogAmount");
        pkRes->SetDefaultValue("(1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("FoggedColor");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    FoggedColor = lerp(FogColor, UnfoggedColor, FogAmount);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment26(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex");
    pkFrag->SetName("ApplyAlphaTest");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for alpha testing based on the al"
        "pha\n"
        "    reference value and alpha test function.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Alpha");
        pkRes->SetVariable("AlphaTestValue");
        pkRes->SetDefaultValue("(1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Function");
        pkRes->SetVariable("AlphaTestFunction");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Fog");
        pkRes->SetVariable("AlphaTestRef");
        pkRes->SetDefaultValue("(0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    // These tests will evaluate to less than 0 if they are t"
             "rue.\n"
             "    float fLessTest = (AlphaTestValue - AlphaTestRef);\n"
             "    float fGreaterTest = -fLessTest;\n"
             "    float fEqualTest = 0.0;\n"
             "    if (fLessTest == 0.0)\n"
             "        fEqualTest = -1.0;\n"
             "\n"
             "    // Clip if AlphaTestValue < AlphaTestRef for:\n"
             "    //  TEST_LESS \n"
             "    //  TEST_EQUAL \n"
             "    //  TEST_LESSEQUAL \n"
             "    clip(fGreaterTest * AlphaTestFunction.x);\n"
             "    \n"
             "    // Clip if AlphaTestValue > AlphaTestRef for:\n"
             "    //  TEST_EQUAL \n"
             "    //  TEST_GREATER \n"
             "    //  TEST_GREATEREQUAL \n"
             "    clip(fLessTest * AlphaTestFunction.y);\n"
             "    \n"
             "    // Clip if AlphaTestValue == AlphaTestRef for:\n"
             "    //  TEST_LESS \n"
             "    //  TEST_GREATER \n"
             "    //  TEST_NOTEQUAL \n"
             "    clip(fEqualTest * AlphaTestFunction.z);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment27(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("ComputeShadingCoefficients");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for computing the coefficients fo"
        "r the \n"
        "    following equations:\n"
        "    \n"
        "    Kdiffuse = MatEmissive + \n"
        "        MatAmbient * Summation(0...N){LightAmbientContribution[N]}"
        " + \n"
        "        MatDiffuse * Summation(0..N){LightDiffuseContribution[N]}\n"
        "        \n"
        "    Kspecular = MatSpecular * Summation(0..N){LightSpecularContrib"
        "ution[N]}\n"
        "    \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("MatEmissive");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("MatDiffuse");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("MatAmbient");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("MatSpecular");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("LightSpecularAccum");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("LightDiffuseAccum");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("LightAmbientAccum");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("bool");
        pkRes->SetVariable("Saturate");
        pkRes->SetDefaultValue("(true)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("Diffuse");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("Specular");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Diffuse = MatEmissive + MatAmbient * LightAmbientAccum + \n"
             "        MatDiffuse * LightDiffuseAccum;\n"
             "    Specular = MatSpecular * LightSpecularAccum;\n"
             "    \n"
             "    if (Saturate)\n"
             "    {\n"
             "        Diffuse = saturate(Diffuse);\n"
             "        Specular = saturate(Specular);\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment28(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("SplitColorAndOpacity");
    pkFrag->SetDescription("\n"
        "    Separate a float4 into a float3 and a float.   \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("ColorAndOpacity");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("Color");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Opacity");
        pkRes->SetVariable("Opacity");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Color.rgb = ColorAndOpacity.rgb;\n"
             "    Opacity = ColorAndOpacity.a;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment29(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("SplitRGBA");
    pkFrag->SetDescription("\n"
        "    Separate a float4 into 4 floats.   \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("ColorAndOpacity");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Red");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Green");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Blue");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Opacity");
        pkRes->SetVariable("Alpha");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Red = ColorAndOpacity.r;\n"
             "    Green = ColorAndOpacity.g;\n"
             "    Blue = ColorAndOpacity.b;\n"
             "    Alpha = ColorAndOpacity.a;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment30(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("CompositeFinalRGBColor");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for computing the final RGB color"
        ".\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("DiffuseColor");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("SpecularColor");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("OutputColor");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    OutputColor.rgb = DiffuseColor.rgb + SpecularColor.rgb;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment31(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("CompositeFinalRGBAColor");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for computing the final RGBA colo"
        "r.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("FinalColor");
        pkRes->SetDefaultValue("(0.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("FinalOpacity");
        pkRes->SetDefaultValue("(1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("OutputColor");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    OutputColor.rgb = FinalColor.rgb;\n"
             "    OutputColor.a = saturate(FinalOpacity);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment32(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("AddFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for adding two float4's. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = V1 + V2;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment33(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("AddFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for adding two float3's. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = V1 + V2;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment34(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("AddFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for adding two float2's. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = V1 + V2;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment35(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("AddFloat");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for adding two floats. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = V1 + V2;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment36(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("MultiplyFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for multiplying two float4's. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = V1 * V2;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment37(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("MultiplyFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for multiplying two float3's. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = V1 * V2;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment38(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("MultiplyFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for multiplying two float2's. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = V1 * V2;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment39(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("MultiplyFloat");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for multiplying two floats. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = V1 * V2;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment40(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("ScaleFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for scaling a float4 by a constan"
        "t. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Scale");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = Scale * V1;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment41(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("ScaleFloat3");
    pkFrag->SetDescription("    \n"
        "    This fragment is responsible for scaling a float3 by a constan"
        "t. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Scale");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = Scale * V1;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment42(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("ScaleFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for scaling a float2 by a constan"
        "t. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Scale");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = Scale * V1;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment43(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("SaturateFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for saturating a float4. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = saturate(V1);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment44(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("SaturateFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for saturating a float3. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = saturate(V1);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment45(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("SaturateFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for saturating a float2. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = saturate(V1);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment46(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("SaturateFloat");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for saturating a float. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = saturate(V1);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment47(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("LerpFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for linearly interpolating two fl"
        "oat4's. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("LerpAmount");
        pkRes->SetDefaultValue("(0.5)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = lerp(V1, V2, LerpAmount);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment48(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("LerpFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for linearly interpolating two fl"
        "oat3's. \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("LerpAmount");
        pkRes->SetDefaultValue("(0.5)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = lerp(V1, V2, LerpAmount);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment49(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("LerpFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for linearly interpolating two fl"
        "oat2's.   \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("LerpAmount");
        pkRes->SetDefaultValue("(0.5)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = lerp(V1, V2, LerpAmount);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment50(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("LerpFloat");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for linearly interpolating two fl"
        "oats.   \n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("V1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("V2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("LerpAmount");
        pkRes->SetDefaultValue("(0.5)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = lerp(V1, V2, LerpAmount);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment51(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("NormalizeFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for normalizing a float4.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("VectorIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("VectorOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    VectorOut = normalize(VectorIn);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment52(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("NormalizeFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for normalizing a float3.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("VectorIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("VectorOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    VectorOut = normalize(VectorIn);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment53(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("NormalizeFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for normalizing a float2.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("VectorIn");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("VectorOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "    VectorOut = normalize(VectorIn);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment54(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("MultiplyScalarSatAddFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for accumulating into the current"
        " color by\n"
        "    multiplying the sampled texture color with the current color. "
        "The \n"
        "    original input alpha channel is preserved. The scalar value is"
        " added to the\n"
        "    sampled color value and then clamped to the range [0.0, 1.0].\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("V1");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("V2");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Scalar");
        pkRes->SetVariable("Scalar");
        pkRes->SetDefaultValue("(0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = V1 * saturate(V2.rgb + Scalar.rrr);\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment55(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("MultiplyAddFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment implements the equation:\n"
        "    Output = (V1 * V2) + V3\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("V1");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("V2");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Scalar");
        pkRes->SetVariable("V3");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = (V1 * V2) + V3;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment56(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("FloatToFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment implements the operation:\n"
        "    Output = float3(Input, Input, Input);\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Input");
        pkRes->SetDefaultValue("(1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = Input.rrr;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment57(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("FloatToFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment implements the operation:\n"
        "    Output = float4(Input, Input, Input, Input);\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("Input");
        pkRes->SetDefaultValue("(1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = Input.rrrr;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment58(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("Float4ToFloat");
    pkFrag->SetDescription("\n"
        "    This fragment implements the operation:\n"
        "    Output = Input.r;\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Input");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = Input.r;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment59(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("Float3ToFloat");
    pkFrag->SetDescription("\n"
        "    This fragment implements the operation:\n"
        "    Output = Input.r;\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetVariable("Input");
        pkRes->SetDefaultValue("(1.0, 1.0, 1.0, 1.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Color");
        pkRes->SetVariable("Output");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    Output = Input.r;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment60(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("ShadowMap");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating the Shadow coeffi"
        "cient.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int");
        pkRes->SetSemantic("LightType");
        pkRes->SetVariable("LightType");
        pkRes->SetDefaultValue("(0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("sampler2D");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("ShadowMap");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("ShadowMapSize");
        pkRes->SetDefaultValue("(512,512)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetVariable("WorldToLightProjMat");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("ShadowBias");
        pkRes->SetDefaultValue("(0.005)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Shadow");
        pkRes->SetVariable("ShadowOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    \n"
             "    if ( LightType != 0)\n"
             "    {\n"
             "      // This fragment only supports directional lights.\n"
             "        ShadowOut = 0.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float4 LightProjPos = mul(WorldPos, WorldToLightProjM"
             "at);\n"
             "        float2 ShadowTexC = 0.5 * LightProjPos.xy / LightProj"
             "Pos.w + \n"
             "                  float2( 0.5, 0.5 );\n"
             "    \n"
             "        ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "\n"
             "        float ShadowLookup = tex2D(ShadowMap, ShadowTexC.xy)."
             "r;\n"
             "        float LightSpaceDepth = saturate(LightProjPos.z / Lig"
             "htProjPos.w);\n"
             "        float2 borderTest = saturate(ShadowTexC) - ShadowTexC"
             ";\n"
             "\n"
             "        if ( (ShadowLookup >= LightSpaceDepth - ShadowBias) |"
             "|\n"
             "            any(borderTest))\n"
             "        {\n"
             "            ShadowOut = 1.0;\n"
             "        }\n"
             "        else\n"
             "        {\n"
             "            ShadowOut = 0.0;\n"
             "        }\n"
             "        \n"
             "       }\n"
             "\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment61(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("VSMShadowMap");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating the Shadow coeffi"
        "cient.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int");
        pkRes->SetSemantic("LightType");
        pkRes->SetVariable("LightType");
        pkRes->SetDefaultValue("(0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("sampler2D");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("ShadowMap");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("ShadowMapSize");
        pkRes->SetDefaultValue("(512,512)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetVariable("WorldToLightProjMat");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("ShadowBias");
        pkRes->SetDefaultValue("(0.005)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("ShadowVSMPowerEpsilon");
        pkRes->SetDefaultValue("(10.0, 0.001)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Shadow");
        pkRes->SetVariable("ShadowOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("    \n"
             "    if ( LightType != 0)\n"
             "    {\n"
             "      // This fragment only supports directional lights.\n"
             "        ShadowOut = 0.0;\n"
             "    }\n"
             "    else\n"
             "    { \n"
             "        float4 LightProjPos = mul(WorldPos, WorldToLightProjM"
             "at);\n"
             "        float2 ShadowTexC = 0.5 * LightProjPos.xy / LightProj"
             "Pos.w + \n"
             "            float2( 0.5, 0.5 );\n"
             "        ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "        \n"
             "        float LightSpaceDepth = saturate(LightProjPos.z / Lig"
             "htProjPos.w);\n"
             "        float2 borderTest = saturate(ShadowTexC) - ShadowTexC"
             ";\n"
             "\n"
             "        float4 vVSM   = tex2D( ShadowMap, ShadowTexC.xy );\n"
             "        float  fAvgZ  = vVSM.r; // Filtered z\n"
             "        float  fAvgZ2 = vVSM.g; // Filtered z-squared\n"
             "        \n"
             "        // Standard shadow map comparison\n"
             "        if((LightSpaceDepth) - ShadowBias <= fAvgZ || any(bor"
             "derTest))\n"
             "        {\n"
             "            ShadowOut = 1.0f;\n"
             "        }\n"
             "        else\n"
             "        {            \n"
             "            // Use variance shadow mapping to compute the max"
             "imum probability \n"
             "            // that the pixel is in shadow\n"
             "            float variance = ( fAvgZ2 ) - ( fAvgZ * fAvgZ );\n"
             "            variance       = \n"
             "                min( 1.0f, max( 0.0f, variance + ShadowVSMPow"
             "erEpsilon.y ) );\n"
             "            ShadowOut = variance;\n"
             "            \n"
             "            float mean     = fAvgZ;\n"
             "            float d        = LightSpaceDepth - mean;\n"
             "            float p_max    = variance / ( variance + d*d );\n"
             "            \n"
             "            ShadowOut = pow( p_max, ShadowVSMPowerEpsilon.x);\n"
             "        }\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment62(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("PCFShadowMap");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating the Shadow coeffi"
        "cient.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int");
        pkRes->SetSemantic("LightType");
        pkRes->SetVariable("LightType");
        pkRes->SetDefaultValue("(0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("sampler2D");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("ShadowMap");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("ShadowMapSize");
        pkRes->SetDefaultValue("(512,512)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetVariable("WorldToLightProjMat");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("ShadowBias");
        pkRes->SetDefaultValue("(0.005)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Shadow");
        pkRes->SetVariable("ShadowOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("DX9");
        pkBlock->SetTarget("ps_2_0");

        pkBlock->SetText("\n"
             "    \n"
             "    if ( LightType != 0)\n"
             "    {\n"
             "      // This fragment only supports directional lights.\n"
             "        ShadowOut = 0.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float4 LightProjPos = mul(WorldPos, WorldToLightProjM"
             "at);\n"
             "        float2 ShadowTexC = 0.5 * LightProjPos.xy / LightProj"
             "Pos.w + \n"
             "                  float2( 0.5, 0.5 );\n"
             "    \n"
             "        ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "    \n"
             "        float ShadowMapSizeInverse = 1.0f / ShadowMapSize;\n"
             "        float2 lerps = frac( ShadowTexC * ShadowMapSize);\n"
             "        float LightSpaceDepth = saturate(LightProjPos.z / Lig"
             "htProjPos.w);\n"
             "        float2 borderTest = saturate(ShadowTexC) - ShadowTexC"
             ";\n"
             "\n"
             "        if (any(borderTest))\n"
             "        {\n"
             "            ShadowOut = 1.0;\n"
             "        }\n"
             "        else\n"
             "        {\n"
             "            LightSpaceDepth -= ShadowBias;\n"
             "                   \n"
             "            float4 SourceVals;\n"
             "            SourceVals.x = tex2D( ShadowMap, ShadowTexC ).r;\n"
             "    \n"
             "            ShadowTexC.x += ShadowMapSizeInverse;\n"
             "            SourceVals.y = tex2D( ShadowMap, ShadowTexC).r;  \n"
             "    \n"
             "            ShadowTexC.y += ShadowMapSizeInverse;            "
             "    \n"
             "            SourceVals.w = tex2D( ShadowMap, ShadowTexC ).r;\n"
             "                \n"
             "            ShadowTexC.x -= ShadowMapSizeInverse;            "
             "    \n"
             "            SourceVals.z = tex2D( ShadowMap, ShadowTexC).r;\n"
             "                \n"
             "            float4 Shade = (LightSpaceDepth <= SourceVals);\n"
             "    \n"
             "            // lerp between the shadow values to calculate ou"
             "r light amount\n"
             "            ShadowOut = lerp(\n"
             "                lerp(Shade.x, Shade.y, lerps.x),\n"
             "                lerp(Shade.z, Shade.w, lerps.x), lerps.y );    "
             "    \n"
             "        }\n"
             "    }\n"
             "\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("DX9/D3D10/Xenon/PS3");
        pkBlock->SetTarget("ps_3_0/ps_4_0");

        pkBlock->SetText("\n"
             "    \n"
             "    if ( LightType != 0)\n"
             "    {\n"
             "      // This fragment only supports directional lights.\n"
             "        ShadowOut = 0.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float4 LightProjPos = mul(WorldPos, WorldToLightProjM"
             "at);\n"
             "        float2 ShadowTexC = 0.5 * LightProjPos.xy / LightProj"
             "Pos.w + \n"
             "                  float2( 0.5, 0.5 );\n"
             "        \n"
             "        ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "        \n"
             "        float ShadowMapSizeInverse = 1.0f / ShadowMapSize;\n"
             "        float LightSpaceDepth = saturate(LightProjPos.z / Lig"
             "htProjPos.w);\n"
             "        float2 borderTest = saturate(ShadowTexC) - ShadowTexC"
             ";        \n"
             "\n"
             "        if (any(borderTest))\n"
             "        {\n"
             "            ShadowOut = 1.0;\n"
             "        }\n"
             "        else\n"
             "        {    \n"
             "            LightSpaceDepth -= ShadowBias;\n"
             "        \n"
             "            ShadowOut = 0;\n"
             "            float4 fOnes = float4(1.0, 1.0, 1.0, 1.0);\n"
             "            float4 fKernels = float4(1.0, 1.0, 1.0, 1.0);\n"
             "            float faKernels[4] = {1.0, 1.0, 1.0, 1.0};\n"
             "            \n"
             "            faKernels[0] = 1-frac(ShadowTexC.y * ShadowMapSiz"
             "e.y);\n"
             "            faKernels[3]= frac(ShadowTexC.y  * ShadowMapSize."
             "y); \n"
             "            \n"
             "            fKernels.x = 1-frac(ShadowTexC.x * ShadowMapSize."
             "x);\n"
             "            fKernels.w = frac(ShadowTexC.x  * ShadowMapSize.x"
             ");\n"
             "                      \n"
             "            float fTotalPercent = 0;\n"
             "            \n"
             "            // This loop is manually unrolled here to avoid l"
             "ong\n"
             "            // shader compilation times.\n"
             "            //for (int i=0; i < 4; i++)\n"
             "            // i == 0\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC;                \n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "0];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[0];\n"
             "            }\n"
             "            // i == 1\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC;                \n"
             "                pos.y += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "1];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[1];\n"
             "            }\n"
             "            // i == 2\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC;                \n"
             "                pos.y += 2 * ShadowMapSizeInverse;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "2];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[2];\n"
             "            }\n"
             "            // i == 3\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC;                \n"
             "                pos.y += 3 * ShadowMapSizeInverse;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "3];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[3];\n"
             "            }\n"
             "            \n"
             "            ShadowOut = ShadowOut / fTotalPercent;         \n"
             "        }\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment63(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("SpotShadowMap");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating the Shadow coeffi"
        "cient.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int");
        pkRes->SetSemantic("LightType");
        pkRes->SetVariable("LightType");
        pkRes->SetDefaultValue("(0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("sampler2D");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("ShadowMap");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("ShadowMapSize");
        pkRes->SetDefaultValue("(512,512)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("LightDirection");
        pkRes->SetVariable("LightDirection");
        pkRes->SetDefaultValue("(1.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Light");
        pkRes->SetVariable("LightPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("CosOfCutoff");
        pkRes->SetDefaultValue("(0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetVariable("WorldToLightProjMat");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("ShadowBias");
        pkRes->SetDefaultValue("(0.0005)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Shadow");
        pkRes->SetVariable("ShadowOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    if (LightType != 2)\n"
             "    {\n"
             "        // This version only supports spotlights.\n"
             "        ShadowOut = 1.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float3 LightToWorldPos = normalize(float3(WorldPos - "
             "LightPos));\n"
             "\n"
             "        if (dot(LightDirection, LightToWorldPos) > CosOfCutof"
             "f)\n"
             "        {\n"
             "            float4 LightProjPos = mul(WorldPos, WorldToLightP"
             "rojMat);\n"
             "\n"
             "            float3 ShadowTexC = LightProjPos.xyz / LightProjP"
             "os.w;               \n"
             "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2( 0"
             ".5, 0.5 );\n"
             "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "\n"
             "            float LightSpaceDepth = clamp(ShadowTexC.z - Shad"
             "owBias, \n"
             "                0.0, 1.0);\n"
             "            float ShadowLookup = tex2D(ShadowMap, ShadowTexC."
             "xy).r;                \n"
             "            ShadowOut = ShadowLookup < LightSpaceDepth ? 0.0 "
             ": 1.0;\n"
             "        }\n"
             "        else\n"
             "        {\n"
             "            // Outside of the light cone is shadowed complete"
             "ly\n"
             "            ShadowOut = 0.0;\n"
             "        }\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("PS3");
        pkBlock->SetTarget("ps_3_0");

        pkBlock->SetText("\n"
             "    if (LightType != 2)\n"
             "    {\n"
             "        // This version only supports spotlights.\n"
             "        ShadowOut = 1.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float3 LightToWorldPos = normalize(float3(WorldPos - "
             "LightPos));\n"
             "\n"
             "        if (dot(LightDirection, LightToWorldPos) > CosOfCutof"
             "f)\n"
             "        {\n"
             "            float4 LightProjPos = mul(WorldPos, WorldToLightP"
             "rojMat);\n"
             "\n"
             "            float3 ShadowTexC = LightProjPos.xyz / LightProjP"
             "os.w;               \n"
             "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2( 0"
             ".5, 0.5 );\n"
             "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "\n"
             "            // Rescale Depth from (-1.0)-(1.0) to (0.0)-(1.0)\n"
             "            ShadowTexC.z = (ShadowTexC.z * 0.5) + 0.5;\n"
             "\n"
             "            float LightSpaceDepth = clamp(ShadowTexC.z - Shad"
             "owBias, \n"
             "                0.0, 1.0);\n"
             "            float ShadowLookup = tex2D(ShadowMap, ShadowTexC."
             "xy).r;                \n"
             "            ShadowOut = ShadowLookup < LightSpaceDepth ? 0.0 "
             ": 1.0;\n"
             "        }\n"
             "        else\n"
             "        {\n"
             "            // Outside of the light cone is shadowed complete"
             "ly\n"
             "            ShadowOut = 0.0;\n"
             "        }\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment64(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("SpotVSMShadowMap");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating the Shadow coeffi"
        "cient.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int");
        pkRes->SetSemantic("LightType");
        pkRes->SetVariable("LightType");
        pkRes->SetDefaultValue("(0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("sampler2D");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("ShadowMap");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("ShadowMapSize");
        pkRes->SetDefaultValue("(512,512)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("LightDirection");
        pkRes->SetVariable("LightDirection");
        pkRes->SetDefaultValue("(1.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Light");
        pkRes->SetVariable("LightPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("CosOfCutoff");
        pkRes->SetDefaultValue("(0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetVariable("WorldToLightProjMat");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("ShadowBias");
        pkRes->SetDefaultValue("(0.0005)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("ShadowVSMPowerEpsilon");
        pkRes->SetDefaultValue("(10.0, 0.001)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Shadow");
        pkRes->SetVariable("ShadowOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    if (LightType != 2)\n"
             "    {\n"
             "        // This version only supports spotlights.\n"
             "        ShadowOut = 1.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float3 LightToWorldPos = normalize(float3(WorldPos - "
             "LightPos));\n"
             "\n"
             "        if (dot(LightDirection, LightToWorldPos) > CosOfCutof"
             "f)\n"
             "        {\n"
             "            float4 LightProjPos = mul(WorldPos, WorldToLightP"
             "rojMat);\n"
             "            float3 ShadowTexC = LightProjPos.xyz / LightProjP"
             "os.w;               \n"
             "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2( 0"
             ".5, 0.5 );\n"
             "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "            float LightSpaceDepth = clamp(ShadowTexC.z - Shad"
             "owBias, \n"
             "                0.0, 1.0);\n"
             "                                                        \n"
             "            float4 vVSM   = tex2D( ShadowMap, ShadowTexC.xy )"
             ";\n"
             "            float  fAvgZ  = vVSM.r; // Filtered z\n"
             "            float  fAvgZ2 = vVSM.g; // Filtered z-squared\n"
             "            \n"
             "            // Standard shadow map comparison\n"
             "            if( (LightSpaceDepth - ShadowBias) <= fAvgZ)\n"
             "            {\n"
             "                ShadowOut = 1.0f;\n"
             "            }\n"
             "            else\n"
             "            {               \n"
             "                // Use variance shadow mapping to compute the"
             " maximum \n"
             "                // probability that the pixel is in shadow\n"
             "                float variance = ( fAvgZ2 ) - ( fAvgZ * fAvgZ"
             " );\n"
             "                variance       = \n"
             "                    min( 1.0f, max( 0.0f, variance + ShadowVS"
             "MPowerEpsilon.y));\n"
             "                ShadowOut = variance;\n"
             "                \n"
             "                float mean     = fAvgZ;\n"
             "                float d        = LightSpaceDepth - mean;\n"
             "                float p_max    = variance / ( variance + d*d "
             ");\n"
             "                \n"
             "                ShadowOut = pow( p_max, ShadowVSMPowerEpsilon"
             ".x);\n"
             "            }\n"
             "\n"
             "        }\n"
             "        else\n"
             "        {\n"
             "            // Outside of the light cone is shadowed complete"
             "ly\n"
             "            ShadowOut = 0.0;\n"
             "        }\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("PS3");
        pkBlock->SetTarget("ps_3_0");

        pkBlock->SetText("\n"
             "    if (LightType != 2)\n"
             "    {\n"
             "        // This version only supports spotlights.\n"
             "        ShadowOut = 1.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float3 LightToWorldPos = normalize(float3(WorldPos - "
             "LightPos));\n"
             "\n"
             "        if (dot(LightDirection, LightToWorldPos) > CosOfCutof"
             "f)\n"
             "        {\n"
             "            float4 LightProjPos = mul(WorldPos, WorldToLightP"
             "rojMat);\n"
             "            float3 ShadowTexC = LightProjPos.xyz / LightProjP"
             "os.w;               \n"
             "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2( 0"
             ".5, 0.5 );\n"
             "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "                       \n"
             "            // Rescale Depth from (-1.0)-(1.0) to (0.0)-(1.0)\n"
             "            ShadowTexC.z = (ShadowTexC.z * 0.5) + 0.5;       "
             "                \n"
             "                       \n"
             "            float LightSpaceDepth = clamp(ShadowTexC.z - Shad"
             "owBias, \n"
             "                0.0, 1.0);\n"
             "                                                        \n"
             "            float4 vVSM   = tex2D( ShadowMap, ShadowTexC.xy )"
             ";\n"
             "            float  fAvgZ  = vVSM.r; // Filtered z\n"
             "            float  fAvgZ2 = vVSM.g; // Filtered z-squared\n"
             "            \n"
             "            // Standard shadow map comparison\n"
             "            if( (LightSpaceDepth - ShadowBias) <= fAvgZ)\n"
             "            {\n"
             "                ShadowOut = 1.0f;\n"
             "            }\n"
             "            else\n"
             "            {               \n"
             "                // Use variance shadow mapping to compute the"
             " maximum \n"
             "                // probability that the pixel is in shadow\n"
             "                float variance = ( fAvgZ2 ) - ( fAvgZ * fAvgZ"
             " );\n"
             "                variance       = \n"
             "                    min( 1.0f, max( 0.0f, variance + ShadowVS"
             "MPowerEpsilon.y));\n"
             "                ShadowOut = variance;\n"
             "                \n"
             "                float mean     = fAvgZ;\n"
             "                float d        = LightSpaceDepth - mean;\n"
             "                float p_max    = variance / ( variance + d*d "
             ");\n"
             "                \n"
             "                ShadowOut = pow( p_max, ShadowVSMPowerEpsilon"
             ".x);\n"
             "            }\n"
             "\n"
             "        }\n"
             "        else\n"
             "        {\n"
             "            // Outside of the light cone is shadowed complete"
             "ly\n"
             "            ShadowOut = 0.0;\n"
             "        }\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment65(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("ShadowCubeMap");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating the Shadow coeffi"
        "cient.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int");
        pkRes->SetSemantic("LightType");
        pkRes->SetVariable("LightType");
        pkRes->SetDefaultValue("(0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("samplerCUBE");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("ShadowMap");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("ShadowMapSize");
        pkRes->SetDefaultValue("(512,512)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Light");
        pkRes->SetVariable("LightPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("ShadowBias");
        pkRes->SetDefaultValue("(0.0005)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Shadow");
        pkRes->SetVariable("ShadowOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    if (LightType == 0 || LightType == 2)\n"
             "    {\n"
             "        // This version only supports point lights.\n"
             "        ShadowOut = 1.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float3 ViewVector = WorldPos - LightPos;    \n"
             "        ViewVector.z = -ViewVector.z;\n"
             "        float3 ViewVectorNrm = normalize(ViewVector);\n"
             "\n"
             "        float fDepth = dot(ViewVector, ViewVector);\n"
             "        fDepth = sqrt(fDepth) * ShadowBias;\n"
             "        float ShadowLookup = texCUBE(ShadowMap, ViewVectorNrm"
             ").r;\n"
             "         \n"
             "        if (ShadowLookup == 0 || ShadowLookup > fDepth )\n"
             "            ShadowOut = 1.0;\n"
             "        else\n"
             "            ShadowOut = 0.0;\n"
             "    }\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment66(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("SpotPCFShadowMap");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for calculating the Shadow coeffi"
        "cient.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("WorldPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("int");
        pkRes->SetSemantic("LightType");
        pkRes->SetVariable("LightType");
        pkRes->SetDefaultValue("(0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("sampler2D");
        pkRes->SetSemantic("Texture");
        pkRes->SetVariable("ShadowMap");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetVariable("ShadowMapSize");
        pkRes->SetDefaultValue("(512,512)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("LightDirection");
        pkRes->SetVariable("LightDirection");
        pkRes->SetDefaultValue("(1.0, 0.0, 0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float3");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Light");
        pkRes->SetVariable("LightPos");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("CosOfCutoff");
        pkRes->SetDefaultValue("(0.0)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetVariable("WorldToLightProjMat");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("ShadowBias");
        pkRes->SetDefaultValue("(0.0005)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetSemantic("Shadow");
        pkRes->SetVariable("ShadowOut");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("DX9");
        pkBlock->SetTarget("ps_2_0");

        pkBlock->SetText("\n"
             "    if (LightType != 2)\n"
             "    {\n"
             "        // This version only supports spotlights.\n"
             "        ShadowOut = 1.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float3 LightToWorldPos = normalize(float3(WorldPos - "
             "LightPos));\n"
             "\n"
             "        if (dot(LightDirection, LightToWorldPos) > CosOfCutof"
             "f)\n"
             "        {\n"
             "            float4 LightProjPos = mul(WorldPos, WorldToLightP"
             "rojMat);\n"
             "            float3 ShadowTexC = LightProjPos.xyz / LightProjP"
             "os.w;               \n"
             "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2( 0"
             ".5, 0.5 );\n"
             "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "            float LightSpaceDepth = clamp(ShadowTexC.z - Shad"
             "owBias, \n"
             "                0.0, 1.0);\n"
             "            float ShadowMapSizeInverse = 1.0 / ShadowMapSize;\n"
             "            float2 lerps = frac( ShadowTexC.xy * ShadowMapSiz"
             "e.xx);\n"
             "\n"
             "            float4 SourceVals;\n"
             "            SourceVals.x = tex2D( ShadowMap, ShadowTexC.xy )."
             "r;\n"
             "\n"
             "            ShadowTexC.x += ShadowMapSizeInverse;\n"
             "            SourceVals.y = tex2D( ShadowMap, ShadowTexC.xy).r"
             ";  \n"
             "\n"
             "            ShadowTexC.y += ShadowMapSizeInverse;            "
             "    \n"
             "            SourceVals.w = tex2D( ShadowMap, ShadowTexC.xy).r"
             ";\n"
             "                \n"
             "            ShadowTexC.x -= ShadowMapSizeInverse;            "
             "    \n"
             "            SourceVals.z = tex2D( ShadowMap, ShadowTexC.xy).r"
             ";\n"
             "                \n"
             "            float4 Shade = (LightSpaceDepth <= SourceVals);\n"
             "\n"
             "            // lerp between the shadow values to calculate ou"
             "r light amount\n"
             "            ShadowOut = lerp(\n"
             "                lerp(Shade.x, Shade.y, lerps.x),\n"
             "                lerp(Shade.z, Shade.w, lerps.x), lerps.y );\n"
             "        }\n"
             "        else\n"
             "        {\n"
             "            // Outside of the light cone is shadowed complete"
             "ly\n"
             "            ShadowOut = 0.0;\n"
             "        }\n"
             "    }    \n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("DX9/D3D10/Xenon");
        pkBlock->SetTarget("ps_3_0/ps_4_0");

        pkBlock->SetText("\n"
             "    if (LightType != 2)\n"
             "    {\n"
             "        // This version only supports spotlights.\n"
             "        ShadowOut = 1.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float3 LightToWorldPos = normalize(float3(WorldPos - "
             "LightPos));\n"
             "\n"
             "        if (dot(LightDirection, LightToWorldPos) > CosOfCutof"
             "f)\n"
             "        {\n"
             "            float4 LightProjPos = mul(WorldPos, WorldToLightP"
             "rojMat);\n"
             "            float3 ShadowTexC = LightProjPos.xyz / LightProjP"
             "os.w;               \n"
             "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2( 0"
             ".5, 0.5 );\n"
             "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "\n"
             "            float LightSpaceDepth = clamp(ShadowTexC.z - Shad"
             "owBias, \n"
             "                0.0, 1.0);\n"
             "            \n"
             "            float ShadowMapSizeInverse = 1.0 / ShadowMapSize;\n"
             "\n"
             "            ShadowTexC.xy = ShadowTexC.xy - ShadowMapSizeInve"
             "rse.xx;\n"
             "               \n"
             "            ShadowOut = 0;\n"
             "            float4 fOnes = float4(1.0, 1.0, 1.0, 1.0);\n"
             "            float4 fKernels = float4(1.0, 1.0, 1.0, 1.0);\n"
             "            float faKernels[4] = {1.0, 1.0, 1.0, 1.0};\n"
             "           \n"
             "            faKernels[0] = 1-frac(ShadowTexC.y * ShadowMapSiz"
             "e.y);\n"
             "            faKernels[3]= frac(ShadowTexC.y  * ShadowMapSize."
             "y); \n"
             "           \n"
             "            fKernels.x = 1-frac(ShadowTexC.x * ShadowMapSize."
             "x);\n"
             "            fKernels.w = frac(ShadowTexC.x  * ShadowMapSize.x"
             ");\n"
             "                              \n"
             "            float fTotalPercent = 0;\n"
             "            \n"
             "            // This loop is manually unrolled here to avoid l"
             "ong\n"
             "            // shader compilation times.            \n"
             "            //for (int i=0; i < 4; i++)\n"
             "            // i == 0\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC.xy;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "                    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "0];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[0];\n"
             "            }\n"
             "            // i == 1\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC.xy;\n"
             "                pos.y += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "                    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "1];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[1];\n"
             "            }\n"
             "            // i == 2\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC.xy;\n"
             "                pos.y += 2 * ShadowMapSizeInverse;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "                    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "2];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[2];\n"
             "            }\n"
             "            // i == 3\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC.xy;\n"
             "                pos.y += 3 * ShadowMapSizeInverse;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "                    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "3];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[3];\n"
             "            }\n"
             "           \n"
             "            ShadowOut = ShadowOut / fTotalPercent;         \n"
             "        }\n"
             "        else\n"
             "        {\n"
             "            // Outside of the light cone is shadowed complete"
             "ly\n"
             "            ShadowOut = 0.0;\n"
             "        }\n"
             "    }    \n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("PS3");
        pkBlock->SetTarget("ps_3_0");

        pkBlock->SetText("\n"
             "    if (LightType != 2)\n"
             "    {\n"
             "        // This version only supports spotlights.\n"
             "        ShadowOut = 1.0;\n"
             "    }\n"
             "    else\n"
             "    {\n"
             "        float3 LightToWorldPos = normalize(float3(WorldPos - "
             "LightPos));\n"
             "\n"
             "        if (dot(LightDirection, LightToWorldPos) > CosOfCutof"
             "f)\n"
             "        {\n"
             "            float4 LightProjPos = mul(WorldPos, WorldToLightP"
             "rojMat);\n"
             "            float3 ShadowTexC = LightProjPos.xyz / LightProjP"
             "os.w;               \n"
             "            ShadowTexC.xy =( 0.5 * ShadowTexC.xy) + float2( 0"
             ".5, 0.5 );\n"
             "            ShadowTexC.y = 1.0f - ShadowTexC.y;\n"
             "\n"
             "            // Rescale Depth from (-1.0)-(1.0) to (0.0)-(1.0)\n"
             "            ShadowTexC.z = (ShadowTexC.z * 0.5) + 0.5;\n"
             "\n"
             "            float LightSpaceDepth = clamp(ShadowTexC.z - Shad"
             "owBias, \n"
             "                0.0, 1.0);\n"
             "            \n"
             "            float ShadowMapSizeInverse = 1.0 / ShadowMapSize;\n"
             "\n"
             "            ShadowTexC.xy = ShadowTexC.xy - ShadowMapSizeInve"
             "rse.xx;\n"
             "               \n"
             "            ShadowOut = 0;\n"
             "            float4 fOnes = float4(1.0, 1.0, 1.0, 1.0);\n"
             "            float4 fKernels = float4(1.0, 1.0, 1.0, 1.0);\n"
             "            float faKernels[4] = {1.0, 1.0, 1.0, 1.0};\n"
             "           \n"
             "            faKernels[0] = 1-frac(ShadowTexC.y * ShadowMapSiz"
             "e.y);\n"
             "            faKernels[3]= frac(ShadowTexC.y  * ShadowMapSize."
             "y); \n"
             "           \n"
             "            fKernels.x = 1-frac(ShadowTexC.x * ShadowMapSize."
             "x);\n"
             "            fKernels.w = frac(ShadowTexC.x  * ShadowMapSize.x"
             ");\n"
             "                              \n"
             "            float fTotalPercent = 0;\n"
             "            \n"
             "            // This loop is manually unrolled here to avoid l"
             "ong\n"
             "            // shader compilation times.            \n"
             "            //for (int i=0; i < 4; i++)\n"
             "            // i == 0\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC.xy;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "                    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "0];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[0];\n"
             "            }\n"
             "            // i == 1\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC.xy;\n"
             "                pos.y += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "                    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "1];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[1];\n"
             "            }\n"
             "            // i == 2\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC.xy;\n"
             "                pos.y += 2 * ShadowMapSizeInverse;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "                    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "2];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[2];\n"
             "            }\n"
             "            // i == 3\n"
             "            {\n"
             "                float4 shadowMapDepth = 0;\n"
             "                float2 pos = ShadowTexC.xy;\n"
             "                pos.y += 3 * ShadowMapSizeInverse;\n"
             "                shadowMapDepth.x = tex2D(ShadowMap, pos).x; \n"
             "                pos.x += ShadowMapSizeInverse;\n"
             "                shadowMapDepth.y = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;        \n"
             "                shadowMapDepth.z = tex2D(ShadowMap, pos).x;\n"
             "                pos.x += ShadowMapSizeInverse;               "
             " \n"
             "                shadowMapDepth.w = tex2D(ShadowMap, pos).x;\n"
             "                \n"
             "                float4 shad = (LightSpaceDepth <= shadowMapDe"
             "pth);\n"
             "                    \n"
             "                ShadowOut += dot(shad, fKernels) * faKernels["
             "3];\n"
             "                fTotalPercent += dot(fOnes, fKernels) * faKer"
             "nels[3];\n"
             "            }\n"
             "           \n"
             "            ShadowOut = ShadowOut / fTotalPercent;         \n"
             "        }\n"
             "        else\n"
             "        {\n"
             "            // Outside of the light cone is shadowed complete"
             "ly\n"
             "            ShadowOut = 0.0;\n"
             "        }\n"
             "    }    \n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NI_NOINLINE static void CreateFragment67(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("ConstructInstanceTransform");
    pkFrag->SetDescription("\n"
        "    This fragment is reponsible for creating the world-instance tr"
        "ansform.\n"
        "    This is a 4x4 transform that is constructed from 3 float4 obje"
        "cts. It \n"
        "    assumes the the last column in the matrix should be (0, 0, 0, "
        "1).\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Position1");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Position2");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Position3");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4x4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("World");
        pkRes->SetVariable("InstanceWorld");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "  \n"
             "    // Build instance world transform matrix.  \n"
             "    InstanceWorld._11 = Position1.x;\n"
             "    InstanceWorld._12 = Position1.y;\n"
             "    InstanceWorld._13 = Position1.z;\n"
             "    InstanceWorld._14 = 0;\n"
             "    \n"
             "    InstanceWorld._21 = Position2.x;\n"
             "    InstanceWorld._22 = Position2.y;\n"
             "    InstanceWorld._23 = Position2.z;\n"
             "    InstanceWorld._24 = 0;    \n"
             "          \n"
             "    InstanceWorld._31 = Position3.x;\n"
             "    InstanceWorld._32 = Position3.y;\n"
             "    InstanceWorld._33 = Position3.z;\n"
             "    InstanceWorld._34 = 0;       \n"
             "    \n"
             "    InstanceWorld._41 = Position1.w;\n"
             "    InstanceWorld._42 = Position2.w;\n"
             "    InstanceWorld._43 = Position3.w;\n"
             "    InstanceWorld._44 = 1;       \n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NiMaterialNodeLibrary* 
    NiStandardMaterialNodeLibrary::CreateMaterialNodeLibrary()
{
    // Create a new NiMaterialNodeLibrary
    NiMaterialNodeLibrary* pkLib = NiNew NiMaterialNodeLibrary(2);

    CreateFragment0(pkLib);
    CreateFragment1(pkLib);
    CreateFragment2(pkLib);
    CreateFragment3(pkLib);
    CreateFragment4(pkLib);
    CreateFragment5(pkLib);
    CreateFragment6(pkLib);
    CreateFragment7(pkLib);
    CreateFragment8(pkLib);
    CreateFragment9(pkLib);
    CreateFragment10(pkLib);
    CreateFragment11(pkLib);
    CreateFragment12(pkLib);
    CreateFragment13(pkLib);
    CreateFragment14(pkLib);
    CreateFragment15(pkLib);
    CreateFragment16(pkLib);
    CreateFragment17(pkLib);
    CreateFragment18(pkLib);
    CreateFragment19(pkLib);
    CreateFragment20(pkLib);
    CreateFragment21(pkLib);
    CreateFragment22(pkLib);
    CreateFragment23(pkLib);
    CreateFragment24(pkLib);
    CreateFragment25(pkLib);
    CreateFragment26(pkLib);
    CreateFragment27(pkLib);
    CreateFragment28(pkLib);
    CreateFragment29(pkLib);
    CreateFragment30(pkLib);
    CreateFragment31(pkLib);
    CreateFragment32(pkLib);
    CreateFragment33(pkLib);
    CreateFragment34(pkLib);
    CreateFragment35(pkLib);
    CreateFragment36(pkLib);
    CreateFragment37(pkLib);
    CreateFragment38(pkLib);
    CreateFragment39(pkLib);
    CreateFragment40(pkLib);
    CreateFragment41(pkLib);
    CreateFragment42(pkLib);
    CreateFragment43(pkLib);
    CreateFragment44(pkLib);
    CreateFragment45(pkLib);
    CreateFragment46(pkLib);
    CreateFragment47(pkLib);
    CreateFragment48(pkLib);
    CreateFragment49(pkLib);
    CreateFragment50(pkLib);
    CreateFragment51(pkLib);
    CreateFragment52(pkLib);
    CreateFragment53(pkLib);
    CreateFragment54(pkLib);
    CreateFragment55(pkLib);
    CreateFragment56(pkLib);
    CreateFragment57(pkLib);
    CreateFragment58(pkLib);
    CreateFragment59(pkLib);
    CreateFragment60(pkLib);
    CreateFragment61(pkLib);
    CreateFragment62(pkLib);
    CreateFragment63(pkLib);
    CreateFragment64(pkLib);
    CreateFragment65(pkLib);
    CreateFragment66(pkLib);
    CreateFragment67(pkLib);

    return pkLib;
}
//---------------------------------------------------------------------------

