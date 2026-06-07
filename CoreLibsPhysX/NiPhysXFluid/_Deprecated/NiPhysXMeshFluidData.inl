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
inline NxParticleData& NiPhysXMeshFluidData::GetFluidData()
{
    return m_kFluidData;
}
//---------------------------------------------------------------------------
inline NxU32 NiPhysXMeshFluidData::GetMaxNumParticles()
{
    return m_uiMaxParticles;
}
//---------------------------------------------------------------------------
inline bool NiPhysXMeshFluidData::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------

