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
inline const NiFixedString& NiPSForce::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiPSForce::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline NiPSForceDefinitions::ForceType NiPSForce::GetType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline bool NiPSForce::GetActive() const
{
    return m_bActive;
}
//---------------------------------------------------------------------------
inline void NiPSForce::SetActive(bool bActive)
{
    m_bActive = bActive;
}
//---------------------------------------------------------------------------
