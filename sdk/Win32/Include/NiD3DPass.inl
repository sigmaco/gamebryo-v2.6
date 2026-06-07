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
// NiD3DPass inline functions
//---------------------------------------------------------------------------
inline const char* NiD3DPass::GetName() const
{
    return m_szName;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetName(char* pcName)
{
    if (pcName)
    {
        NiStrncpy(m_szName, NID3DPASS_NAME_LEN, pcName, 
            NID3DPASS_NAME_LEN - 1);
    }
    else
    {
        m_szName[0] = '\0';
    }
}
//---------------------------------------------------------------------------
inline NiD3DRenderStateGroup* NiD3DPass::GetRenderStateGroup() const
{
    return m_pkRenderStateGroup;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetRenderStateGroup(
    NiD3DRenderStateGroup* pkRenderStateGroup)
{
    if (m_pkRenderStateGroup != pkRenderStateGroup)
        NiD3DRenderStateGroup::ReleaseRenderStateGroup(m_pkRenderStateGroup);

    m_pkRenderStateGroup = pkRenderStateGroup;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetRenderState(unsigned int uiRenderState, 
    unsigned int uiValue, bool bSave)
{
    if (m_pkRenderStateGroup == NULL)
    {
        m_pkRenderStateGroup = 
            NiD3DRenderStateGroup::GetFreeRenderStateGroup();
    }
    m_pkRenderStateGroup->SetRenderState(uiRenderState, uiValue, bSave);
}
//---------------------------------------------------------------------------
inline bool NiD3DPass::RemoveRenderState(unsigned int uiRenderState)
{
    bool bReturn = false;
    if (m_pkRenderStateGroup != NULL)
    {
        m_pkRenderStateGroup->RemoveRenderState(uiRenderState);
        bReturn = true;
    }
    return bReturn;
}
//---------------------------------------------------------------------------
inline bool NiD3DPass::GetRenderState(unsigned int uiRenderState, 
    unsigned int& uiValue, bool& bSave) const
{
    bool bReturn = false;
    if (m_pkRenderStateGroup != NULL)
    {
        m_pkRenderStateGroup->GetRenderState(uiRenderState, uiValue, bSave);
        bReturn = true;
    }
    return bReturn;
}
//---------------------------------------------------------------------------
inline NiD3DShaderConstantMap* NiD3DPass::GetPixelConstantMap() const
{
    return m_spPixelConstantMap;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetPixelConstantMap(NiD3DShaderConstantMap* pkSCMPixel)
{
    m_spPixelConstantMap = pkSCMPixel;
}
//---------------------------------------------------------------------------
inline const char* NiD3DPass::GetPixelShaderProgramFileName() const
{
    return m_pcPixelShaderProgramFile;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetPixelShaderProgramFileName(
    const char* pcProgramFile)
{
    NiFree(m_pcPixelShaderProgramFile);
    m_pcPixelShaderProgramFile = 0;

    if (pcProgramFile && strcmp(pcProgramFile, ""))
    {
        size_t stLen = strlen(pcProgramFile) + 1;
        m_pcPixelShaderProgramFile = NiAlloc(char, stLen);
        NIASSERT(m_pcPixelShaderProgramFile);

        NiStrcpy(m_pcPixelShaderProgramFile, stLen, pcProgramFile);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3DPass::GetPixelShaderProgramEntryPoint() const
{
    return m_pcPixelShaderEntryPoint;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetPixelShaderProgramEntryPoint(
    const char* pcEntryPoint)
{
    NiFree(m_pcPixelShaderEntryPoint);
    m_pcPixelShaderEntryPoint = 0;

    if (pcEntryPoint && strcmp(pcEntryPoint, ""))
    {
        size_t stLen = strlen(pcEntryPoint) + 1;
        m_pcPixelShaderEntryPoint = NiAlloc(char, stLen);
        NIASSERT(m_pcPixelShaderEntryPoint);

        NiStrcpy(m_pcPixelShaderEntryPoint, stLen, pcEntryPoint);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3DPass::GetPixelShaderProgramShaderTarget() const
{
    return m_pcPixelShaderTarget;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetPixelShaderProgramShaderTarget(
    const char* pcShaderTarget)
{
    NiFree(m_pcPixelShaderTarget);
    m_pcPixelShaderTarget = 0;

    if (pcShaderTarget && strcmp(pcShaderTarget, ""))
    {
        size_t stLen = strlen(pcShaderTarget) + 1;
        m_pcPixelShaderTarget = NiAlloc(char, stLen);
        NIASSERT(m_pcPixelShaderTarget);

        NiStrcpy(m_pcPixelShaderTarget, stLen, pcShaderTarget);
    }
}
//---------------------------------------------------------------------------
inline NiD3DPixelShader* NiD3DPass::GetPixelShader() const
{
    return m_spPixelShader;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetPixelShader(NiD3DPixelShader* pkPixelShader)
{
    m_spPixelShader = pkPixelShader;
}
//---------------------------------------------------------------------------
inline NiD3DShaderConstantMap* NiD3DPass::GetVertexConstantMap() const
{
    return m_spVertexConstantMap;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetVertexConstantMap(
    NiD3DShaderConstantMap* pkSCMVertex)
{
    m_spVertexConstantMap = pkSCMVertex;
}
//---------------------------------------------------------------------------
inline const char* NiD3DPass::GetVertexShaderProgramFileName() const
{
    return m_pcVertexShaderProgramFile;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetVertexShaderProgramFileName(
    const char* pcProgramFile)
{
    NiFree(m_pcVertexShaderProgramFile);
    m_pcVertexShaderProgramFile = 0;

    if (pcProgramFile && strcmp(pcProgramFile, ""))
    {
        size_t stLen = strlen(pcProgramFile) + 1;
        m_pcVertexShaderProgramFile = NiAlloc(char, stLen);
        NIASSERT(m_pcVertexShaderProgramFile);

        NiStrcpy(m_pcVertexShaderProgramFile, stLen, pcProgramFile);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3DPass::GetVertexShaderProgramEntryPoint() const
{
    return m_pcVertexShaderEntryPoint;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetVertexShaderProgramEntryPoint(
    const char* pcEntryPoint)
{
    NiFree(m_pcVertexShaderEntryPoint);
    m_pcVertexShaderEntryPoint = 0;

    if (pcEntryPoint && strcmp(pcEntryPoint, ""))
    {
        size_t stLen = strlen(pcEntryPoint) + 1;
        m_pcVertexShaderEntryPoint = NiAlloc(char, stLen);
        NIASSERT(m_pcVertexShaderEntryPoint);

        NiStrcpy(m_pcVertexShaderEntryPoint, stLen, pcEntryPoint);
    }
}
//---------------------------------------------------------------------------
inline const char* NiD3DPass::GetVertexShaderProgramShaderTarget() const
{
    return m_pcVertexShaderTarget;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetVertexShaderProgramShaderTarget(
    const char* pcShaderTarget)
{
    NiFree(m_pcVertexShaderTarget);
    m_pcVertexShaderTarget = 0;

    if (pcShaderTarget && strcmp(pcShaderTarget, ""))
    {
        size_t stLen = strlen(pcShaderTarget) + 1;
        m_pcVertexShaderTarget = NiAlloc(char, stLen);
        NIASSERT(m_pcVertexShaderTarget);

        NiStrcpy(m_pcVertexShaderTarget, stLen, pcShaderTarget);
    }
}
//---------------------------------------------------------------------------
inline NiD3DVertexShader* NiD3DPass::GetVertexShader() const
{
    return m_spVertexShader;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::SetVertexShader(NiD3DVertexShader* pkVertexShader)
{
    m_spVertexShader = pkVertexShader;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DPass::GetCurrentStage() const
{
    return m_uiCurrentStage;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DPass::GetStageCount() const
{
    return m_uiStageCount;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DPass::GetTextureCount() const
{
    return m_uiTexturesPerPass;
}
//---------------------------------------------------------------------------
inline NiD3DTextureStage* NiD3DPass::GetStage(unsigned int uiStageNum) const
{
    return (uiStageNum < m_kStages.GetSize() ? m_kStages.GetAt(uiStageNum) :
        NULL);
}
//---------------------------------------------------------------------------
inline NiD3DError NiD3DPass::AppendStage(NiD3DTextureStage* pkStage)
{
    return SetStage(m_uiCurrentStage, pkStage);
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DPass::GetAvailableStages() const
{
    return ms_uiMaxTextureBlendStages - m_uiCurrentStage;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DPass::GetAvailableTextures() const
{
    return ms_uiMaxSimultaneousTextures - m_uiTexturesPerPass;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DPass::GetTotalAvailableStages() const
{
    return ms_uiMaxTextureBlendStages - m_uiStageCount;
}
//---------------------------------------------------------------------------
inline bool NiD3DPass::IsPassContiguous() const
{
    return (m_uiStageCount == m_uiCurrentStage);
}
//---------------------------------------------------------------------------
inline bool NiD3DPass::IsPassEmpty() const
{
    return (m_uiStageCount == 0);
}
//---------------------------------------------------------------------------
