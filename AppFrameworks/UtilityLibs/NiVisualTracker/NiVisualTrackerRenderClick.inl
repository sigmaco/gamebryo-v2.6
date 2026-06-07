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
inline NiVisualTrackerRenderClick::NiVisualTrackerRenderClick() :
    m_uiNumObjectsDrawn(0), m_fRenderTime(0.0f)
{
}
//---------------------------------------------------------------------------
inline void NiVisualTrackerRenderClick::AppendVisualTracker(
    NiVisualTracker* pkVisualTracker)
{
    NIASSERT(pkVisualTracker);
    m_kVisualTrackers.AddTail(pkVisualTracker);
}
//---------------------------------------------------------------------------
inline void NiVisualTrackerRenderClick::PrependVisualTracker(
    NiVisualTracker* pkVisualTracker)
{
    NIASSERT(pkVisualTracker);
    m_kVisualTrackers.AddHead(pkVisualTracker);
}
//---------------------------------------------------------------------------
inline void NiVisualTrackerRenderClick::RemoveVisualTracker(
    NiVisualTracker* pkVisualTracker)
{
    NIASSERT(pkVisualTracker);
    m_kVisualTrackers.Remove(pkVisualTracker);
}
//---------------------------------------------------------------------------
inline void NiVisualTrackerRenderClick::RemoveAllVisualTrackers()
{
    m_kVisualTrackers.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiTPointerList<NiVisualTrackerPtr>&
    NiVisualTrackerRenderClick::GetVisualTrackers()
{
    return m_kVisualTrackers;
}
//---------------------------------------------------------------------------
inline const NiTPointerList<NiVisualTrackerPtr>&
    NiVisualTrackerRenderClick::GetVisualTrackers() const
{
    return m_kVisualTrackers;
}
//---------------------------------------------------------------------------
