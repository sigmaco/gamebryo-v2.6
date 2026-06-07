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

#include "NiDebug.h"
#include "NiSystem.h"

NiAssertFailProcFuncPtr NiAssertFail::ms_pfnNiAssertFailProc = 
    NiAssertFail::DefaultAssertFail;

bool NiAssertFail::DefaultAssertFail(
    const char* pcExpression,
    const char* pcFile,
    const char* pcFunction,
    const int iLine)
{
    char* pcCaption = "Assert Failed";
    char acString[512];
    NiSprintf(
        acString, 
        sizeof(acString), 
        "\n"
        "Assert Failed: %s\n\n"
        "File: %s\n"
        "Line: %d\n"
        "Function: %s\n",
        pcExpression,
        pcFile,
        iLine,
        pcFunction);

    fprintf(stderr, acString);

    int iButtonResult = MessageBox(
        NULL, 
        acString, 
        pcCaption, 
        MB_ABORTRETRYIGNORE);

    switch(iButtonResult)
    {
    case IDABORT:
        exit(EXIT_FAILURE);
    default:
    case IDRETRY:
        NIDEBUGBREAK();
        return false;

    case IDIGNORE:
        return false;
    }
}
