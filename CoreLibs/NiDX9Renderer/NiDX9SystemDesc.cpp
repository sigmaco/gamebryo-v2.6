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

// Precompiled Header
#include "NiD3DRendererPCH.h"

#include "NiDX9SystemDesc.h"

//---------------------------------------------------------------------------
D3DFORMAT NiDX9DeviceDesc::DisplayFormatInfo::ms_aeDepthStencilFormats[
    NiDX9DeviceDesc::DisplayFormatInfo::DEPTH_STENCIL_NUM] = 
{
    D3DFMT_D16_LOCKABLE,
    D3DFMT_D32,
    D3DFMT_D15S1,
    D3DFMT_D24S8,
    D3DFMT_D24X8,
    D3DFMT_D24X4S4,
    D3DFMT_D32F_LOCKABLE,
    D3DFMT_D24FS8,
    D3DFMT_D16
};

//---------------------------------------------------------------------------
NiDX9DeviceDesc::DisplayFormatInfo::RenderTargetInfo::RenderTargetInfo()
{
    memset(m_akWindowedDSFormats, 0, 
        sizeof(m_akWindowedDSFormats));
    memset(m_akFullscreenDSFormats, 0, 
        sizeof(m_akFullscreenDSFormats));
}
//---------------------------------------------------------------------------
NiDX9DeviceDesc::DisplayFormatInfo::DisplayFormatInfo(LPDIRECT3D9 pkD3D9,
    unsigned int uiAdapter, D3DDEVTYPE kDevType, D3DFORMAT eFormat, 
    bool bFullscreen, bool bWindowed)
{
    m_eFormat = eFormat;
    m_bFullscreen = bFullscreen;
    m_bWindowed = bWindowed;

    // For each texture format that could be a render target, loop
    for (unsigned int uiRendTarget = 0; 
        uiRendTarget < NiDX9Renderer::FBFMT_NUM; uiRendTarget++)
    {
        if (uiRendTarget == NiDX9Renderer::FBFMT_UNKNOWN)
            continue;

        D3DFORMAT eRenderFormat = NiDX9Renderer::GetD3DFormat(
            (NiDX9Renderer::FrameBufferFormat)uiRendTarget);

        HRESULT eD3dRet = pkD3D9->CheckDeviceFormat(uiAdapter,
            kDevType, eFormat, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE,
            eRenderFormat);

        if (FAILED(eD3dRet))
            continue;

        RenderTargetInfo* pkRenderTarget = NiNew RenderTargetInfo;

        unsigned int uiWindowedRTMultiSampleTypes = 0;
        unsigned int uiFullscreenRTMultiSampleTypes = 0;

        // Check for multisample support in the render target type
        // Assume all render targets support D3DMULTISAMPLE_NONE
        unsigned int i = 0;
        for (; i < DFI_MAX_MULTISAMPLE_TYPES; i++)
        {
            DWORD uiResult;
            D3DMULTISAMPLE_TYPE eType = (D3DMULTISAMPLE_TYPE)(i + 1);
            if (bWindowed &&
                SUCCEEDED(pkD3D9->CheckDeviceMultiSampleType(uiAdapter, 
                kDevType, eFormat, true, eType, &uiResult)))
            {
                uiWindowedRTMultiSampleTypes |= (1 << i);
            }

            if (bFullscreen &&
                SUCCEEDED(pkD3D9->CheckDeviceMultiSampleType(uiAdapter, 
                kDevType, eFormat, false, eType, &uiResult)))
            {
                uiFullscreenRTMultiSampleTypes |= (1 << i);
            }
        }

        // Check for depth stencil support for the render target type
        bool bDSFound = false;
        for (i = 0; i < DEPTH_STENCIL_NUM; i++)
        {
            D3DFORMAT eDepthStencilFormat = ms_aeDepthStencilFormats[i];

            HRESULT hr = pkD3D9->CheckDeviceFormat(uiAdapter,
                kDevType, eFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
                eDepthStencilFormat);

            if (FAILED(hr))
                continue;

            hr = pkD3D9->CheckDepthStencilMatch(
                uiAdapter, kDevType, eFormat, eRenderFormat, 
                eDepthStencilFormat);

            if (FAILED(hr))
                continue;

            bDSFound = true;

            DepthStencilInfo* pkWindowed = 
                &(pkRenderTarget->m_akWindowedDSFormats[i]);

            DepthStencilInfo* pkFullscreen = 
                &(pkRenderTarget->m_akFullscreenDSFormats[i]);

            pkWindowed->m_bValid = bWindowed;
            pkFullscreen->m_bValid = bFullscreen;

            // Check for multisample support in the depth stencil type
            for (unsigned int uiMSIndex = 0; 
                uiMSIndex < DFI_MAX_MULTISAMPLE_TYPES; uiMSIndex++)
            {
                unsigned int uiMask = 1 << uiMSIndex;
                D3DMULTISAMPLE_TYPE eType = 
                    (D3DMULTISAMPLE_TYPE)(uiMSIndex + 1);

                if ((uiWindowedRTMultiSampleTypes & uiMask) != 0)
                {
                    DWORD uiResult;
                    if (SUCCEEDED(pkD3D9->CheckDeviceMultiSampleType(
                        uiAdapter, kDevType, eDepthStencilFormat, true, 
                        eType, &uiResult)))
                    {
                        NIASSERT(uiResult < 255);

                        pkWindowed->m_aucMultiSampleQuality[uiMSIndex] = 
                            (unsigned char)uiResult;
                    }
                    else
                    {
                        pkWindowed->m_aucMultiSampleQuality[uiMSIndex] = 0;
                    }
                }
                else
                {
                    pkWindowed->m_aucMultiSampleQuality[uiMSIndex] = 0;
                }

                if ((uiFullscreenRTMultiSampleTypes & uiMask) != 0)
                {
                    DWORD uiResult;
                    if (SUCCEEDED(pkD3D9->CheckDeviceMultiSampleType(
                        uiAdapter, kDevType, eDepthStencilFormat, false, 
                        eType, &uiResult)))
                    {
                        NIASSERT(uiResult < 255);

                        pkFullscreen->m_aucMultiSampleQuality[uiMSIndex] = 
                            (unsigned char)uiResult;
                    }
                    else
                    {
                        pkFullscreen->m_aucMultiSampleQuality[uiMSIndex] = 0;
                    }
                }
                else
                {
                    pkFullscreen->m_aucMultiSampleQuality[uiMSIndex] = 0;
                }
            }
        }

        if (bDSFound)
        {
            m_kRenderTargets.SetAt(eRenderFormat, pkRenderTarget);
        }
        else
        {
            NiDelete pkRenderTarget;
        }
    }
}
//---------------------------------------------------------------------------
NiDX9DeviceDesc::DisplayFormatInfo::~DisplayFormatInfo()
{
    NiTMapIterator kIter = m_kRenderTargets.GetFirstPos();

    while (kIter)
    {
        D3DFORMAT eFormat;
        RenderTargetInfo* pkRenderTarget;
        m_kRenderTargets.GetNext(kIter, eFormat, pkRenderTarget);

        NiDelete pkRenderTarget;
    }
}
//---------------------------------------------------------------------------
bool NiDX9DeviceDesc::DisplayFormatInfo::IsDepthStencilValid(
    bool bWindowed, D3DFORMAT eRenderFormat, D3DFORMAT eDepthFormat) const
{
    RenderTargetInfo* pkRenderTarget = NULL;
    m_kRenderTargets.GetAt(eRenderFormat, pkRenderTarget);
    if (pkRenderTarget == NULL)
        return false;

    unsigned int uiIndex = GetDepthStencilIndex(eDepthFormat);

    if (bWindowed)
        return pkRenderTarget->m_akWindowedDSFormats[uiIndex].m_bValid;
    else
        return pkRenderTarget->m_akFullscreenDSFormats[uiIndex].m_bValid;
}
//---------------------------------------------------------------------------
void NiDX9DeviceDesc::DisplayFormatInfo::GetDepthStencilBPPFromFormat(
    D3DFORMAT eD3DFmt, unsigned int& uiDepthBPP, unsigned int& uiStencilBPP)
{
    switch (eD3DFmt)
    {
    case D3DFMT_D16_LOCKABLE:
        uiDepthBPP = 16;
        uiStencilBPP = 0;
        break;
    case D3DFMT_D24S8:
        uiDepthBPP = 24;
        uiStencilBPP = 8;
        break;
    case D3DFMT_D32:
        uiDepthBPP = 32;
        uiStencilBPP = 0;
        break;
    case D3DFMT_D15S1:
        uiDepthBPP = 15;
        uiStencilBPP = 1;
        break;
    case D3DFMT_D16:
        uiDepthBPP = 16;
        uiStencilBPP = 0;
        break;
    case D3DFMT_D24X8:
        uiDepthBPP = 24;
        uiStencilBPP = 0;
        break;
    case D3DFMT_D24X4S4:
        uiDepthBPP = 24;
        uiStencilBPP = 4;
        break;
    case D3DFMT_UNKNOWN:
    default:
        uiDepthBPP = 0;
        uiStencilBPP = 0;
        break;
    }
}
//---------------------------------------------------------------------------
unsigned int NiDX9DeviceDesc::DisplayFormatInfo::GetMultiSampleQualityLevels(
    D3DMULTISAMPLE_TYPE eMultiSampleType, bool bWindowed, 
    D3DFORMAT eRenderFormat, D3DFORMAT eDepthFormat) const
{
    if (eMultiSampleType == D3DMULTISAMPLE_NONE)
        return 0;

    unsigned int uiMSType = (unsigned int)eMultiSampleType - 1;
    NIASSERT(uiMSType < DFI_MAX_MULTISAMPLE_TYPES);

    RenderTargetInfo* pkRenderTarget = NULL;
    m_kRenderTargets.GetAt(eRenderFormat, pkRenderTarget);
    if (pkRenderTarget == NULL)
        return 0;

    unsigned int uiDSIndex = GetDepthStencilIndex(eDepthFormat);

    if (bWindowed)
    {
        return pkRenderTarget->m_akWindowedDSFormats[uiDSIndex].
            m_aucMultiSampleQuality[uiMSType];
    }
    else
    {
        return pkRenderTarget->m_akFullscreenDSFormats[uiDSIndex].
            m_aucMultiSampleQuality[uiMSType];
    }
}
//---------------------------------------------------------------------------
D3DMULTISAMPLE_TYPE 
    NiDX9DeviceDesc::DisplayFormatInfo::GetHighestMultiSampleType(
    bool bWindowed, D3DFORMAT eRenderFormat, D3DFORMAT eDepthFormat) const
{
    RenderTargetInfo* pkRenderTarget = NULL;
    m_kRenderTargets.GetAt(eRenderFormat, pkRenderTarget);
    if (pkRenderTarget == NULL)
        return D3DMULTISAMPLE_NONE;

    unsigned int uiDSIndex = GetDepthStencilIndex(eDepthFormat);

    DepthStencilInfo* pkDSInfo = NULL;
    if (bWindowed)
        pkDSInfo = &(pkRenderTarget->m_akWindowedDSFormats[uiDSIndex]);
    else
        pkDSInfo = &(pkRenderTarget->m_akFullscreenDSFormats[uiDSIndex]);

    unsigned int uiMSIndex = DFI_MAX_MULTISAMPLE_TYPES - 1;
    while (uiMSIndex > 0 && 
        (pkDSInfo->m_aucMultiSampleQuality[uiMSIndex] == 0))
    {
        uiMSIndex--;
    }

    if (uiMSIndex == 0 && pkDSInfo->m_aucMultiSampleQuality[uiMSIndex] == 0)
        return D3DMULTISAMPLE_NONE;

    return (D3DMULTISAMPLE_TYPE)(uiMSIndex + 1);
}
//---------------------------------------------------------------------------
bool NiDX9DeviceDesc::DisplayFormatInfo::IsMultiSampleValid(
    bool bWindowed, D3DMULTISAMPLE_TYPE eMultiSampleType, 
    D3DFORMAT eRenderFormat, D3DFORMAT eDepthFormat) const
{
    RenderTargetInfo* pkRenderTarget = NULL;
    m_kRenderTargets.GetAt(eRenderFormat, pkRenderTarget);
    if (pkRenderTarget == NULL)
        return false;

    if (eMultiSampleType == D3DMULTISAMPLE_NONE)
        return true;

    unsigned int uiDSIndex = GetDepthStencilIndex(eDepthFormat);

    DepthStencilInfo* pkDSInfo = NULL;
    if (bWindowed)
        pkDSInfo = &(pkRenderTarget->m_akWindowedDSFormats[uiDSIndex]);
    else
        pkDSInfo = &(pkRenderTarget->m_akFullscreenDSFormats[uiDSIndex]);

    unsigned int uiMSIndex = (unsigned int)eMultiSampleType - 1;
    NIASSERT(uiMSIndex < DFI_MAX_MULTISAMPLE_TYPES);

    return (pkDSInfo->m_aucMultiSampleQuality[uiMSIndex] != 0);
}
//---------------------------------------------------------------------------
NiDX9Renderer::DepthStencilFormat 
    NiDX9DeviceDesc::DisplayFormatInfo::FindClosestDepthStencil(
    unsigned int uiDesiredDepthBPP, unsigned int uiDesiredStencilBPP) const
{
    return FindClosestDepthStencil(m_eFormat, uiDesiredDepthBPP,
        uiDesiredStencilBPP);
}
//---------------------------------------------------------------------------
NiDX9Renderer::DepthStencilFormat 
    NiDX9DeviceDesc::DisplayFormatInfo::FindClosestDepthStencil(
    D3DFORMAT eFBFormat, unsigned int uiDesiredDepthBPP, 
    unsigned int uiDesiredStencilBPP) const
{
    NIASSERT(uiDesiredDepthBPP < INT_MAX && uiDesiredStencilBPP < INT_MAX);

    NiDX9Renderer::DepthStencilFormat eFormat = NiDX9Renderer::DSFMT_UNKNOWN;

    RenderTargetInfo* pkRenderTarget = NULL;
    if (m_kRenderTargets.GetAt(eFBFormat, pkRenderTarget))
    {
        NiDX9Renderer::DepthStencilFormat eClosestFormat = 
            NiDX9Renderer::DSFMT_UNKNOWN;
        int iClosestDepth = INT_MAX;
        int iClosestStencil = INT_MAX;

        // Depth stencil formats should be the same for windowed
        // or fullscreen
        for (unsigned int i = 0; i < DEPTH_STENCIL_NUM; i++)
        {
            if (!pkRenderTarget->m_akFullscreenDSFormats[i].m_bValid)
                continue;

            D3DFORMAT eTmpFormat = ms_aeDepthStencilFormats[i];
            unsigned int uiDepth = 0;
            unsigned int uiStencil = 0;

            GetDepthStencilBPPFromFormat(eTmpFormat, uiDepth, uiStencil);
            NIASSERT(uiDepth < INT_MAX && uiStencil < INT_MAX);
            
            if ((abs(iClosestDepth - (int)uiDesiredDepthBPP) >=
                 abs((int)uiDepth - (int)uiDesiredDepthBPP)) &&
                (abs(iClosestStencil - (int)uiDesiredStencilBPP) >=
                 abs((int)uiStencil - (int)uiDesiredStencilBPP)))
            {
                iClosestDepth = (int)uiDepth;
                iClosestStencil = (int)uiStencil;
                eClosestFormat 
                    = (NiDX9Renderer::DepthStencilFormat)eTmpFormat;
            }
        }
        eFormat = eClosestFormat;
    }

    return eFormat;
}
//---------------------------------------------------------------------------
unsigned int NiDX9DeviceDesc::DisplayFormatInfo::GetDepthStencilIndex(
    D3DFORMAT eDepthFormat)
{
    switch (eDepthFormat)
    {
    case D3DFMT_D16_LOCKABLE:
        NIASSERT(ms_aeDepthStencilFormats[0] == D3DFMT_D16_LOCKABLE);
        return 0;
    case D3DFMT_D32:
        NIASSERT(ms_aeDepthStencilFormats[1] == D3DFMT_D32);
        return 1;
    case D3DFMT_D15S1:
        NIASSERT(ms_aeDepthStencilFormats[2] == D3DFMT_D15S1);
        return 2;
    case D3DFMT_D24S8:
        NIASSERT(ms_aeDepthStencilFormats[3] == D3DFMT_D24S8);
        return 3;
    case D3DFMT_D24X8:
        NIASSERT(ms_aeDepthStencilFormats[4] == D3DFMT_D24X8);
        return 4;
    case D3DFMT_D24X4S4:
        NIASSERT(ms_aeDepthStencilFormats[5] == D3DFMT_D24X4S4);
        return 5;
    case D3DFMT_D32F_LOCKABLE:
        NIASSERT(ms_aeDepthStencilFormats[6] == D3DFMT_D32F_LOCKABLE);
        return 6;
    case D3DFMT_D24FS8:
        NIASSERT(ms_aeDepthStencilFormats[7] == D3DFMT_D24FS8);
        return 7;
    case D3DFMT_D16:
        NIASSERT(ms_aeDepthStencilFormats[8] == D3DFMT_D16);
        return 8;
    default:
        NIASSERT(false);
        return DEPTH_STENCIL_NUM;
    }
}
//---------------------------------------------------------------------------
NiDX9DeviceDesc::NiDX9DeviceDesc(LPDIRECT3D9 pkD3D9, 
    unsigned int uiAdapter, D3DDEVTYPE kD3DDevType, 
    D3DFORMAT eCurrDisplayFormat, NiTPointerList<unsigned int>& kFormats) :
    m_kD3DDevType(kD3DDevType)
{

    NIASSERT(pkD3D9);

    HRESULT eD3dRet = pkD3D9->GetDeviceCaps(uiAdapter, m_kD3DDevType, 
        &m_kD3DCaps9);

    if (SUCCEEDED(eD3dRet))
    {
        //  Add in the default windowable mode...

        NiTListIterator kIter = kFormats.GetHeadPos();
        while (kIter)
        {
            D3DFORMAT eFormat = (D3DFORMAT)kFormats.GetNext(kIter);
            if (eFormat == D3DFMT_UNKNOWN)
                continue;

            unsigned int uiTotalAdapterModes = pkD3D9->GetAdapterModeCount(
                uiAdapter, eFormat);
            for (unsigned int ui = 0; ui < uiTotalAdapterModes; ui++)
            {
                D3DDISPLAYMODE kD3DMode;

                bool bWindowedValid = false;
                bool bFullscreenValid = false;

                HRESULT eD3dRet = pkD3D9->EnumAdapterModes(uiAdapter, eFormat,
                    ui, &kD3DMode);
                if (SUCCEEDED(eD3dRet))
                {
                    //  Check for valid full-screen operation
                    eD3dRet = pkD3D9->CheckDeviceType(uiAdapter, m_kD3DDevType,
                        eFormat, kD3DMode.Format, false);

                    bFullscreenValid = true;
                }

                //  Check for valid windowed operation
                eD3dRet = pkD3D9->CheckDeviceType(uiAdapter, m_kD3DDevType, 
                    eCurrDisplayFormat, kD3DMode.Format, true);
                if (SUCCEEDED(eD3dRet))
                {
                    bWindowedValid = true;
                }

                if (bFullscreenValid || bWindowedValid)
                {
                    //  See if this is in the valid format list...
                    const DisplayFormatInfo* pkFormatInfo = 
                        GetFormatInfo(kD3DMode.Format);

                    if (pkFormatInfo == NULL)
                    {
                        //  Create a new entry...
                        pkFormatInfo = NiNew DisplayFormatInfo(pkD3D9, 
                            uiAdapter, m_kD3DDevType, kD3DMode.Format, 
                            bFullscreenValid, bWindowedValid);
                        NIASSERT(pkFormatInfo);

                        m_kScreenFormats.AddTail(
                            (DisplayFormatInfo*)pkFormatInfo);
                    }
                }
            }
        }
    }
    else
    {
        m_kD3DCaps9.DeviceType = (D3DDEVTYPE)0;
    }

    const DisplayFormatInfo* pkFormat = GetFormatInfo(eCurrDisplayFormat); 
    
    if (pkFormat && pkFormat->CanRenderWindowed())
        m_bRenderWindowed = true;
    else
        m_bRenderWindowed = false;
}
//---------------------------------------------------------------------------
NiDX9DeviceDesc::~NiDX9DeviceDesc()
{
    NiTListIterator kListPos = m_kScreenFormats.GetHeadPos();
    while (kListPos)
    {
        DisplayFormatInfo* pkFormatInfo = m_kScreenFormats.GetNext(kListPos);
        NiDelete pkFormatInfo;
    }
}
//---------------------------------------------------------------------------
NiDX9Renderer::FrameBufferFormat 
NiDX9DeviceDesc::GetNearestFrameBufferFormat(bool bWindowed, 
    unsigned int uiDesiredBPP) const
{
    NiDX9Renderer::FrameBufferFormat eFormat = NiDX9Renderer::FBFMT_UNKNOWN;

    if (bWindowed)
    {
        NiTListIterator kPos = m_kScreenFormats.GetHeadPos();
        while (kPos)
        {
            DisplayFormatInfo* pkFormatInfo = 
                (DisplayFormatInfo*)(m_kScreenFormats.GetNext(kPos));
            if (pkFormatInfo)
            {
                if (!pkFormatInfo->CanRenderWindowed())
                {
                    continue;
                }
                else
                {
                    unsigned int uiTestBPP = 
                        NiDX9SystemDesc::GetBPPFromFormat(
                        pkFormatInfo->GetFormat());
                    if (uiTestBPP == uiDesiredBPP)
                    {
                        eFormat = NiDX9Renderer::GetNiFBFormat(
                            pkFormatInfo->GetFormat());
                        break;
                    }
                }
            }
        }
    }
    else
    {
        NiTListIterator kPos = m_kScreenFormats.GetHeadPos();
        while (kPos)
        {
            DisplayFormatInfo* pkFormatInfo = 
                (DisplayFormatInfo*)(m_kScreenFormats.GetNext(kPos));
            if (pkFormatInfo)
            {
                if (!pkFormatInfo->CanRenderFullscreen())
                {
                    continue;
                }
                else
                {
                    unsigned int uiTestBPP = 
                        NiDX9SystemDesc::GetBPPFromFormat(
                            pkFormatInfo->GetFormat());
                    if (uiTestBPP == uiDesiredBPP)
                    {
                        eFormat = NiDX9Renderer::GetNiFBFormat(
                            pkFormatInfo->GetFormat());
                        break;
                    }
                }
            }
        }
    }

    return eFormat;
}
//---------------------------------------------------------------------------
NiDX9Renderer::DepthStencilFormat 
NiDX9DeviceDesc::GetNearestDepthStencilFormat(D3DFORMAT eAdapterMode, 
    D3DFORMAT eFBFormat, unsigned int uiDesiredDepthBPP, 
    unsigned int uiDesiredStencilBPP) const
{
    const DisplayFormatInfo* pkMatchFormatInfo = GetFormatInfo(eAdapterMode);

    if (!pkMatchFormatInfo)
        return NiDX9Renderer::DSFMT_UNKNOWN;

    //  If they desire a stencil buffer, then we need to subtract the
    //  stencil bits from the depth bits when the depth bpp is > 24.
    // For now, we assume that there are either 0, 1 or 8 stencil bits
    if (uiDesiredStencilBPP)
    {
        if (uiDesiredDepthBPP > 16)
        {
            uiDesiredDepthBPP = 24;
            if (uiDesiredStencilBPP != 1)
                uiDesiredStencilBPP = 8;
        }
        else
        {
            if (uiDesiredStencilBPP != 1)
            {
                uiDesiredStencilBPP = 8;
                uiDesiredDepthBPP = 24;
            }
            else
            {
                uiDesiredStencilBPP = 1;
                uiDesiredDepthBPP = 15;
            }
        }
    }

    return pkMatchFormatInfo->FindClosestDepthStencil(eFBFormat, 
        uiDesiredDepthBPP, uiDesiredStencilBPP);
}
//---------------------------------------------------------------------------
NiDX9AdapterDesc::ModeDesc::ModeDesc(D3DDISPLAYMODE& kD3DMode) :
    m_uiWidth(kD3DMode.Width),
    m_uiHeight(kD3DMode.Height),
    m_eD3DFormat(kD3DMode.Format)
{
    m_kRefreshRates.AddHead(kD3DMode.RefreshRate);
    m_uiBPP = NiDX9SystemDesc::GetBPPFromFormat(kD3DMode.Format);
}
//---------------------------------------------------------------------------
NiDX9AdapterDesc::NiDX9AdapterDesc(LPDIRECT3D9 pkD3D9, unsigned int uiAdapter,
    NiTPointerList<unsigned int>& kFormats) :
    m_kModeList(32, 16),
    m_uiAdapter(uiAdapter)
{
    pkD3D9->GetAdapterIdentifier(m_uiAdapter, 0, &m_kAdapterIdentifier);

    // Enumerate available size/format/refresh combinations
    
    NiTListIterator kIter = kFormats.GetHeadPos();
    while (kIter)
    {
        D3DFORMAT eFormat = (D3DFORMAT)kFormats.GetNext(kIter);
        if (eFormat == D3DFMT_UNKNOWN)
            continue;

        D3DDISPLAYMODE kD3DMode;
        unsigned int uiTotalAdapterModes = pkD3D9->GetAdapterModeCount(
            m_uiAdapter, eFormat);

        for (unsigned int i = 0; i < uiTotalAdapterModes; i++)
        {
            HRESULT eD3dRet = pkD3D9->EnumAdapterModes(m_uiAdapter, eFormat, 
                i, &kD3DMode);
            if (FAILED(eD3dRet))
                continue;

            ModeDesc* pkCurrentMode = NULL;

            for (unsigned int j = 0; j < m_kModeList.GetSize(); j++)
            {
                ModeDesc* pkMode = m_kModeList.GetAt(j);

                if (pkMode && (pkMode->m_eD3DFormat == eFormat) &&
                    (pkMode->m_uiWidth == kD3DMode.Width) &&
                    (pkMode->m_uiHeight == kD3DMode.Height))
                {
                    pkCurrentMode = pkMode;
                    pkMode->m_kRefreshRates.AddTail(kD3DMode.RefreshRate);
                }
            }

            if (pkCurrentMode == NULL)
            {
                pkCurrentMode = NiNew ModeDesc(kD3DMode);

                m_kModeList.Add(pkCurrentMode);
            }
        }
    }

    D3DDISPLAYMODE kCurrDisplaymode;
    pkD3D9->GetAdapterDisplayMode(m_uiAdapter, &kCurrDisplaymode);

    m_pkHALDeviceDesc = NiNew NiDX9DeviceDesc(pkD3D9, uiAdapter, 
        D3DDEVTYPE_HAL, kCurrDisplaymode.Format, kFormats);
    m_pkREFDeviceDesc = NiNew NiDX9DeviceDesc(pkD3D9, uiAdapter, 
        D3DDEVTYPE_REF, kCurrDisplaymode.Format, kFormats);
}
//---------------------------------------------------------------------------
NiDX9AdapterDesc::~NiDX9AdapterDesc()
{
    for (unsigned int i = 0; i < m_kModeList.GetSize(); i++)
    {
        NiDelete m_kModeList.GetAt(i);
    }

    NiDelete m_pkHALDeviceDesc;
    NiDelete m_pkREFDeviceDesc;
}
//---------------------------------------------------------------------------
NiDX9AdapterDesc::ModeDesc* NiDX9AdapterDesc::ValidateFullscreenMode(
    D3DFORMAT eFormat, unsigned uiWidth, unsigned uiHeight, 
    unsigned int& uiRefreshRate) const
{
    ModeDesc* pkBestMode = NULL;

    for (unsigned int i = 0; i < m_kModeList.GetSize(); i++)
    {
        ModeDesc* pkMode = m_kModeList.GetAt(i);

        if (pkMode && (pkMode->m_eD3DFormat == eFormat) &&
            (pkMode->m_uiWidth == uiWidth) &&
            (pkMode->m_uiHeight == uiHeight))
        {
            pkBestMode = pkMode;

            uiRefreshRate = pkMode->GetClosestRefreshRate(uiRefreshRate);
        }
    }

    return pkBestMode;
}
//---------------------------------------------------------------------------
NiDX9SystemDesc::NiDX9SystemDesc(LPDIRECT3D9 pkD3D9, 
    NiTPointerList<unsigned int>& kFormats)
{
    m_uiAdapterCount = 0;

    FillAdapterInformation(pkD3D9, kFormats);

#ifdef _DEBUG_DEVICE_INFO_
    Dump();
#endif
}
//---------------------------------------------------------------------------
NiDX9SystemDesc::~NiDX9SystemDesc()
{
    for (unsigned int i = 0; i < m_kAdapterList.GetSize(); i++)
    {
        NiDX9AdapterDesc* pkAdapter = m_kAdapterList.GetAt(i);
        NiDelete pkAdapter;
    }
}
//---------------------------------------------------------------------------
void NiDX9SystemDesc::FillAdapterInformation(LPDIRECT3D9 pkD3D9,
    NiTPointerList<unsigned int>& kFormats)
{
    NIASSERT(pkD3D9);

    m_uiAdapterCount = pkD3D9->GetAdapterCount();
    m_kAdapterList.SetSize(m_uiAdapterCount);

    unsigned int ui;
    for (ui = 0; ui < m_uiAdapterCount; ui++)
    {
        NiDX9AdapterDesc* pAdapter = NiNew NiDX9AdapterDesc(pkD3D9, ui, 
            kFormats);
        NIASSERT(pAdapter);

        m_kAdapterList.SetAt(ui, pAdapter);
    }
}
//---------------------------------------------------------------------------
char* NiDX9SystemDesc::GetD3D9FormatString(D3DFORMAT eD3DFmt)
{
    switch (eD3DFmt)
    {
    case D3DFMT_R8G8B8:
        return "D3DFMT_R8G8B8";
    case D3DFMT_A8R8G8B8:
        return "D3DFMT_A8R8G8B8";
    case D3DFMT_X8R8G8B8:
        return "D3DFMT_X8R8G8B8";
    case D3DFMT_R5G6B5:
        return "D3DFMT_R5G6B5";
    case D3DFMT_X1R5G5B5:
        return "D3DFMT_X1R5G5B5";
    case D3DFMT_A1R5G5B5:
        return "D3DFMT_A1R5G5B5";
    case D3DFMT_A4R4G4B4:
        return "D3DFMT_A4R4G4B4";
    case D3DFMT_R3G3B2:
        return "D3DFMT_R3G3B2";
    case D3DFMT_A8:
        return "D3DFMT_A8";
    case D3DFMT_A8R3G3B2:
        return "D3DFMT_A8R3G3B2";
    case D3DFMT_X4R4G4B4:
        return "D3DFMT_X4R4G4B4";
    case D3DFMT_A2B10G10R10:
        return "D3DFMT_A2B10G10R10";
    case D3DFMT_A8B8G8R8:
        return "D3DFMT_A8B8G8R8";
    case D3DFMT_X8B8G8R8:
        return "D3DFMT_X8B8G8R8";
    case D3DFMT_G16R16:
        return "D3DFMT_G16R16";
    case D3DFMT_A2R10G10B10:
        return "D3DFMT_A2R10G10B10";
    case D3DFMT_A16B16G16R16:
        return "D3DFMT_A16B16G16R16";
    case D3DFMT_A8P8:
        return "D3DFMT_A8P8";
    case D3DFMT_P8:
        return "D3DFMT_P8";
    case D3DFMT_L8:
        return "D3DFMT_L8";
    case D3DFMT_A8L8:
        return "D3DFMT_A8L8";
    case D3DFMT_A4L4:
        return "D3DFMT_A4L4";
    case D3DFMT_V8U8:
        return "D3DFMT_V8U8";
    case D3DFMT_L6V5U5:
        return "D3DFMT_L6V5U5";
    case D3DFMT_X8L8V8U8:
        return "D3DFMT_X8L8V8U8";
    case D3DFMT_Q8W8V8U8:
        return "D3DFMT_Q8W8V8U8";
    case D3DFMT_V16U16:
        return "D3DFMT_V16U16";
    case D3DFMT_A2W10V10U10:
        return "D3DFMT_A2W10V10U10";
    case D3DFMT_UYVY:
        return "D3DFMT_UYVY";
    case D3DFMT_R8G8_B8G8:
        return "D3DFMT_R8G8_B8G8";
    case D3DFMT_YUY2:
        return "D3DFMT_YUY2";
    case D3DFMT_G8R8_G8B8:
        return "D3DFMT_G8R8_G8B8";
    case D3DFMT_DXT1:
        return "D3DFMT_DXT1";
    case D3DFMT_DXT2:
        return "D3DFMT_DXT2";
    case D3DFMT_DXT3:
        return "D3DFMT_DXT3";
    case D3DFMT_DXT4:
        return "D3DFMT_DXT4";
    case D3DFMT_DXT5:
        return "D3DFMT_DXT5";
    case D3DFMT_D16_LOCKABLE:
        return "D3DFMT_D16_LOCKABLE";
    case D3DFMT_D32:
        return "D3DFMT_D32";
    case D3DFMT_D15S1:
        return "D3DFMT_D15S1";
    case D3DFMT_D24S8:
        return "D3DFMT_D24S8";
    case D3DFMT_D24X8:
        return "D3DFMT_D24X8";
    case D3DFMT_D24X4S4:
        return "D3DFMT_D24X4S4";
    case D3DFMT_D16:
        return "D3DFMT_D16";
    case D3DFMT_D32F_LOCKABLE:
        return "D3DFMT_D32F_LOCKABLE";
    case D3DFMT_D24FS8:
        return "D3DFMT_D24FS8";
    case D3DFMT_L16:
        return "D3DFMT_L16";
    case D3DFMT_VERTEXDATA:
        return "D3DFMT_VERTEXDATA";
    case D3DFMT_INDEX16:
        return "D3DFMT_INDEX16";
    case D3DFMT_INDEX32:
        return "D3DFMT_INDEX32";
    case D3DFMT_Q16W16V16U16:
        return "D3DFMT_Q16W16V16U16";
    case D3DFMT_MULTI2_ARGB8:
        return "D3DFMT_MULTI2_ARGB8";
    case D3DFMT_R16F:
        return "D3DFMT_R16F";
    case D3DFMT_G16R16F:
        return "D3DFMT_G16R16F";
    case D3DFMT_A16B16G16R16F:
        return "D3DFMT_A16B16G16R16F";
    case D3DFMT_R32F:
        return "D3DFMT_R32F";
    case D3DFMT_G32R32F:
        return "D3DFMT_G32R32F";
    case D3DFMT_A32B32G32R32F:
        return "D3DFMT_A32B32G32R32F";
    case D3DFMT_CxV8U8:
        return "D3DFMT_CxV8U8";
    case D3DFMT_UNKNOWN:
    default:
        return "UNKNOWN";
    }
}
//---------------------------------------------------------------------------
unsigned int NiDX9SystemDesc::GetBPPFromFormat(D3DFORMAT eD3DFmt)
{
    switch (eD3DFmt)
    {
    case D3DFMT_UNKNOWN:
        return 0;

    case D3DFMT_VERTEXDATA:
        return 0;

    // FOURCC formats
    case D3DFMT_UYVY:
    case D3DFMT_R8G8_B8G8:
    case D3DFMT_YUY2:
    case D3DFMT_G8R8_G8B8:
    case D3DFMT_DXT1:
    case D3DFMT_DXT2:
    case D3DFMT_DXT3:
    case D3DFMT_DXT4:
    case D3DFMT_DXT5:
    case D3DFMT_MULTI2_ARGB8:
        return 0;

    case D3DFMT_R3G3B2:
    case D3DFMT_A8:
    case D3DFMT_P8:
    case D3DFMT_L8:
    case D3DFMT_A4L4:
        return 8;

    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_A8R3G3B2:
    case D3DFMT_X4R4G4B4:
    case D3DFMT_G16R16:
    case D3DFMT_A8P8:
    case D3DFMT_A8L8:
    case D3DFMT_V8U8:
    case D3DFMT_L6V5U5:
    case D3DFMT_D16_LOCKABLE:
    case D3DFMT_D15S1:
    case D3DFMT_D16:
    case D3DFMT_L16:
    case D3DFMT_INDEX16:
    case D3DFMT_R16F:
    case D3DFMT_CxV8U8:
        return 16;

    case D3DFMT_R8G8B8:
        return 24;

    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_A2B10G10R10:
    case D3DFMT_A8B8G8R8:
    case D3DFMT_X8B8G8R8:
    case D3DFMT_A2R10G10B10:
    case D3DFMT_X8L8V8U8:
    case D3DFMT_Q8W8V8U8:
    case D3DFMT_V16U16:
    case D3DFMT_A2W10V10U10:
    case D3DFMT_D32:
    case D3DFMT_D24S8:
    case D3DFMT_D24X8:
    case D3DFMT_D24X4S4:
    case D3DFMT_D32F_LOCKABLE:
    case D3DFMT_D24FS8:
    case D3DFMT_INDEX32:
    case D3DFMT_G16R16F:
    case D3DFMT_R32F:
        return 32;
    
    case D3DFMT_A16B16G16R16:
    case D3DFMT_Q16W16V16U16:
    case D3DFMT_A16B16G16R16F:
    case D3DFMT_G32R32F:
        return 64;

    case D3DFMT_A32B32G32R32F:
        return 128;
        
    default:
        return 0xFFFFFFFF;
    }
}
//---------------------------------------------------------------------------
#ifdef _DEBUG_DEVICE_INFO_
char g_szDump[1024];
class DX9DeviceLogfileInit
{
public:
    DX9DeviceLogfileInit()
    {
        m_pkFile = fopen("DX9DeviceLogfileInit.txt","w");
        fprintf(m_pkFile, "*** Start Log File ***\n");        
    }

