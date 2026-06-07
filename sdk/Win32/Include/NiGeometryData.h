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

#ifndef NIGEOMETRYDATA_H
#define NIGEOMETRYDATA_H

#include "NiBound.h"
#include "NiColor.h"
#include "NiObject.h"
#include "NiPoint3.h"
#include "NiPoint2.h"
#include "NiObjectGroup.h"
#include "NiShaderDeclaration.h"
#include "NiShaderRequirementDesc.h"

class NiAdditionalGeometryData;
NiSmartPointer(NiAdditionalGeometryData);

/**
    This class is deprecated.

    It only exists to support loading old NIF files.
*/

class NIMAIN_ENTRY NiGeometryData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareAbstractStream;

public:
    // *** begin Emergent internal use only ***

    typedef NiShaderRequirementDesc::NBTFlags DataFlags;

    virtual ~NiGeometryData();

    // vertex access
    inline void CreateVertices(bool bInitialize);
    inline unsigned short GetVertexCount() const;
    inline NiPoint3* GetVertices();
    inline const NiPoint3* GetVertices() const;
    virtual void SetActiveVertexCount(unsigned short usActive);
    inline unsigned short GetActiveVertexCount() const;

    // normal access
    inline NiPoint3* GetNormals();
    inline const NiPoint3* GetNormals() const;

    
    inline NiShaderRequirementDesc::NBTFlags GetNormalBinormalTangentMethod() const;
    void StoreNormalBinormalTangent(
        NiShaderRequirementDesc::NBTFlags eNBTMethod);

    // bound access
    inline void SetBound(const NiBound& kBound);
    inline NiBound& GetBound();
    inline const NiBound& GetBound() const;

    // color access
    void CreateColors(bool bInitialize);
    inline NiColorA* GetColors();
    inline const NiColorA* GetColors() const;

    // texture coordinate access
    void CreateTextures(bool bInitialize, unsigned short usSets);
    inline unsigned short GetTextureSets() const;
    inline NiPoint2* GetTextures();
    inline const NiPoint2* GetTextures() const;
    NiPoint2* GetTextureSet(unsigned short usSet);
    const NiPoint2* GetTextureSet(unsigned short usSet) const;

    enum Consistency
    {
        MUTABLE             = 0x0000,
        STATIC              = 0x4000,
        VOLATILE            = 0x8000,
        CONSISTENCY_MASK    = 0xF000
    };

    Consistency GetConsistency() const;
    inline void SetConsistency(Consistency eConsistency);

    // KEEP flags inform renderer that original data cannot be thrown away
    // when the object is precached. An app would use these flags to preserve
    // data so it can be used for purposes other than rendering.

    enum Keep
    {
        KEEP_XYZ          = 0x01,
        KEEP_NORM         = 0x02,
        KEEP_COLOR        = 0x04,
        KEEP_UV           = 0x08,
        KEEP_INDICES      = 0x10,
        KEEP_BONEDATA     = 0x20,

        KEEP_ALL          = (KEEP_XYZ | KEEP_NORM | KEEP_COLOR | KEEP_UV |
                             KEEP_INDICES | KEEP_BONEDATA)
    };

    inline unsigned int GetKeepFlags() const;
    inline void SetKeepFlags(unsigned int uiKeepFlags);

    // COMPRESS flags inform renderer to certain compress packed parameters.

    enum
    {
        COMPRESS_NORM     = 0x01,
        COMPRESS_COLOR    = 0x02,
        COMPRESS_UV       = 0x04,
        COMPRESS_WEIGHT   = 0x08,
        COMPRESS_POSITION = 0x10,
        
        COMPRESS_ALL      = (COMPRESS_NORM | COMPRESS_COLOR | COMPRESS_UV |
                             COMPRESS_WEIGHT | COMPRESS_POSITION)
    };
    
    // interface to renderer-specific geometry data
    class NIMAIN_ENTRY RendererData : public NiMemObject
    {
    };

    inline unsigned int GetCompressFlags() const;
    inline void SetCompressFlags(unsigned int uiCompressFlags);

    inline NiAdditionalGeometryData* GetAdditionalGeometryData();
    void SetAdditionalGeometryData(NiAdditionalGeometryData* pkAddData);

    // block allocation
    virtual unsigned int GetBlockAllocationSize() const;
    virtual NiObjectGroup* GetGroup() const;
    virtual void SetGroup(NiObjectGroup* pkGroup);

    typedef unsigned char VertexMask;

    // *** end Emergent internal use only ***
protected:
    // The constructed object is given ownership of the input arrays and
    // has the responsibility for deleting them when finished with them.
    NiGeometryData (unsigned short usVertices, NiPoint3* pkVertex,
        NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture,
        unsigned short usNumTextureSets,
        NiShaderRequirementDesc::NBTFlags eNBTMethod);

    // streaming support
    NiGeometryData();

    inline void SetNumTextureSets(unsigned short usNumTextureSets);
    inline void SetNormalBinormalTangentMethod(
        NiShaderRequirementDesc::NBTFlags eNBTMethod);

    virtual void SetVertexCount( unsigned short usVertices);

    // block allocation
    NiObjectGroup* m_pkGroup;

    // model data (vertices and vertex attributes)
    unsigned short m_usVertices;
    unsigned short m_usID;

    NiBound m_kBound;
    NiPoint3* m_pkVertex;
    NiPoint3* m_pkNormal;
    NiColorA* m_pkColor;
    NiPoint2* m_pkTexture;

    unsigned short m_usActiveVertices;

    unsigned short m_usDataFlags;

    // Note that a revision ID of 0 implies that the object is static and can
    // be optimized in a renderer - it implies that the application does not
    // intend to morph the data
    unsigned short m_usDirtyFlags;

    unsigned char m_ucKeepFlags;
    unsigned char m_ucCompressFlags;

    
    NiAdditionalGeometryDataPtr m_spAdditionalGeomData;

    static unsigned short ms_usNextID;
    static NiCriticalSection ms_kCritSec;
};

typedef NiPointer<NiGeometryData> NiGeometryDataPtr;

#include "NiGeometryData.inl"

#endif // NIGEOMETRYDATA_H
