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
#include "NiPhysXPCH.h"

#include "NiPhysXMeshModifier.h"

//---------------------------------------------------------------------------
NiImplementRTTI(NiPhysXMeshModifier, NiMeshModifier);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiPhysXMeshModifier::NiPhysXMeshModifier() : NiMeshModifier()
{
    // Intentionally left blank.
}
//---------------------------------------------------------------------------
NiPhysXMeshModifier::~NiPhysXMeshModifier()
{
    // Intentionally left blank.
}
//---------------------------------------------------------------------------
void NiPhysXMeshModifier::CopyMembers(NiPhysXMeshModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiMeshModifier::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
void NiPhysXMeshModifier::LoadBinary(NiStream& kStream)
{
    NiMeshModifier::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXMeshModifier::LinkObject(NiStream& kStream)
{
    NiMeshModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshModifier::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshModifier::SaveBinary(NiStream& kStream)
{
    NiMeshModifier::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshModifier::IsEqual(NiObject* pkObject)
{
    if (!NiMeshModifier::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiMeshModifier::GetViewerStrings(pkStrings);
    pkStrings->Add(NiGetViewerString(NiPhysXMeshModifier::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
