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

#ifndef NSBSHADERLIBRARY_H
#define NSBSHADERLIBRARY_H

#include "NSBShaderLibLibType.h"

#include <NSBShader.h>
#include <NiD3DShaderLibrary.h>
#include <NiTPointerList.h>

NiSmartPointer(NiShaderLibraryDesc);
NiSmartPointer(NSBShaderLibrary);

class NSBSHADERLIB_ENTRY NSBShaderLibrary : public NiD3DShaderLibrary
{
protected:
    NSBShaderLibrary();

public:
    virtual ~NSBShaderLibrary();

    static NSBShaderLibrary* Create(int iDirectoryCount, 
        const char* pacDirectories[], bool bRecurseSubFolders);
    static void Shutdown();
    static NSBShaderLibrary* GetLibrary();

    virtual NiShader* GetShader(NiRenderer* pkRenderer, 
        const char* pcName, unsigned int uiImplementation);
    virtual bool ReleaseShader(const char* pcName, 
        unsigned int uiImplementation);
    virtual bool ReleaseShader(NiShader* pkShader);

    virtual NiShaderLibraryDesc* GetShaderLibraryDesc();

    // *** begin Emergent internal use only ***
    virtual void SetShaderLibraryDesc(NiShaderLibraryDesc* pkLibDesc);

    bool InsertNSBShaderIntoList(NSBShader* pkNSBShader);
    unsigned int GetNSBShaderCount();
    NSBShader* GetNSBShader(const char* pcName);
    bool LoadNSBShaders(const char* pcDirectory, 
        bool bRecurseSubFolders);
    // *** end Emergent internal use only ***

protected:
//    static NSBShaderLibraryPtr ms_spShaderLibrary;
    NiTPointerList<NSBShaderPtr> m_kNSBShaderList;
};

#endif  //NSBSHADERLIBRARY_H
