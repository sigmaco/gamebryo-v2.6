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

#ifndef NID3DSHADERPROGRAMFACTORY_H
#define NID3DSHADERPROGRAMFACTORY_H

#include <NiRefObject.h>
#include <NiSMartPointer.h>
#include <NiTStringMap.h>

#include "NiD3DDefines.h"
#include "NiD3DRendererHeaders.h"

NiSmartPointer(NiD3DShaderProgramCreator);
NiSmartPointer(NiD3DVertexShader);
NiSmartPointer(NiD3DPixelShader);

class NID3D_ENTRY NiD3DShaderProgramFactory : public NiRefObject
{
protected:
    static D3DDevicePtr ms_pkD3DDevice;
    static NiD3DRenderer* ms_pkD3DRenderer;
    static NiD3DRenderState* ms_pkD3DRenderState;

    // *** begin Emergent internal use only ***
    static void SetD3DDevice(D3DDevicePtr pkD3DDevice);
    static void SetD3DRenderState(NiD3DRenderState* pkRS);

public:
    static void SetD3DRenderer(NiD3DRenderer* pkD3DRenderer);

    static NiD3DShaderProgramFactory* Create();
    // *** end Emergent internal use only ***

protected:
    NiD3DShaderProgramFactory();

public:
    virtual ~NiD3DShaderProgramFactory();

    static NiD3DShaderProgramFactory* GetInstance();

    // Vertex shader creation
    static NiD3DVertexShader* CreateVertexShaderFromFile(
        const char* pcFileName, 
        const char* pcShaderName, 
        const char* pcEntryPoint, 
        const char* pcShaderTarget, 
        NiD3DVertexDeclaration hDecl, 
        unsigned int uiUsage, 
        bool bRecoverable = false);
    static NiD3DVertexShader* CreateVertexShaderFromBuffer(
        const void* pvBuffer, 
        unsigned int uiBufferSize, 
        const char* pcExt, 
        const char* pcShaderName, 
        const char* pcEntryPoint, 
        const char* pcShaderTarget, 
        NiD3DVertexDeclaration hDecl, 
        unsigned int uiUsage, 
        bool bRecoverable = false);
    // Pixel  shader creation
    static NiD3DPixelShader* CreatePixelShaderFromFile(
        const char* pcFileName, 
        const char* pcShaderName, 
        const char* pcEntryPoint, 
        const char* pcShaderTarget, 
        bool bRecoverable = false);
    static NiD3DPixelShader* CreatePixelShaderFromBuffer(
        const void* pvBuffer, 
        unsigned int uiBufferSize, 
        const char* pcExt, 
        const char* pcShaderName,
        const char* pcEntryPoint, 
        const char* pcShaderTarget, 
        bool bRecoverable = false);

    // Shader creation flags - apply to all shaders
    static unsigned int GetUniversalShaderCreationFlags();
    static void SetUniversalShaderCreationFlags(unsigned int uiFlags);

    const char* GetFirstProgramDirectory(NiTListIterator& kIter);
    const char* GetNextProgramDirectory(NiTListIterator& kIter);
    void AddProgramDirectory(const char* pcDirectory);
    void RemoveProgramDirectory(const char* pcDirectory);
    void RemoveAllProgramDirectories();

    static bool RegisterShaderCreator(const char* pcExt, 
        NiD3DShaderProgramCreator* pkCreator);
    static void UnregisterShaderCreator(NiD3DShaderProgramCreator* pkCreator);
    static NiD3DShaderProgramCreator* GetShaderCreator(const char* pcFilename);

    // *** begin Emergent internal use only ***
    static void Shutdown();

    // Shader recreation
    static bool RecreateVertexShader(NiD3DVertexShader* pkVertexShader);
    static bool RecreatePixelShader(NiD3DPixelShader* pkPixelShader);

    // Shader releasing
    static void ReleaseVertexShader(NiD3DVertexShader* pkShader);
    static void ReleasePixelShader(NiD3DPixelShader* pkShader);

    // Find functions
    NiD3DVertexShader* GetVertexShader(const char* pcShaderName); 
    NiD3DPixelShader* GetPixelShader(const char* pcShaderName); 

    // Shader insertion/removal
    void InsertVertexShaderIntoMap(NiD3DVertexShader* pkShader);
    void RemoveVertexShaderFromMap(const char* pcShaderName);
    void InsertPixelShaderIntoMap(NiD3DPixelShader* pkShader);
    void RemovePixelShaderFromMap(const char* pcShaderName);

    static bool NiD3DShaderProgramFactory::ResolveShaderFileName(
        const char* pcOriginalName, 
        char* pcTrueName, 
        unsigned int uiStringLen);

    static void _SDMInit();
    static void _SDMShutdown();

    // *** end Emergent internal use only ***

protected:
    static NiD3DShaderProgramFactory* ms_pkFactory;

    // We are only allowing one directory at this stage
    NiTPointerList<char*> m_kProgramDirectories;

    // Maps for shader program names to shaders.
    NiTStringPointerMap<NiD3DVertexShader*>* m_pkVertexShaderMap;
    NiTStringPointerMap<NiD3DPixelShader*>* m_pkPixelShaderMap;

    static void CreateShaderCreatorMap();
    static NiTStringPointerMap<NiD3DShaderProgramCreator*>* 
        ms_pkShaderCreators;
};

typedef NiPointer<NiD3DShaderProgramFactory> NiD3DShaderProgramFactoryPtr;

#include "NiD3DShaderProgramFactory.inl"

#endif  //#ifndef NID3DSHADERPROGRAMFACTORY_H
