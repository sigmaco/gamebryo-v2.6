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
inline NiTCBPosKey::NiTCBPosKey()
{
}
//---------------------------------------------------------------------------
inline NiTCBPosKey::NiTCBPosKey(float fTime, const NiPoint3& pos,
    float fTension, float fContinuity, float fBias)
    :
    NiPosKey(fTime,pos)
{
    m_fTension = fTension;
    m_fContinuity = fContinuity;
    m_fBias = fBias;
}
//---------------------------------------------------------------------------
inline void NiTCBPosKey::SetTension(float fTension)
{
    m_fTension = fTension;
}
//---------------------------------------------------------------------------
inline void NiTCBPosKey::SetContinuity(float fContinuity)
{
    m_fContinuity = fContinuity;
}
//---------------------------------------------------------------------------
inline void NiTCBPosKey::SetBias(float fBias)
{
    m_fBias = fBias;
}
//---------------------------------------------------------------------------
inline float NiTCBPosKey::GetTension() const
{
    return m_fTension;
}
//---------------------------------------------------------------------------
inline float NiTCBPosKey::GetContinuity() const
{
    return m_fContinuity;
}
//---------------------------------------------------------------------------
inline float NiTCBPosKey::GetBias() const
{
    return m_fBias;
}
//---------------------------------------------------------------------------
