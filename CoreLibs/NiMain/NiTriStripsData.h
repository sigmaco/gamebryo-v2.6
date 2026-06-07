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

#ifndef NITRISTRIPSDATA_H
#define NITRISTRIPSDATA_H

#include "NiTriBasedGeomData.h"

/**
    This class is deprecated.

    It only exists to support loading old NIF files.
*/

class NIMAIN_ENTRY NiTriStripsData : public NiTriBasedGeomData
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***

    // The constructed object is given ownership of the input arrays and
    // has the responsibility for deleting them when finished with them.
    NiTriStripsData(unsigned short usVertices, NiPoint3* pkVertex,
        NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture, 
        unsigned short usNumTextureSets,
        NiShaderRequirementDesc::NBTFlags eNBTMethod,
        unsigned short usTriangles, unsigned short usStrips, 
        unsigned short* pusStripLengths, unsigned short* pusStripLists);

    NiTriStripsData();
    virtual ~NiTriStripsData();

    // triangles
    virtual void GetTriangleIndices(unsigned short i, unsigned short& i0,
        unsigned short& i1, unsigned short& i2) const;

    // inline helper for getting the triangle indices quickly with no virtual
    // call.
    inline void GetTriangleIndices_Inline(unsigned short i, unsigned short& i0,
        unsigned short& i1, unsigned short& i2) const;

    inline unsigned short GetStripCount() const;
    inline unsigned short* GetStripLengths() const;
    inline unsigned short* GetStripLists() const;
    inline unsigned short GetStripLengthSum() const;

    virtual unsigned int GetBlockAllocationSize() const;
    // *** end Emergent internal use only ***

protected:
    unsigned short m_usStrips;
    unsigned short* m_pusStripLengths;
    unsigned short* m_pusStripLists;
};

typedef NiPointer<NiTriStripsData> NiTriStripsDataPtr;

#include "NiTriStripsData.inl"

#endif // NITRISTRIPSDATA_H
