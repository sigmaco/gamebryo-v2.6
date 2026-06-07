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

#include "NiTriShapeDynamicData.h"

NiImplementRTTI(NiTriShapeDynamicData,NiTriShapeData);

//---------------------------------------------------------------------------
NiTriShapeDynamicData::NiTriShapeDynamicData(unsigned short usVertices,
    NiPoint3* pkVertex, NiPoint3* pkNormal, NiColorA* pkColor, 
    NiPoint2* pkTexture, unsigned short usNumTextureSets, 
    NiShaderRequirementDesc::NBTFlags eNBTMethod,
    unsigned short usTriangles, 
    unsigned short* pusTriList, unsigned short usActiveVertices,
    unsigned short usActiveTriangles) : 
    NiTriShapeData(usActiveVertices, pkVertex, pkNormal, pkColor, pkTexture, 
        usNumTextureSets, eNBTMethod, usActiveTriangles, pusTriList)
{   
    m_usActiveVertices = usActiveVertices;
    m_usActiveTriangles = usActiveTriangles;
    m_usVertices = usVertices;
    m_usTriangles = usTriangles;
    m_uiTriListLength = 3*m_usTriangles;
}
//---------------------------------------------------------------------------
NiTriShapeDynamicData::NiTriShapeDynamicData()
{
    m_usActiveVertices = 0;
    m_usActiveTriangles = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTriShapeDynamicData);

//---------------------------------------------------------------------------
void NiTriShapeDynamicData::LoadBinary(NiStream& kStream)
{
    NiTriShapeData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_usActiveVertices);
    NiStreamLoadBinary(kStream, m_usActiveTriangles);
}
//---------------------------------------------------------------------------
void NiTriShapeDynamicData::LinkObject(NiStream& kStream)
{
    NiTriShapeData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTriShapeDynamicData::RegisterStreamables(NiStream& kStream)
{
    return NiTriShapeData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiTriShapeDynamicData::SaveBinary(NiStream& kStream)
{
    NiTriShapeData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_usActiveVertices);
    NiStreamSaveBinary(kStream, m_usActiveTriangles);
}
//---------------------------------------------------------------------------
bool NiTriShapeDynamicData::IsEqual(NiObject* pkObject)
{
    if (!NiTriShapeData::IsEqual(pkObject))
        return false;

    NiTriShapeDynamicData* pkData = (NiTriShapeDynamicData*) pkObject;

    if (m_usActiveVertices != pkData->m_usActiveVertices ||
        m_usActiveTriangles != pkData->m_usActiveTriangles)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
