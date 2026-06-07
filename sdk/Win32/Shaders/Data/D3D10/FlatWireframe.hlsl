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

// Simple Skinning Shader

float4x4 ViewProj;
float4x4 WorldViewProj;

static const int MAX_BONES = 30;
float4x3 Bone[MAX_BONES];
float4 WireframeColor;

struct VS_INPUT_SKINNED 
{
    float3 Pos          : POSITION;
    float3 BlendWeights : BLENDWEIGHT;
    int4 BlendIndices   : BLENDINDICES;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
};

struct VS_OUTPUT
{
    float4 Pos : POSITION;
};

VS_OUTPUT VS_SKINNED(VS_INPUT_SKINNED In)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    // Calculate normalized fourth bone weight
    float weight4 = 1.0f - In.BlendWeights[0] - In.BlendWeights[1] - 
        In.BlendWeights[2];
    float4 weights = float4(In.BlendWeights[0], In.BlendWeights[1], 
        In.BlendWeights[2], weight4);

    // Calculate bone transform
    float4x3 BoneTransform;
    BoneTransform = weights[0] * Bone[In.BlendIndices[0]];
    BoneTransform += weights[1] * Bone[In.BlendIndices[1]];
    BoneTransform += weights[2] * Bone[In.BlendIndices[2]];
    BoneTransform += weights[3] * Bone[In.BlendIndices[3]];
    
    float3 BoneSpacePos = mul(float4(In.Pos, 1.0), BoneTransform);

    Out.Pos = mul(float4(BoneSpacePos, 1.0), ViewProj);
    
    return Out;
}

VS_OUTPUT VS(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    Out.Pos = mul(float4(In.Pos, 1.0), WorldViewProj);
    
    return Out;
}

float4 PS(VS_OUTPUT In) : COLOR
{
    return WireframeColor;
}
