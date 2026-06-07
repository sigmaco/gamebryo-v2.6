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
// Textures and Samplers
//---------------------------------------------------------------------------
texture2D BaseTexture;

sampler BaseSampler = sampler_state
{
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------
float4 SelectedColor;
float4x4 WorldViewProj;

//---------------------------------------------------------------------------
// Vertex Shaders
//---------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos      : SV_POSITION;
    float4 Color    : COLOR;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT VSMain(float4 inPos: POSITION, float2 inTex0: TEXCOORD0)
{
    VS_OUTPUT Out;
    Out.Pos = mul(inPos, WorldViewProj);
    Out.TexCoord = inTex0;
    Out.Color = SelectedColor;
    return Out;
}

//---------------------------------------------------------------------------
// Pixel Shaders
//---------------------------------------------------------------------------
float4 PSMain (VS_OUTPUT In) : SV_TARGET
{   
    return In.Color * BaseTexture.Sample(BaseSampler, In.TexCoord);
}
//---------------------------------------------------------------------------