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

#include "stdafx.h"  
#include "NiConfigurableUIMapWriter.h"

NiConfigurableUIConstantsPtr NiConfigurableUIMapWriter::ms_spConstants = 
    NULL;

//---------------------------------------------------------------------------
void NiConfigurableUIMapWriter::SetConstantsLookup(
    NiConfigurableUIConstantsPtr spConstants)
{
    ms_spConstants = spConstants;
}
//---------------------------------------------------------------------------
void NiConfigurableUIMapWriter::Cleanup()
{
    ms_spConstants = NULL;
}
//---------------------------------------------------------------------------
void NiConfigurableUIMapWriter::Init()
{
    ms_spConstants = NiNew NiConfigurableUIConstants();
}

//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::SaveToFile(NiConfigurableUIMap* pkMap, 
    char* pcFilename)
{
    NiFile* pkFile = NiFile::GetFile(pcFilename, NiFile::WRITE_ONLY);
    if (!pkFile || !(*pkFile))
    {
        NiOutputDebugString("Unable to open file, returning NULL\n"); 
        NiDelete pkFile;
        return false;
    }
    bool bReturn = SaveToStream(pkMap, *pkFile);
    NiDelete pkFile;
    return bReturn;
}
//---------------------------------------------------------------------------
char* NiConfigurableUIMapWriter::SaveToString(NiConfigurableUIMap* pkMap)
{
   NiMemStream kStream;
   if (SaveToStream(pkMap, kStream))
   {
        kStream.Freeze(true);
        char* pcStr = (char*) kStream.Str();
        pcStr[kStream.GetSize()] = '\0';
        return pcStr;
   }
   else
   {
       return NULL;
   }
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::SaveToStream(NiConfigurableUIMap* pkMap, 
    NiBinaryStream& kStream)
{
    if ( pkMap == NULL)
        return false;
    
    NiTList<NiSceneCommandInfo*>* pkCommandList = NULL;
    NiTListIterator kIterator = NULL;
    if (ms_spConstants == NULL)
        ms_spConstants = NiNew  NiConfigurableUIConstants();

    const char* pcHeader = ms_spConstants->GetHeader();
    kStream.Write(pcHeader, (unsigned int)strlen(pcHeader));
    kStream.Write("\n", 1);
    const char* pcName = pkMap->GetName();
    kStream.Write("[\"", 2);
    if (strlen(pcName) == 0)
        kStream.Write("UnNamed UI Map", 14);
    else
        kStream.Write(pcName, (unsigned int)strlen(pcName));
    kStream.Write("\"]\n", 3);

    NiSceneCommandInfo* pkCommandInfo = NULL;

    bool bWroteBefore = false;
    for (int i = NiSceneCommandInfo::MOUSE; 
        i < NiSceneCommandInfo::NUM_DEVICES; i++)
    {
        const char* pcBuffer = ms_spConstants->GetUITypeString(i);
        if (bWroteBefore)
            kStream.Write("\n", 1);
        else
            bWroteBefore = true;

        kStream.Write(pcBuffer, (unsigned int)strlen(pcBuffer));
        kStream.Write("\n", 1);

        // Get the correct command list
        if (i == NiSceneCommandInfo::MOUSE && pkMap->UsesMouse())
        {
            pkCommandList = pkMap->GetMouseList()->GetList();
        }
        else if (i == NiSceneCommandInfo::KEYBOARD && pkMap->UsesKeyboard())
        {
            pkCommandList = pkMap->GetKeyboardList()->GetList();
        }
        else
        {
            pkCommandList = NULL;
        }

        if (pkCommandList != NULL)
        {
            kIterator = pkCommandList->GetHeadPos();
            while (kIterator != NULL)
            {
                pkCommandInfo = pkCommandList->Get(kIterator);
                if (pkCommandInfo != NULL)
                {
                    if (!WriteInfo(i, pkCommandInfo, kStream))
                        return false;
                }
                kIterator = pkCommandList->GetNextPos(kIterator);
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteInfo(int UIType, 
    NiSceneCommandInfo* pkInfo, NiBinaryStream& kStream)
{ 
    bool bKeepGoing = true;
    // Step 1: write out modifiers
    if (bKeepGoing)
        bKeepGoing = WriteModifiers(UIType, pkInfo, kStream);
    // Step 2: write out main id code & equals sign
    if (bKeepGoing)
        bKeepGoing = WriteMainInput(UIType, pkInfo, kStream);

    if (bKeepGoing)
    {
        const char acString[] = " = ";
        kStream.Write(acString, (unsigned int)strlen(acString));
    }

    // Step 3: write out command string
    if (bKeepGoing)
        bKeepGoing = WriteName(pkInfo, kStream);
    // Step 4: write out parameters to command
    if (bKeepGoing)
        bKeepGoing = WriteParams(UIType, pkInfo, kStream);
    // Step 5: write out what kind of event it is
    if (bKeepGoing)
        bKeepGoing = WriteEventType(UIType, pkInfo, kStream);
    // Step 6: write out the event code priority
    if (bKeepGoing)
        bKeepGoing = WritePriority(pkInfo, kStream);
    // Step 7: write out command string name
    if (bKeepGoing)
        bKeepGoing = WriteUserCommandName(pkInfo, kStream);

    return bKeepGoing;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteModifiers(int iUIType, 
    NiSceneCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    switch (iUIType)
    {
    case NiSceneCommandInfo::KEYBOARD:
        return WriteKeyboardModifiers(
            (NiSceneKeyboardCommandInfo*)pkCommandInfo, kStream);
        break;
    case NiSceneCommandInfo::MOUSE:
        return WriteMouseModifiers(
            (NiSceneMouseCommandInfo*)pkCommandInfo, kStream);
        break;
    default:
        break;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteKeyboardModifiers(
    NiSceneKeyboardCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{
    if (pkCommandInfo->m_lModifiers == NiSceneKeyboard::KMOD_NONE)
        return true;

    NiSceneKeyboard::Modifiers kMod = 
        (NiSceneKeyboard::Modifiers)pkCommandInfo->m_lModifiers;
    char acString[256];
    acString[0] = '\0';
    const char* pcString = ms_spConstants->GetKeyModifierString(kMod);
    if (pcString == NULL)
        return false;
    NiSprintf(acString, 256, "%s%s + ", acString, pcString);
    kStream.Write(acString, (unsigned int)strlen(acString));
    return true;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteMouseModifiers(
    NiSceneMouseCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    if (pkCommandInfo->m_lModifiers == NiSceneMouse::MMOD_NONE)
    {
        kStream.Write("NONE", 4);
        return true;
    }
    char acString[256];
    unsigned int uiMouseButtons = pkCommandInfo->m_lModifiers;
    NiSceneMouse::Modifiers eCurrentModifier =  NiSceneMouse::MMOD_NONE;
    bool bFirst = true;
    while (uiMouseButtons != NiSceneMouse::MMOD_NONE)
    {
        // Determine which mouse modifier we are currently on
        // We will iterate through this until there are no
        // more modifiers to contend with.

        if ((uiMouseButtons & NiSceneMouse::MMOD_CONTROL) == 
            NiSceneMouse::MMOD_CONTROL)
        {
            uiMouseButtons -= NiSceneMouse::MMOD_CONTROL;
            eCurrentModifier = NiSceneMouse::MMOD_CONTROL;
        }
        else if ((uiMouseButtons & NiSceneMouse::MMOD_MENU) == 
            NiSceneMouse::MMOD_MENU)
        {
            uiMouseButtons -= NiSceneMouse::MMOD_MENU;
            eCurrentModifier = NiSceneMouse::MMOD_MENU;
        }
        else if ((uiMouseButtons & NiSceneMouse::MMOD_WIN) == 
            NiSceneMouse::MMOD_WIN)
        {
            uiMouseButtons -= NiSceneMouse::MMOD_WIN;
            eCurrentModifier = NiSceneMouse::MMOD_WIN;
        }
        else if ((uiMouseButtons & NiSceneMouse::MMOD_SHIFT) == 
            NiSceneMouse::MMOD_SHIFT)
        {
            uiMouseButtons -= NiSceneMouse::MMOD_SHIFT;
            eCurrentModifier = NiSceneMouse::MMOD_SHIFT;
        }        
        else if ((uiMouseButtons & NiSceneMouse::MMOD_LEFT) == 
            NiSceneMouse::MMOD_LEFT)
        {
            uiMouseButtons -= NiSceneMouse::MMOD_LEFT;
            eCurrentModifier = NiSceneMouse::MMOD_LEFT;
        }
        else if ((uiMouseButtons & NiSceneMouse::MMOD_MIDDLE) == 
            NiSceneMouse::MMOD_MIDDLE)
        {
           uiMouseButtons -= NiSceneMouse::MMOD_MIDDLE;
           eCurrentModifier = NiSceneMouse::MMOD_MIDDLE;
        }
        else if ((uiMouseButtons & NiSceneMouse::MMOD_RIGHT) == 
            NiSceneMouse::MMOD_RIGHT)
        {
           uiMouseButtons -= NiSceneMouse::MMOD_RIGHT;
           eCurrentModifier = NiSceneMouse::MMOD_RIGHT;        
        }
        else if ((uiMouseButtons & NiSceneMouse::MMOD_WHEEL) == 
            NiSceneMouse::MMOD_WHEEL)
        {
           uiMouseButtons -= NiSceneMouse::MMOD_WHEEL;
           eCurrentModifier = NiSceneMouse::MMOD_WHEEL;
        }

        // Convert the modifier code to text
        const char* pcString = 
            ms_spConstants->GetMouseModifierString(eCurrentModifier);

        if (pcString == NULL)
            return false;
        // Write the text to the stream
        if (bFirst)
        {
            bFirst = false;
            NiSprintf(acString, 256, "%s", pcString);
        }
        else
        {
            NiSprintf(acString, 256, "%s + %s", acString, pcString);
        }
    }

    kStream.Write(acString, (unsigned int)strlen(acString));
    return true;
} 
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteMainInput(int iUIType, 
    NiSceneCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    bool bRet = false;
    switch (iUIType)
    {
    case NiSceneCommandInfo::KEYBOARD:
        bRet = WriteMainKeyboardInput(
            (NiSceneKeyboardCommandInfo*)pkCommandInfo, kStream);
        break;
    case NiSceneCommandInfo::MOUSE:
        bRet = WriteMainMouseInput(
            (NiSceneMouseCommandInfo*)pkCommandInfo, kStream);
        break;
    default:
        return false;
        break;
    }

    if (bRet == false)
        return false;

    return true;
} 
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteMainKeyboardInput(
    NiSceneKeyboardCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    char acString[256];
    const char* pcString = 
        ms_spConstants->GetKeyTableString(pkCommandInfo->m_eKey);

    if (pcString == NULL)
        return false;
    NiSprintf(acString, 256, "%s", pcString);

    kStream.Write(acString, (unsigned int)strlen(acString));
    return true;
} 
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteMainMouseInput(
    NiSceneMouseCommandInfo*, NiBinaryStream&)
{ 
   //Handled in modifiers for this device
    return true;
} 
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteName(NiSceneCommandInfo* pkInfo, 
    NiBinaryStream& kStream)
{ 
    char* pcString = 
        NiSceneCommandRegistry::GetCommandName(pkInfo->m_uiClassID);
    if (pcString != NULL)
    {
        kStream.Write(pcString, (unsigned int)strlen(pcString));
    }
    else
    {
        pcString = NiSceneCommandRegistry::GetCommandName(NI_NOCOMMAND);
        kStream.Write(pcString, (unsigned int)strlen(pcString));
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteParams(int iUIType, 
    NiSceneCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{
    switch (iUIType)
    {
    case NiSceneCommandInfo::KEYBOARD:
        return WriteKeyboardParams((NiSceneKeyboardCommandInfo*)pkCommandInfo,
            kStream);
        break;
    case NiSceneCommandInfo::MOUSE:
        return WriteMouseParams((NiSceneMouseCommandInfo*)pkCommandInfo,
            kStream);            
        break;
    default:
        break;
    }
    return false;
} 
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteKeyboardParams(
    NiSceneKeyboardCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    char acString[512];
    NiSprintf(acString, 256, "( \0");
    for (unsigned int ui = 0; 
        ui < pkCommandInfo->m_kParamList.GetSize(); ui++)
    {
        if (ui == 0)
        {
            NiSprintf(acString, 256, "%s %f", acString, 
                pkCommandInfo->m_kParamList.GetAt(ui));
        }
        else
        {
            NiSprintf(acString, 256, "%s , %f", acString, 
                pkCommandInfo->m_kParamList.GetAt(ui));
        }
    }
    NiSprintf(acString, 256, "%s ) \0", acString);
    kStream.Write(acString, (unsigned int)strlen(acString));
    return true;
} 
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteMouseParams(
    NiSceneMouseCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    char acString[512];
    NiSprintf(acString, 512, "( \0");
    for (unsigned int ui = 0; 
        ui < pkCommandInfo->m_kParamList.GetSize(); ui++)
    {
        if (ui != 0)
            NiSprintf(acString, 512, "%s , ", acString);

        if (ui == (unsigned int) pkCommandInfo->m_iDxAffectedParameterIndex)
        {
            NiSprintf(acString, 512, "%s%s", acString, "DX*");
        }
        else if (ui == 
            (unsigned int)pkCommandInfo->m_iDyAffectedParameterIndex)
        {
            NiSprintf(acString, 512, "%s%s", acString, "DY*");
        }
        else if (ui == 
            (unsigned int)pkCommandInfo->m_iWheelDeltaAffectedParameterIndex)
        {
            NiSprintf(acString, 512, "%s%s", acString, "WHEEL*");
        }
        else if (ui == 
            (unsigned int)pkCommandInfo->m_iXAffectedParameterIndex)
        {
            NiSprintf(acString, 512, "%s%s", acString, "X*");
        }
        else if (ui == 
            (unsigned int)pkCommandInfo->m_iYAffectedParameterIndex)
        {
            NiSprintf(acString, 512, "%s%s", acString, "Y*");
        }

        NiSprintf(acString, 512, "%s%f", acString, 
            pkCommandInfo->m_kParamList.GetAt(ui));
    
    }
    NiSprintf(acString, 512, "%s ) \0", acString);
    kStream.Write(acString, (unsigned int)strlen(acString));
    return true;
} 
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteEventType(int iUIType, 
    NiSceneCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    switch (iUIType)
    {
    case NiSceneCommandInfo::KEYBOARD:
        return WriteKeyboardEventType(
            (NiSceneKeyboardCommandInfo*)pkCommandInfo, kStream);
        break;
    case NiSceneCommandInfo::MOUSE:
        return WriteMouseEventType(
            (NiSceneMouseCommandInfo*)pkCommandInfo, kStream);
        break;
    default:
        break;
    }
    return false;
} 
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteKeyboardEventType(
    NiSceneKeyboardCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    char acString[256];
    const char* pcString = 
        ms_spConstants->GetSensitivityString(pkCommandInfo->m_eDeviceState);

    if (pcString == NULL)
        return false;
    NiSprintf(acString, 256, " %s ", pcString);
    kStream.Write(acString, (unsigned int)strlen(acString));
    return true;
} 
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteMouseEventType(
    NiSceneMouseCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    char acString[256];
    const char* pcString = 
        ms_spConstants->GetSensitivityString(pkCommandInfo->m_eDeviceState);

    if (pcString == NULL)
        return false;
    NiSprintf(acString, 256, " %s ", pcString);
    kStream.Write(acString, (unsigned int)strlen(acString));
    return true;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WritePriority(
    NiSceneCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    char acString[256];
    NiSprintf(acString, 256, " %d", pkCommandInfo->m_iPriority);
    kStream.Write(acString, (unsigned int)strlen(acString));
    return true;
} 
//---------------------------------------------------------------------------
bool NiConfigurableUIMapWriter::WriteUserCommandName(
    NiSceneCommandInfo* pkCommandInfo, NiBinaryStream& kStream)
{ 
    if (pkCommandInfo->m_pcName == NULL)
        return false;
    char acString[256];
    NiSprintf(acString, 256, " \"%s\" ", pkCommandInfo->m_pcName);
    kStream.Write(acString, (unsigned int)strlen(acString));
    kStream.Write("\n", 1);
    return true;
}
//---------------------------------------------------------------------------
