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

#ifndef XSI_GAMEBRYO_CONVERTER_SIMPLE_LOGGERS_H
#define XSI_GAMEBRYO_CONVERTER_SIMPLE_LOGGERS_H

#include "CrosswalkGamebryoLogger.h"

#include "SLTypes.h"
#include <SIBCString.h>

#include <list>

namespace epg
{
    //---------------------------------------------------------------------------
    /// Simple implementation of logger interface, accumulates error messages.
    ///
    /// This is the default logger. This default instance can be retrieved with GetLogger().
    /// This instance is automatically detroyed when the program exits.
    //---------------------------------------------------------------------------

    class AccumulatingLogger : public Logger
    {
    public:
        static AccumulatingLogger & GetLogger();

        AccumulatingLogger();

        // Logger interface.
        void StartConversion();
        void EndConversion();
        bool Log(LogLevel in_Level, const char * in_Msg);

        // AccumulatingLogger specific API.
        typedef std::list< std::pair<LogLevel,CSIBCString> > LogList;

        const LogList& GetLog() const;
        void ClearLog();

    private:
        LogList m_Logs;
    };

    //---------------------------------------------------------------------------
    /// Simple implementation of logger interface, prints to stdout.
    //---------------------------------------------------------------------------

    class StdoutLogger : public Logger
    {
    public:
        StdoutLogger(LogLevel in_MinPrintableLevel = LOG_INFO);

        // Logger interface.
        void StartConversion();
        void EndConversion();
        bool Log(LogLevel in_Level, const char * in_Msg);

        void SetMinPrintableLevel(LogLevel in_Level);
        LogLevel GetMinPrintableLevel() const;

        int GetDebugMessageCount() const;
        int GetInfoMessageCount() const;
        int GetWarningMessageCount() const;
        int GetErrorMessageCount() const;

    private:
        LogLevel m_MinPrintableLevel;
        int m_DebugMsgCount;
        int m_InfoMsgCount;
        int m_WarningMsgCount;
        int m_ErrorMsgCount;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_SIMPLE_LOGGERS_H */
