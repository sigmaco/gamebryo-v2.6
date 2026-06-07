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

#ifndef NITRISHAPEDYNAMICDATA_H
#define NITRISHAPEDYNAMICDATA_H

#include "NiTriShapeData.h"

/**
    This class is deprecated.

    It only exists to support loading old NIF files.
*/

class NIMAIN_ENTRY NiTriShapeDynamicData : public NiTriShapeData
{
public:
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***

    // The constructed object is given ownership of the input arrays and
    // has the responsibility for deleting them when finished with them.
    NiTriShapeDynamicData(unsigned short usVertices, NiPoint3* pkVertex,
        NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture, 
        unsigned short usNumTextureSets,
        NiShaderRequirementDesc::NBTFlags eNBTMethod,
        unsigned short usTriangles, unsigned short* pusTriList,
        unsigned short usActiveVertices = 0,
        unsigned short usActiveTriangles = 0);

    NiTriShapeDynamicData();

    // For resetting the valid size of the model data arrays.
    // Useful for applications that manipulate the arrays after 
    // creating the model data
    virtual void SetActiveVertexCount(unsigned short usActiveVertices);
    virtual void SetActiveTriangleCount(unsigned short usActiveTriangles);

    // *** end Emergent internal use only ***
protected:
    virtual void SetVertexCount(unsigned short usVertices);
    virtual void SetTriangleCount(unsigned short usTriangles);
};

typedef NiPointer<NiTriShapeDynamicData> NiTriShapeDynamicDataPtr;

#include "NiTriShapeDynamicData.inl"

#endif // NITRISHAPEDYNAMICDATA_H
