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
float4x4 g_World;
float4x4 g_ViewProj;
float4 g_MaterialEmissive;
float4 g_AmbientLight;
float4 g_PointAmbient0;
float4 g_PointDiffuse0;
float4 g_PointWorldPosition0;
float4 g_PointAttenuation0;
float4 g_DirAmbient0;
float4 g_DirDiffuse0;
float4 g_DirWorldDirection0;
float4x4 UVSet0_TexTransform;
float4x4 UVSet1_TexTransform;
float4x4 UVSet2_TexTransform;

//---------------------------------------------------------------------------
// Functions:
//---------------------------------------------------------------------------
/*
    Separate a float4 into a float3 and a float. 
*/
void SplitColorAndOpacity(float4 ColorAndOpacity,
    out float3 Color, out float Opacity)
{
    Color.rgb = ColorAndOpacity.rgb;
    Opacity = ColorAndOpacity.a;
}
//---------------------------------------------------------------------------
/*
    This fragment is responsible for applying the view projection transform
    to the input position. Additionally, this fragment applies the world 
    transform to the input position. 
*/
void TransformPosition(float3 Position,
    float4x4 World, out float4 WorldPos)
{
    // Transform the position into world space for lighting, and projected 
    // space for display
    WorldPos = mul( float4(Position, 1.0f), World );
}
//---------------------------------------------------------------------------
/*
    This fragment is responsible for applying the view projection transform
    to the input world position.
*/
void ProjectPositionWorldToProj(float4 WorldPosition,
    float4x4 ViewProjection, out float4 ProjPos)
{
    ProjPos = mul(WorldPosition, ViewProjection );
}
//---------------------------------------------------------------------------
/*
    This fragment is responsible for applying a transform to the input set
    of texture coordinates.
*/
void TexTransformApply(float2 TexCoord,
    float4x4 TexTransform, out float2 TexCoordOut)
{
    TexCoordOut = mul(float4(TexCoord.x, TexCoord.y, 0.0, 1.0), TexTransform);
}
//---------------------------------------------------------------------------
/*
    This fragment is responsible for applying the world transform to the
    normal.
*/
void TransformNormal(float3 Normal,
    float4x4 World, out float3 WorldNrm)
{
    // Transform the normal into world space for lighting
    WorldNrm = mul( Normal, (float3x3)World );
    WorldNrm = normalize(WorldNrm);
}
//---------------------------------------------------------------------------
/*
    This fragment is responsible for normalizing a float3.
*/
void NormalizeFloat3(float3 VectorIn,
    out float3 VectorOut)
{
    VectorOut = normalize(VectorIn);
}
//---------------------------------------------------------------------------
/*
    This fragment is responsible for accumulating the effect of a light
    on the current pixel.
    LightType can be one of three values:
    0 - Directional
    1 - Point 
    2 - Spot

    Note that the LightType must be a compile-time variable,
    not a runtime constant/uniform variable on most Shader Model 2.0 cards.
    The compiler will optimize out any constants that aren't used.
    Attenuation is defined as (const, linear, quad, range).
    Range is not implemented at this time.
    SpotAttenuation is stored as (cos(theta/2), cos(phi/2), falloff)
    theta is the angle of the inner cone and phi is the angle of the outer
    cone in the traditional DX manner. Gamebryo only allows setting of
    phi, so cos(theta/2) will typically be cos(0) or 1. To disable spot
    effects entirely, set cos(theta/2) and cos(phi/2) to -1 or lower.
*/
void Light(float4 WorldPos,
    float3 WorldNrm,
    int LightType,
    bool SpecularEnable,
    float Shadow,
    float3 WorldViewVector,
    float4 LightPos,
    float3 LightAmbient,
    float3 LightDiffuse,
    float3 LightSpecular,
    float3 LightAttenuation,
    float3 LightSpotAttenuation,
    float3 LightDirection,
    float4 SpecularPower,
    float3 AmbientAccum,
    float3 DiffuseAccum,
    float3 SpecularAccum,
    out float3 AmbientAccumOut,
    out float3 DiffuseAccumOut,
    out float3 SpecularAccumOut)
{
    // Get the world space light vector.
    float3 LightVector;
    float DistanceToLight;
    if (LightType == 0)
    {
        LightVector = -LightDirection;
    }
    else
    {
        LightVector = LightPos - WorldPos;
        DistanceToLight = length(LightVector);
        LightVector = LightVector / DistanceToLight;
    }

    // Take N dot L as intensity.
    float LightNDotL = dot(LightVector, WorldNrm);
    float LightIntensity = max(0, LightNDotL);
    float Attenuate = 1.0;

    if (LightType != 0)
    {
        // Attenuate Here
        Attenuate = LightAttenuation.x + LightAttenuation.y * DistanceToLight +
            LightAttenuation.z * DistanceToLight * DistanceToLight;
        Attenuate = max(1.0, Attenuate);
        Attenuate = 1.0 / Attenuate;

        if (LightType == 2)
        {
            // Get intensity as cosine of light vector and direction.
            float CosAlpha = dot(-LightVector, LightDirection);
            // Factor in inner and outer cone angles.
            CosAlpha = saturate((CosAlpha - LightSpotAttenuation.y) / 
            (LightSpotAttenuation.x - LightSpotAttenuation.y));
            // Power to falloff.
            CosAlpha = pow(CosAlpha, LightSpotAttenuation.z);
            // Multiply the spot attenuation into the overall attenuation.
            Attenuate *= CosAlpha;
        }

        LightIntensity = LightIntensity * Attenuate;
    }

    // Determine the interaction of diffuse color of light and material.
    // Scale by the attenuated intensity.
    DiffuseAccumOut = DiffuseAccum;
    DiffuseAccumOut.rgb += LightDiffuse.rgb * LightIntensity * Shadow;

    // Determine ambient contribution - not affected by shadow
    AmbientAccumOut = AmbientAccum;
    AmbientAccumOut.rgb += LightAmbient.rgb * Attenuate;

    SpecularAccumOut = SpecularAccum;
    if (SpecularEnable)
    {
        // Get the half vector.
        float3 LightHalfVector = LightVector + WorldViewVector;
        LightHalfVector = normalize(LightHalfVector);

        // Determine specular intensity.
        float LightNDotH = max(0, dot(WorldNrm, LightHalfVector));
        float LightSpecIntensity = pow(LightNDotH, SpecularPower.x);
        float SpecularMultiplier = LightNDotL > 0.0 ? 1.0 : 0.0;

        // Attenuate Here
        LightSpecIntensity = LightSpecIntensity * Attenuate * SpecularMultiplier;

        // Determine the interaction of specular color of light and material.
        // Scale by the attenuated intensity.
        SpecularAccumOut.rgb += Shadow * LightSpecIntensity * LightSpecular;
    }
}

