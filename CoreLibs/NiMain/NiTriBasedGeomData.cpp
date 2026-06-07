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

#include "NiTriBasedGeomData.h"

NiImplementRTTI(NiTriBasedGeomData,NiGeometryData);

//---------------------------------------------------------------------------
NiTriBasedGeomData::NiTriBasedGeomData(unsigned short usVertices,
    NiPoint3* pkVertex, NiPoint3* pkNormal, NiColorA* pkColor,
    NiPoint2* pkTexture, unsigned short usNumTextureSets, 
    NiShaderRequirementDesc::NBTFlags eNBTMethod,
    unsigned short usTriangles) :
    NiGeometryData(usVertices, pkVertex, pkNormal, pkColor, pkTexture,
        usNumTextureSets, eNBTMethod),
        m_usTriangles(usTriangles),
        m_usActiveTriangles(usTriangles)
{
    /* */
}
//---------------------------------------------------------------------------
NiTriBasedGeomData::NiTriBasedGeomData() :
    m_usTriangles(0),
    m_usActiveTriangles(0)
{
    /* */
}
//---------------------------------------------------------------------------
NiTriBasedGeomData::~NiTriBasedGeomData()
{
    /* */
}
//---------------------------------------------------------------------------
void NiTriBasedGeomData::GetTriangleIndices(unsigned short /*i*/,
    unsigned short& /*i0*/, unsigned short& /*i1*/,
    unsigned short& /*i2*/) const
{
    NIASSERT(0);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiTriBasedGeomData::LoadBinary(NiStream& kStream)
{
    NiGeometryData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_usTriangles);
    m_usActiveTriangles = m_usTriangles;
}
//---------------------------------------------------------------------------
void NiTriBasedGeomData::LinkObject(NiStream& kStream)
{
    NiGeometryData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTriBasedGeomData::RegisterStreamables(NiStream& kStream)
{
    return NiGeometryData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiTriBasedGeomData::SaveBinary(NiStream& kStream)
{
    NiGeometryData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_usTriangles);
}
//---------------------------------------------------------------------------
bool NiTriBasedGeomData::IsEqual(NiObject* pkObject)
{
    if (!NiGeometryData::IsEqual(pkObject))
        return false;

    NiTriBasedGeomData* pkData = (NiTriBasedGeomData*)pkObject;

    if (m_usTriangles != pkData->m_usTriangles ||
        m_usActiveTriangles != pkData->m_usActiveTriangles)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
