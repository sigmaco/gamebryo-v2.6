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

inline void NiCollisionData::MarkVerticesAsChanged()
{
    m_bWorldVerticesNeedUpdate = true;
}
//---------------------------------------------------------------------------
inline void NiCollisionData::MarkNormalsAsChanged()
{
    m_bWorldNormalsNeedUpdate = true;
}
//---------------------------------------------------------------------------
inline unsigned short NiCollisionData::GetTriangleCount()
{
    return m_usNumTriangles;
}
//---------------------------------------------------------------------------
inline const NiPoint3* NiCollisionData::GetWorldVertices() const
{
    return m_pkWorldVertex;
}
//---------------------------------------------------------------------------
inline const NiPoint3* NiCollisionData::GetWorldNormals() const
{
    return m_pkWorldNormal;
}
//---------------------------------------------------------------------------
inline void NiCollisionData::SetLocalVelocity(const NiPoint3& kLocalVelocity)
{
    m_kLocalVelocity = kLocalVelocity;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiCollisionData::GetLocalVelocity() const
{
    return m_kLocalVelocity;
}
//---------------------------------------------------------------------------
inline void NiCollisionData::SetWorldVelocity(const NiPoint3& kWorldVelocity)
{
    NIASSERT(ms_bComputeWorldVelocity == false);
    m_kWorldVelocity = kWorldVelocity;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiCollisionData::GetWorldVelocity() const
{
    return m_kWorldVelocity;
}
//---------------------------------------------------------------------------
inline void NiCollisionData::SetCollideCallback(NiCollisionGroup::Callback 
    pfnCollideCallback)
{
    m_pfnCollideCallback = pfnCollideCallback;
}
//---------------------------------------------------------------------------
inline void NiCollisionData::SetCollideCallbackData(void* 
    pvCollideCallbackData)
{
    m_pvCollideCallbackData = pvCollideCallbackData;
}
//---------------------------------------------------------------------------
inline NiCollisionGroup::Callback NiCollisionData::GetCollideCallback()
{
    return(NiCollisionGroup::Callback) m_pfnCollideCallback;
}
//---------------------------------------------------------------------------
inline void* NiCollisionData::GetCollideCallbackData()
{
    return m_pvCollideCallbackData;
}
//---------------------------------------------------------------------------
inline void NiCollisionData::SetPropagationMode(PropagationMode ePropagation)
{
    m_ePropagationMode = ePropagation;
}
//---------------------------------------------------------------------------
inline NiCollisionData::PropagationMode 
    NiCollisionData::GetPropagationMode() const
{
    return m_ePropagationMode;
}
//---------------------------------------------------------------------------
inline void NiCollisionData::SetCollisionMode(CollisionMode eCollision)
{
    m_eCollisionMode = eCollision;
    NIASSERT(NiCollisionData::ValidateForCollision(m_pkSceneObject,
        m_eCollisionMode));
}
//---------------------------------------------------------------------------
inline NiCollisionData::CollisionMode 
    NiCollisionData::GetCollisionMode() const
{
    return m_eCollisionMode;
}
//---------------------------------------------------------------------------
inline NiBoundingVolume* NiCollisionData::GetModelSpaceABV() const
{
    return m_pkModelABV;
}
//---------------------------------------------------------------------------
inline NiBoundingVolume* NiCollisionData::GetWorldSpaceABV() const
{
    return m_pkWorldABV;
}
//---------------------------------------------------------------------------
inline void NiCollisionData::SetEnableVelocity(bool bEnable)
{
    ms_bComputeWorldVelocity = bEnable;
}
//---------------------------------------------------------------------------
inline bool NiCollisionData::GetEnableVelocity()
{
    return ms_bComputeWorldVelocity;
}
//---------------------------------------------------------------------------
inline void NiCollisionData::SetEnableAuxCallbacks(bool bEnable)
{
    m_bAuxCallbacks = bEnable;
}
//---------------------------------------------------------------------------
inline bool NiCollisionData::GetEnableAuxCallbacks()
{
    return m_bAuxCallbacks;
}
//---------------------------------------------------------------------------
inline NiCollisionData::CollisionTest NiCollisionData::GetCollisionTestType(
    NiAVObject* pkObj1, NiAVObject* pkObj2) 
{
    // Look for collision data.
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    // Now we assume that we have valid collision data.
    NiCollisionData::CollisionMode eCM1 = pkCD1->GetCollisionMode();
    NiCollisionData::CollisionMode eCM2 = pkCD2->GetCollisionMode();

    if (eCM1 == NOTEST || eCM2 == NOTEST)
        return NOTEST_NOTEST;

    return (NiCollisionData::CollisionTest)(eCM1 | eCM2);
}
//---------------------------------------------------------------------------
