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
inline NiAVObject* NiPSDragForce::GetDragObj() const
{
    return m_pkDragObj;
}
//---------------------------------------------------------------------------
inline void NiPSDragForce::SetDragObj(NiAVObject* pkDragObj)
{
    m_pkDragObj = pkDragObj;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPSDragForce::GetDragAxis() const
{
    return m_kInputData.m_kDragAxis;
}
//---------------------------------------------------------------------------
inline void NiPSDragForce::SetDragAxis(const NiPoint3& kDragAxis)
{
    m_kInputData.m_kDragAxis = kDragAxis;
}
//---------------------------------------------------------------------------
inline float NiPSDragForce::GetPercentage() const
{
    return m_kInputData.m_fPercentage;
}
//---------------------------------------------------------------------------
inline void NiPSDragForce::SetPercentage(float fPercentage)
{
    m_kInputData.m_fPercentage = fPercentage;
}
//---------------------------------------------------------------------------
inline float NiPSDragForce::GetRange() const
{
    return m_kInputData.m_fRange;
}
//---------------------------------------------------------------------------
inline void NiPSDragForce::SetRange(float fRange)
{
    m_kInputData.m_fRange = fRange;
}
//---------------------------------------------------------------------------
inline float NiPSDragForce::GetRangeFalloff() const
{
    return m_kInputData.m_fRangeFalloff;
}
//---------------------------------------------------------------------------
inline void NiPSDragForce::SetRangeFalloff(float fRangeFalloff)
{
    m_kInputData.m_fRangeFalloff = fRangeFalloff;
}
//---------------------------------------------------------------------------
