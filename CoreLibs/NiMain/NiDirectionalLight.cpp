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

#include "NiDirectionalLight.h"

NiImplementRTTI(NiDirectionalLight,NiLight);

//---------------------------------------------------------------------------
NiDirectionalLight::NiDirectionalLight()
{
    m_ucEffectType = NiDynamicEffect::DIR_LIGHT;
    m_kWorldDir = NiPoint3(1.0f, 0.0f, 0.0f);
}
//---------------------------------------------------------------------------
void NiDirectionalLight::UpdateWorldData()
{
    NiLight::UpdateWorldData();

    // update the light direction
    m_kWorld.m_Rotate.GetCol(0, m_kWorldDir);

    IncRevisionID();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiDirectionalLight);
//---------------------------------------------------------------------------
void NiDirectionalLight::CopyMembers(NiDirectionalLight* pDest,
    NiCloningProcess& kCloning)
{
    NiLight::CopyMembers(pDest, kCloning);

    pDest->m_kWorldDir = m_kWorldDir;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiDirectionalLight);
//---------------------------------------------------------------------------
void NiDirectionalLight::LoadBinary(NiStream& stream)
{
    NiLight::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiDirectionalLight::LinkObject(NiStream& stream)
{
    NiLight::LinkObject(stream);

    // Illuminated nodes will be linked by the nodes themselves.  They
    // should not be linked here (else you get a recursive loop).
}
//---------------------------------------------------------------------------
bool NiDirectionalLight::RegisterStreamables(NiStream& stream)
{
    return NiLight::RegisterStreamables(stream);

    // Illuminated nodes are already registered by the nodes themselves.
    // They should not be registered here (else you get a recursive loop).
}
//---------------------------------------------------------------------------
void NiDirectionalLight::SaveBinary(NiStream& stream)
{
    NiLight::SaveBinary(stream);
}
//---------------------------------------------------------------------------
bool NiDirectionalLight::IsEqual(NiObject* pObject)
{
    return NiLight::IsEqual(pObject);
}
//---------------------------------------------------------------------------
void NiDirectionalLight::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiLight::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiDirectionalLight::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
