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

//---------------------------------------------------------------------------
inline bool NiD3D10Shader::GetUsesNiRenderState() const
{
    return m_bUsesNiRenderState;
}
//---------------------------------------------------------------------------
inline bool NiD3D10Shader::SetUsesNiRenderState(bool bUses)
{
    m_bUsesNiRenderState = bUses;
    return true;
}
//---------------------------------------------------------------------------
inline void NiD3D10Shader::GetBoneParameters(bool& bTransposeBones, 
    unsigned int& uiBoneMatrixRegisters, bool& bWorldSpaceBones)
{
    bTransposeBones = m_bTransposeBones;
    uiBoneMatrixRegisters = m_uiBoneMatrixRegisters;
    bWorldSpaceBones = m_bWorldSpaceBones;
}
//---------------------------------------------------------------------------
inline void NiD3D10Shader::SetBoneParameters(bool bTransposeBones, 
    unsigned int uiBoneMatrixRegisters, bool bWorldSpaceBones)
{
    m_bTransposeBones = bTransposeBones;
    m_uiBoneMatrixRegisters = uiBoneMatrixRegisters;
    m_bWorldSpaceBones = bWorldSpaceBones;
}
