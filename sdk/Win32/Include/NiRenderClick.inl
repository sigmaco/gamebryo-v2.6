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

#include "NiRenderer.h"

//---------------------------------------------------------------------------
inline void NiRenderClick::_SDMInit()
{
    ms_kDefaultName = "Render Click";
}
//---------------------------------------------------------------------------
inline void NiRenderClick::_SDMShutdown()
{
    ms_kDefaultName = NULL;
}
//---------------------------------------------------------------------------
inline NiRenderClick::NiRenderClick() : m_kViewport(0.0f, 1.0f, 1.0f, 0.0f),
    m_pfnPreProcessingCallback(NULL),
    m_pfnPostProcessingCallback(NULL),
    m_pvPreProcessingCallbackData(NULL),
    m_pvPostProcessingCallbackData(NULL),
    m_uiClearMode(NiRenderer::CLEAR_NONE),
    m_uiSingleFrameClearMode(NiRenderer::CLEAR_NONE),
    m_kBackgroudColor(0.0f, 0.0f, 0.0f, 0.0f),
    m_kName(ms_kDefaultName)
{
    SetActive(true);
    SetUseRendererBackgroundColor(true);
    SetPersistBackgroundColorToRenderer(false);
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenderClick::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline bool NiRenderClick::ShouldPerformRendering(unsigned int uiFrameID)
{
    if (!GetActive())
    {
        return false;
    }

    if (m_spValidator)
    {
        return m_spValidator->ValidateClick(this, uiFrameID);
    }

    return true;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetValidator(NiRenderClickValidator* pkValidator)
{
    m_spValidator = pkValidator;
}
//---------------------------------------------------------------------------
inline NiRenderClickValidator* NiRenderClick::GetValidator() const
{
    return m_spValidator;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetRenderTargetGroup(
    NiRenderTargetGroup* pkRenderTargetGroup)
{
    m_spRenderTargetGroup = pkRenderTargetGroup;
}
//---------------------------------------------------------------------------
inline NiRenderTargetGroup* NiRenderClick::GetRenderTargetGroup() const
{
    return m_spRenderTargetGroup;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetClearAllBuffers(bool bClearAllBuffers)
{
    if (bClearAllBuffers)
    {
        m_uiClearMode = NiRenderer::CLEAR_ALL;
    }
    else
    {
        m_uiClearMode = NiRenderer::CLEAR_NONE;
    }
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetClearColorBuffers(bool bClearBuffers)
{
    if (bClearBuffers)
    {
        m_uiClearMode |= NiRenderer::CLEAR_BACKBUFFER;
    }
    else
    {
        m_uiClearMode &= ~NiRenderer::CLEAR_BACKBUFFER;
    }
}
//---------------------------------------------------------------------------
inline bool NiRenderClick::GetClearColorBuffers() const
{
    return ((m_uiClearMode & NiRenderer::CLEAR_BACKBUFFER) != 0);
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetClearDepthBuffer(bool bClearBuffer)
{
    if (bClearBuffer)
    {
        m_uiClearMode |= NiRenderer::CLEAR_ZBUFFER;
    }
    else
    {
        m_uiClearMode &= ~NiRenderer::CLEAR_ZBUFFER;
    }
}
//---------------------------------------------------------------------------
inline bool NiRenderClick::GetClearDepthBuffer() const
{
    return ((m_uiClearMode & NiRenderer::CLEAR_ZBUFFER) != 0);
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetClearStencilBuffer(bool bClearBuffer)
{
    if (bClearBuffer)
    {
        m_uiClearMode |= NiRenderer::CLEAR_STENCIL;
    }
    else
    {
        m_uiClearMode &= ~NiRenderer::CLEAR_STENCIL;
    }
}
//---------------------------------------------------------------------------
inline bool NiRenderClick::GetClearStencilBuffer() const
{
    return ((m_uiClearMode & NiRenderer::CLEAR_STENCIL) != 0);
}
//---------------------------------------------------------------------------
inline void NiRenderClick::RequestClearAllBuffersOnce()
{
    m_uiSingleFrameClearMode |= NiRenderer::CLEAR_ALL;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::RequestClearColorBuffersOnce()
{
    m_uiSingleFrameClearMode |= NiRenderer::CLEAR_BACKBUFFER;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::RequestClearDepthBufferOnce()
{
    m_uiSingleFrameClearMode |= NiRenderer::CLEAR_ZBUFFER;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::RequestClearStencilBufferOnce()
{
    m_uiSingleFrameClearMode |= NiRenderer::CLEAR_STENCIL;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetViewport(const NiRect<float>& kViewport)
{
    m_kViewport = kViewport;
}
//---------------------------------------------------------------------------
inline const NiRect<float>& NiRenderClick::GetViewport() const
{
    return m_kViewport;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetActive(bool bActive)
{
    SetBit(bActive, ACTIVE);
}
//---------------------------------------------------------------------------
inline bool NiRenderClick::GetActive() const
{
    return GetBit(ACTIVE);
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetPreProcessingCallbackFunc(Callback pfnCallback,
    void* pvCallbackData)
{
    m_pfnPreProcessingCallback = pfnCallback;
    m_pvPreProcessingCallbackData = pvCallbackData;
}
//---------------------------------------------------------------------------
inline NiRenderClick::Callback NiRenderClick::GetPreProcessingCallbackFunc()
    const
{
    return m_pfnPreProcessingCallback;
}
//---------------------------------------------------------------------------
inline void* NiRenderClick::GetPreProcessingCallbackFuncData() const
{
    return m_pvPreProcessingCallbackData;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetPostProcessingCallbackFunc(Callback pfnCallback,
    void* pvCallbackData)
{
    m_pfnPostProcessingCallback = pfnCallback;
    m_pvPostProcessingCallbackData = pvCallbackData;
}
//---------------------------------------------------------------------------
inline NiRenderClick::Callback NiRenderClick::GetPostProcessingCallbackFunc()
    const
{
    return m_pfnPostProcessingCallback;
}
//---------------------------------------------------------------------------
inline void* NiRenderClick::GetPostProcessingCallbackFuncData() const
{
    return m_pvPostProcessingCallbackData;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetBackgroundColor(const NiColorA& kColor)
{
    m_kBackgroudColor = kColor;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::GetBackgroundColor(NiColorA& kColor)
{
    kColor = m_kBackgroudColor;
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetUseRendererBackgroundColor(bool bUseRendererBG)
{
    SetBit(bUseRendererBG, USE_RENDERER_BG_COLOR);
}
//---------------------------------------------------------------------------
inline bool NiRenderClick::GetUseRendererBackgroundColor()
{
    return GetBit(USE_RENDERER_BG_COLOR);
}
//---------------------------------------------------------------------------
inline void NiRenderClick::SetPersistBackgroundColorToRenderer(bool bPersist)
{
    SetBit(bPersist, PERSIST_BG_COLOR);
}
//---------------------------------------------------------------------------
inline bool NiRenderClick::GetPersistBackgroundColorToRenderer()
{
    return GetBit(PERSIST_BG_COLOR);
}
//---------------------------------------------------------------------------
