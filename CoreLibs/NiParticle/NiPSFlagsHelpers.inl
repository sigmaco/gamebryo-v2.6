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
inline NiUInt32 NiPSFlagsHelpers::GetField(
    NiUInt32 uiFlags,
    NiUInt32 uiMask,
    NiUInt32 uiPos)
{
    return (uiFlags & uiMask) >> uiPos;
}
//---------------------------------------------------------------------------
inline void NiPSFlagsHelpers::SetField(
    NiUInt32& uiFlags,
    NiUInt32 uiVal,
    NiUInt32 uiMask,
    NiUInt32 uiPos)
{
    uiFlags = (uiFlags & ~uiMask) | ((uiVal << uiPos) & uiMask);
}
//---------------------------------------------------------------------------
inline bool NiPSFlagsHelpers::GetBit(NiUInt32 uiFlags, NiUInt32 uiMask)
{
    return (uiFlags & uiMask) != 0;
}
//---------------------------------------------------------------------------
inline void NiPSFlagsHelpers::SetBit(
    NiUInt32& uiFlags,
    bool bVal,
    NiUInt32 uiMask)
{
    if (bVal)
    {
        uiFlags |= uiMask;
    }
    else
    {
        uiFlags &= ~uiMask;
    }
}
//---------------------------------------------------------------------------
inline NiUInt16 NiPSFlagsHelpers::GetGeneration(NiUInt32 uiFlags)
{
    return (NiUInt16) GetField(uiFlags, GENERATION_MASK, GENERATION_POS);
}
//---------------------------------------------------------------------------
inline void NiPSFlagsHelpers::SetGeneration(
    NiUInt32& uiFlags,
    NiUInt16 usGeneration)
{
    SetField(uiFlags, usGeneration, GENERATION_MASK, GENERATION_POS);
}
//---------------------------------------------------------------------------
inline NiUInt16 NiPSFlagsHelpers::GetSpawnerID(NiUInt32 uiFlags)
{
    return (NiUInt16) GetField(uiFlags, SPAWNER_ID_MASK, SPAWNER_ID_POS);
}
//---------------------------------------------------------------------------
inline void NiPSFlagsHelpers::SetSpawnerID(
    NiUInt32& uiFlags,
    NiUInt16 ucSpawnerID)
{
    SetField(uiFlags, ucSpawnerID, SPAWNER_ID_MASK, SPAWNER_ID_POS);
}
//---------------------------------------------------------------------------
inline bool NiPSFlagsHelpers::GetShouldDie(NiUInt32 uiFlags)
{
    return GetBit(uiFlags, SHOULD_DIE_MASK);
}
//---------------------------------------------------------------------------
inline void NiPSFlagsHelpers::SetShouldDie(
    NiUInt32& uiFlags,
    bool bShouldDie)
{
    SetBit(uiFlags, bShouldDie, SHOULD_DIE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPSFlagsHelpers::GetShouldSpawn(NiUInt32 uiFlags)
{
    return GetBit(uiFlags, SHOULD_SPAWN_MASK);
}
//---------------------------------------------------------------------------
inline void NiPSFlagsHelpers::SetShouldSpawn(
    NiUInt32& uiFlags,
    bool bShouldSpawn)
{
    SetBit(uiFlags, bShouldSpawn, SHOULD_SPAWN_MASK);
}
//---------------------------------------------------------------------------
