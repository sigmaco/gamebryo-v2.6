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

#ifndef NID3DXEFFECTFACTORY_H
#define NID3DXEFFECTFACTORY_H

#include "NiD3DXEffectShaderLibType.h"

#include <NiD3DDefines.h>
#include <NiSmartPointer.h>
#include <NiTStringMap.h>

class NiD3DXEffectFile;

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectFactory : public NiMemObject
{
public:
    static NiD3DXEffectFactory* GetInstance(bool bCreateNew = true);

    //*** Helper functions
    // D3DXEffect creation
    static NiD3DXEffectFile* CreateD3DXEffectFromFile(
        const char* pcFileName);
    static NiD3DXEffectFile* CreateD3DXEffectFromCode(
        const char* pcShaderName, unsigned int uiCodeSize, void* pvCode);

    // Shader assembly flags
    static unsigned int GetD3DXEffectCreationFlags();
    static void SetD3DXEffectCreationFlags(unsigned int uiFlags);

    // *** begin Emergent internal use only ***
    static const char* GetD3DXEffectDirectory();
    static void SetD3DXEffectDirectory(const char* pcDirectory);

    bool LoadD3DXEffectFromFile(const char* pcFileName, 
        NiD3DXEffectFile*& pkEffectFile);
    bool LoadD3DXEffectFromCode(const char* pcEffectName, 
        unsigned int uiCodeSize, void* pvCode, 
        NiD3DXEffectFile*& pkEffectFile);

    bool LoadD3DXEffectFromFile_DX9(const char* pcFileName, 
        NiD3DXEffectFile*& pkEffectFile);
    inline bool LoadD3DXEffectFromCode_DX9(const char* pcEffectName, 
        unsigned int uiCodeSize, void* pvCode, 
        NiD3DXEffectFile*& pkEffectFile);

    // Find functions
    virtual NiD3DXEffectFile* GetD3DXEffectFile(const char* pcFileName) const;

    // Shader insertion/removal
    virtual void InsertD3DXEffectFileIntoMap(NiD3DXEffectFile* pkEffectFile);
    virtual void RemoveD3DXEffectFileFromMap(const char* pcName);

    static bool NiD3DXEffectFactory::ResolveD3DXEffectFileName(
        const char* pcOriginalName, char* pcTrueName, 
        unsigned int uiBufferSize);

    static void SetD3DRenderer(NiD3DRenderer* pkD3DRenderer);
    static void SetDevice(D3DDevicePtr pkD3DDevice);

    void HandleLostDevice(unsigned int uiCounter);
    void HandleResetDevice(unsigned int uiCounter);

    inline void DestroyRendererData();

    static void _SDMInit();
    static void _SDMShutdown();

    static HINSTANCE GetApplicationInstance();
    static void SetApplicationInstance(HINSTANCE hInstance);

    static void SetFXLSupport(bool bEnable);
    static bool GetFXLSupport();
    // *** end Emergent internal use only ***

protected:
    NiD3DXEffectFactory();
    virtual ~NiD3DXEffectFactory();

    void UpdateMacros();

    static NiD3DXEffectFactory* ms_pkFactory;

    static char* ms_pcD3DXEffectDirectory;

    // Maps for effect file names to effects.
    NiTStringPointerMap<NiD3DXEffectFile*>* m_pkD3DXEffectMap;

    static D3DDevicePtr ms_pkD3DDevice;
    static NiD3DRenderer* ms_pkD3DRenderer;

    static bool ms_bFXLSupport;

    unsigned int m_uiLostCounter;
    unsigned int m_uiResetCounter;

#if defined(WIN32)
    static HINSTANCE ms_hAppInstance;
#endif
};

typedef NiPointer<NiD3DXEffectFactory> NiD3DXEffectFactoryPtr;

#include "NiD3DXEffectFactory.inl"

#endif  //NID3DXEFFECTFACTORY_H
