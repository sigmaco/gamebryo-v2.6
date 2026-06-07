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

#ifndef NIRENDERERSETTINGS_H
#define NIRENDERERSETTINGS_H

#include <NiMemObject.h>
#include <NiDX9SystemDesc.h>
#include <NiD3D10SystemDesc.h>

class NiDX9RendererDesc;
class NiD3D10RendererDesc;

class NiRendererSettings : public NiMemObject
{
public:
    ~NiRendererSettings();

    enum Vertexprocessing
    {
        VERTEX_UNSUPPORTED,
        VERTEX_HARDWARE,
        VERTEX_MIXED,
        VERTEX_SOFTWARE
    };

    // Screen resolution
    unsigned int m_uiScreenWidth;
    unsigned int m_uiScreenHeight;
    // Minimum and maximum resolution settings - used to limit user's choice
    // 0 means no limit
    unsigned int m_uiMinScreenWidth;
    unsigned int m_uiMinScreenHeight;

    unsigned int m_uiAdapterIdx;
    unsigned int m_uiNVPerfHUDAdapterIdx;

    bool m_bD3D10Renderer;
    bool m_bFullscreen;
    bool m_bNVPerfHUD;
    bool m_bRefRast;
    bool m_bUse16Bit;
    bool m_bPureDevice;
    bool m_bVSync;
    bool m_bStencil;
    bool m_bMultiThread;
    bool m_bRendererDialog;
    bool m_bSaveSettings;

    Vertexprocessing m_eVertexProcessing;

    NiDX9Renderer::FrameBufferFormat m_eDX9RTFormat;
    NiDX9Renderer::DepthStencilFormat m_eDX9DSFormat;
    NiDX9Renderer::FramebufferMode m_eDX9FBFormat;

    unsigned int m_uiD3D10OutputIdx;
    unsigned int m_uiD3D10MultisampleCount;
    unsigned int m_uiD3D10MultisampleQuality;
    DXGI_FORMAT m_eD3D10DSFormat;
    DXGI_FORMAT m_eD3D10RTFormat;
    DXGI_RATIONAL m_kD3D10Refresh;

    // Returns instance of NiRendererSettings and creates one if needed
    static NiRendererSettings* GetInstance();

    // Load / save settings. To alter file name, call SetIniFilename()
    static void LoadSettings();
    static void SaveSettings();

    // Sets file name of .ini file. Default specified by ms_acIniFilename.
    // File by default created in .exe file directory. If file name contains
    // slashes or backslashes, it treated like it contains full path.
    static void SetIniFilename(char* pcFileName);
    static const char* GetIniFilename();

    // Sync settings to/from NiApplication for compatibility
    static void SyncToNiApplication();
    static void SyncFromNiApplication();

    // Process command line
    static void ParseCommandLine();

    static char* ms_acIniFilename;

protected:
    NiFixedString m_kIniFileName;

    static void ReadUInt(const char* pcName, unsigned int& uiVal);
    static void ReadBool(const char* pcName, bool& bVal);

    static void WriteUInt(const char* pcName, unsigned int uiVal);
    static void WriteBool(const char* pcName, bool bVal);

private:
    NiRendererSettings();

    static NiRendererSettings* ms_pkRendererSettings;
};

#include "NiRendererSettings.inl"

#endif // NIRENDERERSETTINGS_H