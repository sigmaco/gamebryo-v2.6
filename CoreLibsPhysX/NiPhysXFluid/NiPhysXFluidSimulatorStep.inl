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
inline void NiPhysXFluidSimulatorStep::SetUpdateNecessary(
    const bool bUpdateNecessary)
{
    m_bUpdateNecessary = bUpdateNecessary;
}
//---------------------------------------------------------------------------
inline bool NiPhysXFluidSimulatorStep::GetUpdateNecessary() const
{
    return m_bUpdateNecessary;
}
//---------------------------------------------------------------------------
inline void NiPhysXFluidSimulatorStep::SetUpdateUseful(
    const bool bUpdateUseful)
{
    m_bUpdateUseful = bUpdateUseful;
}
//---------------------------------------------------------------------------
inline bool NiPhysXFluidSimulatorStep::GetUpdateUseful() const
{
    return m_bUpdateUseful;
}
//---------------------------------------------------------------------------
inline bool NiPhysXFluidSimulatorStep::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------

