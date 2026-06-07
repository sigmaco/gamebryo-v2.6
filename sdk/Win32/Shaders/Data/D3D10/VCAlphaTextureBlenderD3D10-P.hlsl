// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------
sampler2D Texture1 : register(s0);
sampler2D Texture2 : register(s1);
sampler2D Detail : register(s2);

//---------------------------------------------------------------------------
// Functions:
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Input:
//---------------------------------------------------------------------------
struct Input
{
    float4 Position : POSITION0;
    float4 DiffuseAccum : TEXCOORD0;
    float2 UVSet0 : TEXCOORD1;
    float2 UVSet1 : TEXCOORD2;
    float2 UVSet2 : TEXCOORD3;
};

//---------------------------------------------------------------------------
// Output:
//---------------------------------------------------------------------------
struct Output
{
    float4 Color : COLOR0;
};

//---------------------------------------------------------------------------
// Main():
//---------------------------------------------------------------------------
Output fmain(Input In)
{
    Output Out;

    float3 Blend0Color = tex2D(Texture1, In.UVSet0).rgb;
    float3 Blend1Color = tex2D(Texture2, In.UVSet1).rgb;;
    float3 BlendedColor = lerp(Blend1Color, Blend0Color, In.DiffuseAccum.a);
    float3 DetailColor = tex2D(Detail, In.UVSet2).rgb * float3(2.0f, 2.0f, 2.0f);
    float3 LitBlendedColor = In.DiffuseAccum.rgb * BlendedColor.rgb;
    LitBlendedColor = LitBlendedColor.rgb * DetailColor.rgb;
    Out.Color = float4(LitBlendedColor.r, LitBlendedColor.g,
        LitBlendedColor.b, 1.0f); 
    return Out;
}
