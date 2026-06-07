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

#ifndef NIPLUGINTOOLKITDEFINITIONS_H
#define NIPLUGINTOOLKITDEFINITIONS_H

#include "NiPluginToolkitLibType.h"
#include <NiSystem.h>
#include <NiRTTI.h>

#define MAX_STRING_SIZE 5000
#define NIPT_INVALID_INDEX ((unsigned int) -1)

typedef HWND NiWindowRef;
typedef HINSTANCE  NiInstanceRef;
typedef HMODULE NiModuleRef;

enum ReturnCode
{
    UNKNOWN_ERROR = -1,
    SUCCESS = 1,
    FILE_NOT_FOUND,
    DIRECTORY_NOT_FOUND,
    FILE_PARSE_ERROR,
    NO_READER_FOR_FILE,
    NO_WRITER_FOR_FILE,
    IO_FAILURE,
    SCRIPT_OF_SAME_NAME_ALREADY_EXISTS,
    NULL_INPUT_SCRIPT,
    SCRIPT_IGNORED,
    CANCELLED,
    NO_SCRIPTS_FOUND
};

#define NIPLUGINTOOLKIT_MODULE_NAME "NiPluginToolkit" NI_DLL_SUFFIX

extern HMODULE gs_hNiPluginToolkitHandle;

inline bool NiIsKindOfStringMatch(const char* pcClassName, 
    const NiRTTI* pkRTTI)
{
    if (pcClassName == NULL)
        return false;

    if (pkRTTI != NULL)
    {
        if (NiStricmp(pkRTTI->GetName(), pcClassName) == 0)
            return true;
        return NiIsKindOfStringMatch(pcClassName, pkRTTI->GetBaseRTTI());

    }
    return false;
};

inline NiSystemDesc::RendererID
    ConvertNiPluginToolkitToNiSystemDescRenderer(NiInt32 iRenderer)
{
    switch (iRenderer)
    {
    case 5:
        return NiSystemDesc::RENDERER_DX9;
    case 6:
        return NiSystemDesc::RENDERER_XENON;
    case 7:
        return NiSystemDesc::RENDERER_PS3;
    case 9:
        return NiSystemDesc::RENDERER_D3D10;
    default:
        return NiSystemDesc::RENDERER_GENERIC;
    }
}

#endif
