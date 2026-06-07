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

#include "NiProperty.h"
#include "NiTimeController.h"

NiImplementRTTI(NiProperty,NiObjectNET);

//---------------------------------------------------------------------------
void NiProperty::Update(float fTime)
{
    NiTimeController* pkControl = GetControllers();
    for (/**/; pkControl; pkControl = pkControl->GetNext())
        pkControl->Update(fTime);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiProperty::CopyMembers(NiProperty* pkDest,
    NiCloningProcess& kCloning)
{
    NiObjectNET::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiProperty::LoadBinary(NiStream& kStream)
{
    NiObjectNET::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiProperty::LinkObject(NiStream& kStream)
{
    NiObjectNET::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiProperty::RegisterStreamables(NiStream& kStream)
{
    return NiObjectNET::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiProperty::SaveBinary(NiStream& kStream)
{
    NiObjectNET::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiProperty::IsEqual(NiObject* pkObject)
{
    return NiObjectNET::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
void NiProperty::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObjectNET::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiProperty::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
