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

#ifndef NISHADERFACTORY_H
#define NISHADERFACTORY_H

#include "NiRefObject.h"
#include "NiShaderError.h"
#include "NiShaderLibrary.h"
#include "NiTFixedStringMap.h"
#include "NiTListBase.h"
#include "NiGlobalConstantEntry.h"

class NiRenderer;
class NiRenderObject;

// NiShaderFactory
class NIMAIN_ENTRY NiShaderFactory : public NiRefObject
{
protected:
    NiShaderFactory();

public:
    virtual ~NiShaderFactory();

    static void Shutdown();

    static NiShaderFactory* GetInstance();

    static void ReleaseAllShaders();

    static void RegisterLibrary(NiShaderLibrary* pkLibrary);
    static void UnregisterLibrary(NiShaderLibrary* pkLibrary);
    static void UnregisterAllLibraries();

    // Class Creation function - used for loading shader libraries
    typedef bool (*NISHADERFACTORY_CLASSCREATIONCALLBACK)(const char*, 
        NiRenderer*, int, const char*[], bool, NiShaderLibrary**);
    static bool RegisterClassCreationCallback(
        NISHADERFACTORY_CLASSCREATIONCALLBACK pfnCallback);
    static bool LoadAndRegisterShaderLibrary(const char* pcLibName,
        int iDirectoryCount, const char* pacDirectories[], 
        bool bRecurseSubFolders);

    // Run Parser function - used for parsing text files into shader libraries
    typedef unsigned int (*NISHADERFACTORY_RUNPARSERCALLBACK)(const char*, 
        NiRenderer*, const char*, bool);
    static bool RegisterRunParserCallback(
        NISHADERFACTORY_RUNPARSERCALLBACK pfnCallback);
    static unsigned int LoadAndRunParserLibrary(const char* pcLibName, 
        const char* pcDirectory, bool bRecurseSubFolders);

    // Error Reporting function - used to report errors loading shaders
    typedef unsigned int (*NISHADERFACTORY_ERRORCALLBACK)(const char*,
        NiShaderError eErr, bool);
    static bool RegisterErrorCallback(
        NISHADERFACTORY_ERRORCALLBACK pfnCallback);
    static unsigned int ReportError(NiShaderError eErr, bool bRecoverable, 
        const char* pcFmt, ...);

    // Signatures for library & parser callbacks. These function signatures
    // will be what an individual shader library or parser library
    // should implement to create a class or parser.
    typedef bool (*NISHADERLIBRARY_CLASSCREATIONCALLBACK)( 
        NiRenderer*, int, const char*[], bool, NiShaderLibrary**);
    typedef unsigned int (*NISHADERLIBRARY_RUNPARSERCALLBACK)( 
        const char*, bool);

    virtual NiShader* RetrieveShader(const char* pcName, 
        unsigned int uiImplementation, bool bReportNotFound = true);
    virtual NiShader* FindShader(const char* pcName, 
        unsigned int uiImplementation);

    virtual bool ReleaseShaderFromLibrary(const char* pcName, 
        unsigned int uiImplementation);
    virtual bool ReleaseShaderFromLibrary(NiShader* pkShader);

    // Global shader constant mappings
    static bool RegisterGlobalShaderConstant(const NiFixedString& kKey, 
        NiShaderAttributeDesc::AttributeType eType,
        unsigned int uiDataSize, const void* pvInitialData);
    static bool ReleaseGlobalShaderConstant(const NiFixedString& kKey);
    static bool RetrieveGlobalShaderConstant(const NiFixedString& kKey,
        unsigned int& uiDataSize, const void*& pvData);
    static bool UpdateGlobalShaderConstant(const NiFixedString& kKey,
        unsigned int uiDataSize, const void* pvData);

    // *** begin Emergent internal use only ***
    virtual void InsertShader(NiShader* pkShader, 
        unsigned int uiImplementation);
    virtual void RemoveShader(const char* pcName,
        unsigned int uiImplementation);
    virtual void RemoveAllShaders();
    virtual bool IsDefaultImplementation(NiShader* pkShader);

    NiGlobalConstantEntry* GetGlobalShaderConstantEntry(
        const NiFixedString& kKey);

