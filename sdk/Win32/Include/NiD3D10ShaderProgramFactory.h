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

#ifndef NID3D10SHADERPROGRAMFACTORY_H
#define NID3D10SHADERPROGRAMFACTORY_H

#include "NiD3D10RendererLibType.h"

#include "NiD3D10Headers.h"

#include <NiMemObject.h>
#include <NiSmartPointer.h>
#include <NiTFixedStringMap.h>
#include <NiTPointerList.h>

NiSmartPointer(NiD3D10ShaderProgramCreator);
NiSmartPointer(NiD3D10VertexShader);
NiSmartPointer(NiD3D10GeometryShader);
NiSmartPointer(NiD3D10PixelShader);

class NID3D10RENDERER_ENTRY NiD3D10ShaderProgramFactory : public NiMemObject
{
public:
    virtual ~NiD3D10ShaderProgramFactory();

    static NiD3D10ShaderProgramFactory* GetInstance();

    // Vertex shader creation
    static bool CreateVertexShaderFromFile(const char* pcFileName, 
        const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude, 
        const char* pcEntryPoint, const char* pcShaderTarget, 
        unsigned int uiFlags, const char* pcShaderName, 
        NiD3D10VertexShaderPtr& spVertexShader);
    static bool CreateVertexShaderFromBlob(ID3D10Blob* pkShaderCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude,const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10VertexShaderPtr& spVertexShader);
    static bool CreateVertexShaderFromCompiledBlob(
        ID3D10Blob* pkShaderByteCode, const char* pcFileName, 
        const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
        const char* pcEntryPoint, const char* pcShaderTarget, 
        unsigned int uiFlags, const char* pcShaderName, 
        NiD3D10VertexShaderPtr& spVertexShader);
    //static bool CreateVertexShaderFromBuffer(const void* pvBuffer, 
    //    unsigned int uiBufferSize, const char* pcFileName, 
    //    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
    //    const char* pcEntryPoint, const char* pcShaderTarget, 
    //    unsigned int uiFlags, const char* pcShaderName,
    //    NiD3D10VertexShaderPtr& spVertexShader);

    // Geometry shader creation
    static bool CreateGeometryShaderFromFile(const char* pcFileName, 
        const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude, 
        const char* pcEntryPoint, const char* pcShaderTarget, 
        unsigned int uiFlags, const char* pcShaderName, 
        NiD3D10GeometryShaderPtr& spGeometryShader);
    static bool CreateGeometryShaderFromBlob(ID3D10Blob* pkShaderCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude,const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10GeometryShaderPtr& spGeometryShader);
    static bool CreateGeometryShaderFromCompiledBlob(
        ID3D10Blob* pkShaderByteCode, const char* pcFileName, 
        const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
        const char* pcEntryPoint, const char* pcShaderTarget, 
        unsigned int uiFlags, const char* pcShaderName, 
        NiD3D10GeometryShaderPtr& spGeometryShader);
    //static bool CreateGeometryShaderFromBuffer(const void* pvBuffer, 
    //    unsigned int uiBufferSize, const char* pcFileName, 
    //    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
    //    const char* pcEntryPoint, const char* pcShaderTarget, 
    //    unsigned int uiFlags, const char* pcShaderName,
    //    NiD3D10GeometryShaderPtr& spGeometryShader);
    static bool CreateGeometryShaderWithStreamOutputFromFile(
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, 
        const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
        unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
        NiD3D10GeometryShaderPtr& spGeometryShader);
    static bool CreateGeometryShaderWithStreamOutputFromBlob(
        ID3D10Blob* pkShaderCode, const char* pcFileName, 
        const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude, 
        const char* pcEntryPoint, const char* pcShaderTarget, 
        unsigned int uiFlags, const char* pcShaderName, 
        const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
        unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
        NiD3D10GeometryShaderPtr& spGeometryShader);
    static bool CreateGeometryShaderWithStreamOutputFromCompiledBlob(
        ID3D10Blob* pkShaderByteCode, const char* pcFileName, 
        const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude, 
        const char* pcEntryPoint, const char* pcShaderTarget, 
        unsigned int uiFlags, const char* pcShaderName, 
        const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
        unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
        NiD3D10GeometryShaderPtr& spGeometryShader);
    //static bool CreateGeometryShaderWithStreamOutputFromBuffer(
    //    const void* pvBuffer, unsigned int uiBufferSize, 
    //    const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
    //    ID3D10Include* pkInclude, const char* pcEntryPoint, 
    //    const char* pcShaderTarget, unsigned int uiFlags, 
    //    const char* pcShaderName, 
    //    const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
    //    unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
    //    NiD3D10GeometryShaderPtr& spGeometryShader);