    ~DX9DeviceLogfileInit()
    {
        fprintf(m_pkFile, "*** End Log File ***\n");
        fclose(m_pkFile);
    }

    FILE* m_pkFile;
};
//---------------------------------------------------------------------------
static DX9DeviceLogfileInit DX9DeviceLogfileInitObj;

//---------------------------------------------------------------------------
#define OutputToDebuggingLogDI(pcStr)                                   \
{                                                                       \
    fprintf(DX9DeviceLogfileInitObj.m_pkFile,"%s",pcStr);               \
    NiOutputDebugString(pcStr);                                         \
} 
#endif //#ifdef _DEBUG_DEVICE_INFO_
//---------------------------------------------------------------------------
void NiDX9DeviceDesc::DisplayFormatInfo::Dump()
{
#ifdef _DEBUG_DEVICE_INFO_
    NiSprintf(g_szDump, 1024, "\tDISPLAY FORMAT: %s -\n", 
        NiDX9SystemDesc::GetD3D9FormatString(m_eFormat));
    OutputToDebuggingLogDI(g_szDump);

    if (m_bWindowed)
    {
        if (m_bFullscreen)
        {
            NiSprintf(g_szDump, 1024, "\t\tFULLSCREEN and WINDOWED\n");
            OutputToDebuggingLogDI(g_szDump);
        }
        else
        {
            NiSprintf(g_szDump, 1024, "\t\tWINDOWED ONLY\n");
            OutputToDebuggingLogDI(g_szDump);
        }
    }
    else
    {
        if (m_bFullscreen)
        {
            NiSprintf(g_szDump, 1024, "\t\tFULLSCREEN ONLY\n");
            OutputToDebuggingLogDI(g_szDump);
        }
        else
        {
            NIASSERT(0);
        }
    }

    NiTMapIterator pkMapPos = m_kValidDepthStencils.GetFirstPos();

    while (pkMapPos)
    {
        D3DFORMAT eRenderFormat;
        NiTPointerList<unsigned int>* pkList;
        m_kValidDepthStencils.GetNext(pkMapPos, eRenderFormat, pkList);

        NiSprintf(g_szDump, 1024, "\t\tRENDER TARGET: %s\n", 
            NiDX9SystemDesc::GetD3D9FormatString(eRenderFormat));
        OutputToDebuggingLogDI(g_szDump);

        NiTListIterator kPos = pkList->GetHeadPos();
        while (kPos)
        {
            D3DFORMAT eDepthFormat = (D3DFORMAT)(pkList->GetNext(kPos));
            NiSprintf(g_szDump, 1024, "\t\t\tVALID DS: %s\n", 
                NiDX9SystemDesc::GetD3D9FormatString(eDepthFormat));
            OutputToDebuggingLogDI(g_szDump);
        }
    }
#endif  //#ifdef _DEBUG_DEVICE_INFO_
}
//---------------------------------------------------------------------------
void NiDX9DeviceDesc::Dump()
{
#ifdef _DEBUG_DEVICE_INFO_
    NiSprintf(g_szDump, 1024, "DEV DESC: kDevType %s\n",
        (m_kD3DDevType == D3DDEVTYPE_HAL) ? "HAL" : "REF");
    OutputToDebuggingLogDI(g_szDump);

    NiTListIterator kPos = m_kScreenFormats.GetHeadPos();
    while (kPos)
    {
        DisplayFormatInfo* pkFormatInfo = m_kScreenFormats.GetNext(kPos);
        if (pkFormatInfo)
            pkFormatInfo->Dump();
    }
#endif  //#ifdef _DEBUG_DEVICE_INFO_
}
//---------------------------------------------------------------------------
void NiDX9AdapterDesc::ModeDesc::Dump()
{
#ifdef _DEBUG_DEVICE_INFO_
    NiSprintf(g_szDump, 1024, "MODEDESC: %4dx%4d%4d - %s\n", m_uiWidth,
        m_uiHeight, m_uiBPP,
        NiDX9SystemDesc::GetD3D9FormatString(m_d3dFormat));
    OutputToDebuggingLogDI(g_szDump);
#endif  //#ifdef _DEBUG_DEVICE_INFO_
}
//---------------------------------------------------------------------------
void NiDX9AdapterDesc::Dump()
{
#ifdef _DEBUG_DEVICE_INFO_
    NiSprintf(g_szDump, 1024, "ADPTDESC: Adapter %d\n", m_uiAdapter);
    OutputToDebuggingLogDI(g_szDump);

    NiSprintf(g_szDump, 1024, "          Modes %d\n", m_kModeList.GetSize());
    OutputToDebuggingLogDI(g_szDump);
    
    ModeDesc* pkMode;
    for (unsigned int i = 0; i < m_kModeList.GetSize(); i++)
    {
        pkMode = m_kModeList.GetAt(i);
        if (pkMode)
        {
            pkMode->Dump();
        }
    }

    m_pkHALDeviceDesc->Dump();
    m_pkREFDeviceDesc->Dump();
#endif  //#ifdef _DEBUG_DEVICE_INFO_
}
//---------------------------------------------------------------------------
void NiDX9SystemDesc::Dump()
{
#ifdef _DEBUG_DEVICE_INFO_
    NiDX9AdapterDesc* pkAdapter;
    for (unsigned int ui = 0; ui < m_uiAdapterCount; ui++)
    {
        pkAdapter = m_aAdapterList.GetAt(ui);
        pkAdapter->Dump();
        OutputToDebuggingLogDI("\n");
    }
#endif  //#ifdef _DEBUG_DEVICE_INFO_
}
//---------------------------------------------------------------------------
