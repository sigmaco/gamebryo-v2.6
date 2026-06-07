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

#ifndef  NICONFIGURABLEUIMAPWRITER_H
#define  NICONFIGURABLEUIMAPWRITER_H

#include "NiConfigurableUIMap.h"
#include "NiConfigurableUIConstants.h"
#include "NiSceneCommandInfo.h"
#include <NiFile.h>
#include <NiMemStream.h>
#include <NiSystem.h>

class NiConfigurableUIMapWriter : public NiMemObject
{
public:
    static void Init();
    static void SetConstantsLookup(NiConfigurableUIConstantsPtr kConstants);
    static void Cleanup();
    static bool SaveToFile(NiConfigurableUIMap* pkMap, char* pcFilename);
    static char* SaveToString(NiConfigurableUIMap* pkMap);

    static bool SaveToStream(NiConfigurableUIMap* pkMap, 
        NiBinaryStream& kStream);
    static bool WriteInfo(int UIType, NiSceneCommandInfo* pkInfo,
        NiBinaryStream& kStream);
    static bool WriteModifiers(int UIType, NiSceneCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);
    static bool WriteKeyboardModifiers(
        NiSceneKeyboardCommandInfo* pkCommandInfo, NiBinaryStream& kStream);
    static bool WriteMouseModifiers(NiSceneMouseCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);
    static bool WriteMainInput(int UIType, NiSceneCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);
    static bool WriteMainKeyboardInput(
        NiSceneKeyboardCommandInfo* pkCommandInfo, NiBinaryStream& kStream);
    static bool WriteMainMouseInput(NiSceneMouseCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);
    static bool WriteName(NiSceneCommandInfo* pkInfo, NiBinaryStream& kStream);
    static bool WriteParams(int UIType, NiSceneCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);
    static bool WriteKeyboardParams(NiSceneKeyboardCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);
    static bool WriteMouseParams(NiSceneMouseCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);
     static bool WriteEventType(int UIType, NiSceneCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);
    static bool WriteKeyboardEventType(
        NiSceneKeyboardCommandInfo* pkCommandInfo, NiBinaryStream& kStream);
    static bool WriteMouseEventType(NiSceneMouseCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);
    static bool WritePriority(NiSceneCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);
    static bool WriteUserCommandName(NiSceneCommandInfo* pkCommandInfo, 
        NiBinaryStream& kStream);

protected:
    static NiConfigurableUIConstantsPtr ms_spConstants;
};

#endif