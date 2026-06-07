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

#ifndef NIBASERENDERERDESC_H
#define NIBASERENDERERDESC_H

#include "NiRendererSettings.h"
#include <NiMemObject.h>

class NiRendererSettings;

// Abstract class, defines interface for NiXXXRendererDesc classes
class NiBaseRendererDesc : public NiMemObject
{
public:
    enum ResolutionFilters
    {
        RESFILTER_ALL,
        RESFILTER_NORMAL,
        RESFILTER_WIDE
    };

    // Initialization function
    virtual bool Initialize() = 0;

    // Routine that returns pointer to NiRendererSettings
    virtual NiRendererSettings* GetSettings();

    // Returns true for NiD3D10RendererDesc and false for NiDX9RendererDesc
    virtual bool IsD3D10RendererDesc() = 0;

    // Activate method is called when corresponding OptionsView is selected
    virtual void Activate();
    virtual bool IsActive();

    // Adapter retrieving and selection
    virtual void GetAdapterList(NiTObjectArray<NiFixedString>& kList) = 0;
    virtual bool SelectAdapter(unsigned int uiAdaptIdx) = 0;
    virtual unsigned int GetDefaultAdapter() = 0;
    virtual bool GetNVPerfHUDSupport() = 0;
    virtual bool GetDefaultNVPerfHUDSetting() = 0;
    virtual bool SelectPerfHUDAdapter() = 0;

    // Device retrieving and selection
    virtual bool IsDeviceAvailable(bool bRefDevice) = 0;
    virtual void SelectDevice(bool bRefDevice) = 0;
    virtual bool GetDefaultDevice() = 0;

    // Device retrieve / selection options functions
    virtual bool IsPureDeviceAvailable() = 0;
    virtual bool GetDefaultPureDevice() = 0;
    virtual void SelectPureDevice(bool bPure) = 0;
    virtual bool IsVetrexprocessingAvailable() = 0;
    virtual NiRendererSettings::Vertexprocessing
        GetDefaultVertexprocessing() = 0;
    virtual void SelectVertexprocessing(
        NiRendererSettings::Vertexprocessing eVertex) = 0;

    // Resolution functions
    virtual void SetResolutionFilter(ResolutionFilters eResFilter) = 0;
    virtual void GetResolutionList(NiTObjectArray<NiFixedString>& kList) = 0;
    virtual void SelectResolution(unsigned int uiResIdx) = 0;
    virtual unsigned int GetDefaultResolutionIdx() = 0;

    // Windowed mode functions
    virtual bool CanRenderWindowed() = 0;
    virtual void SelectWindowedMode(bool bFullscreen) = 0;
    virtual bool GetDefaultWindowedMode() = 0;

    // Depth buffer format selection functions
    virtual unsigned int GetDefaultDSFormat() = 0;
    virtual void SelectDSFormat(unsigned int uiDSFmtIdx) = 0;
    virtual void GetDepthSurfaceFormatList(
        NiTObjectArray<NiFixedString>& kList) = 0;

    // Render target format selection functions
    virtual unsigned int GetDefaultRTFormat() = 0;
    virtual void SelectRTFormat(unsigned int uiRTFmtIdx) = 0;
    virtual void GetRenderTargetFormatList(
        NiTObjectArray<NiFixedString>& kList) = 0;

    // Lock to vsync functions
    virtual bool GetDefaultVSync();
    virtual void SelectVSync(bool bVSync);

    // Multisample selection functions
    virtual unsigned int GetDefaultMultisample() = 0;
    virtual void SelectMultisample(unsigned int uiMultisample) = 0;
    virtual void GetMultisampleModeList(
        NiTObjectArray<NiFixedString>& kList) = 0;
};

#include "NiBaseRendererDesc.inl"

#endif // NIBASERENDERERDESC_H