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
inline NiRoom::WallArray& NiRoom::GetWallArray()
{
    return m_kWalls;
}
//---------------------------------------------------------------------------
inline const NiPortalList& NiRoom::GetOutgoingPortalList() const
{
    return m_kOutPortal;
}
//---------------------------------------------------------------------------
inline const NiPortalList& NiRoom::GetIncomingPortalList() const
{
    return m_kInPortal;
}
//---------------------------------------------------------------------------
inline const NiAVObjectList& NiRoom::GetFixtureList() const
{
    return m_kFixture;
}
//---------------------------------------------------------------------------
inline unsigned int NiRoom::GetLastRenderedTimestamp() const
{
    return m_uiLastRenderedTimestamp;
}
//---------------------------------------------------------------------------
inline void NiRoom::SetCurrentTimestamp(unsigned int uiTimestamp)
{
    ms_uiCurrentTimestamp = uiTimestamp;
}
//---------------------------------------------------------------------------
inline unsigned int NiRoom::GetCurrentTimestamp()
{
    return ms_uiCurrentTimestamp;
}
//---------------------------------------------------------------------------
