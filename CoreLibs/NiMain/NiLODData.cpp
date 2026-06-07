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

#include "NiLODData.h"

NiImplementRTTI(NiLODData, NiObject);

//---------------------------------------------------------------------------
NiLODData::NiLODData()
{
}
//---------------------------------------------------------------------------
NiLODData::~NiLODData()
{
}
//---------------------------------------------------------------------------
void NiLODData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiLODData::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiLODData::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiLODData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiLODData::IsEqual(NiObject*)
{
    return true;
}
//---------------------------------------------------------------------------
