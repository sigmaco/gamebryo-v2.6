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

// Static variables
NiRendererSettings* NiRendererSettings::ms_pkRendererSettings = NULL;
char* NiRendererSettings::ms_acIniFilename = "AppSettings.ini";

//---------------------------------------------------------------------------
// Constructor - here all settings are initialized to default values
//---------------------------------------------------------------------------
NiRendererSettings::NiRendererSettings() :
    // Default settings. Saved settings and command line parameters
    // will override them.
    m_uiScreenWidth(NiApplication::DEFAULT_WIDTH),
    m_uiScreenHeight(NiApplication::DEFAULT_HEIGHT),
    m_uiMinScreenWidth(640),
    m_uiMinScreenHeight(480),
    m_bD3D10Renderer(false),
    m_bFullscreen(false),
    m_bNVPerfHUD(false),
    m_bRefRast(false),
    m_bUse16Bit(false),
    m_bPureDevice(true),
    m_bVSync(false),
    m_bStencil(false),
    m_bMultiThread(false),
    m_bRendererDialog(true),
    m_bSaveSettings(false),
    m_uiAdapterIdx(0),
    m_uiNVPerfHUDAdapterIdx(0),
    m_eVertexProcessing(VERTEX_HARDWARE),
    m_eDX9RTFormat(NiDX9Renderer::FBFMT_X8R8G8B8),
    m_eDX9DSFormat(NiDX9Renderer::DSFMT_D24S8),
    m_eDX9FBFormat(NiDX9Renderer::FBMODE_DEFAULT),
    m_uiD3D10OutputIdx(0),
    m_uiD3D10MultisampleCount(1),
    m_uiD3D10MultisampleQuality(0),
    m_eD3D10DSFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
    m_eD3D10RTFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
{
    ms_pkRendererSettings = this;
    SetIniFilename(ms_acIniFilename);
}
//---------------------------------------------------------------------------
// Settings loading / saving functions
//---------------------------------------------------------------------------
void NiRendererSettings::LoadSettings()
{
    ReadUInt("ScreenWidth", GetInstance()->m_uiScreenWidth);
    ReadUInt("ScreenHeight", GetInstance()->m_uiScreenHeight);
    ReadUInt("MinScreenWidth", GetInstance()->m_uiMinScreenWidth);
    ReadUInt("MinScreenHeight", GetInstance()->m_uiMinScreenHeight);
    ReadUInt(
        "Vertexprocessing",
        (unsigned int&)(GetInstance()->m_eVertexProcessing));
    ReadUInt(
        "DX9RenderTargetMode",
        (unsigned int&)(GetInstance()->m_eDX9RTFormat));
    ReadUInt(
        "DX9DepthSurfaceMode",
        (unsigned int&)(GetInstance()->m_eDX9DSFormat));
    ReadUInt(
        "DX9FrameBufferMode",
        (unsigned int&)(GetInstance()->m_eDX9FBFormat));
    ReadUInt("D3D10OutputIdx",
        GetInstance()->m_uiD3D10OutputIdx);
    ReadUInt(
        "D3D10MultisampleCount",
        GetInstance()->m_uiD3D10MultisampleCount);
    ReadUInt(
        "D3D10MultisampleQuality",
        GetInstance()->m_uiD3D10MultisampleQuality);
    ReadUInt(
        "D3D10DSFormat",
        (unsigned int&)(GetInstance()->m_eD3D10DSFormat));
    ReadUInt(
        "D3D10RTFormat",
        (unsigned int&)(GetInstance()->m_eD3D10RTFormat));

    ReadBool("D3D10Renderer", GetInstance()->m_bD3D10Renderer);
    ReadBool("Fullscreen", GetInstance()->m_bFullscreen);
    ReadBool("NVPerfHUD", GetInstance()->m_bNVPerfHUD);
    ReadBool("RefRast", GetInstance()->m_bRefRast);
    ReadBool("Use16Bit", GetInstance()->m_bUse16Bit);
    ReadBool("PureDevice", GetInstance()->m_bPureDevice);
    ReadBool("VSync", GetInstance()->m_bVSync);
    ReadBool("MultiThread", GetInstance()->m_bMultiThread);
    ReadBool("RendererDialog", GetInstance()->m_bRendererDialog);
    // Don't read SaveSettings, because that will only ever be true.
}
//---------------------------------------------------------------------------
void NiRendererSettings::SaveSettings()
{
    WriteUInt("ScreenWidth", GetInstance()->m_uiScreenWidth);
    WriteUInt("ScreenHeight", GetInstance()->m_uiScreenHeight);
    WriteUInt("MinScreenWidth", GetInstance()->m_uiMinScreenWidth);
    WriteUInt("MinScreenHeight", GetInstance()->m_uiMinScreenHeight);
    WriteUInt("Vertexprocessing", GetInstance()->m_eVertexProcessing);
    WriteUInt("DX9RenderTargetMode", GetInstance()->m_eDX9RTFormat);
    WriteUInt("DX9DepthSurfaceMode", GetInstance()->m_eDX9DSFormat);
    WriteUInt("DX9FrameBufferMode", GetInstance()->m_eDX9FBFormat);
    WriteUInt("D3D10OutputIdx", GetInstance()->m_uiD3D10OutputIdx);
    WriteUInt(
        "D3D10MultisampleCount",
        GetInstance()->m_uiD3D10MultisampleCount);
    WriteUInt(
        "D3D10MultisampleQuality",
        GetInstance()->m_uiD3D10MultisampleQuality);
    WriteUInt("D3D10DSFormat", GetInstance()->m_eD3D10DSFormat);
    WriteUInt("D3D10RTFormat", GetInstance()->m_eD3D10RTFormat);

    WriteBool("D3D10Renderer", GetInstance()->m_bD3D10Renderer);
    WriteBool("Fullscreen", GetInstance()->m_bFullscreen);
    WriteBool("NVPerfHUD", GetInstance()->m_bNVPerfHUD);
    WriteBool("RefRast", GetInstance()->m_bRefRast);
    WriteBool("Use16Bit", GetInstance()->m_bUse16Bit);
    WriteBool("PureDevice", GetInstance()->m_bPureDevice);
    WriteBool("VSync", GetInstance()->m_bVSync);
    WriteBool("MultiThread", GetInstance()->m_bMultiThread);
    WriteBool("RendererDialog", GetInstance()->m_bRendererDialog);
    // Don't write SaveSettings, because that will only ever be true.
}
//---------------------------------------------------------------------------
// Initialization file name changing function
//---------------------------------------------------------------------------
void NiRendererSettings::SetIniFilename(char* pcFileName)
{
    char acExecutablePath[512];

    GetInstance()->m_kIniFileName = pcFileName;
    // Check if filename contains path
    if (!GetInstance()->m_kIniFileName.Contains("/") &&
        !GetInstance()->m_kIniFileName.Contains("\\"))
    {
        // If not, default path is executable directory path
        NiPath::GetExecutableDirectory(
            acExecutablePath,
            sizeof(acExecutablePath));
        NiStrcat(acExecutablePath, sizeof(acExecutablePath), pcFileName);
        GetInstance()->m_kIniFileName = acExecutablePath;
    };
}
//---------------------------------------------------------------------------
// Functions to synchronize settings with NiApplication for compatibility
//---------------------------------------------------------------------------
void NiRendererSettings::SyncToNiApplication()
{
    NiApplication* pkApp = NiApplication::ms_pkApplication;
    if (!pkApp)
        return;

    pkApp->SetFullscreen(GetInstance()->m_bFullscreen);

    NiAppWindow* pkAppWindow = pkApp->GetAppWindow();
    pkAppWindow->SetWidth(GetInstance()->m_uiScreenWidth);
    pkAppWindow->SetHeight(GetInstance()->m_uiScreenHeight);
}
//---------------------------------------------------------------------------
void NiRendererSettings::SyncFromNiApplication()
{
    NiApplication* pkApp = NiApplication::ms_pkApplication;
    if (!pkApp)
        return;

    GetInstance()->m_bStencil = pkApp->GetStencil();
    GetInstance()->m_bRendererDialog &= pkApp->GetRendererDialog();
    GetInstance()->m_bFullscreen = pkApp->GetFullscreen();

    GetInstance()->m_bD3D10Renderer = pkApp->GetD3D10Renderer();
    GetInstance()->m_bRefRast = pkApp->GetRefRast();
    GetInstance()->m_bNVPerfHUD = pkApp->GetPerfHUD();
    GetInstance()->m_bMultiThread = pkApp->GetMultiThread();
    NiAppWindow* pkWindow = pkApp->GetAppWindow();
    if (pkWindow)
    {
        GetInstance()->m_uiScreenWidth = pkWindow->GetWidth();
        GetInstance()->m_uiScreenHeight = pkWindow->GetHeight();
    }
    if (pkApp->GetSWVertex())
    {
        GetInstance()->m_eVertexProcessing = VERTEX_SOFTWARE;
    }
    else
    {
        GetInstance()->m_eVertexProcessing = VERTEX_HARDWARE;
    }
}
//---------------------------------------------------------------------------
// Command line processing function
//---------------------------------------------------------------------------
void NiRendererSettings::ParseCommandLine()
{
    NiCommand* pkCommand = NiApplication::ms_pkApplication->GetCommand();
    if (!pkCommand)
        return;

    if (pkCommand->Boolean("dlg"))
        GetInstance()->m_bRendererDialog = true;

    if (pkCommand->Boolean("dialog"))
        GetInstance()->m_bRendererDialog = true;

    if (pkCommand->Boolean("showdialog"))
        GetInstance()->m_bRendererDialog = true;

    if (pkCommand->Boolean("dx9"))
        GetInstance()->m_bD3D10Renderer = false;

    if (pkCommand->Boolean("dx10"))
        GetInstance()->m_bD3D10Renderer = true;

    if (pkCommand->Boolean("d3d9"))
        GetInstance()->m_bD3D10Renderer = false;

    if (pkCommand->Boolean("d3d10"))
        GetInstance()->m_bD3D10Renderer = true;

    if (pkCommand->Boolean("nvperf"))
        GetInstance()->m_bNVPerfHUD = true;

    if (pkCommand->Boolean("nvperfhud"))
        GetInstance()->m_bNVPerfHUD = true;

    if (pkCommand->Boolean("perfhud"))
        GetInstance()->m_bNVPerfHUD = true;

    if (pkCommand->Boolean("win"))
        GetInstance()->m_bFullscreen = false;

    if (pkCommand->Boolean("window"))
        GetInstance()->m_bFullscreen = false;

    if (pkCommand->Boolean("full"))
        GetInstance()->m_bFullscreen = true;

    if (pkCommand->Boolean("fullscreen"))
        GetInstance()->m_bFullscreen = true;

    if (pkCommand->Boolean("hal"))
        GetInstance()->m_bRefRast = false;

    if (pkCommand->Boolean("halrast"))
        GetInstance()->m_bRefRast = false;

    if (pkCommand->Boolean("ref"))
        GetInstance()->m_bRefRast = true;

    if (pkCommand->Boolean("refrast"))
        GetInstance()->m_bRefRast = true;

    if (pkCommand->Boolean("pure") && !GetInstance()->m_bRefRast)
        GetInstance()->m_bPureDevice = true;

    if (pkCommand->Boolean("vsync"))
        GetInstance()->m_bVSync = true;

    if (pkCommand->Boolean("hwvertex"))
        GetInstance()->m_eVertexProcessing = VERTEX_HARDWARE;

    if (pkCommand->Boolean("mixvertex"))
        GetInstance()->m_eVertexProcessing = VERTEX_MIXED;

    if (pkCommand->Boolean("swvertex"))
        GetInstance()->m_eVertexProcessing = VERTEX_SOFTWARE;
}
//---------------------------------------------------------------------------
