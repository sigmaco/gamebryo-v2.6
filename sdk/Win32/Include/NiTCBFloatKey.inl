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
inline NiTCBFloatKey::NiTCBFloatKey()
{
}
//---------------------------------------------------------------------------
inline NiTCBFloatKey::NiTCBFloatKey(float fTime, float fValue,
    float fTension, float fContinuity, float fBias)
    :
    NiFloatKey(fTime,fValue)
{
    m_fTension = fTension;
    m_fContinuity = fContinuity;
    m_fBias = fBias;
}
//---------------------------------------------------------------------------
inline void NiTCBFloatKey::SetTension(float fTension)
{
    m_fTension = fTension;
}
//---------------------------------------------------------------------------
inline void NiTCBFloatKey::SetContinuity(float fContinuity)
{
    m_fContinuity = fContinuity;
}
//---------------------------------------------------------------------------
inline void NiTCBFloatKey::SetBias(float fBias)
{
    m_fBias = fBias;
}
//---------------------------------------------------------------------------
inline float NiTCBFloatKey::GetTension() const
{
    return m_fTension;
}
//---------------------------------------------------------------------------
inline float NiTCBFloatKey::GetContinuity() const
{
    return m_fContinuity;
}
//---------------------------------------------------------------------------
inline float NiTCBFloatKey::GetBias() const
{
    return m_fBias;
}
//---------------------------------------------------------------------------
