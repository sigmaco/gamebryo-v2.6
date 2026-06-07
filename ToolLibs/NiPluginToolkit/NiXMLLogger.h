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

#ifndef NIXMLLOGGER_H
#define NIXMLLOGGER_H

#if defined(NI_RELEASE_MODE_LOGGING)
#   undef NI_RELEASE_MODE_LOGGING
#endif
#define NI_RELEASE_MODE_LOGGING 1

#include "NiPluginToolkitLibType.h"
#include <NiLog.h>
#include <NiString.h>

class NIPLUGINTOOLKIT_ENTRY NiXMLLogger
{
public:
    NiXMLLogger();
    ~NiXMLLogger(void);

    void CreateLog(const NiString& kFilename);
    void CloseLog();
    void LogElement(const NiString& kElementName, const NiString& kData);
    void LogData(const NiString& kData);
    void StartElement(const NiString& kElementName);
    void EndElement(const NiString& kElementName);

protected:
    int m_iLogID;
};

#endif NIXMLLOGGER_H