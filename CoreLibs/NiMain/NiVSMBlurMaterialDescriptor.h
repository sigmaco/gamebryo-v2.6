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

#ifndef NIVSMBLURMATERIALDESCRIPTOR_H
#define NIVSMBLURMATERIALDESCRIPTOR_H

#include "NiMaterialDescriptor.h"
#include "NiBitfield.h"
#include "NiVSMBlurMaterial.h"

class NIMAIN_ENTRY NiVSMBlurMaterialDescriptor : public NiMaterialDescriptor
{

public:
    NiVSMBlurMaterialDescriptor();

    void SetTextureUsage(unsigned int uiWhichTexture, unsigned int uiInputUV,
        NiVSMBlurMaterial::TexGenOutput eTextureOutput);

    void GetTextureUsage(unsigned int uiWhichTexture, unsigned int& uiInputUV,
        NiVSMBlurMaterial::TexGenOutput& eTextureOutput);
    NiBeginDefaultBitfieldEnum()
        
    // First Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(TRANSFORM,       2,
        BITFIELDSTART,    0)
    NiDeclareDefaultIndexedBitfieldEntry(NORMAL,          2,
        TRANSFORM,        0)
    NiDeclareDefaultIndexedBitfieldEntry(SPECULAR,        1,
        NORMAL,           0)
    NiDeclareDefaultIndexedBitfieldEntry(VERTEXCOLORS,    1,
        SPECULAR,         0)
    NiDeclareDefaultIndexedBitfieldEntry(AMBDIFFEMISSIVE, 2,
        VERTEXCOLORS,     0)

    // Second Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(LIGHTINGMODE,    1,
        AMBDIFFEMISSIVE,  0)
    NiDeclareDefaultIndexedBitfieldEntry(BLURDIRECTION,   1,
        LIGHTINGMODE,     0)
    NiDeclareDefaultIndexedBitfieldEntry(FOGTYPE,         2,
        BLURDIRECTION,        0)
    NiDeclareDefaultIndexedBitfieldEntry(INPUTUVCOUNT,    3,
        FOGTYPE,          0)
    NiDeclareDefaultIndexedBitfieldEntry(PARALLAXMAPCOUNT,1,
        INPUTUVCOUNT,     0)

    //Third Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(BASEMAPCOUNT,    1,
        PARALLAXMAPCOUNT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(NORMALMAPCOUNT,  1,
        BASEMAPCOUNT,     0)
    NiDeclareDefaultIndexedBitfieldEntry(NORMALMAPTYPE,   2,
        NORMALMAPCOUNT,   0)
    NiDeclareDefaultIndexedBitfieldEntry(DARKMAPCOUNT,    1,
        NORMALMAPTYPE,    0)
    NiDeclareDefaultIndexedBitfieldEntry(DETAILMAPCOUNT,  1,
        DARKMAPCOUNT,     0)
    NiDeclareDefaultIndexedBitfieldEntry(BUMPMAPCOUNT,    1,
        DETAILMAPCOUNT,   0)
    NiDeclareDefaultIndexedBitfieldEntry(GLOSSMAPCOUNT,   1,
        BUMPMAPCOUNT,     0)

    //Fourth Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(GLOWMAPCOUNT,    1,
        GLOSSMAPCOUNT,    0)
    NiDeclareDefaultIndexedBitfieldEntry(ENVMAPTYPE,      3,
        GLOWMAPCOUNT,     0)
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP00COUNT,1,
        ENVMAPTYPE,       0)
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP01COUNT,1,
        CUSTOMMAP00COUNT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP02COUNT,1,
        CUSTOMMAP01COUNT, 0)
    NiDeclareDefaultLastIndexedBitfieldEntry(CUSTOMMAP03COUNT,1,
        CUSTOMMAP02COUNT,
        0)

    // First Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP04COUNT,  1,
        BITFIELDSTART, 1)
    NiDeclareDefaultIndexedBitfieldEntry(DECALMAPCOUNT,     2,
        CUSTOMMAP04COUNT,1)
    NiDeclareDefaultIndexedBitfieldEntry(PERVERTEXFORLIGHTS,1,
        DECALMAPCOUNT,  1)
    NiDeclareDefaultIndexedBitfieldEntry(VSMBLURKERNEL,   4,
        PERVERTEXFORLIGHTS, 1)

    // Second Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(SPOTLIGHTCOUNT,  4,
        VSMBLURKERNEL, 1)
    NiDeclareDefaultIndexedBitfieldEntry(DIRLIGHTCOUNT,   4,
        SPOTLIGHTCOUNT,  1)

    // Third Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(SHADOWMAPFORLIGHT,8,
        DIRLIGHTCOUNT,  1)

    // Fourth Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(PROJLIGHTMAPCOUNT,   2,
        SHADOWMAPFORLIGHT,  1)
    NiDeclareDefaultIndexedBitfieldEntry(PROJLIGHTMAPTYPES,   3,
        PROJLIGHTMAPCOUNT,  1)
    NiDeclareDefaultLastIndexedBitfieldEntry(PROJLIGHTMAPCLIPPED, 3,
        PROJLIGHTMAPTYPES,  1)

    // First Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPCOUNT,   2,
        BITFIELDSTART,  2)
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPTYPES,   3,
        PROJSHADOWMAPCOUNT,  2)
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPCLIPPED, 3,
        PROJSHADOWMAPTYPES,  2)

    // Second Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(MAP00,    3,
        PROJSHADOWMAPCLIPPED, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP00TEXOUTPUT, 1,    MAP00, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP01,    3, MAP00TEXOUTPUT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP01TEXOUTPUT, 1,    MAP01, 2)
    
    // Third Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(MAP02,    3,  MAP01TEXOUTPUT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP02TEXOUTPUT, 1,    MAP02, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP03,    3, MAP02TEXOUTPUT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP03TEXOUTPUT, 1,    MAP03, 2)

    // Fourth Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(MAP04,    3,  MAP03TEXOUTPUT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP04TEXOUTPUT, 1,    MAP04, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP05,    3, MAP04TEXOUTPUT, 2)
    NiDeclareDefaultLastIndexedBitfieldEntry(MAP05TEXOUTPUT, 1, MAP05, 2)
        
    // First Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(MAP06,    3,  BITFIELDSTART, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP06TEXOUTPUT, 1,    MAP06, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP07,    3, MAP06TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP07TEXOUTPUT, 1,    MAP07, 3)

    // Second Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(MAP08,    3, MAP07TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP08TEXOUTPUT, 1,    MAP08, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP09,    3, MAP08TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP09TEXOUTPUT, 1,    MAP09, 3)
    
    // Third Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(MAP10,    3,  MAP09TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP10TEXOUTPUT, 1,    MAP10, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP11,    3, MAP10TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP11TEXOUTPUT, 1,    MAP11, 3)

    // Fourth Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(USERDEFINED00,    1,
        MAP11TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(USERDEFINED01,    1,
        USERDEFINED00, 3)
    NiDeclareDefaultIndexedBitfieldEntry(USERDEFINED02,    1,
        USERDEFINED01, 3)
    NiDeclareDefaultIndexedBitfieldEntry(USERDEFINED03,    1,
        USERDEFINED02, 3)
    NiDeclareDefaultIndexedBitfieldEntry(SHADOWTECHNIQUE,  2,
        USERDEFINED03, 3)       
    NiDeclareDefaultIndexedBitfieldEntry(ALPHATEST,  1, SHADOWTECHNIQUE, 3)
    // Leave 1 bits for future.

    NiEndDefaultBitfieldEnum();   
};

#endif  //#ifndef NIVSMBLURMATERIALDESCRIPTOR_H
