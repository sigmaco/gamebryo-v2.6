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

#include "NiAmbientLight.h"

NiImplementRTTI(NiAmbientLight,NiLight);

//---------------------------------------------------------------------------
NiAmbientLight::NiAmbientLight()
{
    m_ucEffectType = NiDynamicEffect::AMBIENT_LIGHT;
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiAmbientLight);
//---------------------------------------------------------------------------
void NiAmbientLight::CopyMembers(NiAmbientLight* pDest,
    NiCloningProcess& kCloning) 
{
    NiLight::CopyMembers(pDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiAmbientLight);
//---------------------------------------------------------------------------
void NiAmbientLight::LoadBinary(NiStream& stream)
{
    NiLight::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiAmbientLight::LinkObject(NiStream& stream)
{
    NiLight::LinkObject(stream);

    // Illuminated nodes will be linked by the nodes themselves.  They
    // should not be linked here (else you get a recursive loop).
}
//---------------------------------------------------------------------------
bool NiAmbientLight::RegisterStreamables(NiStream& stream)
{
    return NiLight::RegisterStreamables(stream);

    // Illuminated nodes are already registered by the nodes themselves.
    // They should not be registered here (else you get a recursive loop).
}
//---------------------------------------------------------------------------
void NiAmbientLight::SaveBinary(NiStream& stream)
{
    NiLight::SaveBinary(stream);
}
//---------------------------------------------------------------------------
bool NiAmbientLight::IsEqual(NiObject* pObject)
{
    return NiLight::IsEqual(pObject);
}
//---------------------------------------------------------------------------
void NiAmbientLight::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiLight::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiAmbientLight::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

