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
#include "NiD3DRendererPCH.h"

#include "NiD3DUtility.h"

#include <NiSystem.h>

#include <NiRenderObject.h>
#include <NiTriShape.h>
#include <NiTriShapeData.h>
#include <NiTriStrips.h>
#include <NiTriStripsData.h>

bool NiD3DUtility::ms_bLogEnabled = true;

//---------------------------------------------------------------------------
NiD3DUtility::NiD3DUtility()
{
}
//---------------------------------------------------------------------------
NiD3DUtility::~NiD3DUtility()
{
}
//---------------------------------------------------------------------------
void NiD3DUtility::Log(char* pcFmt, ...)
{
    if (!ms_bLogEnabled)
        return;

    char acMessage[1024];

    va_list args;
    va_start(args, pcFmt);
    NiVsprintf(acMessage, 1024, pcFmt, args);
    va_end(args);

    NiOutputDebugString(acMessage);
}
//---------------------------------------------------------------------------
bool NiD3DUtility::GetLogEnabled()
{
    return ms_bLogEnabled;
}
//---------------------------------------------------------------------------
void NiD3DUtility::SetLogEnabled(bool bEnabled)
{
    ms_bLogEnabled = bEnabled;
}
//---------------------------------------------------------------------------
// THIS FUNCTION ASSUMES THE FOLLOWING:
//  pcDrive can hold _MAX_DRIVE
//  pcDir   can hold _MAX_PATH
//  pcFname can hold _MAX_PATH
//  pcExt   can hold _MAX_EXT
bool NiD3DUtility::GetSplitPath(const char* pcFilename, char* pcDrive, 
    char* pcDir, char* pcFname, char* pcExt)
{
#if _MSC_VER >= 1400
    _splitpath_s(pcFilename, pcDrive, _MAX_DRIVE, pcDir, _MAX_PATH,
        pcFname, _MAX_PATH, pcExt, _MAX_EXT);
#else //#if _MSC_VER >= 1400
    _splitpath(pcFilename, pcDrive, pcDir, pcFname, pcExt);
#endif //#if _MSC_VER >= 1400
    return true;
}
//---------------------------------------------------------------------------
bool NiD3DUtility::GetFilePath(const char* pcFilename, char* pcPath,
    unsigned int uiMaxLen)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_FNAME];
    char acFileExt[_MAX_EXT];

#if _MSC_VER >= 1400
    _splitpath_s(pcFilename, acDrive, _MAX_DRIVE, acDir, _MAX_PATH,
        acFileName, _MAX_FNAME, acFileExt, _MAX_EXT);
#else //#if _MSC_VER >= 1400
    _splitpath(pcFilename, acDrive, acDir, acFileName, acFileExt);
#endif //#if _MSC_VER >= 1400

    if ((strlen(acDrive) + strlen(acDir) + 1) > uiMaxLen)
        return false;

    NiSprintf(pcPath, uiMaxLen, "%s%s", acDrive, acDir);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DUtility::GetFileName(const char* pcFilename, char* pcName,
    unsigned int uiMaxLen)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_FNAME];
    char acFileExt[_MAX_EXT];

#if _MSC_VER >= 1400
    _splitpath_s(pcFilename, acDrive, _MAX_DRIVE, acDir, _MAX_PATH,
        acFileName, _MAX_FNAME, acFileExt, _MAX_EXT);
#else //#if _MSC_VER >= 1400
    _splitpath(pcFilename, acDrive, acDir, acFileName, acFileExt);
#endif //#if _MSC_VER >= 1400

    if ((strlen(acFileName) + 1) > uiMaxLen)
        return false;

    NiSprintf(pcName, uiMaxLen, "%s", acFileName);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DUtility::GetFileExtension(const char* pcFilename, char* pcExt, 
    unsigned int uiMaxLen)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_FNAME];
    char acFileExt[_MAX_EXT];

#if _MSC_VER >= 1400
    _splitpath_s(pcFilename, acDrive, _MAX_DRIVE, acDir, _MAX_PATH,
        acFileName, _MAX_FNAME, acFileExt, _MAX_EXT);
#else //#if _MSC_VER >= 1400
    _splitpath(pcFilename, acDrive, acDir, acFileName, acFileExt);
