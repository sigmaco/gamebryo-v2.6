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

#ifndef NIMATERIALTOOLKIT_H
#define NIMATERIALTOOLKIT_H

#pragma warning( disable : 4786)

#include "NiMaterialToolkitLibType.h"
#include <NiMemObject.h>
#include <NiShaderError.h>
#include <NiTStringMap.h>
#include "NiMaterialHelpers.h"

class NiShaderDesc;
class NiMaterialLibrary;

class NIMATERIALTOOLKIT_ENTRY NiMaterialToolkit : public NiMemObject
{
public:   
    static NiMaterialToolkit* CreateToolkit();
    static void DestroyToolkit();
    static NiMaterialToolkit* GetToolkit();

    static void EnableMessageBoxes(bool bEnable);

    bool LoadFromDLL(const char* pcShaderPath);
    
    unsigned int GetLibraryCount();
    NiMaterialLibrary* GetLibraryAt(unsigned int ui);

    const char* GetAppStringForMaterialDesc(const char* pcName);
    bool SetAppStringForMaterialDesc(const char* pcName, 
        const char* pcAppDesc);

    void SetMaterialDirectory(const char* pcDir, bool bAppend = false);
    static void UpdateMaterialDirectory();

    NiShaderDesc* GetMaterialDesc(const char* pcName);
    
    static void ReloadShaders();
    static void UnloadShaders();

protected:
    class NiMaterialDescContainer : public NiMemObject
    {
    public:
        NiMaterialDescContainer();
        ~NiMaterialDescContainer();
        NiShaderDesc* m_pkDesc;
        char* m_pcApplicationDescription;
    };

    NiMaterialToolkit();
    NiMaterialToolkit(NiMaterialToolkit& kToolkit);
    ~NiMaterialToolkit();
    bool ParseNPShaders(const char* pcLibraryPath, 
        const char* pcShaderPath);
    bool LoadShaders();
    void LoadMaterials();
    void UnIndex();

    void SetWorkingDirectoryForMaterials(const char* pcDir);
    void LoadFragmentMaterialCaches();
    void CreateFragmentMaterialCaches();

    static NiMaterialToolkit* ms_pkThis;
    char* m_pcMaterialPath;

    char* m_pcShaderProgramPath;
    bool m_bAppendSubdir;
    bool m_bUpdateShaderProgramDir;

    NiTStringPointerMap<NiMaterialDescContainer*> m_kMaterialDescCache;

    static unsigned int NiMaterialToolkit::ShaderErrorCallback(
        const char* pcError, NiShaderError eError, bool bRecoverable);

    static bool ms_bMessageBoxesEnabled;
};
#endif