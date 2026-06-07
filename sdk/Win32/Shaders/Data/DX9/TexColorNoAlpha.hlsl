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

// Shader that writes out the base map texture color with an alpha value of 0.

sampler BaseSampler : register(s0);

float4x4 WorldViewProj;

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

VS_OUTPUT VS(
    float3 Position : POSITION,
    float2 TexCoord : TEXCOORD)
{
    VS_OUTPUT Out = (VS_OUTPUT) 0;
    Out.Position = mul(float4(Position, 1), WorldViewProj);
    Out.TexCoord = TexCoord;
    return Out;
}

float4 PS(VS_OUTPUT In) : COLOR
{
    float4 TexColor = tex2D(BaseSampler, In.TexCoord);
    return float4(TexColor.rgb, 0);
}
