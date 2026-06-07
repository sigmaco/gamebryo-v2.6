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

#include <NiRenderer.h>

//---------------------------------------------------------------------------
inline NiSceneRenderView::NiSceneRenderView(NiCamera* pkCamera,
    NiCullingProcess* pkCullingProcess, NiEntityErrorInterface* pkErrorHandler,
    bool bAlwaysUseCameraViewport) :
    m_spCamera(pkCamera), m_spCullingProcess(pkCullingProcess),
    m_spErrorHandler(pkErrorHandler),
    m_bAlwaysUseCameraViewport(bAlwaysUseCameraViewport)
{
    NIASSERT(m_spCamera);
    NIASSERT(m_spCullingProcess);
    NIASSERT(m_spErrorHandler);

    m_spRenderingContext = NiNew NiEntityRenderingContext;
    m_spRenderingContext->m_pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(m_spRenderingContext->m_pkRenderer);
}
//---------------------------------------------------------------------------
inline void NiSceneRenderView::SetCamera(NiCamera* pkCamera)
{
    NIASSERT(pkCamera);
    m_spCamera = pkCamera;
}
//---------------------------------------------------------------------------
inline NiCamera* NiSceneRenderView::GetCamera() const
{
    return m_spCamera;
}
//---------------------------------------------------------------------------
inline void NiSceneRenderView::SetCullingProcess(
    NiCullingProcess* pkCullingProcess)
{
    NIASSERT(pkCullingProcess);
    m_spCullingProcess = pkCullingProcess;
}
//---------------------------------------------------------------------------
inline NiCullingProcess* NiSceneRenderView::GetCullingProcess() const
{
    return m_spCullingProcess;
}
//---------------------------------------------------------------------------
inline void NiSceneRenderView::SetErrorHandler(
    NiEntityErrorInterface* pkErrorHandler)
{
    NIASSERT(pkErrorHandler);
    m_spErrorHandler = pkErrorHandler;
}
//---------------------------------------------------------------------------
inline NiEntityErrorInterface* NiSceneRenderView::GetErrorHandler() const
{
    return m_spErrorHandler;
}
//---------------------------------------------------------------------------
inline void NiSceneRenderView::SetAlwaysUseCameraViewport(
    bool bAlwaysUseCameraViewport)
{
    m_bAlwaysUseCameraViewport = bAlwaysUseCameraViewport;
}
//---------------------------------------------------------------------------
inline bool NiSceneRenderView::GetAlwaysUseCameraViewport() const
{
    return m_bAlwaysUseCameraViewport;
}
//---------------------------------------------------------------------------
inline void NiSceneRenderView::AppendScene(NiScene* pkScene)
{
    NIASSERT(pkScene);
    m_kScenes.AddTail(pkScene);
}
//---------------------------------------------------------------------------
inline void NiSceneRenderView::PrependScene(NiScene* pkScene)
{
    NIASSERT(pkScene);
    m_kScenes.AddHead(pkScene);
}
//---------------------------------------------------------------------------
inline void NiSceneRenderView::RemoveScene(NiScene* pkScene)
{
    NIASSERT(pkScene);
    m_kScenes.Remove(pkScene);
}
//---------------------------------------------------------------------------
inline void NiSceneRenderView::RemoveAllScenes()
{
    m_kScenes.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiTPointerList<NiScenePtr>& NiSceneRenderView::GetScenes()
{
    return m_kScenes;
}
//---------------------------------------------------------------------------
inline const NiTPointerList<NiScenePtr>& NiSceneRenderView::GetScenes() const
{
    return m_kScenes;
}
//---------------------------------------------------------------------------
