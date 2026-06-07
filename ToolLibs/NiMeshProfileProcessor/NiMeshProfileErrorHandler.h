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


#ifndef NIMESHPROFILEERRORHANDLER_H
#define NIMESHPROFILEERRORHANDLER_H

#include <NiMeshErrorInterface.h>
#include <NiTPtrSet.h>
#include <NiRefObject.h>
#include <NiString.h>
#include "NiMeshProfileProcessorLibType.h"

#define MAX_PROFILE_ERROR_LENGTH 2048

/// @cond EMERGENT_INTERNAL
/**
    Provides an implementation for the NiMeshErrorInterface and is used by
    the NiMeshProfileProcessor system to report errors, warnings, and 
    annotations.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiMeshProfileErrorHandler : 
    public NiMeshErrorInterface
{
public:
    enum MESSAGE_TYPE
    {
        MT_WARNING = 0,
        MT_ERROR,
        MT_ANNOTATION
    };

    NiMeshProfileErrorHandler(unsigned int uiErrorArraySize = 0);
    virtual ~NiMeshProfileErrorHandler();

    unsigned int GetMessageCount() const;
    const NiString& GetMessage(unsigned int uiIndex) const;
    MESSAGE_TYPE GetMessageType(unsigned int uiIndex) const;
    void ClearMessages();

    bool SaveMessages(const char* pcFileName) const;

    bool ContainsAnnotations() const;
    bool ContainsErrors() const;
    bool ContainsWarnings() const;

    virtual void ReportAnnotation(const char* pcFormat, ...);
    virtual void ReportError(const char* pcFormat, ...);
    virtual void ReportWarning(const char* pcFormat, ...);

protected:

    bool Contains(MESSAGE_TYPE eType) const;

    class Message : public NiRefObject
    {
    public:
        Message(const NiString& kMessage, MESSAGE_TYPE eMessageType);
        ~Message();
        NiString m_kMessage;
        MESSAGE_TYPE m_eType;
        float m_fTimeInSeconds;
    };
    typedef NiPointer<Message> MessagePtr;
    NiTObjectPtrSet<MessagePtr> m_kMessages;
};


#endif // NIMESHPROFILEERRORHANDLER_H
