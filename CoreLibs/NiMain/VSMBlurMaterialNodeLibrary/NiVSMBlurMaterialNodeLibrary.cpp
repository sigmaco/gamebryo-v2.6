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
#include "NiVSMBlurMaterialNodeLibrary.h"

//---------------------------------------------------------------------------
static void CreateFragment0(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment1(NiMaterialNodeLibrary* pkLib)
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

        pkRes->SetType("float4");
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
             ".0f,\n"
             "    ShortSkinBoneTransform[1], 0.0f,\n"
             "    ShortSkinBoneTransform[2], 0.0f,\n"
             "    ShortSkinBoneTransform[3], 1.0f);\n"
             "\n"
             "    // Transform into world space.\n"
             "    WorldPos.xyz = mul(float4(Position, 1.0), ShortSkinBoneTr"
             "ansform);\n"
             "    WorldPos.w = 1.0f;\n"
             "  ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("Cg");
        pkBlock->SetPlatform("PS3");
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
             "    SkinBoneTransform = transpose(float4x4(ShortSkinBoneTrans"
             "form[0],\n"
             "    ShortSkinBoneTransform[1],\n"
             "    ShortSkinBoneTransform[2],\n"
             "    float4(0.0f, 0.0f, 0.0f, 1.0f)));\n"
             "\n"
             "    // Transform into world space.\n"
             "    WorldPos.xyz = mul(ShortSkinBoneTransform, float4(Positio"
             "n, 1.0));\n"
             "    WorldPos.w = 1.0f;\n"
             "  ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
static void CreateFragment2(NiMaterialNodeLibrary* pkLib)
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
             "    ProjPos = mul(WorldPosition, ViewProjection );\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
static void CreateFragment3(NiMaterialNodeLibrary* pkLib)
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
             "    ViewPos = mul(WorldPosition, ViewTransform );\n"
             "    ProjPos = mul(ViewPos, ProjTransform );\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
static void CreateFragment4(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment5(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("AddFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for adding two float4's.   \n"
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
static void CreateFragment6(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("AddFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for adding two float3's.   \n"
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
static void CreateFragment7(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("AddFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for adding two float2's.   \n"
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
static void CreateFragment8(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("AddFloat");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for adding two floats.   \n"
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
static void CreateFragment9(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("MultiplyFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for multiplying two float4's.   \n"
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
static void CreateFragment10(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("MultiplyFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for multiplying two float3's.   \n"
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
static void CreateFragment11(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("MultiplyFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for multiplying two float2's.   \n"
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
static void CreateFragment12(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("MultiplyFloat");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for multiplying two floats.   \n"
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
static void CreateFragment13(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("ScaleFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for scaling a float4 by a constan"
        "t.     \n"
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
static void CreateFragment14(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("ScaleFloat3");
    pkFrag->SetDescription("    \n"
        "    This fragment is responsible for scaling a float3 by a constan"
        "t.   \n"
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
static void CreateFragment15(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("ScaleFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for scaling a float2 by a constan"
        "t.   \n"
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
static void CreateFragment16(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("SaturateFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for saturating a float4.   \n"
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
static void CreateFragment17(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("SaturateFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for saturating a float3.   \n"
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
static void CreateFragment18(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("SaturateFloat2");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for saturating a float2.   \n"
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
static void CreateFragment19(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("SaturateFloat");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for saturating a float.   \n"
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
static void CreateFragment20(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("LerpFloat4");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for linearly interpolating two fl"
        "oat4's.   \n"
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
static void CreateFragment21(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel/Vertex");
    pkFrag->SetName("LerpFloat3");
    pkFrag->SetDescription("\n"
        "    This fragment is responsible for linearly interpolating two fl"
        "oat3's.   \n"
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
static void CreateFragment22(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment23(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment24(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment25(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment26(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment27(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment28(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment29(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment30(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment31(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment32(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("BlurColor");
    pkFrag->SetDescription("\n"
        "    This fragment writes projected depth to all color component ou"
        "tputs.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Proj");
        pkRes->SetVariable("WorldPosProjected");

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
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    float Depth = WorldPosProjected.z / WorldPosProjected.w;\n"
             "    OutputColor = Depth;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
static void CreateFragment33(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("WriteDepthToColor");
    pkFrag->SetDescription("\n"
        "    This fragment writes projected depth to all color component ou"
        "tputs.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Proj");
        pkRes->SetVariable("WorldPosProjected");

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
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    float Depth = WorldPosProjected.z / WorldPosProjected.w;\n"
             "    OutputColor = Depth;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
static void CreateFragment34(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("WriteVSMDepthToColor");
    pkFrag->SetDescription("\n"
        "    This fragment writes projected depth to all color component ou"
        "tputs.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetSemantic("Position");
        pkRes->SetLabel("Proj");
        pkRes->SetVariable("WorldPosProjected");

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
        pkBlock->SetTarget("ps_2_0/ps_4_0");

        pkBlock->SetText("\n"
             "    float Depth = WorldPosProjected.z / WorldPosProjected.w;\n"
             "            \n"
             "    OutputColor.x = Depth;\n"
             "    OutputColor.y = Depth * Depth;\n"
             "    OutputColor.z = 1.0;\n"
             "    OutputColor.w = 1.0;\n"
             "\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
static void CreateFragment35(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Vertex/Pixel");
    pkFrag->SetName("TeeFloat4");
    pkFrag->SetDescription("\n"
        "      This fragment splits and passes through a single float4 inpu"
        "t into two\n"
        "      float4 outputs.\n"
        "    ");
    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Input");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Output1");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert an output resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float4");
        pkRes->SetVariable("Output2");

        pkFrag->AddOutputResource(pkRes);
    }

    // Insert a code block
    {
        NiCodeBlock* pkBlock = NiNew NiCodeBlock();

        pkBlock->SetLanguage("hlsl/Cg");
        pkBlock->SetPlatform("D3D10/DX9/Xenon/PS3");
        pkBlock->SetTarget("vs_1_1/ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "      Output1 = Input;\n"
             "      Output2 = Input;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
static void CreateFragment36(NiMaterialNodeLibrary* pkLib)
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
        pkRes->SetDefaultValue("(false)");

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
static void CreateFragment37(NiMaterialNodeLibrary* pkLib)
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
        pkRes->SetDefaultValue("(false)");

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
             "    float4 ProjTexCoord = TexCoord.xyzz;\n"
             "    ColorOut.a = 1.0f;    \n"
             "    ColorOut.rgb = tex2Dproj(Sampler, ProjTexCoord).rgb;    \n"
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
static void CreateFragment38(NiMaterialNodeLibrary* pkLib)
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
        pkRes->SetDefaultValue("(false)");

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
             "    ColorOut.a = 1.0f;\n"
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
static void CreateFragment39(NiMaterialNodeLibrary* pkLib)
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
        pkRes->SetDefaultValue("(false)");

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
             "    ColorOut.a = 1.0f;\n"
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
static void CreateFragment40(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment41(NiMaterialNodeLibrary* pkLib)
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
static void CreateFragment42(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("HorzBlurSample");
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
        pkRes->SetDefaultValue("(false)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexSizeBase");
        pkRes->SetVariable("MapSize");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("KernelSize");

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
        pkBlock->SetTarget("ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "\n"
             "    // Blurs the texel at vTexCoord in the X direction.   \n"
             "    float fInvTexSize = 1.0 / MapSize.x;\n"
             "    float fHalfBlurKernelSize = KernelSize * 0.5f;\n"
             "    float fInvTotalWeights = 1.0 / (KernelSize * KernelSize);"
             "    \n"
             "    float fBlurWeight = 2;\n"
             "   \n"
             "    float4 fTotalColor = 0;   \n"
             "    float2 vTexCoord = TexCoord;\n"
             "    vTexCoord.x -= fInvTexSize * fHalfBlurKernelSize;    \n"
             "    float i = 0;\n"
             "\n"
             "    // Blur x2\n"
             "    if ( 2 <= KernelSize)\n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.x += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.x += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "\n"
             "    // Blur x4\n"
             "    if ( 4 <= KernelSize)\n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.x += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.x += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "    \n"
             "    // Blur x6\n"
             "    if ( 6 <= KernelSize)\n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.x += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.x += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }    \n"
             "    \n"
             "    // Blur x8\n"
             "    if ( 8 <= KernelSize)\n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.x += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.x += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }    \n"
             "    \n"
             "    // Blur x10\n"
             "    if ( 10 <= KernelSize)\n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.x += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.x += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "\n"
             "    // Blur x12\n"
             "    if ( 12 <= KernelSize)\n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.x += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.x += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "       \n"
             "    // Blur x14\n"
             "    if ( 14 <= KernelSize)\n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.x += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.x += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }    \n"
             "    \n"
             "    // Blur x16\n"
             "    if ( 16 <= KernelSize)\n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.x += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.x += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "        \n"
             "    ColorOut = fTotalColor;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
static void CreateFragment43(NiMaterialNodeLibrary* pkLib)
{
    NiMaterialFragmentNode* pkFrag = NiNew NiMaterialFragmentNode();

    pkFrag->SetType("Pixel");
    pkFrag->SetName("VertBlurSample");
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
        pkRes->SetDefaultValue("(false)");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float2");
        pkRes->SetSemantic("TexSizeBase");
        pkRes->SetVariable("MapSize");

        pkFrag->AddInputResource(pkRes);
    }

    // Insert an input resource
    {
        NiMaterialResource* pkRes = NiNew NiMaterialResource();

        pkRes->SetType("float");
        pkRes->SetVariable("KernelSize");

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
        pkBlock->SetTarget("ps_2_0/vs_4_0/ps_4_0");

        pkBlock->SetText("\n"
             "\n"
             "    // Blurs the texel at vTexCoord in the Y direction.   \n"
             "    float fInvTexSize = 1.0 / MapSize.y;\n"
             "    float fHalfBlurKernelSize = KernelSize * 0.5f;\n"
             "    float fInvTotalWeights = 1.0 / (KernelSize * KernelSize);"
             "    \n"
             "    float fBlurWeight = 2;\n"
             "   \n"
             "    float4 fTotalColor = 0;   \n"
             "    float2 vTexCoord = TexCoord;\n"
             "    vTexCoord.y -= fInvTexSize * fHalfBlurKernelSize;    \n"
             "    float i = 0;\n"
             "\n"
             "    // Blur x2\n"
             "    if ( 2 <= KernelSize)    \n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.y += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.y += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "    \n"
             "    // Blur x4\n"
             "    if ( 4 <= KernelSize)    \n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.y += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.y += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }    \n"
             "\n"
             "    // Blur x6\n"
             "    if ( 6 <= KernelSize)    \n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.y += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.y += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "\n"
             "    // Blur x8\n"
             "    if ( 8 <= KernelSize)    \n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.y += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.y += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "\n"
             "    // Blur x10\n"
             "    if ( 10 <= KernelSize)    \n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.y += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.y += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "\n"
             "    // Blur x12\n"
             "    if ( 12 <= KernelSize)    \n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.y += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.y += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "    \n"
             "    // Blur x14\n"
             "    if ( 14 <= KernelSize)    \n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.y += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.y += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }    \n"
             "\n"
             "    // Blur x16\n"
             "    if ( 16 <= KernelSize)    \n"
             "    {\n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;        \n"
             "        vTexCoord.y += fInvTexSize;\n"
             "        \n"
             "        float fSign = sign( i + 1 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;\n"
             "        \n"
             "        fTotalColor += tex2D(Sampler, vTexCoord) * \n"
             "            fBlurWeight * fInvTotalWeights;\n"
             "        vTexCoord.y += fInvTexSize;            \n"
             "        \n"
             "        fSign = sign( i + 2 - fHalfBlurKernelSize);\n"
             "        fBlurWeight -= fSign * 4;        \n"
             "        i+=2;\n"
             "    }\n"
             "\n"
             "    ColorOut = fTotalColor;\n"
             "    ");


        pkFrag->AddCodeBlock(pkBlock);
    }

    pkLib->AddNode(pkFrag);
}
//---------------------------------------------------------------------------
NiMaterialNodeLibrary* 
    NiVSMBlurMaterialNodeLibrary::CreateMaterialNodeLibrary()
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

    return pkLib;
}
//---------------------------------------------------------------------------

