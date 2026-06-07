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

float4x4 ViewProjMatrix;

static const int MAX_BONES = 20;
float4x3 Bone[MAX_BONES];

struct VS_INPUT 
{
    float4 Pos			: POSITION;
    float4 BlendWeights : BLENDWEIGHT;
    float4 BlendIndices : BLENDINDICES;
    float2 TexCoords    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float2 TexCoords    : TEXCOORD0;
};

sampler BaseSampler : register(s0);

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	// Compensate for lack of UBYTE4 on Geforce3
    int4 indices = D3DCOLORtoUBYTE4(In.BlendIndices);

    // Calculate normalized fourth bone weight
    float weight4 = 1.0f - In.BlendWeights[0] - In.BlendWeights[1] - 
        In.BlendWeights[2];
    float4 weights = float4(In.BlendWeights[0], In.BlendWeights[1], 
        In.BlendWeights[2], weight4);

    // Calculate bone transform
    float4x3 BoneTransform;
	BoneTransform = weights[0] * Bone[indices[0]];
	BoneTransform += weights[1] * Bone[indices[1]];
	BoneTransform += weights[2] * Bone[indices[2]];
	BoneTransform += weights[3] * Bone[indices[3]];
	
	float3 BoneSpacePos = mul(In.Pos, BoneTransform);

	Out.Pos = mul(float4(BoneSpacePos, 1.0), ViewProjMatrix);
	
	Out.TexCoords = In.TexCoords;

	return Out;
}

float4 PS(VS_OUTPUT In) : COLOR
{
   return tex2D(BaseSampler, In.TexCoords);
}

technique SimpleSkinning
<
	string Description = "Minimal skinning shader";
	int BonesPerPartition = MAX_BONES;
	bool UsesNiRenderState = true;
>
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS();
		PixelShader = compile ps_1_1 PS();
	}
}

