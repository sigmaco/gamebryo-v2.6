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

#include "NiNode.h"
#include "NiTriBasedGeom.h"

NiImplementRTTI(NiTriBasedGeom, NiGeometry);

//---------------------------------------------------------------------------
NiTriBasedGeom::NiTriBasedGeom(NiTriBasedGeomData* pkModelData)
    : NiGeometry(pkModelData)
{

}
//---------------------------------------------------------------------------
NiTriBasedGeom::NiTriBasedGeom()
{

}
//---------------------------------------------------------------------------
NiTriBasedGeom::~NiTriBasedGeom()
{
    /* */
}
//---------------------------------------------------------------------------
void NiTriBasedGeom::SetModelData(NiGeometryData* pkModelData)
{
    NiGeometry::SetModelData(pkModelData);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiTriBasedGeom::CopyMembers(NiTriBasedGeom* pkDest,
    NiCloningProcess& kCloning)
{
    NiGeometry::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiTriBasedGeom::LoadBinary(NiStream& kStream)
{
    NiGeometry::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiTriBasedGeom::LinkObject(NiStream& kStream)
{
    NiGeometry::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTriBasedGeom::RegisterStreamables(NiStream& kStream)
{
    return NiGeometry::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiTriBasedGeom::SaveBinary(NiStream& kStream)
{
    NiGeometry::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiTriBasedGeom::IsEqual(NiObject* pkObject)
{
    return NiGeometry::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
