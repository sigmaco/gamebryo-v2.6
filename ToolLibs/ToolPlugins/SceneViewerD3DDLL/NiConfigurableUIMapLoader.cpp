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
#include "NiConfigurableUIMapLoader.h"
#include <NiFile.h>
#include <NiMemStream.h>
#include <NiSystem.h>

const unsigned int NiConfigurableUIMapLoader::ms_uiBufferSize = 512;
char* NiConfigurableUIMapLoader::ms_acDelimiters = " ,\t\n";
NiConfigurableUIConstantsPtr NiConfigurableUIMapLoader::ms_spConstants = 
    NULL;
bool NiConfigurableUIMapLoader::ms_bDebugOutput = false;
//---------------------------------------------------------------------------
void NiConfigurableUIMapLoader::SetConstantsLookup(
    NiConfigurableUIConstantsPtr spConstants)
{
    ms_spConstants = spConstants;
}
//---------------------------------------------------------------------------
void NiConfigurableUIMapLoader::Cleanup()
{
    ms_spConstants = NULL;
}
//---------------------------------------------------------------------------
NiConfigurableUIMap* NiConfigurableUIMapLoader::
    LoadConfigurableUIMapFromFile( char* pcFilename,
    NiSceneKeyboard* pkKeyboard, NiSceneMouse* pkMouse)
{
    NiFile* pkFile = NiFile::GetFile(pcFilename, NiFile::READ_ONLY);
    if (!pkFile && !(*pkFile))
    {
        PrintError("Unable to open file, returning NULL\n"); 
        NiDelete pkFile;
        return NULL;
    }
    NiConfigurableUIMap* pkMap = LoadConfigurableUIMap(*pkFile, pkKeyboard, 
        pkMouse);
    NiDelete pkFile;
    return pkMap;
}
//---------------------------------------------------------------------------
NiConfigurableUIMap* 
    NiConfigurableUIMapLoader::LoadConfigurableUIMapFromString(char* pcMap,
    NiSceneKeyboard* pkKeyboard, NiSceneMouse* pkMouse)
{
    if (pcMap == NULL)
    {
        PrintError("Unable to use character array, returning NULL\n");
        return NULL;
    }
    NiMemStream kStream(pcMap, (unsigned int)strlen(pcMap));
    return LoadConfigurableUIMap(kStream, pkKeyboard, pkMouse);
}
//---------------------------------------------------------------------------
NiConfigurableUIMap* NiConfigurableUIMapLoader::LoadConfigurableUIMap(
    NiBinaryStream& kFile, NiSceneKeyboard* pkKeyboard, 
    NiSceneMouse* pkMouse)
{
    if ((pkMouse == 0) && 
        (pkKeyboard == 0))
    {
        PrintError("Returning a NULL value due to NO UI devices\n"); 
        return NULL;
    }

    char acBuf[ms_uiBufferSize];
    kFile.GetLine(acBuf, ms_uiBufferSize);
    if (acBuf!= NULL && NiStricmp(acBuf, ms_spConstants->GetHeader()) != 0)
        return NULL;

    NiConfigurableUIMap* pkUIMap = NiNew NiConfigurableUIMap();

    pkUIMap->SetKeyboard(pkKeyboard);
    pkUIMap->SetMouse(pkMouse);

    kFile.GetLine(acBuf, ms_uiBufferSize);
    if (acBuf == NULL)
        return NULL;
    char* pcName = strstr(acBuf, "\"") + 1;
    char* pcNameEnd = strstr(pcName, "\"");
    pcNameEnd[0] = '\0';
    pkUIMap->SetName(pcName);

    pkUIMap->Initialize();

    while (kFile.GetLine(acBuf, ms_uiBufferSize))
    {
        bool bReturnVal = true;
        char* pcContext;
        char* pcToken = NiStrtok(acBuf, ms_acDelimiters, &pcContext);
        if (pcToken == NULL)
            return pkUIMap;
        
        if (NiStricmp(pcToken, "\0") == 0)
            bReturnVal = true;
        else
        {
            NiSceneCommandInfo::UserInterfaceType m_eType = 
                ms_spConstants->GetUIType(pcToken);
            if (m_eType == NiSceneCommandInfo::KEYBOARD)
                bReturnVal = InitializeKeyboard(kFile, pkUIMap);
            else if (m_eType == NiSceneCommandInfo::MOUSE)
                bReturnVal = InitializeMouse(kFile, pkUIMap);
            
        }

        if (bReturnVal == false)
        {
            PrintError("Returning a NULL value due to format errors\n");
            return NULL;
        }
    }

    return pkUIMap;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::InitializeKeyboard(NiBinaryStream& kFile, 
    NiConfigurableUIMap* pkUIMap)
{
    char acBuf[ms_uiBufferSize];
    char acBufCopy[ms_uiBufferSize];
    while (kFile.GetLine(acBuf, ms_uiBufferSize) && acBuf != NULL)
    {
        NiStrcpy(acBufCopy, ms_uiBufferSize, acBuf);
        PrintDebug("//------------------------------------------\n");
        if (acBuf[0] == '\0')
        {
            PrintDebug("This line is empty\n");
            return true;
        }

        if (!ParseKeyboardLine(acBuf, pkUIMap))
        {
            PrintError("Unable to parse the Keyboard Expression:\n");
            PrintError(acBufCopy);
            PrintError("\n");
            return false;
        }
    }
    pkUIMap->GetKeyboardList()->PrintDebug();
    return true;

}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::InitializeMouse(NiBinaryStream& kFile, 
    NiConfigurableUIMap* pkUIMap)
{
    char acBuf[ms_uiBufferSize];
    char acBufCopy[ms_uiBufferSize];
    while (kFile.GetLine(acBuf, ms_uiBufferSize) && acBuf != NULL)
    {
        NiStrcpy(acBufCopy, ms_uiBufferSize, acBuf);
        PrintDebug("//------------------------------------------\n");
        if (acBuf[0] == '\0')
        {
            PrintDebug("This line is empty\n");
            return true;
        }
        if (!ParseMouseLine(acBuf, pkUIMap))
        {
            PrintError("Unable to parse the Mouse Expression:\n");
            PrintError(acBufCopy);
            PrintError("\n");
            return false;
        }
    }
    pkUIMap->GetMouseList()->PrintDebug();
    return true;

}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::ParseKeyboardLine(char acLine[], 
    NiConfigurableUIMap* pkUIMap)
{
    if (pkUIMap == NULL)
        return false;

    NiSceneKeyboardCommandInfo* pkCommandInfo = 
        NiNew NiSceneKeyboardCommandInfo;
 
    // Divide into the left and right hand sides of the assignment
    // This simplifies the parsing of the code
    char* pcRHS = strrchr(acLine, '=');
    if (pcRHS == NULL || pcRHS == acLine)
    {
        PrintError("There is no equals sign in the expression\n");
        NiDelete pkCommandInfo;
        return false;
    }
    else
    {   
        pcRHS[0] = '\0';
        pcRHS++;
    }
    char* pcLHS = acLine;
    
    // Now we have two seperate strings to deal with, the LHS and RHS of 
    // the expression
    
    // Deal with the modifiers and the keycode itself
    char* pcModifierEnd = strstr(pcLHS, "+");
    char* pcModifier = pcLHS;
    // If we have a modifier, add it to the list
    char* pcContext;
    if (pcModifierEnd != NULL)
    {
        pcLHS = pcModifierEnd;
        pcLHS[0] = '\0';
        pcLHS++;
        pcModifier = NiStrtok(pcModifier, " \t,\n+", &pcContext);
        if (pcModifier == NULL)
        {
            PrintError("There is nothing before the plus sign in the "
                "expression\n");
            NiDelete pkCommandInfo;
            return false;
        }

        if (!GetBinaryKeyboardModifier(pcModifier, 
            pkCommandInfo->m_lModifiers))
        {
            NiDelete pkCommandInfo;
            return false;
        }

        pcModifier = strstr(pcModifierEnd, "+");
        
        if (pcModifier != NULL)
        {
           PrintError("There are too many + signs in the expression\n");
           NiDelete pkCommandInfo;
           return false;
        }

        pcModifierEnd = pcLHS;
    }
    else
    {
        pcModifierEnd = pcLHS;
        pkCommandInfo->m_lModifiers = 0;
    }
    if (!GetBinaryKeyboardCode(NiStrtok(pcModifierEnd, " \t=\n", 
        &pcContext), pkCommandInfo->m_eKey))
    {
        NiDelete pkCommandInfo;
        return false;
    }

    HandleRHS(pcRHS, pkCommandInfo);
    
    pkUIMap->GetKeyboardList()->Add(pkCommandInfo);

    PrintCommandInfo(pkCommandInfo, pkCommandInfo->m_eKey);
    return true;
}
//---------------------------------------------------------------------------
void NiConfigurableUIMapLoader::Init()
{
    ms_spConstants = NiNew NiConfigurableUIConstants();
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::ParseMouseLine(char acLine[], 
    NiConfigurableUIMap* pkUIMap)
{
    if (pkUIMap == NULL)
        return false;
    NiSceneMouseCommandInfo* pkCommandInfo = 
        NiNew NiSceneMouseCommandInfo;

    // Divide into the left and right hand sides of the assignment
    // This simplifies the parsing of the code
    char* pcRHS = strrchr(acLine, '=');
    if (pcRHS == NULL || pcRHS == acLine)
    {
        PrintError("There is no equals sign in the expression\n");
        NiDelete pkCommandInfo;
        return false;
    }
    else
    {
        pcRHS[0] = '\0';
        pcRHS++;
    }

    // Now we have two seperate strings to deal with, the LHS and RHS of 
    // the expression
    bool bLoop = true;
    while (bLoop)
    {
        char* pcModifierEnd = strrchr(acLine,'+');
    
        if (pcModifierEnd == NULL)
        {
            pcModifierEnd = acLine;
            bLoop = false;
        }
        else
        {
            pcModifierEnd[0] = '\0';
            pcModifierEnd++;
        }

        char* pcContext;
        char* pcModifier = NiStrtok(pcModifierEnd, ms_acDelimiters, 
            &pcContext);
        if (pcModifier == NULL && pcModifierEnd == acLine)
        {
            PrintError(
                "There is nothing before the + in this expression\n");
            return false;
        }
        else if (pcModifier == NULL)
        {
            PrintError(
                "There is nothing after the + in this expression\n");
            NiDelete pkCommandInfo;
            return false;
        }

        if (!GetBinaryMouseModifier(pcModifier, 
            pkCommandInfo->m_lModifiers))
        {
            NiDelete pkCommandInfo;
            return false;
        }
    }

    if (!HandleRHS(pcRHS, pkCommandInfo))
    {
        NiDelete pkCommandInfo;
        return false;
    }

    unsigned int uiWhichButtons = pkCommandInfo->m_lModifiers >> 4;
    /*char* acChar = NiAlloc(char, 256);
    sprintf(acChar, "####MOUSE BUTTONS FROM MOD#### = %d\n", uiWhichButtons);
    PrintDebug(acChar);
    NiDelete acChar;
    */
    pkUIMap->GetMouseList()->Add(pkCommandInfo);

    PrintCommandInfo(pkCommandInfo, uiWhichButtons);    
    return true;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::HandleRHS(char* pcRHS, 
    NiSceneCommandInfo* pkInfo)
{
    char* pcParams = strstr(pcRHS, "(");
    char* pcSensitivity = strstr(pcRHS, ")");

    if ((pcSensitivity == NULL && pcParams != NULL) ||
       (pcSensitivity != NULL && pcParams == NULL))
    {
        PrintError("The parenthesis do not match in the expression\n");
        return false;
    }
    else if (pcSensitivity == NULL && pcParams == NULL)
    {
        PrintError("There are NO parenthesis in the expression\n");
        return false;
    }

    pcSensitivity[0] = '\0';
    pcSensitivity++;
    char* pcContext;
    pcSensitivity = NiStrtok(pcSensitivity, ms_acDelimiters, &pcContext);
    char* pcPriority = NiStrtok(NULL, ms_acDelimiters, &pcContext);
    char* pcName = NiStrtok(NULL, "", &pcContext);
    char* pcNameBegin = strstr(pcName, "\"");
    if ( pcNameBegin== pcName)
        pcName++;
    else if ( pcNameBegin != NULL)
        pcName = pcNameBegin + 1;

    char* pcNameEnd = strstr(pcName, "\"");
    if (pcNameEnd != NULL)
        pcNameEnd[0] = '\0';
    
    pcParams[0] = '\0';
    pcParams++;
    if (!GetCommandID(pcRHS, pkInfo->m_uiClassID))
    {
         NiFree(pcSensitivity);
         NiFree(pcParams);
         NiFree(pcRHS);
         return false;
    }
    if (!GetParameters(pcParams, pkInfo))
    {
         NiFree(pcSensitivity);
         NiFree(pcParams);
         NiFree(pcRHS);
         return false;
    }
    if (!GetSensitivity(pcSensitivity, pkInfo->m_eDeviceState))
    {
         NiFree(pcSensitivity);
         NiFree(pcParams);
         NiFree(pcRHS);
         return false;
    }

    if (pcPriority != NULL)
        pkInfo->m_iPriority = atoi(pcPriority);
    else
        return false;

    pkInfo->SetName(pcName);
    return true;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::GetBinaryKeyboardModifier(char* pcModifier,
    long& lModifier)
{
    if (pcModifier == NULL)
    {
        PrintError("No modifier string passed in to "
            "GetBinaryKeyboardModifier\n");
        return false;
    }
    char* pcContext;
    pcModifier = NiStrtok(pcModifier, " \t\n", &pcContext);

    NiSceneKeyboard::Modifiers eMod = 
        ms_spConstants->GetKeyModifier(pcModifier);
    if (eMod != 0)
    {
        lModifier |= eMod;
        return true;
    }
    const char* pcNone = 
        ms_spConstants->GetKeyModifierString(NiSceneKeyboard::KMOD_NONE);
    if (NiStricmp(pcModifier, pcNone) == 0)
        return true;

    PrintError("No match found for the modifier string \"");
    PrintError(pcModifier);
    PrintError("\" in GetBinaryKeyboardModifier\n");
    return false;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::GetBinaryKeyboardCode(char* pcKeycode,
    NiSceneKeyboard::KeyCode& eKeyCode)
{
    if (pcKeycode == NULL)
    {
        PrintError(
            "No keycode string passed in to GetBinaryKeyboardCode\n");
        return false;
    }
    char* pcContext;
    char* pcModifier = NiStrtok(pcKeycode, " \t\n", &pcContext);

    // Parse the string to determine its keycode
    NiSceneKeyboard::KeyCode eReturn = ms_spConstants->GetKey(pcModifier);
    if (eReturn != 0)
    {
        eKeyCode = eReturn;
        return true;
    }

    // Due to the nature of the enumeration for keycode, we may confuse an
    // error for NO KEY. We check that here and return true if not an error
    const char* pcNone = ms_spConstants->GetKeyTableString(
        NiSceneKeyboard::KEY_NOKEY);
    if (NiStricmp(pcModifier, pcNone) == 0)
    {
        eKeyCode = eReturn;
        return true;
    }

    PrintError("No match found for the keycode \"");
    PrintError(pcKeycode);
    PrintError("\" in GetBinaryKeyboardCode\n");
    return false;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::GetCommandID(char* pcRHS, 
    unsigned int& uiClassID)
{
    char* pcContext;
    pcRHS = NiStrtok(pcRHS, ms_acDelimiters, &pcContext);
    uiClassID = NiSceneCommandRegistry::GetCommandID(pcRHS);
    if (uiClassID != 0)
        return true;
    /*for (unsigned int i = 0; i < (int) NUM_CLASS_IDS; i++)
    {
        char* acStr = NiSceneCommandClassIDStrings[i];
        if (acStr != NULL && NiStricmp(acStr, pcRHS) == 0)
        {
            uiClassID = i;
            return true;
        }
    }*/
    PrintError("No match found for the Command \"");
    PrintError(pcRHS);
    PrintError("\" in GetCommandID\n");
//    delete pcRHS;
    return false;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::GetParameters(char* pcParams, 
    NiSceneCommandInfo* pkInfo)
{
    char* pcMasterParams = pcParams;
    if (pcParams[0] == '\0')
    {
        PrintDebug("There are no parameters\n");
        return true;
    }

    // Now we'll deal with each Parameter individually
    char* acDelimiters = " \t\n,(){}[]#;:\0";

    float fParam;
    char* pcContext;
    pcParams = NiStrtok(pcMasterParams, acDelimiters, &pcContext);
    
    PrintDebug("Parameters:\n");

    unsigned int uiIndex = 0;
 
    while (pcParams != NULL)
    {
        //Handle all the MACRO definitions
        if (NiStricmp("X-AXIS", pcParams) == 0 || 
            NiStricmp("-X-AXIS", pcParams) == 0)
        {
            if (NiStricmp("X-AXIS", pcParams) == 0 )
                pkInfo->m_kParamList.Add(1.0f);
            else
                pkInfo->m_kParamList.Add(-1.0f);    

            pkInfo->m_kParamList.Add(0.0f);
            pkInfo->m_kParamList.Add(0.0f);
            PrintDebug("\tX-AXIS\n");
        }
        else if (NiStricmp("Y-AXIS", pcParams) == 0 || 
            NiStricmp("-Y-AXIS", pcParams) == 0)
        {
            pkInfo->m_kParamList.Add(0.0f);

            if (NiStricmp("Y-AXIS", pcParams) == 0 )
                pkInfo->m_kParamList.Add(1.0f);
            else
                pkInfo->m_kParamList.Add(-1.0f); 
            
            pkInfo->m_kParamList.Add(0.0f);
            PrintDebug("\tY-AXIS\n");
        }
        else if (NiStricmp("Z-AXIS", pcParams) == 0 ||
            NiStricmp("-Z-AXIS", pcParams) == 0)
        {
            pkInfo->m_kParamList.Add(0.0f);
            pkInfo->m_kParamList.Add(0.0f);

            if (NiStricmp("Z-AXIS", pcParams) == 0 )
                pkInfo->m_kParamList.Add(1.0f);
            else
                pkInfo->m_kParamList.Add(-1.0f); 
            
            PrintDebug("\tZ-AXIS\n");
        }
        else if (NiStricmp("NULL", pcParams) == 0 ||
            NiStricmp("ORIGIN", pcParams) == 0)
        {
            pkInfo->m_kParamList.Add(0.0f);
            pkInfo->m_kParamList.Add(0.0f);
            pkInfo->m_kParamList.Add(0.0f); 
            
            PrintDebug("\tORIGIN\n");
        }
        else if ((strstr(pcParams ,"DX\0" ) != 0 || 
            strstr(pcParams,"L/R\0") != 0) && 
            (pkInfo->m_eType == NiSceneCommandInfo::MOUSE || 
            pkInfo->m_eType == NiSceneCommandInfo::GAMEPAD))
        {
            float fScaleFactor = 1.0;
            char* pcSubstring = strstr(pcParams ,"DX\0" );
            if (pcSubstring == NULL)
                pcSubstring = strstr(pcParams ,"L/R\0" );
            char* pcMultiplier = strstr(pcParams, "*\0" );

            if (pcMultiplier == NULL)
            {
                //DO NOTHING
            }
            else if (pcMultiplier > pcSubstring)
            {
                char* pcToken = pcMultiplier+1;
                fScaleFactor = (float) atof(pcToken);
            }

            if (strstr(pcParams, "-\0") != NULL && fScaleFactor > 0)
                fScaleFactor*= -1.0f;

            pkInfo->m_kParamList.Add(1.0f*fScaleFactor);
            if (pkInfo->m_eType == NiSceneCommandInfo::MOUSE)
            {
                NiSceneMouseCommandInfo* pkMouseInfo = 
                    ((NiSceneMouseCommandInfo*) pkInfo);
                pkMouseInfo->m_iDxAffectedParameterIndex = uiIndex;
            }
        }
        else if ((strstr( pcParams, "DY\0") != 0 || 
            strstr(pcParams,"F/B\0") != 0) && 
            (pkInfo->m_eType == NiSceneCommandInfo::MOUSE))
        {
            float fScaleFactor = 1.0;
            char* pcSubstring = strstr(pcParams ,"DY\0" );
            if (pcSubstring == NULL)
                  pcSubstring = strstr(pcParams ,"F/B\0" );
            char* pcMultiplier = strstr(pcParams, "*\0" );

            if (pcMultiplier == NULL)
            {
                //DO NOTHING
            }
            else if (pcMultiplier > pcSubstring)
            {
                char* pcToken = pcMultiplier+1;
                fScaleFactor = (float) atof(pcToken);
            }

            if (strstr(pcParams, "-\0") != NULL && fScaleFactor > 0)
                fScaleFactor*= -1.0f;

            pkInfo->m_kParamList.Add(1.0f*fScaleFactor);
            if (pkInfo->m_eType == NiSceneCommandInfo::MOUSE)
            {
                NiSceneMouseCommandInfo* pkMouseInfo = 
                    ((NiSceneMouseCommandInfo*) pkInfo);
                pkMouseInfo->m_iDyAffectedParameterIndex = uiIndex;
            }
        }
        else if ((strstr(pcParams ,"X\0" ) != 0 ) && 
            (pkInfo->m_eType == NiSceneCommandInfo::MOUSE))
        {
            float fScaleFactor = 1.0;
            char* pcSubstring = strstr(pcParams ,"X\0" );
            char* pcMultiplier = strstr(pcParams, "*\0" );

            if (pcMultiplier == NULL)
            {
                //DO NOTHING
            }
            else if (pcMultiplier > pcSubstring)
            {
                char* pcToken = pcMultiplier+1;
                fScaleFactor = (float) atof(pcToken);
            }

            if (strstr(pcParams, "-\0") != NULL && fScaleFactor > 0)
                fScaleFactor*= -1.0f;

            pkInfo->m_kParamList.Add(1.0f*fScaleFactor);
            if (pkInfo->m_eType == NiSceneCommandInfo::MOUSE)
            {
                NiSceneMouseCommandInfo* pkMouseInfo = 
                    ((NiSceneMouseCommandInfo*) pkInfo);
                pkMouseInfo->m_iXAffectedParameterIndex = uiIndex;
            }
        }
        else if ((strstr( pcParams, "Y\0") != 0) && 
            (pkInfo->m_eType == NiSceneCommandInfo::MOUSE || 
            pkInfo->m_eType == NiSceneCommandInfo::GAMEPAD))
        {
            float fScaleFactor = 1.0;
            char* pcSubstring = strstr(pcParams ,"Y\0" );
            char* pcMultiplier = strstr(pcParams, "*\0" );

            if (pcMultiplier == NULL)
            {
                //DO NOTHING
            }
            else if (pcMultiplier > pcSubstring)
            {
                char* pcToken = pcMultiplier+1;
                fScaleFactor = (float) atof(pcToken);
            }

            if (strstr(pcParams, "-\0") != NULL && fScaleFactor > 0)
                fScaleFactor*= -1.0f;

            pkInfo->m_kParamList.Add(1.0f*fScaleFactor);
            if (pkInfo->m_eType == NiSceneCommandInfo::MOUSE)
            {
                NiSceneMouseCommandInfo* pkMouseInfo = 
                    ((NiSceneMouseCommandInfo*) pkInfo);
                pkMouseInfo->m_iYAffectedParameterIndex = uiIndex;
            }
        }
        else if (strstr( pcParams, "WHEEL\0") != 0 && 
            pkInfo->m_eType == NiSceneCommandInfo::MOUSE)
        {
            float fScaleFactor = 1.0;
            char* pcSubstring = strstr(pcParams ,"WHEEL\0" );
            char* pcMultiplier = strstr(pcParams, "*\0" );

            if (pcMultiplier == NULL)
            {
                //DO NOTHING
            }
            else if (pcMultiplier > pcSubstring)
            {
                char* pcToken = pcMultiplier+1;
                fScaleFactor = (float) atof(pcToken);
            }

            if (strstr(pcParams, "-\0") != NULL && fScaleFactor > 0)
                fScaleFactor*= -1.0f;

            pkInfo->m_kParamList.Add(1.0f*fScaleFactor);
            NiSceneMouseCommandInfo* pkMouseInfo = 
                    ((NiSceneMouseCommandInfo*) pkInfo);
            pkMouseInfo->m_iWheelDeltaAffectedParameterIndex = uiIndex;
        }
        else if (NiStricmp("TRUE", pcParams) == 0)
        {
            pkInfo->m_kParamList.Add(1.0f);
            PrintDebug("\tTRUE\n");
        }
        else if (NiStricmp("FALSE", pcParams) == 0)
        {
            pkInfo->m_kParamList.Add(0.0f);
            PrintDebug("\tFALSE\n");
        }
        else
        {
            fParam = (float)atof(pcParams);
            pkInfo->m_kParamList.Add(fParam);
            /*char* acStr = NiAlloc(char, 256);
            sprintf(acStr, "\t%s = %f\n", pcParams, fParam );
            PrintDebug(acStr);
            NiDelete acStr;*/
        }
        
        uiIndex = pkInfo->m_kParamList.GetSize();
        pcParams = NiStrtok(NULL, acDelimiters, &pcContext);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::GetSensitivity(char* pcSensitivity, 
    NiDevice::DeviceState& eDeviceState)
{
    if (pcSensitivity == NULL)
        return false;

    char* pcContext;
    pcSensitivity = NiStrtok(pcSensitivity, ms_acDelimiters, &pcContext);

    //Parse the string to determine the sensitivity
    eDeviceState = ms_spConstants->GetSensitivity(pcSensitivity);
    if (eDeviceState != NiDevice::DEVICE_ERROR)
        return true;

    PrintError("No match found for the Sensitivity setting \"");
    PrintError(pcSensitivity);
    PrintError("\" in GetSensitivity\n");
    //delete pcSensitivity;
    return false;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::GetBinaryMouseModifier(char* pcModifier, 
    long& lModifiers)
{
    if (pcModifier == NULL)
    {
        PrintError(
            "No modifier string passed in to GetBinaryMouseModifier\n");
        return false;
    }
    char* pcContext;
    pcModifier = NiStrtok(pcModifier, ms_acDelimiters, &pcContext);

    // Parse the string to determine the modifier
    NiSceneMouse::Modifiers lMod = 
        ms_spConstants->GetMouseModifier(pcModifier);
    if (lMod != 0)
    {
        lModifiers |= lMod;
        return true;
    }

    // We may inadvertantly say that the modifier 0 is an error, so we
    // explicitly check for the NO MODIFIERS string
    const char* pcNone = ms_spConstants->GetMouseModifierString(
        NiSceneMouse::MMOD_NONE);

    if (NiStricmp(pcNone, pcModifier) == 0)
        return true;

    PrintError("No match found for the modifier string \"");
    PrintError(pcModifier);
    PrintError("\" in GetBinaryMouseModifier\n");
    return false;
}
//---------------------------------------------------------------------------
void NiConfigurableUIMapLoader::PrintCommandInfo(
    NiSceneCommandInfo* pkCommandInfo, long)
{
    if (!ms_bDebugOutput)
        return;

    PrintDebug("  ***** FINAL COMMAND INFO *****\n");
    char acStr[256];
    NiSprintf(acStr, 256, "\tUI TYPE = %d\n", pkCommandInfo->m_eType);
    PrintDebug(acStr);
    
    NiSprintf(acStr, 256, "\tCOMMAND CLASS ID = %d\n",  
        pkCommandInfo->m_uiClassID);
    PrintDebug(acStr);
    
    NiSprintf(acStr, 256, "\tSENSITIVITY = %d\n",
        pkCommandInfo->m_eDeviceState);
    PrintDebug(acStr);
    
    if (pkCommandInfo->m_eType == NiSceneCommandInfo::KEYBOARD)
    {
        NiSceneKeyboardCommandInfo* pkKeyCommandInfo = 
            (NiSceneKeyboardCommandInfo*) pkCommandInfo;
        NiSprintf(acStr, 256, "\tKEYCODE = %d\n", pkKeyCommandInfo->m_eKey);
        PrintDebug(acStr);
    }
    else if (pkCommandInfo->m_eType == NiSceneCommandInfo::MOUSE)
    {
        NiSceneMouseCommandInfo* pkMouseCommandInfo = 
            (NiSceneMouseCommandInfo*) pkCommandInfo;
        NiSprintf(acStr, 256, "\tDX AFFECTED PARAM = %d\n", 
            pkMouseCommandInfo->m_iDxAffectedParameterIndex);
        PrintDebug(acStr);
        NiSprintf(acStr, 256, "\tDY AFFECTED PARAM = %d\n", 
            pkMouseCommandInfo->m_iDyAffectedParameterIndex);
        PrintDebug(acStr);
    }
    

    int iHi = pkCommandInfo->m_lModifiers >> 16;
    int iLo = pkCommandInfo->m_lModifiers & 0x0000FFFF;
    NiSprintf(acStr, 256, "\tMODIFIERS = %d %d\n", iHi , iLo);
    PrintDebug(acStr);

    PrintDebug("\tPARAMETERS:\n");
    for (unsigned int i = 0; i < pkCommandInfo->m_kParamList.GetSize(); i++)
    {
        NiSprintf(acStr, 256, "\t  PARAM[%d] = %f\n", i, 
            pkCommandInfo->m_kParamList.GetAt(i));
        PrintDebug(acStr);
    }
    char acMsg[256];
    NiSprintf(acMsg, 256, "\tPRIORITY = %d\n", pkCommandInfo->m_iPriority);
    PrintDebug(acMsg);

    PrintDebug("  ************ END ************\n");
}
//---------------------------------------------------------------------------
void NiConfigurableUIMapLoader::ShowDebugOutputStrings(bool b)
{
    ms_bDebugOutput = b;
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMapLoader::IsShowingDebugOutputStrings()
{
    return ms_bDebugOutput;
}
//---------------------------------------------------------------------------
#ifdef NIDEBUG
    void NiConfigurableUIMapLoader::PrintDebug(char* pcDebug)
#else
    void NiConfigurableUIMapLoader::PrintDebug(char*)
#endif
{
    if (!ms_bDebugOutput)
        return;
    NiOutputDebugString(pcDebug);
}
//---------------------------------------------------------------------------
#ifdef NIDEBUG
    void NiConfigurableUIMapLoader::PrintError(char* pcError)
#else
    void NiConfigurableUIMapLoader::PrintError(char*)
#endif
{
    NiOutputDebugString(pcError);
}
//---------------------------------------------------------------------------
