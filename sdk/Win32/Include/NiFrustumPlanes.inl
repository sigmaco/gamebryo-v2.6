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
inline NiFrustumPlanes::NiFrustumPlanes()
{
    m_uiActivePlanes = ALL_PLANES_SET;
}
//---------------------------------------------------------------------------
inline NiFrustumPlanes::NiFrustumPlanes(const NiCamera& kCam)
{
    m_uiActivePlanes = ALL_PLANES_SET;
    Set(kCam);
}
//---------------------------------------------------------------------------
inline NiFrustumPlanes::NiFrustumPlanes(const NiFrustum& kFrust, 
    const NiTransform& kXform)
{
    m_uiActivePlanes = ALL_PLANES_SET;
    Set(kFrust, kXform);
}
//---------------------------------------------------------------------------
inline const NiPlane& NiFrustumPlanes::GetPlane(unsigned int ePlane) const
{
    return m_akCullingPlanes[ePlane];
}
//---------------------------------------------------------------------------
inline void NiFrustumPlanes::SetPlane(unsigned int ePlane, 
    const NiPlane& kPlane)
{
    m_akCullingPlanes[ePlane] = kPlane;
}
//---------------------------------------------------------------------------
inline bool NiFrustumPlanes::IsPlaneActive(unsigned int ePlane) const
{
    return (m_uiActivePlanes & (1 << ePlane)) ? true : false;
}
//---------------------------------------------------------------------------
inline bool NiFrustumPlanes::IsAnyPlaneActive() const
{
    return m_uiActivePlanes ? true : false;
}
//---------------------------------------------------------------------------
inline bool NiFrustumPlanes::AreAllPlanesActive() const
{
    return (m_uiActivePlanes == ALL_PLANES_SET) ? true : false;
}
//---------------------------------------------------------------------------
inline void NiFrustumPlanes::EnablePlane(unsigned int ePlane)
{
    m_uiActivePlanes = m_uiActivePlanes | (1 << ePlane);
}
//---------------------------------------------------------------------------
inline void NiFrustumPlanes::DisablePlane(unsigned int ePlane)
{
    m_uiActivePlanes = m_uiActivePlanes & ~(1 << ePlane);
}
//---------------------------------------------------------------------------
inline void NiFrustumPlanes::EnableAllPlanes()
{
    m_uiActivePlanes = ALL_PLANES_SET;
}
//---------------------------------------------------------------------------
inline void NiFrustumPlanes::DisableAllPlanes()
{
    m_uiActivePlanes = 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiFrustumPlanes::GetActivePlaneState() const
{
    return m_uiActivePlanes;
}
//---------------------------------------------------------------------------
inline void NiFrustumPlanes::SetActivePlaneState(unsigned int uiState)
{
    m_uiActivePlanes = uiState;
}
//---------------------------------------------------------------------------
