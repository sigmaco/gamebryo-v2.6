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
inline const NiFixedString& NiD3D10EffectParameter::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiD3D10EffectParameter::GetConstantBufferName()
    const
{
    return m_kConstantBufferName;
}
//---------------------------------------------------------------------------
inline void NiD3D10EffectParameter::SetD3D10Parameter(
    ID3D10EffectVariable* pkParam)
{
    // if you change the parameter null the saved data fro mthe old param.
    if(m_pkParam != pkParam) 
        DestroyRendererData();
    m_pkParam = pkParam;
}
//---------------------------------------------------------------------------
inline ID3D10EffectVariable* NiD3D10EffectParameter::GetD3D10Parameter() const
{
    return m_pkParam;
}
//---------------------------------------------------------------------------
inline bool NiD3D10EffectParameter::RequiresTime() const
{
    return (m_ePredefinedMapping == NiShaderConstantMap::SCM_DEF_CONSTS_TIME ||
        m_ePredefinedMapping == NiShaderConstantMap::SCM_DEF_CONSTS_SINTIME ||
        m_ePredefinedMapping == NiShaderConstantMap::SCM_DEF_CONSTS_COSTIME ||
        m_ePredefinedMapping == NiShaderConstantMap::SCM_DEF_CONSTS_TANTIME ||
        m_ePredefinedMapping == 
        NiShaderConstantMap::SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME);
}
//---------------------------------------------------------------------------
inline void NiD3D10EffectParameter::SetUsed(bool bUsed)
{
    m_bUsed = bUsed;
}
//---------------------------------------------------------------------------
inline bool NiD3D10EffectParameter::GetUsed() const
{
    return m_bUsed;
}
//---------------------------------------------------------------------------
inline NiShaderConstantMap::DefinedMappings 
    NiD3D10EffectParameter::GetPredefinedMapping() const
{
    return m_ePredefinedMapping;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10EffectParameter::GetArrayCount() const
{
    return m_uiArrayCount;
}
//---------------------------------------------------------------------------
inline NiD3D10EffectParameterDesc::ParameterType 
    NiD3D10EffectParameter::GetType () const
{
    return m_eParameterType;
}   
//---------------------------------------------------------------------------
