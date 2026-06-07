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
inline NiAVObject* NiPSSphericalCollider::GetColliderObj() const
{
    return m_pkColliderObj;
}
//---------------------------------------------------------------------------
inline void NiPSSphericalCollider::SetColliderObj(NiAVObject* pkColliderObj)
{
    m_pkColliderObj = pkColliderObj;
}
//---------------------------------------------------------------------------
inline float NiPSSphericalCollider::GetRadius() const
{
    return m_kInputData.m_fRadius;
}
//---------------------------------------------------------------------------
inline void NiPSSphericalCollider::SetRadius(float fRadius)
{
    if (fRadius >= 0.0f)
    {
        m_kInputData.m_fRadius = fRadius;
    }
}
//---------------------------------------------------------------------------
