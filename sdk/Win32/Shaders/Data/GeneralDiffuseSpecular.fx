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
// GeneralDiffuseSpecular.fx
// The techniques in this file are applied to objects in the 3D scene 
// rendering directly in the art assets.  Generally speaking, all of them
// implement some variation of a lighting model that is based on two HDR
// image cube maps, and two point light sources.  The HDR cube maps (one for
// specular and one for diffuse) are artist generated and applied to almost
// everything in the demo scene.  The point lights are: one fire light 
// source and the point light of the blaster gun.  The application code 
// is responsible for assigning the closest fire light.
//---------------------------------------------------------------------------

// Textures
texture BaseMap
<
    string NTM = "base";
>;

texture DarkMap
<
    string NTM = "dark";
>;

texture GlowMap
<
    string NTM = "glow";
>;

texture NormalMap
< 
    string NTM = "shader";
    string description = "The biased normals in tangent space.";
    int NTMIndex = 0;
>;

texture LightingMap_cube 
<
    string NTM = "shader";
    string description = "The diffuse lighting cube map.";    
    int NTMIndex = 1;
>;

texture ReflectionMap_cube
< 
    string NTM = "shader";
    string description = "The specular reflection cube map.";
    int NTMIndex = 2;
>;

// Samplers
sampler BaseSampler = sampler_state
{
    Texture = (BaseMap);
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
    MAGFILTER = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

sampler DarkSampler = sampler_state
{
    Texture = (DarkMap);
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
    MAGFILTER = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

sampler GlowSampler = sampler_state
{
    Texture = (GlowMap);
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
    MAGFILTER = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

sampler NormalSampler = sampler_state
{
    Texture = (NormalMap);
    MagFilter = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

sampler LightCube = sampler_state
{
    Texture = (LightingMap_cube);
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

sampler SpecularCube = sampler_state
{
    Texture = (ReflectionMap_cube);
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

sampler AlphaSampler = sampler_state
{
    Texture = (NormalMap);
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
    MAGFILTER = LINEAR;
    ADDRESSU = CLAMP;
    ADDRESSV = CLAMP;
};

// Constants
float bumpiness : ATTRIBUTE 
<
    float min = 0.0;
    float max = 2.0;
> = 1.0;

float GlobalDimmer : ATTRIBUTE = 1.0f;
float GlowMultiplier : ATTRIBUTE = 1.0f;

float4x4 View : VIEW;
float4x4 ViewProj : VIEWPROJ;
float4x4 Proj : PROJ;
float4x4 WorldViewProj : WORLDVIEWPROJECTION;
float4x4 inv_view_matrix : INVVIEW;
float4x4 world_matrix : WORLD;

static const int MAX_BONES = 29;
float4x3 SkinBone[MAX_BONES] : SKINBONEMATRIX3;

// Light hookups
float3 LightPos0 : POSITION
<
    string Object = "PointLight";
    int ObjectIndex = 0;
> = {0.0f, 0.0f, 0.0f};

float4 LightDiff0 : DIFFUSE
<
    string Object = "PointLight";
    int ObjectIndex = 0;
> = {0.0f, 0.0f, 0.0f, 0.0f};

float3 LightAtten0 : ATTENUATION
<
    string Object = "PointLight";
    int ObjectIndex = 0;
> = {1.0f, 0.0f, 0.0f};

float3 LightPos1 : POSITION
<
    string Object = "PointLight";
    int ObjectIndex = 1;
> = {0.0f, 0.0f, 0.0f};

float4 LightDiff1 : DIFFUSE
<
    string Object = "PointLight";
    int ObjectIndex = 1;
> = {0.0f, 0.0f, 0.0f, 0.0f};

float3 LightAtten1 : ATTENUATION
<
    string Object = "PointLight";
    int ObjectIndex = 1;
> = {1.0f, 0.0f, 0.0f};

bool g_bModifyAlpha : GLOBAL = false;
float g_fCharacterDist : GLOBAL = 80;
float g_fDiffLightMultiplier : GLOBAL = 1.0f;
float g_fSpecLightMultiplier : GLOBAL = 0.5f;
float g_fPointLightMultiplier : GLOBAL = 1.0f;

//--------------------------------------------------------------//
// Transform Utility functions
//--------------------------------------------------------------//
float4 CalculatePosition(float4 inPos, float3 inNrm, out float4 outPos, 
    out float3 outNrm)
{
    float4 ProjectedPos = mul(inPos, WorldViewProj);
    
    outPos = mul(inPos, world_matrix);

    float3 viewNrm = mul(inNrm, (float3x3)world_matrix);
    
    // turn into DX space
    outNrm = normalize(viewNrm.xzy);

    return ProjectedPos;
}

float4 CalculatePositionNBT(float4 inPos, float3 inNrm, float3 inBiNrm, 
    float3 inTan, out float4 outPos, out float3 outNrm, out float3 outBiNrm, 
    out float3 outTan)
{
    float4 ProjectedPos = mul(inPos, WorldViewProj);
    
    outPos = mul(inPos, world_matrix);

    // Munge up the basis vectors to get a good matrix here for the texture
    // operations. This matrix should convert from tangent space to world
    // space.
    float3 viewNrm = mul(inNrm, (float3x3)world_matrix);
    float3 viewBiNrm = mul(inBiNrm, (float3x3)world_matrix);
    float3 viewTan = mul(inTan, (float3x3)world_matrix);
    
    // turn into DX space
    outNrm = normalize(viewNrm.xzy);
    outBiNrm = normalize(viewBiNrm.xzy);
    outTan = normalize(viewTan.xzy);

    return ProjectedPos;
}

float4 CalculateSkinnedPositionNBT(float4 inPos, float3 inBlendWeights,
    int4 inBlendIndices, float3 inNrm, float3 inBiNrm, float3 inTan,
    out float4 outPos, out float3 outNrm, out float3 outBiNrm, 
    out float3 outTan)
{
    // Calculate normalized fourth bone weight
    float weight4 = 1.0f - inBlendWeights[0] - inBlendWeights[1] - 
        inBlendWeights[2];

    // Calculate bone transform
    float4x3 SkinBoneTransform;
    SkinBoneTransform = inBlendWeights[0] * SkinBone[inBlendIndices[0]];
    SkinBoneTransform += inBlendWeights[1] * SkinBone[inBlendIndices[1]];
    SkinBoneTransform += inBlendWeights[2] * SkinBone[inBlendIndices[2]];
    SkinBoneTransform += weight4 * SkinBone[inBlendIndices[3]];
    
    outPos = float4(mul(inPos, SkinBoneTransform), 1.0);
    float4 ProjectedPos = mul(outPos, ViewProj);
    
    // Munge up the basis vectors to get a good matrix here for the texture
    // operations. This matrix should convert from tangent space to world
    // space.
    float3 viewNrm = mul(inNrm, (float3x3)SkinBoneTransform);
    float3 viewBiNrm = mul(inBiNrm, (float3x3)SkinBoneTransform);
    float3 viewTan = mul(inTan, (float3x3)SkinBoneTransform);
    
    // turn into DX space
    outNrm = normalize(viewNrm.xzy);
    outBiNrm = normalize(viewBiNrm.xzy);
    outTan = normalize(viewTan.xzy);

    return ProjectedPos;
}

float4 CalculateDiffuseLightContribution(float3 inWorldPos, float3 inWorldNrm,
    float3 inLightPos, float4 inLightDiff, float3 inLightAtten)
{
    float3 LightVec = inLightPos - inWorldPos;
    float LightDist = length(LightVec);
    float3 LightDir = normalize(LightVec);
    
    float Atten = 1.0 / (inLightAtten.x + inLightAtten.y * LightDist + 
        inLightAtten.z * LightDist * LightDist);
    
    float4 Color = max(0, dot(LightDir, inWorldNrm)) * Atten * inLightDiff * 
        g_fPointLightMultiplier;
    
    return Color;
}

//--------------------------------------------------------------//
// Standard: base map, dark map, 2 point lights, diffuse cube map, 
//   specular cube map, and normal map (with gloss in alpha channel)
//--------------------------------------------------------------//

struct VS_OUTPUT_Standard 
{
    float4 Pos  :     POSITION;
    float2 Tex0 :     TEXCOORD0;
    float2 Tex1 :     TEXCOORD1;
    float3 Row0 :     TEXCOORD2;
    float3 Row1 :     TEXCOORD3;
    float4 Row2 :     TEXCOORD4;
    float3 View :     TEXCOORD5;  
    float4 Diff :     TEXCOORD6;
    float4 AlphaPos : TEXCOORD7;
};

VS_OUTPUT_Standard VSMainStandard(float3 inPos: POSITION,
    float4 inDiff: COLOR0, 
    float2 inTex0: TEXCOORD0,
    float2 inTex1: TEXCOORD1,
    float3 inNrm: NORMAL,
    float3 inBiNrm: BINORMAL,
    float3 inTan: TANGENT)
{
    VS_OUTPUT_Standard Out;

    float4 worldPos;
    float3 worldNrm;
    Out.Pos = CalculatePositionNBT(float4(inPos, 1.0f), // in position
        inNrm, inBiNrm * bumpiness, inTan * bumpiness,  // in NBT
        worldPos,                                       // out position
        worldNrm, Out.Row1, Out.Row0);                  // out NBT
    Out.Row2.xyz = worldNrm;

    // Pack MRT z position into the fourth component of Row2.
    Out.Row2.w = mul(worldPos, View).z;

    Out.Tex0 = inTex0;
    Out.Tex1 = inTex1;
    
    // Calculate dynamic lighting
    float4 Diffuse = CalculateDiffuseLightContribution(worldPos.xyz,
        worldNrm.xzy, LightPos0, LightDiff0, LightAtten0);

    Diffuse += CalculateDiffuseLightContribution(worldPos.xyz,
        worldNrm.xzy, LightPos1, LightDiff1, LightAtten1);
    
    Out.Diff = inDiff * Diffuse;

    // Pass the world space view vector into the pixel shader to calculate
    // the reflection.
    float4 worldSpaceViewLoc = inv_view_matrix[3];
    float3 worldSpaceViewVector = worldSpaceViewLoc - worldPos;
    float3 worldSpaceViewNormalized = normalize(worldSpaceViewVector);

    // turn into DX space
    Out.View = worldSpaceViewNormalized.xzy;
    
    // Propagate alpha position.
    Out.AlphaPos = Out.Pos;

    return Out;
}

float4 PSMainStandard(float2 inTex0: TEXCOORD0, float2 inTex1: TEXCOORD1, 
    float3 inRow0: TEXCOORD2, float3 inRow1: TEXCOORD3, 
    float4 inRow2: TEXCOORD4, float3 inView: TEXCOORD5, 
    float4 inDiff: TEXCOORD6, float4 inAlphaPos : TEXCOORD7,
    out float4 outMRTValues : COLOR1) : COLOR0
{
    // Output MRT values. Position is stored as the fourth component.
    outMRTValues = float4(normalize(mul(inRow2.xyz, (float3x3) View)),
        inRow2.w);

    // Sample the base map.
    float4 color = tex2D(BaseSampler, inTex0);
    
    // Sample the dark map.
    color *= tex2D(DarkSampler, inTex1);
    
    // Get the normal in texture space.
    float4 normalTexSpace;
    normalTexSpace = tex2D(NormalSampler, inTex0);

    // Save off the gloss map value from the normal map's alpha slot.
    float glossValue = normalTexSpace.a;
    // Bias the normal in texture space so it is signed.
    normalTexSpace = 2.0 * normalTexSpace - 1.0;

    // Convert the normal to world space and look up the diffuse lighting
    // value. Modulate that with the base map.
    float3 normalWorldSpace = 
        mul(normalTexSpace, float3x3(inRow0, inRow1, inRow2.xyz));
    float4 diffuseColor = inDiff + 
        GlobalDimmer * texCUBE(LightCube, normalWorldSpace) * 
        g_fDiffLightMultiplier;
    color *= diffuseColor;

    // Reflect the view vector around the normal and look up the reflection
    // map for specular highlights.
    float3 reflectionLookup = 2 * normalWorldSpace *
        dot(normalWorldSpace, inView) - inView * dot(normalWorldSpace,
            normalWorldSpace);
    float4 reflectionColor = texCUBE(SpecularCube, reflectionLookup) *
        g_fSpecLightMultiplier;

    // Modulate the reflection by the gloss map.
    reflectionColor *= glossValue;

    // Add in reflection.
    color += reflectionColor;
    
    // Modify alpha for x-ray effect.
    if (g_bModifyAlpha)
    {
        float fDistFalloff = smoothstep(g_fCharacterDist,
            g_fCharacterDist + 50, outMRTValues.w);
        float2 vAlphaTex = float2(
            (inAlphaPos.x / inAlphaPos.w) * 0.5 + 0.5,
            (-inAlphaPos.y / inAlphaPos.w) * 0.5 + 0.5);
        float fAlpha = 1 - tex2D(AlphaSampler, vAlphaTex).r;
        color.a = saturate(fAlpha + fDistFalloff);
    }

    return color;
}

//--------------------------------------------------------------//
// Standard Skinned: base map, 2 point lights, diffuse cube map, 
//   specular cube map, and normal map (with gloss in alpha channel)
//   No dark map or vertex colors
//--------------------------------------------------------------//

// VS_OUTPUT struct uses Standard version

VS_OUTPUT_Standard VSMainSkinned(float3 inPos: POSITION,
    float4 inDiff: COLOR0, 
    float3 inBlendWeights : BLENDWEIGHT, 
    int4 inBlendIndices : BLENDINDICES,
    float2 inTex0: TEXCOORD0, 
    float2 inTex1: TEXCOORD1, 
    float3 inNrm: NORMAL, 
    float3 inBiNrm: BINORMAL, float3 inTan: TANGENT)
{
    VS_OUTPUT_Standard Out;

    float4 worldPos;
    float3 worldNrm;
    Out.Pos = CalculateSkinnedPositionNBT(float4(inPos, 1.0f), inBlendWeights,
        inBlendIndices, inNrm, inBiNrm * bumpiness, inTan * bumpiness, 
        worldPos, worldNrm, Out.Row1, Out.Row0);
    Out.Row2.xyz = worldNrm;

    // Pack MRT z position into the fourth component of Row2.
    Out.Row2.w = mul(worldPos, View).z;

    Out.Tex0 = inTex0;
    Out.Tex1 = inTex1;
    
    // Calculate dynamic lighting
    float4 Diffuse = CalculateDiffuseLightContribution(worldPos.xyz,
        worldNrm.xzy, LightPos0, LightDiff0, LightAtten0);

    Diffuse += CalculateDiffuseLightContribution(worldPos.xyz,
        worldNrm.xzy, LightPos1, LightDiff1, LightAtten1);
    
    Out.Diff = Diffuse;

    // Pass the world space view vector into the pixel shader to calculate
    // the reflection.
    float4 worldSpaceViewLoc = inv_view_matrix[3];
    float3 worldSpaceViewVector = worldSpaceViewLoc - worldPos;
    float3 worldSpaceViewNormalized = normalize(worldSpaceViewVector);

    // turn into DX space
    Out.View = worldSpaceViewNormalized.xzy;
    
    // Propagate alpha position.
    Out.AlphaPos = Out.Pos;

    return Out;
}

float4 PSMainSkinned(float2 inTex0: TEXCOORD0, float2 inTex1: TEXCOORD1, 
    float3 inRow0: TEXCOORD2, float3 inRow1: TEXCOORD3, 
    float4 inRow2: TEXCOORD4, float3 inView: TEXCOORD5, 
    float4 inDiff: TEXCOORD6, float4 inAlphaPos : TEXCOORD7,
    out float4 outMRTValues : COLOR1) : COLOR0
{
    // Output MRT values. Position is stored as the fourth component.
    outMRTValues = float4(normalize(mul(inRow2.xyz, (float3x3) View)),
        inRow2.w);

    // Sample the base map.
    float4 color = tex2D(BaseSampler, inTex0);
    
    // Get the normal in texture space.
    float4 normalTexSpace;
    normalTexSpace = tex2D(NormalSampler, inTex0);

    // Save off the gloss map value from the normal map's alpha slot.
    float glossValue = normalTexSpace.a;
    // Bias the normal in texture space so it is signed.
    normalTexSpace = 2.0 * normalTexSpace - 1.0;

    // Convert the normal to world space and look up the diffues lighting
    // value. Modulate that with the base map.
    float3 normalWorldSpace = 
        mul(normalTexSpace, float3x3(inRow0, inRow1, inRow2.xyz));
    float4 diffuseColor = inDiff + 
        GlobalDimmer * texCUBE(LightCube, normalWorldSpace) *
        g_fDiffLightMultiplier;
    color *= diffuseColor;

    // Reflect the view vector around the normal and look up the reflection
    // map for specular highlights.
    float3 reflectionLookup = 2 * normalWorldSpace *
        dot(normalWorldSpace, inView) - inView * dot(normalWorldSpace,
            normalWorldSpace);
    float4 reflectionColor = texCUBE(SpecularCube, reflectionLookup) * 
        g_fSpecLightMultiplier;

    // Modulate the reflection by the gloss map.
    reflectionColor *= glossValue;

    // Add in reflection.
    color += reflectionColor;

    // Modify alpha for x-ray effect.
    if (g_bModifyAlpha)
    {
        float fDistFalloff = smoothstep(g_fCharacterDist,
            g_fCharacterDist + 50, outMRTValues.w);
        float2 vAlphaTex = float2(
            (inAlphaPos.x / inAlphaPos.w) * 0.5 + 0.5,
            (-inAlphaPos.y / inAlphaPos.w) * 0.5 + 0.5);
        float fAlpha = 1 - tex2D(AlphaSampler, vAlphaTex).r;
        color.a = saturate(fAlpha + fDistFalloff);
    }

    return color;
}

//--------------------------------------------------------------//
// NoNormal: base map, dark map, 2 point lights, and diffuse cube map, 
//   with no normal or gloss maps or specular cube map
//--------------------------------------------------------------//

struct VS_OUTPUT_NoNormal 
{
    float4 Pos  :      POSITION;
    float2 Tex0 :      TEXCOORD0;
    float2 Tex1 :      TEXCOORD1;
    float3 Norm :      TEXCOORD2; 
    float4 Diff :      TEXCOORD3;
    float4 MRTValues : TEXCOORD4;
    float4 AlphaPos:   TEXCOORD5;
};

VS_OUTPUT_NoNormal VSMainNoNormal(float4 inPos: POSITION,
    float4 inDiff: COLOR0, 
    float2 inTex0: TEXCOORD0, float2 inTex1: TEXCOORD1,
    float3 inNrm: NORMAL)
{
    VS_OUTPUT_NoNormal Out;

    float4 worldPos;
    float3 worldNrm;
    Out.Pos = CalculatePosition(inPos, inNrm, worldPos, worldNrm);
    Out.Norm = worldNrm;
    
    // Compute MRT values.
    Out.MRTValues = float4(normalize(mul(worldNrm, (float3x3) View)),
        mul(worldPos, View).z);

    Out.Tex0 = inTex0;
    Out.Tex1 = inTex1;
    
    // Calculate dynamic lighting
    float4 Diffuse = CalculateDiffuseLightContribution(worldPos.xyz,
        worldNrm.xzy, LightPos0, LightDiff0, LightAtten0);

    Diffuse += CalculateDiffuseLightContribution(worldPos.xyz,
        worldNrm.xzy, LightPos1, LightDiff1, LightAtten1);
    
    Out.Diff = inDiff * Diffuse;
    Out.Diff = Diffuse;

    // Propagate alpha position.
    Out.AlphaPos = Out.Pos;

    return Out;
}

float4 PSMainNoNormal(float2 inTex0: TEXCOORD0,
    float2 inTex1: TEXCOORD1, float3 inNorm: TEXCOORD2, 
    float4 inDiff: TEXCOORD3, float4 inMRTValues : TEXCOORD4,
    float4 inAlphaPos : TEXCOORD5, out float4 outMRTValues : COLOR1) : COLOR0
{
    // Output MRT values.
    outMRTValues = inMRTValues;
    
    // Sample the base map.
    float4 color = tex2D(BaseSampler, inTex0);
    
    // Sample the dark map.
    color *= tex2D(DarkSampler, inTex1);
    
    // Use world-space normal to look up the diffuse lighting
    // value. Modulate that with the base map.
    float4 diffuseColor = inDiff + GlobalDimmer * texCUBE(LightCube, inNorm) *
        g_fDiffLightMultiplier;
    color *= diffuseColor;
    
    // Modify alpha for x-ray effect.
    if (g_bModifyAlpha)
    {
        float fDistFalloff = smoothstep(g_fCharacterDist,
            g_fCharacterDist + 50, outMRTValues.w);
        float2 vAlphaTex = float2(
            (inAlphaPos.x / inAlphaPos.w) * 0.5 + 0.5,
            (-inAlphaPos.y / inAlphaPos.w) * 0.5 + 0.5);
        float fAlpha = 1 - tex2D(AlphaSampler, vAlphaTex).r;
        color.a = saturate(fAlpha + fDistFalloff);
    }

    return color;
}

//--------------------------------------------------------------//
// Glow: base map, glow map, 2 point lights, diffuse cube map, 
//   specular cube map, and normal map (with gloss in alpha channel)
//--------------------------------------------------------------//

// VS_OUTPUT struct and VS function use Standard versions

float4 PSMainGlow(float2 inTex0: TEXCOORD0, float2 inTex1: TEXCOORD1,
    float3 inRow0: TEXCOORD2, float3 inRow1: TEXCOORD3, 
    float4 inRow2: TEXCOORD4, float3 inView: TEXCOORD5, 
    float4 inDiff : TEXCOORD6, float4 inAlphaPos : TEXCOORD7,
    out float4 outMRTValues : COLOR1) : COLOR0
{
    // Output MRT values. Position is stored as the fourth component.
    outMRTValues = float4(normalize(mul(inRow2.xyz, (float3x3) View)),
        inRow2.w);
    
    // Sample the base map.
    float4 color = tex2D(BaseSampler, inTex0);
    
    // Get the normal in texture space.
    float4 normalTexSpace;
    normalTexSpace = tex2D(NormalSampler, inTex0);

    // Save off the gloss map value from the normal map's alpha slot.
    float glossValue = normalTexSpace.a;
    // Bias the normal in texture space so it is signed.
    normalTexSpace = 2.0 * normalTexSpace - 1.0;

    // Convert the normal to world space and look up the diffues lighting
    // value. Modulate that with the base map.
    float3 normalWorldSpace = 
        mul(normalTexSpace, float3x3(inRow0, inRow1, inRow2.xyz));
    float4 diffuseColor = inDiff + 
        GlobalDimmer * texCUBE(LightCube, normalWorldSpace) *
        g_fDiffLightMultiplier;
    color *= diffuseColor;

    // Reflect the view vector around the normal and look up the reflection
    // map for specular highlights.
    float3 reflectionLookup = 2 * normalWorldSpace *
        dot(normalWorldSpace, inView) - inView * dot(normalWorldSpace,
            normalWorldSpace);
    float4 reflectionColor = texCUBE(SpecularCube, reflectionLookup) *
        g_fSpecLightMultiplier;

    // Modulate the reflection by the gloss map.
    reflectionColor *= glossValue;

    // Add in reflection.
    color += reflectionColor;

    // Sample the glow map.
    color += (tex2D(GlowSampler, inTex0) * GlowMultiplier);
    
    // Modify alpha for x-ray effect.
    if (g_bModifyAlpha)
    {
        float fDistFalloff = smoothstep(g_fCharacterDist,
            g_fCharacterDist + 50, outMRTValues.w);
        float2 vAlphaTex = float2(
            (inAlphaPos.x / inAlphaPos.w) * 0.5 + 0.5,
            (-inAlphaPos.y / inAlphaPos.w) * 0.5 + 0.5);
        float fAlpha = 1 - tex2D(AlphaSampler, vAlphaTex).r;
        color.a = saturate(fAlpha + fDistFalloff);
    }
    
    return color;
}

//--------------------------------------------------------------//
// GlowOnly: base map and glow map
//--------------------------------------------------------------//

struct VS_OUTPUT_GlowOnly 
{
    float4 Pos  :     POSITION;
    float2 Tex0 :     TEXCOORD0;
    float4 VC0  :     COLOR0;
};

VS_OUTPUT_GlowOnly VSGlowOnly(
    float4 inPos: POSITION,
    float2 inTex0: TEXCOORD0,
    float4 inVC: COLOR0)
{
    VS_OUTPUT_GlowOnly Out;

    Out.Pos = mul(inPos, WorldViewProj);
    Out.Tex0 = inTex0;
    Out.VC0 = inVC;
    
    return Out;
}

float4 PSGlowOnly(float2 inTex0: TEXCOORD0, float4 inVC0 : COLOR0,
    out float4 outMRTValues : COLOR1) : COLOR0
{
    outMRTValues = float4(0.0, 0.0, 0.0, 0.0);

    // Sample the base map.
    float4 color = tex2D(BaseSampler, inTex0);
    
    color.rgb *= GlowMultiplier;
    color.a *= inVC0.a;
    
    return color;
}

//--------------------------------------------------------------//
// Technique Section
//--------------------------------------------------------------//
technique GeneralDiffuseSpecular
<
    string NBTMethod = "ATI";
    string description = "Shader for environment of MetalWars demo. "
        "Base map, dark map, 2 point lights, diffuse cube map, " 
        "specular cube map, and normal map (with gloss in alpha channel). "
        "This shader was designed and built specifically for the "
        "MetalWars demo.";
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 VSMainStandard();
        PixelShader = compile ps_2_0 PSMainStandard();
    }
}
//--------------------------------------------------------------//
technique GeneralDiffuseSpecularSkinned
<
    string NBTMethod = "ATI";
    string description = "Shader for skinned character of MetalWars demo. "
        "Base map, 2 point lights, diffuse cube map, " 
        "specular cube map, and normal map (with gloss in alpha channel) ."
        "This shader was designed and built specifically for the "
        "MetalWars demo.";
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
    int BonesPerPartition = MAX_BONES;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 VSMainSkinned();
        PixelShader = compile ps_2_0 PSMainSkinned();
    }
}
//--------------------------------------------------------------//
technique GeneralDiffuseSpecularNoNormal
<
    string description = "Shader for environment of MetalWars demo. "
        "Base map, dark map, 2 point lights, and diffuse cube map, " 
        "with no normal or gloss maps or specular cube map. "
        "This shader was designed and built specifically for the "
        "MetalWars demo.";
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 VSMainNoNormal();
        PixelShader = compile ps_2_0 PSMainNoNormal();
    }
}
//--------------------------------------------------------------//
technique GeneralDiffuseSpecularGlow
<
    string NBTMethod = "ATI";
    string description = "Shader for environment of MetalWars demo. "
        "Base map, glow map, 2 point lights, diffuse cube map, " 
        "specular cube map, and normal map (with gloss in alpha channel). "
        "This shader was designed and built specifically for the "
        "MetalWars demo.";
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 VSMainStandard();
        PixelShader = compile ps_2_0 PSMainGlow();
    }
}
//--------------------------------------------------------------//
technique GeneralDiffuseSpecularGlowSkinned
<
    string NBTMethod = "ATI";
    string description = "Shader for skinned character in MetalWars demo. "
        "Base map, glow map, 2 point lights, diffuse cube map, " 
        "specular cube map, and normal map (with gloss in alpha channel). "
        "This shader was designed and built specifically for the "
        "MetalWars demo.";
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
    int BonesPerPartition = MAX_BONES;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 VSMainSkinned();
        PixelShader = compile ps_2_0 PSMainGlow();
    }
}
//--------------------------------------------------------------//
technique GeneralGlow
<
    string description = "Shader for environment of MetalWars demo. "
        "Base map and glow map. "
        "This shader was designed and built specifically for the "
        "MetalWars demo."; 
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 VSGlowOnly();
        PixelShader = compile ps_2_0 PSGlowOnly();
    }
}
//--------------------------------------------------------------//
