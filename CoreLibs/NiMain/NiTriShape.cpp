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

#include "NiCamera.h"
#include "NiTriShape.h"

NiImplementRTTI(NiTriShape,NiTriBasedGeom);

//---------------------------------------------------------------------------
NiTriShape::NiTriShape(unsigned short usVertices, NiPoint3* pkVertex,
    NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture, 
    unsigned short usNumTextureSets,
    NiShaderRequirementDesc::NBTFlags eNBTMethod,
    unsigned short usTriangles, unsigned short* pusTriList) :
    NiTriBasedGeom(NiNew NiTriShapeData(usVertices, pkVertex, pkNormal,
        pkColor, pkTexture, usNumTextureSets, eNBTMethod, usTriangles, 
        pusTriList))
{
}
//---------------------------------------------------------------------------
NiTriShape::NiTriShape(NiTriShapeData* pkModelData) :
    NiTriBasedGeom(pkModelData)
{
}
//---------------------------------------------------------------------------
NiTriShape::NiTriShape()
{
    // called by NiTriShape::CreateObject
}
//---------------------------------------------------------------------------
void NiTriShape::GetModelTriangle(unsigned short usTriangle,
    NiPoint3*& pkP0, NiPoint3*& pkP1, NiPoint3*& pkP2)
{
    NiTriShapeData* pkModelData = 
        NiSmartPointerCast(NiTriShapeData, m_spModelData);
    NIASSERT(usTriangle < pkModelData->GetTriangleCount());
    NiPoint3* pkVertex = pkModelData->GetVertices();
    unsigned short* pusTriList = pkModelData->GetTriList();

    unsigned int uiStart = 3*usTriangle;
    pkP0 = &pkVertex[pusTriList[uiStart++]];
    pkP1 = &pkVertex[pusTriList[uiStart++]];
    pkP2 = &pkVertex[pusTriList[uiStart]];
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTriShape);
//---------------------------------------------------------------------------
void NiTriShape::CopyMembers(NiTriShape* pkDest,
    NiCloningProcess& kCloning)
{
    NiTriBasedGeom::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTriShape);
//---------------------------------------------------------------------------
void NiTriShape::LoadBinary(NiStream& kStream)
{
    NiTriBasedGeom::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiTriShape::LinkObject(NiStream& kStream)
{
    NiTriBasedGeom::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTriShape::RegisterStreamables(NiStream& kStream)
{
    return NiTriBasedGeom::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiTriShape::SaveBinary(NiStream& kStream)
{
    NiTriBasedGeom::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiTriShape::IsEqual(NiObject* pkObject)
{
    return NiTriBasedGeom::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
