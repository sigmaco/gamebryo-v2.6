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

#ifndef NITRISHAPE_H
#define NITRISHAPE_H

#include "NiTriBasedGeom.h"
#include "NiTriShapeData.h"

/**
    This class is deprecated.

    It only exists to support loading old NIF files.
*/

class NIMAIN_ENTRY NiTriShape : public NiTriBasedGeom
{
    NiDeclareRTTI;
    NiDeclareClone(NiTriShape);
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***

    // The constructed object is given ownership of the input arrays and
    // has the responsibility for deleting them when finished with them.
    NiTriShape(unsigned short usVertices, NiPoint3* pkVertex,
        NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture, 
        unsigned short usNumTextureSets,
        NiShaderRequirementDesc::NBTFlags eNBTMethod,
        unsigned short usTriangles, unsigned short* pusTriList);

    // The constructed object shares the input data.
    NiTriShape(NiTriShapeData* pkModelData);

    // triangle access
    inline unsigned int GetTriListLength() const;
    inline unsigned short* GetTriList() const;
    virtual void GetModelTriangle(unsigned short usTriangle, NiPoint3*& pkP0,
        NiPoint3*& pkP1, NiPoint3*& pkP2);

    // *** end Emergent internal use only ***
protected:
    // streaming support
    NiTriShape();
};

typedef NiPointer<NiTriShape> NiTriShapePtr;

#include "NiTriShape.inl"

#endif
