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

#ifndef NIMESSAGEBOX_H
#define NIMESSAGEBOX_H

#include "NiSystemLibType.h"
#include "NiRTLib.h"

typedef unsigned int (*MessageBoxFunction) 
    (const char* pcText, const char* pcCaption, void* pvExtraData);

class NISYSTEM_ENTRY NiMessageBoxUtilities
{
public:
    static void SetMessageBoxFunction(MessageBoxFunction pfnMessageBox);
    static MessageBoxFunction GetMessageBoxFunction();

    static unsigned int DisplayMessage(const char* pcText, 
        const char* pcCaption, void* pvExtraData = NULL);

protected:
    static unsigned int DefaultMessageBox(const char* pcText, 
        const char* pcCaption, void* pvExtraData = NULL);

    static MessageBoxFunction ms_pfnMessageBox;
};

NISYSTEM_ENTRY unsigned int NiMessageBox(const char* pcText, 
    const char* pcCaption, void* pvExtraData = NULL);

#include "NiMessageBox.inl"

#endif //NIMESSAGEBOX_H
