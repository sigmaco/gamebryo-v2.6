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

#include "NiLinesData.h"

NiImplementRTTI(NiLinesData,NiGeometryData);

//---------------------------------------------------------------------------
NiLinesData::NiLinesData(unsigned short usVertices, NiPoint3* pkVertex,
    NiColorA* pkColor, NiPoint2* pkTexture, unsigned short usNumTextureSets, 
    NiShaderRequirementDesc::NBTFlags eNBTMethod, NiBool* pkFlags) :
    NiGeometryData(usVertices, pkVertex, 0, pkColor, pkTexture,
        usNumTextureSets, eNBTMethod)
{
    if (pkFlags)
    {
        // polyline specified by caller
        m_pkFlags = pkFlags;
    }
    else
    {
        // vertices treated as disjoint set of line segments
        m_pkFlags = NiAlloc(NiBool, usVertices);
        NIASSERT(m_pkFlags);
        for (unsigned short i = 0; i < usVertices; i++)
            m_pkFlags[i] = (i & 1 ? false : true);
    }
}
//---------------------------------------------------------------------------
NiLinesData::NiLinesData()
{
    m_pkFlags = 0;
}
//---------------------------------------------------------------------------
NiLinesData::~NiLinesData()
{
    NiFree(m_pkFlags);
}
//---------------------------------------------------------------------------
void NiLinesData::Replace(NiBool* pkFlags)
{
    NiFree(m_pkFlags);
    m_pkFlags = pkFlags;
}
//---------------------------------------------------------------------------
void NiLinesData::SetData(NiBool* pkFlags)
{
    m_pkFlags = pkFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiLinesData);
//---------------------------------------------------------------------------
void NiLinesData::LoadBinary(NiStream& kStream)
{
    NiGeometryData::LoadBinary(kStream);

    m_pkFlags = NiAlloc(NiBool, m_usVertices);
    NIASSERT(m_pkFlags);
    NiStreamLoadBinary(kStream, m_pkFlags, m_usVertices);
}
//---------------------------------------------------------------------------
void NiLinesData::LinkObject(NiStream& kStream)
{
    NiGeometryData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiLinesData::RegisterStreamables(NiStream& kStream)
{
    return NiGeometryData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiLinesData::SaveBinary(NiStream& kStream)
{
    NiGeometryData::SaveBinary(kStream);

    NIASSERT(m_pkFlags);
    NiStreamSaveBinary(kStream, m_pkFlags, m_usVertices);
}
//---------------------------------------------------------------------------
bool NiLinesData::IsEqual(NiObject* pkObject)
{
    if (!NiGeometryData::IsEqual(pkObject))
        return false;

    NiLinesData* pkLines = (NiLinesData*) pkObject;

    if ((m_pkFlags && !pkLines->m_pkFlags) || 
        (!m_pkFlags && pkLines->m_pkFlags))
    {
        return false;
    }

    if (m_pkFlags)
    {
        for (unsigned short i = 0; i < m_usVertices; i++)
        {
            if (m_pkFlags[i] != pkLines->m_pkFlags[i])
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
