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

#ifndef XSI_GAMEBRYO_CONVERTER_LOGGER_H
#define XSI_GAMEBRYO_CONVERTER_LOGGER_H

namespace epg
{
    //---------------------------------------------------------------------------
    /// Severity level of a particular log entry.
    //---------------------------------------------------------------------------

    enum LogLevel
    {
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR
    };

    //---------------------------------------------------------------------------
    /// Logger interface.
    //---------------------------------------------------------------------------

    class Logger
    {
    public:
        /// Create an empty logger.
        Logger();

        /// Destroy a logger.
        virtual ~Logger();

        // @{
        // Surrounds the conversion between start and end events.
        virtual void StartConversion() = 0;
        virtual void EndConversion() = 0;
        // @}

        // @{
        // Log a message.
        // Return false if the logger determines that the error should be fatal.
        virtual bool Log(LogLevel in_Level, const char * in_Msg) = 0;
        bool Logf(LogLevel in_Level, const char * in_Msg, ...);
        // @}

    private:
        // Disable copying.
        Logger(const Logger &);
        void operator=(const Logger &);
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_LOGGER_H */
