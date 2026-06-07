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
inline const char* NiD3D10Pass::GetVertexShaderProgramFileName() const
{
    return m_pcVertexShaderProgramFile;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetVertexShaderProgramFileName(
    const char* pcProgramFile)
{
    NiFree(m_pcVertexShaderProgramFile);
    m_pcVertexShaderProgramFile = 0;

    if (pcProgramFile && pcProgramFile[0] != '\0')
    {
        size_t stLen = strlen(pcProgramFile) + 1;
        m_pcVertexShaderProgramFile = NiAlloc(char, stLen);
        NIASSERT(m_pcVertexShaderProgramFile);

        NiStrcpy(m_pcVertexShaderProgramFile, stLen, pcProgramFile);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3D10Pass::GetVertexShaderProgramEntryPoint() const
{
    return m_pcVertexShaderEntryPoint;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetVertexShaderProgramEntryPoint(
    const char* pcEntryPoint)
{
    NiFree(m_pcVertexShaderEntryPoint);
    m_pcVertexShaderEntryPoint = 0;

    if (pcEntryPoint && pcEntryPoint[0] != '\0')
    {
        size_t stLen = strlen(pcEntryPoint) + 1;
        m_pcVertexShaderEntryPoint = NiAlloc(char, stLen);
        NIASSERT(m_pcVertexShaderEntryPoint);

        NiStrcpy(m_pcVertexShaderEntryPoint, stLen, pcEntryPoint);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3D10Pass::GetVertexShaderProgramShaderTarget() const
{
    return m_pcVertexShaderTarget;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetVertexShaderProgramShaderTarget(
    const char* pcShaderTarget)
{
    NiFree(m_pcVertexShaderTarget);
    m_pcVertexShaderTarget = 0;

    if (pcShaderTarget && pcShaderTarget[0] != '\0')
    {
        size_t stLen = strlen(pcShaderTarget) + 1;
        m_pcVertexShaderTarget = NiAlloc(char, stLen);
        NIASSERT(m_pcVertexShaderTarget);

        NiStrcpy(m_pcVertexShaderTarget, stLen, pcShaderTarget);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3D10Pass::GetGeometryShaderProgramFileName() const
{
    return m_pcGeometryShaderProgramFile;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetGeometryShaderProgramFileName(
    const char* pcProgramFile)
{
    NiFree(m_pcGeometryShaderProgramFile);
    m_pcGeometryShaderProgramFile = 0;

    if (pcProgramFile && pcProgramFile[0] != '\0')
    {
        size_t stLen = strlen(pcProgramFile) + 1;
        m_pcGeometryShaderProgramFile = NiAlloc(char, stLen);
        NIASSERT(m_pcGeometryShaderProgramFile);

        NiStrcpy(m_pcGeometryShaderProgramFile, stLen, pcProgramFile);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3D10Pass::GetGeometryShaderProgramEntryPoint() const
{
    return m_pcGeometryShaderEntryPoint;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetGeometryShaderProgramEntryPoint(
    const char* pcEntryPoint)
{
    NiFree(m_pcGeometryShaderEntryPoint);
    m_pcGeometryShaderEntryPoint = 0;

    if (pcEntryPoint && pcEntryPoint[0] != '\0')
    {
        size_t stLen = strlen(pcEntryPoint) + 1;
        m_pcGeometryShaderEntryPoint = NiAlloc(char, stLen);
        NIASSERT(m_pcGeometryShaderEntryPoint);

        NiStrcpy(m_pcGeometryShaderEntryPoint, stLen, pcEntryPoint);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3D10Pass::GetGeometryShaderProgramShaderTarget() const
{
    return m_pcGeometryShaderTarget;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetGeometryShaderProgramShaderTarget(
    const char* pcShaderTarget)
{
    NiFree(m_pcGeometryShaderTarget);
    m_pcGeometryShaderTarget = 0;

    if (pcShaderTarget && pcShaderTarget[0] != '\0')
    {
        size_t stLen = strlen(pcShaderTarget) + 1;
        m_pcGeometryShaderTarget = NiAlloc(char, stLen);
        NIASSERT(m_pcGeometryShaderTarget);

        NiStrcpy(m_pcGeometryShaderTarget, stLen, pcShaderTarget);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3D10Pass::GetPixelShaderProgramFileName() const
{
    return m_pcPixelShaderProgramFile;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetPixelShaderProgramFileName(
    const char* pcProgramFile)
{
    NiFree(m_pcPixelShaderProgramFile);
    m_pcPixelShaderProgramFile = 0;

    if (pcProgramFile && pcProgramFile[0] != '\0')
    {
        size_t stLen = strlen(pcProgramFile) + 1;
        m_pcPixelShaderProgramFile = NiAlloc(char, stLen);
        NIASSERT(m_pcPixelShaderProgramFile);

        NiStrcpy(m_pcPixelShaderProgramFile, stLen, pcProgramFile);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3D10Pass::GetPixelShaderProgramEntryPoint() const
{
    return m_pcPixelShaderEntryPoint;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetPixelShaderProgramEntryPoint(
    const char* pcEntryPoint)
{
    NiFree(m_pcPixelShaderEntryPoint);
    m_pcPixelShaderEntryPoint = 0;

    if (pcEntryPoint && pcEntryPoint[0] != '\0')
    {
        size_t stLen = strlen(pcEntryPoint) + 1;
        m_pcPixelShaderEntryPoint = NiAlloc(char, stLen);
        NIASSERT(m_pcPixelShaderEntryPoint);

        NiStrcpy(m_pcPixelShaderEntryPoint, stLen, pcEntryPoint);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3D10Pass::GetPixelShaderProgramShaderTarget() const
{
    return m_pcPixelShaderTarget;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetPixelShaderProgramShaderTarget(
    const char* pcShaderTarget)
{
    NiFree(m_pcPixelShaderTarget);
    m_pcPixelShaderTarget = 0;

    if (pcShaderTarget && pcShaderTarget[0] != '\0')
    {
        size_t stLen = strlen(pcShaderTarget) + 1;
        m_pcPixelShaderTarget = NiAlloc(char, stLen);
        NIASSERT(m_pcPixelShaderTarget);

        NiStrcpy(m_pcPixelShaderTarget, stLen, pcShaderTarget);
    }
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetStreamOutSettings(
    const NiStreamOutSettings& kStreamOutSettings)
{
    m_kStreamOutSettings = kStreamOutSettings;
}
//---------------------------------------------------------------------------
inline NiStreamOutSettings& NiD3D10Pass::GetStreamOutSettings()
{
    return m_kStreamOutSettings;
}                        

//---------------------------------------------------------------------------
inline const char* NiD3D10Pass::GetName() const
{
    return m_acName;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetName(char* pcName)
{
    NiStrcpy(m_acName, MAX_NAME_LENGTH, pcName);
}
//---------------------------------------------------------------------------
inline NiD3D10RenderStateGroup* NiD3D10Pass::GetRenderStateGroup() const
{
    return m_spRenderStateGroup;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetRenderStateGroup(
    NiD3D10RenderStateGroup* pkRenderStateGroup)
{
    m_spRenderStateGroup = pkRenderStateGroup;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10Pass::GetVertexConstantMapCount() const
{
    return m_kVertexShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline NiD3D10ShaderConstantMap* NiD3D10Pass::GetVertexConstantMap(
    unsigned int uiIndex) const
{
    if (uiIndex < m_kVertexShaderConstantMaps.GetSize())
        return m_kVertexShaderConstantMaps.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline void NiD3D10Pass::SetVertexConstantMap(unsigned int uiIndex, 
    NiD3D10ShaderConstantMap* pkSCMVertex)
{
    m_kVertexShaderConstantMaps.SetAtGrow(uiIndex, pkSCMVertex);

    m_bConstantMapsLinked = false;
}
//---------------------------------------------------------------------------
inline NiD3D10VertexShader* NiD3D10Pass::GetVertexShader() const
{
    return m_spVertexShader;
}

//---------------------------------------------------------------------------
