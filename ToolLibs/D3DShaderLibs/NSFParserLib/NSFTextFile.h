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

#ifndef NSFTEXTFILE_H
#define NSFTEXTFILE_H

#include "NSFParserLibLibType.h"
#include <NiMemObject.h>

class NSFPARSERLIB_ENTRY NSFTextFile : public NiMemObject
{
public:
    NSFTextFile();
    ~NSFTextFile();
    
    int Load(const char* pszFilename);

    inline char* GetFilename()
    {
        return m_pcFilename;
    };

    inline char* GetData()
    {
        return m_pcData;
    };

    inline unsigned int& GetSize()
    {
        return m_uiSize;
    };

    inline unsigned int& GetPos()
    {
        return m_uiPos;
    };

protected:
    char* m_pcFilename;
    char* m_pcData;
    unsigned int m_uiSize;
    unsigned int m_uiPos;
};

#endif  //NSFTEXTFILE_H
