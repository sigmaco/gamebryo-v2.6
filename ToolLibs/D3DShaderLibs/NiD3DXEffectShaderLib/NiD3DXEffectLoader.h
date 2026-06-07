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

#ifndef NID3DXEFFECTLOADER_H
#define NID3DXEFFECTLOADER_H

#include <NiMain.h>
#include <NiCriticalSection.h>
#include "NiD3DXInclude.h"

class NiShaderLibraryDesc;
class NiD3DXEffectShaderLibrary;

class NiD3DXEffectLoader : public NiMemObject
{
protected:
    NiD3DXEffectLoader();
    ~NiD3DXEffectLoader();

public:
    static NiD3DXEffectLoader* Create();
    static void Destroy();

    bool LoadAllD3DXEffectFiles(NiD3DXEffectShaderLibrary* pkLibrary, 
        const char* pcDirectory, bool bRecurseDirectories);

    // *** end Emergent internal use only ***
    static void* CreateEffectFromFile(void* pvD3DDevice,
        const char* pcShaderPath, unsigned int uiCreationFlags, 
        const D3DXMACRO* pkMacros, char*& pcError);

    static void* CreateEffectFromCode(void* pvD3DDevice, void* pvCode, 
        unsigned int uiCodeSize, unsigned int uiCreationFlags, 
        const D3DXMACRO* pkMacros, char*& pcError,
        ID3DXInclude* pkInclude = NULL);

    static char* GetShaderText(const char* pcShaderName,
        unsigned int& uiLength);
    // *** begin Emergent internal use only ***

protected:
    unsigned int GetD3DXEffectFileCount();
    const char* GetFirstD3DXEffectFile();
    const char* GetNextD3DXEffectFile();

    void FindAllD3DXEffectFiles(const char* pcDirectory, 
        bool bRecurseDirectories);
    unsigned int FindAllD3DXEffectFilesInDirectory(const char* pcDirectory, 
        const char* pcExt, bool bRecurseDirectories, 
        NiTPointerList<char*>* pkFileList);

    bool ProcessD3DXEffectFile(const char* pcFilename, const char* pcExt,
        NiTPointerList<char*>* pkFileList);

    bool LoadAllD3DXEffectFiles(NiD3DXEffectShaderLibrary* pkLibrary);

    static void* CreateEffectCompilerFromFile(const char* pcShaderPath, 
        unsigned int uiCreationFlags, const D3DXMACRO* pkMacros, 
        char*& pcError);
    static void* CreateEffectCompilerFromCode(void* pvCode, 
        unsigned int uiCodeSize, unsigned int uiCreationFlags, 
        const D3DXMACRO* pkMacros, char*& pcError,
        ID3DXInclude* pkInclude = NULL);

    static NiD3DXEffectLoader* ms_pkLoader;

    NiTListIterator m_kD3DXEffectListIter;
    NiTPointerList<char*> m_kD3DXEffectList;

    NiCriticalSection m_kCriticalSection;
    NiD3DXInclude m_kD3DXInclude;
};

#endif  //NID3DXEFFECTLOADER_H
