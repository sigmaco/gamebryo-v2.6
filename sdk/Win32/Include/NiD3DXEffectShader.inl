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
// NiD3DXEffectShader inline functions
//---------------------------------------------------------------------------
inline void NiD3DXEffectShader::SetEffectTechnique(
    NiD3DXEffectTechnique* pkD3DXEffectTechnique)
{
    m_pkD3DXEffectTechnique = pkD3DXEffectTechnique;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectShader::SetTechniquePtr(D3DXTechniquePtr pkTechnique)
{
    m_pkTechnique = pkTechnique;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DXEffectShader::GetBoneMatrixRegisters() const
{
    return m_uiBoneMatrixRegisters;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectShader::SetBoneMatrixRegisters(
    unsigned int uiRegisters)
{
    m_uiBoneMatrixRegisters = uiRegisters;
}
//---------------------------------------------------------------------------
inline NiD3DXEffectShader::BoneMatrixCalcMethod 
    NiD3DXEffectShader::GetBoneCalcMethod() const
{
    return m_eBoneCalcMethod;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectShader::SetBoneCalcMethod(
    NiD3DXEffectShader::BoneMatrixCalcMethod eMethod)
{
    m_eBoneCalcMethod = eMethod;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectShader::SetSoftwareVP(bool bSoftwareVP)
{
    m_bSoftwareVP = bSoftwareVP;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectShader::SetActualImplementation(
    unsigned int uiActualImplementation)
{
    m_uiActualImplementation = uiActualImplementation;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DXEffectShader::GetActualImplementation() const
{
    return m_uiActualImplementation;
}
//---------------------------------------------------------------------------

