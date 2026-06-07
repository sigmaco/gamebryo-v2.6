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

#ifndef NID3DXEFFECTSHADERLIBRARY_H
#define NID3DXEFFECTSHADERLIBRARY_H

#include "NiD3DXEffectShaderLibType.h"

#include <NiD3DShaderLibrary.h>
#include <NiTPointerList.h>

NiSmartPointer(NiShaderLibraryDesc);

class NiD3DXEffectFile;
class NiD3DXEffectTechniqueSet;

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectShaderLibrary : 
    public NiD3DShaderLibrary
{
protected:
    NiD3DXEffectShaderLibrary();

public:
    virtual ~NiD3DXEffectShaderLibrary();

    static NiD3DXEffectShaderLibrary* Create(NiD3DRenderer* pkD3DRenderer, 
        int iDirectoryCount, const char* apcDirectories[], 
        bool bRecurseSubFolders);
    static void Shutdown();

    virtual void SetRenderer(NiD3DRenderer* pkRenderer);

    virtual NiShader* GetShader(NiRenderer* pkRenderer, 
        const char* pcName, unsigned int uiImplementation);
    virtual bool ReleaseShader(const char* pcName, 
        unsigned int uiImplementation);
    virtual bool ReleaseShader(NiShader* pkShader);

    virtual NiShaderLibraryDesc* GetShaderLibraryDesc();

    // *** begin Emergent internal use only ***
    virtual void SetShaderLibraryDesc(NiShaderLibraryDesc* pkLibDesc);

    bool InsertD3DXEffectFileIntoList(NiD3DXEffectFile* pkD3DXEffectFile);
    unsigned int GetD3DXEffectFileCount();
    NiD3DXEffectFile* GetD3DXEffectFile(const char* pcName);
    bool LoadD3DXEffectFiles(const char* pcDirectory, 
        bool bRecurseSubFolders);
    // *** end Emergent internal use only ***

protected:
    NiTPointerList<NiD3DXEffectFile*> m_kD3DXEffectFileList;
    NiTStringMap<NiD3DXEffectTechniqueSet*> m_kD3DXEffectTechniqueMap;

    static NiD3DXEffectShaderLibrary* ms_pkLibrary;
};

NiSmartPointer(NiD3DXEffectShaderLibrary);

#endif  //NID3DXEFFECTSHADERLIBRARY_H
