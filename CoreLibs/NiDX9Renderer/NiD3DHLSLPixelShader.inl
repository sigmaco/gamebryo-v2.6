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
inline NiD3DHLSLPixelShader::NiD3DHLSLPixelShader(NiD3DRenderer* pkRenderer) :
    NiD3DPixelShader(pkRenderer), 
    m_pcEntryPoint(NULL),
    m_pcShaderTarget(NULL),
    m_pkConstantTable(NULL)
{ /* */ }
//---------------------------------------------------------------------------
inline const char* NiD3DHLSLPixelShader::GetEntryPoint() const
{
    return m_pcEntryPoint;
}
//---------------------------------------------------------------------------
inline void NiD3DHLSLPixelShader::SetEntryPoint(const char* pcEntryPoint)
{
    if (m_pcEntryPoint == pcEntryPoint)
        return;

    NiFree(m_pcEntryPoint);
    if (pcEntryPoint && pcEntryPoint[0] != '\0')
    {
        size_t stLen = strlen(pcEntryPoint) + 1;
        char* pcNewEntryPoint = NiAlloc(char, stLen);
        NIASSERT(pcNewEntryPoint);
        NiStrcpy(pcNewEntryPoint, stLen, pcEntryPoint);
        m_pcEntryPoint = pcNewEntryPoint;
    }
    else
    {
        m_pcEntryPoint = NULL;
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3DHLSLPixelShader::GetShaderTarget() const
{
    return m_pcShaderTarget;
}
//---------------------------------------------------------------------------
inline void NiD3DHLSLPixelShader::SetShaderTarget(const char* pcShaderTarget)
{
    if (m_pcShaderTarget == pcShaderTarget)
        return;

    NiFree(m_pcShaderTarget);
    if (pcShaderTarget && pcShaderTarget[0] != '\0')
    {
        size_t stLen = strlen(pcShaderTarget) + 1;
        char* pcNewShaderTarget = NiAlloc(char, stLen);
        NIASSERT(pcNewShaderTarget);
        NiStrcpy(pcNewShaderTarget, stLen, pcShaderTarget);
        m_pcShaderTarget = pcNewShaderTarget;
    }
    else
    {
        pcShaderTarget = NULL;
    }
}
//---------------------------------------------------------------------------
inline LPD3DXCONSTANTTABLE NiD3DHLSLPixelShader::GetConstantTable() const
{
    return m_pkConstantTable;
}
//---------------------------------------------------------------------------
inline void NiD3DHLSLPixelShader::SetConstantTable(
    LPD3DXCONSTANTTABLE pkTable)
{
    if (m_pkConstantTable == pkTable)
        return;

    if (m_pkConstantTable)
        m_pkConstantTable->Release();

    m_pkConstantTable = pkTable;

    if (m_pkConstantTable)
        m_pkConstantTable->AddRef();
}
//---------------------------------------------------------------------------

