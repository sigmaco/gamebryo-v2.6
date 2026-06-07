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
#include "NiWireframeProperty.h"

NiImplementRTTI(NiWireframeProperty,NiProperty);

NiWireframePropertyPtr NiWireframeProperty::ms_spDefault;

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiWireframeProperty);
//---------------------------------------------------------------------------
void NiWireframeProperty::CopyMembers(NiWireframeProperty* pkDest,
    NiCloningProcess& kCloning)
{
    NiProperty::CopyMembers(pkDest, kCloning);
    pkDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiWireframeProperty);
//---------------------------------------------------------------------------
void NiWireframeProperty::LoadBinary(NiStream& kStream)
{
    NiProperty::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiWireframeProperty::LinkObject(NiStream& kStream)
{
    NiProperty::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiWireframeProperty::RegisterStreamables(NiStream& kStream)
{
    return NiProperty::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiWireframeProperty::SaveBinary(NiStream& kStream)
{
    NiProperty::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiWireframeProperty::IsEqual(NiObject* pkObject)
{
    if (!NiProperty::IsEqual(pkObject))
        return false;

    NiWireframeProperty* pkWire = (NiWireframeProperty*) pkObject;

    if (GetWireframe() != pkWire->GetWireframe())
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiWireframeProperty::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiProperty::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiWireframeProperty::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_bWireframe", GetWireframe() ));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiStaticDataManager
//---------------------------------------------------------------------------
void NiWireframeProperty::_SDMInit()
{
    ms_spDefault = NiNew NiWireframeProperty;
}
//---------------------------------------------------------------------------
    
void NiWireframeProperty::_SDMShutdown()
{
    NIASSERT(ms_spDefault->GetRefCount() == 1);
    ms_spDefault = NULL;
}
//---------------------------------------------------------------------------
