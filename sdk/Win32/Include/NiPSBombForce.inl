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
inline NiAVObject* NiPSBombForce::GetBombObj() const
{
    return m_pkBombObj;
}
//---------------------------------------------------------------------------
inline void NiPSBombForce::SetBombObj(NiAVObject* pkBombObj)
{
    m_pkBombObj = pkBombObj;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPSBombForce::GetBombAxis() const
{
    return m_kInputData.m_kBombAxis;
}
//---------------------------------------------------------------------------
inline void NiPSBombForce::SetBombAxis(const NiPoint3& kBombAxis)
{
    m_kInputData.m_kBombAxis = kBombAxis;
}
//---------------------------------------------------------------------------
inline float NiPSBombForce::GetDecay() const
{
    return m_kInputData.m_fDecay;
}
//---------------------------------------------------------------------------
inline void NiPSBombForce::SetDecay(float fDecay)
{
    m_kInputData.m_fDecay = fDecay;
}
//---------------------------------------------------------------------------
inline float NiPSBombForce::GetDeltaV() const
{
    return m_kInputData.m_fDeltaV;
}
//---------------------------------------------------------------------------
inline void NiPSBombForce::SetDeltaV(float fDeltaV)
{
    m_kInputData.m_fDeltaV = fDeltaV;
}
//---------------------------------------------------------------------------
inline NiPSForceDefinitions::BombData::DecayType NiPSBombForce::GetDecayType()
    const
{
    return m_kInputData.m_eDecayType;
}
//---------------------------------------------------------------------------
inline void NiPSBombForce::SetDecayType(
    NiPSForceDefinitions::BombData::DecayType eDecayType)
{
    m_kInputData.m_eDecayType = eDecayType;
}
//---------------------------------------------------------------------------
inline NiPSForceDefinitions::BombData::SymmType NiPSBombForce::GetSymmType()
    const
{
    return m_kInputData.m_eSymmType;
}
//---------------------------------------------------------------------------
inline void NiPSBombForce::SetSymmType(
    NiPSForceDefinitions::BombData::SymmType eSymmType)
{
    m_kInputData.m_eSymmType = eSymmType;
}
//---------------------------------------------------------------------------
