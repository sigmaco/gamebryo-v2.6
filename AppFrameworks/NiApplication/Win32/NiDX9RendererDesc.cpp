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
#include "NiDX9RendererDesc.h"

#ifdef NIDX9RENDERER_IMPORT
#pragma comment(lib, "NiDX9Renderer" NI_DLL_SUFFIX ".lib")
#else   // #ifdef NIDX9RENDERER_IMPORT
#pragma comment(lib, "NiDX9Renderer.lib")
#endif  // #ifdef NIDX9RENDERER_IMPORT

//---------------------------------------------------------------------------
NiDX9RendererDesc::NiDX9RendererDesc() :
    m_pkSystemDesc(NULL),
    m_pkAdapterDesc(NULL),
    m_pkDeviceDesc(NULL),
    m_bSupportsNVPerfHUD(false),
    m_eResolutionFilter(RESFILTER_ALL)
{
}

//---------------------------------------------------------------------------
// Initialization
//---------------------------------------------------------------------------
bool NiDX9RendererDesc::Initialize()
{
    // Initialize adapter, device, etc. pointers. If function returns false,
    // it indicates that something goes wrong (maybe, Direct3D is not
    // supported on that system) and no class methods should be used.

    if (!GetSettings())
    {
        NiMessageBox(
            "Error in NiDX9RendererDesc::Initialize()\n"
            "NULL pointer to NiRendererSettings class.\n"
            "This class must be instantiated before\n"
            "performing any calls to NiSettingsDialog",
            "NiDX9RendererDesc Error");
        return 0;
    }

    m_pkSystemDesc = NiDX9Renderer::GetSystemDesc();
    if (!m_pkSystemDesc)
    {
        NiMessageBox(
            "Error in NiDX9RendererDesc::Initialize()\n"
            "NULL pointer to NiDX9SystemDesc structure.\n"
            "Maybe, Direct3D 9 is not supported on this system?",
            "NiDX9RendererDesc Error");
        return false;
    }

    if (!BuildAdapterList())
    {
        NiMessageBox(
            "Error in NiDX9RendererDesc::Initialize()\n"
            "Adapter list is empty.\n"
            "Maybe, Direct3D 9 is not supported on this system?",
            "NiDX9RendererDesc Error");
        return false;
    }

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

    return true;
}

//---------------------------------------------------------------------------
// Rebuild functions. Each settings depends on some, and some depends on it.
// So, when some setting is changed, all dependents must be re-enumerated
//---------------------------------------------------------------------------
void NiDX9RendererDesc::RebuildAdapterDependentInfo()
{
    // Reinitialize functions that depend on adapter only

    // Update device. This fn will call RebuildDeviceDependentInfo()
    GetDefaultDevice();
}

//---------------------------------------------------------------------------
void NiDX9RendererDesc::RebuildDeviceDependentInfo()
{
    // Reinitialize functions that depend on adapter and device only

    // Update fullscreen. This fn will call RebuildWindowedDependentInfo()
    GetDefaultWindowedMode();

    BuildResolutionList();
    GetDefaultResolutionIdx();
}

//---------------------------------------------------------------------------
// Adapter functions: selection, enumeration, properties
//---------------------------------------------------------------------------
bool NiDX9RendererDesc::BuildAdapterList()
{
    // Prepare adapter list for view. This list is needed because view part
    // knows only indexes in list with adapter names, not real indexes
    m_kAdapters.RemoveAll();
    m_bSupportsNVPerfHUD = false;

    const NiDX9AdapterDesc* pkPerfHUDAdapter;
    for (unsigned int i = 0; i < m_pkSystemDesc->GetAdapterCount(); i++)
    {
        const NiDX9AdapterDesc* pkAdapter = m_pkSystemDesc->GetAdapter(i);
        if (!pkAdapter)
            return false;

        // Check NVPerfHUD adapter
        NiFixedString kAdapterName = pkAdapter->GetStringDescription();
        if (kAdapterName.ContainsNoCase("PerfHUD"))
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
            pkPerfHUDAdapter->GetAdapterIndex();
    }

    return m_kAdapters.GetSize() > 0;
}

