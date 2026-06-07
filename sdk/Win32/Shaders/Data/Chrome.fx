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

//--------------------------------------------------------------//
// Chrome
//--------------------------------------------------------------//

// textures
texture BaseMap
<
    string NTM = "base";
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
};

sampler NormalSampler = sampler_state
{
    Texture = (NormalMap);
    MagFilter = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
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

// Constants
float bumpiness : ATTRIBUTE 
<
    float min = 0.0;
    float max = 2.0;
> = 1.0;

float4x4 WorldViewProj : WORLDVIEWPROJECTION;
float4x4 ViewProj : VIEWPROJECTION;
float4x4 inv_view_matrix : INVVIEW;
float4x4 world_matrix : WORLD;

static const int MAX_BONES = 29;
float4x3 SkinBone[MAX_BONES] : SKINBONEMATRIX3;

//--------------------------------------------------------------//
// Transform Utility functions
//--------------------------------------------------------------//
float4 CalculateSkinnedPosition(float4 inPos, float4 inBlendWeights,
    float4 inBlendIndices)
{
	// Compensate for lack of UBYTE4 on Geforce3
    int4 indices = D3DCOLORtoUBYTE4(inBlendIndices);

    // Calculate normalized fourth bone weight
    float weight4 = 1.0f - inBlendWeights[0] - inBlendWeights[1] - 
        inBlendWeights[2];

    // Calculate bone transform
    float4x3 SkinBoneTransform;
	SkinBoneTransform = inBlendWeights[0] * SkinBone[indices[0]];
	SkinBoneTransform += inBlendWeights[1] * SkinBone[indices[1]];
	SkinBoneTransform += inBlendWeights[2] * SkinBone[indices[2]];
	SkinBoneTransform += weight4 * SkinBone[indices[3]];
	
	float4 WorldSpacePos = float4(mul(inPos, SkinBoneTransform), 1.0);
    float4 ProjectedPos = mul(WorldSpacePos, ViewProj);
    
    return ProjectedPos;
}

float4 CalculateSkinnedPositionNBT(float4 inPos, float4 inBlendWeights,
    float4 inBlendIndices, float3 inNrm, float3 inBiNrm, float3 inTan,
    out float4 outPos, out float3 outNrm, out float3 outBiNrm, 
    out float3 outTan)
{
	// Compensate for lack of UBYTE4 on Geforce3
    int4 indices = D3DCOLORtoUBYTE4(inBlendIndices);

    // Calculate normalized fourth bone weight
    float weight4 = 1.0f - inBlendWeights[0] - inBlendWeights[1] - 
        inBlendWeights[2];

    // Calculate bone transform
    float4x3 SkinBoneTransform;
	SkinBoneTransform = inBlendWeights[0] * SkinBone[indices[0]];
	SkinBoneTransform += inBlendWeights[1] * SkinBone[indices[1]];
	SkinBoneTransform += inBlendWeights[2] * SkinBone[indices[2]];
	SkinBoneTransform += weight4 * SkinBone[indices[3]];
	
	outPos = float4(mul(inPos, SkinBoneTransform), 1.0);
    float4 ProjectedPos = mul(outPos, ViewProj);
    
    // Munge up the basis vectors to get a good matrix here for the texture
    // operations. This matrix should convert from tangent space to world
    // space.
    float3 viewNrm = mul(inNrm, (float3x3)SkinBoneTransform);
    float3 viewBiNrm = mul(inBiNrm, (float3x3)SkinBoneTransform);
    float3 viewTan = mul(inTan, (float3x3)SkinBoneTransform);
    
    // turn into DX space
    outNrm = viewNrm.xzy;
    outBiNrm = viewBiNrm.xzy;
    outTan = viewTan.xzy;

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
    outNrm = viewNrm.xzy;
    outBiNrm = viewBiNrm.xzy;
    outTan = viewTan.xzy;

    return ProjectedPos;
}
//--------------------------------------------------------------//
// Single Pass
//--------------------------------------------------------------//

struct VS_OUTPUT_SinglePass 
{
    float4 Pos:	    POSITION;
    float2 Tex:	    TEXCOORD0;
    float3 Row0:	TEXCOORD1;
    float3 Row1:	TEXCOORD2;
    float3 Row2:	TEXCOORD3;
    float3 View:	TEXCOORD4;  
};

VS_OUTPUT_SinglePass singlePassVSMain( float4 inPos: POSITION,
    float2 inTex: TEXCOORD0, float3 inNrm: NORMAL,
    float3 inBiNrm: BINORMAL, float3 inTan: TANGENT )
{
    VS_OUTPUT_SinglePass Out;

    float4 worldPos;
    Out.Pos = CalculatePositionNBT(inPos, inNrm, inBiNrm * bumpiness, 
        inTan * bumpiness, worldPos, Out.Row2, Out.Row1, Out.Row0);
    Out.Tex = inTex;

    // Pass the world space view vector into the pixel shader to calculate
    // the reflection.
    float4 worldSpaceViewLoc = inv_view_matrix[3];
    float3 worldSpaceViewVector = worldSpaceViewLoc - worldPos;
    float3 worldSpaceViewNormalized = normalize(worldSpaceViewVector);

    // turn into DX space
    Out.View = worldSpaceViewNormalized.xzy;

    return Out;
}

VS_OUTPUT_SinglePass singlePassSkinnedVSMain( float4 inPos: POSITION,
    float4 inBlendWeights : BLENDWEIGHT, float4 inBlendIndices : BLENDINDICES,
    float2 inTex: TEXCOORD0, float3 inNrm: NORMAL,
    float3 inBiNrm: BINORMAL, float3 inTan: TANGENT )
{
    VS_OUTPUT_SinglePass Out;

    float4 worldPos;
    Out.Pos = CalculateSkinnedPositionNBT(inPos, inBlendWeights,
        inBlendIndices, inNrm, inBiNrm * bumpiness, inTan * bumpiness,
        worldPos, Out.Row2, Out.Row1, Out.Row0);
    Out.Tex = inTex;

    // Pass the world space view vector into the pixel shader to calculate
    // the reflection.
    float4 worldSpaceViewLoc = inv_view_matrix[3];
    float3 worldSpaceViewVector = worldSpaceViewLoc - worldPos;
    float3 worldSpaceViewNormalized = normalize(worldSpaceViewVector);

    // turn into DX space
    Out.View = worldSpaceViewNormalized.xzy;

    return Out;
}


float4 singlePassPSMain( float2 inTex: TEXCOORD0, float3 inRow0: TEXCOORD1,
    float3 inRow1: TEXCOORD2, float3 inRow2: TEXCOORD3,
    float3 inView: TEXCOORD4 ) : COLOR0
{
    // Sample the base map.
    float4 color;
    color = tex2D(BaseSampler, inTex);

    // Get the normal in texture space.
    float4 normalTexSpace;
    normalTexSpace = tex2D(NormalSampler, inTex);

    // Save off the gloss map value from the normal map's alpha slot.
    float glossValue = normalTexSpace.a;
    // Bias the normal in texture space so it is signed.
    normalTexSpace = 2.0 * normalTexSpace - 1.0;

    // Convert the normal to world space and look up the diffues lighting
    // value. Modulate that with the base map.
    float3 normalWorldSpace = 
        mul(normalTexSpace, float3x3(inRow0, inRow1, inRow2));
    float4 diffuseColor = texCUBE(LightCube, normalWorldSpace);
    color *= diffuseColor;

    // Reflect the view vector around the normal and look up the reflection
    // map for specular highlights.
    float3 reflectionLookup = 2 * normalWorldSpace *
        dot(normalWorldSpace, inView) - inView * dot(normalWorldSpace,
            normalWorldSpace);
    float4 reflectionColor = texCUBE(SpecularCube, reflectionLookup);

    // Modulate the reflection by the gloss map.
    reflectionColor *= glossValue;

    // Add in reflection.
    color += reflectionColor;

    return color;
}

//--------------------------------------------------------------//
// Three Pass
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Base Pass
//--------------------------------------------------------------//
struct VS_OUTPUT_ThreePassBase 
{
    float4 Pos:	POSITION;
    float2 Tex:	TEXCOORD0;
};

// Note that this vertex shader does not need all the inputs, but it is a
// limitation of the Gamebryo shader system that the first pass vertex shader
// is used to determine the packing definition.
VS_OUTPUT_ThreePassBase threePassBaseVSMain( float4 inPos: POSITION,
    float2 inTex: TEXCOORD0, float3 inNrm: NORMAL, float3 inBiNrm: BINORMAL,
    float3 inTan: TANGENT )
{
    VS_OUTPUT_ThreePassBase Out;

    Out.Pos = mul(inPos, WorldViewProj);
    Out.Tex = inTex;

    return Out;
}

VS_OUTPUT_ThreePassBase threePassBaseSkinnedVSMain( float4 inPos: POSITION,
    float4 inBlendWeights : BLENDWEIGHT, float4 inBlendIndices : BLENDINDICES,
    float2 inTex: TEXCOORD0, float3 inNrm: NORMAL, float3 inBiNrm: BINORMAL,
    float3 inTan: TANGENT )
{
    VS_OUTPUT_ThreePassBase Out;

    Out.Pos = CalculateSkinnedPosition(inPos, inBlendWeights, inBlendIndices);
    Out.Tex = inTex;

    return Out;
}

float4 threePassBasePSMain( float2 inTex: TEXCOORD0 ) : COLOR0
{
    // Output base color:
    float4 color;
    color = tex2D(BaseSampler, inTex);
    return color;
}

//--------------------------------------------------------------//
// Diffuse Lighting
//--------------------------------------------------------------//

struct VS_OUTPUT_ThreePassDiffuse 
{
    float4 Pos:	    POSITION;
    float2 Tex:	    TEXCOORD0;
    float3 Row0:	TEXCOORD1;
    float3 Row1:	TEXCOORD2;
    float3 Row2:	TEXCOORD3;
};

VS_OUTPUT_ThreePassDiffuse threePassDiffuseVSMain( float4 inPos: POSITION,
    float2 inTex: TEXCOORD0, float3 inNrm: NORMAL, float3 inBiNrm: BINORMAL,
    float3 inTan: TANGENT )
{
    VS_OUTPUT_ThreePassDiffuse Out;

    float4 worldPos; // Note that this is unused, and should get optimized out
    float3x3 tempMat;
    Out.Pos = CalculatePositionNBT(inPos, inNrm, inBiNrm * bumpiness, 
        inTan * bumpiness, worldPos, tempMat[0], tempMat[1], tempMat[2]);
    Out.Tex = inTex;

    // Put the world space view vector into the w components of the texture
    // coordinates for the pixel shader to perform the texm3x3tex.
    Out.Row0[0] = tempMat[2][0];
    Out.Row0[1] = tempMat[1][0];
    Out.Row0[2] = tempMat[0][0];
    Out.Row1[0] = tempMat[2][1];
    Out.Row1[1] = tempMat[1][1];
    Out.Row1[2] = tempMat[0][1];
    Out.Row2[0] = tempMat[2][2];
    Out.Row2[1] = tempMat[1][2];
    Out.Row2[2] = tempMat[0][2];

    return Out;
}

VS_OUTPUT_ThreePassDiffuse threePassDiffuseSkinnedVSMain( 
    float4 inPos: POSITION, float4 inBlendWeights : BLENDWEIGHT, 
    float4 inBlendIndices : BLENDINDICES, float2 inTex: TEXCOORD0, 
    float3 inNrm: NORMAL, float3 inBiNrm: BINORMAL, float3 inTan: TANGENT )
{
    VS_OUTPUT_ThreePassDiffuse Out;

    float4 worldPos; // Note that this is unused, and should get optimized out
    float3x3 tempMat;
    Out.Pos = CalculateSkinnedPositionNBT(inPos, inBlendWeights,
        inBlendIndices, inNrm, inBiNrm * bumpiness, inTan * bumpiness,
        worldPos, tempMat[0], tempMat[1], tempMat[2]);
    Out.Tex = inTex;
    
    // Put the world space view vector into the w components of the texture
    // coordinates for the pixel shader to perform the texm3x3tex.
    Out.Row0[0] = tempMat[2][0];
    Out.Row0[1] = tempMat[1][0];
    Out.Row0[2] = tempMat[0][0];
    Out.Row1[0] = tempMat[2][1];
    Out.Row1[1] = tempMat[1][1];
    Out.Row1[2] = tempMat[0][1];
    Out.Row2[0] = tempMat[2][2];
    Out.Row2[1] = tempMat[1][2];
    Out.Row2[2] = tempMat[0][2];

    return Out;
}

float4 threePassDiffusePSMain(float2 inTex: TEXCOORD0,
   float3 inRow0: TEXCOORD1, float3 inRow1: TEXCOORD2,
   float3 inRow2: TEXCOORD3) : COLOR0
{
    // These instructions should compile under ps_1_1 to a dependent read
    // using a texm3x3tex instruction.
    float3 normal = tex2D(NormalSampler, inTex);
    float3 cubeVec;
    cubeVec.x = dot(normal * 2 - 1, inRow0);
    cubeVec.y = dot(normal * 2 - 1, inRow1);
    cubeVec.z = dot(normal * 2 - 1, inRow2);
    float4 color = texCUBE(LightCube, cubeVec);

    return color;
}

//--------------------------------------------------------------//
// Specular Addition
//--------------------------------------------------------------//

struct VS_OUTPUT_ThreePassSpec 
{
    float4 Pos:	    POSITION;
    float2 Tex:	    TEXCOORD0;
    float4 Row0:	TEXCOORD1;
    float4 Row1:	TEXCOORD2;
    float4 Row2:	TEXCOORD3;
};

VS_OUTPUT_ThreePassSpec threePassSpecVSMain( float4 inPos: POSITION,
    float2 inTex: TEXCOORD0, float3 inNrm: NORMAL, float3 inBiNrm: BINORMAL,
    float3 inTan: TANGENT )
{
    VS_OUTPUT_ThreePassSpec Out;

    float4 worldPos;
    float3x3 tempMat;
    Out.Pos = CalculatePositionNBT(inPos, inNrm, inBiNrm * bumpiness, 
        inTan * bumpiness, worldPos, tempMat[0], tempMat[1], tempMat[2]);
    Out.Tex = inTex;

    // Pass the world space view vector into the pixel shader to calculate
    // the reflection.
    float4 worldSpaceViewLoc = inv_view_matrix[3];
    float3 worldSpaceViewVector = worldSpaceViewLoc - worldPos;
    worldSpaceViewVector = normalize(worldSpaceViewVector);

    // Put the world space view vector into the w components of the texture
    // coordinates for the pixel shader to perform the texm3x3vspec.
    Out.Row0[0] = tempMat[2][0];
    Out.Row0[1] = tempMat[1][0];
    Out.Row0[2] = tempMat[0][0];
    Out.Row0[3] = worldSpaceViewVector.x;
    Out.Row1[0] = tempMat[2][1];
    Out.Row1[1] = tempMat[1][1];
    Out.Row1[2] = tempMat[0][1];
    Out.Row1[3] = worldSpaceViewVector.y;
    Out.Row2[0] = tempMat[2][2];
    Out.Row2[1] = tempMat[1][2];
    Out.Row2[2] = tempMat[0][2];
    Out.Row2[3] = worldSpaceViewVector.z;
    
    return Out;
}

VS_OUTPUT_ThreePassSpec threePassSpecSkinnedVSMain( float4 inPos: POSITION,
    float4 inBlendWeights : BLENDWEIGHT, float4 inBlendIndices : BLENDINDICES,
    float2 inTex: TEXCOORD0, float3 inNrm: NORMAL, float3 inBiNrm: BINORMAL,
    float3 inTan: TANGENT )
{
    VS_OUTPUT_ThreePassSpec Out;

    float4 worldPos;
    float3x3 tempMat;
    Out.Pos = CalculateSkinnedPositionNBT(inPos, inBlendWeights, 
        inBlendIndices, inNrm, inBiNrm * bumpiness, inTan * bumpiness,
        worldPos, tempMat[0], tempMat[1], tempMat[2]);
    Out.Tex = inTex;

    // Pass the world space view vector into the pixel shader to calculate
    // the reflection.
    float4 worldSpaceViewLoc = inv_view_matrix[3];
    float3 worldSpaceViewVector = worldSpaceViewLoc - worldPos;
    worldSpaceViewVector = normalize(worldSpaceViewVector);

    // Put the world space view vector into the w components of the texture
    // coordinates for the pixel shader to perform the texm3x3vspec.
    Out.Row0[0] = tempMat[2][0];
    Out.Row0[1] = tempMat[1][0];
    Out.Row0[2] = tempMat[0][0];
    Out.Row0[3] = worldSpaceViewVector.x;
    Out.Row1[0] = tempMat[2][1];
    Out.Row1[1] = tempMat[1][1];
    Out.Row1[2] = tempMat[0][1];
    Out.Row1[3] = worldSpaceViewVector.y;
    Out.Row2[0] = tempMat[2][2];
    Out.Row2[1] = tempMat[1][2];
    Out.Row2[2] = tempMat[0][2];
    Out.Row2[3] = worldSpaceViewVector.z;
    
    return Out;
}

float4 threePassSpecPSMain( float2 inTex: TEXCOORD0, float4 inRow0: TEXCOORD1,
   float4 inRow1: TEXCOORD2, float4 inRow2: TEXCOORD3) : COLOR0
{
    // This block comes from a book. It should compile under ps_1_1 target
    // to 4 texture ops that perform a texm3x3vspec operation.
    float4 normal = tex2D(NormalSampler, inTex);
    float3 cubeVec;
    float3x3 mat = float3x3((float3)inRow0, (float3)inRow1,
        (float3)inRow2);
    cubeVec = mul(((float3)normal) * 2 - 1, transpose(mat));
    float3 view;
    view.x = inRow0.w;
    view.y = inRow1.w;
    view.z = inRow2.w;
    cubeVec = dot(cubeVec, view) * cubeVec * 2 - view * dot(cubeVec, cubeVec);
    float4 color = texCUBE(SpecularCube, cubeVec);

    // Modulate by the normal maps alpha which is the gloss map for this
    // shader.
    color *= normal.a;
    return color;
}
//--------------------------------------------------------------//
// Technique Section for Chrome
//--------------------------------------------------------------//
technique Chrome
<
    string NBTMethod = "ATI";
    int NBTSourceUV = 0;
    string description = "This shader produces a chrome effect using a "
        "base map, diffuse cube map, specular cube map, and normal map "
        "(that contains gloss in alpha channel). It has a PS2.0 version "
        "that can render in one pass, and a PS1.1 version that can render "
        "in three passes.";
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
    int implementation = 0;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 singlePassVSMain();
        PixelShader = compile ps_2_0 singlePassPSMain();

        AlphaBlendEnable = false;
    }
}

