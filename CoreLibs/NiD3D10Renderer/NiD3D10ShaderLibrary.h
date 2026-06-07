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

#ifndef NID3D10SHADERLIBRARY_H
#define NID3D10SHADERLIBRARY_H

#include "NiD3D10RendererLibType.h"

#include <NiShaderLibrary.h>
#include <NiShaderLibraryDesc.h>

class NID3D10RENDERER_ENTRY NiD3D10ShaderLibrary : public NiShaderLibrary
{
public:
    NiD3D10ShaderLibrary(const char* pcName);
    virtual ~NiD3D10ShaderLibrary();

    static void SetDirectoryInfo(const char* pcDirectory);
    static const char* GetDirectory();
    static void SetPlatform(unsigned int uiPlatform);
    static unsigned int GetPlatform();
    static void SetVersionInfo(const char* pcVersionName, 
        unsigned int uiVersion);
    static unsigned int GetVersionNumber(const char* pcVersionName);

    virtual NiShaderLibraryDesc* GetShaderLibraryDesc();

    // *** begin Emergent internal use only ***
    virtual void SetShaderLibraryDesc(NiShaderLibraryDesc* pkLibDesc);
    // *** end Emergent internal use only ***

protected:
    static char* ms_pcDirectory;
    static unsigned int ms_uiPlatform;
    static unsigned int ms_uiVSVersion;
    static unsigned int ms_uiGSVersion;
    static unsigned int ms_uiPSVersion;
    static unsigned int ms_uiUserVersion;

    NiShaderLibraryDescPtr m_spShaderLibraryDesc;
};

NiSmartPointer(NiD3D10ShaderLibrary);

#endif  //#ifndef NID3D10SHADERLIBRARY_H
