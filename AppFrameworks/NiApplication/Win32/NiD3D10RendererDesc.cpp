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

#include "NiApplicationPCH.h"
#include "NiD3D10RendererDesc.h"
#include <NiD3D10Renderer.h>
#include <NiD3D10PixelFormat.h>

#ifdef NID3D10RENDERER_IMPORT
    #pragma comment(lib, "NiD3D10Renderer" NI_DLL_SUFFIX ".lib")
#else   // #ifdef NID3D10RENDERER_IMPORT
    #pragma comment(lib, "NiD3D10Renderer.lib")
#endif  // #ifdef NID3D10RENDERER_IMPORT

//---------------------------------------------------------------------------
NiD3D10RendererDesc::NiD3D10RendererDesc() :
    m_pkAdapterDesc(NULL),
    m_pkDeviceDesc(NULL),
    m_pkOutputDesc(NULL),
    m_bSupportsNVPerfHUD(false),
    m_eResolutionFilter(RESFILTER_ALL)
{
    /* */
}
//---------------------------------------------------------------------------
bool NiD3D10RendererDesc::Initialize()
{
    // Initialize adapter, device, etc. pointers. If function returns false,
    // it indicates that something goes wrong (maybe, Direct3D 10 is not
    // supported on that system) and no class methods should be used.

    if (!GetSettings())
    {
        NiMessageBox(
            "Error in NiD3D10RendererDesc::Initialize()\n"
            "NULL pointer to NiRendererSettings class.\n"
            "This class must be instantiated before\n"
            "performing any calls to NiSettingsDialog",
            "NiD3D10RendererDesc Error");
        return 0;
    }

    NiD3D10SystemDesc::GetSystemDesc(m_spSystemDesc);
    if (!m_spSystemDesc)
    {
        NiMessageBox(
            "Error in NiD3D10RendererDesc::Initialize()\n"
            "NULL pointer to NiD3D10SystemDesc structure.\n"
            "Maybe, Direct3D 10 is not supported on this system?",
            "NiD3D10RendererDesc Error");
        return false;
    }

    if (m_spSystemDesc->IsEnumerationValid() == false)
    {
        // Give system another chance to enumerate
        m_spSystemDesc->Enumerate();
        if (m_spSystemDesc->IsEnumerationValid() == false)
        {
            // Enumeration still not valid; probably, D3D10 unsupported.
            return false;
        }
    }

    if (!BuildAdapterList())
    {
        NiMessageBox(
            "Error in NiD3D10RendererDesc::Initialize()\n"
            "Adapter list is empty.\n"
            "Maybe, Direct3D 10 is not supported on this system?",
            "NiD3D10RendererDesc Error");
        return false;
    }

    // Store whether the application is requesting a reference rasterizer
    bool bRefRastRequested = GetSettings()->m_bRefRast;

    // After adapter change all capabilities will be reenumerated.
    bool bRes = false;
    // Select NVPerfHUD adapter if requested and it is supported
    if (GetSettings()->m_bNVPerfHUD && m_bSupportsNVPerfHUD)
        bRes = SelectPerfHUDAdapter();

    // Select default adapter if NVPerfHUD sel fails or it is not requested
    if (!bRes)
        bRes = SelectAdapter(GetDefaultAdapter());

    // If after all that adapter cannot be selected, exit with error
    if (!bRes)
        return false;

    // If the selected adapter supports only a reference rasterizer, confirm that
    // the app is requesting a reference rasterizer before returning.
    if (!bRefRastRequested && 
        m_pkAdapterDesc->GetHWDevice() == NULL)
    {
        // Restore refrast settings, which may have changed
        GetSettings()->m_bRefRast = bRefRastRequested;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Rebuild functions. Each settings depends on some, and some depends on it.
// So, when some setting is changed, all dependents must be re-enumerated
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::RebuildAdapterDependentInfo()
{
    // Reinitialize functions that depend on adapter only

    // Select display
    BuildDisplayList();
    SelectDisplay(GetDefaultDisplay());
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::RebuildDeviceDependentInfo()
{
    // Reinitialize functions that depend on adapter and device only

    // Update fullscreen. This fn will call RebuildWindowedDependentInfo()
    GetDefaultWindowedMode();

    BuildResolutionList();
    GetDefaultResolutionIdx();

    BuildDSFormatsList();
    GetDefaultDSFormat();

    BuildMultisamplesList();
    GetDefaultMultisample();
}
//---------------------------------------------------------------------------
bool ConvertWCHARToCHAR(char* pcDestination, const WCHAR* pwSource,
    unsigned int uiDestChar)
{
    if (pcDestination == NULL || pwSource == NULL || uiDestChar == 0)
        return false;

    int iResult = WideCharToMultiByte(CP_ACP, 0, pwSource, -1, pcDestination,
        uiDestChar*sizeof(char), NULL, NULL);
    pcDestination[uiDestChar - 1] = 0;

    return (iResult != 0);
}
//---------------------------------------------------------------------------
bool NiD3D10RendererDesc::BuildAdapterList()
{
    // Prepare adapter list for view. This list is needed because view part
    // knows only indexes in list with adapter names, not real indexes
    m_kAdapters.RemoveAll();
    m_bSupportsNVPerfHUD = false;
    char acName[256];

    const NiD3D10AdapterDesc* pkPerfHUDAdapter;
    for (unsigned int i = 0; i < m_spSystemDesc->GetAdapterCount(); i++)
    {
        const NiD3D10AdapterDesc* pkAdapter =
            m_spSystemDesc->GetAdapterDesc(i);
        if (!pkAdapter)
            return false;

        // Check NVPerfHUD adapter
        ConvertWCHARToCHAR(acName, pkAdapter->GetDesc()->Description, 256);
        NiFixedString kAdapterName = acName;
        if (pkAdapter->IsPerfHUD())
        {
            m_bSupportsNVPerfHUD = true;
            pkPerfHUDAdapter = pkAdapter;
            continue;
        }

        m_kAdapters.Add(pkAdapter);
    }

    // Move NVPerfHud adapter to the end of list - it is not visible in dlg
    if (m_bSupportsNVPerfHUD)
    {
        m_kAdapters.Add(pkPerfHUDAdapter);
        GetSettings()->m_uiNVPerfHUDAdapterIdx =
            pkPerfHUDAdapter->GetIndex();
    }

    return m_kAdapters.GetSize() > 0;
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::GetAdapterList(
    NiTObjectArray<NiFixedString>& kList)
{
    // Fill adapter names array for view
    unsigned int uiAdapterCount = m_kAdapters.GetSize();
    char acName[256];
    // NVPerfHUD adapter is last in list and won't be shown to user
    if (m_bSupportsNVPerfHUD)
        uiAdapterCount--;

    for (unsigned int i = 0; i < uiAdapterCount; i++)
    {
        const NiD3D10AdapterDesc* pkAdapter = m_kAdapters[i];
        ConvertWCHARToCHAR(acName, pkAdapter->GetDesc()->Description, 256);
        kList.Add(NiFixedString(acName));
    }
}
//---------------------------------------------------------------------------
unsigned int NiD3D10RendererDesc::GetDefaultAdapter()
{
    unsigned int uiAdapterCount = m_kAdapters.GetSize();

    if (GetSettings()->m_bNVPerfHUD && m_bSupportsNVPerfHUD)
        uiAdapterCount--;

    if (GetSettings()->m_uiAdapterIdx >= uiAdapterCount)
        GetSettings()->m_uiAdapterIdx = uiAdapterCount - 1;

    return GetSettings()->m_uiAdapterIdx;
}
//---------------------------------------------------------------------------
bool NiD3D10RendererDesc::SelectAdapter(unsigned int uiAdaptIdx)
{
    if (uiAdaptIdx >= m_kAdapters.GetSize())
        return false;

    // Update default adapter desc pointer, save old for safety
    const NiD3D10AdapterDesc* pkOldAdapter = m_pkAdapterDesc;
    m_pkAdapterDesc = m_kAdapters[uiAdaptIdx];

    if (!IsDeviceAvailable(true) && !IsDeviceAvailable(false))
    {
        NiMessageBox(
            "NiDX9AdapterDesc::GetDevice() returned\n"
            "NULL for both REF and HAL devices,\n"
            "cannot select chosen adapter.",
            "Adapter Error");

        m_pkAdapterDesc = pkOldAdapter;

        return false;
    }

    GetSettings()->m_bNVPerfHUD = false;
    GetSettings()->m_uiAdapterIdx = m_pkAdapterDesc->GetIndex();

    RebuildAdapterDependentInfo();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10RendererDesc::SelectPerfHUDAdapter()
{
    // PerfHUD adapter is always last in list
    if (m_bSupportsNVPerfHUD)
    {
        // Update default adapter desc pointer, save old for safety
        const NiD3D10AdapterDesc* pkOldAdapter = m_pkAdapterDesc;
        m_pkAdapterDesc = m_kAdapters[m_kAdapters.GetSize() - 1];

        if (!IsDeviceAvailable(true) && !IsDeviceAvailable(false))
        {
            NiMessageBox(
                "NiDX9AdapterDesc::GetDevice() returned\n"
                "NULL for both REF and HAL devices,\n"
                "cannot select NVPerfHUD adapter.",
                "Adapter Error");

            m_pkAdapterDesc = pkOldAdapter;

            return false;
        }

        GetSettings()->m_bNVPerfHUD = true;
        GetSettings()->m_uiNVPerfHUDAdapterIdx =
            m_pkAdapterDesc->GetIndex();

        RebuildAdapterDependentInfo();

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiD3D10RendererDesc::BuildDisplayList()
{
    m_kDisplays.RemoveAll();

    if (!m_pkAdapterDesc)
        return false;

    const unsigned int m_iOutputCount = m_pkAdapterDesc->GetOutputCount();
    for (unsigned int i = 0; i < m_iOutputCount; ++i)
    {
        const NiD3D10OutputDesc* pkOutput =
            m_pkAdapterDesc->GetOutputDesc(i);
        NIASSERT(pkOutput);

        m_kDisplays.Add(pkOutput);
    }

    return m_kDisplays.GetSize() > 0;
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::GetDisplayList(
    NiTObjectArray<NiFixedString>& kList)
{
    const unsigned int uiOutputCount = m_kDisplays.GetSize();
    for (unsigned int uiOutputIdx = 0;
        uiOutputIdx < uiOutputCount;
        ++uiOutputIdx)
    {
        const NiD3D10OutputDesc* pkOutput = m_kDisplays[uiOutputIdx];

        const DXGI_OUTPUT_DESC* pkDXGIOutputDesc = pkOutput->GetDesc();
        if (!pkDXGIOutputDesc)
            continue;

        char acDescr[32];
        ConvertWCHARToCHAR(acDescr, pkDXGIOutputDesc->DeviceName, 32);

        kList.Add(NiFixedString(acDescr));
    }
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::SelectDisplay(unsigned int uiIdx)
{
    if (uiIdx >= m_kDisplays.GetSize())
        return;

    m_pkOutputDesc = m_kDisplays[uiIdx];

    GetSettings()->m_uiD3D10OutputIdx = uiIdx;

    GetDefaultDevice();
}
//---------------------------------------------------------------------------
unsigned int NiD3D10RendererDesc::GetDefaultDisplay()
{
    if (GetSettings()->m_uiD3D10OutputIdx >= m_kDisplays.GetSize())
        GetSettings()->m_uiD3D10OutputIdx = m_kDisplays.GetSize() - 1;

    return GetSettings()->m_uiD3D10OutputIdx;
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::SelectDevice(bool bIsRefDevice)
{
    if (GetSettings()->m_bNVPerfHUD && m_bSupportsNVPerfHUD)
        bIsRefDevice = true;

    if (!IsDeviceAvailable(bIsRefDevice))
        bIsRefDevice = !bIsRefDevice;

    if (bIsRefDevice)
        m_pkDeviceDesc = m_pkAdapterDesc->GetRefDevice();
    else
        m_pkDeviceDesc = m_pkAdapterDesc->GetHWDevice();

    if (!GetSettings()->m_bNVPerfHUD || !m_bSupportsNVPerfHUD)
        GetSettings()->m_bRefRast = bIsRefDevice;

    RebuildDeviceDependentInfo();
}
//---------------------------------------------------------------------------
bool NiD3D10RendererDesc::BuildResolutionList()
{
    m_kModes.RemoveAll();

    const DXGI_MODE_DESC* pkDXGIModeDesc
        = m_pkOutputDesc->GetDisplayModeArray();

    for (unsigned int i = 0; i < m_pkOutputDesc->GetDisplayModeCount(); i++)
    {
        const DXGI_MODE_DESC* pkMode = (pkDXGIModeDesc + i);

        bool bAddToList = false;
        switch (m_eResolutionFilter)
        {
        case RESFILTER_ALL:
            bAddToList = true;
            break;

        case RESFILTER_NORMAL:
            bAddToList = !IsWideScreenMode(pkMode);
            break;

        case RESFILTER_WIDE:
            bAddToList = IsWideScreenMode(pkMode);
            break;
        }

        if (pkMode->Width < GetSettings()->m_uiMinScreenWidth ||
            pkMode->Height < GetSettings()->m_uiMinScreenHeight)
        {
            bAddToList = false;
        }

        // Filter resolution according to aspect ratio selection
        if (!bAddToList)
            continue;

        // Add mode to list if it is not already there
        for (unsigned int j = 0; j < m_kModes.GetSize(); j++)
        {
            if (m_kModes[j]->Height == pkMode->Height &&
                m_kModes[j]->Width == pkMode->Width)
            {
                bAddToList = false;
                break;
            }
        }

        if (bAddToList)
            m_kModes.Add(pkMode);
    }

    return m_kModes.GetSize() > 0;
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::GetResolutionList(
    NiTObjectArray<NiFixedString>& kList)
{
    char acStr[256];

    const unsigned int uiModesCount = m_kModes.GetSize();
    for (unsigned int uiModeIdx = 0; uiModeIdx < uiModesCount; ++uiModeIdx)
    {
        const DXGI_MODE_DESC* pkMode = m_kModes[uiModeIdx];

        NiSprintf(acStr, 256, "%u x %u", pkMode->Width,
            pkMode->Height);

        kList.Add(NiFixedString(acStr));
    }
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::SelectResolution(unsigned int uiResIdx)
{
    GetSettings()->m_uiScreenWidth = m_kModes[uiResIdx]->Width;
    GetSettings()->m_uiScreenHeight = m_kModes[uiResIdx]->Height;
    GetSettings()->m_kD3D10Refresh = m_kModes[uiResIdx]->RefreshRate;

    BuildRTFormatsList();
    GetDefaultRTFormat();
};
//---------------------------------------------------------------------------
unsigned int NiD3D10RendererDesc::GetDefaultResolutionIdx()
{
    int iWidth = GetSettings()->m_uiScreenWidth;
    int iHeight = GetSettings()->m_uiScreenHeight;

    // Difference between requested and current modes - deltaX * deltaY
    int iMinDiff = abs((int)m_kModes[0]->Width - iWidth) *
        abs((int)m_kModes[0]->Height - iHeight);
    unsigned int uiIdx = 0;
    // Find lowest difference
    for (unsigned i = 1; i < m_kModes.GetSize(); i++)
    {
        int iDiff = abs((int)m_kModes[i]->Width - iWidth) *
            abs((int)m_kModes[i]->Height - iHeight);
        if (iDiff < iMinDiff)
        {
            iMinDiff = iDiff;
            uiIdx = i;
        }
    }

    // Save found res
    SelectResolution(uiIdx);

    return uiIdx;
}
//---------------------------------------------------------------------------
bool NiD3D10RendererDesc::BuildMultisamplesList()
{
    m_kMultisamples.RemoveAll();

    unsigned int uiHighestMultisampleCount = 
        m_pkDeviceDesc->GetHighestMultisampleCount();

    for (unsigned int i = 0; i < uiHighestMultisampleCount; i++)
    {
        unsigned int uiQualityLevels = 0;

        if (m_pkDeviceDesc->GetMultisampleSupport(i, uiQualityLevels))
        {
            for (unsigned int j = 0; j < uiQualityLevels; j++)
                m_kMultisamples.Add((i << 16) + j);
        }
    }

    // Add default multisample if none found
    if (!m_kMultisamples.GetSize())
        m_kMultisamples.Add(1 << 16);

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10RendererDesc::GetDefaultMultisample()
{
    // If desired count or quality is higher than available, update settings
    // to maximum supported count or quality
    unsigned int uiDesiredCount = GetSettings()->m_uiD3D10MultisampleCount;
    unsigned int uiHighestCount =
        m_pkDeviceDesc->GetHighestMultisampleCount();
    if (uiDesiredCount > uiHighestCount)
        uiDesiredCount = uiHighestCount;

    unsigned int uiDesiredQuality =
        GetSettings()->m_uiD3D10MultisampleQuality;
    unsigned int uiQualityLevels = 0;
    m_pkDeviceDesc->GetMultisampleSupport(uiDesiredCount, uiQualityLevels);
    if (uiDesiredQuality > uiQualityLevels - 1)
        uiDesiredQuality = uiQualityLevels - 1;

    // Multisamples count must be 1 or more
    if (!uiDesiredCount)
        uiDesiredCount = 1;

    unsigned int uiMultisampleIdx = 0;
    for (unsigned int i = 0; i < m_kMultisamples.GetSize(); i++)
    {
        // If we found requested multisample, store its index and exit.
        if ((m_kMultisamples[i] >> 16) == uiDesiredCount &&
            (m_kMultisamples[i] & 0xffff) == uiDesiredQuality)
        {
            uiMultisampleIdx = i;
            break;
        }
    }

    GetSettings()->m_uiD3D10MultisampleCount = uiDesiredCount;
    GetSettings()->m_uiD3D10MultisampleQuality = uiDesiredQuality;

    return uiMultisampleIdx;
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::SelectMultisample(unsigned int uiMSampleIdx)
{
    if (uiMSampleIdx >= m_kMultisamples.GetSize())
        uiMSampleIdx = m_kMultisamples.GetSize() - 1;

    GetSettings()->m_uiD3D10MultisampleCount =
        m_kMultisamples[uiMSampleIdx] >> 16;
    GetSettings()->m_uiD3D10MultisampleQuality =
        m_kMultisamples[uiMSampleIdx] & 0xffff;
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::GetMultisampleModeList(
    NiTObjectArray<NiFixedString>& kList)
{
    for (unsigned int i = 0; i < m_kMultisamples.GetSize(); i++)
    {
        char acStr[256];
        unsigned int uiCount = m_kMultisamples[i] >> 16;
        unsigned int uiQuality = m_kMultisamples[i] & 0xffff;

        if (uiCount == 1)
        {
            NIASSERT(uiQuality == 0);
            NiSprintf(acStr, sizeof(acStr), "No Multisampling");
        }
        else
        {
            bool bMultipleQualityLevels = (uiQuality > 0 || 
                (i + 1< m_kMultisamples.GetSize() && 
                (uiCount == m_kMultisamples[i + 1] >> 16)));
            if (bMultipleQualityLevels)
            {
                NiSprintf(
                    acStr,
                    sizeof(acStr),
                    "%i Multisamples, Level %i",
                    uiCount,
                    uiQuality);
            }
            else
            {
                NiSprintf(acStr, sizeof(acStr), "%i Multisamples", uiCount);
            }

        }
        kList.Add(acStr);
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RendererDesc::BuildRTFormatsList()
{
    m_kRTFormats.RemoveAll();

    unsigned int uiDisplayModeCount = m_pkOutputDesc->GetDisplayModeCount();
    if (!uiDisplayModeCount)
        return false;

    const DXGI_MODE_DESC* pkDXGIModeDesc
        = m_pkOutputDesc->GetDisplayModeArray();
    if (!pkDXGIModeDesc)
        return false;

    unsigned int uiMaxUsedCount = 0;

    for (unsigned int i = 0; i < uiDisplayModeCount; i++)
    {
        if (GetSettings()->m_uiScreenWidth == (pkDXGIModeDesc + i)->Width &&
            GetSettings()->m_uiScreenHeight == (pkDXGIModeDesc + i)->Height)
        {
            unsigned int uiModeIndx = uiMaxUsedCount;
            for (unsigned int j=0; j < uiMaxUsedCount; j++)
            {
                if (m_kRTFormats[j] == (pkDXGIModeDesc + i)->Format)
                {
                    uiModeIndx = j;
                    break;
                }
            }
            if (uiModeIndx == uiMaxUsedCount)
            {
                m_kRTFormats.Add((pkDXGIModeDesc + i)->Format);
                uiMaxUsedCount++;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10RendererDesc::GetDefaultRTFormat()
{
    for (unsigned int i = 0; i < m_kRTFormats.GetSize(); i++)
    {
        if (m_kRTFormats[i] == GetSettings()->m_eD3D10RTFormat)
            return i;
    }

    // If last selected/loaded RT format is unavailable, default it to first
    GetSettings()->m_eD3D10RTFormat = m_kRTFormats[0];
    return GetSettings()->m_eD3D10RTFormat;
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::SelectRTFormat(unsigned int uiRTFmtIdx)
{
    // Exit if invalid index specified
    if (uiRTFmtIdx >= m_kRTFormats.GetSize())
        return;

    GetSettings()->m_eD3D10RTFormat = m_kRTFormats[uiRTFmtIdx];
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::GetRenderTargetFormatList(
    NiTObjectArray<NiFixedString>& kList)
{
    const unsigned int uiFormatsCount = m_kRTFormats.GetSize();
    for (unsigned int uiFmtIdx = 0; uiFmtIdx < uiFormatsCount; ++uiFmtIdx)
    {
        const char* pcFormatName =
            NiD3D10PixelFormat::GetFormatName(m_kRTFormats[uiFmtIdx], false);
        if (pcFormatName)
            kList.Add(NiFixedString(pcFormatName));
    }
}
//---------------------------------------------------------------------------
// Depth buffer format functions
//---------------------------------------------------------------------------
bool NiD3D10RendererDesc::BuildDSFormatsList()
{
    // Selecting D3D10 depth buffer format is not supported for now
    m_kDSFormats.RemoveAll();

    // Check stencil settings before adding non-stencil-capable formats.
    if (!GetSettings()->m_bStencil)
        m_kDSFormats.Add(DXGI_FORMAT_D16_UNORM);
    m_kDSFormats.Add(DXGI_FORMAT_D24_UNORM_S8_UINT);
    m_kDSFormats.Add(DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
    if (!GetSettings()->m_bStencil)
        m_kDSFormats.Add(DXGI_FORMAT_D32_FLOAT);


    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10RendererDesc::GetDefaultDSFormat()
{
    for (unsigned int i = 0; i < m_kDSFormats.GetSize(); i++)
    {
        if (m_kDSFormats[i] == GetSettings()->m_eD3D10DSFormat)
            return i;
    }

    // If last selected/loaded DS format is unavailable, default it to first
    GetSettings()->m_eD3D10DSFormat = m_kDSFormats[0];
    return GetSettings()->m_eD3D10DSFormat;
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::SelectDSFormat(unsigned int uiDSFmtIdx)
{
    // Exit if invalid index specified
    if (uiDSFmtIdx >= m_kDSFormats.GetSize())
        return;

    GetSettings()->m_eD3D10DSFormat = m_kDSFormats[uiDSFmtIdx];

    BuildMultisamplesList();
    GetDefaultMultisample();
}
//---------------------------------------------------------------------------
void NiD3D10RendererDesc::GetDepthSurfaceFormatList(
    NiTObjectArray<NiFixedString>& kList)
{
    const unsigned int uiFormatsCount = m_kDSFormats.GetSize();
    for (unsigned int uiFmtIdx = 0; uiFmtIdx < uiFormatsCount; ++uiFmtIdx)
    {
        const char* pcFormatName =
            NiD3D10PixelFormat::GetFormatName(m_kDSFormats[uiFmtIdx], false);
        if (pcFormatName)
            kList.Add(NiFixedString(pcFormatName));
    }
}
//---------------------------------------------------------------------------
NiRendererPtr NiD3D10RendererDesc::CreateD3D10Renderer(NiWindowRef pWnd)
{
    // Pointer to creation parameters
    NiRendererSettings* pkRendererSettings =
        NiRendererSettings::GetInstance();

    NiD3D10RendererPtr spRenderer;
    NiD3D10Renderer::CreationParameters kParams(pWnd);

    // Add debug flags in debug build
    kParams.m_uiCreateFlags =
#if defined(NIDEBUG)
        NiD3D10Renderer::CREATE_DEVICE_DEBUG |
        NiD3D10Renderer::CREATE_DEVICE_SWITCH_TO_REF;
#else //#if defined(NIDEBUG)
        0;
#endif //#if defined(NIDEBUG)

    // Set single-threaded flag if application requests it
    if (!pkRendererSettings->m_bMultiThread)
    {
        kParams.m_uiCreateFlags |=
            NiD3D10Renderer::CREATE_DEVICE_SINGLETHREADED;
    }

    // Select HAL or Ref device
    if (pkRendererSettings->m_bRefRast)
        kParams.m_eDriverType = NiD3D10Renderer::DRIVER_REFERENCE;
    else
        kParams.m_eDriverType = NiD3D10Renderer::DRIVER_HARDWARE;

    // Set depth buffer parameters
    kParams.m_bCreateDepthStencilBuffer = true;
    kParams.m_eDepthStencilFormat = pkRendererSettings->m_eD3D10DSFormat;

    // Set adapter index
    if (pkRendererSettings->m_bNVPerfHUD)
    {
        // If NVPerfHUD selected, always select ref device
        kParams.m_uiAdapterIndex =
            pkRendererSettings->m_uiNVPerfHUDAdapterIdx;
        kParams.m_eDriverType = NiD3D10Renderer::DRIVER_REFERENCE;
    }
    else
    {
        kParams.m_uiAdapterIndex = pkRendererSettings->m_uiAdapterIdx;
    }

    // Set display (output) index
    kParams.m_uiOutputIndex = pkRendererSettings->m_uiD3D10OutputIdx;

    // Set multisample parameters
    kParams.m_kSwapChain.SampleDesc.Count =
        pkRendererSettings->m_uiD3D10MultisampleCount;
    kParams.m_kSwapChain.SampleDesc.Quality =
        pkRendererSettings->m_uiD3D10MultisampleQuality;

    // Select fullscreen or windowed mode
    kParams.m_kSwapChain.Windowed = !pkRendererSettings->m_bFullscreen;

    // Select screen parameters (resolution, format, refresh, etc)
    DXGI_MODE_DESC kModeDesc;
    kModeDesc.Width = pkRendererSettings->m_uiScreenWidth;
    kModeDesc.Height = pkRendererSettings->m_uiScreenHeight;
    kModeDesc.Format = pkRendererSettings->m_eD3D10RTFormat;
    kModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    kModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    kModeDesc.RefreshRate = pkRendererSettings->m_kD3D10Refresh;
    kParams.m_kSwapChain.BufferDesc = kModeDesc;

    // And, finally, call renderer creation function
    NiD3D10Renderer::Create(kParams, spRenderer);

    // Set vsync if renderer has created
    if (spRenderer)
        spRenderer->SetSyncInterval(pkRendererSettings->m_bVSync ? 1 : 0);

    return NiSmartPointerCast(NiRenderer, spRenderer);
}
//---------------------------------------------------------------------------
