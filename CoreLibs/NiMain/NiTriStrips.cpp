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

#include "NiTriStrips.h"
#include "NiCamera.h"

NiImplementRTTI(NiTriStrips,NiTriBasedGeom);

//---------------------------------------------------------------------------
NiTriStrips::NiTriStrips(unsigned short usVertices, NiPoint3* pkVertex,
    NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture, 
    unsigned short usNumTextureSets,
    NiShaderRequirementDesc::NBTFlags eNBTMethod,
    unsigned short usTriangles, unsigned short usStrips, 
    unsigned short* pusStripLengths, unsigned short* pusStripLists) :
    NiTriBasedGeom(NiNew NiTriStripsData(usVertices, pkVertex,
        pkNormal, pkColor, pkTexture, usNumTextureSets, 
        eNBTMethod, usTriangles, usStrips, pusStripLengths, pusStripLists))
{
}
//---------------------------------------------------------------------------
NiTriStrips::NiTriStrips(NiTriStripsData* pkModelData) :
    NiTriBasedGeom(pkModelData)
{
}
//---------------------------------------------------------------------------
NiTriStrips::NiTriStrips()
{
    // called by NiTriStrips::CreateObject
}
//---------------------------------------------------------------------------
void NiTriStrips::GetModelTriangle(unsigned short usTriangle, 
    NiPoint3*& pkP0, NiPoint3*& pkP1, NiPoint3*& pkP2)
{
    NiTriStripsData* pkModelData = 
        NiSmartPointerCast(NiTriStripsData, m_spModelData);
    NIASSERT(usTriangle < pkModelData->GetTriangleCount() );
    NiPoint3* pkVertex = pkModelData->GetVertices();

    unsigned short i0, i1, i2;
    GetTriangleIndices(usTriangle, i0, i1, i2);
    pkP0 = &pkVertex[i0];
    pkP1 = &pkVertex[i1];
    pkP2 = &pkVertex[i2];
}

//---------------------------------------------------------------------------
             
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTriStrips);

//---------------------------------------------------------------------------
void NiTriStrips::CopyMembers(NiTriStrips* pkDest,
    NiCloningProcess& kCloning)
{
    NiTriBasedGeom::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// kStreaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTriStrips);

//---------------------------------------------------------------------------
void NiTriStrips::LoadBinary(NiStream& kStream)
{
    NiTriBasedGeom::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiTriStrips::LinkObject(NiStream& kStream)
{
    NiTriBasedGeom::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTriStrips::RegisterStreamables(NiStream& kStream)
{
    return NiTriBasedGeom::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiTriStrips::SaveBinary(NiStream& kStream)
{
    NiTriBasedGeom::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiTriStrips::IsEqual(NiObject* pObject)
{
    return NiTriBasedGeom::IsEqual(pObject);
}
//---------------------------------------------------------------------------
void NiTriStrips::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiTriBasedGeom::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiTriStrips::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
