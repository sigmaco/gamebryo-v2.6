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

#ifndef NITERRAINVERTEXDESCRIPTOR_H
#define NITERRAINVERTEXDESCRIPTOR_H

#include <NiGPUProgramDescriptor.h>
#include <NiBitfield.h>

#include "NiTerrainLibType.h"

/**
    This class manages the bit fields that are constructed from the terrain 
    material descriptor.
    
    It is used when determining how to construct a shade tree in order to
    generate a vertex shader.
*/
class NITERRAIN_ENTRY NiTerrainMaterialVertexDescriptor :
    public NiGPUProgramDescriptor
{
public:

    /// Convert the bit field to a human readable string.
    ///
    /// This is useful when debugging the values in the bit field.
    NiString ToString();

    /// @cond EMERGENT_INTERNAL
    NiBeginDefaultBitfieldEnum()

    // First byte, index 0    
    NiDeclareDefaultIndexedBitfieldEntry(OUTPUTWORLDNBT, 1, BITFIELDSTART, 0)
    NiDeclareDefaultIndexedBitfieldEntry(TANGENTS, 1, OUTPUTWORLDNBT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(FOGTYPE, 2, TANGENTS, 0)    
    NiDeclareDefaultIndexedBitfieldEntry(NUM_TEXCOORDS, 4, FOGTYPE, 0);

    NiDeclareDefaultIndexedBitfieldEntry(OUTPUT_WORLDVIEW, 1,
        BITFIELDSTART, 1);
    NiDeclareDefaultIndexedBitfieldEntry(MORPH_MODE, 2,
        OUTPUT_WORLDVIEW, 1);

    NiEndDefaultBitfieldEnum()
    /// @endcond
};

#endif
