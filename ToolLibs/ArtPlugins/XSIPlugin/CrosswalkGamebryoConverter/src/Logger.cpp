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

#include "CrosswalkGamebryoSimpleLoggers.h"

#include <stdarg.h>

namespace epg
{
    //---------------------------------------------------------------------------
    // Logger.
    //---------------------------------------------------------------------------

    Logger::Logger()
    {
    }
    //---------------------------------------------------------------------------
    Logger::~Logger()
    {
        // Do nothing.
    }
    //---------------------------------------------------------------------------
    bool Logger::Logf(LogLevel in_Level, const char * in_ErrorMsg, ...)
    {
        va_list args;
        va_start(args, in_ErrorMsg);
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), in_ErrorMsg, args);
        return Log(in_Level, buffer);
    }

    //---------------------------------------------------------------------------
    // Automatically cleaned-up instance of accumulating logger; default logger.
    //---------------------------------------------------------------------------

    namespace
    {
        AccumulatingLogger * g_AccumulatingLogger;

        struct AccumulatingLoggerCleanup
        {
            ~AccumulatingLoggerCleanup()
            {
                delete g_AccumulatingLogger;
                g_AccumulatingLogger = 0;
            }
        };

        AccumulatingLoggerCleanup cleaner;
    }

    //---------------------------------------------------------------------------
    // Simple implementation of logger interface, accumulates error messages.
    //---------------------------------------------------------------------------

    AccumulatingLogger::AccumulatingLogger()
    {
        // Do nothing.
    }
    //---------------------------------------------------------------------------
    AccumulatingLogger & AccumulatingLogger::GetLogger()
    {
        if (! g_AccumulatingLogger)
            g_AccumulatingLogger = new AccumulatingLogger;

        return *g_AccumulatingLogger;
    }
    //---------------------------------------------------------------------------
    void AccumulatingLogger::StartConversion()
    {
        ClearLog();
        Log(LOG_INFO, "Conversion started.");
    }
    //---------------------------------------------------------------------------
    void AccumulatingLogger::EndConversion()
    {
        Log(LOG_INFO, "Conversion ended.");
    }
    //---------------------------------------------------------------------------
    bool AccumulatingLogger::Log(LogLevel in_Level, const char * in_Msg)
    {
        m_Logs.push_back(std::make_pair(in_Level, CSIBCString(in_Msg)));
        return in_Level < LOG_ERROR;
    }
    //---------------------------------------------------------------------------
    const AccumulatingLogger::LogList& AccumulatingLogger::GetLog() const
    {
        return m_Logs;
    }
    //---------------------------------------------------------------------------
    void AccumulatingLogger::ClearLog()
    {
        m_Logs.clear();
    }

    //---------------------------------------------------------------------------
    // Simple implementation of logger interface, prints to stderr.
    //---------------------------------------------------------------------------

    StdoutLogger::StdoutLogger(LogLevel in_MinPrintableLevel)
        : m_MinPrintableLevel( in_MinPrintableLevel )
        , m_DebugMsgCount(0)
        , m_InfoMsgCount(0)
        , m_WarningMsgCount(0)
        , m_ErrorMsgCount(0)
    {
        // Do nothing.
    }
    //---------------------------------------------------------------------------
    void StdoutLogger::StartConversion()
    {
        m_DebugMsgCount   = 0;
        m_InfoMsgCount    = 0;
        m_WarningMsgCount = 0;
        m_ErrorMsgCount   = 0;

        Log(LOG_INFO, "Conversion started.");
    }
    //---------------------------------------------------------------------------
    void StdoutLogger::EndConversion()
    {
        Log(LOG_INFO, "Conversion ended.");
    }
    //---------------------------------------------------------------------------
    bool StdoutLogger::Log(LogLevel in_Level, const char * in_Msg)
    {
        const char * levelName;
        switch (in_Level)
        {
            case LOG_DEBUG:     ++m_DebugMsgCount;
                                 levelName = "Debug  "; break;
            case LOG_INFO:       ++m_InfoMsgCount;
                                 levelName = "Info   "; break;
            case LOG_WARNING:    ++m_WarningMsgCount;
                                 levelName = "Warning"; break;
            case LOG_ERROR:      ++m_ErrorMsgCount;
                                 levelName = "Error  "; break;
            default:             levelName = "???    "; break;
        }

        if (in_Level >= m_MinPrintableLevel)
            printf("%s: %s\n", levelName, in_Msg);

        return in_Level < LOG_ERROR;
    }
    //---------------------------------------------------------------------------
    void StdoutLogger::SetMinPrintableLevel(LogLevel in_Level)
    {
        m_MinPrintableLevel = in_Level;
    }
    //---------------------------------------------------------------------------
    LogLevel StdoutLogger::GetMinPrintableLevel() const
    {
        return m_MinPrintableLevel;
    }
    //---------------------------------------------------------------------------
    int StdoutLogger::GetDebugMessageCount() const
    {
        return m_DebugMsgCount;
    }
    //---------------------------------------------------------------------------
    int StdoutLogger::GetInfoMessageCount() const
    {
        return m_InfoMsgCount;
    }
    //---------------------------------------------------------------------------
    int StdoutLogger::GetWarningMessageCount() const
    {
        return m_WarningMsgCount;
    }
    //---------------------------------------------------------------------------
    int StdoutLogger::GetErrorMessageCount() const
    {
        return m_ErrorMsgCount;
    }
    //---------------------------------------------------------------------------
}
