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
inline NxShapeType NiPhysXShapeDesc::GetType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetType(const NxShapeType eType)
{
    m_eType = eType;
}
//---------------------------------------------------------------------------
inline NxShape* NiPhysXShapeDesc::GetShape() const
{
    return m_pkShape;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetShape(NxShape* pkShape)
{
    m_pkShape = pkShape;
}
//---------------------------------------------------------------------------
inline NxMat34 NiPhysXShapeDesc::GetLocalPose() const
{
    return m_kLocalPose;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetLocalPose(const NxMat34& kPose)
{
    m_kLocalPose = kPose;
}
//---------------------------------------------------------------------------
inline NxU32 NiPhysXShapeDesc::GetShapeFlags() const
{
    return m_uiShapeFlags;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetShapeFlags(const NxU32 uiShapeFlags)
{
    m_uiShapeFlags = uiShapeFlags;
}
//---------------------------------------------------------------------------
inline NxCollisionGroup NiPhysXShapeDesc::GetShapeGroup() const
{
    return m_uiShapeGroup;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetShapeGroup(
    const NxCollisionGroup uiShapeGroup)
{
    m_uiShapeGroup = uiShapeGroup;
}
//---------------------------------------------------------------------------
inline NxMaterialIndex NiPhysXShapeDesc::GetMaterialIndex() const
{
    return m_uiMaterialIndex;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetMaterialIndex(
    const NxMaterialIndex uiMaterialIndex)
{
    m_uiMaterialIndex = uiMaterialIndex;
}
//---------------------------------------------------------------------------
inline NxReal NiPhysXShapeDesc::GetDensity() const
{
    return m_fDensity;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetDensity(const NxReal fDensity)
{
    m_fDensity = fDensity;
}
//---------------------------------------------------------------------------
inline NxReal NiPhysXShapeDesc::GetMass() const
{
    return m_fMass;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetMass(const NxReal fMass)
{
    m_fMass = fMass;
}
//---------------------------------------------------------------------------
inline NxReal NiPhysXShapeDesc::GetSkinWidth() const
{
    return m_fSkinWidth;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetSkinWidth(const NxReal fSkinWidth)
{
    m_fSkinWidth = fSkinWidth;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPhysXShapeDesc::GetName() const
{
    return m_kShapeName;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetName(const NiFixedString& kShapeName)
{
    m_kShapeName = kShapeName;    
}
//---------------------------------------------------------------------------
inline NxU32 NiPhysXShapeDesc::GetNonInteractingComponentType() const
{
    return m_uiNonInteractingCompartmentTypes;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetNonInteractingComponentType(
    const NxU32 uiNonInteractingCompType)
{
    m_uiNonInteractingCompartmentTypes = uiNonInteractingCompType;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::GetCollisionBits(NxU32 auiCollisionBits[4]) const
{
    auiCollisionBits[0] = m_auiCollisionBits[0];
    auiCollisionBits[1] = m_auiCollisionBits[1];
    auiCollisionBits[2] = m_auiCollisionBits[2];
    auiCollisionBits[3] = m_auiCollisionBits[3];
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetCollisionBits(const NxU32 auiCollisionBits[4])
{
    m_auiCollisionBits[0] = auiCollisionBits[0];
    m_auiCollisionBits[1] = auiCollisionBits[1];
    m_auiCollisionBits[2] = auiCollisionBits[2];
    m_auiCollisionBits[3] = auiCollisionBits[3];
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::GetPlaneShapeData(NxVec3& kNormal, NxReal& kD)
    const
{
    kD = m_fVal1;
    kNormal = m_kPoint1;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetPlaneShapeData(const NxVec3& kNormal,
    const NxReal kD)
{
    m_fVal1 = kD;
    m_kPoint1 = kNormal;
}
//---------------------------------------------------------------------------
inline NxReal NiPhysXShapeDesc::GetSphereShapeRadius() const
{
    return m_fVal1;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetSphereShapeRadius(const NxReal kRadius)
{
    m_fVal1 = kRadius;
}
//---------------------------------------------------------------------------
inline NxVec3 NiPhysXShapeDesc::GetBoxShapeDimensions() const
{
    return m_kPoint1;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetBoxShapeDimensions(const NxVec3& kDimensions)
{
    m_kPoint1 = kDimensions;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::GetCapsuleShapeData(NxReal& fRadius,
    NxReal& fHeight, NxU32& uiFlags) const
{
    fRadius = m_fVal1;
    fHeight = m_fVal2;
    uiFlags = m_uiCapsuleFlags;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetCapsuleShapeData(const NxReal& fRadius,
    const NxReal& fHeight, const NxU32& uiFlags)
{
    m_fVal1 = fRadius;
    m_fVal2 = fHeight;
    m_uiCapsuleFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline void NiPhysXShapeDesc::SetMeshDesc(NiPhysXMeshDesc* pkMeshDesc)
{
    m_spMeshDesc = pkMeshDesc;
}
//---------------------------------------------------------------------------
inline NiPhysXMeshDescPtr NiPhysXShapeDesc::GetMeshDesc()
{
    return m_spMeshDesc;
}   
//---------------------------------------------------------------------------
inline bool NiPhysXShapeDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
