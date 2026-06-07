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

#ifndef WIN32SHADERTOOLKIT_H
#define WIN32SHADERTOOLKIT_H

// NiApplication framework
#include <NiMain.h>

#include <NiShaderFactory.h>

NiSmartPointer(Win32ShaderToolkit);

class Win32ShaderToolkit : public NiRefObject
{
protected:
    Win32ShaderToolkit();
    
public:
    ~Win32ShaderToolkit();

    static Win32ShaderToolkit* Create();
    static void Shutdown();

protected:
    // NiShader Functions
    virtual bool RunShaderParsers(const char* pcShaderDir);
    virtual bool RunShaderLibraries(const char* pcShaderDir);
    virtual bool RegisterShaderParsers();
    virtual bool RegisterShaderLibraries();
#if !defined(_USRDLL)
    static bool LibraryClassCreate(const char* pcLibFile, 
        NiRenderer* pkRenderer, int iDirectoryCount, 
        const char* apcDirectories[], 
        bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);
    static unsigned int RunParser(const char* pcLibFile, 
        NiRenderer* pkRenderer, const char* pcDirectory, 
        bool bRecurseSubFolders);
#endif
    static unsigned int ShaderErrorCallback(const char* pcError, 
        NiShaderError eError, bool bRecoverable);

    static Win32ShaderToolkitPtr ms_spWin32ShaderToolkit;

#if defined(_USRDLL)
    NiTObjectArray<NiString> m_kShaderLibraries;
    NiTObjectArray<NiString> m_kParserLibraries;
#else
    NiTPrimitiveArray<NiShaderFactory::NISHADERLIBRARY_CLASSCREATIONCALLBACK> 
        m_kShaderLibraries;
    NiTPrimitiveArray<NiShaderFactory::NISHADERLIBRARY_RUNPARSERCALLBACK>
        m_kParserLibraries;
#endif

    unsigned int m_uiActiveCallbackIdx;
};

#endif // WIN32SHADERTOOLKIT_H
