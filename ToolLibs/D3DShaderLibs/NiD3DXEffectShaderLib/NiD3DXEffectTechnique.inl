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
// NiD3DXEffectTechnique inline functions
//---------------------------------------------------------------------------
inline const char* NiD3DXEffectTechnique::GetShaderName() const
{
    return m_pkReqDesc->GetName();
}
//---------------------------------------------------------------------------
inline const char* NiD3DXEffectTechnique::GetDescription() const
{
    return m_pkReqDesc->GetDescription();
}
//---------------------------------------------------------------------------
inline const char* NiD3DXEffectTechnique::GetTechniqueName() const
{
    if (m_pcTechniqueName)
        return m_pcTechniqueName;
    else
        return m_pkReqDesc->GetName();
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DXEffectTechnique::GetImplementation() const
{
    return m_uiImplementation;
}
//---------------------------------------------------------------------------
inline D3DXTechniquePtr NiD3DXEffectTechnique::GetTechniquePtr() const
{
    return m_pkTechnique;
}
//---------------------------------------------------------------------------
inline NiD3DXEffectFile* NiD3DXEffectTechnique::GetFile() const
{
    return m_pkFile;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectTechnique::SetFile(NiD3DXEffectFile* pkFile)
{
    m_pkFile = pkFile;
}
//---------------------------------------------------------------------------
inline bool NiD3DXEffectTechnique::GetTimeParameterPresent() const
{
    return m_bTimeParameter;
}
//---------------------------------------------------------------------------
inline const NiSemanticAdapterTable&
    NiD3DXEffectTechnique::GetSemanticAdapterTable() const
{
    return m_kAdapterTable;
}
//---------------------------------------------------------------------------
inline NiSemanticAdapterTable& NiD3DXEffectTechnique::GetSemanticAdapterTable()
{
    return m_kAdapterTable;
}
//---------------------------------------------------------------------------