//---------------------------------------------------------------------------
void NiDX9RendererDesc::GetAdapterList(NiTObjectArray<NiFixedString>& kList)
{
    // Fill adapter names array for view layer
    unsigned int uiAdapterCount = m_kAdapters.GetSize();
    // NVPerfHUD adapter is last in list and won't be shown to user
    if (m_bSupportsNVPerfHUD)
        uiAdapterCount--;

    for (unsigned int i = 0; i < uiAdapterCount; i++)
    {
        const char* pcAdapterName = m_kAdapters[i]->GetStringDescription();

        // In some cases adapter name string may be empty (remote desktop,
        // non-DX9 adapter), so we fill string with some visible info
        if (!pcAdapterName || !*pcAdapterName)
            pcAdapterName = "<Unknown adapter>";

        kList.Add(NiFixedString(pcAdapterName));
    }
}

//---------------------------------------------------------------------------
unsigned int NiDX9RendererDesc::GetDefaultAdapter()
{
    unsigned int uiAdapterCount = m_kAdapters.GetSize();

    if (GetSettings()->m_bNVPerfHUD && m_bSupportsNVPerfHUD)
        uiAdapterCount--;

    if (GetSettings()->m_uiAdapterIdx >= uiAdapterCount)
        GetSettings()->m_uiAdapterIdx = uiAdapterCount - 1;

    return GetSettings()->m_uiAdapterIdx;
}

//---------------------------------------------------------------------------
bool NiDX9RendererDesc::SelectAdapter(unsigned int uiAdaptIdx)
{
    if (uiAdaptIdx >= m_kAdapters.GetSize())
        return false;

    // Update default adapter desc pointer, save old for safety
    const NiDX9AdapterDesc* pkOldAdapter = m_pkAdapterDesc;
    m_pkAdapterDesc = m_kAdapters[uiAdaptIdx];

    if (!IsDeviceAvailable(true) && !IsDeviceAvailable(false))
    {
        NiMessageBox(
            "NiDX9AdapterDesc::GetDevice() returned\n"
            "NULL for both REF and HAL devices,\n"
            "cannot select choosen adapter.",
            "Adapter Error");

        m_pkAdapterDesc = pkOldAdapter;

        return false;
    }

    GetSettings()->m_bNVPerfHUD = false;
    GetSettings()->m_uiAdapterIdx = m_pkAdapterDesc->GetAdapterIndex();

    RebuildAdapterDependentInfo();

    return true;
}

