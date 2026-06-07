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
inline Ni3DRenderView::Ni3DRenderView(NiCamera* pkCamera,
    NiCullingProcess* pkCullingProcess, bool bAlwaysUseCameraViewport) :
    m_spCamera(pkCamera), m_spCullingProcess(pkCullingProcess),
    m_bAlwaysUseCameraViewport(bAlwaysUseCameraViewport)
{
}
//---------------------------------------------------------------------------
inline void Ni3DRenderView::SetCamera(NiCamera* pkCamera)
{
    m_spCamera = pkCamera;
}
//---------------------------------------------------------------------------
inline NiCamera* Ni3DRenderView::GetCamera() const
{
    return m_spCamera;
}
//---------------------------------------------------------------------------
inline void Ni3DRenderView::SetCullingProcess(
    NiCullingProcess* pkCullingProcess)
{
    m_spCullingProcess = pkCullingProcess;
}
//---------------------------------------------------------------------------
inline NiCullingProcess* Ni3DRenderView::GetCullingProcess() const
{
    return m_spCullingProcess;
}
//---------------------------------------------------------------------------
inline void Ni3DRenderView::SetAlwaysUseCameraViewport(
    bool bAlwaysUseCameraViewport)
{
    m_bAlwaysUseCameraViewport = bAlwaysUseCameraViewport;
}
//---------------------------------------------------------------------------
inline bool Ni3DRenderView::GetAlwaysUseCameraViewport() const
{
    return m_bAlwaysUseCameraViewport;
}
//---------------------------------------------------------------------------
inline void Ni3DRenderView::AppendScene(NiAVObject* pkScene)
{
    NIASSERT(pkScene);
    m_kScenes.AddTail(pkScene);
}
//---------------------------------------------------------------------------
inline void Ni3DRenderView::PrependScene(NiAVObject* pkScene)
{
    NIASSERT(pkScene);
    m_kScenes.AddHead(pkScene);
}
//---------------------------------------------------------------------------
inline void Ni3DRenderView::RemoveScene(NiAVObject* pkScene)
{
    NIASSERT(pkScene);
    m_kScenes.Remove(pkScene);
}
//---------------------------------------------------------------------------
inline void Ni3DRenderView::RemoveAllScenes()
{
    m_kScenes.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiTPointerList<NiAVObjectPtr>& Ni3DRenderView::GetScenes()
{
    return m_kScenes;
}
//---------------------------------------------------------------------------
inline const NiTPointerList<NiAVObjectPtr>& Ni3DRenderView::GetScenes() const
{
    return m_kScenes;
}
//---------------------------------------------------------------------------