    // Pixel  shader creation
    static bool CreatePixelShaderFromFile(const char* pcFileName, 
        const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude, 
        const char* pcEntryPoint, const char* pcShaderTarget, 
        unsigned int uiFlags, const char* pcShaderName,
        NiD3D10PixelShaderPtr& spPixelShader);
    static bool CreatePixelShaderFromBlob(ID3D10Blob* pkShaderCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude,const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10PixelShaderPtr& spPixelShader);
    static bool CreatePixelShaderFromCompiledBlob(
        ID3D10Blob* pkShaderByteCode, const char* pcFileName, 
        const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
        const char* pcEntryPoint, const char* pcShaderTarget, 
        unsigned int uiFlags, const char* pcShaderName, 
        NiD3D10PixelShaderPtr& spPixelShader);
    //static bool CreatePixelShaderFromBuffer(const void* pvBuffer, 
    //    unsigned int uiBufferSize, const char* pcFileName, 
    //    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude,
    //    const char* pcEntryPoint, const char* pcShaderTarget, 
    //    unsigned int uiFlags, const char* pcShaderName,
    //    NiD3D10PixelShaderPtr& spPixelShader);

    // Shader creation flags - apply to all shaders
    static unsigned int GetUniversalShaderCreationFlags();
    static void SetUniversalShaderCreationFlags(unsigned int uiFlags);

    const char* GetFirstProgramDirectory(NiTListIterator& kIter);
    const char* GetNextProgramDirectory(NiTListIterator& kIter);
    void AddProgramDirectory(const char* pcDirectory);
    void RemoveProgramDirectory(const char* pcDirectory);
    void RemoveAllProgramDirectories();

    static bool RegisterShaderCreator(const char* pcExt, 
        NiD3D10ShaderProgramCreator* pkCreator);
    static void UnregisterShaderCreator(
        NiD3D10ShaderProgramCreator* pkCreator);
    static NiD3D10ShaderProgramCreator* GetShaderCreator(
        const char* pcFilename);

    // *** begin Emergent internal use only ***

    // Shader recreation
    static bool RecreateVertexShader(NiD3D10VertexShader* pkVertexShader);
    static bool RecreateGeometryShader(
        NiD3D10GeometryShader* pkGeometryShader);
    static bool RecreatePixelShader(NiD3D10PixelShader* pkPixelShader);

    // Shader releasing
    static void ReleaseVertexShader(NiD3D10VertexShader* pkShader);
    static void ReleaseGeometryShader(NiD3D10GeometryShader* pkShader);
    static void ReleasePixelShader(NiD3D10PixelShader* pkShader);

    // Find functions
    NiD3D10VertexShader* GetVertexShader(const NiFixedString& kShaderName); 
    NiD3D10GeometryShader* GetGeometryShader(const NiFixedString& kShaderName);
    NiD3D10PixelShader* GetPixelShader(const NiFixedString& kShaderName); 

    // Shader insertion/removal
    void InsertVertexShaderIntoMap(NiD3D10VertexShader* pkShader);
    inline void RemoveVertexShaderFromMap(NiFixedString& kShaderName);
    void InsertGeometryShaderIntoMap(NiD3D10GeometryShader* pkShader);
    inline void RemoveGeometryShaderFromMap(NiFixedString& kShaderName);
    void InsertPixelShaderIntoMap(NiD3D10PixelShader* pkShader);
    inline void RemovePixelShaderFromMap(NiFixedString& kShaderName);

    static bool NiD3D10ShaderProgramFactory::ResolveShaderFileName(
        const NiFixedString& kOriginalName, NiFixedString& kTrueName);

    static void _SDMInit();
    static void _SDMShutdown();

    // *** end Emergent internal use only ***

protected:
    NiD3D10ShaderProgramFactory();

    static ID3D10Blob* ReadShaderFile(const NiFixedString& kFileName);

    // We are only allowing one directory at this stage
    NiTPointerList<char*> m_kProgramDirectories;

    // Maps for shader program names to shaders.
    NiTFixedStringMap<NiD3D10VertexShader*> m_kVertexShaderMap;
    NiTFixedStringMap<NiD3D10GeometryShader*> m_kGeometryShaderMap;
    NiTFixedStringMap<NiD3D10PixelShader*> m_kPixelShaderMap;

    NiTFixedStringMap<NiD3D10ShaderProgramCreator*> m_kShaderCreators;

    static NiD3D10ShaderProgramFactory* ms_pkFactory;
};

typedef NiPointer<NiD3D10ShaderProgramFactory> NiD3D10ShaderProgramFactoryPtr;

#include "NiD3D10ShaderProgramFactory.inl"

#endif  //#ifndef NID3D10SHADERPROGRAMFACTORY_H
