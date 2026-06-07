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

#include "NiPointLight.h"

NiImplementRTTI(NiPointLight,NiLight);

//---------------------------------------------------------------------------
NiPointLight::NiPointLight()
{
    m_ucEffectType = NiDynamicEffect::POINT_LIGHT;
    m_fAtten0 = 0.0f;
    m_fAtten1 = 1.0f;
    m_fAtten2 = 0.0f;
}
//---------------------------------------------------------------------------
void NiPointLight::UpdateWorldData()
{
    NiLight::UpdateWorldData();
    IncRevisionID();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPointLight);
//---------------------------------------------------------------------------
void NiPointLight::CopyMembers(NiPointLight* pDest,
    NiCloningProcess& kCloning)
{
    NiLight::CopyMembers(pDest, kCloning);

    pDest->m_fAtten0 = m_fAtten0;
    pDest->m_fAtten1 = m_fAtten1;
    pDest->m_fAtten2 = m_fAtten2;

    // The list m_illuminatedNodeList is not processed.  The application
    // has the responsibility for cloning the relationships between the
    // lights and nodes.
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPointLight);
//---------------------------------------------------------------------------
void NiPointLight::LoadBinary(NiStream& stream)
{
    NiLight::LoadBinary(stream);

    NiStreamLoadBinary(stream,m_fAtten0);
    NiStreamLoadBinary(stream,m_fAtten1);
    NiStreamLoadBinary(stream,m_fAtten2);
}
//---------------------------------------------------------------------------
void NiPointLight::LinkObject(NiStream& stream)
{
    NiLight::LinkObject(stream);

    // Illuminated nodes will be linked by the nodes themselves.  They
    // should not be linked here (else you get a recursive loop).
}
//---------------------------------------------------------------------------
bool NiPointLight::RegisterStreamables(NiStream& stream)
{
    return NiLight::RegisterStreamables(stream);

    // Illuminated nodes are already registered by the nodes themselves.
    // They should not be registered here (else you get a recursive loop).
}
//---------------------------------------------------------------------------
void NiPointLight::SaveBinary(NiStream& stream)
{
    NiLight::SaveBinary(stream);

    // attributes
    NiStreamSaveBinary(stream,m_fAtten0);
    NiStreamSaveBinary(stream,m_fAtten1);
    NiStreamSaveBinary(stream,m_fAtten2);
}
//---------------------------------------------------------------------------
bool NiPointLight::IsEqual(NiObject* pObject)
{
    if ( !NiLight::IsEqual(pObject) )
        return false;

    NiPointLight* pLight = (NiPointLight*) pObject;

    // attributes
    if ( m_fAtten0 != pLight->m_fAtten0
    ||   m_fAtten1 != pLight->m_fAtten1
    ||   m_fAtten2 != pLight->m_fAtten2 )
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPointLight::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiLight::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiPointLight::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_fAtten0",m_fAtten0));
    pStrings->Add(NiGetViewerString("m_fAtten1",m_fAtten1));
    pStrings->Add(NiGetViewerString("m_fAtten2",m_fAtten2));
}
//---------------------------------------------------------------------------
