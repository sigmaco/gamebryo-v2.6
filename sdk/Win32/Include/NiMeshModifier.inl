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
inline void NiMeshModifier::AddSubmitSyncPoint(
    const NiSyncArgs::SyncPoint uiSyncPt)
{
    m_kSubmitPoints.AddUnique(uiSyncPt);
}
//---------------------------------------------------------------------------
inline void NiMeshModifier::RemoveSubmitSyncPoint(
    const NiSyncArgs::SyncPoint uiSyncPt)
{
    NiInt32 iFoundAt = m_kSubmitPoints.Find(uiSyncPt);
    
    if (iFoundAt != -1)
        m_kSubmitPoints.RemoveAt(iFoundAt);
}
//---------------------------------------------------------------------------
inline bool NiMeshModifier::SupportsSubmitSyncPoint(
    const NiSyncArgs::SyncPoint uiSyncPt) const
{
    if (uiSyncPt == NiSyncArgs::SYNC_ANY)
        return m_kSubmitPoints.GetSize() > 0;
        
    return m_kSubmitPoints.Find(uiSyncPt) != -1;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshModifier::GetSubmitSyncPointCount() const
{
    return m_kSubmitPoints.GetSize();
}
//---------------------------------------------------------------------------
inline NiSyncArgs::SyncPoint NiMeshModifier::GetSubmitSyncPoint(
    const NiUInt32 uiIndex) const
{
    return m_kSubmitPoints.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiMeshModifier::AddCompleteSyncPoint(
    const NiSyncArgs::SyncPoint uiSyncPt)
{
    m_kCompletePoints.AddUnique(uiSyncPt);
}
//---------------------------------------------------------------------------
inline void NiMeshModifier::RemoveCompleteSyncPoint(
    const NiSyncArgs::SyncPoint uiSyncPt)
{
    NiInt32 iFoundAt = m_kSubmitPoints.Find(uiSyncPt);
    
    if (iFoundAt != -1)
        m_kCompletePoints.RemoveAt(iFoundAt);
}
//---------------------------------------------------------------------------
inline bool NiMeshModifier::SupportsCompleteSyncPoint(
    const NiSyncArgs::SyncPoint uiSyncPt) const
{
    if (uiSyncPt == NiSyncArgs::SYNC_ANY)
        return m_kCompletePoints.GetSize() > 0;
        
    return m_kCompletePoints.Find(uiSyncPt) != -1;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMeshModifier::GetCompleteSyncPointCount() const
{
    return m_kCompletePoints.GetSize();
}
//---------------------------------------------------------------------------
inline NiSyncArgs::SyncPoint NiMeshModifier::GetCompleteSyncPoint(
    const NiUInt32 uiIndex) const
{
    return m_kCompletePoints.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiMeshModifier::SetSelectiveUpdateFlags(bool& bSelectiveUpdate,
    bool& bSelectiveUpdateTransforms, bool& bRigid)
{
    bSelectiveUpdate = false;
    bSelectiveUpdateTransforms = false;
    bRigid = true;
}
//---------------------------------------------------------------------------
