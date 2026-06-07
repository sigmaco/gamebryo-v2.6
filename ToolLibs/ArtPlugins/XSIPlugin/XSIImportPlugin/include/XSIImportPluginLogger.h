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

#ifndef XSI_IMPORT_PLUGIN_LOGGER_H
#define XSI_IMPORT_PLUGIN_LOGGER_H

#include "CrosswalkGamebryoLogger.h"

#include "NiPluginToolkitLibType.h"

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class NIPLUGINTOOLKIT_ENTRY NiXMLLogger;

namespace epg
{
    //---------------------------------------------------------------------------
    // XML tag used by logger.
    //---------------------------------------------------------------------------

    #define XSI_GAMEBRYO_XML_TAG            "XSI_to_Gamebryo"
    #define XSI_GAMEBRYO_XML_DEBUG_TAG      "Debug"
    #define XSI_GAMEBRYO_XML_INFO_TAG       "Info"
    #define XSI_GAMEBRYO_XML_WARNING_TAG    "Warning"
    #define XSI_GAMEBRYO_XML_ERROR_TAG      "Error"
    #define XSI_GAMEBRYO_XML_UNKNOWN_TAG    "Unknown"

    //---------------------------------------------------------------------------
    // XSI import plugin logger.
    //---------------------------------------------------------------------------

    class XSIImportPluginLogger : public Logger
    {
    public:
        XSIImportPluginLogger(NiXMLLogger* pkXMLLogger);
        ~XSIImportPluginLogger();

        virtual void StartConversion();
        virtual void EndConversion();

        virtual bool Log(LogLevel in_Level, const char* in_Msg);

    private:
        NiXMLLogger*        m_pkXMLLogger;
        bool                m_ConversionStarted;
    };
}

#endif /* XSI_IMPORT_PLUGIN_LOGGER_H */
