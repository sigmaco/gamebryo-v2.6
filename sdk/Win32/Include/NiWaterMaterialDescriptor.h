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

#ifndef NIWATERMATERIALDESCRIPTOR_H
#define NIWATERMATERIALDESCRIPTOR_H

#include <NiMaterialDescriptor.h>
#include <NiBitfield.h>

#include "NiTerrainLibType.h"

/**
     This class encapuslates a bit field used to describe a NiWaterMaterial 
     shader. The NiWaterMaterial generates shaders based on the defined water
     properties and dynamic effects that affect the light. The 
     NiWaterMaterialDescriptor provides an easy way of describing all the 
     information that is needed to generate a shader.
 */
class NITERRAIN_ENTRY NiWaterMaterialDescriptor : public NiMaterialDescriptor
{
public:

    /// Default constructor.
    NiWaterMaterialDescriptor();

    /// @cond EMERGENT_INTERNAL
    //{@
    NiBeginDefaultBitfieldEnum()

    // first byte index 0;
    NiDeclareDefaultIndexedBitfieldEntry(TANGENTS, 1, BITFIELDSTART, 0)
    NiDeclareDefaultIndexedBitfieldEntry(ENABLE_SUNLIGHT, 1, TANGENTS, 0)
    NiDeclareDefaultIndexedBitfieldEntry(USE_PLANAR_REFLECTION, 1, 
        ENABLE_SUNLIGHT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(USE_CUBE_MAP_REFLECTION, 1, 
        USE_PLANAR_REFLECTION, 0);
    NiDeclareDefaultIndexedBitfieldEntry(FOGTYPE, 2, USE_CUBE_MAP_REFLECTION, 
        0)

    // first byte index 1;
    NiDeclareDefaultIndexedBitfieldEntry(NUM_TEXCOORDS, 4, BITFIELDSTART, 1)
    NiDeclareDefaultIndexedBitfieldEntry(REFRACTION_MAP, 1, NUM_TEXCOORDS, 1)
    NiDeclareDefaultIndexedBitfieldEntry(REFLECTION_MAP, 1, REFRACTION_MAP, 1)
    NiDeclareDefaultIndexedBitfieldEntry(NORMAL_MAP, 1, REFLECTION_MAP, 1);
    NiDeclareDefaultIndexedBitfieldEntry(BASE_MAP, 1, NORMAL_MAP, 1);

    NiEndDefaultBitfieldEnum();
    //@}
    /// @endcond
};

#endif