#endif //#if _MSC_VER >= 1400

    size_t stExtLen = strlen(acFileExt);
    NIASSERT(stExtLen < uiMaxLen);
    NiStrncpy(pcExt, uiMaxLen, acFileExt, stExtLen);

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3DUtility::SetString(char*& pcDest, unsigned int uiDestSize, 
    const char* pcSrc)
{
    if (pcSrc && pcSrc[0] != '\0')
    {
        size_t stSrcSize = strlen(pcSrc) + 1;
        if (pcDest)
        {
            // See if the length is ok
            if (uiDestSize <= stSrcSize)
            {
                NiFree(pcDest);
                pcDest = 0;
            }
        }

        if (!pcDest)
        {
            pcDest = NiAlloc(char, stSrcSize);
            uiDestSize = (unsigned int)stSrcSize;
        }

        NiStrcpy(pcDest, uiDestSize, pcSrc);
        return uiDestSize;
    }
    else
    {
        NiFree(pcDest);
        pcDest = 0;
        return 0;
    }
}
//---------------------------------------------------------------------------
bool NiD3DUtility::WriteData(NiBinaryStream& kStream, void* pvData, 
    unsigned int uiSize, unsigned int uiComponentSize)
{
    NiStreamSaveBinary(kStream, uiSize);

    if (uiSize)
    {
        unsigned int uiWrote =
            kStream.BinaryWrite(pvData, uiSize, &uiComponentSize, 1);
        if (uiWrote != uiSize)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DUtility::ReadData(NiBinaryStream& kStream, void*& pvData,
    unsigned int& uiSize)
{
    unsigned int uiReadSize;
    NiStreamLoadBinary(kStream, uiReadSize);

    if (uiReadSize > uiSize)
    {
        uiSize = uiReadSize;
        return false;
    }

    if (uiSize)
    {
        unsigned int uiRead = kStream.Read(pvData, uiSize);
        if (uiRead != uiSize)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DUtility::AllocateAndReadData(NiBinaryStream& kStream, void*& pvData,
    unsigned int& uiDataSize, unsigned int uiComponentSize)
{
    NiStreamLoadBinary(kStream, uiDataSize);

    if (uiDataSize)
    {
        pvData = NiAlloc(unsigned char, uiDataSize);
        NIASSERT(pvData);

        unsigned int uiRead =
            kStream.BinaryRead(pvData, uiDataSize, &uiComponentSize, 1);

        if (uiRead != uiDataSize)
            return false;
    }
    else
    {
        pvData = NULL;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DUtility::ConvertCHARToWCHAR(WCHAR* pwDestination, 
    const char* pcSource, unsigned int uiDestChar)
{
    if (pwDestination == NULL || pcSource == NULL || uiDestChar == 0)
        return false;

    int iResult = MultiByteToWideChar(CP_ACP, 0, pcSource, -1, pwDestination, 
        uiDestChar);
    pwDestination[uiDestChar - 1] = 0;
    
    return (iResult != 0);
}
//---------------------------------------------------------------------------
bool NiD3DUtility::ConvertWCHARToCHAR(char* pcDestination, 
    const WCHAR* pwSource, unsigned int uiDestChar)
{
    if (pcDestination == NULL || pwSource == NULL || uiDestChar == 0)
        return false;

    int iResult = WideCharToMultiByte(CP_ACP, 0, pwSource, -1, pcDestination, 
        uiDestChar*sizeof(char), NULL, NULL);
    pcDestination[uiDestChar - 1] = 0;
    
    return (iResult != 0);
}
//---------------------------------------------------------------------------
bool NiD3DUtility::ConvertTCHARToCHAR(char* pcDestination, 
    const TCHAR* ptSource, unsigned int uiDestChar)
{
    if (pcDestination == NULL || ptSource == NULL || uiDestChar == 0)
        return false;

#ifdef _UNICODE
    return ConvertWCHARToCHAR(pcDestination, ptSource, uiDestChar);
#else
    NiStrncpy(pcDestination, uiDestChar, ptSource, uiDestChar - 1);
    return true;
#endif   
}
//---------------------------------------------------------------------------
bool NiD3DUtility::ConvertTCHARToWCHAR(WCHAR* pwDestination, 
    const TCHAR* ptSource, unsigned int uiDestChar)
{
    if ( pwDestination == NULL || ptSource == NULL || uiDestChar == 0)
        return false;

#ifdef _UNICODE
    wcsncpy(pwDestination, ptSource, uiDestChar);
    pwDestination[uiDestChar - 1] = '\0';
    return true;
#else
    return ConvertCHARToWCHAR(pwDestination, ptSource, uiDestChar);
#endif    
}
//---------------------------------------------------------------------------
bool NiD3DUtility::ConvertCHARToTCHAR(TCHAR* ptDestination, 
    const char* pcSource, unsigned int uiDestChar)
{
    if ( ptDestination == NULL || pcSource==NULL || uiDestChar == 0)
        return false;
        
#ifdef _UNICODE
    return ConvertCHARToWCHAR(ptDestination, pcSource, uiDestChar);
#else
    NiStrncpy(ptDestination, uiDestChar, pcSource, uiDestChar - 1);
    ptDestination[uiDestChar - 1] = L'\0';
    return true;
#endif    
}
//---------------------------------------------------------------------------
bool NiD3DUtility::ConvertWCHARToTCHAR(TCHAR* ptDestination, 
    const WCHAR* pwSource, unsigned int uiDestChar)
{
    if (ptDestination == NULL || pwSource == NULL || uiDestChar == 0)
        return false;

#ifdef _UNICODE
    wcsncpy(ptDestination, pwSource, uiDestChar);
    ptDestination[uiDestChar - 1] = L'\0';    
    return S_OK;
#else
    return ConvertWCHARToCHAR(ptDestination, pwSource, uiDestChar);
#endif
}
//---------------------------------------------------------------------------
