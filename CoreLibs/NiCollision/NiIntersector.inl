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

inline NiIntersector::IntersectionType 
    NiIntersector::GetIntersectionType() const
{
    return m_eIntersectionType;
}
//----------------------------------------------------------------------------
inline float NiIntersector::GetContactTime() const
{
    return m_fContactTime;
}
//----------------------------------------------------------------------------
inline const NiPoint3& NiIntersector::GetContactPoint() const
{
    return m_kContactPoint;
}
//----------------------------------------------------------------------------
inline const NiPoint3& NiIntersector::GetContactNormal() const
{
    return m_kContactNormal;
}
//----------------------------------------------------------------------------
