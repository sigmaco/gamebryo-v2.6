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

#include "NiXMLLogger.h"

NiXMLLogger::NiXMLLogger()
{
    m_iLogID = -1;
}
//---------------------------------------------------------------------------
NiXMLLogger::~NiXMLLogger()
{
    if (m_iLogID != -1)
    {
        NILOGDIRECT(m_iLogID, "</LogFile>\n");
        NiLogger::CloseLog(m_iLogID);
    }
}
//---------------------------------------------------------------------------
void NiXMLLogger::CreateLog(const NiString& kFilename)
{
    if (m_iLogID != -1)
    {
        NILOGDIRECT(m_iLogID, "</LogFile>\n");
        NiLogger::CloseLog(m_iLogID);
        m_iLogID = -1;
    }
    m_iLogID = NiLogger::OpenLog(kFilename);
    if (m_iLogID != -1)
    {
        NILOGDIRECT(m_iLogID, "<?xml version=\"1.0\"?>\n");
        NILOGDIRECT(m_iLogID, "<LogFile>\n");
    }
}
//---------------------------------------------------------------------------
void NiXMLLogger::CloseLog()
{
    if (m_iLogID != -1)
    {
        NILOGDIRECT(m_iLogID, "</LogFile>\n");
        NiLogger::CloseLog(m_iLogID);
        m_iLogID = -1;
    }
}
//---------------------------------------------------------------------------
void NiXMLLogger::LogElement(const NiString& kElementName, 
    const NiString& kData)
{
    if ((m_iLogID != -1) && (kData != ""))
    {
        // Before logging the string, make sure it doesn't have any %
        NiString kTempString = kData;
        kTempString.Replace("%", "%%");
        NiString kLogString = "<" + kElementName + ">" + kTempString; 
        NiString kAppendString = "</" + kElementName + ">\n";
        // check against logger character count limitations
        if ((kLogString.Length() + kAppendString.Length()) > 1024)
        {
            kLogString = kLogString.GetSubstring(0, 
                1023 - kAppendString.Length());
        }
        kLogString += kAppendString;
        NILOGDIRECT(m_iLogID, kLogString);
    }
}
//---------------------------------------------------------------------------
void NiXMLLogger::LogData(const NiString& kData)
{
    if (m_iLogID != -1)
    {
        // check against logger character count limitations
        if (kData.Length() > 1024)
        {
            NILOGDIRECT(m_iLogID, kData.GetSubstring(0, 1023));
        }
        else
        {
            NILOGDIRECT(m_iLogID, kData);
        }
    }
}
//---------------------------------------------------------------------------
void NiXMLLogger::StartElement(const NiString& kElementName)
{
    if ((m_iLogID != -1) && (kElementName != ""))
    {
        NiString kLogString = "<" + kElementName + ">\n";
        NILOGDIRECT(m_iLogID, kLogString);
    }
}
//---------------------------------------------------------------------------
void NiXMLLogger::EndElement(const NiString& kElementName)
{
    if ((m_iLogID != -1) && (kElementName != ""))
    {
        NiString kLogString = "</" + kElementName + ">\n";
        NILOGDIRECT(m_iLogID, kLogString);
    }
}
//---------------------------------------------------------------------------

