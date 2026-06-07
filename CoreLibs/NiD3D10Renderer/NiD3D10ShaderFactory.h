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

#ifndef NID3D10SHADERFACTORY_H
#define NID3D10SHADERFACTORY_H

#include "NiD3D10RendererLibType.h"

#include <NiShaderFactory.h>
#include <NiTFixedStringMap.h>

class NiRenderer;

class NID3D10RENDERER_ENTRY NiD3D10ShaderFactory : public NiShaderFactory
{
protected:
    NiD3D10ShaderFactory();

public:
    virtual ~NiD3D10ShaderFactory();

    virtual NiShader* RetrieveShader(const char* pcName, 
        unsigned int uiImplementation, bool bReportNotFound = true);
    virtual NiShader* FindShader(const char* pcName, 
        unsigned int uiImplementation);

    virtual bool ReleaseShaderFromLibrary(const char* pcName, 
        unsigned int uiImplementation);
    virtual bool ReleaseShaderFromLibrary(NiShader* pkShader);

    // *** begin Emergent internal use only ***

    virtual void InsertShader(NiShader* pkShader, 
        unsigned int uiImplementation);
    virtual void RemoveShader(const char* pcName,
        unsigned int uiImplementation);
    virtual void RemoveAllShaders();

    virtual void InsertLibrary(NiShaderLibrary* pkLibrary);
    virtual NiShaderLibrary* FindLibrary(const char* pcName);
    virtual void RemoveLibrary(const char* pcName);
    virtual void RemoveAllLibraries();
    virtual NiShaderLibrary* GetFirstLibrary();
    virtual NiShaderLibrary* GetNextLibrary();

    virtual void PurgeAllShaderRendererData();
    virtual void RestoreAllShaderRendererData();

#if defined(_USRDLL)
    virtual void FreeLibraryDLLs();
    virtual void* GetFirstLibraryDLL(const char*& pcName);
    virtual void* GetNextLibraryDLL(const char*& pcName);
    virtual void ClearLibraryDLLs();
#endif
    virtual bool IsDefaultImplementation(NiShader* pkShader);

    static void _SDMInit();
    static void _SDMShutdown();

    static NiShaderFactory* Create();
    static void Destroy();

    virtual const unsigned int GetMajorVertexShaderVersion(
        const unsigned int uiVersion);
    virtual const unsigned int GetMinorVertexShaderVersion(
        const unsigned int uiVersion);
    virtual const unsigned int CreateVertexShaderVersion(
        const unsigned int uiMajorVersion, const unsigned int uiMinorVersion);
    virtual const unsigned int GetMajorGeometryShaderVersion(
        const unsigned int uiVersion);
    virtual const unsigned int GetMinorGeometryShaderVersion(
        const unsigned int uiVersion);
    virtual const unsigned int CreateGeometryShaderVersion(
        const unsigned int uiMajorVersion, const unsigned int uiMinorVersion);
    virtual const unsigned int GetMajorPixelShaderVersion(
        const unsigned int uiVersion);
    virtual const unsigned int GetMinorPixelShaderVersion(
        const unsigned int uiVersion);
    virtual const unsigned int CreatePixelShaderVersion(
        const unsigned int uiMajorVersion, const unsigned int uiMinorVersion);

    bool SetAsActiveFactory();
    inline static bool IsActiveFactory();
    static NiD3D10ShaderFactory* GetD3D10ShaderFactory();

    virtual const char* GetRendererString() const;

    virtual NISHADERFACTORY_CLASSCREATIONCALLBACK 
        GetDefaultClassCreateCallback() const;
    virtual NISHADERFACTORY_RUNPARSERCALLBACK GetDefaultRunParserCallback()
        const;
    virtual NISHADERFACTORY_ERRORCALLBACK GetDefaultErrorCallback() const;

    // *** end Emergent internal use only ***

protected:
    virtual const char* GetFirstProgramDirectory(NiTListIterator& kIter);
    virtual const char* GetNextProgramDirectory(NiTListIterator& kIter);
    virtual void AddProgramDirectory(const char* pacDirectory);
    virtual void RemoveProgramDirectory(const char* pacDirectory);
    virtual void RemoveAllProgramDirectories();

    static const char* GetCompilerName(unsigned int uiVersion);

    NiTFixedStringMap<NiShader*> m_kShaderMap;
    NiTFixedStringMap<NiShaderLibraryPtr> m_kLibraryMap;
    NiTMapIterator m_kLibraryIter;

    NiCriticalSection m_kShaderMapLock;

#if defined(_USRDLL)
    // Default Class Creation and RunParser functions for use
    // with DLL builds
    static bool DefaultDLLCreateClass(const char* pcLibName, 
        NiRenderer* pkRenderer, int iDirectoryCount, 
        const char* apcDirectories[], bool bRecurseSubFolders, 
        NiShaderLibrary** ppkLibrary);
    static unsigned int DefaultDLLRunParser(const char* pcLibName, 
        NiRenderer* pkRenderer, const char* pcDirectory, 
        bool bRecurseSubFolders);


    NiTMapIterator m_kLibraryDLLIter;
    NiTStringMap<HMODULE> m_kLoadedShaderLibDLLs;
#endif //#if defined(_USRDLL)

    static NiD3D10ShaderFactory* ms_pkD3D10ShaderFactory;
};

typedef NiPointer<NiD3D10ShaderFactory> NiD3D10ShaderFactoryPtr;

#include "NiD3D10ShaderFactory.inl"

#endif  // #ifndef NID3D10SHADERFACTORY_H
