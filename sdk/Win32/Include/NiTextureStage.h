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

#ifndef NITEXTURESTAGE_H
#define NITEXTURESTAGE_H

#include "NiRefObject.h"
#include "NiSmartPointer.h"

NiSmartPointer(NiTextureStage);

// The texture stage class represents a single texture sampler or stage
// within a texture pass.

class NIMAIN_ENTRY NiTextureStage : public NiRefObject
{
public:

    enum TSTextureFlags
    {
        TSTF_IGNORE         = 0x00000000,
        // NiTexturingProperty Map to use
        TSTF_NONE           = 0x00100000,
        TSTF_NDL_BASE       = 0x00200000,
        TSTF_NDL_DARK       = 0x00300000,
        TSTF_NDL_DETAIL     = 0x00400000,
        TSTF_NDL_GLOSS      = 0x00500000,
        TSTF_NDL_GLOW       = 0x00600000,
        TSTF_NDL_BUMP       = 0x00700000,
        TSTF_NDL_NORMAL     = 0x00800000,
        TSTF_NDL_PARALLAX   = 0x00900000,
        TSTF_NDL_TYPEMASK   = 0x00F00000,
        // Flags for map to use
        TSTF_MAP_DECAL      = 0x01000000,
        TSTF_MAP_SHADER     = 0x02000000,
        TSTF_MAP_MASK       = 0x0F000000,
        // Flag to indicate usage
        TSTF_MAP_USE_INDEX  = 0x10000000,
        TSTF_MAP_USE_MASK   = 0xF0000000,
        // Index for decals and shader maps
        TSTF_INDEX_MASK     = 0x0000FFFF,
    };

    enum TSTextureTransformFlags
    {
        TSTTF_IGNORE                = 0x00000000,
        TSTTF_GLOBAL                = 0x10000000,
        TSTTF_CONSTANT              = 0x20000000,
        TSTTF_SOURCE_MASK           = 0xf0000000,
        TSTTF_NI_NO_CALC            = 0x00000000,
        TSTTF_NI_WORLD_PARALLEL     = 0x00000001,
        TSTTF_NI_WORLD_PERSPECTIVE  = 0x00000002,
        TSTTF_NI_WORLD_SPHERE_MAP   = 0x00000003,
        TSTTF_NI_CAMERA_SPHERE_MAP  = 0x00000004,
        TSTTF_NI_SPECULAR_CUBE_MAP  = 0x00000005,
        TSTTF_NI_DIFFUSE_CUBE_MAP   = 0x00000006
    };

    enum TSObjTextureFlags
    {
        TSOTF_IGNORE        = 0x0000,
        TSOTF_INDEX_MASK    = 0x00FF,
        TSOTF_TYPE_MASK     = 0xFF00,
        TSOTF_TYPE_SHIFT    = 8
    };

    NiTextureStage();
    virtual ~NiTextureStage();
};

#include "NiTextureStage.inl"

#endif // NITEXTURESTAGE_H
