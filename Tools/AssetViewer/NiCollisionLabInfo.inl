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

// NiCollisionLabInfo.inl


//---------------------------------------------------------------------------
inline bool NiCollisionLabInfo::GetDynamicSim()
{
    return m_bDynamicSimOn;
}
//---------------------------------------------------------------------------
inline void NiCollisionLabInfo::SetDynamicSim(bool bValue)
{
    m_bDynamicSimOn = bValue;
}
//---------------------------------------------------------------------------
inline void NiCollisionLabInfo::SetRemainingDistance(float fValue)
{
    m_fRemainingDistance = fValue;
}
//---------------------------------------------------------------------------
inline float NiCollisionLabInfo::GetRemainingDistance()
{
    return m_fRemainingDistance;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiCollisionLabInfo::GetDirection()
{
    return m_kDirection;
}
//---------------------------------------------------------------------------
inline void NiCollisionLabInfo::SetDirection(NiPoint3& kDir)
{
    m_kDirection = kDir;
}
//---------------------------------------------------------------------------
inline bool NiCollisionLabInfo::GetCollisionState()
{
    return m_bCollided; 
}
//---------------------------------------------------------------------------
inline void NiCollisionLabInfo::SetCollisionState(bool bState)
{
    m_bCollided = bState;
}
//---------------------------------------------------------------------------
inline float NiCollisionLabInfo::GetVelocity()
{ 
    return m_fVelocity;
}
//---------------------------------------------------------------------------
inline void NiCollisionLabInfo::SetVelocity(float fValue)
{
    m_fVelocity = fValue; 
}
//---------------------------------------------------------------------------
inline void NiCollisionLabInfo::SetAuxCallbacks(bool bValue)
{
    m_bAuxCallbacks = bValue; 
}
//---------------------------------------------------------------------------
inline bool NiCollisionLabInfo::GetAuxCallbacks()
{
    return m_bAuxCallbacks; 
}
//---------------------------------------------------------------------------
inline float NiCollisionLabInfo::GetIntersectionTime()
{
    return m_kIntersectData.fTime;
}
//---------------------------------------------------------------------------

