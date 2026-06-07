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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10SystemDesc.h"
#include "NiD3D10Error.h"
#include "NiD3D10PixelFormat.h"
#include "NiD3D10Renderer.h"

NiD3D10SystemDesc* NiD3D10SystemDesc::ms_pkSystemDesc = NULL;
NiCriticalSection NiD3D10SystemDesc::ms_kCriticalSection;

//---------------------------------------------------------------------------
void NiD3D10SystemDesc::GetSystemDesc(NiD3D10SystemDescPtr& spSystemDesc)
{
    ms_kCriticalSection.Lock();
    if (ms_pkSystemDesc)
    {
        spSystemDesc = ms_pkSystemDesc;
    }
    else
    {
        spSystemDesc = NiNew NiD3D10SystemDesc;
        ms_pkSystemDesc = spSystemDesc;
    }
    ms_kCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
NiD3D10SystemDesc::NiD3D10SystemDesc() :
    m_hDXGI(NULL),
    m_pfnCreateDXGIFactory(NULL),
    m_pkFactory(NULL),
    m_bEnumerationValid(false)
{
    if (LoadDXGI() == false)
    {
        // Error should have already been reported by this point, and
        // the library released.
        return;
    }

    if (CreateFactory() == false)
    {
        // Error should have already been reported by this point, and the
        // factory released.
        ReleaseDXGI();
        return;
    }

    if (EnumerateAdapters() == false)
    {
        // Error should have already been reported by this point, but
        // library and factory remain so enumeration can be attempted again.
        return;
    }
}
//---------------------------------------------------------------------------
NiD3D10SystemDesc::~NiD3D10SystemDesc()
{
    ms_pkSystemDesc = NULL;

    ReleaseAdapters();

    ReleaseFactory();

    ReleaseDXGI();
}
//---------------------------------------------------------------------------
bool NiD3D10SystemDesc::IsEnumerationValid() const
{
    return m_bEnumerationValid;
}
//---------------------------------------------------------------------------
bool NiD3D10SystemDesc::Enumerate()
{
    ReleaseAdapters();
    EnumerateAdapters();

    return m_bEnumerationValid;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10SystemDesc::GetAdapterCount() const
{
    return m_kAdapters.GetEffectiveSize();
}
//---------------------------------------------------------------------------
const NiD3D10AdapterDesc* NiD3D10SystemDesc::GetAdapterDesc(
    unsigned int uiIndex) const
{
    // Effective size must equal actual size, since we only either
    // add all adapters to an empty array or remove all adapters at once.
    NIASSERT(m_kAdapters.GetEffectiveSize() == m_kAdapters.GetSize());
    if (uiIndex < m_kAdapters.GetSize())
        return m_kAdapters.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
IDXGIFactory* NiD3D10SystemDesc::GetFactory() const
{
    return m_pkFactory;
}
//---------------------------------------------------------------------------
bool NiD3D10SystemDesc::LoadDXGI()
{
    ms_kCriticalSection.Lock();

    if (m_hDXGI != NULL)
    {
        ms_kCriticalSection.Unlock();
        return true;
    }

    m_hDXGI = LoadLibrary("DXGI.dll");

    if (m_hDXGI == NULL)
    {
        NiD3D10Error::ReportError(
            NiD3D10Error::NID3D10ERROR_DXGI_LIB_MISSING);
        ms_kCriticalSection.Unlock();
        return false;
    }

    m_pfnCreateDXGIFactory = (NILPCREATEDXGIFACTORY)
        GetProcAddress(m_hDXGI, "CreateDXGIFactory");
    if (m_pfnCreateDXGIFactory == NULL)
    {
        NiD3D10Error::ReportError(NiD3D10Error::NID3D10ERROR_DXGI_LIB_MISSING,
            "Library loaded but CreateDXGIFactory procedure not found; "
            "releasing library.");
        ms_kCriticalSection.Unlock();

        ReleaseDXGI();
        return false;
    }

    ms_kCriticalSection.Unlock();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10SystemDesc::ReleaseDXGI()
{
    ms_kCriticalSection.Lock();
    if (m_hDXGI)
    {
        FreeLibrary(m_hDXGI);
        m_hDXGI = NULL;
        m_pfnCreateDXGIFactory = NULL;
    }

    ms_kCriticalSection.Unlock();
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10SystemDesc::CreateFactory()
{
    NIASSERT(m_pfnCreateDXGIFactory);

    HRESULT hr = 
        m_pfnCreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pkFactory);

    if (FAILED(hr) || m_pkFactory == NULL)
    {
        const unsigned int uiBufferSize = 1024;
        char acErrorMessage[uiBufferSize];
        if (FAILED(hr))
        {
            NiSprintf(acErrorMessage, uiBufferSize, 
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiSprintf(acErrorMessage, uiBufferSize, 
                "No error message from DXGI, but factory is NULL.");
        }
        NiD3D10Error::ReportError(
            NiD3D10Error::NID3D10ERROR_FACTORY_CREATION_FAILED,
            acErrorMessage);

        ReleaseFactory();
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10SystemDesc::ReleaseFactory()
{
    if (m_pkFactory)
    {
        m_pkFactory->Release();
        m_pkFactory = NULL;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10SystemDesc::EnumerateAdapters()
{
    NIASSERT(m_kAdapters.GetEffectiveSize() == 0);
    m_bEnumerationValid = false;

    if (m_pkFactory == false)
        return false;

    unsigned int uiAdapterIndex = 0;
    for ( ; ; )
    {
        IDXGIAdapter* pkAdapter = NULL;
        HRESULT hr = 
            m_pkFactory->EnumAdapters(uiAdapterIndex, &pkAdapter);
        if (FAILED(hr))
        {
            if (hr != DXGI_ERROR_NOT_FOUND)
            {
                NiD3D10Error::ReportWarning("IDXGIFactory::EnumAdapters "
                    "failed with error HRESULT = 0x%08X.", (unsigned int)hr);
            }
            break;
        }

        NiD3D10AdapterDesc* pkAdapterDesc = NiNew NiD3D10AdapterDesc;
        pkAdapterDesc->m_pkAdapter = pkAdapter;
        pkAdapterDesc->m_uiIndex = uiAdapterIndex;
        hr = pkAdapter->GetDesc(&(pkAdapterDesc->m_kAdapterDesc));

        uiAdapterIndex++;

        if (FAILED(hr))
        {
            NiD3D10Error::ReportWarning("IDXGIAdapter::GetDesc failed with "
                "error HRESULT = 0x%08X; deleting adapter descriptor.", 
                (unsigned int)hr);

            NiDelete pkAdapterDesc;
            continue;
        }

        if (pkAdapterDesc->EnumerateDevices(m_pkFactory) == false)
        {
            NiD3D10Error::ReportWarning("NiD3D10AdapterDesc::EnumerateDevices "
                "failed; deleting adapter descriptor.");
            NiDelete pkAdapterDesc;
            continue;
        }

        if (pkAdapterDesc->EnumerateOutputs(m_pkFactory) == false)
        {
            NiD3D10Error::ReportWarning("NiD3D10AdapterDesc::EnumerateOutputs "
                "failed; deleting adapter descriptor.");
            NiDelete pkAdapterDesc;
            continue;
        }

        m_kAdapters.Add(pkAdapterDesc);
    }

    if (m_kAdapters.GetSize() > 0)
        m_bEnumerationValid = true;

    return m_bEnumerationValid;
}
//---------------------------------------------------------------------------
bool NiD3D10SystemDesc::ReleaseAdapters()
{
    m_bEnumerationValid = false;

    unsigned int uiAdapterCount = m_kAdapters.GetSize();
    for (unsigned int i = 0; i < uiAdapterCount; i++)
    {
        NiD3D10AdapterDesc* pkAdapterDesc = m_kAdapters.GetAt(i);
        NiDelete pkAdapterDesc;
        m_kAdapters.RemoveAt(i);
    }

    return true;
}
//---------------------------------------------------------------------------
NiD3D10AdapterDesc::NiD3D10AdapterDesc() :
    m_pkAdapter(NULL),
    m_pkHWDevice(NULL),
    m_pkRefDevice(NULL),
    m_uiIndex(UINT_MAX),
    m_bIsPerfHUD(false)
{
    memset(&m_kAdapterDesc, 0, sizeof(m_kAdapterDesc));
}
//---------------------------------------------------------------------------
NiD3D10AdapterDesc::~NiD3D10AdapterDesc()
{
    if (m_pkAdapter)
        m_pkAdapter->Release();

    unsigned int uiOutputCount = m_kOutputs.GetSize();
    for (unsigned int i = 0; i < uiOutputCount; i++)
    {
        NiD3D10OutputDesc* pkOutputDesc = m_kOutputs.GetAt(i);
        NiDelete pkOutputDesc;
    }

    NiDelete m_pkHWDevice;
    NiDelete m_pkRefDevice;
}
//---------------------------------------------------------------------------
const NiD3D10DeviceDesc* NiD3D10AdapterDesc::GetHWDevice() const
{
    // For PerfHUD adapter, "ref" device is actually HW.
    return (m_bIsPerfHUD ? m_pkRefDevice : m_pkHWDevice);
}
//---------------------------------------------------------------------------
const NiD3D10DeviceDesc* NiD3D10AdapterDesc::GetRefDevice() const
{
    return m_pkRefDevice;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10AdapterDesc::GetOutputCount() const
{
    return m_kOutputs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
const NiD3D10OutputDesc* NiD3D10AdapterDesc::GetOutputDesc(
    unsigned int uiIndex) const
{
    // Effective size must equal actual size, since we only either
    // add all outputs to an empty array or remove all outputs at once.
    NIASSERT(m_kOutputs.GetEffectiveSize() == m_kOutputs.GetSize());
    if (uiIndex < m_kOutputs.GetSize())
        return m_kOutputs.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
const DXGI_ADAPTER_DESC* NiD3D10AdapterDesc::GetDesc() const
{
    return &m_kAdapterDesc;
}
//---------------------------------------------------------------------------
IDXGIAdapter* NiD3D10AdapterDesc::GetAdapter() const
{
    return m_pkAdapter;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10AdapterDesc::GetIndex() const
{
    return m_uiIndex;
}
//---------------------------------------------------------------------------
bool NiD3D10AdapterDesc::IsPerfHUD() const
{
    return m_bIsPerfHUD;
}
//---------------------------------------------------------------------------
bool NiD3D10AdapterDesc::EnumerateDevices(IDXGIFactory* pkFactory)
{
    if (pkFactory == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call EnumerateDevices without "
            "a valid factory.");
        return false;
    }

    // Description is WCHAR, so wcsstr must be used instead of strstr.
    m_bIsPerfHUD = 
        (wcsstr(m_kAdapterDesc.Description, L"PerfHUD") != NULL);

    NIASSERT(m_pkHWDevice == NULL && m_pkRefDevice == NULL);

    const D3D10_DRIVER_TYPE aeDevTypeArray[] =
    { 
        D3D10_DRIVER_TYPE_HARDWARE,
        D3D10_DRIVER_TYPE_REFERENCE,
    };
    const unsigned int uiDevTypeArrayCount = 
        sizeof(aeDevTypeArray) / sizeof(*aeDevTypeArray);

    for (unsigned int i = 0; i < uiDevTypeArrayCount; i++)
    {
        D3D10_DRIVER_TYPE eType = aeDevTypeArray[i];
        IDXGIAdapter* pkAdapter = NULL;

        // PerfHUD adapter needs to be Reference, and needs the adapter to be 
        // specified.
        if (m_bIsPerfHUD && (eType != D3D10_DRIVER_TYPE_REFERENCE))
            continue;
        else if (m_bIsPerfHUD || eType == D3D10_DRIVER_TYPE_HARDWARE)
            pkAdapter = m_pkAdapter;

        ID3D10Device* pkD3DDevice = NiD3D10Renderer::CreateTempDevice(
            m_pkAdapter, eType, 0);
        if (pkD3DDevice == NULL)
            continue;

        NiD3D10DeviceDesc* pkDeviceDesc = NiNew NiD3D10DeviceDesc;
        pkDeviceDesc->m_eDeviceType = eType;
        pkDeviceDesc->m_pkAdapter = this;

        unsigned int* puiQualityDest = pkDeviceDesc->m_auiMSQualities;
        for (unsigned int j = 1; j <= D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT; j++)
        {
            HRESULT hr = pkD3DDevice->CheckMultisampleQualityLevels(
                DXGI_FORMAT_R8G8B8A8_UNORM, j, puiQualityDest);
            if (FAILED(hr))
                *puiQualityDest = 0;
            else
                pkDeviceDesc->m_uiHighestMultisampleCount = j;
            puiQualityDest++;
        }
        pkD3DDevice->Release();

        if (eType == D3D10_DRIVER_TYPE_HARDWARE)
        {
            m_pkHWDevice = pkDeviceDesc;
        }
        else
        {
            NIASSERT(eType == D3D10_DRIVER_TYPE_REFERENCE);
            m_pkRefDevice = pkDeviceDesc;
        }
    }

    return (m_pkHWDevice != NULL || m_pkRefDevice != NULL);
}
//---------------------------------------------------------------------------
bool NiD3D10AdapterDesc::EnumerateOutputs(IDXGIFactory* pkFactory)
{
    if (pkFactory == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call EnumerateOutputs without "
            "a valid factory.");
        return false;
    }

    NIASSERT(m_kOutputs.GetEffectiveSize() == 0);

    unsigned int uiOutputIndex = 0;
    for ( ; ; )
    {
        IDXGIOutput* pkOutput = NULL;
        HRESULT hr = m_pkAdapter->EnumOutputs(uiOutputIndex, &pkOutput);
        if (FAILED(hr))
        {
            if (hr != DXGI_ERROR_NOT_FOUND)
            {
                NiD3D10Error::ReportWarning("IDXGIAdapter::EnumOutputs failed "
                    "with error HRESULT = 0x%08X.", 
                    (unsigned int)hr);
            }
            break;
        }

        NiD3D10OutputDesc* pkOutputDesc = NiNew NiD3D10OutputDesc;
        pkOutputDesc->m_uiIndex = uiOutputIndex;
        pkOutputDesc->m_pkAdapter = this;
        pkOutputDesc->m_pkOutput = pkOutput;
        hr = pkOutput->GetDesc(&(pkOutputDesc->m_kOutputDesc));
        uiOutputIndex++;

        if (FAILED(hr))
        {
            NiD3D10Error::ReportWarning("IDXGIOutput::GetDesc failed with "
                "error HRESULT = 0x%08X; deleting output descriptor.", 
                (unsigned int)hr);

            NiDelete pkOutputDesc;
            continue;
        }

        //Enumerate display modes
        if (pkOutputDesc->EnumerateDisplayModes())
        {
            m_kOutputs.Add(pkOutputDesc);
        }
        else
        {
            NiD3D10Error::ReportWarning(
                "NiD3D10OutputDesc::EnumerateDisplayModes failed; "
                "deleting output descriptor.");
            NiDelete pkOutputDesc;
        }
    }
    
    return (m_kOutputs.GetSize() > 0);
}
//---------------------------------------------------------------------------
NiD3D10DeviceDesc::NiD3D10DeviceDesc() :
    m_pkAdapter(NULL),
    m_eDeviceType(D3D10_DRIVER_TYPE_NULL),
    m_uiHighestMultisampleCount(0)
{
    memset(m_auiMSQualities, 0, sizeof(m_auiMSQualities));
}
//---------------------------------------------------------------------------
NiD3D10DeviceDesc::~NiD3D10DeviceDesc()
{
    /* */
}
//---------------------------------------------------------------------------
D3D10_DRIVER_TYPE NiD3D10DeviceDesc::GetDeviceType() const
{
    return m_eDeviceType;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10DeviceDesc::GetHighestMultisampleCount() const
{
    return m_uiHighestMultisampleCount;
}
//---------------------------------------------------------------------------
bool NiD3D10DeviceDesc::GetMultisampleSupport(unsigned int uiSamples, 
    unsigned int& uiQualityLevels) const
{
    uiQualityLevels = 0;
    if (uiSamples > 0 &&
        uiSamples <= D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT)
    {
        uiQualityLevels = m_auiMSQualities[uiSamples - 1];
    }
    return (uiQualityLevels != 0);
}
//---------------------------------------------------------------------------
NiD3D10OutputDesc::NiD3D10OutputDesc() :
    m_uiIndex(0),
    m_pkAdapter(NULL),
    m_pkOutput(NULL),
    m_pkDisplayModes(NULL),
    m_uiDisplayModeCount(0)
{
    memset(&m_kOutputDesc, 0, sizeof(m_kOutputDesc));
}
//---------------------------------------------------------------------------
NiD3D10OutputDesc::~NiD3D10OutputDesc()
{
    if (m_pkOutput)
        m_pkOutput->Release();
    NiFree(m_pkDisplayModes);
}
//---------------------------------------------------------------------------
const DXGI_OUTPUT_DESC* NiD3D10OutputDesc::GetDesc() const
{
    return &m_kOutputDesc;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10OutputDesc::GetDisplayModeCount() const
{
    return m_uiDisplayModeCount;
}
//---------------------------------------------------------------------------
const DXGI_MODE_DESC* NiD3D10OutputDesc::GetDisplayModeArray() const
{
    return m_pkDisplayModes;
}
//---------------------------------------------------------------------------
IDXGIOutput* NiD3D10OutputDesc::GetOutput() const
{
    return m_pkOutput;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10OutputDesc::GetIndex() const
{
    return m_uiIndex;
}
//---------------------------------------------------------------------------
bool NiD3D10OutputDesc::EnumerateDisplayModes()
{
    NIASSERT(m_pkDisplayModes == NULL);

    const DXGI_FORMAT aeAllowedAdapterFormatArray[] = 
    {
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_R16G16B16A16_FLOAT,
        DXGI_FORMAT_R10G10B10A2_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
    };

    const unsigned int uiAllowedAdapterFormatArrayCount = 
        sizeof(aeAllowedAdapterFormatArray) / 
        sizeof(*aeAllowedAdapterFormatArray);

    unsigned int uiTotalModeCount = 0;
    unsigned int auiModeCounts[uiAllowedAdapterFormatArrayCount];

    unsigned int i = 0;
    for (; i < uiAllowedAdapterFormatArrayCount; i++)
    {
        DXGI_FORMAT eFormat = aeAllowedAdapterFormatArray[i];

        unsigned int uiModeCount = 0;

        HRESULT hr = m_pkOutput->GetDisplayModeList(eFormat, 0, &uiModeCount,
            NULL);

        if (FAILED(hr))
        {
            if (hr != DXGI_ERROR_NOT_FOUND)
            {
                NiD3D10Error::ReportWarning("IDXGIOutput::GetDisplayModeList "
                    "failed with error HRESULT = 0x%08X.", (unsigned int)hr);
            }
            continue;
        }

        uiTotalModeCount += uiModeCount;
        auiModeCounts[i] = uiModeCount;
    }

    if (uiTotalModeCount == 0)
        return false;

    m_pkDisplayModes = NiAlloc(DXGI_MODE_DESC, uiTotalModeCount);
    NIASSERT(m_pkDisplayModes);

    DXGI_MODE_DESC* pkIterator = m_pkDisplayModes;
    for (i = 0; i < uiAllowedAdapterFormatArrayCount; i++)
    {
        unsigned int uiModeCount = auiModeCounts[i];
        if (uiModeCount == 0)
            continue;

        DXGI_FORMAT eFormat = aeAllowedAdapterFormatArray[i];

        HRESULT hr = m_pkOutput->GetDisplayModeList(eFormat, 0, &uiModeCount, 
            pkIterator);

        if (FAILED(hr) || uiModeCount != auiModeCounts[i])
        {
            if (FAILED(hr))
            {
                NiD3D10Error::ReportWarning("IDXGIOutput::GetDisplayModeList "
                    "failed with error HRESULT = 0x%08X.", (unsigned int)hr);
            }
            else
            {
                NiD3D10Error::ReportWarning("IDXGIOutput::GetDisplayModeList "
                    "returned different mode counts on subsequent calls "
                    "for format %s", 
                    NiD3D10PixelFormat::GetFormatName(eFormat, false));
            }

            NiFree(m_pkDisplayModes);
            m_pkDisplayModes = NULL;
            return false;
        }

        pkIterator += uiModeCount;
    }

    m_uiDisplayModeCount = uiTotalModeCount;
    return true;
}
//---------------------------------------------------------------------------
