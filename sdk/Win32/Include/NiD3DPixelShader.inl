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
// NiD3DPixelShader inline functions
//---------------------------------------------------------------------------
inline NiD3DPixelShader::NiD3DPixelShader(NiD3DRenderer* pkRenderer) :
    NiD3DShaderProgram(pkRenderer, NiGPUProgram::PROGRAM_PIXEL), 
    m_hShader(0) 
{
}
//---------------------------------------------------------------------------
inline NiD3DPixelShaderHandle NiD3DPixelShader::GetShaderHandle()
{
    return m_hShader;
}
//---------------------------------------------------------------------------
inline void NiD3DPixelShader::SetShaderHandle(NiD3DPixelShaderHandle hShader)
{
    m_hShader = hShader;
}
//---------------------------------------------------------------------------
