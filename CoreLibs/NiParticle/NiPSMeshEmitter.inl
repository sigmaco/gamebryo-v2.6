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
inline bool NiPSMeshEmitter::AddMeshEmitter(NiMesh* pkMeshEmitter)
{
    NIASSERT(pkMeshEmitter);

    if (ValidateMesh(pkMeshEmitter))
    {
        m_kMeshEmitters.Add(pkMeshEmitter);
        return true;
    }
    else
    {
        NILOG("NiPSMeshEmitter::AddMeshEmitter: Could not add unsupported "
            "NiMesh object named \"%s\".\n", (const char*)
            pkMeshEmitter->GetName());
    }

    return false;
}
//---------------------------------------------------------------------------
inline void NiPSMeshEmitter::RemoveMeshEmitter(NiUInt32 uiIndex)
{
    NIASSERT(uiIndex < m_kMeshEmitters.GetSize());
    m_kMeshEmitters.RemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiPSMeshEmitter::RemoveAllMeshEmitters()
{
    m_kMeshEmitters.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiMesh* NiPSMeshEmitter::GetMeshEmitter(NiUInt32 uiIndex)
{
    NIASSERT(uiIndex < m_kMeshEmitters.GetSize());
    return m_kMeshEmitters.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPSMeshEmitter::GetMeshEmitterCount()
{
    return m_kMeshEmitters.GetSize();
}
//---------------------------------------------------------------------------
inline NiPSMeshEmitter::MeshEmissionType NiPSMeshEmitter::GetMeshEmissionType()
{
    return m_eMeshEmissionType;
}
//---------------------------------------------------------------------------
inline void NiPSMeshEmitter::SetMeshEmissionType(
    NiPSMeshEmitter::MeshEmissionType eMeshEmissionType)
{
    m_eMeshEmissionType = eMeshEmissionType;
}
//---------------------------------------------------------------------------
inline NiPSMeshEmitter::InitialVelocityType 
    NiPSMeshEmitter::GetInitialVelocityType()
{
    return m_eInitialVelocityType;
}
//---------------------------------------------------------------------------
inline void NiPSMeshEmitter::SetInitialVelocityType(
    NiPSMeshEmitter::InitialVelocityType eInitialVelocityType)
{
    m_eInitialVelocityType = eInitialVelocityType;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPSMeshEmitter::GetEmitAxis()
{
    return m_kEmitAxis;
}
//---------------------------------------------------------------------------
inline void NiPSMeshEmitter::SetEmitAxis(const NiPoint3& kEmitAxis)
{
    m_kEmitAxis = kEmitAxis;
}
//---------------------------------------------------------------------------
