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

#ifndef NID3DSHADERPROGRAMCREATORCG_H
#define NID3DSHADERPROGRAMCREATORCG_H

#include "NiCgShaderLibType.h"

#include "NiD3DShaderProgramCreator.h"
#include "NiD3DShaderProgramFactory.h"

class NICGSHADERLIB_ENTRY NiD3DShaderProgramCreatorCg : 
    public NiD3DShaderProgramCreator
{
public:
    virtual ~NiD3DShaderProgramCreatorCg();

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

    static NiD3DShaderProgramCreatorCg* GetInstance();
    static void Shutdown();
    // *** end Emergent internal use only ***

protected:
    NiD3DShaderProgramCreatorCg();

    virtual bool LoadShaderCodeFromFile(
        const char* pcFileName, 
        const char* pcEntryPoint, 
        CGprofile eProfile,
        void*& pvCode, 
        unsigned int& uiCodeSize, 
        void*& pvConstantData, 
        unsigned int& uiConstantSize, 
        CGprogram& kProgram, 
        bool bRecoverable);
    virtual bool LoadShaderCodeFromBuffer(
        const void* pvBuffer, 
        const char* pcEntryPoint, 
        CGprofile eProfile, 
        void*& pvCode, 
        unsigned int& uiCodeSize, 
        void*& pvConstantData, 
        unsigned int& uiConstantSize, 
        CGprogram& kProgram, 
        bool bRecoverable);

    CGprofile GetEnumFromProfileString(const char* pcShaderTarget);
    char* m_pcShaderTargetTemp;
    unsigned int m_uiShaderTargetTempSize;

    void* m_pvConstantBuffer;
    unsigned int m_uiConstantBufferSize;

    static NiD3DShaderProgramCreatorCg* ms_pkCreator;
};

#include "NiD3DShaderProgramCreatorCg.inl"

#endif  //NID3DSHADERPROGRAMCREATORCG_H
