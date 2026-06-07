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
//---------------------------------------------------------------------------
//  NiPointLight inline functions

//---------------------------------------------------------------------------
inline void NiPointLight::SetConstantAttenuation(float fC)
{
    m_fAtten0 = fC;
    IncRevisionID();
}
//---------------------------------------------------------------------------
inline float NiPointLight::GetConstantAttenuation() const
{
    return m_fAtten0;
}
//---------------------------------------------------------------------------
inline void NiPointLight::SetLinearAttenuation(float fL)
{
    m_fAtten1 = fL;
    IncRevisionID();
}
//---------------------------------------------------------------------------
inline float NiPointLight::GetLinearAttenuation() const
{
    return m_fAtten1;
}
//---------------------------------------------------------------------------
inline void NiPointLight::SetQuadraticAttenuation(float fQ)
{
    m_fAtten2 = fQ;
    IncRevisionID();
}
//---------------------------------------------------------------------------
inline float NiPointLight::GetQuadraticAttenuation() const
{
    return m_fAtten2;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPointLight::GetWorldLocation() const
{
    return m_kWorld.m_Translate;
}
//---------------------------------------------------------------------------
