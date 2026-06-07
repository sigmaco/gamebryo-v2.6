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

#ifndef NID3DSHADERPROGRAMCREATORASM_H
#define NID3DSHADERPROGRAMCREATORASM_H

#include "NiD3DShaderProgramCreator.h"
#include "NiD3DShaderProgramFactory.h"

class NID3D_ENTRY NiD3DShaderProgramCreatorAsm : 
    public NiD3DShaderProgramCreator
{
public:
    virtual ~NiD3DShaderProgramCreatorAsm();

    // Shader assembly flags
    virtual unsigned int GetShaderCreationFlags() const;
    virtual void SetShaderCreationFlags(unsigned int uiFlags);

    // *** begin Emergent internal use only ***

    // These functions should only be called from NiD3DShaderProgramFactory

    // Vertex shader creation
    virtual NiD3DVertexShader* CreateVertexShaderFromFile(
        const char* pcFileName, 
        const char* pcShaderName, 
        const char* pcEntryPoint, 
        const char* pcShaderTarget, 
        NiD3DVertexDeclaration hDecl, 
        unsigned int uiUsage,
        bool bRecoverable = false);
    virtual NiD3DVertexShader* CreateVertexShaderFromBuffer(
        const void* pvBuffer, 
        unsigned int uiBufferSize, 
        const char* pcShaderName, 
        const char* pcEntryPoint, 
        const char* pcShaderTarget, 
        NiD3DVertexDeclaration hDecl, 
        unsigned int uiUsage, 
        bool bRecoverable = false);
    // Pixel  shader creation
    virtual NiD3DPixelShader* CreatePixelShaderFromFile(
        const char* pcFileName, 
        const char* pcShaderName, 
        const char* pcEntryPoint, 
        const char* pcShaderTarget, 
        bool bRecoverable = false);
    virtual NiD3DPixelShader* CreatePixelShaderFromBuffer(
        const void* pvBuffer, 
        unsigned int uiBufferSize, 
        const char* pcShaderName, 
        const char* pcEntryPoint, 
        const char* pcShaderTarget, 
        bool bRecoverable = false);

    // Shader recreation
    virtual bool RecreateVertexShader(NiD3DVertexShader* pkVertexShader);
    virtual bool RecreatePixelShader(NiD3DPixelShader* pkPixelShader);

    static void _SDMInit();
    static void _SDMShutdown();

    static NiD3DShaderProgramCreatorAsm* GetInstance();
    static void Shutdown();
    // *** end Emergent internal use only ***

protected:
    NiD3DShaderProgramCreatorAsm();

    virtual bool LoadShaderCodeFromFile(
        const char* pcFileName, 
        void*& pvCode, 
        unsigned int& uiCodeSize, 
        bool bRecoverable);
    virtual bool LoadShaderCodeFromBuffer(
        const void* pvBuffer, 
        unsigned int uiBufferSize, 
        void*& pvCode, 
        unsigned int& uiCodeSize, 
        bool bRecoverable);

    static NiD3DShaderProgramCreatorAsm* ms_pkCreator;
};

#include "NiD3DShaderProgramCreatorAsm.inl"

#endif  //NID3DSHADERPROGRAMCREATORASM_H
