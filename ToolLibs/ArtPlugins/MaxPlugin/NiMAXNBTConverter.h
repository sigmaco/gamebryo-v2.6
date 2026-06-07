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

#ifndef NIMAXNBTCONVERTER_H
#define NIMAXNBTCONVERTER_H

#include "NiMax.h"
#include <NiShaderRequirementDesc.h>

class NiBinaryStream;
class NiGeometry;
class NiTriShape;
class NiTriStrips;
class NiPoint3;
class NiPoint2;

class NiMAXNBTConverter
{
public:
    NiMAXNBTConverter();
    ~NiMAXNBTConverter();

    static bool SetupBinormalTangentData(
        NiMesh* pkMesh,
        unsigned short usUVSet,
        NiShaderRequirementDesc::NBTFlags eNBTMethod);

protected:
    static bool CreateBTStreams(
        NiMesh* pkMesh,
        NiUInt16 usUVSet);

    static bool CreateBasisMatrices_MAX(
        unsigned int uiNumVertices, 
        NiTStridedRandomAccessIterator<NiPoint3>& kVertices,
        NiTStridedRandomAccessIterator<NiPoint3>& kNormals,
        NiTStridedRandomAccessIterator<NiPoint2>& kTextures, 
        NiTStridedRandomAccessIterator<NiPoint3>& kBiNormals,
        NiTStridedRandomAccessIterator<NiPoint3>& kTangents,
        unsigned int uiNumIndices, 
        NiTStridedRandomAccessIterator<unsigned int>& kIndices);
};

#endif  //NIMAXNBTCONVERTER_H
