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
// NiD3DShaderProgram inline functions
//---------------------------------------------------------------------------
inline const char* NiD3DShaderProgram::GetName() const
{
    return m_pszName;
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgram::SetName(const char* pszName)
{
    if (pszName && strcmp(pszName, ""))
    {
        size_t stNewLen = strlen(pszName) + 1;
        if (m_pszName)
        {
            if (strlen(m_pszName) < stNewLen)
            {
                NiFree(m_pszName);
                m_pszName = 0;
            }
        }

        if (!m_pszName)
            m_pszName = NiAlloc(char, stNewLen);
        
        NIASSERT(m_pszName);
        NiStrcpy(m_pszName, stNewLen, pszName);
    }
    else
    {
        NiFree(m_pszName);
        m_pszName = 0;
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3DShaderProgram::GetShaderProgramName() const
{
    return m_pszShaderProgramName;
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgram::SetShaderProgramName(const char* pszName)
{
    if (pszName && strcmp(pszName, ""))
    {
        size_t stNewLen = strlen(pszName) + 1;
        if (m_pszShaderProgramName)
        {
            if (strlen(m_pszShaderProgramName) < stNewLen)
            {
                NiFree(m_pszShaderProgramName);
                m_pszShaderProgramName = 0;
            }
        }

        if (!m_pszShaderProgramName)
            m_pszShaderProgramName = NiAlloc(char,stNewLen);
        
        NIASSERT(m_pszShaderProgramName);
        NiStrcpy(m_pszShaderProgramName, stNewLen, pszName);
    }
    else
    {
        // This is likely a very bad thing...
        NiFree(m_pszShaderProgramName);
        m_pszShaderProgramName = 0;
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DShaderProgram::GetCodeSize() const
{
    return m_uiCodeSize;
}
//---------------------------------------------------------------------------
inline void* NiD3DShaderProgram::GetCode() const
{
    return m_pvCode;
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgram::SetCode(unsigned int uiSize, void* pvCode)
{
    m_uiCodeSize = uiSize;
    m_pvCode = pvCode;
}
//---------------------------------------------------------------------------
inline NiD3DShaderProgramCreator* NiD3DShaderProgram::GetCreator() const
{
    return m_pkCreator;
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgram::SetCreator(
    NiD3DShaderProgramCreator* pkCreator)
{
    m_pkCreator = pkCreator;
}
//---------------------------------------------------------------------------
