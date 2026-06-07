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

#include "NiMeshProfileErrorHandler.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiMeshProfileErrorHandler::~NiMeshProfileErrorHandler()
{
    ClearMessages();
}
//---------------------------------------------------------------------------
void NiMeshProfileErrorHandler::ReportAnnotation(const char* pcFormat, ...)
{
    NIASSERT(pcFormat);
    va_list kArgs;
    va_start(kArgs, pcFormat);
    char acMessage[MAX_PROFILE_ERROR_LENGTH];
    int iRet = NiVsnprintf(&acMessage[0], MAX_PROFILE_ERROR_LENGTH, 
        NI_TRUNCATE, pcFormat, kArgs);
    NIASSERT(iRet);
    if (iRet)
        m_kMessages.Add(NiNew Message(&acMessage[0], MT_ANNOTATION));
    va_end(kArgs);
    return;
}
//---------------------------------------------------------------------------
void NiMeshProfileErrorHandler::ReportError(const char* pcFormat, ...)
{
    NIASSERT(pcFormat);
    va_list kArgs;
    va_start(kArgs, pcFormat);
    char acMessage[MAX_PROFILE_ERROR_LENGTH];

    int iRet = NiVsnprintf(&acMessage[0], MAX_PROFILE_ERROR_LENGTH,
        NI_TRUNCATE, pcFormat, kArgs);

    NIASSERT(iRet);
    if (iRet)
        m_kMessages.Add(NiNew Message(&acMessage[0], MT_ERROR));
    va_end(kArgs);
    return;
}
//---------------------------------------------------------------------------
void NiMeshProfileErrorHandler::ReportWarning(const char* pcFormat, ...)
{
    NIASSERT(pcFormat);
    va_list kArgs;
    va_start(kArgs, pcFormat);
    char acMessage[MAX_PROFILE_ERROR_LENGTH];
    int iRet = NiVsnprintf(&acMessage[0], MAX_PROFILE_ERROR_LENGTH, 
        NI_TRUNCATE,pcFormat, kArgs);
    NIASSERT(iRet);
    if (iRet)
        m_kMessages.Add(NiNew Message(&acMessage[0], MT_WARNING));
    va_end(kArgs);
    return;
}
//---------------------------------------------------------------------------
NiMeshProfileErrorHandler::Message::Message(const NiString& kMessage,
    MESSAGE_TYPE eMessageType) : m_kMessage(kMessage), m_eType(eMessageType)
{
    m_fTimeInSeconds = NiGetCurrentTimeInSec();
}
//---------------------------------------------------------------------------
NiMeshProfileErrorHandler::Message::~Message()
{
}
//---------------------------------------------------------------------------
NiMeshProfileErrorHandler::NiMeshProfileErrorHandler(unsigned int
    uiErrorArraySize) : m_kMessages(uiErrorArraySize)
{
}
//---------------------------------------------------------------------------
unsigned int NiMeshProfileErrorHandler::GetMessageCount() const
{
    return m_kMessages.GetSize();
}
//---------------------------------------------------------------------------
const NiString& NiMeshProfileErrorHandler::GetMessage(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kMessages.GetSize());
    return m_kMessages.GetAt(uiIndex)->m_kMessage;
}
//---------------------------------------------------------------------------
NiMeshProfileErrorHandler::MESSAGE_TYPE NiMeshProfileErrorHandler::
    GetMessageType(unsigned int uiIndex)
    const
{
    NIASSERT(uiIndex < m_kMessages.GetSize());
    return m_kMessages.GetAt(uiIndex)->m_eType;
}
//---------------------------------------------------------------------------
void NiMeshProfileErrorHandler::ClearMessages()
{
    m_kMessages.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiMeshProfileErrorHandler::SaveMessages(const char* pcFileName) const
{
    NiFile* pkFile = NiFile::GetFile(pcFileName, NiFile::WRITE_ONLY);
    if (!pkFile)
        return false;

    NiUInt32 uiCount = m_kMessages.GetSize();

    // Create the output buffer
    char acBuffer[1024];

    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiUInt32 uiMessageLength = 0;
        switch(GetMessageType(ui))
        {
        case MT_ANNOTATION:
            {
                const NiString& kMessage = GetMessage(ui);
                NiStrcpy(acBuffer, sizeof(acBuffer), (const char*)kMessage);
                uiMessageLength = kMessage.Length();
                break;
            }
        case MT_WARNING:
            {
                const NiString& kMessage = GetMessage(ui);
                NiSprintf(acBuffer, sizeof(acBuffer), "WARNING: %s", 
                    (const char*)kMessage);
                uiMessageLength = kMessage.Length() + 9;
                break;
            }
        case MT_ERROR:
            {
                const NiString& kMessage = GetMessage(ui);
                NiSprintf(acBuffer, sizeof(acBuffer), "ERROR: %s", 
                    (const char*)kMessage);
                uiMessageLength = kMessage.Length() + 7;
                break;
            }
        default:
            {
                const NiString& kMessage = GetMessage(ui);
                NiSprintf(acBuffer, sizeof(acBuffer), 
                    "INVALID: %s\n", 
                    (const char*)kMessage);
                uiMessageLength = kMessage.Length() + 9;
            }
        }
        
        pkFile->Write(acBuffer, uiMessageLength);
    }

    NiDelete pkFile;
    pkFile = NULL;

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshProfileErrorHandler::Contains(MESSAGE_TYPE eType) const
{
    NiUInt32 uiCount = m_kMessages.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        if (GetMessageType(ui) == eType)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiMeshProfileErrorHandler::ContainsAnnotations() const
{
    return Contains(MT_ANNOTATION);
}
//---------------------------------------------------------------------------
bool NiMeshProfileErrorHandler::ContainsErrors() const
{
    return Contains(MT_ERROR);
}
//---------------------------------------------------------------------------
bool NiMeshProfileErrorHandler::ContainsWarnings() const
{
    return Contains(MT_WARNING);
}
//---------------------------------------------------------------------------