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

#ifndef NSBD3D10SHADERLIBRARY_H
#define NSBD3D10SHADERLIBRARY_H

#include "NSBShaderLibLibType.h"

#include <NSBD3D10Shader.h>
#include <NiD3D10ShaderLibrary.h>
#include <NiTPointerList.h>

NiSmartPointer(NiShaderLibraryDesc);
NiSmartPointer(NSBD3D10ShaderLibrary);

class NSBSHADERLIB_ENTRY NSBD3D10ShaderLibrary : public NiD3D10ShaderLibrary
{
protected:
    NSBD3D10ShaderLibrary();

public:
    virtual ~NSBD3D10ShaderLibrary();

    static NSBD3D10ShaderLibrary* Create(int iDirectoryCount, 
        const char* pacDirectories[], bool bRecurseSubFolders);
    static void Shutdown();
    static NSBD3D10ShaderLibrary* GetLibrary();

    virtual NiShader* GetShader(NiRenderer* pkRenderer, 
        const char* pcName, unsigned int uiImplementation);
    virtual bool ReleaseShader(const char* pcName, 
        unsigned int uiImplementation);
    virtual bool ReleaseShader(NiShader* pkShader);

    virtual NiShaderLibraryDesc* GetShaderLibraryDesc();

    // *** begin Emergent internal use only ***
    virtual void SetShaderLibraryDesc(NiShaderLibraryDesc* pkLibDesc);

    bool InsertNSBD3D10ShaderIntoList(NSBD3D10Shader* pkNSBD3D10Shader);
    unsigned int GetNSBD3D10ShaderCount();
    NSBD3D10Shader* GetNSBD3D10Shader(const char* pcName);
    bool LoadNSBD3D10Shaders(const char* pcDirectory, 
        bool bRecurseSubFolders);
    // *** end Emergent internal use only ***

protected:
    NiTPointerList<NSBD3D10ShaderPtr> m_kNSBD3D10ShaderList;
};

#endif  //NSBD3D10SHADERLIBRARY_H
