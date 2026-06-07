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
inline NiFixedString NiD3D10EffectTechnique::GetShaderName() const
{
    return NiFixedString(m_pkReqDesc->GetName());
}
//---------------------------------------------------------------------------
inline NiFixedString NiD3D10EffectTechnique::GetDescription() const
{
    return NiFixedString(m_pkReqDesc->GetDescription());
}
//---------------------------------------------------------------------------
inline NiFixedString NiD3D10EffectTechnique::GetTechniqueName() const
{
    if (m_kTechniqueName)
        return m_kTechniqueName;
    else
        return NiFixedString(m_pkReqDesc->GetName());
}
//---------------------------------------------------------------------------
inline NiUInt32 NiD3D10EffectTechnique::GetImplementation() const
{
    return m_uiImplementation;
}
//---------------------------------------------------------------------------
inline NiD3D10EffectFile* NiD3D10EffectTechnique::GetFile() const
{
    return m_pkFile;
}
//---------------------------------------------------------------------------
inline void NiD3D10EffectTechnique::SetFile(NiD3D10EffectFile* pkFile)
{
    m_pkFile = pkFile;
}
//---------------------------------------------------------------------------
inline bool NiD3D10EffectTechnique::GetTimeParameterPresent() const
{
    return m_bTimeParameter;
}
//---------------------------------------------------------------------------
inline bool NiD3D10EffectTechnique::HasParametersOfType
    (NiD3D10EffectParameterDesc::ParameterType kType)
{
    return m_bHasParameterType[kType];  
}
