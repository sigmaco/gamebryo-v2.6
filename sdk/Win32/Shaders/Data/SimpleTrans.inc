float4x4 WorldViewProjection    : WORLDVIEWPROJECTION;

inline float4 WorldTrans(float3 Pos)
{
    return mul(float4(Pos, 1), WorldViewProjection);
}
