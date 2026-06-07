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

#ifndef NISTANDARDVERTEXPROGRAMDESCRIPTOR_H
#define NISTANDARDVERTEXPROGRAMDESCRIPTOR_H

#include "NiGPUProgramDescriptor.h"
#include "NiBitfield.h"
#include "NiStandardMaterial.h"

class NIMAIN_ENTRY NiStandardVertexProgramDescriptor : 
    public NiGPUProgramDescriptor
{
public:
    NiStandardVertexProgramDescriptor();

    void SetOutputUVUsage(unsigned int uiWhichOutputUV, unsigned int uiInputUV,
        NiStandardMaterial::TexGenOutput eOutputUV);

    void GetOutputUVUsage(unsigned int uiWhichOutputUV, 
        unsigned int& uiInputUV, NiStandardMaterial::TexGenOutput& eOutputUV);

    NiString ToString();

    NiBeginDefaultBitfieldEnum()
        
    // First Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(TRANSFORM,    2, BITFIELDSTART, 0)
    NiDeclareDefaultIndexedBitfieldEntry(OUTPUTWORLDPOS, 1, TRANSFORM, 0)
    NiDeclareDefaultIndexedBitfieldEntry(OUTPUTWORLDNBT, 1, OUTPUTWORLDPOS, 0)
    NiDeclareDefaultIndexedBitfieldEntry(OUTPUTWORLDVIEW, 1, OUTPUTWORLDNBT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(OUTPUTTANGENTVIEW,1, OUTPUTWORLDVIEW,
        0)
    NiDeclareDefaultIndexedBitfieldEntry(NORMAL,   2, OUTPUTTANGENTVIEW, 0)

    // Second Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(SPECULAR, 1, NORMAL, 0)
    NiDeclareDefaultIndexedBitfieldEntry(FOGTYPE,  2, SPECULAR, 0)
    NiDeclareDefaultIndexedBitfieldEntry(ENVMAPTYPE,   3, FOGTYPE, 0)
    NiDeclareDefaultIndexedBitfieldEntry(PROJLIGHTMAPCOUNT, 2, ENVMAPTYPE, 0)

    // Third Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(PROJLIGHTMAPTYPES,3,
        PROJLIGHTMAPCOUNT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPCOUNT,2,
        PROJLIGHTMAPTYPES, 0)
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPTYPES,3,
        PROJSHADOWMAPCOUNT, 0)

    // Fourth Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(OUTPUTUVCOUNT,4,PROJSHADOWMAPTYPES, 0)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET00,3, OUTPUTUVCOUNT, 0)
    NiDeclareDefaultLastIndexedBitfieldEntry(UVSET00TEXOUTPUT,1, UVSET00, 0)

    // First Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(UVSET01,  3, BITFIELDSTART, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET01TEXOUTPUT, 1, UVSET01, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET02,  3, UVSET01TEXOUTPUT, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET02TEXOUTPUT, 1, UVSET02, 1)

    // Second Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(UVSET03,  3, UVSET02TEXOUTPUT, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET03TEXOUTPUT, 1, UVSET03, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET04,  3, UVSET03TEXOUTPUT, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET04TEXOUTPUT, 1, UVSET04, 1)

    // Third Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(UVSET05,  3, UVSET04TEXOUTPUT, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET05TEXOUTPUT, 1, UVSET05, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET06,  3, UVSET05TEXOUTPUT, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET06TEXOUTPUT, 1, UVSET06, 1)

    // Fourth Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(UVSET07,  3, UVSET06TEXOUTPUT, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET07TEXOUTPUT, 1, UVSET07, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET08,  3, UVSET07TEXOUTPUT, 1)
    NiDeclareDefaultLastIndexedBitfieldEntry(UVSET08TEXOUTPUT,1, UVSET08, 1)

    // First Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(UVSET09,  3, BITFIELDSTART, 2)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET09TEXOUTPUT, 1, UVSET09, 2)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET10,  3, UVSET09TEXOUTPUT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET10TEXOUTPUT, 1, UVSET10, 2)

    // Second Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(UVSET11,  3, UVSET10TEXOUTPUT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(UVSET11TEXOUTPUT, 1, UVSET11, 2)
    NiDeclareDefaultIndexedBitfieldEntry(POINTLIGHTCOUNT,  4, 
        UVSET11TEXOUTPUT, 2)

    // Third Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(SPOTLIGHTCOUNT, 4, POINTLIGHTCOUNT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(DIRLIGHTCOUNT,  4, SPOTLIGHTCOUNT, 2)

    // Fourth Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(VERTEXCOLORS,  1, DIRLIGHTCOUNT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(VERTEXLIGHTSONLY, 1, VERTEXCOLORS, 2)
    NiDeclareDefaultIndexedBitfieldEntry(AMBDIFFEMISSIVE, 2, VERTEXLIGHTSONLY,
        2)
    NiDeclareDefaultIndexedBitfieldEntry(LIGHTINGMODE, 2, AMBDIFFEMISSIVE, 2)
    NiDeclareDefaultIndexedBitfieldEntry(APPLYMODE, 1, LIGHTINGMODE, 2)

    NiEndDefaultBitfieldEnum();       
};
#endif  //#ifndef NISTANDARDVERTEXPROGRAMDESCRIPTOR_H
