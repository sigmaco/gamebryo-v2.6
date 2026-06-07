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
// NiD3DXEffectParameter inline functions
//---------------------------------------------------------------------------
inline const NiFixedString& NiD3DXEffectParameter::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectParameter::SetParameterPtr(D3DXParameterPtr pkParam)
{
    m_pkParam = pkParam;
}
//---------------------------------------------------------------------------
inline D3DXParameterPtr NiD3DXEffectParameter::GetParameterPtr() const
{
    return m_pkParam;
}
//---------------------------------------------------------------------------
inline bool NiD3DXEffectParameter::RequiresTime() const
{
    return (m_ePredefinedMapping == NiShaderConstantMap::SCM_DEF_CONSTS_TIME ||
        m_ePredefinedMapping == NiShaderConstantMap::SCM_DEF_CONSTS_SINTIME ||
        m_ePredefinedMapping == NiShaderConstantMap::SCM_DEF_CONSTS_COSTIME ||
        m_ePredefinedMapping == NiShaderConstantMap::SCM_DEF_CONSTS_TANTIME ||
        m_ePredefinedMapping == 
        NiShaderConstantMap::SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME);
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectParameter::SetUsed(bool bUsed)
{
    m_bUsed = bUsed;
}
//---------------------------------------------------------------------------
inline bool NiD3DXEffectParameter::GetUsed() const
{
    return m_bUsed;
}
//---------------------------------------------------------------------------
inline NiD3DShaderConstantMap::DefinedMappings 
    NiD3DXEffectParameter::GetPredefinedMapping() const
{
    return m_ePredefinedMapping;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DXEffectParameter::GetArrayCount() const
{
    return m_uiArrayCount;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectParameter::ClearTextureReferences(
    LPD3DXBASEEFFECT pkEffect)
{
    if (m_eParameterType == NiD3DXEffectParameterDesc::PT_TEXTURE)
        pkEffect->SetTexture(m_pkParam, NULL);
}
//---------------------------------------------------------------------------
