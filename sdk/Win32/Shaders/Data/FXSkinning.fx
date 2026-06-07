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

// Simple FX Skinning Shader

float4x4 ViewProj      : VIEWPROJ;

static const int MAX_BONES = 20;
float4x3 Bone[MAX_BONES] : SKINBONEMATRIX3;

struct VS_INPUT 
{
    float4 Pos          : POSITION;
    float3 BlendWeights : BLENDWEIGHT;
    float4 BlendIndices : BLENDINDICES;
    float2 TexCoords    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos          : POSITION;
    float2 TexCoords    : TEXCOORD0;
};

texture BaseMap
<
    string NTM = "base";
>;

sampler BaseSampler = sampler_state
{
    Texture = (BaseMap);
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};

VS_OUTPUT VS(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    // Compensate for lack of UBYTE4 on Geforce3
    int4 indices = D3DCOLORtoUBYTE4(In.BlendIndices);
    // same as In.BlendIndices.bgra * 255.5;

    // Calculate normalized fourth bone weight
    float weight4 = 1.0f 
        - In.BlendWeights[0] 
        - In.BlendWeights[1] 
        - In.BlendWeights[2];
        
    float4 weights = float4(
        In.BlendWeights[0], 
        In.BlendWeights[1], 
        In.BlendWeights[2], 
        weight4);

    // Calculate bone transform
    float4x3 BoneTransform;
    BoneTransform = weights[0] * Bone[indices[0]];
    BoneTransform += weights[1] * Bone[indices[1]];
    BoneTransform += weights[2] * Bone[indices[2]];
    BoneTransform += weights[3] * Bone[indices[3]];
    
    float3 BoneSpacePos = mul(In.Pos, BoneTransform);

    Out.Pos = mul(float4(BoneSpacePos, 1.0), ViewProj);
    
    Out.TexCoords = In.TexCoords;

    return Out;
}

float4 PS(VS_OUTPUT In) : COLOR
{
   return tex2D(BaseSampler, In.TexCoords);
}

technique FXSkinning
<
    string Description = "This shader performs skinning in the vertex shader "
    "using 20 bones. It uses HLSL shaders through an FX file. "
    "This shader does not transform normals and "
    "cannot perform lighting calculations. "
    "This shader does apply a base map. ";

    int BonesPerPartition = MAX_BONES;
    bool UsesNiRenderState = true;
    bool BlendIndicesAsD3DColor = true;
>
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader = compile ps_2_0 PS();
    }
}
