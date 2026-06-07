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
inline NiAVObject* NiPSGravityForce::GetGravityObj() const
{
    return m_pkGravityObj;
}
//---------------------------------------------------------------------------
inline void NiPSGravityForce::SetGravityObj(NiAVObject* pkGravityObj)
{
    m_pkGravityObj = pkGravityObj;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPSGravityForce::GetGravityAxis() const
{
    return m_kInputData.m_kGravityAxis;
}
//---------------------------------------------------------------------------
inline void NiPSGravityForce::SetGravityAxis(const NiPoint3& kGravityAxis)
{
    m_kInputData.m_kGravityAxis = kGravityAxis;
}
//---------------------------------------------------------------------------
inline float NiPSGravityForce::GetDecay() const
{
    return m_kInputData.m_fDecay;
}
//---------------------------------------------------------------------------
inline void NiPSGravityForce::SetDecay(float fDecay)
{
    m_kInputData.m_fDecay = fDecay;
}
//---------------------------------------------------------------------------
inline float NiPSGravityForce::GetStrength() const
{
    return m_kInputData.m_fStrength;
}
//---------------------------------------------------------------------------
inline void NiPSGravityForce::SetStrength(float fStrength)
{
    m_kInputData.m_fStrength = fStrength;
}
//---------------------------------------------------------------------------
inline NiPSForceDefinitions::GravityData::ForceType
    NiPSGravityForce::GetForceType() const
{
    return m_kInputData.m_eForceType;
}
//---------------------------------------------------------------------------
inline void NiPSGravityForce::SetForceType(
    NiPSForceDefinitions::GravityData::ForceType eForceType)
{
    m_kInputData.m_eForceType = eForceType;
}
//---------------------------------------------------------------------------
inline float NiPSGravityForce::GetTurbulence() const
{
    return m_kInputData.m_fTurbulence;
}
//---------------------------------------------------------------------------
inline void NiPSGravityForce::SetTurbulence(float fTurbulence)
{
    m_kInputData.m_fTurbulence = fTurbulence;
}
//---------------------------------------------------------------------------
inline float NiPSGravityForce::GetTurbulenceScale() const
{
    return m_kInputData.m_fTurbulenceScale;
}
//---------------------------------------------------------------------------
inline void NiPSGravityForce::SetTurbulenceScale(float fTurbulenceScale)
{
    m_kInputData.m_fTurbulenceScale = fTurbulenceScale;
}
//---------------------------------------------------------------------------
