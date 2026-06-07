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

#ifndef NiD3D10ShaderProgramCreatorHLSL_H
#define NiD3D10ShaderProgramCreatorHLSL_H

#include "NiD3D10RendererLibType.h"

#include "NiD3D10ShaderProgramCreator.h"

class NID3D10RENDERER_ENTRY NiD3D10ShaderProgramCreatorHLSL : 
    public NiD3D10ShaderProgramCreator
{
public:
    virtual ~NiD3D10ShaderProgramCreatorHLSL();

    // Shader assembly flags
    virtual unsigned int GetShaderCreationFlags() const;
    virtual void SetShaderCreationFlags(unsigned int uiFlags);

    // *** begin Emergent internal use only ***

    // These functions should only be called from NiD3D10ShaderProgramFactory

    // Shader compilation
    virtual bool CompileAndCreateVertexShader(ID3D10Blob* pkShaderCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10VertexShaderPtr& spVertexShader);
    virtual bool CreateVertexShader(ID3D10Blob* pkShaderByteCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10VertexShaderPtr& spVertexShader);
    virtual bool CompileAndCreateGeometryShader(ID3D10Blob* pkShaderCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, 
        const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
        unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
        NiD3D10GeometryShaderPtr& spGeometryShader);
    virtual bool CreateGeometryShader(ID3D10Blob* pkShaderByteCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, 
        const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
        unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
        NiD3D10GeometryShaderPtr& spGeometryShader);
    virtual bool CompileAndCreatePixelShader(ID3D10Blob* pkShaderCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10PixelShaderPtr& spPixelShader);
    virtual bool CreatePixelShader(ID3D10Blob* pkShaderByteCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10PixelShaderPtr& spPixelShader);

    // Shader recreation
    virtual bool RecreateVertexShader(NiD3D10VertexShader* pkVertexShader);
    virtual bool RecreateGeometryShader(
        NiD3D10GeometryShader* pkGeometryShader);
    virtual bool RecreatePixelShader(NiD3D10PixelShader* pkPixelShader);

    static void _SDMInit();
    static void _SDMShutdown();

    static NiD3D10ShaderProgramCreatorHLSL* GetInstance();
    static void Shutdown();
    // *** end Emergent internal use only ***

protected:
    NiD3D10ShaderProgramCreatorHLSL();

    static NiD3D10ShaderProgramCreatorHLSL* ms_pkCreator;
};

//#include "NiD3D10ShaderProgramCreatorHLSL.inl"

#endif  //NiD3D10ShaderProgramCreatorHLSL_H