technique ThreePassChrome
<
    string NBTMethod = "ATI";
    int NBTSourceUV = 0;
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
    string shadername = "Chrome";
    int implementation = 1;
>
{
    pass Diffuse_Lighting
    {
        ALPHABLENDENABLE = FALSE;

        VertexShader = compile vs_1_1 threePassDiffuseVSMain();
        
#if defined(_XENON)
        PixelShader = compile ps_1_1 threePassDiffusePSMain();
#else
        PixelShader = asm
        {
            // Compiled version of threePassDiffusePSMain
            // As of December 2006 DX SDK, compilation of HLSL code to
            // ps_1_x is not longer supported.

            ps_1_1
            tex t0
            texm3x3pad t1, t0_bx2
            texm3x3pad t2, t0_bx2
            texm3x3vspec t3, t0_bx2
            mul r0, t0.w, t3
        };
        Sampler[0] = NormalSampler;
        Sampler[3] = SpecularCube;
#endif
    }

    pass Base_Pass
    {
        ALPHABLENDENABLE = TRUE;
        ALPHATESTENABLE = FALSE;
        BLENDOP = ADD;
        DESTBLEND = ZERO;
        SRCBLEND = DESTCOLOR;

        VertexShader = compile vs_1_1 threePassBaseVSMain();
        
#if defined(_XENON)
        PixelShader = compile ps_1_1 threePassBasePSMain();
#else
        PixelShader = asm
        {
            // Compiled version of threePassBasePSMain
            // As of December 2006 DX SDK, compilation of HLSL code to
            // ps_1_x is not longer supported.

            ps_1_1
            tex t0
            mov r0, t0
        };
        Sampler[0] = BaseSampler;
#endif
    }

    pass Specular_Addition
    {
        ALPHABLENDENABLE = TRUE;
        ALPHATESTENABLE = FALSE;
        BLENDOP = ADD;
        DESTBLEND = ONE;
        SRCBLEND = ONE;

        VertexShader = compile vs_1_1 threePassSpecVSMain();
        
#if defined(_XENON)
        PixelShader = compile ps_1_1 threePassSpecPSMain();
#else
        PixelShader = asm
        {
            // Compiled version of threePassSpecPSMain 
            // As of December 2006 DX SDK, compilation of HLSL code to
            // ps_1_x is not longer supported.

            ps_1_1
            tex t0
            texm3x3pad t1, t0_bx2
            texm3x3pad t2, t0_bx2
            texm3x3vspec t3, t0_bx2
            mul r0, t0.w, t3
        };
        Sampler[0] = NormalSampler;
        Sampler[3] = SpecularCube;
#endif
    }
}

