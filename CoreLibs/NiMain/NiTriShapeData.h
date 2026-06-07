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

#ifndef NITRISHAPEDATA_H
#define NITRISHAPEDATA_H

#include "NiTriBasedGeomData.h"

/**
    This class is deprecated.

    It only exists to support loading old NIF files.
*/

class NIMAIN_ENTRY NiTriShapeData : public NiTriBasedGeomData
{
public:
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***

    // The constructed object is given ownership of the input arrays and
    // has the responsibility for deleting them when finished with them.
    NiTriShapeData(unsigned short usVertices, NiPoint3* pkVertex,
        NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture,
        unsigned short usNumTextureSets,
        NiShaderRequirementDesc::NBTFlags eNBTMethod,
        unsigned short usTriangles, unsigned short* pusTriList);

    NiTriShapeData();

    virtual ~NiTriShapeData();

    // triangles
    inline unsigned int GetTriListLength() const;
    inline unsigned short* GetTriList();
    inline const unsigned short* GetTriList() const;
    virtual void GetTriangleIndices(unsigned short i, unsigned short& i0,
        unsigned short& i1, unsigned short& i2) const;

    // inline helper for getting the triangle indices quickly with no virtual
    // call.
    inline void GetTriangleIndices_Inline(unsigned short i, unsigned short& i0,
        unsigned short& i1, unsigned short& i2) const;

    // reallocation support
    void Replace(unsigned short usTriangles, unsigned short* pusTriList);

    // normal sharing support
    void LetVerticesShareNormal(unsigned short usIndex0, 
        unsigned short usIndex1);

    // For temporarily using data only.  This call is like Replace(), 
    // but does not delete any pointers.  Be careful to call this only 
    // on objects created by the default (empty) constructor to prevent 
    // memory leaks.  These pointers will be owned (and deleted upon 
    // destruction) by this object so use this call carefully.
    void SetData(unsigned short usTriangles, unsigned short* pusTriList);

    // *** end Emergent internal use only ***

protected:
    unsigned int m_uiTriListLength;
    unsigned short* m_pusTriList;

    class NIMAIN_ENTRY SharedNormalArray : public NiMemObject
    {
    public:
        SharedNormalArray();

        unsigned short GetCount();
        unsigned short* GetArray();
        void SetArray(unsigned short usCount, unsigned short* pusArray);

    private:
        unsigned short m_usNumSharedNormals;

        // This pointer will not own the memory it points to, so it must
        // not be deleted in this class.
        unsigned short* m_pusSharedNormalIndexArray;
    };

    SharedNormalArray* m_pkSharedNormals;
    unsigned short m_usSharedNormalsArraySize;

    // Sub-class to handle memory management for SharedNormalArrays
    class NIMAIN_ENTRY SNAMemBlock : public NiMemObject
    {
    public:
        SNAMemBlock();
        SNAMemBlock(unsigned int uiSize);
        ~SNAMemBlock();

        unsigned short* m_pusBlock;
        unsigned short* m_pusFreeBlock;
        unsigned int m_uiBlockSize;
        unsigned int m_uiFreeBlockSize;

        SNAMemBlock* m_pkNext;
    };

    SNAMemBlock* m_pkSNAMemoryBlocks;
};

NiSmartPointer(NiTriShapeData);

#include "NiTriShapeData.inl"

#endif
