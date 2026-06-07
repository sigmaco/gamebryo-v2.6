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
#include "NiDitherProperty.h"

NiImplementRTTI(NiDitherProperty,NiProperty);

NiDitherPropertyPtr NiDitherProperty::ms_spDefault;

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiDitherProperty);
//---------------------------------------------------------------------------
void NiDitherProperty::CopyMembers(NiDitherProperty* pkDest,
    NiCloningProcess& kCloning)
{
    NiProperty::CopyMembers(pkDest, kCloning);
    pkDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiDitherProperty);
//---------------------------------------------------------------------------
void NiDitherProperty::LoadBinary(NiStream& kStream)
{
    NiProperty::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiDitherProperty::LinkObject(NiStream& kStream)
{
    NiProperty::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiDitherProperty::RegisterStreamables(NiStream& kStream)
{
    return NiProperty::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiDitherProperty::SaveBinary(NiStream& kStream)
{
    NiProperty::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiDitherProperty::IsEqual(NiObject* pkObject)
{
    if (!NiProperty::IsEqual(pkObject))
        return false;

    NiDitherProperty* pkDither = (NiDitherProperty*) pkObject;

    if (GetDithering() != pkDither->GetDithering())
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiDitherProperty::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiProperty::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiDitherProperty::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_bDither", GetDithering()));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiDitherProperty::_SDMInit()
{
    ms_spDefault = NiNew NiDitherProperty;
}
//---------------------------------------------------------------------------
    
void NiDitherProperty::_SDMShutdown()
{
    NIASSERT(ms_spDefault->GetRefCount() == 1);
    ms_spDefault = NULL;
}
//---------------------------------------------------------------------------