//---------------------------------------------------------------------------

/*
This fragment is responsible for computing the coefficients for the 
following equations:
Kdiffuse = MatEmissive + 
    MatAmbient * Summation(0...N){LightAmbientContribution[N]} + 
    MatDiffuse * Summation(0..N){LightDiffuseContribution[N]}
Kspecular = MatSpecular * Summation(0..N){LightSpecularContribution[N]}
*/
void ComputeShadingCoefficients(float3 MatEmissive,
    float3 MatDiffuse, float3 MatAmbient,
    float3 MatSpecular, float3 LightSpecularAccum,
    float3 LightDiffuseAccum, float3 LightAmbientAccum,
    bool Saturate,  out float3 Diffuse, out float3 Specular)
{
    Diffuse = MatEmissive + MatAmbient * LightAmbientAccum + 
    MatDiffuse * LightDiffuseAccum;
    Specular = MatSpecular * LightSpecularAccum;

    if (Saturate)
    {
        Diffuse = saturate(Diffuse);
        Specular = saturate(Specular);
    }
}

//---------------------------------------------------------------------------
/*
This fragment is responsible for computing the final RGBA color.
*/
void CompositeFinalRGBAColor(float3 FinalColor,
    float FinalOpacity, out float4 OutputColor)
{
    OutputColor.rgb = FinalColor.rgb;
    OutputColor.a = saturate(FinalOpacity);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Input:
//---------------------------------------------------------------------------
struct Input
{
    float3 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 UVSet0 : TEXCOORD0;
    float2 UVSet1 : TEXCOORD1;
    float2 UVSet2 : TEXCOORD2;
    float4 VertexColors : COLOR0;
};
//---------------------------------------------------------------------------
// Output:
//---------------------------------------------------------------------------
struct Output
{
    float4 PosProjected : POSITION0;
    float4 DiffuseAccum : TEXCOORD0;
    float2 UVSet0 : TEXCOORD1;
    float2 UVSet1 : TEXCOORD2;
    float2 UVSet2 : TEXCOORD3;
};
//---------------------------------------------------------------------------
// Main():
//---------------------------------------------------------------------------
Output vmain(Input In)
{
    Output Out;

    float3 Color;
    float Opacity;
    SplitColorAndOpacity(In.VertexColors, Color, Opacity);

    float4 WorldPos;
    TransformPosition(In.Position, g_World, WorldPos);
    ProjectPositionWorldToProj(WorldPos, g_ViewProj, Out.PosProjected);

    TexTransformApply(In.UVSet0, UVSet0_TexTransform, Out.UVSet0);
    TexTransformApply(In.UVSet1, UVSet1_TexTransform, Out.UVSet1);
    TexTransformApply(In.UVSet2, UVSet2_TexTransform, Out.UVSet2);
    
    float3 WorldNrm;
    TransformNormal(In.Normal, g_World, WorldNrm);
    WorldNrm = normalize(WorldNrm);

    float3 AmbientAccumOut;
    float3 DiffuseAccumOut;
    float3 SpecularAccumOut;
    Light(WorldPos, WorldNrm, int(1), bool(false), 
        float(1.0), float3(0.0, 0.0, 0.0), g_PointWorldPosition0, 
        g_PointAmbient0, g_PointDiffuse0, float3(0.0, 0.0, 0.0), g_PointAttenuation0, 
        float3(-1.0, -1.0, 0.0), float3(1.0, 0.0, 0.0), 
        float4(1.0, 1.0, 1.0, 1.0), g_AmbientLight, float3(0.0, 0.0, 0.0), 
        float3(0.0, 0.0, 0.0), AmbientAccumOut, 
        DiffuseAccumOut, SpecularAccumOut);

    Light(WorldPos, WorldNrm, int(0), bool(false), 
        float(1.0), float3(0.0, 0.0, 0.0), float4(0.0, 0.0, 0.0, 0.0), g_DirAmbient0, 
        g_DirDiffuse0, float3(0.0, 0.0, 0.0), float3(0.0, 1.0, 0.0), 
        float3(-1.0, -1.0, 0.0), g_DirWorldDirection0, 
        float4(1.0, 1.0, 1.0, 1.0), AmbientAccumOut, 
        DiffuseAccumOut, float3(0.0, 0.0, 0.0), 
        AmbientAccumOut, DiffuseAccumOut, 
        SpecularAccumOut);

    float3 Diffuse;
    float3 Specular;
    ComputeShadingCoefficients(g_MaterialEmissive, Color, 
        In.VertexColors, float3(1.0, 1.0, 1.0), float3(0.0, 0.0, 0.0), 
        DiffuseAccumOut, AmbientAccumOut, bool(false), 
        Diffuse, Specular);
    
    CompositeFinalRGBAColor(Diffuse, Opacity, Out.DiffuseAccum);
    return Out;
}
