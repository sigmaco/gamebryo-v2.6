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
// NiD3DVertexShader inline functions
//---------------------------------------------------------------------------
inline NiD3DVertexShader::NiD3DVertexShader(NiD3DRenderer* pkRenderer) :
    NiD3DShaderProgram(pkRenderer, NiGPUProgram::PROGRAM_VERTEX), 
    m_uiUsage(0),
    m_hShader(0),
    m_hDecl(0)
{
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DVertexShader::GetUsage() const
{
    return m_uiUsage;
}
//---------------------------------------------------------------------------
inline void NiD3DVertexShader::SetUsage(unsigned int uiUsage)
{
    m_uiUsage = uiUsage;
}
//---------------------------------------------------------------------------
inline NiD3DVertexShaderHandle NiD3DVertexShader::GetShaderHandle() const
{
    return m_hShader;
}
//---------------------------------------------------------------------------
inline void NiD3DVertexShader::SetShaderHandle(
    NiD3DVertexShaderHandle hShader)
{
    m_hShader = hShader;
}
//---------------------------------------------------------------------------
inline NiD3DVertexDeclaration NiD3DVertexShader::GetVertexDeclaration() const
{
    return m_hDecl;
}
//---------------------------------------------------------------------------
inline void NiD3DVertexShader::SetVertexDeclaration(
    NiD3DVertexDeclaration hDecl)
{
    m_hDecl = hDecl;
}
//---------------------------------------------------------------------------