    virtual void InsertLibrary(NiShaderLibrary* pkLibrary);
    virtual NiShaderLibrary* FindLibrary(const char* pcName);
    virtual void RemoveLibrary(const char* pcName);
    virtual void RemoveAllLibraries();
    virtual NiShaderLibrary* GetFirstLibrary();
    virtual NiShaderLibrary* GetNextLibrary();

    // Indicate subdirectory for any supporting shader program files
    static const char* GetFirstShaderProgramFileDirectory(
        NiTListIterator& kIter);
    static const char* GetNextShaderProgramFileDirectory(
        NiTListIterator& kIter);
    static void AddShaderProgramFileDirectory(const char* pcDirectory);
    static void RemoveShaderProgramFileDirectory(const char* pcDirectory);
    static void RemoveAllShaderProgramFileDirectories();

    // Shader version conversion
    virtual const unsigned int GetMajorVertexShaderVersion(
        const unsigned int uiVersion) = 0;
    virtual const unsigned int GetMinorVertexShaderVersion(
        const unsigned int uiVersion) = 0;
    virtual const unsigned int CreateVertexShaderVersion(
        const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
        = 0;
    virtual const unsigned int GetMajorGeometryShaderVersion(
        const unsigned int uiVersion) = 0;
    virtual const unsigned int GetMinorGeometryShaderVersion(
        const unsigned int uiVersion) = 0;
    virtual const unsigned int CreateGeometryShaderVersion(
        const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
        = 0;
    virtual const unsigned int GetMajorPixelShaderVersion(
        const unsigned int uiVersion) = 0;
    virtual const unsigned int GetMinorPixelShaderVersion(
        const unsigned int uiVersion) = 0;
    virtual const unsigned int CreatePixelShaderVersion(
        const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
        = 0;

    virtual const char* GetRendererString() const = 0;

#if defined(_USRDLL)
    virtual void FreeLibraryDLLs();
    virtual void* GetFirstLibraryDLL(const char*& pcName);
    virtual void* GetNextLibraryDLL(const char*& pcName);
    virtual void ClearLibraryDLLs();
#endif
    NiGlobalConstantEntry* GetFirstGlobalShaderConstant(NiTMapIterator& kIter,
        NiFixedString& kName);
    NiGlobalConstantEntry* GetNextGlobalShaderConstant(NiTMapIterator& kIter,
        NiFixedString& kName);

    NISHADERFACTORY_CLASSCREATIONCALLBACK GetClassCreateCallback() const;
    NISHADERFACTORY_RUNPARSERCALLBACK GetRunParserCallback() const;
    NISHADERFACTORY_ERRORCALLBACK GetErrorCallback() const;

    virtual NISHADERFACTORY_CLASSCREATIONCALLBACK 
        GetDefaultClassCreateCallback() const;
    virtual NISHADERFACTORY_RUNPARSERCALLBACK GetDefaultRunParserCallback()
        const;
    virtual NISHADERFACTORY_ERRORCALLBACK GetDefaultErrorCallback() const;

    // *** end Emergent internal use only ***

protected:
    // Pass on directory to whoever needs it.
    virtual const char* GetFirstProgramDirectory(NiTListIterator& kIter) = 0;
    virtual const char* GetNextProgramDirectory(NiTListIterator& kIter) = 0;
    virtual void AddProgramDirectory(const char* pcDirectory) = 0;
    virtual void RemoveProgramDirectory(const char* pcDirectory) = 0;
    virtual void RemoveAllProgramDirectories() = 0;

    static NiShaderFactory* ms_pkShaderFactory;
    
    NiTFixedStringMap<NiGlobalConstantEntry*> m_kGlobalConstantMap;

    NISHADERFACTORY_CLASSCREATIONCALLBACK m_pfnClassCreate;
    NISHADERFACTORY_RUNPARSERCALLBACK m_pfnRunParser;

    NISHADERFACTORY_ERRORCALLBACK m_pfnErrorCallback;
};

typedef NiPointer<NiShaderFactory> NiShaderFactoryPtr;

#endif  //#ifndef NISHADERFACTORY_H
