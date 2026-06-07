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

#ifndef NID3D10EFFECTSHADERLIBRARY_H
#define NID3D10EFFECTSHADERLIBRARY_H

#include "NiD3D10EffectShaderLibType.h"

#include <NiD3D10Headers.h>
#include <NiD3D10ShaderLibrary.h>
#include <NiTPointerList.h>
#include <NiTFixedStringMap.h>

NiSmartPointer(NiShaderLibraryDesc);
NiSmartPointer(NiD3D10EffectShaderLibrary);

class NiD3D10EffectFile;
class NiD3D10EffectTechniqueSet;

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectShaderLibrary : 
    public NiD3D10ShaderLibrary
{
protected:
    NiD3D10EffectShaderLibrary();

public:
    virtual ~NiD3D10EffectShaderLibrary();

    static NiD3D10EffectShaderLibrary* Create(NiRenderer* pkRenderer, 
        NiUInt32 uiDirectoryCount, const char* apcDirectories[], 
        bool bRecurseSubFolders);

    virtual NiShader* GetShader(NiRenderer* pkRenderer, 
        const char* pcName, unsigned int uiImplementation);
    virtual bool ReleaseShader(const char* pcName, 
        unsigned int uiImplementation);
    virtual bool ReleaseShader(NiShader* pkShader);

    bool LoadEffect(void* pvBuffer, NiUInt32 uiBufferSize, 
        const char* pcEffectFileName);

    // *** begin Emergent internal use only ***
    bool InsertD3D10EffectFileIntoList(NiD3D10EffectFile* pkD3D10EffectFile);
    NiUInt32 GetD3D10EffectFileCount();
    NiD3D10EffectFile* GetD3D10EffectFile(const char* pcName);
    bool LoadD3D10EffectFiles(const char* pcDirectory, 
        bool bRecurseSubFolders);

    ID3D10Device* GetTempDevice() const;
    // *** end Emergent internal use only ***

protected:
    void SetTempDevice(ID3D10Device* pkDevice);
    void ReleaseTempDevice();

    NiTPointerList<NiD3D10EffectFile*> m_kD3D10EffectFileList;
    NiTFixedStringMap<NiD3D10EffectTechniqueSet*> m_kD3D10EffectTechniqueMap;

    ID3D10Device* m_pkTempDevice;

    static NiD3D10EffectShaderLibrary* ms_pkLibrary;
};

#endif  //NID3D10EFFECTSHADERLIBRARY_H
