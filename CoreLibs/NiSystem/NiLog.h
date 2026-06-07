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

#ifndef NILOG_H
#define NILOG_H

#include <NiSystem.h>
#include "NiLogBehavior.h"
#include "NiSystemLibType.h"
#include "NiCriticalSection.h"

#if defined(_WII)
    #include "NiWiiRemoteFile.h"
#endif

//---------------------------------------------------------------------------
// The NiLogger build configurations are:
// 1) Normal Debug and Release mode -- NiLogger is enabled, NILOG, and 
//    NILOGDIRECT calls are compiled in.
// 2) Shipping mode non-dll or with NI_LOGGER_DISABLE defined -- NiLogger is
//    disabled, NILOG, and NILOGDIRECT calls are optimized out.  Any 
//    references to NiLogger are undefined.

// Message types are defined here.  Note, the types are #define's
// instead of NiLogger enums so that when logging is disabled, 
// NILOG calls will compile but be optimized out.
#define NIMESSAGE_GENERAL_0 0
#define NIMESSAGE_GENERAL_1 1
#define NIMESSAGE_GENERAL_2 2 
#define NIMESSAGE_MEMORY_0 3
#define NIMESSAGE_MEMORY_1 4
#define NIMESSAGE_MEMORY_2 5
#define NIMESSAGE_MEMORY_TIMESTAMP 6
#define NIMESSAGE_RESERVED_FOR_LOGDIRECT 7
#define NIMESSAGE_MAX_INTERNAL_INDEX NIMESSAGE_RESERVED_FOR_LOGDIRECT
#define NIMESSAGE_MAX_TYPES 32
// user-defined types should start at NIMESSAGE_MAX_INTERNAL_INDEX+1
// and be less than NIMESSAGE_MAX_TYPES.

// The following macros aren't pretty, but they make the syntax for using 
// the log system much easier.  The only ones that should be used directly
// are NILOG and NILOGDIRECT.
#define LOG_CONCAT2(name, l) name##l
#define LOG_CONCAT1(name, l) LOG_CONCAT2(name, l)
#if defined (_MSC_VER) && (_MSC_VER >= 1310)
    #define INSERT_UNIQUE_NAME(name) LOG_CONCAT1(name,__COUNTER__)
#else //#if defined (_MSC_VER) && (_MSC_VER >= 1310)
    // IMPORTANT NOTE: This macro do not expand properly if compiled with
    // Visual Studio's Edit And Continue enabled (/ZI compiler option).
    #define INSERT_UNIQUE_NAME(name) LOG_CONCAT1(name,__LINE__)
#endif //#if defined (_MSC_VER) && (_MSC_VER >= 1310)

// In release mode, NiOutputDebugString is defined as a ((void)0) macro.
// In debug mode, replace NiOutputDebugString calls with NILOG equivalents.
#if defined(NIDEBUG)
    #define REPLACE_NIOUTPUTDEBUGSTRING_WITH_NILOG
#endif

#ifndef NI_LOGGER_DISABLE
    #if defined(NISHIPPING) && !defined(_USRDLL)
        #define NI_LOGGER_DISABLE 1
    #endif
#endif

#if !defined(NI_LOGGER_DISABLE)
    #ifdef __GNUC__
        #define NILOG NiLogger NI_UNUSED \
            INSERT_UNIQUE_NAME(xx_Log_)
        #define NILOGDIRECT NiLoggerDirect NI_UNUSED \
            INSERT_UNIQUE_NAME(xx_Log_)
    #else
        #define NILOG NiLogger INSERT_UNIQUE_NAME(xx_Log_)
        #define NILOGDIRECT NiLoggerDirect INSERT_UNIQUE_NAME(xx_Log_)
    #endif // #ifdef __GNUC__

    #ifdef REPLACE_NIOUTPUTDEBUGSTRING_WITH_NILOG
        #define NiOutputDebugString(x) NILOG("%s", x)
    #endif // #ifdef REPLACE_NIOUTPUTDEBUGSTRING_WITH_NILOG
