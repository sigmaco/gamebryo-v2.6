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

#ifndef NIWIN32FILEFINDER_H
#define NIWIN32FILEFINDER_H
#include <NiMemObject.h>
#include <NiTArray.h>

#include "NiMaterialToolkitLibType.h"

class NIMATERIALTOOLKIT_ENTRY NiWin32FoundFile : public NiMemObject
{
public:
    NiWin32FoundFile() 
    {
        m_strPath[0] ='\0';
        m_uiSize = 0;
    };
    char m_strPath[MAX_PATH];
    unsigned int m_uiSize;
    FILETIME ftLastWriteTime;
};

class NIMATERIALTOOLKIT_ENTRY NiWin32FileFinder : public NiMemObject
{
public:
    NiWin32FileFinder(const char* strPath, bool bRecurseSubDirs = true, 
        const char* pcExtension = "\0"); 

    NiWin32FoundFile* GetNextFile();

    bool HasMoreFiles();

    void ResetIterator();

    ~NiWin32FileFinder();

protected:
    void AddFile(const char* strPath, unsigned int uiSize, 
        FILETIME kLastWriteTime);
    void Find(const char* strPath, bool bRecurseSubDirs, 
        const char* pcExtension);

    NiTPrimitiveArray<NiWin32FoundFile*> m_kFoundFiles;
    unsigned int m_uiIterator;
};

#endif //NIWIN32FILEFINDER_H
