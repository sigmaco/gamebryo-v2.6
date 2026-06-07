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

#ifndef NSBLOADER_H
#define NSBLOADER_H

#include <NiMain.h>

class NiShaderLibraryDesc;
class NSBShaderLibrary;

#ifdef WIN32
class NSBD3D10ShaderLibrary;
#endif

class NSBLoader : public NiMemObject
{
protected:
    NSBLoader();

public:
    ~NSBLoader();

    static NSBLoader* Create();
    static void Destroy();

    bool LoadAllNSBFiles(NSBShaderLibrary* pkLibrary, 
        const char* pszDirectory, bool bRecurseDirectories);

#ifdef WIN32
    bool LoadAllNSBD3D10Files(NSBD3D10ShaderLibrary* pkLibrary,
        const char* pszDirectory, bool bRecurseDirectories);
#endif

    unsigned int GetBinaryFileCount();
    const char* GetFirstBinaryFile();
    const char* GetNextBinaryFile();

protected:
    void FindAllNSBFiles(const char* pszDirectory, 
        bool bRecurseDirectories);
    unsigned int LoadAllNSBFilesInDirectory(const char* pszDirectory,
        const char* pszExt, bool bRecurseDirectories, 
        NiTPointerList<char*>* pkFileList);

    bool ProcessNSBFile(const char* pszFilename, const char* pszExt,
        NiTPointerList<char*>* pkFileList);

    bool LoadAllNSBFiles(NSBShaderLibrary* pkLibrary);

#ifdef WIN32
    bool LoadAllNSBD3D10Files(NSBD3D10ShaderLibrary* pkLibrary);
#endif

    static NSBLoader* ms_pkLoader;

    NiTListIterator m_kBinaryListIter;
    NiTPointerList<char*> m_kBinaryList;
};

#endif  //NSBLOADER_H
