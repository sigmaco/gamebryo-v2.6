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

#ifndef NID3D10EFFECTLOADER_H
#define NID3D10EFFECTLOADER_H

#include "NiD3D10EffectShaderLibType.h"

#include <NiMain.h>

class NiShaderLibraryDesc;
class NiD3D10EffectShaderLibrary;

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectLoader : public NiMemObject
{
protected:
    NiD3D10EffectLoader();
    ~NiD3D10EffectLoader();

public:
    static NiD3D10EffectLoader* GetInstance(bool bCreateNew = true);
    static void Destroy();

    bool LoadAllD3D10EffectFiles(
        NiD3D10EffectShaderLibrary* pkLibrary, 
        const char* pcDirectory, 
        bool bRecurseDirectories);

    bool LoadEffect(
        NiD3D10EffectShaderLibrary* pkLibrary, 
        void* pvBuffer, 
        NiUInt32 uiBufferSize, 
        bool bCompiled,
        const char* pcEffectFileName);

    void AddFileExtension(const char* pcExtension, bool bCompiled);
    void RemoveFileExtension(const char* pcExtension);

    // *** end Emergent internal use only ***
    static char* GetShaderText(const char* pcShaderName,
        NiUInt32& uiLength);
    // *** begin Emergent internal use only ***

protected:
    NiUInt32 GetD3D10EffectFileCount() const;
    const char* GetFirstD3D10EffectFile() const;
    const char* GetNextD3D10EffectFile() const;

    void FindAllD3D10EffectFiles(const char* pcDirectory, 
        bool bRecurseDirectories);
    NiUInt32 FindAllD3D10EffectFilesInDirectory(
        const char* pcDirectory, 
        const NiFixedString& kExt, 
        bool bRecurseDirectories, 
        NiTPointerList<NiFixedString>* pkFileList);

    bool ProcessD3D10EffectFile(
        const char* pcFilename, 
        const NiFixedString& kExt,
        NiTPointerList<NiFixedString>* pkFileList);

    bool LoadStoredD3D10EffectFiles(NiD3D10EffectShaderLibrary* pkLibrary);

    static NiFixedString StandardizeExtension(const char* pcFilename);

    static NiD3D10EffectLoader* ms_pkLoader;

    mutable NiTListIterator m_kD3D10EffectListIter;
    NiTPointerList<NiFixedString> m_kD3D10EffectList;

    NiTFixedStringMap<bool> m_kShaderFileExtensions;
};

#endif  //NID3D10EFFECTLOADER_H
