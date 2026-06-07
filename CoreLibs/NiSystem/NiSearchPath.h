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
//---------------------------------------------------------------------------

#ifndef NISEARCHPATH_H
#define NISEARCHPATH_H

#include "NiSystem.h"
#include "NiSystemLibType.h"
#include <NiMemObject.h>

class NISYSTEM_ENTRY NiSearchPath : public NiMemObject
{
public:
    NiSearchPath();
    virtual ~NiSearchPath();
    void SetFilePath(const char* pcFilePath);
    void SetReferencePath(const char* pcReferencePath);
    
    virtual void Reset();
    virtual bool GetNextSearchPath(char* pcPath, unsigned int uiStringLen);

    inline static void SetDefaultPath(const char* pcPath);
    inline static const char* GetDefaultPath();

protected:
    unsigned int m_uiNextPath;

    char m_acFilePath[NI_MAX_PATH];
    char m_acReferencePath[NI_MAX_PATH];

    static char ms_acDefPath[NI_MAX_PATH];
};

#include "NiSearchPath.inl"

#endif

