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
// NiD3DShader inline functions
//---------------------------------------------------------------------------
inline NiD3DShader::NiD3DShader() :
    NiD3DShaderInterface(), 
    m_bUsesNiRenderState(false), 
    m_bUsesNiLightState(false),
    m_bConvertBlendIndicesToD3DColor(false),
    m_pkRenderStateGroup(0), 
    m_uiCurrentPass(0), 
    m_uiPassCount(0), 
    m_kPasses(2, 2)
{
    m_kPasses.RemoveAll();
}
//---------------------------------------------------------------------------
inline bool NiD3DShader::IsInitialized()
{
    return m_bInitialized;
}
//---------------------------------------------------------------------------
inline bool NiD3DShader::GetUsesNiRenderState() const
{
    return m_bUsesNiRenderState;
}
//---------------------------------------------------------------------------
inline bool NiD3DShader::SetUsesNiRenderState(bool bUses)
{
    m_bUsesNiRenderState = bUses;
    return true;
}
//---------------------------------------------------------------------------
inline bool NiD3DShader::GetUsesNiLightState() const
{
    return m_bUsesNiLightState;
}
//---------------------------------------------------------------------------
inline void NiD3DShader::SetUsesNiLightState(bool bUses)
{
    m_bUsesNiLightState = bUses;
}
//---------------------------------------------------------------------------
inline bool NiD3DShader::GetConvertBlendIndicesToD3DColor() const
{
    return m_bConvertBlendIndicesToD3DColor;
}
//---------------------------------------------------------------------------
inline void NiD3DShader::SetConvertBlendIndicesToD3DColor(bool bConvert)
{
    m_bConvertBlendIndicesToD3DColor = bConvert;
}
//---------------------------------------------------------------------------
inline void NiD3DShader::ResetSCMExtraData(NiRenderObject* pkGeometry)
{
    if (pkGeometry)
    {
        NiSCMExtraData* pkShaderData = 
            (NiSCMExtraData*)
            pkGeometry->GetExtraData(NiD3DShader::ms_kNDLShaderMapName);
        if (pkShaderData)
            pkShaderData->Reset();
    }
}
//---------------------------------------------------------------------------
inline void NiD3DShader::PackDynamicEffect(
    const NiTextureEffect* pkTextureEffect, NiD3DTextureStage* pkStage,
    bool bSave, NiD3DRenderer* pkD3DRenderer)
{
    ms_apfnDynEffectPackers[(unsigned int)
        pkTextureEffect->GetTextureCoordGen()](
        pkTextureEffect->GetWorldProjectionMatrix(),
        pkTextureEffect->GetWorldProjectionTranslation(), pkStage, bSave,
        pkD3DRenderer);
}
//---------------------------------------------------------------------------
inline void NiD3DShader::UseWorldSpaceSphereMaps(bool bWorldSpace)
{
    ms_apfnDynEffectPackers[NiTextureEffect::SPHERE_MAP] =
        (bWorldSpace ? &PackWorldSphereEffect : &PackCameraSphereEffect);
}
//---------------------------------------------------------------------------
