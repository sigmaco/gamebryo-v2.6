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
inline const NiFixedString& NiD3D10ShaderProgram::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderProgram::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline ID3D10Blob* NiD3D10ShaderProgram::GetShaderByteCode() const
{
    return m_pkShaderByteCode;
}

//---------------------------------------------------------------------------
inline NiD3D10ShaderProgramCreator* NiD3D10ShaderProgram::GetCreator() const
{
    return m_pkCreator;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderProgram::SetCreator(NiD3D10ShaderProgramCreator* pkCreator)
{
    m_pkCreator = pkCreator;
}


//---------------------------------------------------------------------------
inline ID3D10ShaderReflection* NiD3D10ShaderProgram::GetShaderReflection() const
{
    return m_pkReflection;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderProgram::ClearShaderReflection()
{
    if (m_pkReflection)
        m_pkReflection->Release();
    m_pkReflection = NULL;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10ShaderProgram::GetConstantBufferCount() const
{
    return m_uiConstantBufferCount;
}
