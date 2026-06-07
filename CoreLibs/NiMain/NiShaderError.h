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

#ifndef NISHADERERROR_H
#define NISHADERERROR_H

#include "NiMainLibType.h"
typedef unsigned int NiShaderError;

// Upper 16 bits of error indicate which shader system a given error belongs to
// Lower 16 bits used by each shader system to specify error

enum NiShaderErrorPrefix
{
    // Errors in the global shader system have this prefix
    NISHADERERR_GLOBALPREFIX        = 0x0000,

    // Generic SHADER errors will have this prefix
    NISHADERERR_SHADERPREFIX           = 0x8000,

    // Unused         = 0x4000,

    // DX9 Shader errors will have this prefix
    NISHADERERROR_DX9PREFIX         = 0x2000,

    // Xenon Shader errors will have this prefix
    NISHADERERROR_XENONPREFIX       = 0x1000

    // As new shader systems are added, they can be enumerated here
};

#define NiDeclareShaderError(name, prefix, value) \
    const NiShaderError name = (NiShaderError)(((prefix) << 16) | (value))

#define NiDeclareGenericShaderError(name, value) \
    NiDeclareShaderError(name, NISHADERERR_GLOBALPREFIX, (value))

NiDeclareGenericShaderError(NISHADERERR_OK,             0x0000);
NiDeclareGenericShaderError(NISHADERERR_UNKNOWN,        0x0001);
NiDeclareGenericShaderError(NISHADERERR_SHADERNOTFOUND, 0x0100);

// ConstantMapEntry not found
NiDeclareGenericShaderError(NISHADERERR_ENTRYNOTFOUND,        0x0010);

// ConstantMapEntry key exists in map
NiDeclareGenericShaderError(NISHADERERR_DUPLICATEENTRYKEY,    0x0020);

// ConstantMapEntry failed to add
NiDeclareGenericShaderError(NISHADERERR_ENTRYNOTADDED,        0x0030);

// ConstantMapEntry invalid mapping
NiDeclareGenericShaderError(NISHADERERR_INVALIDMAPPING,       0x0040);

// ConstantMapEntry failed to be set on the device
NiDeclareGenericShaderError(NISHADERERR_SETCONSTANTFAILED,    0x0050);

// ConstantMapEntry failed to find NiDynamicEffect; default values used
NiDeclareGenericShaderError(NISHADERERR_DYNEFFECTNOTFOUND,    0x0060);

// Class that holds settings for NiSHADERErrors.
class NIMAIN_ENTRY NiShaderErrorSettings
{
public:
    inline static bool GetAllowDynEffectNotFound()
    {
        return ms_bAllowDynEffectNotFound;
    }
    inline static void SetAllowDynEffectNotFound(bool bAllowDynEffectNotFound)
    {
        ms_bAllowDynEffectNotFound = bAllowDynEffectNotFound;
    }

private:
    static bool ms_bAllowDynEffectNotFound;
};

#endif  //#ifndef NISHADERERROR_H
