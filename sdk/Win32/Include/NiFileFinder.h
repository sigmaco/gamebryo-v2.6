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


#ifndef NIFILEFINDER_H
#define NIFILEFINDER_H
#include <NiTArray.h>
#include <NiString.h>
#include "NiPluginToolkitLibType.h"

class NIPLUGINTOOLKIT_ENTRY NiFoundFile : public NiMemObject
{
    public:
        NiFoundFile() {m_strPath = NiString("\0"); m_uiSize = 0;};
        NiString m_strPath;
        unsigned int m_uiSize;
};

class NIPLUGINTOOLKIT_ENTRY NiFileFinder : public NiMemObject
{
    public:
        NiFileFinder(const char* pcPath, bool bRecurseSubDirs = true, 
            const char* pcExtension = "\0"); 

        NiFoundFile* GetNextFile();

        bool HasMoreFiles();

        void ResetIterator();

        ~NiFileFinder();

        unsigned int GetFileCount();

    protected:
        void AddFile(const char* pcPath, unsigned int uiSize);
        void Find(NiString strPath, bool bRecurseSubDirs, 
            const char* pcExtension);

        NiTPrimitiveArray<NiFoundFile*> m_kFoundFiles;
        unsigned int m_uiIterator;
};

#endif