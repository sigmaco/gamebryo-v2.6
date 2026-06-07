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
inline NiDX9RenderState::NiRenderStateSetting::NiRenderStateSetting() : 
    m_uiCurrValue(0x7FFFFFFF), 
    m_uiPrevValue(0x7FFFFFFF)
{
    //* *//
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::NiRenderStateSetting::SetValue(
    unsigned int uiValue, bool bSave)
{
    if (bSave)
        m_uiPrevValue = m_uiCurrValue;
    m_uiCurrValue = uiValue;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::NiRenderStateSetting::InitValue(
    unsigned int uiValue)
{
    m_uiCurrValue = uiValue;
    m_uiPrevValue = uiValue;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::NiRenderStateSetting::SaveValue()
{
    m_uiPrevValue = m_uiCurrValue;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::NiRenderStateSetting::RestoreValue()
{
    m_uiCurrValue = m_uiPrevValue;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::NiRenderStateSetting::GetValue() const
{
    return m_uiCurrValue;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::NiRenderStateSetting::GetPrevValue() 
const
{
    return m_uiPrevValue;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::DisableAlpha()
{
    ApplyAlpha(m_spDisabledAlphaProperty);
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetSrcAlphaCaps()
{
    return m_uiSrcAlphaCaps;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetDestAlphaCaps()
{
    return m_uiDestAlphaCaps;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetD3DBlendMode(
    NiAlphaProperty::AlphaFunction eBlend)
{
    return m_auiAlphaMapping[eBlend];
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetD3DTestMode(
    NiAlphaProperty::TestFunction eTest)
{
    return m_auiAlphaTestMapping[eTest];
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetBoneCount(unsigned short usNumBones)
{
    SetRenderState(D3DRS_VERTEXBLEND, m_aD3DVertexBlendMappings[usNumBones]);
}
//---------------------------------------------------------------------------
inline float NiDX9RenderState::GetMaxFogValue() const
{
    return m_fMaxFogValue;
}
//---------------------------------------------------------------------------
inline float NiDX9RenderState::GetMaxFogFactor() const
{
    return m_fMaxFogFactor;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetMaxFogValue(float fFogVal)
{
    if (fFogVal <= 0.0f)
        fFogVal = 1e-5f;

    m_fMaxFogValue = fFogVal;
    m_fMaxFogFactor = 1.0f / fFogVal - 1.0f;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::GetCameraNearAndFar(float& fNear, float& fFar)
{
    fNear = m_fCameraNear;
    fFar = m_fCameraFar;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetCameraNearAndFar(float fNear, float fFar)
{
    m_fCameraNear = fNear;
    m_fCameraFar = fFar;
    m_fCameraDepthRange = fFar - fNear;
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetLeftHanded()
{
    return m_uiLeftHanded ? true : false;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetLeftHanded(bool bLeft)
{
    m_uiLeftHanded = bLeft ? 1 : 0;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetPixelShader(NiD3DPixelShaderHandle hShader, 
    bool bSave)
{
    if (bSave)
        m_hPreviousPixelShader = m_hCurrentPixelShader;
    if (m_hCurrentPixelShader != hShader)
    {
        m_hCurrentPixelShader = hShader;
        m_pkD3DDevice->SetPixelShader(m_hCurrentPixelShader);
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(PIXEL_SHADER_CHANGES, 1);
    }
}
//---------------------------------------------------------------------------
inline NiD3DPixelShaderHandle NiDX9RenderState::GetPixelShader() const
{
    return m_hCurrentPixelShader;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestorePixelShader()
{
    SetPixelShader(m_hPreviousPixelShader);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ClearPixelShader(
    NiD3DPixelShaderHandle hShader)
{
    if (m_hCurrentPixelShader == hShader)
    {
        // We need to clear this!
        m_hCurrentPixelShader = 0;
        m_pkD3DDevice->SetPixelShader(0);
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(PIXEL_SHADER_CHANGES, 1);
    }

    if (m_hPreviousPixelShader == hShader)
    {
        // This is likely a bad thing to have happening
        // If someone calls restore on this, they will have lost what
        // was happening
        m_hPreviousPixelShader = 0;
    }
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetVertexShader(
    NiD3DVertexShaderHandle hShader, bool bSave)
{
    if (bSave)
        m_hPreviousVertexShader = m_hCurrentVertexShader;
    if (m_hCurrentVertexShader != hShader)
    {
        m_hCurrentVertexShader = hShader;
        m_pkD3DDevice->SetVertexShader(m_hCurrentVertexShader);
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(VERTEX_SHADER_CHANGES, 1);
    }
}
//---------------------------------------------------------------------------
inline NiD3DVertexShaderHandle NiDX9RenderState::GetVertexShader() const
{
    return m_hCurrentVertexShader;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreVertexShader()
{
    SetVertexShader(m_hPreviousVertexShader);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ClearVertexShader(
    NiD3DVertexShaderHandle hShader)
{
    if (m_hCurrentVertexShader == hShader)
    {
        // We need to clear this!
        m_hCurrentVertexShader = 0;
        m_pkD3DDevice->SetVertexShader(0);
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(VERTEX_SHADER_CHANGES, 1);
    }

    if (m_hPreviousVertexShader == hShader)
    {
        // This is likely a bad thing to have happening
        // If someone calls restore on this, they will have lost what
        // if happening
        m_hPreviousVertexShader = 0;
    }
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetFVF(unsigned int uiFVF, bool bSave)
{
    if (uiFVF != 0 && (m_bDeclaration || m_uiCurrentFVF != uiFVF))
    {
        m_bDeclaration = false;
        if (bSave)
            m_uiPreviousFVF = m_uiCurrentFVF;
        m_uiCurrentFVF = uiFVF;
        m_pkD3DDevice->SetFVF(m_uiCurrentFVF);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetFVF() const
{
    if (m_bDeclaration)
        return 0;
    else
        return m_uiCurrentFVF;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreFVF()
{
    SetFVF(m_uiPreviousFVF);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ClearFVF(unsigned int uiFVF)
{
    if (uiFVF == 0 || m_bDeclaration)
        return;

    if (m_uiCurrentFVF == uiFVF)
    {
        // We need to clear this!
        m_uiCurrentFVF = D3DFVF_XYZ;
        m_pkD3DDevice->SetFVF(D3DFVF_XYZ);
    }

    if (m_uiPreviousFVF == uiFVF)
    {
        // This is likely a bad thing to have happening
        // If someone calls restore on this, they will have lost what
        // was happening
        m_uiPreviousFVF = D3DFVF_XYZ;
    }
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetDeclaration(
    NiD3DVertexDeclaration hDeclaration, bool bSave)
{
    if (hDeclaration != NULL && 
        (!m_bDeclaration || m_hCurrentVertexDeclaration != hDeclaration))
    {
        m_bDeclaration = true;
        if (bSave)
            m_hPreviousVertexDeclaration = m_hCurrentVertexDeclaration;
        m_hCurrentVertexDeclaration = hDeclaration;
        m_pkD3DDevice->SetVertexDeclaration(m_hCurrentVertexDeclaration);
    }
}
//---------------------------------------------------------------------------
inline NiD3DVertexDeclaration NiDX9RenderState::GetDeclaration() const
{
    if (m_bDeclaration)
        return m_hCurrentVertexDeclaration;
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreDeclaration()
{
    SetDeclaration(m_hPreviousVertexDeclaration);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ClearDeclaration(
    NiD3DVertexDeclaration hDeclaration)
{
    if (hDeclaration == NULL || !m_bDeclaration)
        return;

    if (m_hCurrentVertexDeclaration == hDeclaration)
    {
        // We need to clear this!
        m_hCurrentVertexDeclaration = 0;
        m_pkD3DDevice->SetVertexDeclaration(0);
    }

    if (m_hPreviousVertexDeclaration == hDeclaration)
    {
        // This is likely a bad thing to have happening
        // If someone calls restore on this, they will have lost what
        // if happening
        m_hPreviousVertexDeclaration = 0;
    }
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetRenderState(D3DRENDERSTATETYPE eState, 
    unsigned int uiValue, bool bSave)
{
    if (m_akRenderStateSettings[eState].GetValue() != uiValue)
    {
        m_pkD3DDevice->SetRenderState(eState, uiValue);
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(RENDER_STATE_CHANGES, 1);
    }
    m_akRenderStateSettings[eState].SetValue(uiValue, bSave);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetRenderState(D3DRENDERSTATETYPE eState, 
    unsigned int uiValue)
{
    if (m_akRenderStateSettings[eState].GetValue() != uiValue)
    {
        m_pkD3DDevice->SetRenderState(eState, uiValue);
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(RENDER_STATE_CHANGES, 1);
        m_akRenderStateSettings[eState].SetValue(uiValue, false);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetRenderState(
    D3DRENDERSTATETYPE eState) const
{
    return m_akRenderStateSettings[eState].GetValue();
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreRenderState(D3DRENDERSTATETYPE eState)
{
    SetRenderState(eState, m_akRenderStateSettings[eState].GetPrevValue());
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetTextureStageState(unsigned int uiStage, 
    D3DTEXTURESTAGESTATETYPE eState, unsigned int uiValue, bool bSave)
{
    if (m_akTextureStageStateSettings[uiStage][eState].GetValue() != uiValue)
        m_pkD3DDevice->SetTextureStageState(uiStage, eState, uiValue);

    m_akTextureStageStateSettings[uiStage][eState].SetValue(uiValue, 
        bSave);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetTextureStageState(unsigned int uiStage, 
    D3DTEXTURESTAGESTATETYPE eState, unsigned int uiValue)
{
    if (m_akTextureStageStateSettings[uiStage][eState].GetValue() != uiValue)
    {
        m_pkD3DDevice->SetTextureStageState(uiStage, eState, uiValue);
        m_akTextureStageStateSettings[uiStage][eState].SetValue(uiValue, 
            false);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetTextureStageState(
    unsigned int uiStage, D3DTEXTURESTAGESTATETYPE eState) const
{
    return m_akTextureStageStateSettings[uiStage][eState].GetValue();
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreTextureStageState(unsigned int uiStage, 
    D3DTEXTURESTAGESTATETYPE eState)
{
    SetTextureStageState(uiStage, eState, 
        m_akTextureStageStateSettings[uiStage][eState].GetPrevValue());
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetSamplerState(unsigned int uiStage, 
    NiD3DSamplerState eState, unsigned int uiValue, bool bSave)
{
    D3DSAMPLERSTATETYPE eD3DState = 
        (D3DSAMPLERSTATETYPE)ms_auiSamplesStateMappings[eState];
    SetSamplerState(uiStage, eD3DState, uiValue, bSave);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetSamplerState(unsigned int uiStage, 
    NiD3DSamplerState eState, unsigned int uiValue)
{
    D3DSAMPLERSTATETYPE eD3DState = 
        (D3DSAMPLERSTATETYPE)ms_auiSamplesStateMappings[eState];
    SetSamplerState(uiStage, eD3DState, uiValue);
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetSamplerState(unsigned int uiStage, 
    NiD3DSamplerState eState)
{
    D3DSAMPLERSTATETYPE eD3DState = 
        (D3DSAMPLERSTATETYPE)ms_auiSamplesStateMappings[eState];
    return GetSamplerState(uiStage, eD3DState);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreSamplerState(unsigned int uiStage, 
    NiD3DSamplerState eState)
{
    D3DSAMPLERSTATETYPE eD3DState = 
        (D3DSAMPLERSTATETYPE)ms_auiSamplesStateMappings[eState];
    RestoreSamplerState(uiStage, eD3DState);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetSamplerState(unsigned int uiStage, 
    D3DSAMPLERSTATETYPE eState, unsigned int uiValue, bool bSave)
{
    if (m_akSamplerStateSettings[uiStage][eState].GetValue() != uiValue)
    {
        m_pkD3DDevice->SetSamplerState(uiStage, eState, uiValue);
    }
    m_akSamplerStateSettings[uiStage][eState].SetValue(uiValue, bSave);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetSamplerState(unsigned int uiStage, 
    D3DSAMPLERSTATETYPE eState, unsigned int uiValue)
{
    if (m_akSamplerStateSettings[uiStage][eState].GetValue() != uiValue)
    {
        m_pkD3DDevice->SetSamplerState(uiStage, eState, uiValue);
        m_akSamplerStateSettings[uiStage][eState].SetValue(uiValue, false);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetSamplerState(unsigned int uiStage, 
    D3DSAMPLERSTATETYPE eState)
{
    return m_akSamplerStateSettings[uiStage][eState].GetValue();
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreSamplerState(unsigned int uiStage, 
    D3DSAMPLERSTATETYPE eState)
{
    SetSamplerState(uiStage, eState, 
        m_akSamplerStateSettings[uiStage][eState].GetPrevValue());
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::InitTextures()
{
    for (unsigned int ui = 0; ui < NI_MAX_TEXTURE_STAGES; ui++)
        m_apkTextureStageTextures[ui] = 0;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetTexture(unsigned int uiStage, 
    D3DBaseTexturePtr pkTexture)
{
    NIASSERT(uiStage < NI_MAX_TEXTURE_STAGES);
    if (m_apkTextureStageTextures[uiStage] != pkTexture)
    {
        m_apkTextureStageTextures[uiStage] = pkTexture;
        m_pkD3DDevice->SetTexture(uiStage, pkTexture);
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(TEXTURE_CHANGES, 1);
    }
}
//---------------------------------------------------------------------------
inline D3DBaseTexturePtr NiDX9RenderState::GetTexture(unsigned int uiStage)
{
    NIASSERT(uiStage < NI_MAX_TEXTURE_STAGES);
    return m_apkTextureStageTextures[uiStage];
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ClearTexture(D3DBaseTexturePtr pkTexture)
{
    if (!pkTexture)
        return;
    for (unsigned int uiStage = 0; uiStage < NI_MAX_TEXTURE_STAGES; 
        uiStage++)
    {
        if (m_apkTextureStageTextures[uiStage] == pkTexture)
        {
            m_apkTextureStageTextures[uiStage] = 0;
            m_pkD3DDevice->SetTexture(uiStage, 0);
        }
    }
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::SetVertexShaderConstantB(
    unsigned int uiStartRegister, const BOOL* pbConstantData, 
    unsigned int uiBoolCount, bool bSave)
{
    return m_spShaderConstantManager->SetVertexShaderConstantB(
        uiStartRegister, pbConstantData, uiBoolCount, bSave);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetVertexShaderConstantB(
    unsigned int uiStartRegister, BOOL* pbConstantData, 
    unsigned int uiBoolCount)
{
    return m_spShaderConstantManager->GetVertexShaderConstantB(
        uiStartRegister, pbConstantData, uiBoolCount);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::RestoreVertexShaderConstantB(
    unsigned int uiStartRegister, unsigned int uiBoolCount)
{
    return m_spShaderConstantManager->RestoreVertexShaderConstantB(
        uiStartRegister, uiBoolCount);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::SetVertexShaderConstantF(
    unsigned int uiStartRegister, const float* pfConstantData, 
    unsigned int uiFloat4Count, bool bSave)
{
    return m_spShaderConstantManager->SetVertexShaderConstantF(
        uiStartRegister, pfConstantData, uiFloat4Count, bSave);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetVertexShaderConstantF(
    unsigned int uiStartRegister, float* pfConstantData, 
    unsigned int uiFloat4Count)
{
    return m_spShaderConstantManager->GetVertexShaderConstantF(
        uiStartRegister, pfConstantData, uiFloat4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::RestoreVertexShaderConstantF(
    unsigned int uiStartRegister, unsigned int uiFloat4Count)
{
    return m_spShaderConstantManager->RestoreVertexShaderConstantF(
        uiStartRegister, uiFloat4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::SetVertexShaderConstantI(
    unsigned int uiStartRegister, const int* piConstantData, 
    unsigned int uiInt4Count, bool bSave)
{
    return m_spShaderConstantManager->SetVertexShaderConstantI(
        uiStartRegister, piConstantData, uiInt4Count, bSave);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetVertexShaderConstantI(
    unsigned int uiStartRegister, int* piConstantData, 
    unsigned int uiInt4Count)
{
    return m_spShaderConstantManager->GetVertexShaderConstantI(
        uiStartRegister, piConstantData, uiInt4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::RestoreVertexShaderConstantI(
    unsigned int uiStartRegister, unsigned int uiInt4Count)
{
    return m_spShaderConstantManager->RestoreVertexShaderConstantI(
        uiStartRegister, uiInt4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::SetVertexShaderConstant(
    unsigned int uiStartRegister, const float* pfConstantData, 
    unsigned int uiFloat4Count, bool bSave)
{
    return m_spShaderConstantManager->SetVertexShaderConstantF(
        uiStartRegister, pfConstantData, uiFloat4Count, bSave);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetVertexShaderConstant(
    unsigned int uiStartRegister, float* pfConstantData, 
    unsigned int uiFloat4Count)
{
    return m_spShaderConstantManager->GetVertexShaderConstantF(
        uiStartRegister, pfConstantData, uiFloat4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::RestoreVertexShaderConstant(
    unsigned int uiStartRegister, unsigned int uiFloat4Count)
{
    return m_spShaderConstantManager->RestoreVertexShaderConstantF(
        uiStartRegister, uiFloat4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::SetPixelShaderConstantB(
    unsigned int uiStartRegister, const BOOL* pbConstantData, 
    unsigned int uiBoolCount, bool bSave)
{
    return m_spShaderConstantManager->SetPixelShaderConstantB(
        uiStartRegister, pbConstantData, uiBoolCount, bSave);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetPixelShaderConstantB(
    unsigned int uiStartRegister, BOOL* pbConstantData, 
    unsigned int uiBoolCount)
{
    return m_spShaderConstantManager->GetPixelShaderConstantB(
        uiStartRegister, pbConstantData, uiBoolCount);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::RestorePixelShaderConstantB(
    unsigned int uiStartRegister, unsigned int uiBoolCount)
{
    return m_spShaderConstantManager->RestorePixelShaderConstantB(
        uiStartRegister, uiBoolCount);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::SetPixelShaderConstantF(
    unsigned int uiStartRegister, const float* pfConstantData, 
    unsigned int uiFloat4Count, bool bSave)
{
    return m_spShaderConstantManager->SetPixelShaderConstantF(
        uiStartRegister, pfConstantData, uiFloat4Count, bSave);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetPixelShaderConstantF(
    unsigned int uiStartRegister, float* pfConstantData, 
    unsigned int uiFloat4Count)
{
    return m_spShaderConstantManager->GetPixelShaderConstantF(
        uiStartRegister, pfConstantData, uiFloat4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::RestorePixelShaderConstantF(
    unsigned int uiStartRegister, unsigned int uiFloat4Count)
{
    return m_spShaderConstantManager->RestorePixelShaderConstantF(
        uiStartRegister, uiFloat4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::SetPixelShaderConstantI(
    unsigned int uiStartRegister, const int* piConstantData, 
    unsigned int uiInt4Count, bool bSave)
{
    return m_spShaderConstantManager->SetPixelShaderConstantI(
        uiStartRegister, piConstantData, uiInt4Count, bSave);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetPixelShaderConstantI(
    unsigned int uiStartRegister, int* piConstantData, 
    unsigned int uiInt4Count)
{
    return m_spShaderConstantManager->GetPixelShaderConstantI(
        uiStartRegister, piConstantData, uiInt4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::RestorePixelShaderConstantI(
    unsigned int uiStartRegister, unsigned int uiInt4Count)
{
    return m_spShaderConstantManager->RestorePixelShaderConstantI(
        uiStartRegister, uiInt4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::SetPixelShaderConstant(
    unsigned int uiStartRegister, const float* pfConstantData, 
    unsigned int uiFloat4Count, bool bSave)
{
    return m_spShaderConstantManager->SetPixelShaderConstantF(
        uiStartRegister, pfConstantData, uiFloat4Count, bSave);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetPixelShaderConstant(
    unsigned int uiStartRegister, float* pfConstantData, 
    unsigned int uiFloat4Count)
{
    return m_spShaderConstantManager->GetPixelShaderConstantF(
        uiStartRegister, pfConstantData, uiFloat4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::RestorePixelShaderConstant(
    unsigned int uiStartRegister, unsigned int uiFloat4Count)
{
    return m_spShaderConstantManager->RestorePixelShaderConstantF(
        uiStartRegister, uiFloat4Count);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::FastOverwriteVertexShaderConstantB(
    unsigned int uiStartRegister, const BOOL* pbConstantData, 
    unsigned int uiBoolCount)
{
    m_spShaderConstantManager->FastOverwriteVertexShaderConstantB(
        uiStartRegister, pbConstantData, uiBoolCount);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::MarkVertexShaderConstantsDirtyB(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    m_spShaderConstantManager->MarkVertexShaderConstantsDirtyB(
        uiFirstDirtyRegister, uiFirstCleanRegister);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::FastOverwriteVertexShaderConstantF(
    unsigned int uiStartRegister, const float* pfConstantData,
    unsigned uiFloat4Count)
{
    m_spShaderConstantManager->FastOverwriteVertexShaderConstantF(
        uiStartRegister, pfConstantData, uiFloat4Count);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::MarkVertexShaderConstantsDirtyF(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    m_spShaderConstantManager->MarkVertexShaderConstantsDirtyF(
        uiFirstDirtyRegister, uiFirstCleanRegister);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::FastOverwriteVertexShaderConstantI(
    unsigned int uiStartRegister, const int* piConstantData, 
    unsigned int uiInt4Count)
{
    m_spShaderConstantManager->FastOverwriteVertexShaderConstantI(
        uiStartRegister, piConstantData, uiInt4Count);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::MarkVertexShaderConstantsDirtyI(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    m_spShaderConstantManager->MarkVertexShaderConstantsDirtyI(
        uiFirstDirtyRegister, uiFirstCleanRegister);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::FastOverwritePixelShaderConstantB(
    unsigned int uiStartRegister, const BOOL* pbConstantData, 
    unsigned int uiBoolCount)
{
    m_spShaderConstantManager->FastOverwritePixelShaderConstantB(
        uiStartRegister, pbConstantData, uiBoolCount);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::MarkPixelShaderConstantsDirtyB(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    m_spShaderConstantManager->MarkPixelShaderConstantsDirtyB(
        uiFirstDirtyRegister, uiFirstCleanRegister);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::FastOverwritePixelShaderConstantF(
    unsigned int uiStartRegister, const float* pfConstantData,
    unsigned uiFloat4Count)
{
    m_spShaderConstantManager->FastOverwritePixelShaderConstantF(
        uiStartRegister, pfConstantData, uiFloat4Count);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::MarkPixelShaderConstantsDirtyF(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    m_spShaderConstantManager->MarkPixelShaderConstantsDirtyF(
        uiFirstDirtyRegister, uiFirstCleanRegister);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::FastOverwritePixelShaderConstantI(
    unsigned int uiStartRegister, const int* piConstantData, 
    unsigned int uiInt4Count)
{
    m_spShaderConstantManager->FastOverwritePixelShaderConstantI(
        uiStartRegister, piConstantData, uiInt4Count);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::MarkPixelShaderConstantsDirtyI(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    m_spShaderConstantManager->MarkPixelShaderConstantsDirtyI(
        uiFirstDirtyRegister, uiFirstCleanRegister);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::CommitShaderConstants()
{
    m_spShaderConstantManager->CommitChanges();
}
//---------------------------------------------------------------------------
inline const unsigned int NiDX9RenderState::GetFlags()
{
    return m_uiFlags;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ClearFlags()
{
    m_uiFlags = 0x0;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ClearFlag(unsigned int uiFlag)
{
    m_uiFlags &= ~uiFlag;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetFlag(unsigned int uiFlag)
{
    m_uiFlags |= uiFlag;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ToggleFlag(unsigned int uiFlag)
{
    m_uiFlags ^= uiFlag;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::CopyFlag(unsigned int uiFlag, bool bSet)
{
    if (bSet)
        m_uiFlags |= uiFlag;
    else
        m_uiFlags &= ~uiFlag;
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::IsFlag(unsigned int uiFlag)
{
    return (uiFlag & m_uiFlags) ? true : false;
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::IsNotFlag(unsigned int uiFlag)
{
    return (uiFlag & m_uiFlags) ? false : true;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetForceNormalizeNormals(bool bSet)
{
    m_bForceNormalizeNormals = bSet;
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetForceNormalizeNormals() const
{
    return m_bForceNormalizeNormals;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetInternalNormalizeNormals(bool bSet)
{
    m_bInternalNormalizeNormals = bSet;
}
//---------------------------------------------------------------------------
inline bool NiDX9RenderState::GetInternalNormalizeNormals() const
{
    return m_bInternalNormalizeNormals;
}
//---------------------------------------------------------------------------

