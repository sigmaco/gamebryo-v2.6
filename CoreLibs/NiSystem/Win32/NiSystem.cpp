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
#include "NiSystemPCH.h"

#include "NiSystem.h"
#include "NiVersion.h"
#include "NiFile.h"

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED =
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED = 
    GAMEBRYO_MODULE_VERSION_STRING(NiSystem);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void NiGetenv(size_t* pstDestLength, char* pcDest, size_t stDestSize, 
    const char* pcSrc)
{
#if _MSC_VER >= 1400
    getenv_s(pstDestLength, pcDest, stDestSize, pcSrc);
#else // #if _MSC_VER >= 1400
    
    NIASSERT(pstDestLength != 0 && pcDest != 0 && stDestSize != 0);

    char* pcResult = getenv(pcSrc);
    if (pcResult)
    {
        NiStrcpy(pcDest, stDestSize, pcResult);
        *pstDestLength = strlen(pcDest);
    }
    else
    {
        *pstDestLength = 0;
    }
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
// Timer functions
//---------------------------------------------------------------------------
LARGE_INTEGER g_freq;
LARGE_INTEGER g_initial;

//---------------------------------------------------------------------------
void NiResetBaseTime()
{
    QueryPerformanceFrequency(&g_freq);
    QueryPerformanceCounter(&g_initial);
}
//---------------------------------------------------------------------------
float NiGetCurrentTimeInSec()
{
    static bool bFirst = true;
    
    if (bFirst)
    {
        NiResetBaseTime();
        bFirst = false;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (float)((long double)
        (counter.QuadPart - g_initial.QuadPart) / 
        (long double) g_freq.QuadPart);
}
//---------------------------------------------------------------------------
unsigned int NiGetPerformanceCounter()
{
    LARGE_INTEGER counter;

    QueryPerformanceCounter(&counter);
    return counter.LowPart;
}
//---------------------------------------------------------------------------
int NiStricmp(const char* s1, const char* s2)
{
#if _MSC_VER >= 1400
    return _stricmp(s1, s2);
#else // #if _MSC_VER >= 1400
    return stricmp(s1, s2);
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
int NiStrnicmp(const char* s1, const char* s2, size_t n)
{
#if _MSC_VER >= 1400
    return _strnicmp(s1, s2, n);
#else // #if _MSC_VER >= 1400
    return strnicmp(s1, s2, n);
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
char* NiStrdup(const char* pcString)
{
    if (pcString == NULL)
        return NULL;

    size_t stLen = strlen(pcString);
    char* pcReturn = NiAlloc(char, stLen+1);
    memcpy(pcReturn, pcString, stLen);
    pcReturn[stLen] = '\0';
    return pcReturn;
}
//---------------------------------------------------------------------------
// _rotr operated on unsigned int (4 bytes on a PC).  So the non-Windows
// version should be 32 bits as well.
unsigned int NiRotr(unsigned int x, int n)
{
    return _rotr(x, n);
}
//---------------------------------------------------------------------------
unsigned int NiGetFileSize (const char* pFilename)
{
    // get file statistics
    struct _stat info;
    int result = _stat(pFilename,&info);

    if ( result != 0 )
        // could not get statistics (maybe file does not exist)
        return 0;

    return int(info.st_size);
}
//---------------------------------------------------------------------------
void NiInitTestEnvironment()
{
    DWORD dwMode = SetErrorMode(0);
    SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);
}
//---------------------------------------------------------------------------

