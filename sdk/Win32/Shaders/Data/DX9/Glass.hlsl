// This file comes from the DolphinVS sample from ATi's Rendermonkey sample.
// The algorithm and the majority of the file are ATi's work.

//**************************************************************//
//  Effect File exported by RenderMonkey
//
//  - For this version, exporting to FX file is not perfect. 
//    There are many informations doesn't get exported. 
//    Currently, exporter does not try to resolve naming conflicts
//    ( functions, variables etc )
//    Please fix these issues before using this Effect file
//**************************************************************//

float4x4 view_proj_matrix : WorldViewProj;
float4x4 inv_world_view : InvWorldView;

texture RainbowMap
<
    string filename = "C:/Program Files/ATI Technologies/RenderMonkey/v1.0/Examples/Media/Textures/Rainbow.tga";
>;

texture EnvMap 
<
    string  filename = "C:/Program Files/ATI Technologies/RenderMonkey/v1.0/Examples/Media/Textures/Snow.dds";
>;

//--------------------------------------------------------------//
// Complex Glass
//--------------------------------------------------------------//
float refractionScale = 1.000000;
float reflectionScale = 1.000000;
float indexOfRefractionRatio = 1.140000;
float ambient = 0.200000;
float rainbowSpread = 0.180000;
float rainbowScale = 0.200000;
float4 baseColor = { 0.722019, 0.704711, 0.760274, 1.000000 };

//--------------------------------------------------------------//
// Object
//--------------------------------------------------------------//
sampler Environment : register(s0) = sampler_state
{
   Texture = (EnvMap);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};

sampler Rainbow : register(s1) = sampler_state
{
   Texture = (RainbowMap);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};

struct VS_OUTPUT {
   float4 Pos:     POSITION;
   float3 normal:  TEXCOORD0;
   float3 viewVec: TEXCOORD1;
};

VS_OUTPUT ComplexGlassVS(float4 Pos: POSITION, float3 normal: NORMAL){
   VS_OUTPUT Out;

    Out.Pos = mul(Pos, view_proj_matrix);
        
    // put normal into "DX" model space
    Out.normal = normal.xzy;
    
    // put view vec into "DX" model space
    float3 kWSCamera = mul(float4(0.0f, 0.0f, 0.0f, 1.0f), inv_world_view);
    float3 viewVec = kWSCamera - Pos;
        
    Out.viewVec = viewVec.xzy;
    
    return Out;
}

float4 ComplexGlassPS(float3 normal: TEXCOORD0, float3 viewVec: TEXCOORD1) : COLOR {
   normal = normalize(normal);
   viewVec = normalize(viewVec);

   // Look up the reflection
   float3 reflVec = reflect(-viewVec, normal);
   float4 reflection = texCUBE(Environment, reflVec.xyz);

   // We'll use Snell's refraction law:
   // n  * sin(theta ) = n  * sin(theta )
   //  i            i     r            r

   // sin(theta )
   //          i
   float cosine = dot(viewVec, normal);
   float sine = sqrt(1 - cosine * cosine);

   // sin(theta )
   //          r
   float sine2 = saturate(indexOfRefractionRatio * sine);
   float cosine2 = sqrt(1 - sine2 * sine2);

   // Out of the sine and cosine of the angle between the
   // refraction vector and the normal we can construct the
   // refraction vector itself given two base vectors.
   // These two base vectors are the negative normal and
   // a tangent vector along the path of the incoming vector
   // relative to the surface.
   float3 x = -normal;
   float3 y = normalize(cross(cross(viewVec, normal), normal));

   // Refraction
   float3 refrVec = x * cosine2 + y * sine2;
   float4 refraction = texCUBE(Environment, refrVec.xyz);

   // Colors refract differently and the difference is more
   // visible the stronger the refraction. We'll fake this
   // effect by adding some rainbowish colors accordingly.
   float4 rainbow = tex1D(Rainbow, pow(cosine, rainbowSpread));

   float4 rain = rainbowScale * rainbow * baseColor;
   float4 refl = reflectionScale * reflection;
   float4 refr = refractionScale * refraction * baseColor;

   // There is more light reflected at sharp angles and less
   // light refracted. There is more color separation of refracted
   // light at sharper angles
   return sine * refl + (1 - sine2) * refr + sine2 * rain + ambient;
}
