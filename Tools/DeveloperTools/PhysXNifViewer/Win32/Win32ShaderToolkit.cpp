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


#include "Win32ShaderToolkit.h"
#include <NiD3DShaderFactory.h>
#include <NiD3D10ShaderFactory.h>

// This sample only has static library configurations
#ifndef _LIB
#define _LIB
#endif

#include <NiD3DXEffectShaderLib.h>
#include <NiD3D10EffectShaderLib.h>
#include <NiBinaryShaderLib.h>
#include <NiD3D10BinaryShaderLib.h>
#include <NSBShaderLib.h>
#include <NSFParserLib.h>

#ifndef _USRDLL
#pragma comment(lib, "NiD3DXEffectShaderLib.lib")
#pragma comment(lib, "NiD3D10EffectShaderLib.lib")
#pragma comment(lib, "NiBinaryShaderLib.lib")
#pragma comment(lib, "NiD3D10BinaryShaderLib.lib")
#pragma comment(lib, "NSBShaderLib.lib")
#pragma comment(lib, "NSFParserLib.lib")
#endif

Win32ShaderToolkitPtr Win32ShaderToolkit::ms_spWin32ShaderToolkit = 0;

//---------------------------------------------------------------------------
Win32ShaderToolkit::Win32ShaderToolkit()
{
}
//---------------------------------------------------------------------------
Win32ShaderToolkit::~Win32ShaderToolkit()
{
}
//---------------------------------------------------------------------------
Win32ShaderToolkit* Win32ShaderToolkit::Create()
{
    if (ms_spWin32ShaderToolkit != 0)
        return ms_spWin32ShaderToolkit;

    ms_spWin32ShaderToolkit = NiNew Win32ShaderToolkit();
    ms_spWin32ShaderToolkit->m_uiActiveCallbackIdx = 0;

    NIASSERT(NiRenderer::GetRenderer());
    bool bD3D10 = (NiRenderer::GetRenderer()->GetRendererID() == NiSystemDesc::RENDERER_D3D10);
    
    NiShaderFactory::RegisterErrorCallback(ShaderErrorCallback);

    char acShaderLibraryDir[NI_MAX_PATH];
    char acShaderProgramDir[NI_MAX_PATH];
    
    acShaderLibraryDir[0] = 0;

#if _MSC_VER >= 1400
    size_t stSize = 0;
    getenv_s(&stSize, NULL, 0, "EGB_SHADER_LIBRARY_PATH");
    if (stSize != 0)
    {
        NIASSERT(stSize < NI_MAX_PATH);
        char acBuf[NI_MAX_PATH];
        getenv_s(&stSize, acBuf, NI_MAX_PATH, "EGB_SHADER_LIBRARY_PATH");
        NiSprintf(acShaderLibraryDir, NI_MAX_PATH, "%s", acBuf);
    }
#else //#if _MSC_VER >= 1400
    char *pcPath = getenv("EGB_SHADER_LIBRARY_PATH");
    if (pcPath)
        NiSprintf(acShaderLibraryDir, NI_MAX_PATH, "%s", pcPath);
#endif //#if _MSC_VER >= 1400

    if (!acShaderLibraryDir[0])
    {
        NiMessageBox("EGB_SHADER_LIBRARY_PATH environment "
            "variable not found", "ERROR");
        return 0;
    }

    NiPath::RemoveDotDots(acShaderLibraryDir);

    NiStrcat(acShaderLibraryDir, NI_MAX_PATH, "\\Data");
    if (bD3D10)
        NiSprintf(acShaderProgramDir, NI_MAX_PATH, "%s\\D3D10", acShaderLibraryDir);
    else
        NiSprintf(acShaderProgramDir, NI_MAX_PATH, "%s\\DX9", acShaderLibraryDir);

    NiPath::Standardize(acShaderLibraryDir);
    NiPath::Standardize(acShaderProgramDir);
    NiShaderFactory::AddShaderProgramFileDirectory(acShaderProgramDir);

#if !defined(_USRDLL)
    ms_spWin32ShaderToolkit->m_kParserLibraries.Add(
        &NSFParserLib_RunShaderParser);

    if (bD3D10)
    {
        ms_spWin32ShaderToolkit->m_kShaderLibraries.Add(
            &NiD3D10EffectShaderLib_LoadShaderLibrary);
    }
    else
    {
        ms_spWin32ShaderToolkit->m_kShaderLibraries.Add(
            &NiD3DXEffectShaderLib_LoadShaderLibrary);
    }

    ms_spWin32ShaderToolkit->m_kShaderLibraries.Add(
        &NSBShaderLib_LoadShaderLibrary);
#endif
    
    // First, we will run the parsers. This is done to make sure that
    // any text-based shader files that have been modified are re-compiled
    // to binary before loading all the binary representations.
    if (!ms_spWin32ShaderToolkit->RunShaderParsers(acShaderLibraryDir))
    {
        NiMessageBox("Failed to run shader parsers!", "ERROR");
        return false;
    }

    if (!ms_spWin32ShaderToolkit->RunShaderLibraries(acShaderLibraryDir))
    {
        NiMessageBox("Failed to run shader parsers!", "ERROR");
        return false;
    }

    return ms_spWin32ShaderToolkit;;
}
//---------------------------------------------------------------------------
void Win32ShaderToolkit::Shutdown()
{
    ms_spWin32ShaderToolkit = 0;
}
//---------------------------------------------------------------------------
bool Win32ShaderToolkit::RunShaderParsers(const char* pcShaderDir)
{
#if !defined(_USRDLL)
    NiShaderFactory::RegisterRunParserCallback(RunParser);
#else
    // Use the default DLL creation callback in DLL builds
#endif

    if (!RegisterShaderParsers())
        return false;

    for (m_uiActiveCallbackIdx = 0; 
        m_uiActiveCallbackIdx < m_kParserLibraries.GetSize(); 
        m_uiActiveCallbackIdx++)
    {
        const char* pcLibName = NULL;
#if defined(_USRDLL)
        pcLibName = m_kParserLibraries.GetAt(m_uiActiveCallbackIdx);
#endif
        NiShaderFactory::LoadAndRunParserLibrary(pcLibName, pcShaderDir, true);
    }
    return true;
}
//---------------------------------------------------------------------------
bool Win32ShaderToolkit::RegisterShaderParsers()
{
#if defined(_USRDLL)
    m_kParserLibraries.Add("NSFParserLib" NI_DLL_SUFFIX ".dll");
#endif
    return true;
}
//---------------------------------------------------------------------------
bool Win32ShaderToolkit::RunShaderLibraries(const char* pcShaderDir)
{
#if !defined(_USRDLL)
    NiShaderFactory::RegisterClassCreationCallback(LibraryClassCreate);
#else
    // Use the default DLL creation callback in DLL builds
#endif

    if (!RegisterShaderLibraries())
        return false;

    unsigned int uiCount = 1;
    const char* apcDirectories[1];
    apcDirectories[0] = pcShaderDir;

    for (m_uiActiveCallbackIdx = 0; 
        m_uiActiveCallbackIdx < m_kShaderLibraries.GetSize();
        m_uiActiveCallbackIdx++)
    {
        const char* pcLibName = NULL;
#if defined(_USRDLL)
        pcLibName = m_kShaderLibraries.GetAt(m_uiActiveCallbackIdx);
#endif
        if (!NiShaderFactory::LoadAndRegisterShaderLibrary(pcLibName, uiCount, 
            apcDirectories, true))
        {
            NiMessageBox("Failed to load shader library!", "ERROR");
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool Win32ShaderToolkit::RegisterShaderLibraries()
{
#if defined(_USRDLL)
    m_kShaderLibraries.Add("NiD3D10EffectShaderLib" NI_DLL_SUFFIX ".dll");
    m_kShaderLibraries.Add("NiD3DXEffectShaderLib" NI_DLL_SUFFIX ".dll");
    m_kShaderLibraries.Add("NSBShaderLib" NI_DLL_SUFFIX ".dll");
#endif
    return true;
}
//---------------------------------------------------------------------------
#if !defined(_USRDLL)
// Only defined in non-DLL builds
bool Win32ShaderToolkit::LibraryClassCreate(const char*, 
    NiRenderer* pkRenderer, int iDirectoryCount, const char* apcDirectories[],
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
    *ppkLibrary = NULL;

    if (ms_spWin32ShaderToolkit->m_uiActiveCallbackIdx >= 
        ms_spWin32ShaderToolkit->m_kShaderLibraries.GetSize())
    {
        return false;
    }

    NiShaderFactory::NISHADERLIBRARY_CLASSCREATIONCALLBACK pkCallback =
        ms_spWin32ShaderToolkit->m_kShaderLibraries.GetAt(
        ms_spWin32ShaderToolkit->m_uiActiveCallbackIdx);
    if (pkCallback)
    {
        if (!pkCallback(pkRenderer, iDirectoryCount, apcDirectories, 
            bRecurseSubFolders, ppkLibrary))
        {
            return false;
        }
    }

    return true;
}
#endif
//---------------------------------------------------------------------------
#if !defined(_USRDLL)
// Only defined in non-DLL builds
unsigned int Win32ShaderToolkit::RunParser(const char*, 
    NiRenderer*, const char* pcDirectory, 
    bool bRecurseSubFolders)
{
    unsigned int uiCount = 0;
    if (ms_spWin32ShaderToolkit->m_uiActiveCallbackIdx >= 
        ms_spWin32ShaderToolkit->m_kParserLibraries.GetSize())
    {
        return false;
    }

    NiShaderFactory::NISHADERLIBRARY_RUNPARSERCALLBACK pkCallback =
        ms_spWin32ShaderToolkit->m_kParserLibraries.GetAt(
        ms_spWin32ShaderToolkit->m_uiActiveCallbackIdx);
    if (pkCallback)
    {
        uiCount += pkCallback(pcDirectory, bRecurseSubFolders);
    }

    return uiCount;
}
#endif
//---------------------------------------------------------------------------
unsigned int Win32ShaderToolkit::ShaderErrorCallback(const char* pcError, 
    NiShaderError, bool)
{
    NI_UNUSED_ARG(pcError);
    NiOutputDebugString("ERROR: ");
    NiOutputDebugString(pcError);
    return 0;
}
//---------------------------------------------------------------------------
