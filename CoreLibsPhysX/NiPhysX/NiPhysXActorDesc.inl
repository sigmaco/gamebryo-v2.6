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
inline NiUInt32 NiPhysXActorDesc::GetNumPoses() const
{
    return m_uiNumPoses;
}
//---------------------------------------------------------------------------
inline NxMat34 NiPhysXActorDesc::GetPose(const NiUInt32 uiIndex) const
{
    return m_pkPoses[uiIndex];
}
//---------------------------------------------------------------------------
inline NiFixedString NiPhysXActorDesc::GetActorName() const
{
    return m_kActorName;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetActorName(NiFixedString& kActorName)
{
    m_kActorName = kActorName;
}
//---------------------------------------------------------------------------
inline NxReal NiPhysXActorDesc::GetDensity() const
{
    return m_fDensity;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetDensity(const NxReal fDensity)
{
    m_fDensity = fDensity;
}
//---------------------------------------------------------------------------
inline NxU32 NiPhysXActorDesc::GetActorFlags() const
{
    return m_uiActorFlags;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetActorFlags(const NxU32 uiActorFlags)
{
    m_uiActorFlags = uiActorFlags;
}
//---------------------------------------------------------------------------
inline NxActorGroup NiPhysXActorDesc::GetActorGroup() const
{
    return m_uiActorGroup;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetActorGroup(const NxActorGroup uiActorGroup)
{
    m_uiActorGroup = uiActorGroup;
}
//---------------------------------------------------------------------------
inline NiPhysXBodyDesc* NiPhysXActorDesc::GetBodyDesc()
{
    return m_spBodyDesc;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetBodyDesc(NiPhysXBodyDesc* pkBodyDesc)
{
    m_spBodyDesc = pkBodyDesc;
}
//---------------------------------------------------------------------------
inline NiTObjectArray<NiPhysXShapeDescPtr>& NiPhysXActorDesc::GetActorShapes()
{
    return m_kActorShapes;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetActorShapes(
    const NiTObjectArray<NiPhysXShapeDescPtr>& kActorShapes)
{
    m_kActorShapes.RemoveAll();
    for (NiUInt32 ui = 0; ui < kActorShapes.GetSize(); ui++)
        m_kActorShapes.Add(kActorShapes.GetAt(ui));
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetDominanceGroup(
    NxDominanceGroup uiDominanceGroup)
{
    m_uiDominanceGroup = uiDominanceGroup;
}
//---------------------------------------------------------------------------
inline NxDominanceGroup NiPhysXActorDesc::GetDominanceGroup()
{
    return m_uiDominanceGroup;
}
//---------------------------------------------------------------------------
inline NxU32 NiPhysXActorDesc::GetContactReportFlags()
{
    return m_uiContactReportFlags;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetContactReportFlags(NxU32 uiContactReportFlags)
{
    m_uiContactReportFlags = uiContactReportFlags;
}
//---------------------------------------------------------------------------
inline NxU16 NiPhysXActorDesc::GetForceFieldMaterial()
{
    return m_uiForceFieldMaterial;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetForceFieldMaterial(NxU16 uiForceFieldMaterial)
{
    m_uiForceFieldMaterial = uiForceFieldMaterial;
}
//---------------------------------------------------------------------------
inline NxActor* NiPhysXActorDesc::GetActor() const
{
    return m_pkActor;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetActor(NxActor* pkActor)
{
    m_pkActor = pkActor;
}
//---------------------------------------------------------------------------
inline NiPhysXActorDesc* NiPhysXActorDesc::GetActorParent() const
{
    return m_pkActorParent;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetActorParent(NiPhysXActorDesc* pkActorDesc)
{
    m_pkActorParent = pkActorDesc;
}
//---------------------------------------------------------------------------
inline NiPhysXRigidBodySrcPtr NiPhysXActorDesc::GetSource() const
{
    return m_spSource;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetSource(NiPhysXRigidBodySrcPtr spSource)
{
    m_spSource = spSource;
}
//---------------------------------------------------------------------------
inline NiPhysXRigidBodyDestPtr NiPhysXActorDesc::GetDest() const
{
    return m_spDest;
}
//---------------------------------------------------------------------------
inline void NiPhysXActorDesc::SetDest(NiPhysXRigidBodyDestPtr spDest)
{
    m_spDest = spDest;
}
//---------------------------------------------------------------------------
inline bool NiPhysXActorDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
