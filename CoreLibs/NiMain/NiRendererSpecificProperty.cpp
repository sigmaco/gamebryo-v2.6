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

#include "NiRendererSpecificProperty.h"

NiImplementRTTI(NiRendererSpecificProperty, NiProperty);

NiRendererSpecificPropertyPtr NiRendererSpecificProperty::ms_spDefault;

//---------------------------------------------------------------------------
NiRendererSpecificProperty::NiRendererSpecificProperty()
{
}
//---------------------------------------------------------------------------
NiRendererSpecificProperty::~NiRendererSpecificProperty()
{
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiRendererSpecificProperty);

//---------------------------------------------------------------------------
void NiRendererSpecificProperty::CopyMembers(
    NiRendererSpecificProperty* pkDest,
    NiCloningProcess& kCloning)
{
    NiProperty::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiRendererSpecificProperty);
//---------------------------------------------------------------------------
void NiRendererSpecificProperty::LoadBinary(NiStream& kStream)
{
    NiProperty::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiRendererSpecificProperty::LinkObject(NiStream& kStream)
{
    NiProperty::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiRendererSpecificProperty::RegisterStreamables(NiStream& kStream)
{
    if (!NiProperty::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiRendererSpecificProperty::SaveBinary(NiStream& kStream)
{
    NiProperty::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiRendererSpecificProperty::IsEqual(NiObject* pkObject)
{
    if (!NiProperty::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiRendererSpecificProperty::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiProperty::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiRendererSpecificProperty::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiRendererSpecificProperty::_SDMInit()
{
    ms_spDefault = NiNew NiRendererSpecificProperty;
}
//---------------------------------------------------------------------------
    
void NiRendererSpecificProperty::_SDMShutdown()
{
    NIASSERT(ms_spDefault->GetRefCount() == 1);
    ms_spDefault = NULL;
}
//---------------------------------------------------------------------------
