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
inline bool NiDX9RendererDesc::IsD3D10RendererDesc()
{
    return false;
}

//---------------------------------------------------------------------------
// NVPerfHUD adapter functions
//---------------------------------------------------------------------------
inline bool NiDX9RendererDesc::GetNVPerfHUDSupport()
{
    return m_bSupportsNVPerfHUD;
}

//---------------------------------------------------------------------------
inline bool NiDX9RendererDesc::GetDefaultNVPerfHUDSetting()
{
    return m_bSupportsNVPerfHUD && GetSettings()->m_bNVPerfHUD;
}

//---------------------------------------------------------------------------
// Device functions
//---------------------------------------------------------------------------
inline bool NiDX9RendererDesc::IsDeviceAvailable(bool bRefDevice)
{
    const NiDX9DeviceDesc* pkDev = m_pkAdapterDesc->GetDevice(
        bRefDevice ? D3DDEVTYPE_REF : D3DDEVTYPE_HAL);

    return pkDev != NULL;
}

//---------------------------------------------------------------------------
inline bool NiDX9RendererDesc::GetDefaultDevice()
{
    SelectDevice(GetSettings()->m_bRefRast);
    return GetSettings()->m_bRefRast;
}

//---------------------------------------------------------------------------
inline bool NiDX9RendererDesc::IsPureDeviceAvailable()
{
    // Disable pure checkbox when NVPerfHUD selected
    if (GetSettings()->m_bNVPerfHUD)
        return false;

    // Pure can be selected only with HAL device
    if (GetSettings()->m_bRefRast)
        return false;
    else
        return true;
}

//---------------------------------------------------------------------------
inline bool NiDX9RendererDesc::GetDefaultPureDevice()
{
    if (!IsPureDeviceAvailable())
        return false;

    return GetSettings()->m_bPureDevice;
}

//---------------------------------------------------------------------------
inline void NiDX9RendererDesc::SelectPureDevice(bool bPure)
{
    if (!IsPureDeviceAvailable())
        bPure = false;

    GetSettings()->m_bPureDevice = bPure;
}

//---------------------------------------------------------------------------
// Vertexprocessing functions
//---------------------------------------------------------------------------
inline bool NiDX9RendererDesc::IsVetrexprocessingAvailable()
{
    // All vertexprocessing types can be set for both HAL and REF in DX9
    // The exception is selected pure device and NVPerfHUD selection
    if (GetDefaultPureDevice() || GetSettings()->m_bNVPerfHUD)
        return false;
    else
        return true;
}

//---------------------------------------------------------------------------
inline NiRendererSettings::Vertexprocessing
    NiDX9RendererDesc::GetDefaultVertexprocessing()
{
    if (!IsVetrexprocessingAvailable())
        return NiRendererSettings::VERTEX_UNSUPPORTED;

    return GetSettings()->m_eVertexProcessing;
}

//---------------------------------------------------------------------------
inline void NiDX9RendererDesc::SelectVertexprocessing (
    NiRendererSettings::Vertexprocessing eVertex)
{
    if (eVertex == NiRendererSettings::VERTEX_HARDWARE ||
        eVertex == NiRendererSettings::VERTEX_MIXED ||
        eVertex == NiRendererSettings::VERTEX_SOFTWARE)
    {
        GetSettings()->m_eVertexProcessing = eVertex;
    }
}

//---------------------------------------------------------------------------
// Windowed mode functions
//---------------------------------------------------------------------------
inline void NiDX9RendererDesc::SelectWindowedMode(bool bFullscreen)
{
    if (!bFullscreen && !CanRenderWindowed())
        bFullscreen = true;

    GetSettings()->m_bFullscreen = bFullscreen;
}

//---------------------------------------------------------------------------
inline bool NiDX9RendererDesc::CanRenderWindowed()
{
    return m_pkDeviceDesc->CanRenderWindowed();
}

//---------------------------------------------------------------------------
inline bool NiDX9RendererDesc::GetDefaultWindowedMode()
{
    SelectWindowedMode(GetSettings()->m_bFullscreen);
    return GetSettings()->m_bFullscreen;
}

//---------------------------------------------------------------------------
// Resolution functions
//---------------------------------------------------------------------------
inline bool NiDX9RendererDesc::IsWideScreenMode(
    const NiDX9AdapterDesc::ModeDesc* pkMode) const
{
    float fAspect = (float)pkMode->m_uiWidth / (float)pkMode->m_uiHeight;

    if (fAspect > 1.37f)    // Greater than Academy Aperture aspect ratio
        return true;

    return false;
}

//---------------------------------------------------------------------------
inline void NiDX9RendererDesc::SetResolutionFilter(
    ResolutionFilters eResFilter)
{
    if (eResFilter != RESFILTER_NORMAL && eResFilter != RESFILTER_WIDE)
        eResFilter = RESFILTER_ALL;

    m_eResolutionFilter = eResFilter;

    BuildResolutionList();
}
