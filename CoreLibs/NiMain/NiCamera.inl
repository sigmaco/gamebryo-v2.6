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
inline const NiPoint3& NiCamera::GetWorldLocation() const
{
    return m_kWorld.m_Translate;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiCamera::GetWorldDirection() const
{
    NiPoint3 kDVector;
    m_kWorld.m_Rotate.GetCol(0, kDVector);
    return kDVector;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiCamera::GetWorldUpVector() const
{
    NiPoint3 kUVector;
    m_kWorld.m_Rotate.GetCol(1, kUVector);
    return kUVector;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiCamera::GetWorldRightVector() const
{
    NiPoint3 kRVector;
    m_kWorld.m_Rotate.GetCol(2, kRVector);
    return kRVector;
}
//---------------------------------------------------------------------------
inline const float* NiCamera::GetWorldToCameraMatrix() const
{
    return &m_aafWorldToCam[0][0];
}
//---------------------------------------------------------------------------
inline const NiFrustum& NiCamera::GetViewFrustum() const
{
    return m_kViewFrustum;
}
//---------------------------------------------------------------------------
inline void NiCamera::SetMinNearPlaneDist(float fMin)
{
    m_fMinNearPlaneDist = fMin;
}
//---------------------------------------------------------------------------
inline float NiCamera::GetMinNearPlaneDist() const
{
    return m_fMinNearPlaneDist;
}
//---------------------------------------------------------------------------
inline void NiCamera::SetMaxFarNearRatio(float fMax)
{
    m_fMaxFarNearRatio = fMax;
}
//---------------------------------------------------------------------------
inline float NiCamera::GetMaxFarNearRatio() const
{
    return m_fMaxFarNearRatio; 
}
//---------------------------------------------------------------------------
inline void NiCamera::SetViewPort(const NiRect<float>& kPort)
{
    m_kPort = kPort;
}
//---------------------------------------------------------------------------
inline const NiRect<float>& NiCamera::GetViewPort() const
{
    return m_kPort;
}
//---------------------------------------------------------------------------
inline void NiCamera::SetLODAdjust(float fLODAdjust)
{
    m_fLODAdjust = fLODAdjust;
}
//---------------------------------------------------------------------------
inline float NiCamera::GetLODAdjust() const
{
    return m_fLODAdjust;
}
//---------------------------------------------------------------------------
