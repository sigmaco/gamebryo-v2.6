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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiViewerStrings.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcRttiName)
{
    size_t stLen = strlen(pcRttiName) + 9;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "--- %s ---", pcRttiName);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, bool bValue)
{
    size_t stLen = strlen(pcPrefix) + 9;
    char* pcString = NiAlloc(char, stLen);
    if (bValue)
        NiSprintf(pcString, stLen, "%s = true", pcPrefix);
    else
        NiSprintf(pcString, stLen, "%s = false", pcPrefix);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, char cValue)
{
    size_t stLen = strlen(pcPrefix) + 5;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %c", pcPrefix, cValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, unsigned char ucValue)
{
    size_t stLen = strlen(pcPrefix) + 7;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %u", pcPrefix, (unsigned int)ucValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, short sValue)
{
    size_t stLen = strlen(pcPrefix) + 10;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %hd", pcPrefix, sValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, unsigned short usValue)
{
    size_t stLen = strlen(pcPrefix) + 9;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %hu", pcPrefix, usValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, int iValue)
{
    size_t stLen = strlen(pcPrefix) + 15;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %d", pcPrefix, iValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, unsigned int uiValue)
{
    size_t stLen = strlen(pcPrefix) + 14;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %u", pcPrefix, uiValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, long lValue)
{
    size_t stLen = strlen(pcPrefix) + 15;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %ld", pcPrefix, lValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, unsigned long ulValue)
{
    size_t stLen = strlen(pcPrefix) + 14;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %lu", pcPrefix, ulValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, float fValue)
{
    size_t stLen = strlen(pcPrefix) + 20;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %g", pcPrefix, fValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, double dValue)
{
    size_t stLen = strlen(pcPrefix) + 20;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %g", pcPrefix, dValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, void* pvValue)
{
    size_t stLen = strlen(pcPrefix) + 16;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = %p", pcPrefix, pvValue);
    return pcString;
}
//---------------------------------------------------------------------------
char* NiGetViewerString(const char* pcPrefix, const char* pcValue)
{
    char* pcString;

    if (pcValue)
    {
        size_t stLen = strlen(pcPrefix) + strlen(pcValue) + 4;
        pcString = NiAlloc(char, stLen);
        NiSprintf(pcString, stLen, "%s = %s", pcPrefix, pcValue);
    }
    else
    {
        size_t stLen = strlen(pcPrefix) + 8;
        pcString = NiAlloc(char, stLen);
        NiSprintf(pcString, stLen, "%s = NULL", pcPrefix);
    }

    return pcString;
}
//---------------------------------------------------------------------------
