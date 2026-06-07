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

#ifndef NID3D10SHADERPROGRAMCREATOR_H
#define NID3D10SHADERPROGRAMCREATOR_H

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"

#include <NiMemObject.h>
#include <NiSmartPointer.h>

NiSmartPointer(NiD3D10GeometryShader);
NiSmartPointer(NiD3D10PixelShader);
NiSmartPointer(NiD3D10VertexShader);

class NID3D10RENDERER_ENTRY NiD3D10ShaderProgramCreator : public NiMemObject
{
public:
    virtual ~NiD3D10ShaderProgramCreator();

    // Shader creation flags - apply to all shaders
    static unsigned int GetUniversalShaderCreationFlags();
    static void SetUniversalShaderCreationFlags(unsigned int uiFlags);

    // Shader creation flags - apply only to specific shaders
    virtual unsigned int GetShaderCreationFlags() const = 0;
    virtual void SetShaderCreationFlags(unsigned int uiFlags) = 0;

    // *** begin Emergent internal use only ***
    
    // These functions should only be called from NiD3D10ShaderProgramFactory

    virtual bool CompileAndCreateVertexShader(ID3D10Blob* pkShaderByteCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10VertexShaderPtr& spVertexShader) = 0;
    virtual bool CreateVertexShader(ID3D10Blob* pkShaderByteCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10VertexShaderPtr& spVertexShader) = 0;
    virtual bool CompileAndCreateGeometryShader(ID3D10Blob* pkShaderByteCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, 
        const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
        unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
        NiD3D10GeometryShaderPtr& spGeometryShader) = 0;
    virtual bool CreateGeometryShader(ID3D10Blob* pkShaderByteCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, 
        const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
        unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
        NiD3D10GeometryShaderPtr& spGeometryShader) = 0;
    virtual bool CompileAndCreatePixelShader(ID3D10Blob* pkShaderByteCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10PixelShaderPtr& spPixelShader) = 0;
    virtual bool CreatePixelShader(ID3D10Blob* pkShaderByteCode, 
        const char* pcFileName, const D3D10_SHADER_MACRO* pkDefines, 
        ID3D10Include* pkInclude, const char* pcEntryPoint, 
        const char* pcShaderTarget, unsigned int uiFlags, 
        const char* pcShaderName, NiD3D10PixelShaderPtr& spPixelShader) = 0;

    // Shader recreation
    virtual bool RecreateVertexShader(NiD3D10VertexShader* pkVertexShader) = 0;
    virtual bool RecreateGeometryShader(
        NiD3D10GeometryShader* pkGeometryShader) = 0;
    virtual bool RecreatePixelShader(NiD3D10PixelShader* pkPixelShader) = 0;

    // *** end Emergent internal use only ***

protected:
    NiD3D10ShaderProgramCreator();
};

#include "NiD3D10ShaderProgramCreator.inl"

#endif  //NID3D10SHADERPROGRAMCREATOR_H
