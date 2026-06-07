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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiBinaryShaderLibPCH.h"

#include "NSBUtility.h"
#include "NSBStateGroup.h"
#include "NSBConstantMap.h"

#include <NiSystem.h>

//---------------------------------------------------------------------------
#if defined(NIDEBUG)
unsigned int NSBUtility::ms_uiIndent = 0;
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
bool NSBUtility::SaveBinaryStateGroup(NiBinaryStream& kStream,
    NSBStateGroup* pkStateGroup)
{
    unsigned int uiValue;

    if (pkStateGroup)
    {
        uiValue = 1;
        NiStreamSaveBinary(kStream, uiValue);
        if (!pkStateGroup->SaveBinary(kStream))
            return false;
    }
    else
    {
        uiValue = 0;
        NiStreamSaveBinary(kStream, uiValue);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBUtility::LoadBinaryStateGroup(NiBinaryStream& kStream, 
    NSBStateGroup*& pkStateGroup)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);

    if (uiValue == 0)
    {
        // No render state group was written
        return true;
    }

    pkStateGroup = NiNew NSBStateGroup();
    NIASSERT(pkStateGroup);

    if (!pkStateGroup->LoadBinary(kStream))
    {
        NiDelete pkStateGroup;
        pkStateGroup = 0;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBUtility::SaveBinaryConstantMap(NiBinaryStream& kStream,
    NSBConstantMap* pkMap)
{
    unsigned int uiValue;

    if (pkMap)
    {
        uiValue = 1;
        NiStreamSaveBinary(kStream, uiValue);

        if (!pkMap->SaveBinary(kStream))
            return false;
    }
    else
    {
        uiValue = 0;
        NiStreamSaveBinary(kStream, uiValue);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBUtility::LoadBinaryConstantMap(NiBinaryStream& kStream,
    NSBConstantMap*& pkMap)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);

    if (uiValue == 0)
    {
        // No constant map stored
        pkMap = 0;
        return true;
    }

    pkMap = NiNew NSBConstantMap();
    NIASSERT(pkMap);

    if (!pkMap->LoadBinary(kStream))
    {
        NiDelete pkMap;
        pkMap = 0;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBUtility::IndentInsert()
{
    ms_uiIndent += 4;
}
//---------------------------------------------------------------------------
void NSBUtility::IndentRemove()
{
    ms_uiIndent -= 4;
}
//---------------------------------------------------------------------------
void NSBUtility::Dump(FILE* pf, bool bIndent, const char* pcFmt, ...)
{
    char acTemp[1024];
    char acMessage[1024];

    if (bIndent && (ms_uiIndent != 0))
    {
        acMessage[0] = ' ';
        acMessage[1] = 0;
        for (unsigned int ui = 1; ui < ms_uiIndent; ui++)
            NiStrcat(acMessage, 1024, " ");
    }
    else
    {
        acMessage[0] = 0;
    }

    va_list args;
    va_start(args, pcFmt);
    NiVsprintf(acTemp, 1024, pcFmt, args);
    va_end(args);

    NiStrcat(acMessage, 1024, acTemp);
    if (pf)
        fputs(acMessage, pf);
    else
        NiOutputDebugString(acMessage);
}
//---------------------------------------------------------------------------

#endif  //#if defined(NIDEBUG)
