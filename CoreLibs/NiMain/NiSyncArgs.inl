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
inline NiUInt32 NiSyncArgs::GetTaskGroupID(SyncPoint eSubmit,
    SyncPoint eComplete)
{
    return (eSubmit << 16) | eComplete;
}
//---------------------------------------------------------------------------
inline void NiSyncArgs::GetTaskGroupID(NiUInt32& uiTaskGroupID,
    NiUInt32& uiTaskGroupMask)
{
    uiTaskGroupID = (m_uiSubmitPoint << 16) | m_uiCompletePoint;

    uiTaskGroupMask = 0xFFFFFFFF;
    if (m_uiSubmitPoint == SYNC_ANY)
        uiTaskGroupMask ^= 0xFFFF0000;
    if (m_uiCompletePoint == SYNC_ANY)
        uiTaskGroupMask ^= 0x0000FFFF;
}
//---------------------------------------------------------------------------