technique ChromeSkinned
<
    string NBTMethod = "ATI";
    int NBTSourceUV = 0;
    string description = "This shader produces a chrome effect using a "
        "base map, diffuse cube map, specular cube map, and normal map "
        "(that contains gloss in alpha channel). It has a PS2.0 version "
        "that can render in one pass, and a PS1.1 version that can render "
        "in three passes. This version performs skinning using 29 bones per "
        "partition. ";
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
    int implementation = 0;
	int BonesPerPartition = MAX_BONES;
    bool BlendIndicesAsD3DColor = true;
>
{
    pass Single_Pass
    {
        VertexShader = compile vs_1_1 singlePassSkinnedVSMain();
        PixelShader = compile ps_2_0 singlePassPSMain();

        AlphaBlendEnable = false;
    }
}

technique ThreePassChromeSkinned
<
    string NBTMethod = "ATI";
    int NBTSourceUV = 0;
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
    string shadername = "ChromeSkinned";
    int implementation = 1;
	int BonesPerPartition = MAX_BONES;
    bool BlendIndicesAsD3DColor = true;
>
{
    pass Diffuse_Lighting
    {
        ALPHABLENDENABLE = FALSE;

        VertexShader = compile vs_1_1 threePassDiffuseSkinnedVSMain();
        
#if defined(_XENON)
        PixelShader = compile ps_1_1 threePassDiffusePSMain();
#else
        PixelShader = asm
        {
            // Compiled version of threePassDiffusePSMain
            // As of December 2006 DX SDK, compilation of HLSL code to
            // ps_1_x is not longer supported.

            ps_1_1
            tex t0
            texm3x3pad t1, t0_bx2
            texm3x3pad t2, t0_bx2
            texm3x3vspec t3, t0_bx2
            mul r0, t0.w, t3
        };
        Sampler[0] = NormalSampler;
        Sampler[3] = SpecularCube;
#endif
    }

    pass Base_Pass
    {
        ALPHABLENDENABLE = TRUE;
        ALPHATESTENABLE = FALSE;
        BLENDOP = ADD;
        DESTBLEND = ZERO;
        SRCBLEND = DESTCOLOR;

        VertexShader = compile vs_1_1 threePassBaseSkinnedVSMain();
        
#if defined(_XENON)
        PixelShader = compile ps_1_1 threePassBasePSMain();
#else
        PixelShader = asm
        {
            // Compiled version of threePassBasePSMain
            // As of December 2006 DX SDK, compilation of HLSL code to
            // ps_1_x is not longer supported.

            ps_1_1
            tex t0
            mov r0, t0
        };
        Sampler[0] = BaseSampler;
#endif
    }

    pass Specular_Addition
    {
        ALPHABLENDENABLE = TRUE;
        ALPHATESTENABLE = FALSE;
        BLENDOP = ADD;
        DESTBLEND = ONE;
        SRCBLEND = ONE;

        VertexShader = compile vs_1_1 threePassSpecSkinnedVSMain();
        
#if defined(_XENON)
        PixelShader = compile ps_1_1 threePassSpecPSMain();
#else
        PixelShader = asm
        {
            // Compiled version of threePassSpecPSMain 
            // As of December 2006 DX SDK, compilation of HLSL code to
            // ps_1_x is not longer supported.

            ps_1_1
            tex t0
            texm3x3pad t1, t0_bx2
            texm3x3pad t2, t0_bx2
            texm3x3vspec t3, t0_bx2
            mul r0, t0.w, t3
        };
        Sampler[0] = NormalSampler;
        Sampler[3] = SpecularCube;
#endif

    }
}
