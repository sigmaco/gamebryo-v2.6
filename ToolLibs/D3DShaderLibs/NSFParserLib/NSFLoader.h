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

#ifndef NSFLOADER_H
#define NSFLOADER_H

#include <NiMain.h>

class NiShaderLibraryDesc;

class NSFLoader : public NiMemObject
{
protected:
    NSFLoader();

public:
    ~NSFLoader();

    static NSFLoader* Create();
    static void Destroy();

    unsigned int ParseAllNSFFiles(const char* pszDirectory, 
        bool bRecurseDirectories);

    unsigned int GetTextFileCount();
    const char* GetFirstTextFile();
    const char* GetNextTextFile();

protected:
    void FindAllNSFTextFiles(const char* pszDirectory, 
        bool bRecurseDirectories);

    unsigned int LoadAllNSFFilesInDirectory(const char* pszDirectory, 
        const char* pszExt, bool bRecurseDirectories,
        NiTPointerList<char*>* pkFileList);
    bool ProcessNSFFile(const char* pszFilename, const char* pszExt,
        NiTPointerList<char*>* pkFileList);

    unsigned int LoadAllNSFTextFiles();

    static NSFLoader* ms_pkLoader;

    NiTListIterator m_kNSFTextIter;
    NiTPointerList<char*> m_kNSFTextList;
};

#endif  //NSFLOADER_H
