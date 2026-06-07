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

#include "NiMainPCH.h"
#include "NiScreenElementsData.h"

NiImplementRTTI(NiScreenElementsData, NiTriShapeData);

const unsigned short NiScreenElementsData::ms_usInvalid = (unsigned short)~0;

//---------------------------------------------------------------------------
NiScreenElementsData::NiScreenElementsData()
{
    m_akPolygon = 0;
    m_ausPIndexer = 0;
    m_usMaxPQuantity = 0;
    m_usPQuantity = 0;
    m_usMaxVQuantity = 0;
    m_usMaxIQuantity = 0;
}
//---------------------------------------------------------------------------
NiScreenElementsData::~NiScreenElementsData()
{
    NiFree(m_akPolygon);
    NiFree(m_ausPIndexer);
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::IsValid(int iPolygon) const
{
    return 0 <= iPolygon && iPolygon < (int)m_usMaxPQuantity &&
        m_ausPIndexer[iPolygon] != ms_usInvalid;
}
//---------------------------------------------------------------------------
int NiScreenElementsData::GetNumVertices(int iPolygon) const
{
    if (IsValid(iPolygon))
        return (int)GetPolygon(iPolygon).m_usNumVertices;

    return 0;
}
//---------------------------------------------------------------------------
int NiScreenElementsData::GetNumTriangles(int iPolygon) const
{
    if (IsValid(iPolygon))
        return (int)GetPolygon(iPolygon).m_usNumTriangles;

    return 0;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetVertex(int iPolygon, int iVertex,
    NiPoint2& kValue) const
{
    if (IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
        {
            int i = iVertex + (int)kPoly.m_usVOffset;
            kValue.x = m_pkVertex[i].x;
            kValue.y = m_pkVertex[i].y;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetVertices(int iPolygon, NiPoint2* akValue) const
{
    if (IsValid(iPolygon) && akValue)
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        NiPoint3* akVertex = &m_pkVertex[kPoly.m_usVOffset];
        for (int i = 0; i < (int)kPoly.m_usNumVertices; i++)
        {
            akValue[i].x = akVertex[i].x;
            akValue[i].y = akVertex[i].y;
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetRectangle(int iPolygon, float& fLeft,
    float& fTop, float& fWidth, float& fHeight) const
{
    if (IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            int i = (int)kPoly.m_usVOffset;
            fLeft = m_pkVertex[i].x;
            fTop = m_pkVertex[i].y;
            i += 2;
            fWidth = m_pkVertex[i].x - fLeft;
            fHeight = m_pkVertex[i].y - fTop;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetNormal(int iPolygon, int iVertex,
    NiPoint3& kValue) const
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
        {
            int i = iVertex + (int)kPoly.m_usVOffset;
            kValue = m_pkNormal[i];
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetNormals(int iPolygon, NiPoint3* akValue) const
{
    if (m_pkNormal && IsValid(iPolygon) && akValue)
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            const NiPoint3* akNormal = &m_pkNormal[kPoly.m_usVOffset];
            int iNumBytes = kPoly.m_usNumVertices * sizeof(NiPoint3);
            NiMemcpy(akValue, akNormal, iNumBytes);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetNormals(int iPolygon, NiPoint3& kUpperLeft,
    NiPoint3& kLowerLeft, NiPoint3& kLowerRight, NiPoint3& kUpperRight) const
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            const NiPoint3* akNormal = &m_pkNormal[kPoly.m_usVOffset];
            kUpperLeft = akNormal[0];
            kLowerLeft = akNormal[1];
            kLowerRight = akNormal[2];
            kUpperRight = akNormal[3];
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetColor(int iPolygon, int iVertex,
    NiColorA& kValue) const
{
    if (m_pkColor && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
        {
            int i = iVertex + (int)kPoly.m_usVOffset;
            kValue = m_pkColor[i];
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetColors(int iPolygon, NiColorA* akValue) const
{
    if (m_pkColor && IsValid(iPolygon) && akValue)
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            const NiColorA* akColor = &m_pkColor[kPoly.m_usVOffset];
            int iNumBytes = kPoly.m_usNumVertices * sizeof(NiColorA);
            NiMemcpy(akValue, akColor, iNumBytes);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetColors(int iPolygon, NiColorA& kUpperLeft,
    NiColorA& kLowerLeft, NiColorA& kLowerRight, NiColorA& kUpperRight) const
{
    if (m_pkColor && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            const NiColorA* akColor = &m_pkColor[kPoly.m_usVOffset];
            kUpperLeft = akColor[0];
            kLowerLeft = akColor[1];
            kLowerRight = akColor[2];
            kUpperRight = akColor[3];
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetTexture(int iPolygon, int iVertex,
    unsigned short usSet, NiPoint2& kValue) const
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (usSet < GetTextureSets())
        {
            const NiPoint2* akTexture = GetTextureSet(usSet);
            if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
            {
                int i = iVertex + (int)kPoly.m_usVOffset;
                kValue = akTexture[i];
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetTextures(int iPolygon, unsigned short usSet,
    NiPoint2* akValue) const
{
    if (m_pkTexture && IsValid(iPolygon) && akValue)
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            if (usSet < GetTextureSets())
            {
                const NiPoint2* akTSet = GetTextureSet(usSet);
                const NiPoint2* akTexture = &akTSet[kPoly.m_usVOffset];
                int iNumBytes = kPoly.m_usNumVertices * sizeof(NiPoint2);
                NiMemcpy(akValue, akTexture, iNumBytes);
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetTextures(int iPolygon, unsigned short usSet,
    NiPoint2& kUpperLeft, NiPoint2& kLowerLeft, NiPoint2& kLowerRight,
    NiPoint2& kUpperRight) const
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            if (usSet < GetTextureSets())
            {
                const NiPoint2* akTSet = GetTextureSet(usSet);
                const NiPoint2* akTexture = &akTSet[kPoly.m_usVOffset];
                kUpperLeft = akTexture[0];
                kLowerLeft = akTexture[1];
                kLowerRight = akTexture[2];
                kUpperRight = akTexture[3];
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
NiPoint3* NiScreenElementsData::GetVertices(int iPolygon)
{
    if (IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkVertex[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
const NiPoint3* NiScreenElementsData::GetVertices(int iPolygon) const
{
    if (IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkVertex[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
NiPoint3* NiScreenElementsData::GetNormals(int iPolygon)
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkNormal[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
const NiPoint3* NiScreenElementsData::GetNormals(int iPolygon) const
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkNormal[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
NiColorA* NiScreenElementsData::GetColors(int iPolygon)
{
    if (m_pkColor && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkColor[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
const NiColorA* NiScreenElementsData::GetColors(int iPolygon) const
{
    if (m_pkColor && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkColor[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
NiPoint2* NiScreenElementsData::GetTextures(int iPolygon,
    unsigned short usSet)
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            if (usSet < GetTextureSets())
            {
                NiPoint2* akTexture = GetTextureSet(usSet);
                return &akTexture[kPoly.m_usVOffset];
            }
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
const NiPoint2* NiScreenElementsData::GetTextures(int iPolygon,
    unsigned short usSet) const
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            if (usSet < GetTextureSets())
            {
                const NiPoint2* akTexture = GetTextureSet(usSet);
                return &akTexture[kPoly.m_usVOffset];
            }
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
unsigned short* NiScreenElementsData::GetIndices(int iPolygon)
{
    if (IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pusTriList[kPoly.m_usIOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
const unsigned short* NiScreenElementsData::GetIndices(int iPolygon) const
{
    if (IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pusTriList[kPoly.m_usIOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiScreenElementsData);
//---------------------------------------------------------------------------
void NiScreenElementsData::LoadBinary(NiStream& kStream)
{
    NiTriShapeData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_usMaxPQuantity);
    m_akPolygon = NiAlloc(Polygon, m_usMaxPQuantity);
    m_ausPIndexer = NiAlloc(unsigned short,m_usMaxPQuantity);

    unsigned int uiMaxPQuantity = (unsigned int)m_usMaxPQuantity;
    NiStreamLoadBinary(kStream, m_akPolygon, uiMaxPQuantity);
    NiStreamLoadBinary(kStream, m_ausPIndexer, uiMaxPQuantity);
    NiStreamLoadBinary(kStream, m_usPGrowBy);
    NiStreamLoadBinary(kStream, m_usPQuantity);
    NiStreamLoadBinary(kStream, m_usMaxVQuantity);
    NiStreamLoadBinary(kStream, m_usVGrowBy);
    NiStreamLoadBinary(kStream, m_usMaxIQuantity);
    NiStreamLoadBinary(kStream, m_usIGrowBy);
}
//---------------------------------------------------------------------------
void NiScreenElementsData::LinkObject(NiStream& kStream)
{
    NiTriShapeData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::RegisterStreamables(NiStream& kStream)
{
    return NiTriShapeData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiScreenElementsData::SaveBinary(NiStream& kStream)
{
    NiTriShapeData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_usMaxPQuantity);

    unsigned int uiMaxPQuantity = (unsigned int)m_usMaxPQuantity;
    NiStreamSaveBinary(kStream, m_akPolygon, uiMaxPQuantity);
    NiStreamSaveBinary(kStream, m_ausPIndexer, uiMaxPQuantity);
    NiStreamSaveBinary(kStream, m_usPGrowBy);
    NiStreamSaveBinary(kStream, m_usPQuantity);
    NiStreamSaveBinary(kStream, m_usMaxVQuantity);
    NiStreamSaveBinary(kStream, m_usVGrowBy);
    NiStreamSaveBinary(kStream, m_usMaxIQuantity);
    NiStreamSaveBinary(kStream, m_usIGrowBy);
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::IsEqual(NiObject* pkObject)
{
    if (!NiTriShapeData::IsEqual(pkObject))
        return false;

    NiScreenElementsData* pkSED = (NiScreenElementsData*)pkObject;

    // compare array and growth sizes
    if (m_usMaxPQuantity != pkSED->m_usMaxPQuantity ||
        m_usPGrowBy != pkSED->m_usPGrowBy ||
        m_usPQuantity != pkSED->m_usPQuantity ||
        m_usMaxVQuantity != pkSED->m_usMaxVQuantity ||
        m_usVGrowBy != pkSED->m_usVGrowBy ||
        m_usMaxIQuantity != pkSED->m_usMaxIQuantity ||
        m_usIGrowBy != pkSED->m_usIGrowBy)
    {
        return false;
    }

    // compare polygons
    int iBytes = m_usMaxPQuantity * sizeof(Polygon);
    if (memcmp(m_akPolygon, pkSED->m_akPolygon, iBytes) != 0)
        return false;

    // compare polygon indexing
    iBytes = m_usMaxPQuantity * sizeof(unsigned short);
    if (memcmp(m_ausPIndexer, pkSED->m_ausPIndexer, iBytes) != 0)
        return false;

    return true;
}
//---------------------------------------------------------------------------