//---------------------------------------------------------------------------
// NVPerfHUD adapter functions
//---------------------------------------------------------------------------
bool NiDX9RendererDesc::SelectPerfHUDAdapter()
{
    // PerfHUD adapter is always last in list
    if (m_bSupportsNVPerfHUD)
    {
        // Update default adapter desc pointer, save old for safety
        const NiDX9AdapterDesc* pkOldAdapter = m_pkAdapterDesc;
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
            m_pkAdapterDesc->GetAdapterIndex();

        RebuildAdapterDependentInfo();

        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
// Device functions
//---------------------------------------------------------------------------
void NiDX9RendererDesc::SelectDevice(bool bIsRefDevice)
{
    if (GetSettings()->m_bNVPerfHUD && m_bSupportsNVPerfHUD)
        bIsRefDevice = true;

    if (!IsDeviceAvailable(bIsRefDevice))
        bIsRefDevice = !bIsRefDevice;

    m_pkDeviceDesc = m_pkAdapterDesc->GetDevice(
        bIsRefDevice ? D3DDEVTYPE_REF : D3DDEVTYPE_HAL);

    NIASSERT(m_pkDeviceDesc);

    if (!GetSettings()->m_bNVPerfHUD || !m_bSupportsNVPerfHUD)
        GetSettings()->m_bRefRast = bIsRefDevice;

    RebuildDeviceDependentInfo();
}

//---------------------------------------------------------------------------
// Resolution functions
//---------------------------------------------------------------------------
bool NiDX9RendererDesc::BuildResolutionList()
{
    m_kModes.RemoveAll();

    for (unsigned int i = 0; i < m_pkAdapterDesc->GetModeCount(); i++)
    {
        const NiDX9AdapterDesc::ModeDesc* pkMode =
            m_pkAdapterDesc->GetMode(i);

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

        if (pkMode->m_uiWidth < GetSettings()->m_uiMinScreenWidth ||
            pkMode->m_uiHeight < GetSettings()->m_uiMinScreenHeight)
                bAddToList = false;

        // Filter resolution according to aspect ratio selection
        if (!bAddToList)
            continue;

        // Add mode to list if it is not already there
        for (unsigned int j = 0; j < m_kModes.GetSize(); j++)
        {
            if (m_kModes[j]->m_uiHeight == pkMode->m_uiHeight &&
                m_kModes[j]->m_uiWidth == pkMode->m_uiWidth)
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
void NiDX9RendererDesc::GetResolutionList(
    NiTObjectArray<NiFixedString>& kList)
{
    char acStr[256];

    const unsigned int uiModesCount = m_kModes.GetSize();
    for (unsigned int uiModeIdx = 0; uiModeIdx < uiModesCount; ++uiModeIdx)
    {
        const NiDX9AdapterDesc::ModeDesc* pkMode = m_kModes[uiModeIdx];

        NiSprintf(acStr, 256, "%u x %u", pkMode->m_uiWidth,
            pkMode->m_uiHeight);

        kList.Add(NiFixedString(acStr));
    }
}

//---------------------------------------------------------------------------
void NiDX9RendererDesc::SelectResolution(unsigned int uiResIdx)
{
    GetSettings()->m_uiScreenWidth = m_kModes[uiResIdx]->m_uiWidth;
    GetSettings()->m_uiScreenHeight = m_kModes[uiResIdx]->m_uiHeight;

    BuildRTFormatsList();
    SelectRTFormat(GetDefaultRTFormat());
};

//---------------------------------------------------------------------------
unsigned int NiDX9RendererDesc::GetDefaultResolutionIdx()
{
    int iWidth = GetSettings()->m_uiScreenWidth;
    int iHeight = GetSettings()->m_uiScreenHeight;

    if (!m_kModes.GetSize())
        return 0;

    // Difference between requested and current modes - deltaX * deltaY
    int iMinDiff = abs((int)m_kModes[0]->m_uiWidth - iWidth) +
        abs((int)m_kModes[0]->m_uiHeight - iHeight);
    unsigned int uiIdx = 0;
    // Find lowest difference
    for (unsigned i = 1; i < m_kModes.GetSize(); i++)
    {
        int iDiff = abs((int)m_kModes[i]->m_uiWidth - iWidth) +
            abs((int)m_kModes[i]->m_uiHeight - iHeight);
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
// Multisample functions
//---------------------------------------------------------------------------
bool NiDX9RendererDesc::BuildMultisamplesList()
{
    m_kMultisamples.RemoveAll();

    D3DFORMAT eRTFormat = NiDX9Renderer::GetD3DFormat(
        GetSettings()->m_eDX9RTFormat);
    D3DFORMAT eDSFormat = NiDX9Renderer::GetD3DFormat(
        GetSettings()->m_eDX9DSFormat);

    const NiDX9DeviceDesc::DisplayFormatInfo* pkDFInfo =
        m_pkDeviceDesc->GetFormatInfo(eRTFormat);

    // No multisampling
    m_kMultisamples.Add(NiDX9Renderer::FBMODE_DEFAULT);

    // Non-maskable levels
    const unsigned int uiQualitiesCount =
        pkDFInfo->GetMultiSampleQualityLevels(
        D3DMULTISAMPLE_NONMASKABLE,
        !GetSettings()->m_bFullscreen,
        eRTFormat,
        eDSFormat);

    unsigned int uiQuality;
    for (uiQuality = 0; uiQuality < uiQualitiesCount; ++uiQuality)
    {
        const unsigned int uiMode =
            (NiDX9Renderer::FBMODE_MULTISAMPLES_NONMASKABLE | uiQuality);

        m_kMultisamples.Add((NiDX9Renderer::FramebufferMode)uiMode);
    }

    // Maskable levels
    for (unsigned int uiNumSamples = 1; uiNumSamples < 16; ++uiNumSamples)
    {
        D3DMULTISAMPLE_TYPE eMSType =
            (D3DMULTISAMPLE_TYPE)(uiNumSamples + 1);

        if (!pkDFInfo->IsMultiSampleValid(
            !GetSettings()->m_bFullscreen,
            eMSType,
            eRTFormat,
            eDSFormat))
        {
            continue;
        }

        const unsigned int uiQualitiesCount =
            pkDFInfo->GetMultiSampleQualityLevels(
            eMSType,
            !GetSettings()->m_bFullscreen,
            eRTFormat,
            eDSFormat);

        for (uiQuality = 0; uiQuality < uiQualitiesCount; uiQuality++)
        {
            const unsigned int uiMode =
                (uiNumSamples << 16) | uiQuality;

            m_kMultisamples.Add((NiDX9Renderer::FramebufferMode)uiMode);
        }
    }

    return true;
}

//---------------------------------------------------------------------------
unsigned int NiDX9RendererDesc::GetDefaultMultisample()
{
    for (unsigned int i = 0; i < m_kMultisamples.GetSize(); i++)
    {
        if (m_kMultisamples[i] == GetSettings()->m_eDX9FBFormat)
            return i;
    }

    // If last selected/loaded DS format is unavailable, default it to first
    GetSettings()->m_eDX9FBFormat = m_kMultisamples[0];
    return GetSettings()->m_eDX9FBFormat;
}

//---------------------------------------------------------------------------
void NiDX9RendererDesc::SelectMultisample(unsigned int uiMSampleIdx)
{
    // Exit if invalid index specified
    if (uiMSampleIdx >= m_kMultisamples.GetSize())
        return;

    GetSettings()->m_eDX9FBFormat = m_kMultisamples[uiMSampleIdx];
}

//---------------------------------------------------------------------------
void NiDX9RendererDesc::GetMultisampleModeList(
    NiTObjectArray<NiFixedString>& kList)
{
    const unsigned int uiModesCount = m_kMultisamples.GetSize();
    for (unsigned int uiModeIdx = 0; uiModeIdx < uiModesCount; ++uiModeIdx)
    {
        unsigned int uiMode = m_kMultisamples[uiModeIdx];

        // Decoding mode
        char acStr[256];
        if (uiMode == NiDX9Renderer::FBMODE_DEFAULT)
        {
            NiSprintf(acStr, 256, "No Multisampling");
        }
        else if (uiMode & NiDX9Renderer::FBMODE_MULTISAMPLES_NONMASKABLE)
        {
            // Non-maskable mode
            unsigned int uiLevel =
                uiMode & NiDX9Renderer::FBMODE_QUALITY_MASK;
            NiSprintf(
                acStr,
                sizeof(acStr),
                "Nonmaskable, Level %u",
                uiLevel);
        }
        else if (uiMode & (0xFF << 16))
        {
            // Maskable mode
            unsigned int uiNumSamples = (uiMode & (0xFF << 16)) >> 16;
            unsigned int uiLevel =
              uiMode & NiDX9Renderer::FBMODE_QUALITY_MASK;

            bool bMultipleQualityLevels = (uiLevel > 0 || 
                (uiModeIdx + 1 < m_kMultisamples.GetSize() && 
                (uiNumSamples == m_kMultisamples[uiModeIdx + 1] >> 16)));
            if (bMultipleQualityLevels)
            {
                NiSprintf(
                    acStr,
                    sizeof(acStr),
                    "%i Multisamples, Level %i",
                    uiNumSamples + 1,
                    uiLevel);
            }
            else
            {
                NiSprintf(acStr, sizeof(acStr), 
                    "%i Multisamples", 
                    uiNumSamples + 1);
            }
        }
        else
        {
            // Unknown mode?
            continue;
        }

        kList.Add(NiFixedString(acStr));
    }
}

//---------------------------------------------------------------------------
// RT format functions
//---------------------------------------------------------------------------
bool NiDX9RendererDesc::BuildRTFormatsList()
{
    m_kRTFormats.RemoveAll();

    for (unsigned int i = 0; i < m_pkAdapterDesc->GetModeCount(); i++)
    {
        const NiDX9AdapterDesc::ModeDesc* pkMode =
            m_pkAdapterDesc->GetMode(i);

        if (GetSettings()->m_uiScreenWidth != pkMode->m_uiWidth ||
            GetSettings()->m_uiScreenHeight != pkMode->m_uiHeight)
        {
            continue;
        };

        bool bAddFormat = true;
        for (unsigned j = 0; j < m_kRTFormats.GetSize(); j++)
        {
            if (pkMode->m_eD3DFormat == m_kRTFormats[j])
            {
                bAddFormat = false;
                break;
            };
        }

        if (bAddFormat)
        {
            m_kRTFormats.Add(
                NiDX9Renderer::GetNiFBFormat(pkMode->m_eD3DFormat));
        }
    }

    // If list is empty for some reason, add one default entry
    if (!m_kRTFormats.GetSize())
        m_kRTFormats.Add(NiDX9Renderer::FBFMT_UNKNOWN);

    return true;
}

//---------------------------------------------------------------------------
unsigned int NiDX9RendererDesc::GetDefaultRTFormat()
{
    for (unsigned int i = 0; i < m_kRTFormats.GetSize(); i++)
    {
        if (m_kRTFormats[i] == GetSettings()->m_eDX9RTFormat)
            return i;
    }

    // If last selected/loaded RT format is unavailable, default it to first
    GetSettings()->m_eDX9RTFormat = m_kRTFormats[0];
    return GetSettings()->m_eDX9RTFormat;
}

//---------------------------------------------------------------------------
void NiDX9RendererDesc::SelectRTFormat(unsigned int uiRTFmtIdx)
{
    // Exit if invalid index specified
    if (uiRTFmtIdx >= m_kRTFormats.GetSize())
        return;

    GetSettings()->m_eDX9RTFormat = m_kRTFormats[uiRTFmtIdx];

    // Rebuild dependent lists
    BuildDSFormatsList();
    SelectDSFormat(GetDefaultDSFormat());
    BuildMultisamplesList();
    SelectMultisample(GetDefaultMultisample());
}

//---------------------------------------------------------------------------
const char* NiDX9RendererDesc::GetRTFormatName(
    NiDX9Renderer::FrameBufferFormat eFormat) const
{
    char* pcName = NULL;
    switch (eFormat)
    {
    case NiDX9Renderer::FBFMT_UNKNOWN:
        pcName = "Unknown";
        break;

    case NiDX9Renderer::FBFMT_R8G8B8:
        pcName = "R8G8B8";
        break;

    case NiDX9Renderer::FBFMT_A8R8G8B8:
        pcName = "A8R8G8B8";
        break;

    case NiDX9Renderer::FBFMT_X8R8G8B8:
        pcName = "X8R8G8B8";
        break;

    case NiDX9Renderer::FBFMT_R5G6B5:
        pcName = "R5G6B5";
        break;

    case NiDX9Renderer::FBFMT_X1R5G5B5:
        pcName = "X1R5G5B5";
        break;

    case NiDX9Renderer::FBFMT_A1R5G5B5:
        pcName = "A1R5G5B5";
        break;

    case NiDX9Renderer::FBFMT_A4R4G4B4:
        pcName = "A4R4G4B4";
        break;

    case NiDX9Renderer::FBFMT_R3G3B2:
        pcName = "R3G3B2";
        break;

    case NiDX9Renderer::FBFMT_A8:
        pcName = "A8";
        break;

    case NiDX9Renderer::FBFMT_A8R3G3B2:
        pcName = "A8R3G3B2";
        break;

    case NiDX9Renderer::FBFMT_X4R4G4B4:
        pcName = "X4R4G4B4";
        break;

    case NiDX9Renderer::FBFMT_R16F:
        pcName = "R16F";
        break;

    case NiDX9Renderer::FBFMT_G16R16F:
        pcName = "G16R16F";
        break;

    case NiDX9Renderer::FBFMT_A16B16G16R16F:
        pcName = "A16B16G16R16F";
        break;

    case NiDX9Renderer::FBFMT_R32F:
        pcName = "R32F";
        break;

    case NiDX9Renderer::FBFMT_G32R32F:
        pcName = "G32R32F";
        break;

    case NiDX9Renderer::FBFMT_A32B32G32R32F:
        pcName = "A32B32G32R32F";
        break;

    default:
        {
            // Unknown format?
        }
    }

    return pcName;
}

//---------------------------------------------------------------------------
void NiDX9RendererDesc::GetRenderTargetFormatList(
    NiTObjectArray<NiFixedString>& kList)
{
    const unsigned int uiFormatsCount = m_kRTFormats.GetSize();
    for (unsigned int uiFmtIdx = 0; uiFmtIdx < uiFormatsCount; ++uiFmtIdx)
    {
        const char* pcFormatName = GetRTFormatName(m_kRTFormats[uiFmtIdx]);
        if (pcFormatName)
            kList.Add(NiFixedString(pcFormatName));
    }
}

//---------------------------------------------------------------------------
// Depth buffer format functions
//---------------------------------------------------------------------------
bool NiDX9RendererDesc::BuildDSFormatsList()
{
    m_kDSFormats.RemoveAll();
    // Depth-Stencil Surface format list
    const int iDSFmtCount = 7;
    const NiDX9Renderer::DepthStencilFormat aDSFmtList[iDSFmtCount] =
    {
        NiDX9Renderer::DSFMT_D15S1,
        NiDX9Renderer::DSFMT_D16,
        NiDX9Renderer::DSFMT_D16_LOCKABLE,
        NiDX9Renderer::DSFMT_D24X8,
        NiDX9Renderer::DSFMT_D24S8,
        NiDX9Renderer::DSFMT_D24X4S4,
        NiDX9Renderer::DSFMT_D32
    };

    // Current RT fromat
    D3DFORMAT eD3DRTFmt = NiDX9Renderer::GetD3DFormat(
        GetSettings()->m_eDX9RTFormat);

    const NiDX9DeviceDesc::DisplayFormatInfo* pkInfo =
        m_pkDeviceDesc->GetFormatInfo(eD3DRTFmt);

    // Get all possible DS formats for it
    for (unsigned int uiDSFmtIdx = 0;
        uiDSFmtIdx < iDSFmtCount;
        uiDSFmtIdx++)
    {
        NiDX9Renderer::DepthStencilFormat eDSFmt =
            aDSFmtList[uiDSFmtIdx];
        D3DFORMAT eD3DDSFmt = NiDX9Renderer::GetD3DFormat(eDSFmt);

        if (pkInfo->IsDepthStencilValid(
            !GetSettings()->m_bFullscreen,
            eD3DRTFmt,
            eD3DDSFmt))
        {
            // If app requests stencil buffer, list only fmts, supporting it
            if (!GetSettings()->m_bStencil ||
                IsDepthFormatSupportsStencil(eDSFmt))
            {
                m_kDSFormats.Add(eDSFmt);
            }
        }
    }
    // If list is empty for some reason, add one default entry
    if (!m_kDSFormats.GetSize())
        m_kDSFormats.Add(NiDX9Renderer::DSFMT_UNKNOWN);

    return true;
}

//---------------------------------------------------------------------------
unsigned int NiDX9RendererDesc::GetDefaultDSFormat()
{
    for (unsigned int i = 0; i < m_kDSFormats.GetSize(); i++)
    {
        if (m_kDSFormats[i] == GetSettings()->m_eDX9DSFormat)
            return i;
    }

    // If last selected/loaded DS format is unavailable, default it to first
    GetSettings()->m_eDX9DSFormat = m_kDSFormats[0];
    return GetSettings()->m_eDX9DSFormat;
}

//---------------------------------------------------------------------------
void NiDX9RendererDesc::SelectDSFormat(unsigned int uiDSFmtIdx)
{
    // Exit if invalid index specified
    if (uiDSFmtIdx >= m_kDSFormats.GetSize())
        return;

    GetSettings()->m_eDX9DSFormat = m_kDSFormats[uiDSFmtIdx];

    BuildMultisamplesList();
    GetDefaultMultisample();
}

//---------------------------------------------------------------------------
bool NiDX9RendererDesc::IsDepthFormatSupportsStencil(
    NiDX9Renderer::DepthStencilFormat eFormat) const
{
    switch (eFormat)
    {
    case NiDX9Renderer::DSFMT_D15S1:
    case NiDX9Renderer::DSFMT_D24S8:
    case NiDX9Renderer::DSFMT_D24X4S4:
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
const char* NiDX9RendererDesc::GetDSFormatName(
    NiDX9Renderer::DepthStencilFormat eFormat) const
{
    char* pcName = NULL;

    switch (eFormat)
    {
    case NiDX9Renderer::DSFMT_UNKNOWN:
        pcName = "Unknown";
        break;

    case NiDX9Renderer::DSFMT_D16_LOCKABLE:
        pcName = "D16 Lockable";
        break;

    case NiDX9Renderer::DSFMT_D32:
        pcName = "D32";
        break;

    case NiDX9Renderer::DSFMT_D15S1:
        pcName = "D15S1";
        break;

    case NiDX9Renderer::DSFMT_D24S8:
        pcName = "D24S8";
        break;

    case NiDX9Renderer::DSFMT_D16:
        pcName = "D16";
        break;

    case NiDX9Renderer::DSFMT_D24X8:
        pcName = "D24X8";
        break;

    case NiDX9Renderer::DSFMT_D24X4S4:
        pcName = "D24X4S4";
        break;

    default:
        // Unknown format?
        break;
    }

    return pcName;
}

//---------------------------------------------------------------------------
void NiDX9RendererDesc::GetDepthSurfaceFormatList(
    NiTObjectArray<NiFixedString>& kList)
{
    const unsigned int uiFormatsCount = m_kDSFormats.GetSize();
    for (unsigned int uiFmtIdx = 0; uiFmtIdx < uiFormatsCount; ++uiFmtIdx)
    {
        const char* pcFormatName = GetDSFormatName(m_kDSFormats[uiFmtIdx]);
        if (pcFormatName)
            kList.Add(NiFixedString(pcFormatName));
    }
}

//---------------------------------------------------------------------------
// Renderer creation routine
//---------------------------------------------------------------------------
NiRendererPtr NiDX9RendererDesc::CreateDX9Renderer(
    NiWindowRef pWndDevice,
    NiWindowRef pWndFocus)
{
    // Pointer to creation parameters
    NiRendererSettings* pkRendererSettings =
        NiRendererSettings::GetInstance();

    unsigned int eFlags = NiDX9Renderer::USE_NOFLAGS;
    if (pkRendererSettings->m_bStencil)
        eFlags |= NiDX9Renderer::USE_STENCIL;
    if (pkRendererSettings->m_bMultiThread)
        eFlags |= NiDX9Renderer::USE_MULTITHREADED;
    if (pkRendererSettings->m_bUse16Bit)
        eFlags |= NiDX9Renderer::USE_16BITBUFFERS;
    if (pkRendererSettings->m_bFullscreen)
        eFlags |= NiDX9Renderer::USE_FULLSCREEN;

    // Set presentation interval according to vsync setting
    NiDX9Renderer::PresentationInterval ePresInt;
    if (pkRendererSettings->m_bVSync)
        ePresInt = NiDX9Renderer::PRESENT_INTERVAL_ONE;
    else
        ePresInt = NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE;

    // Select adapter and device properties
    NiDX9Renderer::DeviceDesc eDesc;
    unsigned int uiAdapterIdx;
    if (pkRendererSettings->m_bNVPerfHUD)
    {
        uiAdapterIdx = pkRendererSettings->m_uiNVPerfHUDAdapterIdx;
        // NVPerfHUD is always a REF device
        eDesc = NiDX9Renderer::DEVDESC_REF;
    }
    else
    {
        uiAdapterIdx = pkRendererSettings->m_uiAdapterIdx;
        // Set REF/HAL device with SW/HW vertex processing
        if (pkRendererSettings->m_bRefRast)
        {
            switch (pkRendererSettings->m_eVertexProcessing)
            {
            case NiRendererSettings::VERTEX_HARDWARE:
                eDesc = NiDX9Renderer::DEVDESC_REF_HWVERTEX;
                break;

            case NiRendererSettings::VERTEX_MIXED:
                eDesc = NiDX9Renderer::DEVDESC_REF_MIXEDVERTEX;
                break;

            default:    // VERTEX_SOFTWARE - default if unspecified for REF
                eDesc = NiDX9Renderer::DEVDESC_REF;
                break;
            }
        }
        else
        {
            if (pkRendererSettings->m_bPureDevice)
            {
                eDesc = NiDX9Renderer::DEVDESC_PURE;
            }
            else
            {
                switch (pkRendererSettings->m_eVertexProcessing)
                {
                case NiRendererSettings::VERTEX_MIXED:
                    eDesc = NiDX9Renderer::DEVDESC_HAL_MIXEDVERTEX;
                    break;

                case NiRendererSettings::VERTEX_SOFTWARE:
                    eDesc = NiDX9Renderer::DEVDESC_HAL_SWVERTEX;
                    break;

                default:    // VERTEX_HARDWARE default if unspecified for HAL
                    eDesc = NiDX9Renderer::DEVDESC_HAL_HWVERTEX;
                    break;
                }
            }
        }
    }

    // And call creation function
    return NiDX9Renderer::Create(
        pkRendererSettings->m_uiScreenWidth,
        pkRendererSettings->m_uiScreenHeight,
        eFlags,
        pWndDevice,
        pWndFocus,
        uiAdapterIdx,
        eDesc,
        pkRendererSettings->m_eDX9RTFormat,
        pkRendererSettings->m_eDX9DSFormat,
        ePresInt,
        NiDX9Renderer::SWAPEFFECT_DEFAULT,
        pkRendererSettings->m_eDX9FBFormat);
}
