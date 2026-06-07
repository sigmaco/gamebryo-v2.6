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

#include "NiBool.h"
#include "NiShadeProperty.h"

NiImplementRTTI(NiShadeProperty,NiProperty);

NiShadePropertyPtr NiShadeProperty::ms_spDefault;

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiShadeProperty);
//---------------------------------------------------------------------------
void NiShadeProperty::CopyMembers(NiShadeProperty* pDest,
    NiCloningProcess& kCloning)
{
    NiProperty::CopyMembers(pDest, kCloning);
    pDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiShadeProperty);
//---------------------------------------------------------------------------
void NiShadeProperty::LoadBinary(NiStream& kStream)
{
    NiProperty::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiShadeProperty::LinkObject(NiStream& kStream)
{
    NiProperty::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiShadeProperty::RegisterStreamables(NiStream& kStream)
{
    return NiProperty::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiShadeProperty::SaveBinary(NiStream& kStream)
{
    NiProperty::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiShadeProperty::IsEqual(NiObject* pkObject)
{
    if (!NiProperty::IsEqual(pkObject))
        return false;

    NiShadeProperty* pkShade = (NiShadeProperty*) pkObject;

    if (GetSmooth() != pkShade->GetSmooth())
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiShadeProperty::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiProperty::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiShadeProperty::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_bSmooth",GetSmooth()));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiShadeProperty::_SDMInit()
{
    ms_spDefault = NiNew NiShadeProperty;
}
//---------------------------------------------------------------------------
    
void NiShadeProperty::_SDMShutdown()
{
    NIASSERT(ms_spDefault->GetRefCount() == 1);
    ms_spDefault = NULL;
}
//---------------------------------------------------------------------------
