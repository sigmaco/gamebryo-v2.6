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

#ifndef NID3D10EFFECTFACTORY_H
#define NID3D10EFFECTFACTORY_H

#include "NiD3D10EffectShaderLibType.h"
#include "NiD3D10Include.h"

#include <NiCriticalSection.h>
#include <NiSmartPointer.h>
#include <NiTStringMap.h>

class NiD3D10EffectFile;

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectFactory : public NiMemObject
{
public:
    static NiD3D10EffectFactory* GetInstance(bool bCreateNew = true);

    //*** Helper functions
    // D3D10Effect creation
    static NiD3D10EffectFile* LoadD3D10EffectFromMemory(
        ID3D10Device* pkDevice,
        const char* pcShaderName, 
        NiUInt32 uiCodeSize, 
        void* pvCode,
        bool bCompiled,
        const char* pcPathToFile = NULL);

    // Effect compilation flags
    static NiUInt32 GetD3D10EffectHLSLFlags();
    static void SetD3D10EffectHLSLFlags(NiUInt32 uiHLSLFlags);

    static NiUInt32 GetD3D10EffectFXFlags();
    static void SetD3D10EffectFXFlags(NiUInt32 uiFXFlags);

    // *** begin Emergent internal use only ***
    static const char* GetD3D10EffectDirectory();
    static void SetD3D10EffectDirectory(const char* pcDirectory);

    NiD3D10EffectFile* CompileD3D10Effect(
        const char* pcEffectName, 
        NiUInt32 uiCodeSize, 
        void* pvCode,
        bool bCompiled,
        const char* pcPathToFile = NULL);

    inline bool CreateD3D10Effect(
        ID3D10Device* pkDevice,
        NiD3D10EffectFile* pkD3D10EffectFile,
        bool bParse = true);

    // Find functions
    virtual NiD3D10EffectFile* GetD3D10EffectFile(
        const char* pcFileName) const;

    // Shader insertion/removal
    virtual void InsertD3D10EffectFileIntoMap(NiD3D10EffectFile* pkEffectFile);
    virtual void RemoveD3D10EffectFileFromMap(const char* pcName);

    static bool NiD3D10EffectFactory::ResolveD3D10EffectFileName(
        const char* pcOriginalName, 
        char* pcTrueName, 
        unsigned int uiBufferSize);

    void DestroyRendererData();

    static void _SDMInit();
    static void _SDMShutdown();

    static void SetFXLSupport(bool bEnable);
    static bool GetFXLSupport();
    // *** end Emergent internal use only ***

protected:
    NiD3D10EffectFactory();
    virtual ~NiD3D10EffectFactory();

    void UpdateMacros();

    static NiD3D10EffectFactory* ms_pkFactory;

    static NiUInt32 ms_uiFXFlags;

    static char* ms_pcD3D10EffectDirectory;

    // Maps for effect file names to effects.
    NiTStringPointerMap<NiD3D10EffectFile*>* m_pkD3D10EffectMap;

    static bool ms_bFXLSupport;

    NiCriticalSection m_kCriticalSection;
    NiD3D10Include m_kD3D10Include;

    unsigned int m_uiLostCounter;
    unsigned int m_uiResetCounter;
};

NiSmartPointer(NiD3D10EffectFactory);

#include "NiD3D10EffectFactory.inl"

#endif  //NID3D10EFFECTFACTORY_H
