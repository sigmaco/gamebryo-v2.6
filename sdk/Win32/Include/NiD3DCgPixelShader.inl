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
inline NiD3DCgPixelShader::NiD3DCgPixelShader(NiD3DRenderer* pkRenderer) :
    NiD3DPixelShader(pkRenderer), 
    m_pcEntryPoint(NULL),
    m_pcShaderTarget(NULL),
    m_kCgProgram(NULL)
{ /* */ }
//---------------------------------------------------------------------------
inline const char* NiD3DCgPixelShader::GetEntryPoint() const
{
    return m_pcEntryPoint;
}
//---------------------------------------------------------------------------
inline void NiD3DCgPixelShader::SetEntryPoint(const char* pcEntryPoint)
{
    if (m_pcEntryPoint == pcEntryPoint)
        return;

    NiFree(m_pcEntryPoint);
    if (pcEntryPoint && pcEntryPoint[0] != '\0')
    {
        unsigned int uiLen = strlen(pcEntryPoint) + 1;
        char* pcNewEntryPoint = NiAlloc(char, uiLen);
        NIASSERT(pcNewEntryPoint);
        NiStrcpy(pcNewEntryPoint, uiLen, pcEntryPoint);
        m_pcEntryPoint = pcNewEntryPoint;
    }
    else
    {
        m_pcEntryPoint = NULL;
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3DCgPixelShader::GetShaderTarget() const
{
    return m_pcShaderTarget;
}
//---------------------------------------------------------------------------
inline void NiD3DCgPixelShader::SetShaderTarget(const char* pcShaderTarget)
{
    if (m_pcShaderTarget == pcShaderTarget)
        return;

    NiFree(m_pcShaderTarget);
    if (pcShaderTarget && pcShaderTarget[0] != '\0')
    {
        unsigned int uiLen = strlen(pcShaderTarget) + 1;
        char* pcNewShaderTarget = NiAlloc(char, uiLen);
        NIASSERT(pcNewShaderTarget);
        NiStrcpy(pcNewShaderTarget, uiLen, pcShaderTarget);
        m_pcShaderTarget = pcNewShaderTarget;
    }
    else
    {
        pcShaderTarget = NULL;
    }
}
//---------------------------------------------------------------------------
inline CGprogram NiD3DCgPixelShader::GetCgProgram() const
{
    return m_kCgProgram;
}
//---------------------------------------------------------------------------
inline void NiD3DCgPixelShader::SetCgProgram(CGprogram kProgram)
{
    m_kCgProgram = kProgram;
}
//---------------------------------------------------------------------------

