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
inline float NiPSysRotationModifier::GetInitialRotSpeed() const
{
    return m_fInitialRotSpeed;
}
//---------------------------------------------------------------------------
inline float NiPSysRotationModifier::GetInitialRotSpeedVar() const
{
    return m_fInitialRotSpeedVar;
}
//---------------------------------------------------------------------------
inline bool NiPSysRotationModifier::GetRandomRotSpeedSign() const
{
    return m_bRandomRotSpeedSign;
}
//---------------------------------------------------------------------------
inline float NiPSysRotationModifier::GetInitialRotAngle() const
{
    return m_fInitialRotAngle;
}
//---------------------------------------------------------------------------
inline float NiPSysRotationModifier::GetInitialRotAngleVar() const
{
    return m_fInitialRotAngleVar;
}
//---------------------------------------------------------------------------
inline bool NiPSysRotationModifier::GetRandomInitialAxis() const
{
    return m_bRandomInitialAxis;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPSysRotationModifier::GetInitialAxis() const
{
    return m_kInitialAxis;
}
//---------------------------------------------------------------------------
