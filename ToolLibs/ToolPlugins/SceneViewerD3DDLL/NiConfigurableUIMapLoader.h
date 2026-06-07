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

#ifndef  NICONFIGURABLEUIMAPLOADER_H
#define  NICONFIGURABLEUIMAPLOADER_H

#include "NiConfigurableUIMap.h"
#include <NiMain.h>
#include "NiConfigurableUIConstants.h"

class NiConfigurableUIMapLoader : public NiMemObject
{
public:
    static void Init();
    static void SetConstantsLookup(NiConfigurableUIConstantsPtr kConstants);
    static void Cleanup();
    static NiConfigurableUIMap* LoadConfigurableUIMapFromFile(
        char* pcFilename, NiSceneKeyboard* pkKeyboard, NiSceneMouse* pkMouse);
    static NiConfigurableUIMap* LoadConfigurableUIMapFromString(
        char* pcFilename, NiSceneKeyboard* pkKeyboard, NiSceneMouse* pkMouse);
    static bool ParseKeyboardLine(char acLine[], NiConfigurableUIMap* pkUIMap);
    static bool ParseMouseLine(char acLine[], NiConfigurableUIMap* pkUIMap);
    
    static void ShowDebugOutputStrings(bool b);
    static bool IsShowingDebugOutputStrings();

protected:
    static NiConfigurableUIConstantsPtr ms_spConstants;
    
    static NiConfigurableUIMap* LoadConfigurableUIMap(NiBinaryStream& kFile,
        NiSceneKeyboard* pkKeyboard, NiSceneMouse* pkMouse);
    static bool InitializeKeyboard(NiBinaryStream& kFile, 
        NiConfigurableUIMap* pkUIMap);
    static bool InitializeMouse(NiBinaryStream& kFile, 
        NiConfigurableUIMap* pkUIMap);
    
    static const unsigned int ms_uiBufferSize;
    static char* ms_acDelimiters;

    static bool GetBinaryKeyboardModifier(char* pcModifier, long& lModifier);
    static bool GetBinaryKeyboardCode(char* pcKeycode, 
        NiSceneKeyboard::KeyCode& kKeycode);
    static bool GetCommandID(char* pcRHS, unsigned int& uiClassID);
    static bool GetParameters(char* pcParams, NiSceneCommandInfo* pkInfo);
    static bool GetSensitivity(char* pcSensitivity, 
        NiDevice::DeviceState& eDeviceState);
    static bool GetBinaryMouseModifier(char* pcModifier, long& m_lModifiers);

    static bool HandleRHS(char* pcRHS, NiSceneCommandInfo* pkInfo);
    static void PrintCommandInfo(NiSceneCommandInfo* pkCommandInfo, long lKey);
    static void PrintDebug(char* pcDebug);
    static void PrintError(char* pcError);
    static bool ms_bDebugOutput;
};

#endif