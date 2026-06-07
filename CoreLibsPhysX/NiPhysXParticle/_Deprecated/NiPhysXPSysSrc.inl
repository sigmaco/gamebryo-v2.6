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
inline NiParticleSystem* NiPhysXPSysSrc::GetSource()
{
    return m_pkSource;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSysSrc::UpdateFromSceneGraph(const float, 
    const NiTransform&, const float, 
    const bool)
{
    // This function deliberately left blank
}
//---------------------------------------------------------------------------
inline void NiPhysXPSysSrc::UpdateToActors(const float, 
    const float)
{
    // This function deliberately left blank
}
//---------------------------------------------------------------------------
