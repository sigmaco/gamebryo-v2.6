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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

//---------------------------------------------------------------------------
void NiPhysXUserOutput::reportError(NxErrorCode code, const char *message,
    const char *file, int line)
{
#ifdef _PS3
    const char* pcErrorText = "";
#else
    char* pcErrorText = "";
#endif

    switch (code)
    {
        case NXE_NO_ERROR:
            pcErrorText = "No Error";
            break;
        case NXE_INVALID_PARAMETER:
            pcErrorText = "Invalid Parameter";
            break;
        case NXE_INVALID_OPERATION:
            pcErrorText = "Invalid Operation";
            break;
        case NXE_OUT_OF_MEMORY:
            pcErrorText = "Out of Memory";
            break;
        case NXE_INTERNAL_ERROR:
            pcErrorText = "Internal Error";
            break;
        case NXE_ASSERTION:
            pcErrorText = "Assertion";
            break;
        case NXE_DB_INFO:
            pcErrorText = "Debug Info";
            break;
        case NXE_DB_WARNING:
            pcErrorText = "Debug Warning";
            break;
        case NXE_DB_PRINT:
            pcErrorText = "Debug Print";
            break;
    }
    size_t stLength = strlen(pcErrorText) + strlen(message)
        + strlen(file) + 40;
    char* pcString = (char*)NiMalloc(stLength);
    
    NiSprintf(pcString, stLength, "PhysX Error: %s: %s in %s line %d\n",
        pcErrorText, message, file, line);

    NiOutputDebugString(pcString);
            
    NiFree(pcString);
}
//--------------------------------------------------------------------------- 
NxAssertResponse NiPhysXUserOutput::reportAssertViolation (const char *message,
    const char *file, int line)
{
    size_t stLength = strlen(message) + strlen(file) + 40;
    char* pcString = (char*)NiMalloc(stLength);
    
    NiSprintf(pcString, stLength, "PhysX Assert Violation: %s in %s line %d\n",
        message, file, line);
    
    NiOutputDebugString(pcString);
    
    NiFree(pcString);
    
    NIASSERT(false);

    return NX_AR_CONTINUE;
}
//--------------------------------------------------------------------------- 
#ifdef NIDEBUG
void NiPhysXUserOutput::print(const char *message)
#else
void NiPhysXUserOutput::print(const char*)
#endif
{
    NiOutputDebugString(message);
}   
//--------------------------------------------------------------------------- 
