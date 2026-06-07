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

#include "XSIImportPluginLogger.h"

#include "NiXMLLogger.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // XSI import plugin logger.
    //---------------------------------------------------------------------------

    XSIImportPluginLogger::XSIImportPluginLogger(NiXMLLogger* pkXMLLogger)
        : m_pkXMLLogger(pkXMLLogger)
        , m_ConversionStarted(false)
    {
    }

    //---------------------------------------------------------------------------
    XSIImportPluginLogger::~XSIImportPluginLogger()
    {
        EndConversion();
    }

    //---------------------------------------------------------------------------
    void XSIImportPluginLogger::StartConversion()
    {
        if (m_pkXMLLogger && !m_ConversionStarted)
        {
            m_pkXMLLogger->StartElement(XSI_GAMEBRYO_XML_TAG);
            Log(LOG_INFO, "Conversion started.");
            m_ConversionStarted = true;
        }
    }

    //---------------------------------------------------------------------------
    void XSIImportPluginLogger::EndConversion()
    {
        if (m_pkXMLLogger && m_ConversionStarted)
        {
            Log(LOG_INFO, "Conversion ended.");
            m_pkXMLLogger->EndElement(XSI_GAMEBRYO_XML_TAG);
            m_ConversionStarted = false;
        }
    }

    //---------------------------------------------------------------------------
    bool XSIImportPluginLogger::Log(LogLevel in_Level, const char* in_Msg)
    {
        if (in_Msg && in_Msg[0] && m_pkXMLLogger)
        {
            const char * levelName;
            switch (in_Level)
            {
                case LOG_DEBUG:   levelName = XSI_GAMEBRYO_XML_DEBUG_TAG;   break;
                case LOG_INFO:    levelName = XSI_GAMEBRYO_XML_INFO_TAG;    break;
                case LOG_WARNING: levelName = XSI_GAMEBRYO_XML_WARNING_TAG; break;
                case LOG_ERROR:   levelName = XSI_GAMEBRYO_XML_ERROR_TAG;   break;
                default:          levelName = XSI_GAMEBRYO_XML_UNKNOWN_TAG; break;
            }
            m_pkXMLLogger->LogElement(levelName, in_Msg);
        }

        return in_Level < LOG_ERROR;
    }

    //---------------------------------------------------------------------------
}
