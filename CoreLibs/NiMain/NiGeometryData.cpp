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

// Precompiled Header
#include "NiMainPCH.h"

#include <NiMemoryDefines.h>
#include "NiGeometryData.h"
#include "NiRenderer.h"
#include "NiAdditionalGeometryData.h"

NiImplementRTTI(NiGeometryData, NiObject);

// zero is not used
unsigned short NiGeometryData::ms_usNextID = 1;

//---------------------------------------------------------------------------
NiCriticalSection NiGeometryData::ms_kCritSec;
//---------------------------------------------------------------------------
NiGeometryData::NiGeometryData(unsigned short usVertices, NiPoint3* pkVertex, 
    NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture,
    unsigned short usNumTextureSets,
    NiShaderRequirementDesc::NBTFlags eNBTMethod) :
    m_pkGroup(NULL),
    m_usVertices(usVertices),
    m_pkVertex(pkVertex),
    m_pkNormal(pkNormal),
    m_pkColor(pkColor),
    m_pkTexture(pkTexture),
    m_usActiveVertices(usVertices),
    m_usDataFlags(0),
    m_usDirtyFlags(0), // marks all geometry as MUTABLE by default
    m_ucKeepFlags(0),
    m_ucCompressFlags(0),
    m_spAdditionalGeomData(0)
{
    SetNumTextureSets(usNumTextureSets);
    SetNormalBinormalTangentMethod(eNBTMethod);

    NIMEMASSERT(NiVerifyAddress(pkVertex));
    NIMEMASSERT(NiVerifyAddress(pkNormal));
    NIMEMASSERT(NiVerifyAddress(pkColor));
    NIMEMASSERT(NiVerifyAddress(pkTexture));
    unsigned short usActiveVertices = GetActiveVertexCount();
    if (usActiveVertices > 0 && m_pkVertex != NULL)
        m_kBound.ComputeFromData(usActiveVertices, m_pkVertex);


    ms_kCritSec.Lock();
    m_usID = ms_usNextID++;
    ms_kCritSec.Unlock();
    SetConsistency(MUTABLE);
}
//---------------------------------------------------------------------------
NiGeometryData::NiGeometryData() :
    m_pkGroup(NULL),
    m_usVertices(0),
    m_pkVertex(NULL),
    m_pkNormal(NULL),
    m_pkColor(NULL),
    m_pkTexture(NULL),
    m_usActiveVertices(0),
    m_usDataFlags(0),
    m_usDirtyFlags(0), // marks all geometry as MUTABLE by default
    m_ucKeepFlags(0),
    m_ucCompressFlags(0),
    m_spAdditionalGeomData(0)
{

    ms_kCritSec.Lock();
    m_usID = ms_usNextID++;    
    ms_kCritSec.Unlock();
    SetConsistency(MUTABLE);
}
//---------------------------------------------------------------------------
NiGeometryData::~NiGeometryData()
{
    SetAdditionalGeometryData(NULL);

    if (GetGroup())
    {
        if (m_pkVertex)
            GetGroup()->DecRefCount();
        if (m_pkNormal)
            GetGroup()->DecRefCount();
        if (m_pkColor)
            GetGroup()->DecRefCount();
        if (m_pkTexture)
            GetGroup()->DecRefCount();
    }
    else
    {
        NiDelete[] m_pkVertex;
        NiDelete[] m_pkNormal;
        NiDelete[] m_pkColor;
        NiDelete[] m_pkTexture;
    }
}
//---------------------------------------------------------------------------
void NiGeometryData::CreateVertices(bool bInitialize)
{
    // NB: Any code that allocates the vertex array by calling this function
    // should also set the bound.

    if (!m_pkVertex)
        m_pkVertex = NiNew NiPoint3[m_usVertices];

    if (bInitialize)
        memset(m_pkVertex, 0, sizeof(m_pkVertex[0]) * m_usVertices);
}
//---------------------------------------------------------------------------
void NiGeometryData::StoreNormalBinormalTangent(
    NiShaderRequirementDesc::NBTFlags eNBTMethod)
{
    if (GetNormalBinormalTangentMethod() == eNBTMethod)
        return;

    SetNormalBinormalTangentMethod(eNBTMethod);

    if (!m_pkNormal)
        return;

    NIASSERT(!GetGroup());

    unsigned int uiFactor = 1;
    if (eNBTMethod != NiShaderRequirementDesc::NBT_METHOD_NONE)
        uiFactor = 3;

    NiPoint3* pkNormal = NiNew NiPoint3[m_usVertices * uiFactor];
    for (unsigned int uiIndex = 0; uiIndex < m_usVertices; uiIndex++)
        pkNormal[uiIndex] = m_pkNormal[uiIndex];

    if (eNBTMethod != NiShaderRequirementDesc::NBT_METHOD_NONE)
    {
        memset((pkNormal + m_usVertices), 0, 
            sizeof(pkNormal[0] * m_usVertices * 2));
    }

    NiDelete[] m_pkNormal;
    m_pkNormal = pkNormal;
}
//---------------------------------------------------------------------------
void NiGeometryData::CreateColors(bool bInitialize)
{
    if (!m_pkColor)
        m_pkColor = NiNew NiColorA[m_usVertices];

    if (bInitialize)
    {
        for (unsigned short usIndex = 0; usIndex < m_usVertices; usIndex++)
            m_pkColor[usIndex] = NiColorA::BLACK;
    }
}
//---------------------------------------------------------------------------
void NiGeometryData::CreateTextures(bool bInitialize, unsigned short usSets)
{
    NIASSERT(!GetGroup());

    if (m_pkTexture && usSets != GetTextureSets())
    {
        NiDelete[] m_pkTexture;
        m_pkTexture = NULL;
    }

    unsigned int uiQuantity = usSets * m_usVertices;
    if (!m_pkTexture)
    {
        m_pkTexture = NiNew NiPoint2[uiQuantity];
        SetNumTextureSets(usSets);
    }

    if (bInitialize)
        memset(m_pkTexture, 0, sizeof(m_pkTexture[0]) * uiQuantity);
}
//---------------------------------------------------------------------------
NiPoint2* NiGeometryData::GetTextureSet(unsigned short usSet)
{
    if (!m_pkTexture || usSet >= GetTextureSets())
        return 0;

    return &m_pkTexture[m_usVertices * usSet];
}
//---------------------------------------------------------------------------
const NiPoint2* NiGeometryData::GetTextureSet(unsigned short usSet) const
{
    if (!m_pkTexture || usSet >= GetTextureSets())
        return 0;

    return &m_pkTexture[m_usVertices * usSet];
}
//---------------------------------------------------------------------------
void NiGeometryData::SetAdditionalGeometryData(
    NiAdditionalGeometryData* pkAddData)
{
    if (pkAddData)
    {
        NIASSERT(pkAddData->GetVertexCount() == GetVertexCount());    
    }

    m_spAdditionalGeomData = pkAddData;
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiGeometryData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 114))
    {
        unsigned int uiID;
        NiStreamLoadBinary(kStream, uiID);
        m_pkGroup = kStream.GetGroupFromID(uiID);
    }

    NiStreamLoadBinary(kStream, m_usVertices);
    m_usActiveVertices = m_usVertices;

    NiStreamLoadBinary(kStream, m_ucKeepFlags);
    NiStreamLoadBinary(kStream, m_ucCompressFlags);

    // flag to indicate existence of vertices
    NiBool bHasVertex;
    NiStreamLoadBinary(kStream, bHasVertex);

    if (bHasVertex)
    {
        if (GetGroup())
        {
            m_pkVertex = (NiPoint3*)GetGroup()->Allocate(m_usVertices * 
                sizeof(NiPoint3));
        }
        else
        {
            m_pkVertex = NiNew NiPoint3[m_usVertices];
        }
        NIASSERT(m_pkVertex);
        NiPoint3::LoadBinary(kStream, m_pkVertex, m_usVertices);
    }

    //Set number of normals to number of vertices.
    unsigned int uiNumberOfNormals = m_usVertices;
    NiStreamLoadBinary(kStream, m_usDataFlags);
    //Scale by 3 if binormals and tangents were streamed.
    if ((m_usDataFlags & NiShaderRequirementDesc::NBT_METHOD_MASK) 
        != NiShaderRequirementDesc::NBT_METHOD_NONE)
        uiNumberOfNormals *= 3;

    // flag to indicate existence of normals
    NiBool bHasNormal;
    NiStreamLoadBinary(kStream, bHasNormal);

    if (bHasNormal)
    {
        if (GetGroup())
        {
            m_pkNormal = (NiPoint3*)GetGroup()->Allocate(uiNumberOfNormals * 
                sizeof(NiPoint3));
        }
        else
        {
            m_pkNormal = NiNew NiPoint3[uiNumberOfNormals];
        }
        NIASSERT(m_pkNormal);
        NiPoint3::LoadBinary(kStream, m_pkNormal, uiNumberOfNormals);
    }

    m_kBound.LoadBinary(kStream);

    // flag to indicate existence of colors
    NiBool bHasColor;
    NiStreamLoadBinary(kStream, bHasColor);

    if (bHasColor)
    {
        if (GetGroup())
        {
            m_pkColor = (NiColorA*)GetGroup()->Allocate(m_usVertices * 
                sizeof(NiColorA));
        }
        else
        {
            m_pkColor = NiNew NiColorA[m_usVertices];
        }
        NIASSERT(m_pkColor);
        NiColorA::LoadBinary(kStream, m_pkColor, m_usVertices);
    }

    if (GetTextureSets() > 0)
    {
        unsigned int uiQuantity = m_usVertices * GetTextureSets();
        if (GetGroup())
        {
            m_pkTexture = (NiPoint2*)GetGroup()->Allocate(uiQuantity * 
                sizeof(NiPoint2));
        }
        else
        {
            m_pkTexture = NiNew NiPoint2[uiQuantity];
        }
        NIASSERT(m_pkTexture);
        NiPoint2::LoadBinary(kStream, m_pkTexture, uiQuantity);
    }

    // Only data in CONSISTENCY_MASK should be present
    NiStreamLoadBinary(kStream, m_usDirtyFlags);
    NIASSERT((m_usDirtyFlags & ~CONSISTENCY_MASK) == 0);

    // m_usID is a derived quantity and must not be saved
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 3, 0, 7))
    {
        kStream.ReadLinkID(); // m_spAdditionalGeomData
    }
}
//---------------------------------------------------------------------------
void NiGeometryData::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 3, 0, 7))
    {
        m_spAdditionalGeomData = (NiAdditionalGeometryData*) 
            kStream.GetObjectFromLinkID();
    }
}
//---------------------------------------------------------------------------
bool NiGeometryData::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    if (m_spAdditionalGeomData && 
        !m_spAdditionalGeomData->RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiGeometryData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    unsigned int uiID = kStream.GetIDFromGroup(m_pkGroup);
    NiStreamSaveBinary(kStream, uiID);

    NiStreamSaveBinary(kStream, m_usVertices);
    NiStreamSaveBinary(kStream, m_ucKeepFlags);
    NiStreamSaveBinary(kStream, m_ucCompressFlags);

    // flag to indicate existence of vertices
    NiBool bHasVertex = (m_pkVertex != NULL);
    NiStreamSaveBinary(kStream, bHasVertex);
    if (bHasVertex)
    {
        NiPoint3::SaveBinary(kStream, m_pkVertex, m_usVertices);
    }

    // Ensure we don't save texture sets that don't exist
    NIASSERT(GetTextureSets() == 0 || m_pkTexture != NULL);
    if (m_pkTexture == NULL)
        SetNumTextureSets(0);

    NiStreamSaveBinary(kStream, m_usDataFlags);
    unsigned int uiNumberOfNormals = m_usVertices;
    if ((m_usDataFlags & NiShaderRequirementDesc::NBT_METHOD_MASK) 
        != NiShaderRequirementDesc::NBT_METHOD_NONE)
        uiNumberOfNormals *= 3;

    // flag to indicate existence of normals
    NiBool bHasNormal = (m_pkNormal != NULL);
    NiStreamSaveBinary(kStream, bHasNormal);
    if (bHasNormal)
    {
        NiPoint3::SaveBinary(kStream, m_pkNormal, uiNumberOfNormals);
    }

    m_kBound.SaveBinary(kStream);

    // flag to indicate existence of colors
    NiBool bHasColor = (m_pkColor != NULL);
    NiStreamSaveBinary(kStream, bHasColor);
    if (bHasColor)
    {
        NiColorA::SaveBinary(kStream, m_pkColor, m_usVertices);
    }

    unsigned int uiQuantity = m_usVertices * GetTextureSets();
    if (uiQuantity != 0 && m_pkTexture != NULL)
        NiPoint2::SaveBinary(kStream, m_pkTexture, uiQuantity);

    NiStreamSaveBinary(kStream, (unsigned short) (m_usDirtyFlags &
        CONSISTENCY_MASK));
    // m_usID is a derived quantity and must not be saved

    kStream.SaveLinkID(m_spAdditionalGeomData);
}
//---------------------------------------------------------------------------
bool NiGeometryData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiGeometryData* pkData = (NiGeometryData*) pkObject;
    unsigned int uiIndex;

    // vertices
    if (m_usVertices != pkData->m_usVertices)
        return false;

    if (m_usActiveVertices != pkData->m_usActiveVertices)
        return false;

    if ((m_pkVertex && !pkData->m_pkVertex)
        || (!m_pkVertex && pkData->m_pkVertex))
    {
        return false;
    }

    if (GetKeepFlags() != pkData->GetKeepFlags())
        return false;

    if (GetCompressFlags() != pkData->GetCompressFlags())
        return false;

    if (m_pkVertex)
    {
        for (uiIndex = 0; uiIndex < m_usVertices; uiIndex++)
        {
            if (m_pkVertex[uiIndex] != pkData->m_pkVertex[uiIndex])
                return false;
        }
    }

    // normals
    if ((m_pkNormal && !pkData->m_pkNormal)
        || (!m_pkNormal && pkData->m_pkNormal))
    {
        return false;
    }

    if (m_pkNormal)
    {
        if (GetNormalBinormalTangentMethod() != 
            pkData->GetNormalBinormalTangentMethod())
        {
            return false;
        }
        unsigned int uiCount = 
            (GetNormalBinormalTangentMethod() == 
                NiShaderRequirementDesc::NBT_METHOD_NONE) ? 
                    m_usVertices : 3 * m_usVertices; 
        for (uiIndex = 0; uiIndex < uiCount; uiIndex++)
        {
            if (m_pkNormal[uiIndex] != pkData->m_pkNormal[uiIndex])
                return false;
        }
    }

    // bounds
    if (m_kBound != pkData->m_kBound)
        return false;

    // colors
    if ((m_pkColor && !pkData->m_pkColor) || 
        (!m_pkTexture && pkData->m_pkTexture))
    {
        return false;
    }

    if (m_pkColor)
    {
        for (uiIndex = 0; uiIndex < m_usVertices; uiIndex++)
        {
            if (m_pkColor[uiIndex] != pkData->m_pkColor[uiIndex])
                return false;
        }
    }

    // texture coordinates
    if ((m_pkTexture && !pkData->m_pkTexture) || 
       (!m_pkTexture && pkData->m_pkTexture))
    {
        return false;
    }

    if (m_pkTexture)
    {
        if (GetTextureSets() != pkData->GetTextureSets())
            return false;

        unsigned int uiQuantity = m_usVertices * GetTextureSets();
        for (uiIndex = 0; uiIndex < uiQuantity; uiIndex++)
        {
            if (m_pkTexture[uiIndex] != pkData->m_pkTexture[uiIndex])
                return false;
        }
    }

    if ((m_spAdditionalGeomData && !pkData->m_spAdditionalGeomData) || 
       (!m_spAdditionalGeomData && pkData->m_spAdditionalGeomData))
    {
        return false;
    }
    
    if (m_spAdditionalGeomData && pkData->m_spAdditionalGeomData && 
        !(m_spAdditionalGeomData->IsEqual(pkData->m_spAdditionalGeomData)))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiGeometryData::GetBlockAllocationSize() const
{
    unsigned int uiSize = m_pkVertex ? (m_usVertices*sizeof(NiPoint3)) : 0;

    if (m_pkNormal)
    {
        uiSize += m_usVertices * 
            ((GetNormalBinormalTangentMethod() != 0) ? 3 : 1) * 
            sizeof(NiPoint3);
    }

    if (m_pkColor)
        uiSize += m_usVertices * sizeof(NiColorA);

    if (m_pkTexture)
        uiSize += m_usVertices * GetTextureSets() * sizeof(NiPoint2);


    return NiObject::GetBlockAllocationSize() + uiSize;
}
//---------------------------------------------------------------------------
NiObjectGroup* NiGeometryData::GetGroup() const
{
    return m_pkGroup;
}
//---------------------------------------------------------------------------
void NiGeometryData::SetGroup(NiObjectGroup* pkGroup)
{
    m_pkGroup = pkGroup;
}
//---------------------------------------------------------------------------
