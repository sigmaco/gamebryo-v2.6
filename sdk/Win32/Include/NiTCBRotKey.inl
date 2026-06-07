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
inline NiTCBRotKey::NiTCBRotKey()
{
}
//---------------------------------------------------------------------------
inline NiTCBRotKey::NiTCBRotKey(float fTime, float fAngle,
    const NiPoint3& axis)
    :
    NiRotKey(fTime,fAngle,axis)
{
}
//---------------------------------------------------------------------------
inline NiTCBRotKey::NiTCBRotKey(float fTime, const NiQuaternion& quat)
    :
    NiRotKey(fTime,quat)
{
}
//---------------------------------------------------------------------------
inline void NiTCBRotKey::SetTension(float fTension)
{
    m_fTension = fTension;
}
//---------------------------------------------------------------------------
inline void NiTCBRotKey::SetContinuity(float fContinuity)
{
    m_fContinuity = fContinuity;
}
//---------------------------------------------------------------------------
inline void NiTCBRotKey::SetBias(float fBias)
{
    m_fBias = fBias;
}
//---------------------------------------------------------------------------
inline float NiTCBRotKey::GetTension() const
{
    return m_fTension;
}
//---------------------------------------------------------------------------
inline float NiTCBRotKey::GetContinuity() const
{
    return m_fContinuity;
}
//---------------------------------------------------------------------------
inline float NiTCBRotKey::GetBias() const
{
    return m_fBias;
}
//---------------------------------------------------------------------------
