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

#ifndef NITRIBASEDGEOMDATA_H
#define NITRIBASEDGEOMDATA_H

#include "NiGeometryData.h"
#include "NiPlane.h"

/**
    This class is deprecated.

    It only exists to support loading old NIF files.
*/

class NIMAIN_ENTRY NiTriBasedGeomData : public NiGeometryData
{
    NiDeclareRTTI;
    NiDeclareAbstractStream;

public:
    // *** begin Emergent internal use only ***
    virtual ~NiTriBasedGeomData();

    // triangles
    inline unsigned short GetTriangleCount() const;

    virtual void SetActiveTriangleCount(unsigned short usActive);
    inline unsigned short GetActiveTriangleCount() const;
    virtual void GetTriangleIndices(unsigned short i, unsigned short& i0,
        unsigned short& i1, unsigned short& i2) const;

    // *** end Emergent internal use only ***
protected:
    // The constructed object is given ownership of the input arrays and
    // has the responsibility for deleting them when finished with them.
    NiTriBasedGeomData(unsigned short usVertices, NiPoint3* pkVertex,
        NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture, 
        unsigned short usNumTextureSets,
        NiShaderRequirementDesc::NBTFlags eNBTMethod,
        unsigned short usTriangles);

    NiTriBasedGeomData();

    virtual void SetTriangleCount(unsigned short usTriangles);

    unsigned short m_usTriangles;
    unsigned short m_usActiveTriangles;
};

typedef NiPointer<NiTriBasedGeomData> NiTriBasedGeomDataPtr;

#include "NiTriBasedGeomData.inl"

#endif
