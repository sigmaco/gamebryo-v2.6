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

#ifndef NID3D10SYSTEMDESC_H
#define NID3D10SYSTEMDESC_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiTArray.h>

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"

class NiD3D10DeviceDesc;
class NiD3D10OutputDesc;
class NiD3D10AdapterDesc;

NiSmartPointer(NiD3D10SystemDesc);

typedef HRESULT (WINAPI *NILPCREATEDXGIFACTORY)(REFIID, void**);

//---------------------------------------------------------------------------
class NID3D10RENDERER_ENTRY NiD3D10SystemDesc : public NiRefObject
{
public:
    static void GetSystemDesc(NiD3D10SystemDescPtr& spSystemDesc);

    bool IsEnumerationValid() const;
    bool Enumerate();

    unsigned int GetAdapterCount() const;
    const NiD3D10AdapterDesc* GetAdapterDesc(unsigned int uiIndex) const;

    IDXGIFactory* GetFactory() const;

protected:
    NiD3D10SystemDesc();
    ~NiD3D10SystemDesc();

    bool LoadDXGI();
    bool ReleaseDXGI();

    bool CreateFactory();
    bool ReleaseFactory();

    bool EnumerateAdapters();
    bool ReleaseAdapters();

    NiTPrimitiveArray<NiD3D10AdapterDesc*> m_kAdapters;

    static NiD3D10SystemDesc* ms_pkSystemDesc;
    static NiCriticalSection ms_kCriticalSection;

    HINSTANCE m_hDXGI;
    NILPCREATEDXGIFACTORY m_pfnCreateDXGIFactory;
    IDXGIFactory* m_pkFactory;

    bool m_bEnumerationValid;
};
//---------------------------------------------------------------------------
class NID3D10RENDERER_ENTRY NiD3D10AdapterDesc : public NiMemObject
{
public:
    const NiD3D10DeviceDesc* GetHWDevice() const;
    const NiD3D10DeviceDesc* GetRefDevice() const;

    unsigned int GetOutputCount() const;
    const NiD3D10OutputDesc* GetOutputDesc(unsigned int uiIndex) const;
    const DXGI_ADAPTER_DESC* GetDesc() const;

    IDXGIAdapter* GetAdapter() const;
    unsigned int GetIndex() const;

    bool IsPerfHUD() const;
    
protected:
    NiD3D10AdapterDesc();
    ~NiD3D10AdapterDesc();

    bool EnumerateDevices(IDXGIFactory* pkFactory);
    bool EnumerateOutputs(IDXGIFactory* pkFactory);

    unsigned int m_uiIndex;
    IDXGIAdapter* m_pkAdapter;

    DXGI_ADAPTER_DESC m_kAdapterDesc;

    NiTPrimitiveArray<NiD3D10OutputDesc*> m_kOutputs;

    NiD3D10DeviceDesc* m_pkHWDevice;
    NiD3D10DeviceDesc* m_pkRefDevice;

    bool m_bIsPerfHUD;

    friend class NiD3D10SystemDesc;
};
//---------------------------------------------------------------------------
class NID3D10RENDERER_ENTRY NiD3D10DeviceDesc : public NiMemObject
{
public:
    D3D10_DRIVER_TYPE GetDeviceType() const;

    unsigned int GetHighestMultisampleCount() const;
    bool GetMultisampleSupport(unsigned int uiSamples, 
        unsigned int& uiQualityLevels) const;

protected:
    NiD3D10DeviceDesc();
    ~NiD3D10DeviceDesc();

    NiD3D10AdapterDesc* m_pkAdapter;
    D3D10_DRIVER_TYPE m_eDeviceType;

    unsigned int m_auiMSQualities[D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT];
    unsigned int m_uiHighestMultisampleCount;

    friend class NiD3D10AdapterDesc;
};
//---------------------------------------------------------------------------
class NID3D10RENDERER_ENTRY NiD3D10OutputDesc : public NiMemObject
{
public:
    const DXGI_OUTPUT_DESC* GetDesc() const;
    unsigned int GetDisplayModeCount() const;
    const DXGI_MODE_DESC* GetDisplayModeArray() const;

    IDXGIOutput* GetOutput() const;
    unsigned int GetIndex() const;

protected:
    NiD3D10OutputDesc();
    ~NiD3D10OutputDesc();

    bool EnumerateDisplayModes();
    unsigned int m_uiIndex;

    NiD3D10AdapterDesc* m_pkAdapter;
    IDXGIOutput* m_pkOutput;

    DXGI_OUTPUT_DESC m_kOutputDesc;

    DXGI_MODE_DESC* m_pkDisplayModes;
    unsigned int m_uiDisplayModeCount;

    friend class NiD3D10AdapterDesc;
};
//---------------------------------------------------------------------------

//#include "NiD3D10SystemDesc.inl"

#endif // #ifndef NID3D10SYSTEMDESC_H
