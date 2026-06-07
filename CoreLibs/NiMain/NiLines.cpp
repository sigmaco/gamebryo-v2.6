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
#include "NiLines.h"
#include "NiRenderer.h"

NiImplementRTTI(NiLines, NiGeometry);

//---------------------------------------------------------------------------
NiLines::NiLines(unsigned short usVertices, NiPoint3* pkVertex,
    NiColorA* pkColor, NiPoint2* pkTexture, unsigned short usNumTextureSets, 
    NiShaderRequirementDesc::NBTFlags eNBTMethod, NiBool* pkFlags) :
    NiGeometry(NiNew NiLinesData(usVertices, pkVertex, pkColor, pkTexture, 
        usNumTextureSets, eNBTMethod, pkFlags))
{
}
//---------------------------------------------------------------------------
NiLines::NiLines(NiLinesData* pkModelData) :
    NiGeometry(pkModelData)
{
}
//---------------------------------------------------------------------------
NiLines::NiLines()
{
    // called by NiLines::CreateObject
}

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiLines);
//---------------------------------------------------------------------------
void NiLines::CopyMembers(NiLines* pkDest,
    NiCloningProcess& kCloning)
{
    NiGeometry::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiLines);
//---------------------------------------------------------------------------
void NiLines::LoadBinary(NiStream& kStream)
{
    NiGeometry::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiLines::LinkObject(NiStream& kStream)
{
    NiGeometry::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiLines::RegisterStreamables(NiStream& kStream)
{
    return NiGeometry::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiLines::SaveBinary(NiStream& kStream)
{
    NiGeometry::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiLines::IsEqual(NiObject* pkObject)
{
    return NiGeometry::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
