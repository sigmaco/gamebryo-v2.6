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

#ifndef NiD3D10RENDERERDESC_H
#define NiD3D10RENDERERDESC_H

#include "NiBaseRendererDesc.h"
#include <NiD3D10SystemDesc.h>

// Forward declarations
class NiRendererSettings;
class NiD3D10AdapterDesc;
class NiD3D10DeviceDesc;
class NiD3D10SystemDesc;
class NiD3D10OutputDesc;

// Direct3D 10 enumeration, settings selection and render creation routines
class NiD3D10RendererDesc : public NiBaseRendererDesc
{
public:
    NiD3D10RendererDesc();

    // Initialization function
    virtual bool Initialize();

    // Returns true for NiD3D10RendererDesc and false for NiDX9RendererDesc
    virtual bool IsD3D10RendererDesc();

    // Renderer creation routine
    static NiRendererPtr CreateD3D10Renderer(NiWindowRef pWnd);

    // Adapter retrieving and selection
    virtual void GetAdapterList(NiTObjectArray<NiFixedString>& kList);
    virtual bool SelectAdapter(unsigned int uiAdaptIdx);
    virtual unsigned int GetDefaultAdapter();
    virtual bool GetNVPerfHUDSupport();
    virtual bool GetDefaultNVPerfHUDSetting();
    virtual bool SelectPerfHUDAdapter();

    // Display functions (D3D10 specific)
    virtual void GetDisplayList(NiTObjectArray<NiFixedString>& kList);
    virtual void SelectDisplay(unsigned int uiIdx);
    virtual unsigned int GetDefaultDisplay();

    // Device retrieving and selection
    virtual bool IsDeviceAvailable(bool bRefDevice);
    virtual void SelectDevice(bool bRefDevice);
    virtual bool GetDefaultDevice();

    // Device retrieve / selection options functions
    virtual bool IsPureDeviceAvailable();
    virtual bool GetDefaultPureDevice();
    virtual void SelectPureDevice(bool bPure);
    virtual bool IsVetrexprocessingAvailable();
    virtual NiRendererSettings::Vertexprocessing GetDefaultVertexprocessing();
    virtual void SelectVertexprocessing (
        NiRendererSettings::Vertexprocessing eVertex);

    // Resolution functions
    virtual void SetResolutionFilter(ResolutionFilters eResFilter);
    virtual void GetResolutionList(NiTObjectArray<NiFixedString>& kList);
    virtual void SelectResolution(unsigned int uiResIdx);
    virtual unsigned int GetDefaultResolutionIdx();

    // Windowed mode functions
    virtual bool CanRenderWindowed();
    virtual void SelectWindowedMode(bool bFullscreen);
    virtual bool GetDefaultWindowedMode();

    // Depth buffer format selection functions
    virtual unsigned int GetDefaultDSFormat();
    virtual void SelectDSFormat(unsigned int uiDSFmtIdx);
    virtual void GetDepthSurfaceFormatList(
        NiTObjectArray<NiFixedString>& kList);

    // Render target format selection functions
    virtual unsigned int GetDefaultRTFormat();
    virtual void SelectRTFormat(unsigned int uiRTFmtIdx);
    virtual void GetRenderTargetFormatList(
        NiTObjectArray<NiFixedString>& kList);

    // Multisample selection functions
    virtual unsigned int GetDefaultMultisample();
    virtual void SelectMultisample(unsigned int uiMultisample);
    virtual void GetMultisampleModeList(
        NiTObjectArray<NiFixedString>& kList);

protected:
    // Pointers to current system, adapter and device descs for convenience
    NiD3D10SystemDescPtr m_spSystemDesc;
    const NiD3D10AdapterDesc* m_pkAdapterDesc;
    const NiD3D10DeviceDesc* m_pkDeviceDesc;
    const NiD3D10OutputDesc* m_pkOutputDesc;

    // List of all adapters, NVPerfHUD presence flag
    NiTPrimitiveArray<const NiD3D10AdapterDesc*> m_kAdapters;
    bool m_bSupportsNVPerfHUD;

    // List of all supported displays
    NiTPrimitiveArray<const NiD3D10OutputDesc*> m_kDisplays;

    // List of supported display modes
    NiTPrimitiveArray<const DXGI_MODE_DESC*> m_kModes;
    ResolutionFilters m_eResolutionFilter;

    // Supported multisamples array. Higher 16 bits are count, lower - quality
    NiTPrimitiveArray<unsigned int> m_kMultisamples;
    // Supported render target and depth surface formats
    NiTPrimitiveArray<DXGI_FORMAT> m_kRTFormats;
    NiTPrimitiveArray<DXGI_FORMAT> m_kDSFormats;

    // Functions, that rebuild info, dependent on adapter and device selection
    void RebuildAdapterDependentInfo();
    void RebuildDeviceDependentInfo();

    // Enumerations routines
    bool BuildAdapterList();
    bool BuildDisplayList();
    bool BuildResolutionList();
    bool BuildMultisamplesList();
    bool BuildRTFormatsList();
    bool BuildDSFormatsList();

    // Support routines
    bool IsWideScreenMode(const DXGI_MODE_DESC* pkMode) const;
};

#include "NiD3D10RendererDesc.inl"

#endif  // NiD3D10RENDERERDESC_H