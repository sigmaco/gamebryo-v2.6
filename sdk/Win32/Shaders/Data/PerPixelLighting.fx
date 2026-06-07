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

// Per Pixel Lighting shader

#include "SimpleTrans.inc"

// textures
texture NormalMap 
< 
    string NTM = "shader";
>;

texture BaseMap 
< 
    string NTM = "base";
>;

float3 MSLightPos : Position
<
    string Object = "PointLight";
    string Space = "Model";
>;

float AmbientIntensity          : GLOBAL = 0.2f;

// Vertex Shaders
struct VS_OUTPUT
{
    float4 Position   : POSITION;
    float2 NormalTex  : TEXCOORD0;
    float2 BaseTex    : TEXCOORD1;
    float3 TSLightDir : TEXCOORD2;
};

VS_OUTPUT VS(float3 Position : POSITION,
    float3 Normal   : NORMAL,
    float3 Binormal : BINORMAL,
    float3 Tangent  : TANGENT,
    float2 TexCoord : TEXCOORD)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    // Transform position
    Out.Position = WorldTrans(Position);

    // Fill in texture coordinates
    Out.NormalTex = TexCoord;
    Out.BaseTex = TexCoord;
    
    // Get model-space light direction
    float3 MSLightDir = MSLightPos - Position;

    // Transform into tangent-space light direction
    float fX = dot(Tangent, MSLightDir);
    float fY = dot(Binormal, MSLightDir);
    float fZ = dot(Normal, MSLightDir);
    Out.TSLightDir = normalize(float3(fX, fY, fZ)) * 0.5f + 0.5f;
     
    return Out;
}

struct VS_OUTPUT_NO_PS
{
    float4 Position   : POSITION;
    float3 TSLightDir : COLOR;
    float2 NormalTex  : TEXCOORD0;
    float2 BaseTex    : TEXCOORD1;
};

VS_OUTPUT_NO_PS VS_NO_PS(float3 Position : POSITION,
    float3 Normal   : NORMAL,
    float3 Binormal : BINORMAL,
    float3 Tangent  : TANGENT,
    float2 TexCoord : TEXCOORD)
{
    VS_OUTPUT_NO_PS Out = (VS_OUTPUT_NO_PS)0;

    // Perform calculations as if using PS
    VS_OUTPUT Temp = VS(Position, Normal, Binormal, Tangent, TexCoord);
    Out.Position = Temp.Position;
    Out.NormalTex = Temp.NormalTex;
    Out.BaseTex = Temp.BaseTex;
        
    // Normalize and bias tangent-space light direction into color
    Out.TSLightDir = Temp.TSLightDir * 0.5f + 0.5f; 
    
    return Out;
}

// Samplers
sampler BaseSampler = sampler_state
{ 
    Texture = (BaseMap);
    AddressU = Clamp;
    AddressV = Clamp;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler NormalSampler = sampler_state
{ 
    Texture = (NormalMap);
    AddressU = Clamp;
    AddressV = Clamp;
    MipFilter = LINEAR; 
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

// Pixel shader
float4 PS(VS_OUTPUT In) : COLOR
{
    float3 BiasedNormal = tex2D(NormalSampler, In.NormalTex);
    float3 Normal = BiasedNormal * 2.0f - 1.0f;
    float3 LightDir = In.TSLightDir * 2.0f - 1.0f;
    
    float LightIntensity = saturate(dot(Normal, LightDir)) 
        + AmbientIntensity;
    
    float4 Base = tex2D(BaseSampler, In.BaseTex);

    return Base * LightIntensity;
}

// techniques
technique PerPixelLightingPS
<
	string Description = "This shader applies a base map and performs "
	"per-pixel lighting using a normal map and a point light. This version "
	"of the shader does the lighting computations in a pixel shader.";

    string NBTMethod = "ATI";
    int NBTSourceUV = 0;
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
#if defined(_XENON)
        PixelShader = compile ps_1_1 PS();
#else
        PixelShader = asm
        {
            // Compiled version of PS
            // As of December 2006 DX SDK, compilation of HLSL code to
            // ps_1_x is not longer supported.

            ps_1_1
            def c1, 1, 0, 0, 0
            tex t0
            tex t1
            texcoord t2
            dp3_sat r0, t0_bx2, t2_bx2
            dp3 r1, c1, c0
            add r0.w, r0.w, r1.w
            mul r0, t1, r0.w
        };
        Sampler[0] = NormalSampler;
        Sampler[1] = BaseSampler;
        PixelShaderConstant[0] = AmbientIntensity;
#endif

        AlphaBlendEnable    = FALSE;
    }
}

technique PerPixelLightingNoPS
<
	string Description = "This shader applies a base map and performs "
	"per-pixel lighting using a normal map and a point light. This version "
	"of the shader does the lighting computations in the fixed-function "
	"texture pipeline, though it does require a vertex shader.";
	//
	// Internal note:
	//  Since Xenon does not support fixed function pipeline, we will 
	//  conditionally use the PS here. 

    string NBTMethod = "ATI";
    int NBTSourceUV = 0;
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS_NO_PS();
#if defined(_XENON)
        PixelShader = compile ps_1_1 PS();
#else
        PixelShader = NULL;

        Texture[0] = (NormalMap);
        ColorOp[0] = DotProduct3;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
        AddressU[0] = Clamp;
        AddressV[0] = Clamp;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;

        Texture[1] = (BaseMap);
        ColorOp[1] = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1] = SelectArg1;
        AlphaArg1[1] = Texture;
        AlphaArg2[1] = Current;
        AddressU[1] = Clamp;
        AddressV[1] = Clamp;
        MipFilter[1] = LINEAR;
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
#endif

        AlphaBlendEnable = FALSE;
    }
}