#else
    #if defined(__GNUC__) || defined(__CWCC__)
        #if defined(__GNUC__) && (__GNUC__ >= 3)
            #define NiNullFunction(...)
        #else
            inline void NiNullFunction(int, const char*,  ...) { }
            inline void NiNullFunction(const char*,  ...) { }
        #endif

        #define NILOG NiNullFunction
        #define NILOGDIRECT NiNullFunction
        #if defined(NIDEBUG)
            #define NiOutputDebugString NiNullFunction
        #endif
    #elif defined (_MSC_VER) && (_MSC_VER >= 1300)
        #define NILOG __noop
        #define NILOGDIRECT __noop
        #if defined(NIDEBUG)
            #define NiOutputDebugString __noop
        #endif
    #else
        #define NILOG ((void)0)
        #define NILOGDIRECT ((void)0)
        #if defined(NIDEBUG)
            #define NiOutputDebugString ((void)0)
        #endif
    #endif // #ifdef __GNUC__
#endif
// End macro defines.



class NISYSTEM_ENTRY NiLogMessageOptions
{
public:
    NiLogMessageOptions();
    bool m_bOutputToDebugWindow;
    int m_iLogID;
    bool m_bPrependTimestamp;
};

class NISYSTEM_ENTRY NiLogger
{
public:
    // don't call this explictly, use the NILOG macro.
    NiLogger(int iMessageType, const char* pcFormat,  ...);

    // don't call this explictly, use the NILOG macro - iMessageType = 0
    NiLogger(const char* pcFormat,  ...);
        
    enum OpenMode
    {
        OPEN_APPEND,
        OPEN_OVERWRITE,
        OPEN_UNIQUENAME
    };
    static void _SDMInit();
    static void _SDMShutdown();

    static int OpenLog(const char* pcFilename, OpenMode eOpenMode = 
        OPEN_OVERWRITE, bool bFlushOnWrite = false,
        bool bCommitToDisk = false);
    static void CloseLog(int iLogID);
    static void CloseAllLogs();
    static void FlushAllLogs();

    // Get/Set MessageBehavior options for each message type
    static void SetOutputToDebugWindow(int iMessageType, bool bOutput);
    static bool GetOutputToDebugWindow(int iMessageType);

    static void SetPrependTimestamp(int iMessageType, bool bTimestamp);
    static bool GetPrependTimestamp(int iMessageType);

    static void SetOutputToLogFile(int iMessageType, int iLogID);
    static int GetOutputToLogFile(int iMessageType);

    // internal use only
    static unsigned int UnixToDos(char* pcString, unsigned int uiStringLen);

protected:
    NiLogger() { }
    class NiLogFile
    {
    public:
        char m_acName[256];

#if defined(WIN32) || defined(_XENON) || defined (_PS3)
        FILE* m_pkFile;
#elif defined(_WII)
        NiWiiRemoteFile* m_pkFile;
#else
        int m_iFile;
#endif

        NiLogFile();
        ~NiLogFile();

        void Init(const char* pcName, OpenMode eOpenMode, bool bFlushOnWrite,
            bool bCommitToDisk);
        void Free();

        void Log(char* pcMessage);
        bool IsActive();

    protected:
        bool OpenFile(OpenMode eOpenMode, bool bCommitToDisk);
        void CloseFile();

        bool m_bFlushOnWrite;
    };

    enum
    {
        MAX_NUM_LOGFILES = 16
    };

    static unsigned int ms_uiLogFileCount;
    static NiLogFile ms_akLogFile[MAX_NUM_LOGFILES];

    enum 
    {
        MAX_OUTPUT_LENGTH = 1024
    };

    bool OkayToOutput();
    void FormatOutput(const char* pcFormat, va_list kArgList);
    void LogBuffer();

    static char ms_acBuffer[MAX_OUTPUT_LENGTH]; // for formatting
    static NiLogMessageOptions ms_akMessageOptions[NIMESSAGE_MAX_TYPES];

    int m_iMessageType;
    static NiFastCriticalSection ms_kCriticalSection;
};

class NISYSTEM_ENTRY NiLoggerDirect : public NiLogger
{
public:
    // don't call this explictly, use the NILOGDIRECT macro.
    NiLoggerDirect(int iLogID, const char* pcFormat,  ...);
};


#include "NiLog.inl"
#endif // #ifndef NILOG_H
