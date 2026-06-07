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
inline NiAVObject* NiPSysDragModifier::GetDragObj() const
{
    return m_pkDragObj;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPSysDragModifier::GetDragAxis() const
{
    return m_kDragAxis;
}
//---------------------------------------------------------------------------
inline float NiPSysDragModifier::GetPercentage() const
{
    return m_fPercentage;
}
//---------------------------------------------------------------------------
inline float NiPSysDragModifier::GetRange() const
{
    return m_fRange;
}
//---------------------------------------------------------------------------
inline float NiPSysDragModifier::GetRangeFalloff() const
{
    return m_fFalloff;
}
//---------------------------------------------------------------------------
