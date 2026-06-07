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

#include "NiAVObjectPalette.h"
#include "NiCloningProcess.h"
#include <NiSystem.h>

NiImplementRTTI(NiAVObjectPalette, NiObject);

//---------------------------------------------------------------------------
// Base interface - NiAVObjectPalette
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiAVObjectPalette::CopyMembers(NiAVObjectPalette* pkDest, 
                                    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);
}


//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiAVObjectPalette::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiAVObjectPalette::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiAVObjectPalette::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiAVObjectPalette::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiAVObjectPalette::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;
    return true;
}
//---------------------------------------------------------------------------
void NiAVObjectPalette::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiAVObjectPalette::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

