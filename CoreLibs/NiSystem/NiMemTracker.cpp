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
// Precompiled Header
#include "NiSystemPCH.h"

#include "NiMemTracker.h"
#include "NiLog.h"

const NiFLF NiFLF::UNKNOWN("",0,"");

size_t NiMemTracker::ms_stBreakOnAllocID = INT_MAX;
void* NiMemTracker::ms_pvBreakOnAllocRangeStart = NULL;
void* NiMemTracker::ms_pvBreakOnAllocRangeEnd = NULL;
const char* NiMemTracker::ms_pcBreakOnFunctionName = "@*";
NiMemTracker* NiMemTracker::ms_pkTracker = NULL;
bool NiMemTracker::ms_bOutputLeaksToDebugStream = true;
size_t NiMemTracker::ms_stBreakOnSizeRequested = INT_MAX;

// These defines determine which channel is used for logging
#define MEM_LOG_COMPLETE    NIMESSAGE_MEMORY_0
#define MEM_LOG_LEAK        NIMESSAGE_MEMORY_1

// Use this setting to determine whether or not to flush the log file
// every time a line is sent or wait until the internal buffer is 
// full to flush the log file.
#define MEM_FLUSH_LOG_ON_WRITE false

//---------------------------------------------------------------------------
void NiMemTracker::Initialize()
{
    m_stCurrentAllocID = 0;
    ResetSummaryStats();

#ifndef NI_LOGGER_DISABLE
    if (m_bWriteToLog)
    {
        char acLogPath[NI_MAX_PATH];
        NIVERIFY(NiPath::GetExecutableDirectory(acLogPath, NI_MAX_PATH));

        char acOverviewPath[NI_MAX_PATH];
        char acLeakPath[NI_MAX_PATH];

        NiSprintf(acOverviewPath, NI_MAX_PATH, "%sNiMemory-Overview.xml", 
            acLogPath);
        NiSprintf(acLeakPath, NI_MAX_PATH, "%sNiMemory-Leaks.xml", acLogPath);

        m_iLoggerCompleteChannel = NiLogger::OpenLog(acOverviewPath, 
            NiLogger::OPEN_OVERWRITE, MEM_FLUSH_LOG_ON_WRITE);

        m_iLoggerLeakChannel = NiLogger::OpenLog(acLeakPath, 
            NiLogger::OPEN_OVERWRITE, MEM_FLUSH_LOG_ON_WRITE);

        if (m_iLoggerCompleteChannel != -1)
        {
            NiLogger::SetOutputToLogFile(MEM_LOG_COMPLETE,
                m_iLoggerCompleteChannel);
        }

        if (m_iLoggerLeakChannel != -1)
        {
            NiLogger::SetOutputToLogFile(MEM_LOG_LEAK,
                m_iLoggerLeakChannel);
        }

    #ifdef _PS3
        time_t tCurrentTime = 0;
        struct tm* pkSysTime;
        struct tm* pkLocalTime;
        pkSysTime = gmtime(&tCurrentTime);
        pkLocalTime = gmtime(&tCurrentTime);
    #elif defined(_WII)
        struct tm kSysTime;
        struct tm kLocalTime;

        struct tm* pkSysTime = &kSysTime;
        struct tm* pkLocalTime = &kLocalTime;

        // time/gmttime functions don't work on the Wii.
        // Get the time using Wii functions and copy it
        // to the tm structs.
        OSCalendarTime tCurrentTime;
        OSTicksToCalendarTime(OSGetTime(), &tCurrentTime);

        pkSysTime->tm_mon = tCurrentTime.mon;
        pkSysTime->tm_mday = tCurrentTime.mday;
        pkSysTime->tm_year = tCurrentTime.year - 1900;
        pkSysTime->tm_hour = tCurrentTime.hour;
        pkSysTime->tm_min = tCurrentTime.min;
        pkSysTime->tm_sec = tCurrentTime.sec;
        pkLocalTime->tm_hour = tCurrentTime.hour;
        pkLocalTime->tm_min = tCurrentTime.min;
        pkLocalTime->tm_sec = tCurrentTime.sec;
    #else
    #if defined(_MSC_VER) && (_MSC_VER >= 1400)
        __time64_t tCurrentTime;
        _time64(&tCurrentTime);
        struct tm kSysTime;
        struct tm* pkSysTime = &kSysTime;
        _gmtime64_s(pkSysTime, &tCurrentTime);
        struct tm kLocalTime;
        struct tm* pkLocalTime = &kLocalTime;
        _localtime64_s(pkLocalTime, &tCurrentTime);
    #else //#if defined(_MSC_VER) && (_MSC_VER >= 1400)
        time_t tCurrentTime;
        time(&tCurrentTime);
        struct tm* pkSysTime;
        pkSysTime = gmtime(&tCurrentTime);
        struct tm* pkLocalTime;
        pkLocalTime = localtime(&tCurrentTime);
    #endif //#if defined(_MSC_VER) && (_MSC_VER >= 1400)
    #endif

        NIASSERT(pkSysTime);

        NILOG(MEM_LOG_COMPLETE, "<?xml version=\"1.0\"?>\n");
        NILOG(MEM_LOG_COMPLETE, "<?xml-stylesheet type=\"text/xsl\""
            " href=\"memoryreport.xsl\"?>\n");
        NILOG(MEM_LOG_COMPLETE, 
            "<memory_log date=\"%02d/%02d/%04d - %2d:%02d:%02d UTC "
            "(%2d:%02d:%02d local)\" version=\"2\" ", 
            pkSysTime->tm_mon+1, pkSysTime->tm_mday, 1900+pkSysTime->tm_year,
            pkSysTime->tm_hour, pkSysTime->tm_min, pkSysTime->tm_sec,
            pkLocalTime->tm_hour, pkLocalTime->tm_min, pkLocalTime->tm_sec);
        NILOG(MEM_LOG_COMPLETE, ">\n");
        NILOG(MEM_LOG_COMPLETE, "<memory_dump timestamp='%f' >\n",
            NiGetCurrentTimeInSec());

        NILOG(MEM_LOG_LEAK, "<?xml version=\"1.0\"?>\n");
        NILOG(MEM_LOG_LEAK, "<?xml-stylesheet type=\"text/xsl\""
            " href=\"memoryleak.xsl\"?>\n");
        NILOG(MEM_LOG_LEAK, 
            "<memory_log date=\"%02d/%02d/%04d - %2d:%02d:%02d UTC "
            "(%2d:%02d:%02d local)\" version=\"2\" ", 
            pkSysTime->tm_mon+1, pkSysTime->tm_mday, 1900+pkSysTime->tm_year,
            pkSysTime->tm_hour, pkSysTime->tm_min, pkSysTime->tm_sec,
            pkLocalTime->tm_hour, pkLocalTime->tm_min, pkLocalTime->tm_sec);
        NILOG(MEM_LOG_LEAK, ">\n");
    }
#endif // #ifndef NI_LOGGER_DISABLE

    if (m_pkActualAllocator)
        m_pkActualAllocator->Initialize();

#ifndef NI_LOGGER_DISABLE
    // Must create these after the allocators get initialized
    if (m_bWriteToLog)
    {
        CreateXSLForLeaks();
        CreateXSLForFreeReport();
    }
#endif // #ifndef NI_LOGGER_DISABLE
}
//---------------------------------------------------------------------------
void NiMemTracker::CreateXSLForLeaks()
{
    if (!m_bWriteToLog)
        return;

    const char* pcXSLFile = 
        "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
        "\n"
        "<xsl:stylesheet\n"
        "version=\"1.0\"\n"
        "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n"
        "<xsl:template match=\"/\">\n"
        "        \n"
        "<html>\n"
        "<head>\n"
        "        <title>Memory Leak Report</title>\n"
        "</head>\n"
        "\n"
        "<body>\n"
        "        <h1 align=\"center\">Memory Leak Report</h1>\n"
        "        <h2 align=\"center\">Date: <xsl:value-of select=\""
        "memory_log/@date\"/></h2>\n"
        "        <h3 align=\"center\">Summary:</h3>\n"
        "        \n"
        "        <table cellpadding=\"5\" border=\"2\" align=\"center\""
        " style=\"font-family:Arial,sans-serif; font-size:10pt\">\n"
        "            <tbody>\n"
        "               <tr style=\"color:black; background-color:#e9d6e7\">\n"
        "                    <th>Statistic</th><th>Value</th>    \n"
        "                </tr>\n"
        "                <tr>\n"
        "                    <td >Total Active Size</td>\n"
        "                    <td align=\"center\"> <xsl:value-of select=\""
        "sum(memory_log/active_memory_dump/alloc_unit/@size)\"/> bytes</td>\n"
        "                </tr>\n"
        "                <tr>\n"
        "                    <td>Active Alloc Count</td>\n"
        "                    <td align=\"center\"> <xsl:value-of select=\""
        "count(memory_log/active_memory_dump/alloc_unit)\"/></td>\n"
        "                </tr>\n"
        "            </tbody>\n"
        "        </table>\n"
        "        \n"
        "        <h3 align=\"center\">Leaked Allocations:</h3>\n"
        "        \n"
        "        <table cellpadding=\"5\" border=\"2\" align=\"center\" "
        "style=\"font-family:Arial,sans-serif; font-size:10pt\">\n"
        "            <tbody>\n"
        "               <tr style=\"color:black; background-color:#e9d6e7\">\n"
        "                    <th>Allocation #</th>\n"
        "                    <th>Size in bytes</th>\n"
        "                    <th>Birth Time</th>\n"
        "                    <th>File</th>\n"
        "                    <th>Line</th>\n"
        "                    <th>Function</th>\n"
        "                    <th>Long Filename</th>\n"
        "                </tr>\n"
        "                <xsl:for-each select=\""
        "memory_log/active_memory_dump/alloc_unit\">\n"
        "                <xsl:sort select=\"@id\" data-type=\"number\" "
        "order=\"ascending\"/>\n"
        "                    <tr>\n"
        "                        <td><xsl:value-of select=\"@id\"/></td>\n"
        "                        <td><xsl:value-of select=\"@size\"/></td>\n"
        "                        <td><xsl:value-of select=\"@alloc_time\"/>"
        "</td>\n"
        "                        <td><xsl:value-of select=\"@file\"/></td>\n"
        "                        <td><xsl:value-of select=\"@line\"/></td>\n"
        "                        <td><xsl:value-of select=\"@func\"/></td>\n"
        "                        <td><a><xsl:attribute name=\"href\">"
        "file:///<xsl:value-of select=\"@long_file\"/>\n"
        "                            </xsl:attribute><xsl:value-of select=\""
        "@long_file\"/></a></td>\n"
        "                    </tr>\n"
        "                </xsl:for-each>\n"
        "            </tbody>\n"
        "        </table>\n"
        "        \n"
        "</body>\n"
        "\n"
        "</html>\n"
        "\n"
        "</xsl:template>\n"
        "</xsl:stylesheet>\n";
    
        char acLogPath[NI_MAX_PATH];
        NIVERIFY(NiPath::GetExecutableDirectory(acLogPath, NI_MAX_PATH));

        char acFilename[NI_MAX_PATH];

        NiSprintf(acFilename, NI_MAX_PATH, "%smemoryleak.xsl", 
            acLogPath);

#if defined(_WII)
        if (!NiWiiRemoteFile::FileExists(acFilename))
        {
            NiWiiRemoteFile kFile(acFilename, NiWiiRemoteFile::CREATE, false);
            kFile.PutS(pcXSLFile);
        }
#else
        // Only write the XSL file if it doesn't already exist
        if (!NiFile::Access(acFilename, NiFile::READ_ONLY))
        {
            NiFile* pkFile = NiFile::GetFile(acFilename, NiFile::WRITE_ONLY);
            pkFile->PutS(pcXSLFile);
            NiDelete pkFile;
        }
#endif
}
//---------------------------------------------------------------------------
void NiMemTracker::CreateXSLForFreeReport()
{
    if (!m_bWriteToLog)
        return;

    const char* pcXSLFile = 
    "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
    "\n"
    "<xsl:stylesheet\n"
    "version=\"1.0\"\n"
    "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n"
    "<xsl:key name=\"sizes\" match=\"alloc_unit\" use=\"@size\"/>\n"
    "<xsl:key name=\"files\" match=\"alloc_unit\" use=\"@long_file\"/>\n"
    "<xsl:key name=\"funcs\" match=\"alloc_unit\" use=\"@func\"/>\n"
    "<xsl:template match=\"/\">\n"
    "        \n"
    "<html>\n"
    "<head>\n"
    "        <title>Memory Report</title>\n"
    "</head>\n"
    "\n"
    "<body>\n"
    "        <h1 align=\"center\">Memory Report</h1>\n"
    "        <h2 align=\"center\">Date: <xsl:value-of select=\"memory_log/"
    "@date\"/></h2>\n"
    "        <h3 align=\"center\">Summary:</h3>\n"
    "        \n"
    "        <table cellpadding=\"5\" border=\"2\" align=\"center\" "
    "style=\"font-family:Arial,sans-serif; font-size:10pt\">\n"
    "            <tbody>\n"
    "                <tr style=\"color:black; background-color:#e9d6e7\">\n"
    "                    <th>Statistic</th><th>Value</th>   \n" 
    "                </tr>\n"
    "                <tr>\n"
    "                    <td >Application Lifetime</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@timestamp\"/> seconds</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td >Total Active Size</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@TotalActiveSize\"/> bytes</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Peak Active Size</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@PeakActiveSize\"/> bytes</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Accumulated Size</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@AccumulatedSize\"/> bytes</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Allocated But Unused Size</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@AllocatedButUnusedSize\"/> bytes</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Percent of Allocations Unused</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "format-number(memory_log/memory_summary/@AllocatedButUnusedSize div "
    "memory_log/memory_summary/@AccumulatedSize * 100.0, '#.##')\"/>%</td>\n"
    "                </tr>\n"  
    "                <tr>\n"
    "                    <td>Active Alloc Count</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@ActiveAllocCount\"/></td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Peak Active Alloc Count</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@PeakActiveAllocCount\"/></td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Total Alloc Count</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@TotalAllocCount\"/></td>\n"
    "                </tr>\n"
    "                <tr>\n"   
    "                    <td>Total Tracker Overhead</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@TotalTrackerOverhead\"/> bytes</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Peak Tracker Overhead</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@PeakTrackerOverhead\"/> bytes</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Accumulated Overhead</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@AccumulatedTrackerOverhead\"/> bytes</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td >Total Active External Size</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@TotalActiveExternalSize\"/> bytes</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Peak Active External Size</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@PeakActiveExternalSize\"/> bytes</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Accumulated External Size</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@AccumulatedExternalSize\"/> bytes</td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Active External Alloc Count</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@ActiveExternalAllocCount\"/></td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Peak Active External Alloc Count</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@PeakExternalActiveAllocCount\"/></td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Total External Alloc Count</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "memory_log/memory_summary/@TotalExternalAllocCount\"/></td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Allocation Sizes &lt; 1024 bytes</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "count(memory_log/memory_dump/alloc_unit[@size &lt; 1024])\"/></td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Allocation Life Spans &lt; 1 second</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "count(memory_log/memory_dump/alloc_unit[@life_span &lt; 1.0])\"/></td>\n"
    "                </tr>\n"
    "                <tr>\n"
    "                    <td>Percent Life Spans &lt; 1 second</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "format-number(count(memory_log/memory_dump/alloc_unit"
    "[@life_span &lt; 1.0]) div memory_log/memory_summary/@TotalAllocCount"
    " * 100.0, '#.##')\"/>%</td>\n"
    "                </tr>\n"
    "                 <tr>\n"
    "                    <td>Percent Sizes &lt; 1024 bytes</td>\n"
    "                    <td align=\"center\"> <xsl:value-of select=\""
    "format-number(count(memory_log/memory_dump/alloc_unit[@size &lt; 1024])"
    " div memory_log/memory_summary/@TotalAllocCount * 100.0, '#.##')\"/>%"
    "</td>\n"
    "                </tr>\n"
    "                \n"
    "            </tbody>\n"
    "        </table>\n"
    "        \n"
    "        <h3 align=\"center\">Top 50 Largest Allocations:</h3>\n"
    "        \n"
    "        <table cellpadding=\"5\" border=\"2\" align=\"center\" "
    "style=\"font-family:Arial,sans-serif; font-size:10pt\">\n"
    "            <tbody>\n"
    "                <tr style=\"color:black; background-color:#e9d6e7\">\n"
    "                    <th>Allocation #</th>\n"
    "                    <th>Size in bytes</th>\n"
    "                    <th>Life Span</th>\n"
    "                    <th>File</th>\n"
    "                    <th>Line</th>\n"
    "                    <th>Function</th>\n"
    "                </tr>\n"
    "                    <xsl:for-each select=\"memory_log/memory_dump/"
    "alloc_unit\">\n"
    "                    <xsl:sort select=\"@size\" data-type=\"number\" "
    "order=\"descending\"/>\n"
    "                        <xsl:if test=\"position() &lt; 50\">\n"
    "                        <tr>\n"
    "                            <td><xsl:value-of select=\"@id\"/></td>\n"
    "                            <td><xsl:value-of select=\"@size\"/></td>\n"
    "                            <td><xsl:value-of select=\"@life_span\"/>"
    "</td>\n"
    "                            <td><xsl:value-of select=\"@file\"/></td>\n"
    "                            <td><xsl:value-of select=\"@line\"/></td>\n"
    "                            <td><xsl:value-of select=\"@func\"/></td>\n"
    "                        </tr>\n"
    "                        </xsl:if>\n"
    "                    </xsl:for-each>\n"
    "                \n"
    "            </tbody>\n"
    "        </table>\n"
    "        \n"
    "        <h3 align=\"center\">Top 50 Smallest Allocations:</h3>\n"
    "        \n"
    "        <table cellpadding=\"5\" border=\"2\" align=\"center\" "
    "style=\"font-family:Arial,sans-serif; font-size:10pt\">\n"
    "            <tbody>\n"
    "                <tr style=\"color:black; background-color:#e9d6e7\">\n"
    "                    <th>Allocation #</th>\n"
    "                    <th>Size in bytes</th>\n"
    "                    <th>Life Span</th>\n"
    "                    <th>File</th>\n"
    "                    <th>Line</th>\n"
    "                    <th>Function</th>\n"
    "                </tr>\n"
    "                    <xsl:for-each select=\"memory_log/memory_dump"
    "/alloc_unit\">\n"
    "                    <xsl:sort select=\"@size\" data-type=\"number\" "
    "order=\"ascending\"/>\n"
    "                        <xsl:if test=\"position() &lt; 50\">\n"
    "                        <tr>\n"
    "                            <td><xsl:value-of select=\"@id\"/></td>\n"
    "                            <td><xsl:value-of select=\"@size\"/></td>\n"
    "                            <td><xsl:value-of select=\"@life_span\"/>"
    "</td>\n"
    "                            <td><xsl:value-of select=\"@file\"/></td>\n"
    "                            <td><xsl:value-of select=\"@line\"/></td>\n"
    "                            <td><xsl:value-of select=\"@func\"/></td>\n"
    "                        </tr>\n"
    "                        </xsl:if>\n"
    "                    </xsl:for-each>\n"
    "                \n"
    "            </tbody>\n"
    "        </table>\n"
    "        \n"
    "        <h3 align=\"center\">Top 50 Shortest-lived Allocations:</h3>\n"
    "        \n"
    "        <table cellpadding=\"5\" border=\"2\" align=\"center\" "
    "style=\"font-family:Arial,sans-serif; font-size:10pt\">\n"
    "            <tbody>\n"
    "                <tr style=\"color:black; background-color:#e9d6e7\">\n"
    "                    <th>Allocation #</th>\n"
    "                    <th>Size in bytes</th>\n"
    "                    <th>Life Span</th>\n"
    "                    <th>File</th>\n"
    "                    <th>Line</th>\n"
    "                    <th>Function</th>\n"
    "                </tr>\n"
    "                    <xsl:for-each select=\"memory_log/memory_dump/"
    "alloc_unit\">\n"
    "                    <xsl:sort select=\"@life_span\" data-type=\"number\""
    " order=\"ascending\"/>\n"
    "                        <xsl:if test=\"position() &lt; 50\">\n"
    "                        <tr>\n"
    "                            <td><xsl:value-of select=\"@id\"/></td>\n"
    "                            <td><xsl:value-of select=\"@size\"/></td>\n"
    "                            <td><xsl:value-of select=\"@life_span\"/>"
    "</td>\n"
    "                            <td><xsl:value-of select=\"@file\"/></td>\n"
    "                            <td><xsl:value-of select=\"@line\"/></td>\n"
    "                            <td><xsl:value-of select=\"@func\"/></td>\n"
    "                        </tr>\n"
    "                        </xsl:if>\n"
    "                    </xsl:for-each>\n"
    "                \n"
    "            </tbody>\n"
    "        </table>\n"
    "        \n"
    "        <h3 align=\"center\">Number of Allocations by File</h3>\n"
    "        <table cellpadding=\"5\" border=\"2\" align=\"center\" "
    "style=\"font-family:Arial,sans-serif; font-size:10pt\">\n"
    "            <tbody>\n"
    "                <tr style=\"color:black; background-color:#e9d6e7\">\n"
    "                    <th>Filename</th>\n"
    "                    <th>Count</th>\n"
    "                </tr>\n"
    "                <xsl:for-each select=\"memory_log/memory_dump/"
    "alloc_unit[generate-id() = generate-id(key('files',@long_file)[1])]\">\n"
    "                <xsl:sort select=\"count(../alloc_unit[@long_file="
    "current()/@long_file])\" data-type=\"number\" order=\"descending\"/>\n"
    "                <tr>\n"
    "<!--                   <td><a><xsl:attribute name=\"href\">file:///"
    "<xsl:value-of select=\"@long_file\"/></xsl:attribute><xsl:value-of "
    "select=\"@long_file\"/></a>\n"
    "                    </td>-></!-->\n"
    "                    <td><xsl:value-of select=\"@long_file\"/></td>\n"
    "                    <td><xsl:value-of select=\"count(../alloc_unit["
    "@long_file=current()/@long_file])\"/></td>\n"
    "                </tr>\n"
    "                </xsl:for-each>\n"
    "\n"
    "            </tbody>\n"
    "        </table>\n"
    "        \n"
    "        <h3 align=\"center\">Number of Allocations by Function</h3>\n"
    "        <table cellpadding=\"5\" border=\"2\" align=\"center\" style=\""
    "font-family:Arial,sans-serif; font-size:10pt\">\n"
    "            <tbody>\n"
    "                <tr style=\"color:black; background-color:#e9d6e7\">\n"
    "                    <th>Function</th>\n"
    "                    <th>Count</th>\n"
    "                </tr>\n"
    "                <xsl:for-each select=\"memory_log/memory_dump/alloc_unit["
    "generate-id() = generate-id(key('funcs',@func)[1])]\">\n"
    "                <xsl:sort select=\"count(../alloc_unit[@func=current()/"
    "@func])\" data-type=\"number\" order=\"descending\"/>\n"
    "                <tr>\n"
    "                    <td><xsl:value-of select=\"@func\"/></td>\n"
    "                    <td><xsl:value-of select=\"count(../alloc_unit["
    "@func=current()/@func])\"/></td>\n"
    "                </tr>\n"
    "                </xsl:for-each>\n"
    "\n"
    "            </tbody>\n"
    "        </table>\n"
    "        \n"
    "        <h3 align=\"center\">Number of Allocations by Size</h3>\n"
    "        <table cellpadding=\"5\" border=\"2\" align=\"center\" style=\""
    "font-family:Arial,sans-serif; font-size:10pt\">\n"
    "            <tbody>\n"
    "                <tr style=\"color:black; background-color:#e9d6e7\">\n"
    "                    <th>Size in bytes</th>\n"
    "                    <th>Count</th>\n"
    "                </tr>\n"
    "                <xsl:for-each select=\"memory_log/memory_dump/alloc_unit"
    "[generate-id() = generate-id(key('sizes',@size)[1])]\">\n"
    "                <xsl:sort select=\"@size\" data-type=\"number\"/>\n"
    "                <tr>\n"
    "                    <td><xsl:value-of select=\"@size\"/></td>\n"
    "                    <td><xsl:value-of select=\"count(../alloc_unit["
    "@size=current()/@size])\"/></td>\n"
    "                </tr>\n"
    "                </xsl:for-each>\n"
    "\n"
    "            </tbody>\n"
    "        </table>\n"
    "\n"
    "</body>\n"
    "\n"
    "</html>\n"
    "\n"
    "</xsl:template>\n"
    "\n"
    "</xsl:stylesheet>\n";

    char acLogPath[NI_MAX_PATH];
    NIVERIFY(NiPath::GetExecutableDirectory(acLogPath, NI_MAX_PATH));

    char acFilename[NI_MAX_PATH];

    NiSprintf(acFilename, NI_MAX_PATH, "%smemoryreport.xsl", 
        acLogPath);

#if defined(_WII)
        if (!NiWiiRemoteFile::FileExists(acFilename))
        {
            NiWiiRemoteFile kFile(acFilename, NiWiiRemoteFile::CREATE, false);
            kFile.PutS(pcXSLFile);
        }
#else
    // Only write the XSL file if it doesn't already exist
    if (!NiFile::Access(acFilename, NiFile::READ_ONLY))
    {
        NiFile* pkFile = NiFile::GetFile(acFilename, NiFile::WRITE_ONLY);
        pkFile->PutS(pcXSLFile);
        NiDelete pkFile;
    }
#endif
}
//---------------------------------------------------------------------------
void NiMemTracker::Shutdown()
{
    if (m_stActiveAllocationCount != 0)
    {
        char acString[256];
        NiSprintf(acString, 256, "         There are %d leaked allocations"
            ".\n", m_stActiveAllocationCount);
        NiOutputDebugString("\n\n//----------------------------------------"
            "-----------------------------------\n");
        NiOutputDebugString("//       NiMemTracker Report:\n");
        NiOutputDebugString("//-------------------------------------------"
            "--------------------------------\n");
        NiOutputDebugString(acString);
        NiOutputDebugString("//-------------------------------------------"
            "--------------------------------\n");

        OutputLeakedMemoryToDebugStream();

        NiOutputDebugString("//-------------------------------------------"
            "--------------------------------\n");
    }

#ifndef NI_LOGGER_DISABLE
    if (m_bWriteToLog)
    {
        NILOG(MEM_LOG_COMPLETE, "</memory_dump>\n");

        LogMemoryReport();
        LogSummaryStats();
        
        NILOG(MEM_LOG_COMPLETE, "</memory_log>\n");
        NILOG(MEM_LOG_LEAK, "</memory_log>\n");

        NiLogger::CloseLog(m_iLoggerLeakChannel);
        NiLogger::CloseLog(m_iLoggerCompleteChannel);
    }
#endif // #ifndef NI_LOGGER_DISABLE

    if (m_pkActualAllocator)
        m_pkActualAllocator->Shutdown();
}
//---------------------------------------------------------------------------
void NiMemTracker::ResetSummaryStats()
{
    m_fPeakMemoryTime = 0.0f;
    m_fPeakAllocationCountTime = 0.0f;
    m_stActiveMemory = 0;
    m_stPeakMemory = 0;
    m_stAccumulatedMemory = 0;
    m_stUnusedButAllocatedMemory = 0;

    m_stActiveAllocationCount = 0;
    m_stPeakAllocationCount = 0;
    m_stAccumulatedAllocationCount = 0;

    m_stActiveExternalMemory = 0; 
    m_stPeakExternalMemory = 0; 
    m_stAccumulatedExternalMemory = 0; 
    
    m_stActiveExternalAllocationCount = 0;
    m_stPeakExternalAllocationCount = 0;
    m_stAccumulatedExternalAllocationCount = 0;
}
//---------------------------------------------------------------------------
NiMemTracker::NiMemTracker(NiAllocator* pkActualAllocator, 
    bool bWriteToLog, unsigned int uiInitialSize, unsigned int uiGrowBy,
    bool bAlwaysValidateAll, bool bCheckArrayOverruns) : 
    m_fPeakMemoryTime(0.0f), m_fPeakAllocationCountTime(0.0f), 
    m_bAlwaysValidateAll(bAlwaysValidateAll), 
    m_stReservoirGrowBy(uiGrowBy),
    m_bCheckArrayOverruns(bCheckArrayOverruns), m_ucFillChar(0xbd),
    m_bWriteToLog(bWriteToLog)
{
    ms_pkTracker = this;
    m_pkActualAllocator = pkActualAllocator;
    memset(m_pkActiveMem, 0, ms_uiHashSize*sizeof(NiAllocUnit*));    
    if (uiInitialSize > 0)
    {
        // create initial set of tracking data structures
        m_stReservoirBufferSize = 1;
        m_pkReservoir = (NiAllocUnit*)calloc(uiInitialSize,
            sizeof(NiAllocUnit));

        // If you hit this NIMEMASSERT, then the memory manager failed to 
        // allocate internal memory for tracking the allocations
        NIMEMASSERT(m_pkReservoir != NULL);

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
        // throw an exception in this situation
        if (m_pkReservoir == NULL) 
            throw "Unable to allocate RAM for internal memory tracking data";
#endif

        // initialize the allocation units
        for (unsigned int i = 0; i < uiInitialSize-1; i++)
        {
            m_pkReservoir[i].m_kFLF = NiFLF::UNKNOWN;
            m_pkReservoir[i].m_pkNext = &m_pkReservoir[i+1];
        }
        m_pkReservoir[uiInitialSize-1].m_kFLF = NiFLF::UNKNOWN;
        m_pkReservoir[uiInitialSize-1].m_pkNext = NULL;

        m_ppkReservoirBuffer = (NiAllocUnit**)
            NiExternalMalloc(sizeof(NiAllocUnit*));

        // If you hit this NIMEMASSERT, then the memory manager failed to 
        // allocate internal memory for tracking the allocations
        NIMEMASSERT(m_ppkReservoirBuffer != NULL);

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
        // throw an exception in this situation
        if (m_ppkReservoirBuffer == NULL) 
            throw "Unable to allocate RAM for internal memory tracking data";
#endif

        m_ppkReservoirBuffer[0] = m_pkReservoir;

        unsigned int uiInitialBytes = uiInitialSize*sizeof(NiAllocUnit) + 
            sizeof(NiAllocUnit*);
        m_stActiveTrackerOverhead = uiInitialBytes;
        m_stPeakTrackerOverhead = uiInitialBytes;
        m_stAccumulatedTrackerOverhead = uiInitialBytes;
    }
    else
    {
        m_stReservoirBufferSize = 0;
    }
}
//---------------------------------------------------------------------------
NiMemTracker::~NiMemTracker()
{
    // Free all memory used by the hash table of alloc units
    if (m_ppkReservoirBuffer)
    {
        for (unsigned int i = 0; i < m_stReservoirBufferSize; i++)
        {
            NiExternalFree(m_ppkReservoirBuffer[i]);
        }
        NiExternalFree(m_ppkReservoirBuffer);
    }

    ms_pkTracker = NULL;

    NiExternalDelete m_pkActualAllocator;
}
//---------------------------------------------------------------------------
void NiMemTracker::GrowReservoir()
{
    NIMEMASSERT(!m_pkReservoir);

    m_pkReservoir = NiExternalAlloc(NiAllocUnit, m_stReservoirGrowBy);

    // If you hit this NIMEMASSERT, then the memory manager failed to allocate 
    // internal memory for tracking the allocations
    NIMEMASSERT(m_pkReservoir != NULL);

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    // throw an exception in this situation
    if (m_pkReservoir == NULL) 
        throw "Unable to allocate RAM for internal memory tracking data";
#endif

    m_stActiveTrackerOverhead += sizeof(NiAllocUnit) * 
        m_stReservoirGrowBy + sizeof(NiAllocUnit*);

    if (m_stActiveTrackerOverhead > 
        m_stPeakTrackerOverhead)
    {
        m_stPeakTrackerOverhead = 
            m_stActiveTrackerOverhead;
    }
    m_stAccumulatedTrackerOverhead += sizeof(NiAllocUnit) * 
        m_stReservoirGrowBy + sizeof(NiAllocUnit*);

    // Build a linked-list of the elements in the reservoir
    // Initialize the allocation units
    for (unsigned int i = 0; i < m_stReservoirGrowBy-1; i++)
    {
        m_pkReservoir[i].m_kFLF = NiFLF::UNKNOWN;
        m_pkReservoir[i].m_pkNext = &m_pkReservoir[i+1];
    }
    m_pkReservoir[m_stReservoirGrowBy-1].m_kFLF = NiFLF::UNKNOWN;
    m_pkReservoir[m_stReservoirGrowBy-1].m_pkNext = NULL;

    // Add this address to the reservoir buffer so it can be freed later
    NiAllocUnit **pkTemp = (NiAllocUnit**) NiExternalRealloc(
        m_ppkReservoirBuffer, 
        (m_stReservoirBufferSize + 1) * sizeof(NiAllocUnit*));

    // If you hit this NIMEMASSERT, then the memory manager failed to allocate 
    // internal memory for tracking the allocations
    NIMEMASSERT(pkTemp != NULL);
    if (pkTemp)
    {
        m_ppkReservoirBuffer = pkTemp;
        m_ppkReservoirBuffer[m_stReservoirBufferSize] = m_pkReservoir;
        m_stReservoirBufferSize++;
    }
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    else
    {
        // throw an exception in this situation
        throw "Unable to allocate RAM for internal memory tracking data";
    }
#endif
}
//---------------------------------------------------------------------------
void* NiMemTracker::Allocate(
    size_t& stSizeInBytes,
    size_t& stAlignment,
    NiMemHint kHint,
    NiMemEventType eEventType,
    const char* pcFile,
    int iLine,
    const char* pcFunction)
{
    m_kCriticalSection.Lock();
    
    size_t stSizeOriginal = stSizeInBytes;
    float fTime = NiGetCurrentTimeInSec();  
    
    if (m_bCheckArrayOverruns)
    {
       stSizeInBytes= PadForArrayOverrun(stAlignment, stSizeInBytes);
    }

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // function name.
    NIMEMASSERT(strcmp(ms_pcBreakOnFunctionName, pcFunction) != 0);

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // allocation ID.
    NIMEMASSERT(ms_stBreakOnAllocID != m_stCurrentAllocID);

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // allocation request size.
    NIMEMASSERT(ms_stBreakOnSizeRequested != stSizeOriginal);

    // Actually perform the allocation. Note that size and alignment
    // may be adjusted by the call.
    void* pvMem = m_pkActualAllocator->Allocate(stSizeInBytes, stAlignment,
        kHint, eEventType, pcFile, iLine, pcFunction);   

    if (pvMem == NULL)
    {
        // If you hit this, your memory request was not satisfied
        m_kCriticalSection.Unlock();
        return NULL;
    }

    NIMEMASSERT(!FindAllocUnit(pvMem));

    // update summary statistics
    m_stActiveAllocationCount++;
    m_stAccumulatedAllocationCount++;
    if (m_stActiveAllocationCount > m_stPeakAllocationCount)
    {
        m_stPeakAllocationCount = m_stActiveAllocationCount;
        m_fPeakAllocationCountTime = fTime;
    }

    m_stActiveMemory += stSizeInBytes;
    m_stAccumulatedMemory += stSizeInBytes;
    if (m_stActiveMemory > m_stPeakMemory)
    {
        m_stPeakMemory = m_stActiveMemory;
        m_fPeakMemoryTime = fTime;
    }

    
    // If you hit this NIMEMASSERT, your memory request result was smaller 
    // than the input.
    NIMEMASSERT(stSizeInBytes >= stSizeOriginal);

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific
    // allocation address range.
    NIMEMASSERT(!IsInsideBreakRange(pvMem, stSizeInBytes));

    // Pad the start and end of the allocation with the pad character 
    // so that we can check for array under and overruns. Note that the
    // address is shifted to hide the padding before the allocation.
    if (m_bCheckArrayOverruns)
    {
        MemoryFillForArrayOverrun(pvMem, stAlignment, stSizeOriginal);
    }

    // Fill the originally requested memory size with the pad character.
    // This will allow us to see how much of the allocation was 
    // actually used.
#if defined(_WII) && defined(__CWCC__)
    // CodeWarrior on Wii prepends extra bytes to object arrays (arrays 
    // allocated with new[]). Excluding from the fill pattern since CW 
    // doesn't necessarily use these bytes, which results in a false 
    // positive of "unused memory allocated" when memory is released.
    if (eEventType == NI_OPER_NEW_ARRAY &&
        stSizeOriginal > NI_MEM_WII_CW_NEW_ARRAY_OVERHEAD)
    {
        void* pvArrayStart = 
            reinterpret_cast<void*>(reinterpret_cast<size_t>(pvMem)+
                NI_MEM_WII_CW_NEW_ARRAY_OVERHEAD);

        size_t stSizeWithoutOverhead = 
            stSizeOriginal-NI_MEM_WII_CW_NEW_ARRAY_OVERHEAD;

        MemoryFillWithPattern(pvArrayStart, stSizeWithoutOverhead);
    }
    else
#endif
    {
        MemoryFillWithPattern(pvMem, stSizeOriginal);
    }

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    try
    {
#endif

    // If you hit this NIMEMASSERT, the somehow you have allocated a memory
    // unit to an address that already exists. This should never happen
    // and is an indicator that something has gone wrong in the sub-allocator.
    NIMEMASSERT(FindAllocUnit(pvMem) == NULL);

    // Grow the tracking unit reservoir if necessary
    if (!m_pkReservoir)
        GrowReservoir();

    // If you hit this NIMEMASSERT, the free store for allocation units
    // does not exist. This should only happen if the reservoir 
    // needed to grow and was unable to satisfy the request. In other words,
    // you may be out of memory.
    NIMEMASSERT (m_pkReservoir != NULL);

    // Get an allocation unit from the reservoir
    NiAllocUnit* pkUnit = m_pkReservoir;
    m_pkReservoir = pkUnit->m_pkNext;

    // fill in the known information
    pkUnit->Reset();
    pkUnit->m_stAllocationID = m_stCurrentAllocID;
    pkUnit->m_stAlignment = stAlignment;
    pkUnit->m_ulAllocThreadId = NiGetCurrentThreadId();
    pkUnit->m_eAllocType = eEventType;
    pkUnit->m_kAllocHint = kHint;
    pkUnit->m_fAllocTime = fTime;
    pkUnit->m_pvMem = pvMem;
    pkUnit->m_stSizeRequested = stSizeOriginal;
    pkUnit->m_stSizeAllocated = stSizeInBytes;
    pkUnit->m_kFLF.Set(pcFile, iLine, pcFunction);

    // If you hit this NIMEMASSERT, then this allocation was made from a 
    // source that isn't setup to use this memory tracking software, use the 
    // stack frame to locate the source and include NiMemManager.h.
    NIMEMASSERTUNIT(eEventType != NI_UNKNOWN, pkUnit);

    // Insert the new allocation into the hash table
    InsertAllocUnit(pkUnit);

    // Validate every single allocated unit in memory
    if (m_bAlwaysValidateAll)
    {
        bool bValidateAllAllocUnits = ValidateAllAllocUnits();
        NI_UNUSED_ARG(bValidateAllAllocUnits);
        NIMEMASSERTUNIT(bValidateAllAllocUnits, pkUnit);
    }

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    }
    catch(const char *err)
    {
        // Deal with the errors
        // Deal with the errors
    }
#endif

    ++m_stCurrentAllocID;
    m_kCriticalSection.Unlock();
    
    return pvMem;
}   
//---------------------------------------------------------------------------
void NiMemTracker::InsertAllocUnit(NiAllocUnit* pkUnit)
{
    NIMEMASSERT(pkUnit != NULL && pkUnit->m_pvMem != NULL);
    unsigned int uiHashIndex = AddressToHashIndex(pkUnit->m_pvMem);

    // Remap the new allocation unit to the head of the hash entry
    if (m_pkActiveMem[uiHashIndex])
    {
        NIMEMASSERTUNIT(m_pkActiveMem[uiHashIndex]->m_pkPrev == NULL, pkUnit);
        m_pkActiveMem[uiHashIndex]->m_pkPrev = pkUnit;
    }

    NIMEMASSERTUNIT(pkUnit->m_pkNext == NULL, pkUnit);
    pkUnit->m_pkNext = m_pkActiveMem[uiHashIndex];
    pkUnit->m_pkPrev = NULL;
    m_pkActiveMem[uiHashIndex] = pkUnit;
}
//---------------------------------------------------------------------------
void NiMemTracker::RemoveAllocUnit(NiAllocUnit* pkUnit)
{
    NIMEMASSERT(pkUnit != NULL && pkUnit->m_pvMem != NULL);
    unsigned int uiHashIndex = AddressToHashIndex(pkUnit->m_pvMem);

    // If you hit this NIMEMASSERT, somehow we have emptied the
    // hash table for this bucket. This should not happen
    // and is indicative of a serious error in the memory
    // tracking infrastructure.
    NIMEMASSERTUNIT(m_pkActiveMem[uiHashIndex] != NULL, pkUnit);

    if (m_pkActiveMem[uiHashIndex] == pkUnit)
    {
        NIMEMASSERTUNIT(pkUnit->m_pkPrev == NULL, pkUnit);
        m_pkActiveMem[uiHashIndex] = pkUnit->m_pkNext;
        
        if (m_pkActiveMem[uiHashIndex])
            m_pkActiveMem[uiHashIndex]->m_pkPrev = NULL;
    }
    else
    {      
        if (pkUnit->m_pkPrev)
        {
            NIMEMASSERTUNIT(pkUnit->m_pkPrev->m_pkNext == pkUnit, pkUnit);
            pkUnit->m_pkPrev->m_pkNext = pkUnit->m_pkNext;
        }
        if (pkUnit->m_pkNext)
        {
            NIMEMASSERTUNIT(pkUnit->m_pkNext->m_pkPrev == pkUnit, pkUnit);
            pkUnit->m_pkNext->m_pkPrev = pkUnit->m_pkPrev;
        }
    }
}
//---------------------------------------------------------------------------
void* NiMemTracker::Reallocate(
    void* pvMemory,
    size_t& stSizeInBytes,
    size_t& stAlignment,
    NiMemHint kHint,
    NiMemEventType eEventType,
    size_t stSizeCurrent,
    const char* pcFile,
    int iLine,
    const char* pcFunction)
{
    // Store the original request size for later use.
    size_t stSizeOriginal = stSizeInBytes;

    // If the address is null, then this should function just 
    // like an allocation routine.
    if (pvMemory == NULL)
    {
        return Allocate(stSizeInBytes, stAlignment, kHint, eEventType,
            pcFile, iLine, pcFunction);
    }

    // If the requested size is 0, then this should function
    // just like a deallocation routine.
    if (stSizeInBytes == 0)
    {
        Deallocate(pvMemory, eEventType, NI_MEM_DEALLOC_SIZE_DEFAULT);
        return NULL;
    }

    m_kCriticalSection.Lock();
    
    float fTime = NiGetCurrentTimeInSec();

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    try
    {
#endif
    // A reallocation is tracked with two allocation units. The first
    // is the old unit. The second unit tracks the new allocation, which 
    // may or may not have the same address as the old allocation. 
    // Its allocation type is set to the event type passed into this function.

    NiAllocUnit* pkUnit = FindAllocUnit(pvMemory);

    if (pkUnit == NULL)
    {
        // If you hit this NIMEMASSERT, you tried to reallocate RAM that 
        // wasn't allocated by this memory manager.
        NIMEMASSERT(pkUnit != NULL);
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
        throw "Request to reallocate RAM that was never allocated";
#endif
        m_kCriticalSection.Unlock();
    
        return NULL;
    }

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // function name.
    NIMEMASSERT(strcmp(ms_pcBreakOnFunctionName, pcFunction) != 0);

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // allocation ID.
    NIMEMASSERT(ms_stBreakOnAllocID != pkUnit->m_stAllocationID);

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // allocation request size.
    NIMEMASSERT(ms_stBreakOnSizeRequested != pkUnit->m_stSizeRequested);

    // If you hit this NIMEMASSERT, you requested a breakpoint on a 
    // specific allocation address range.
    NIMEMASSERT(!IsInsideBreakRange(pkUnit->m_pvMem, 
        pkUnit->m_stSizeRequested));

    // If you hit this NIMEMASSERT, then the allocation unit that is about 
    // to be deleted requested an initial size that doesn't match
    // what is currently the 'size' argument for deallocation.   
    // This is most commonly caused by the lack of a virtual destructor
    // for a class that is used polymorphically in an array.
    if (stSizeCurrent != NI_MEM_DEALLOC_SIZE_DEFAULT)
        NIMEMASSERT(stSizeCurrent == pkUnit->m_stSizeRequested);

    if (m_bCheckArrayOverruns)
    {
        // If you hit this NIMEMASSERT, you have code that overwrites
        // either before or after the range of an allocation.
        // Check the pkUnit for information about which allocation
        // is being overwritten. 
        NIMEMASSERT(!CheckForArrayOverrun(pvMemory,
            pkUnit->m_stAlignment, pkUnit->m_stSizeRequested));   
        stSizeInBytes= PadForArrayOverrun(stAlignment, stSizeInBytes);

        if (stSizeCurrent != NI_MEM_DEALLOC_SIZE_DEFAULT)
        {
            stSizeCurrent = PadForArrayOverrun(pkUnit->m_stAlignment, 
                stSizeCurrent);
        }
    }
    
    // If you hit this NIMEMASSERT, then the allocation unit that is about to 
    // be reallocated is damaged. 
    NIMEMASSERT(ValidateAllocUnit(pkUnit));

    // Alignment should be the same between reallocations
    NIMEMASSERT(pkUnit->m_stAlignment == stAlignment);

    // Determine how much memory was actually set
    size_t stSizeUnused = MemoryBytesWithPattern(pkUnit->m_pvMem,
        pkUnit->m_stSizeRequested);
    m_stUnusedButAllocatedMemory += stSizeUnused;

    // Save the thread id that freed the memory
    unsigned long ulFreeThreadId = NiGetCurrentThreadId();

    // Perform the actual memory reallocation
    void* pvNewMemory = m_pkActualAllocator->Reallocate(pvMemory,
        stSizeInBytes, stAlignment, kHint, eEventType, 
        stSizeCurrent, pcFile, iLine, pcFunction);

    // If you hit this NIMEMASSERT, then the reallocation was unable to
    // be satisfied
    NIMEMASSERT(pvNewMemory != NULL);

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific
    // allocation address range.
    NIMEMASSERT(!IsInsideBreakRange(pvNewMemory, stSizeInBytes));

    // update summary statistics
    size_t stPreReallocSize = pkUnit->m_stSizeRequested;
    
    m_stAccumulatedAllocationCount++;


    // Determine whether the post-realloc size is larger or smaller than
    // the original size, and update the statistics accordingly
    size_t stIncreaseInSize = 0;
    if (stSizeInBytes >= pkUnit->m_stSizeAllocated)
    {
        stIncreaseInSize = stSizeInBytes - pkUnit->m_stSizeAllocated;
        m_stActiveMemory += stIncreaseInSize;
        m_stAccumulatedMemory += stIncreaseInSize;
    }
    else
    {
        m_stActiveMemory -= pkUnit->m_stSizeAllocated - stSizeInBytes;
    }

    if (m_stActiveMemory > m_stPeakMemory)
    {
        m_stPeakMemory = m_stActiveMemory;
        m_fPeakMemoryTime = fTime;
    }

    // Pad the start and end of the allocation with the pad character 
    // so that we can check for array under and overruns.
    if (m_bCheckArrayOverruns)
    {
        MemoryFillForArrayOverrun(pvNewMemory, stAlignment, stSizeOriginal);
    }

    // Fill the originally requested memory size with the pad character.
    // This will allow us to see how much of the allocation was 
    // actually used.
    // For reallocation, we only want to fill the unused section
    // that was just allocated.
    if (stIncreaseInSize)
    {
        MemoryFillWithPattern((char*)pvNewMemory + stPreReallocSize, 
            stIncreaseInSize);
    }

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // allocation ID
    NIMEMASSERT(m_stCurrentAllocID != ms_stBreakOnAllocID);

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // allocation request size.
    NIMEMASSERT(ms_stBreakOnSizeRequested != stSizeOriginal);

    // If you hit this NIMEMASSERT, then this reallocation was made from a  
    // that isn't setup to use this memory tracking software, use the stack
    // source frame to locate the source and include NiMemManager.h.
    NIMEMASSERT(eEventType != NI_UNKNOWN);

    // If you hit this NIMEMASSERT, you were trying to reallocate RAM that was 
    // not allocated in a way that is compatible with realloc. In other 
    // words, you have a allocation/reallocation mismatch.
    NIMEMASSERT(pkUnit->m_eAllocType == NI_MALLOC ||
            pkUnit->m_eAllocType == NI_REALLOC ||
            pkUnit->m_eAllocType == NI_ALIGNEDMALLOC ||
            pkUnit->m_eAllocType == NI_ALIGNEDREALLOC);

    // If you hit this NIMEMASSERT, you performed a reallocation with a hint
    // mismatch.  The hint passed into reallocate should match the hint passed
    // in when it was originally allocated.
    NIMEMASSERT(pkUnit->m_kAllocHint.GetRaw() == kHint.GetRaw());

    // update allocation unit
    NiMemEventType eDeallocType = eEventType;
    float fDeallocTime = fTime;
    
    // Write out the freed memory to the memory log
    LogAllocUnit(pkUnit, MEM_LOG_COMPLETE, "\t", eDeallocType,
        fDeallocTime, ulFreeThreadId, stSizeUnused);

    // Remove this allocation unit from the hash table
    RemoveAllocUnit(pkUnit);   
    --m_stActiveAllocationCount;

    if (m_bAlwaysValidateAll)
        NIMEMASSERT(ValidateAllAllocUnits());

    // Recycle the allocation unit
    // Add it to the front of the reservoir 
    pkUnit->m_kFLF = NiFLF::UNKNOWN;
    pkUnit->m_pkNext = m_pkReservoir;
    m_pkReservoir = pkUnit;
    pkUnit = NULL;

    // Grow the tracking unit reservoir if necessary
    if (!m_pkReservoir)
        GrowReservoir();

    // If you hit this NIMEMASSERT, the free store for allocation units
    // does not exist. This should only happen if the reservoir 
    // needed to grow and was unable to satisfy the request. In other words,
    // you may be out of memory.
    NIMEMASSERT (m_pkReservoir != NULL);

    // Get an allocation unit from the reservoir
    NiAllocUnit* pkNewUnit = m_pkReservoir;
    m_pkReservoir = pkNewUnit->m_pkNext;

    // fill in the known information
    pkNewUnit->Reset();
    pkNewUnit->m_stAllocationID = m_stCurrentAllocID; 
    pkNewUnit->m_stAlignment = stAlignment;
    pkNewUnit->m_ulAllocThreadId = NiGetCurrentThreadId();
    pkNewUnit->m_eAllocType = eEventType;
    pkNewUnit->m_kAllocHint = kHint;
    pkNewUnit->m_fAllocTime = fTime;
    pkNewUnit->m_pvMem = pvNewMemory;
    pkNewUnit->m_stSizeAllocated = stSizeInBytes;
    pkNewUnit->m_stSizeRequested = stSizeOriginal;
    pkNewUnit->m_kFLF.Set(pcFile, iLine, pcFunction);

    // If you hit this NIMEMASSERT, then this allocation was made from a source
    // that isn't setup to use this memory tracking software, use the 
    // stack frame to locate the source and include NiMemManager.h.
    NIMEMASSERT(eEventType != NI_UNKNOWN);

    // Insert the new allocation into the hash table
    InsertAllocUnit(pkNewUnit);
    ++m_stActiveAllocationCount;
    ++m_stCurrentAllocID;

    if (m_bAlwaysValidateAll)
        NIMEMASSERT(ValidateAllAllocUnits());


#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    }
    catch(const char *err)
    {
    }
#endif
    m_kCriticalSection.Unlock();
    
    return pvNewMemory;
}
//---------------------------------------------------------------------------
void NiMemTracker::Deallocate(
    void* pvMemory,
    NiMemEventType eEventType,
    size_t stSizeInBytes)
{
    if (pvMemory)
    {

        m_kCriticalSection.Lock();
    
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
        try {
#endif
        float fTime = NiGetCurrentTimeInSec();

        // Search the tracking unit hash table to find the address
        NiAllocUnit* pkUnit = FindAllocUnit(pvMemory);

        if (pkUnit == NULL)
        {
            // If you hit this NIMEMASSERT, you tried to deallocate RAM that 
            // wasn't allocated by this memory manager. This may also
            // be indicative of a double deletion. Please check the pkUnit
            // FLF for information about the allocation.
            NIMEMASSERT(pkUnit != NULL);
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
            throw "Request to deallocate RAM that was never allocated";
#endif
            m_kCriticalSection.Unlock();
    
            return;
        }

        // If you hit this NIMEMASSERT, you requested a breakpoint on a 
        // specific allocation ID.
        NIMEMASSERTUNIT(ms_stBreakOnAllocID != pkUnit->m_stAllocationID, 
            pkUnit);

        // If you hit this NIMEMASSERT, you requested a breakpoint on a 
        // specific allocation request size.
        NIMEMASSERTUNIT(ms_stBreakOnSizeRequested != pkUnit->m_stSizeRequested,
            pkUnit);

        // If you hit this NIMEMASSERT, you requested a breakpoint on a 
        // specific allocation address range.
        NIMEMASSERTUNIT(!IsInsideBreakRange(pkUnit->m_pvMem, 
            pkUnit->m_stSizeRequested), pkUnit);

        // If you hit this NIMEMASSERT, then the allocation unit that is about 
        // to be deleted requested an initial size that doesn't match
        // what is currently the 'size' argument for deallocation.
        // This is most commonly caused by the lack of a virtual destructor
        // for a class that is used polymorphically in an array.
        if (stSizeInBytes != NI_MEM_DEALLOC_SIZE_DEFAULT)
            NIMEMASSERTUNIT(stSizeInBytes== pkUnit->m_stSizeRequested, pkUnit);

        // If you hit this NIMEMASSERT, then the allocation unit that is about 
        // to be deallocated is damaged.
        bool bValidateAllocUnit = ValidateAllocUnit(pkUnit);
        NI_UNUSED_ARG(bValidateAllocUnit);
        NIMEMASSERTUNIT(bValidateAllocUnit, pkUnit);

        // If you hit this NIMEMASSERT, then this deallocation was made from a 
        // source that isn't setup to use this memory tracking software, 
        // use the stack frame to locate the source and include
        // NiMemManager.h
        NIMEMASSERTUNIT(eEventType != NI_UNKNOWN, pkUnit);

        // If you hit this NIMEMASSERT, you were trying to deallocate RAM that 
        // was not allocated in a way that is compatible with the 
        // deallocation method requested. In other words, you have a 
        // allocation/deallocation mismatch.
        NIMEMASSERTUNIT((eEventType == NI_OPER_DELETE && 
            pkUnit->m_eAllocType == NI_OPER_NEW) ||
            (eEventType == NI_OPER_DELETE_ARRAY && 
            pkUnit->m_eAllocType == NI_OPER_NEW_ARRAY) ||
            (eEventType == NI_FREE && 
            pkUnit->m_eAllocType == NI_MALLOC) ||
            (eEventType == NI_FREE && 
            pkUnit->m_eAllocType == NI_REALLOC) ||
            (eEventType == NI_REALLOC && 
            pkUnit->m_eAllocType == NI_MALLOC) ||
            (eEventType == NI_REALLOC && 
            pkUnit->m_eAllocType == NI_REALLOC) ||
            (eEventType == NI_ALIGNEDFREE && 
            pkUnit->m_eAllocType == NI_ALIGNEDMALLOC) ||
            (eEventType == NI_ALIGNEDFREE && 
            pkUnit->m_eAllocType == NI_ALIGNEDREALLOC) ||
            (eEventType == NI_ALIGNEDREALLOC && 
            pkUnit->m_eAllocType == NI_ALIGNEDMALLOC) ||
            (eEventType == NI_ALIGNEDREALLOC && 
            pkUnit->m_eAllocType == NI_ALIGNEDREALLOC) ||
            (eEventType == NI_UNKNOWN), pkUnit);

        // update allocation unit
        NiMemEventType eDeallocType = eEventType;
        float fDeallocTime = fTime;

        // Determine how much memory was actually set
        size_t stSizeUnused = MemoryBytesWithPattern(pvMemory,
            pkUnit->m_stSizeRequested);
        m_stUnusedButAllocatedMemory += stSizeUnused;

        // Save the thread id that freed the memory
        unsigned long ulFreeThreadId = NiGetCurrentThreadId();

        if (m_bCheckArrayOverruns)
        {
            // If you hit this NIMEMASSERT, you have code that overwrites
            // either before or after the range of an allocation.
            // Check the pkUnit for information about which allocation
            // is being overwritten. 
            bool bCheckForArrayOverrun =
                CheckForArrayOverrun(pvMemory, pkUnit->m_stAlignment,
                    pkUnit->m_stSizeRequested);
            NI_UNUSED_ARG(bCheckForArrayOverrun);
            NIMEMASSERTUNIT(!bCheckForArrayOverrun, pkUnit);

            if (stSizeInBytes != NI_MEM_DEALLOC_SIZE_DEFAULT)
            {
                stSizeInBytes= PadForArrayOverrun(pkUnit->m_stAlignment,
                    stSizeInBytes);
            }
        }

        // Perform the actual deallocation
        m_pkActualAllocator->Deallocate(pvMemory, eEventType, stSizeInBytes);

        // Remove this allocation unit from the hash table
        RemoveAllocUnit(pkUnit);

        // update summary statistics
        --m_stActiveAllocationCount;
        m_stActiveMemory -= pkUnit->m_stSizeAllocated;
        
        // Validate every single allocated unit in memory
        if (m_bAlwaysValidateAll)
        {
            bool bValidateAllAllocUnits =
                ValidateAllAllocUnits();
            NI_UNUSED_ARG(bValidateAllAllocUnits);
            NIMEMASSERTUNIT(bValidateAllAllocUnits, pkUnit);
        }

        // Write out the freed memory to the memory log
        LogAllocUnit(pkUnit, MEM_LOG_COMPLETE, "\t", eDeallocType,
            fDeallocTime, ulFreeThreadId, stSizeUnused);
        
        // Recycle the allocation unit
        // Add it to the front of the reservoir 
        pkUnit->m_kFLF = NiFLF::UNKNOWN;
        pkUnit->m_pkNext = m_pkReservoir;
        m_pkReservoir = pkUnit;
   
        // Validate every single allocated unit in memory
        if (m_bAlwaysValidateAll)
        {
            bool bValidateAllAllocUnits =
                ValidateAllAllocUnits();
            NI_UNUSED_ARG(bValidateAllAllocUnits);
            NIMEMASSERTUNIT(bValidateAllAllocUnits, pkUnit);
        }
        m_kCriticalSection.Unlock();
    
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)

    }
    catch(const char *err)
    {
        // Deal with the errors

    }
#endif
    }
}
//---------------------------------------------------------------------------
bool NiMemTracker::TrackAllocate(
    const void* const pvMemory, 
    size_t stSizeInBytes, 
    NiMemHint kHint,
    NiMemEventType eEventType, 
    const char* pcFile, 
    int iLine,
    const char* pcFunction)
{
    m_kCriticalSection.Lock();
    
    size_t stSizeOriginal = stSizeInBytes;
    float fTime = NiGetCurrentTimeInSec();  
    
    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // function name.
    NIMEMASSERT(strcmp(ms_pcBreakOnFunctionName, pcFunction) != 0);

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // allocation ID.
    NIMEMASSERT(ms_stBreakOnAllocID != m_stCurrentAllocID);

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific 
    // allocation request size.
    NIMEMASSERT(ms_stBreakOnSizeRequested != stSizeOriginal);

    // Actually perform the allocation. Note that size and alignment
    // may be adjusted by the call.
    m_pkActualAllocator->TrackAllocate(pvMemory, stSizeInBytes, kHint,
        eEventType, pcFile, iLine, pcFunction);   

    // update overall summary statistics
    m_stActiveAllocationCount++;
    m_stAccumulatedAllocationCount++;
    if (m_stActiveAllocationCount > m_stPeakAllocationCount)
    {
        m_stPeakAllocationCount = m_stActiveAllocationCount;
        m_fPeakAllocationCountTime = fTime;
    }

    m_stActiveMemory += stSizeInBytes;
    m_stAccumulatedMemory += stSizeInBytes;
    if (m_stActiveMemory > m_stPeakMemory)
    {
        m_stPeakMemory = m_stActiveMemory;
        m_fPeakMemoryTime = fTime;
    }

    // update external-specific summary statistics
    m_stActiveExternalAllocationCount++;
    m_stAccumulatedExternalAllocationCount++;
    if (m_stActiveExternalAllocationCount > m_stPeakExternalAllocationCount)
    {
        m_stPeakExternalAllocationCount = m_stActiveExternalAllocationCount;
    }

    m_stActiveExternalMemory += stSizeInBytes;
    m_stAccumulatedExternalMemory += stSizeInBytes;
    if (m_stActiveExternalMemory > m_stPeakExternalMemory)
    {
        m_stPeakExternalMemory = m_stActiveExternalMemory;
    }

    if (pvMemory == NULL)
    {
        // If you hit this, your memory request was not satisfied
        m_kCriticalSection.Unlock();
        return true;
    }

    // If you hit this NIMEMASSERT, you requested a breakpoint on a specific
    // allocation address range.
    NIMEMASSERT(!IsInsideBreakRange(pvMemory, stSizeInBytes));

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    try
    {
#endif

    // If you hit this NIMEMASSERT, the somehow you have allocated a memory
    // unit to an address that already exists. This should never happen
    // and is an indicator that something has gone wrong in the sub-allocator.
    NIMEMASSERT(FindAllocUnit(pvMemory) == NULL);

    // Grow the tracking unit reservoir if necessary
    if (!m_pkReservoir)
        GrowReservoir();

    // If you hit this NIMEMASSERT, the free store for allocation units
    // does not exist. This should only happen if the reservoir 
    // needed to grow and was unable to satisfy the request. In other words,
    // you may be out of memory.
    NIMEMASSERT (m_pkReservoir != NULL);

    // Get an allocation unit from the reservoir
    NiAllocUnit* pkUnit = m_pkReservoir;
    m_pkReservoir = pkUnit->m_pkNext;

    // fill in the known information
    pkUnit->Reset();
    pkUnit->m_stAllocationID = m_stCurrentAllocID;
    pkUnit->m_stAlignment = 0;
    pkUnit->m_ulAllocThreadId = NiGetCurrentThreadId();
    pkUnit->m_eAllocType = eEventType;
    pkUnit->m_kAllocHint = kHint;
    pkUnit->m_fAllocTime = fTime;
    pkUnit->m_pvMem = (void*) pvMemory;
    pkUnit->m_stSizeRequested = stSizeOriginal;
    pkUnit->m_stSizeAllocated = stSizeInBytes;
    pkUnit->m_kFLF.Set(pcFile, iLine, pcFunction);

    // If you hit this NIMEMASSERT, then this allocation was made from a 
    // source that isn't setup to use this memory tracking software, use the 
    // stack frame to locate the source and include NiMemManager.h.
    NIMEMASSERTUNIT(eEventType != NI_UNKNOWN, pkUnit);

    // Insert the new allocation into the hash table
    InsertAllocUnit(pkUnit);

    // Validate every single allocated unit in memory
    if (m_bAlwaysValidateAll)
    {
        bool bValidateAllAllocUnits = ValidateAllAllocUnits();
        NI_UNUSED_ARG(bValidateAllAllocUnits);
        NIMEMASSERTUNIT(bValidateAllAllocUnits, pkUnit);
    }

#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
    }
    catch(const char *err)
    {
        // Deal with the errors
        // Deal with the errors
    }
#endif

    ++m_stCurrentAllocID;
    m_kCriticalSection.Unlock();
    
    return true;
}
//---------------------------------------------------------------------------
bool NiMemTracker::TrackDeallocate(
    const void* const pvMemory, 
    NiMemEventType eEventType)
{
    if (pvMemory)
    {
        m_kCriticalSection.Lock();
    
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
        try {
#endif
        float fTime = NiGetCurrentTimeInSec();

        // Search the tracking unit hash table to find the address
        NiAllocUnit* pkUnit = FindAllocUnit(pvMemory);

        if (pkUnit == NULL)
        {
            // If you hit this NIMEMASSERT, you tried to deallocate RAM that 
            // wasn't allocated by this memory manager. This may also
            // be indicative of a double deletion. Please check the pkUnit
            // FLF for information about the allocation.
            NIMEMASSERT(pkUnit != NULL);
#if defined(NIMEMORY_ENABLE_EXCEPTIONS)
            throw "Request to deallocate RAM that was never allocated";
#endif
            m_kCriticalSection.Unlock();

            return true;
        }

        // If you hit this NIMEMASSERT, you requested a breakpoint on a 
        // specific allocation ID.
        NIMEMASSERTUNIT(ms_stBreakOnAllocID != pkUnit->m_stAllocationID, 
            pkUnit);

        // If you hit this NIMEMASSERT, you requested a breakpoint on a 
        // specific allocation request size.
        NIMEMASSERTUNIT(ms_stBreakOnSizeRequested != pkUnit->m_stSizeRequested,
            pkUnit);

        // If you hit this NIMEMASSERT, you requested a breakpoint on a 
        // specific allocation address range.
        NIMEMASSERTUNIT(!IsInsideBreakRange(pkUnit->m_pvMem, 
            pkUnit->m_stSizeRequested), pkUnit);

        // If you hit this NIMEMASSERT, then the allocation unit that is about 
        // to be deallocated is damaged.
        bool bValidateAllocUnit = ValidateAllocUnit(pkUnit);
        NI_UNUSED_ARG(bValidateAllocUnit);
        NIMEMASSERTUNIT(bValidateAllocUnit, pkUnit);

        // If you hit this NIMEMASSERT, then this deallocation was made from a 
        // source that isn't setup to use this memory tracking software, 
        // use the stack frame to locate the source and include
        // NiMemManager.h
        NIMEMASSERTUNIT(eEventType == NI_EXTERNALFREE, pkUnit);

        // If you hit this NIMEMASSERT, you were trying to deallocate RAM that 
        // was not allocated in a way that is compatible with the 
        // deallocation method requested. In other words, you have a 
        // allocation/deallocation mismatch.
        NIMEMASSERTUNIT(pkUnit->m_eAllocType == NI_EXTERNALALLOC, pkUnit);

        // update allocation unit
        NiMemEventType eDeallocType = eEventType;
        float fDeallocTime = fTime;

        // because we don't touch the tracked external address, 
        // simply state the the unused amount is zero
        size_t stSizeUnused = 0;

        // Save the thread id that freed the memory
        unsigned long ulFreeThreadId = NiGetCurrentThreadId();

        // Perform the actual deallocation
        m_pkActualAllocator->TrackDeallocate(pvMemory, eEventType);

        // Remove this allocation unit from the hash table
        RemoveAllocUnit(pkUnit);

        // update overall summary statistics
        --m_stActiveAllocationCount;
        m_stActiveMemory -= pkUnit->m_stSizeAllocated;
        
        // update external-specific summary statistics
        --m_stActiveExternalAllocationCount;
        m_stActiveExternalMemory -= pkUnit->m_stSizeAllocated;

        // Validate every single allocated unit in memory
        if (m_bAlwaysValidateAll)
        {
            bool bValidateAllAllocUnits =
                ValidateAllAllocUnits();
            NI_UNUSED_ARG(bValidateAllAllocUnits);
            NIMEMASSERTUNIT(bValidateAllAllocUnits, pkUnit);
        }

        // Write out the freed memory to the memory log
        LogAllocUnit(pkUnit, MEM_LOG_COMPLETE, "\t", eDeallocType,
            fDeallocTime, ulFreeThreadId, stSizeUnused);
        
        // Recycle the allocation unit
        // Add it to the front of the reservoir 
        pkUnit->m_kFLF = NiFLF::UNKNOWN;
        pkUnit->m_pkNext = m_pkReservoir;
        m_pkReservoir = pkUnit;
   
        // Validate every single allocated unit in memory
        if (m_bAlwaysValidateAll)
        {
            bool bValidateAllAllocUnits =
                ValidateAllAllocUnits();
            NI_UNUSED_ARG(bValidateAllAllocUnits);
            NIMEMASSERTUNIT(bValidateAllAllocUnits, pkUnit);
        }
        m_kCriticalSection.Unlock();
    
#ifdef NIMEMORY_ENABLE_EXCEPTIONS

    }
    catch(const char *err)
    {
        // Deal with the errors

    }
#endif
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiMemTracker::SetMarker(const char* pcMarkerType, 
    const char* pcClassifier, const char* pcString)
{
    NI_UNUSED_ARG(pcClassifier);
    NI_UNUSED_ARG(pcMarkerType);
    NI_UNUSED_ARG(pcString);

#if !defined NI_LOGGER_DISABLE && (!defined(NISHIPPING))
    if (!m_bWriteToLog)
        return true;

    // lock the critical section to keep from inserting bogus
    // markers into the stream
    m_kCriticalSection.Lock();

    const char* pcPrefix =  "\t";
    int iChannel = MEM_LOG_COMPLETE;

    // Write out the marker to the log
    NILOG(iChannel, pcPrefix);
    NILOG(iChannel, "<alloc_marker ");
    
    // the marker may not be XML-safe, so we need to auto-format it
    const char* pcFormattedString = FormatForXML(pcMarkerType);
    NILOG(iChannel, "marker_type='%s' ", pcFormattedString);

    // the marker may not be XML-safe, so we need to auto-format it
    pcFormattedString = FormatForXML(pcClassifier);
    NILOG(iChannel, "classifier='%s' ", pcFormattedString);
    
    // the marker may not be XML-safe, so we need to auto-format it
    pcFormattedString = FormatForXML(pcString);
    NILOG(iChannel, "marker='%s' ", pcFormattedString); 

    NILOG(iChannel, "next_id='%d' ", GetCurrentAllocationID());
    NILOG(iChannel, "thread_id='%lX' ", NiGetCurrentThreadId()); 
    NILOG(iChannel, "time='%08f' ", NiGetCurrentTimeInSec()); 
    NILOG(iChannel, "/>\n");
    
    m_kCriticalSection.Unlock();
#endif
    return true;
}
//---------------------------------------------------------------------------
NiAllocUnit* NiMemTracker::FindAllocUnit(const void* pvMem) const
{
    // Just in case...
    NIMEMASSERT(pvMem != NULL);

    // Use the address to locate the hash index. Note that we shift off the 
    // lower four bits. This is because most allocated addresses will be on 
    // four-, eight- or even sixteen-byte boundaries. If we didn't do this, 
    // the hash index would not have very good coverage.

    unsigned int uiHashIndex = AddressToHashIndex(pvMem);

    NiAllocUnit* pkUnit = m_pkActiveMem[uiHashIndex];
    while(pkUnit)
    {
        if (pkUnit->m_pvMem == pvMem) 
            return pkUnit;

        pkUnit = pkUnit->m_pkNext;
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool NiMemTracker::VerifyAddress(const void* pvMemory)
{
    m_kCriticalSection.Lock();
    
    // Handle both regular pointers and array offset values.
    // from NiStandardAllocator.
    NiAllocUnit* pkUnit = FindAllocUnit(pvMemory);
    if (pkUnit)
    {
        if (m_bCheckArrayOverruns && 
            pkUnit->m_eAllocType != NI_EXTERNALALLOC)
        {
            void* pvMemStore = pkUnit->m_pvMem;
            if(CheckForArrayOverrun(pvMemStore,
               pkUnit->m_stAlignment, pkUnit->m_stSizeRequested))
            {
                NiOutputDebugString("Memory Overrun Found At "
                        "Allocation:\n");
                OutputAllocUnitToDebugStream(pkUnit);
                m_kCriticalSection.Unlock();
                return false;
            }
        }


        m_kCriticalSection.Unlock();
        return true;
    }

    // The Xbox 360 inserts an alignment-sized header into the allocation
    // when a destructor needs to be called. This value holds the count of 
    // elements that were allocated. However, it will be unknown to this 
    // function if such a header were actually added since all we have is
    // an address. Therefore, we must search backwards to some reasonable 
    // degree of certainty to find the matching allocation unit.

    // 4 is the minimum offset/alignment for all supported compilers.
    size_t stOffset = 4;

    // The maximum offset that is supported by this function is 128. 
    // Anything larger is probably unreasonable on modern systems.
    while (stOffset <= 128)
    {
        unsigned char* pcAdjustedMemory = ((unsigned char*)pvMemory) - 
            stOffset;
        pkUnit = FindAllocUnit(pcAdjustedMemory);

        // We must be careful that the the allocation unit exists and is 
        // big enough to actually encompass the address and isn't actually 
        // right in front of it.
        if (pkUnit && (pcAdjustedMemory + pkUnit->m_stSizeRequested) >= 
            (unsigned char*)pvMemory)
        {
            if (m_bCheckArrayOverruns && 
                pkUnit->m_eAllocType != NI_EXTERNALALLOC)
            {
                void* pvMemStore = pkUnit->m_pvMem;
                if(CheckForArrayOverrun(pvMemStore,
                   pkUnit->m_stAlignment, pkUnit->m_stSizeRequested))
                {
                    NiOutputDebugString("Memory Overrun Found At "
                        "Allocation:\n");

                    OutputAllocUnitToDebugStream(pkUnit);
                    m_kCriticalSection.Unlock();
                    return false;
                }

            }
            m_kCriticalSection.Unlock();
            return true;
        }
        // If we've found an allocation unit that exists before the 
        // address we are looking up, there is no need to keep searching,
        // this address is unknown to us.
        else if (pkUnit)
        {
            m_kCriticalSection.Unlock();
            return false;
        }

        // Find the next alignment that is permissible.
        stOffset *= 2;
    }
   
    m_kCriticalSection.Unlock();
    return false;
}
//---------------------------------------------------------------------------
void* NiMemTracker::FindContainingAllocation(const void* pvMemory)
{
    m_kCriticalSection.Lock();
    
    for (unsigned int uiHash = 0; uiHash < ms_uiHashSize; uiHash++)
    {
        NiAllocUnit* pkUnit = m_pkActiveMem[uiHash];
        while(pkUnit)
        {
            void* pvBottom = pkUnit->m_pvMem;
            void* pvTop = (void*)(
                ((size_t)pvBottom) + pkUnit->m_stSizeAllocated);
            if (pvBottom <= pvMemory && pvMemory < pvTop) 
            {
                void* pvResult = pkUnit->m_pvMem;
                m_kCriticalSection.Unlock();
                return pvResult;
            }

            pkUnit = pkUnit->m_pkNext;
        }
    }
   
    m_kCriticalSection.Unlock();
    return NULL;
}
//---------------------------------------------------------------------------
void NiMemTracker::MemoryFillForArrayOverrun(void*& pvMemory, 
    size_t stAlignment, size_t stSizeOriginal)
{   
    char* pcMemArray = (char*) pvMemory;
    pvMemory = pcMemArray + stAlignment;
    MemoryFillWithPattern(pcMemArray, stAlignment);

    pcMemArray = pcMemArray + stAlignment + stSizeOriginal;
    MemoryFillWithPattern(pcMemArray, stAlignment);
}
//---------------------------------------------------------------------------
void NiMemTracker::MemoryFillWithPattern(void* pvMemory, 
    size_t stSizeInBytes)
{
    unsigned char* pcMemArray = (unsigned char*) pvMemory;
    for (unsigned int ui = 0; ui < stSizeInBytes; ui++)
    {
        pcMemArray[ui] = m_ucFillChar;
    }
}
//---------------------------------------------------------------------------
size_t NiMemTracker::MemoryBytesWithPattern(void* pvMemory, 
    size_t stSizeInBytes) const
{
    unsigned char* pcMemArray = (unsigned char*) pvMemory;
    size_t numBytes = 0;
    for (unsigned int ui = 0; ui < stSizeInBytes; ui++)
    {
        if (pcMemArray[ui] == m_ucFillChar)
        {
            numBytes++;
        }
    }

    return numBytes;
}
//---------------------------------------------------------------------------
bool NiMemTracker::CheckForArrayOverrun(void*& pvMemory, 
    size_t stAlignment, size_t stSizeOriginal) const
{
    NIASSERT(m_bCheckArrayOverruns);

    char* pcMemArray = (char*) pvMemory;
    pcMemArray -= stAlignment;
    pvMemory = pcMemArray;

    if (stAlignment != MemoryBytesWithPattern(pcMemArray, stAlignment))
        return true;
    
    pcMemArray = pcMemArray + stAlignment + stSizeOriginal;
    if (stAlignment != MemoryBytesWithPattern(pcMemArray, stAlignment))
        return true;
    
    return false;
}
//---------------------------------------------------------------------------
size_t NiMemTracker::PadForArrayOverrun(
    size_t stAlignment, 
    size_t stSizeOriginal)
{
    return stSizeOriginal + 2 * stAlignment;
}
//---------------------------------------------------------------------------
bool NiMemTracker::ValidateAllocUnit(const NiAllocUnit* pkUnit) const
{
    if (pkUnit->m_stAllocationID > m_stCurrentAllocID)
        return false;

    if (pkUnit->m_stSizeAllocated < pkUnit->m_stSizeRequested)
        return false;

    if (pkUnit->m_stSizeAllocated == 0 ||  pkUnit->m_stSizeRequested == 0)
        return false;

    if (pkUnit->m_pvMem == NULL)
        return false;
    
    if (pkUnit->m_pkNext != NULL && pkUnit->m_pkNext->m_pkPrev != pkUnit)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiMemTracker::ValidateAllAllocUnits() const
{
    unsigned int uiActiveCount = 0;
    for (unsigned int uiHashIndex = 0; uiHashIndex < ms_uiHashSize; 
        uiHashIndex++)
    {
        NiAllocUnit* pkUnit = m_pkActiveMem[uiHashIndex];
        NiAllocUnit* pkPrev = NULL;

        while(pkUnit)
        {
            if (!ValidateAllocUnit(pkUnit))
                return false;
            
            if (pkUnit->m_pkPrev != pkPrev)
                return false;

            if (m_bCheckArrayOverruns)
            {
                void* pvMemStore = pkUnit->m_pvMem;
                if (CheckForArrayOverrun(pvMemStore, pkUnit->m_stAlignment, 
                    pkUnit->m_stSizeRequested))
                {
                    NiOutputDebugString("Memory Overrun Found At "
                        "Allocation:\n");

                    OutputAllocUnitToDebugStream(pkUnit);
                    return false;
                }
            }

            // continue to the next unit
            pkPrev = pkUnit;
            pkUnit = pkUnit->m_pkNext;
            uiActiveCount++;
        }
    }   

    if (uiActiveCount != this->m_stActiveAllocationCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiMemTracker::LogAllocUnit(
    const NiAllocUnit *pkUnit,
    int iChannel,
    const char* pcPrefix,
    NiMemEventType eDeallocType,
    float fDeallocTime,
    unsigned long ulDeallocThreadId,
    size_t stSizeUnused) const
{
    NI_UNUSED_ARG(eDeallocType);
    NI_UNUSED_ARG(fDeallocTime);
    NI_UNUSED_ARG(iChannel);
    NI_UNUSED_ARG(pcPrefix);
    NI_UNUSED_ARG(pkUnit);
    NI_UNUSED_ARG(stSizeUnused);
    NI_UNUSED_ARG(ulDeallocThreadId);

    if (!m_bWriteToLog)
        return;

    NIMEMASSERT(pcPrefix != NULL);

    NILOG(iChannel, pcPrefix);
    NILOG(iChannel, "<alloc_unit ");
    NILOG(iChannel, "id='%d' ", 
        pkUnit->m_stAllocationID);
    NILOG(iChannel, "alloc_thread_id='%lX' ", 
        pkUnit->m_ulAllocThreadId);
    NILOG(iChannel, "dealloc_thread_id='%lX' ", 
        ulDeallocThreadId);
    NILOG(iChannel, "alloc_time='%08f' ", 
        pkUnit->m_fAllocTime);
    NILOG(iChannel, "dealloc_time='%08f' ", 
        fDeallocTime);
    NILOG(iChannel, "life_span='%08f' " , 
        fDeallocTime - pkUnit->m_fAllocTime);
    NILOG(iChannel, "addr='0x%p' ", 
        pkUnit->m_pvMem);
    NILOG(iChannel, "size='%d' ", 
        pkUnit->m_stSizeAllocated);
    NILOG(iChannel, "size_requested='%d' ", 
        pkUnit->m_stSizeRequested);
    NILOG(iChannel, "size_unused='%d' ", 
        stSizeUnused);
    NILOG(iChannel, "alignment='%d' ", 
        pkUnit->m_stAlignment);
    NILOG(iChannel, "alloc_type='%s' ", 
        NiMemManager::NiMemEventTypeToString(pkUnit->m_eAllocType));
    NILOG(iChannel, "alloc_hint='%X' ",
        pkUnit->m_kAllocHint.GetRaw());
    NILOG(iChannel, "dealloc_type='%s' ", 
        NiMemManager::NiMemEventTypeToString(eDeallocType));
    NILOG(iChannel, "file='%s' ", 
        pkUnit->m_kFLF.SourceFileStripper());
    NILOG(iChannel, "line='%d' ", 
        pkUnit->m_kFLF.m_uiLine);
    NILOG(iChannel, "func='%s' ", 
        FormatForXML(pkUnit->m_kFLF.m_pcFunc));
    NILOG(iChannel, "long_file='%s' ", 
        pkUnit->m_kFLF.m_pcFile);
    NILOG(iChannel, "/>\n");
}
//---------------------------------------------------------------------------
void NiMemTracker::LogMemoryReport() const
{
    if (!m_bWriteToLog)
        return;

    LogActiveMemoryReport();
}
//---------------------------------------------------------------------------
const char* NiMemTracker::FormatForXML(const char* pcInString)
{
    static char acOutString[1024];
    NIMEMASSERT(pcInString != NULL);
    size_t stLen = strlen(pcInString);

    // If this assert is hit, then the length of the input string is longer
    // than the sanitization buffer (1024 characters including the NULL).
    NIMEMASSERT(stLen < 1024);

    unsigned int uiIdx = 0;
    for (size_t stSrcIdx = 0; (stSrcIdx < stLen) && (uiIdx < 1023); stSrcIdx++)
    {
        acOutString[uiIdx] = pcInString[stSrcIdx];
        
        if (acOutString[uiIdx] == '<')
        {
            acOutString[uiIdx++] = '&';
            acOutString[uiIdx++] = 'l';
            acOutString[uiIdx++] = 't';
            acOutString[uiIdx++] = ';';
        }
        else if (acOutString[uiIdx] == '>')
        {
            acOutString[uiIdx++] = '&';
            acOutString[uiIdx++] = 'g';
            acOutString[uiIdx++] = 't';
            acOutString[uiIdx++] = ';';
        }
        else if (acOutString[uiIdx] == '&')
        {
            acOutString[uiIdx++] = '&';
            acOutString[uiIdx++] = 'a';
            acOutString[uiIdx++] = 'm';
            acOutString[uiIdx++] = 'p';
            acOutString[uiIdx++] = ';';
        }
        else if (acOutString[uiIdx] == '`')
        {
            acOutString[uiIdx++] = ' ';
        }
        else if (acOutString[uiIdx] == '\'')
        {
            acOutString[uiIdx++] = ' ';
        }
        else
        {
            uiIdx++;
        }
    }

    // If this assert is hit, then the XML-sanitized version of the input
    // was at least 1023 characters and was probably clipped at 1023.
    NIMEMASSERT(uiIdx < 1023);

    acOutString[uiIdx] = '\0';
    return acOutString;
}
//---------------------------------------------------------------------------
void NiMemTracker::LogActiveMemoryReport() const
{
    if (!m_bWriteToLog)
        return;

    NILOG(MEM_LOG_LEAK, "<active_memory_dump timestamp='%f' >\n",
        NiGetCurrentTimeInSec());

    for (unsigned int uiHashIndex = 0; uiHashIndex < ms_uiHashSize; 
        uiHashIndex++)
    {
        NiAllocUnit* pkUnit = m_pkActiveMem[uiHashIndex];
        while(pkUnit)
        {
            LogAllocUnit(pkUnit, MEM_LOG_LEAK, "\t");

            // continue to the next unit
            pkUnit = pkUnit->m_pkNext;
        }
    }
    
    NILOG(MEM_LOG_LEAK, "</active_memory_dump>\n");
}
//---------------------------------------------------------------------------
void NiMemTracker::OutputLeakedMemoryToDebugStream() const
{
    if (!ms_bOutputLeaksToDebugStream)
        return;

    NiOutputDebugString("\nLeaked Memory Report:\n");
    for (unsigned int uiHashIndex = 0; uiHashIndex < ms_uiHashSize; 
        uiHashIndex++)
    {
        NiAllocUnit* pkUnit = m_pkActiveMem[uiHashIndex];
        while(pkUnit)
        {
            OutputAllocUnitToDebugStream(pkUnit);

            // continue to the next unit
            pkUnit = pkUnit->m_pkNext;
        }
    }   
}

//---------------------------------------------------------------------------
void NiMemTracker::OutputActiveToDebugStream(const char* pcStringId,
    size_t stMinAllocID) const
{
    NI_UNUSED_ARG(pcStringId);

    ms_pkTracker->m_kCriticalSection.Lock();
    NILOG("\nActive Memory Report: \"%s\"\n", pcStringId);

    size_t stBaseAllocID = stMinAllocID;
    NILOG("Base Allocation ID: %d\n", stBaseAllocID);

    size_t stNumAllocs = 0;
    size_t stTotalBytes = 0;

    // Find the min/max and byte count
    for (unsigned int uiHashIndex = 0; uiHashIndex < ms_uiHashSize; 
            uiHashIndex++)
    {
        NiAllocUnit* pkUnit = m_pkActiveMem[uiHashIndex];
        while(pkUnit)
        {
            if (pkUnit->m_stAllocationID >= stMinAllocID)
            {
                stNumAllocs++;
                stTotalBytes += pkUnit->m_stSizeAllocated;
            }

            // continue to the next unit
            pkUnit = pkUnit->m_pkNext;
        }
    }

    // Iterate over all allocations in the order of their allocation
    for (size_t stCount = 0; stCount < stNumAllocs;)
    {
        NiAllocUnit* pkClosestUnit = NULL;

        // Find all the units we need to iterate through
        for (unsigned int uiHashIndex = 0; uiHashIndex < ms_uiHashSize; 
            uiHashIndex++)
        {
            NiAllocUnit* pkUnit = m_pkActiveMem[uiHashIndex];
            while(pkUnit)
            {
                if (pkUnit->m_stAllocationID >= stMinAllocID)
                {
                    if (pkClosestUnit == NULL)
                    {
                        pkClosestUnit = pkUnit;
                    }
                    else if (pkUnit->m_stAllocationID < 
                        pkClosestUnit->m_stAllocationID)
                    {
                        pkClosestUnit = pkUnit;
                    }
                }
                // continue to the next unit
                pkUnit = pkUnit->m_pkNext;
            }
        }   

        if (pkClosestUnit)
        {
            OutputAllocUnitToDebugStream(pkClosestUnit,
                stBaseAllocID, false);
            stMinAllocID = pkClosestUnit->m_stAllocationID + 1;
            stCount++;
        }
        else
        {
            break;
        }
    }

    NILOG("\n=============================================================\n");
    NILOG("Active Allocation Count (within id range): %d\n", stNumAllocs);
    NILOG("Total Bytes: %d\n", stTotalBytes);

    
    ms_pkTracker->m_kCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
void NiMemTracker::OutputAllocUnitToDebugStream(NiAllocUnit* pkUnit,
    size_t stBaseAllocID, bool bShowPointer) const
{
    NI_UNUSED_ARG(pkUnit);
    NI_UNUSED_ARG(stBaseAllocID);

    NIMEMASSERT(pkUnit->m_pvMem != 0);

    if (bShowPointer)
    {
        NILOG("%s(%d) : id = %d\tsize = %d\t"
            "addr='0x%p'\n",
            pkUnit->m_kFLF.m_pcFile,
            pkUnit->m_kFLF.m_uiLine,
            pkUnit->m_stAllocationID - stBaseAllocID,
            pkUnit->m_stSizeAllocated,
            pkUnit->m_pvMem);
    }
    else
    {
        NILOG("%s(%d) : id = %d\tsize = %d\t\n",
            pkUnit->m_kFLF.m_pcFile,
            pkUnit->m_kFLF.m_uiLine,
            pkUnit->m_stAllocationID - stBaseAllocID,
            pkUnit->m_stSizeAllocated);
    }

}
//---------------------------------------------------------------------------
void NiMemTracker::LogSummaryStats() const
{
    if (!m_bWriteToLog)
        return;

    NILOG(MEM_LOG_COMPLETE, "\t<memory_summary timestamp='%f' ",
        NiGetCurrentTimeInSec());
    NILOG(MEM_LOG_COMPLETE, "TotalActiveSize='%d' ", 
        m_stActiveMemory);
    NILOG(MEM_LOG_COMPLETE, "PeakActiveSize='%d' ", 
        m_stPeakMemory);
    NILOG(MEM_LOG_COMPLETE, "AccumulatedSize='%d' ", 
        m_stAccumulatedMemory);
    NILOG(MEM_LOG_COMPLETE, "AllocatedButUnusedSize='%d' ",
        m_stUnusedButAllocatedMemory);
    NILOG(MEM_LOG_COMPLETE, "ActiveAllocCount='%d' ", 
        m_stActiveAllocationCount);
    NILOG(MEM_LOG_COMPLETE, "PeakActiveAllocCount='%d' ", 
        m_stPeakAllocationCount);
    NILOG(MEM_LOG_COMPLETE, "TotalAllocCount='%d' ",
        m_stAccumulatedAllocationCount);    
    
    NILOG(MEM_LOG_COMPLETE, "TotalActiveExternalSize='%d' ", 
        m_stActiveExternalMemory);
    NILOG(MEM_LOG_COMPLETE, "PeakActiveExternalSize='%d' ", 
        m_stPeakExternalMemory);
    NILOG(MEM_LOG_COMPLETE, "AccumulatedExternalSize='%d' ", 
        m_stAccumulatedExternalMemory);
    NILOG(MEM_LOG_COMPLETE, "ActiveExternalAllocCount='%d' ", 
        m_stActiveExternalAllocationCount);
    NILOG(MEM_LOG_COMPLETE, "PeakExternalActiveAllocCount='%d' ", 
        m_stPeakExternalAllocationCount);
    NILOG(MEM_LOG_COMPLETE, "TotalExternalAllocCount='%d' ",
        m_stAccumulatedExternalAllocationCount);    
    
    NILOG(MEM_LOG_COMPLETE, "TotalTrackerOverhead='%d' ", 
        m_stActiveTrackerOverhead);    
    NILOG(MEM_LOG_COMPLETE, "PeakTrackerOverhead='%d' ", 
        m_stPeakTrackerOverhead);    
    NILOG(MEM_LOG_COMPLETE, "AccumulatedTrackerOverhead='%d' ", 
        m_stAccumulatedTrackerOverhead);   

    NILOG(MEM_LOG_COMPLETE, "></memory_summary>\n");
}
//---------------------------------------------------------------------------
