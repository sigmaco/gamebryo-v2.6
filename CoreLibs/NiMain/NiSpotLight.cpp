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

#include "NiSpotLight.h"

NiImplementRTTI(NiSpotLight,NiPointLight);

//---------------------------------------------------------------------------
NiSpotLight::NiSpotLight()
{
    m_ucEffectType = NiDynamicEffect::SPOT_LIGHT;
    m_kWorldDir = NiPoint3(1.0f, 0.0f, 0.0f);

    m_fSpotExponent = 1.0f;
    m_fOuterSpotAngle = 0.0f;
    m_fInnerSpotAngle = 0.0f;
    m_fOuterSpotAngleCos = 1.0f;
    m_fInnerSpotAngleCos = 1.0f;
}
//---------------------------------------------------------------------------
void NiSpotLight::UpdateWorldData()
{
    NiPointLight::UpdateWorldData();

    // update the light direction
    m_kWorld.m_Rotate.GetCol(0, m_kWorldDir);

    IncRevisionID();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiSpotLight);
//---------------------------------------------------------------------------
void NiSpotLight::CopyMembers(NiSpotLight* pDest,
    NiCloningProcess& kCloning)
{
    NiPointLight::CopyMembers(pDest, kCloning);

    pDest->m_kWorldDir = m_kWorldDir;
    pDest->m_fOuterSpotAngle = m_fOuterSpotAngle;
    pDest->m_fInnerSpotAngle = m_fInnerSpotAngle;
    pDest->m_fSpotExponent = m_fSpotExponent;
    pDest->m_fOuterSpotAngleCos = m_fOuterSpotAngleCos;
    pDest->m_fInnerSpotAngleCos = m_fInnerSpotAngleCos;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSpotLight);
//---------------------------------------------------------------------------
void NiSpotLight::LoadBinary(NiStream& stream)
{
    NiPointLight::LoadBinary(stream);

    // attributes
    NiStreamLoadBinary(stream,m_fOuterSpotAngle);

    if (stream.GetFileVersion() >= NiStream::GetVersion(20, 2, 0, 5))
        NiStreamLoadBinary(stream, m_fInnerSpotAngle);
    
    NiStreamLoadBinary(stream,m_fSpotExponent);

    // Set the spot angles to themselves to update the cosine caches.
    SetSpotAngle(m_fOuterSpotAngle);
    SetInnerSpotAngle(m_fInnerSpotAngle);
}
//---------------------------------------------------------------------------
void NiSpotLight::LinkObject(NiStream& stream)
{
    NiPointLight::LinkObject(stream);

    // Illuminated nodes will be linked by the nodes themselves.  They
    // should not be linked here (else you get a recursive loop).
}
//---------------------------------------------------------------------------
bool NiSpotLight::RegisterStreamables(NiStream& stream)
{
    return NiPointLight::RegisterStreamables(stream);

    // Illuminated nodes are already registered by the nodes themselves.
    // They should not be registered here (else you get a recursive loop).
}
//---------------------------------------------------------------------------
void NiSpotLight::SaveBinary(NiStream& stream)
{
    NiPointLight::SaveBinary(stream);

    // attributes
    NiStreamSaveBinary(stream,m_fOuterSpotAngle);
    NiStreamSaveBinary(stream,m_fInnerSpotAngle);
    NiStreamSaveBinary(stream,m_fSpotExponent);
}
//---------------------------------------------------------------------------
bool NiSpotLight::IsEqual(NiObject* pObject)
{
    if ( !NiPointLight::IsEqual(pObject) )
        return false;

    NiSpotLight* pLight = (NiSpotLight*) pObject;

    // attributes
    if ( m_fOuterSpotAngle != pLight->m_fOuterSpotAngle
    || m_fSpotExponent != pLight->m_fSpotExponent 
    || m_fInnerSpotAngle != pLight->m_fInnerSpotAngle)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSpotLight::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiPointLight::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiSpotLight::ms_RTTI.GetName()));

    pStrings->Add(m_kWorldDir.GetViewerString("m_kWorldDir"));

    pStrings->Add(NiGetViewerString("m_fOuterSpotAngle",m_fOuterSpotAngle));
    pStrings->Add(NiGetViewerString("m_fInnerSpotAngle",m_fInnerSpotAngle));
    pStrings->Add(NiGetViewerString("m_fSpotExponent",m_fSpotExponent));
}
//---------------------------------------------------------------------------
