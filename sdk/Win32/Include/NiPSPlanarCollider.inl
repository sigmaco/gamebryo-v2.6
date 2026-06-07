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
inline NiAVObject* NiPSPlanarCollider::GetColliderObj() const
{
    return m_pkColliderObj;
}
//---------------------------------------------------------------------------
inline void NiPSPlanarCollider::SetColliderObj(NiAVObject* pkColliderObj)
{
    m_pkColliderObj = pkColliderObj;
}
//---------------------------------------------------------------------------
inline float NiPSPlanarCollider::GetWidth() const
{
    return m_kInputData.m_fWidth;
}
//---------------------------------------------------------------------------
inline void NiPSPlanarCollider::SetWidth(float fWidth)
{
    m_kInputData.m_fWidth = fWidth;
}
//---------------------------------------------------------------------------
inline float NiPSPlanarCollider::GetHeight() const
{
    return m_kInputData.m_fHeight;
}
//---------------------------------------------------------------------------
inline void NiPSPlanarCollider::SetHeight(float fHeight)
{
    m_kInputData.m_fHeight = fHeight;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPSPlanarCollider::GetXAxis() const
{
    return m_kInputData.m_kXAxis;
}
//---------------------------------------------------------------------------
inline void NiPSPlanarCollider::SetXAxis(const NiPoint3& kXAxis)
{
    m_kInputData.m_kXAxis = kXAxis;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPSPlanarCollider::GetYAxis() const
{
    return m_kInputData.m_kYAxis;
}
//---------------------------------------------------------------------------
inline void NiPSPlanarCollider::SetYAxis(const NiPoint3& kYAxis)
{
    m_kInputData.m_kYAxis = kYAxis;
}
//---------------------------------------------------------------------------
