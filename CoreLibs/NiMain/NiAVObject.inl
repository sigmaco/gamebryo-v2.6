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
//  NiAVObject inline functions

//---------------------------------------------------------------------------
inline void NiAVObject::DetachParent()
{
    m_pkParent = 0;
}
//---------------------------------------------------------------------------
inline NiNode* NiAVObject::GetParent()
{
    return m_pkParent;
}
//---------------------------------------------------------------------------
inline const NiNode* NiAVObject::GetParent() const
{
    return m_pkParent;
}
//---------------------------------------------------------------------------
inline bool NiAVObject::GetAppCulled() const
{
    return GetBit(APP_CULLED_MASK);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetAppCulled(bool bAppCulled)
{
    SetBit(bAppCulled, APP_CULLED_MASK);
}
//---------------------------------------------------------------------------
inline bool NiAVObject::GetSelectiveUpdate() const
{
    return GetBit(SELECTIVE_UPDATE_MASK);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetSelectiveUpdate(bool bUpdate)
{
    SetBit(bUpdate, SELECTIVE_UPDATE_MASK);    
}
//---------------------------------------------------------------------------
inline bool NiAVObject::GetSelectiveUpdateTransforms() const
{
    return GetBit(SELECTIVE_XFORMS_MASK);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetSelectiveUpdateTransforms(bool bUpdate)
{
    SetBit(bUpdate, SELECTIVE_XFORMS_MASK);
}
//---------------------------------------------------------------------------
inline bool NiAVObject::GetSelectiveUpdatePropertyControllers() const
{
    return GetBit(SELECTIVE_PROP_CONTROLLER_MASK);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetSelectiveUpdatePropertyControllers(bool bUpdate)
{
    SetBit(bUpdate, SELECTIVE_PROP_CONTROLLER_MASK);
}
//---------------------------------------------------------------------------
inline bool NiAVObject::GetSelectiveUpdateRigid() const
{
    return GetBit(SELECTIVE_RIGID_MASK);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetSelectiveUpdateRigid(bool bRigid)
{
    SetBit(bRigid, SELECTIVE_RIGID_MASK);
}
//---------------------------------------------------------------------------
inline bool NiAVObject::GetSelectiveUpdateTransformsOverride() const
{
    return GetBit(SELECTIVE_XFORMS_OVERRIDE_MASK);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetSelectiveUpdateTransformsOverride(bool bOverride)
{
    SetBit(bOverride, SELECTIVE_XFORMS_OVERRIDE_MASK);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetTranslate(const NiPoint3& kTrn)
{
    m_kLocal.m_Translate = kTrn;
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetTranslate(float x, float y, float z)
{
    m_kLocal.m_Translate = NiPoint3(x,y,z);
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiAVObject::GetTranslate() const
{
    return m_kLocal.m_Translate;
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetRotate(const NiMatrix3& kRot)
{
    m_kLocal.m_Rotate = kRot;
}
//---------------------------------------------------------------------------
inline const NiMatrix3& NiAVObject::GetRotate() const
{
    return m_kLocal.m_Rotate;
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetRotate(float fAngle, float x, float y, float z)
{
    m_kLocal.m_Rotate.MakeRotation(fAngle, x, y, z);
}
//---------------------------------------------------------------------------
inline void NiAVObject::GetRotate(float& fAngle, float& x, float& y, 
    float& z) const
{
    m_kLocal.m_Rotate.ExtractAngleAndAxis(fAngle, x, y, z);
}
//---------------------------------------------------------------------------
inline void NiAVObject::GetRotate(NiQuaternion &kQuat) const
{
    kQuat.FromRotation(m_kLocal.m_Rotate);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetRotate(const NiQuaternion &kQuat)
{
    kQuat.ToRotation(m_kLocal.m_Rotate);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetRotateAboutCenter(const NiMatrix3& kRot,
    const NiPoint3& kCen, const NiPoint3 &kTrans)
{
    m_kLocal.m_Rotate = kRot;
    m_kLocal.m_Translate = kTrans+kCen-kRot*kCen;
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetScale(float fScale)
{
    // We do not support negative scales.  The assert lets you know in debug
    // mode that the negative scale was passed.  However, to avoid a crash
    // in release mode(negative scale does screw up bounding spheres), we'll
    // just go ahead and change sign on the scale.
    NIASSERT( fScale >= 0.0f );
    m_kLocal.m_fScale = NiAbs(fScale);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetWorldTranslate(const NiPoint3& kTrn)
{
    m_kWorld.m_Translate = kTrn;
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetWorldRotate(const NiMatrix3& kRot)
{
    m_kWorld.m_Rotate = kRot;
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetWorldScale(float fScale)
{
    m_kWorld.m_fScale = fScale;
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetWorldBound(const NiBound& kBound)
{
    m_kWorldBound = kBound;
}
//---------------------------------------------------------------------------
inline float NiAVObject::GetScale() const
{
    return m_kLocal.m_fScale;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiAVObject::GetWorldTranslate() const
{
    return m_kWorld.m_Translate;
}
//---------------------------------------------------------------------------
inline const NiMatrix3& NiAVObject::GetWorldRotate() const
{
    return m_kWorld.m_Rotate;
}
//---------------------------------------------------------------------------
inline float NiAVObject::GetWorldScale() const
{
    return m_kWorld.m_fScale;
}
//---------------------------------------------------------------------------
inline const NiTransform& NiAVObject::GetWorldTransform() const
{
    return m_kWorld;
}
//---------------------------------------------------------------------------
inline const NiTransform& NiAVObject::GetLocalTransform() const
{
    return m_kLocal;
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetLocalTransform(const NiTransform& kTransform)
{
    m_kLocal = kTransform;

    // We do not support negative scales.  The assert lets you know in debug
    // mode that the negative scale was passed.  However, to avoid a crash
    // in release mode(negative scale does screw up bounding spheres), we'll
    // just go ahead and change sign on the scale.
    NIASSERT( m_kLocal.m_fScale >= 0.0f );
    m_kLocal.m_fScale = NiAbs(m_kLocal.m_fScale);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetLocalFromWorldTransform(
    const NiTransform& kWorldTransform)
{
    NiAVObject* pkParent = (NiAVObject*)GetParent();
    if (pkParent)
    {
        NiTransform kParentWorldInv;
        pkParent->GetWorldTransform().Invert(kParentWorldInv);
        NiTransform kLocalTransform(kParentWorldInv * kWorldTransform);
        SetLocalTransform(kLocalTransform);
    }
    else
    {
        SetLocalTransform(kWorldTransform);
    }
}
//---------------------------------------------------------------------------
inline const NiBound& NiAVObject::GetWorldBound() const
{
    return m_kWorldBound;
}
//---------------------------------------------------------------------------
inline const NiPropertyList& NiAVObject::GetPropertyList() const
{
    return m_kPropertyList;
}
//---------------------------------------------------------------------------
inline NiPropertyList& NiAVObject::GetPropertyList()
{
    return m_kPropertyList;
}
//---------------------------------------------------------------------------
inline void NiAVObject::AttachProperty(NiProperty* pkProperty)
{
    NIASSERT(GetProperty(pkProperty->Type()) == NULL);
    m_kPropertyList.AddHead(pkProperty);
}
//---------------------------------------------------------------------------
inline void NiAVObject::DetachProperty(NiProperty* pkProperty)
{
    m_kPropertyList.Remove(pkProperty);
}
//---------------------------------------------------------------------------
inline void NiAVObject::DetachAllProperties()
{
    m_kPropertyList.RemoveAll();
}
//---------------------------------------------------------------------------
inline void NiAVObject::UpdateWorldBound()
{
    // NOTE: This virtual function is intentionally inlined.
    // stub for derived classes
}
//---------------------------------------------------------------------------
inline bool NiAVObject::IsVisualObject() const
{ 
    return m_kWorldBound.GetRadius() != 0.0f; 
}
//---------------------------------------------------------------------------
inline void NiAVObject::UpdateObjectControllers(float fTime, bool bProperties)
{
    if (bProperties)
    {
        NiTListIterator kPos = m_kPropertyList.GetHeadPos();
        while (kPos)
        {
            NiProperty* pkProperty = m_kPropertyList.GetNext(kPos);
            if (pkProperty && pkProperty->GetControllers())
                pkProperty->Update(fTime);
        }
    }

    NiTimeController* pkControl = GetControllers();
    for (/**/; pkControl; pkControl = pkControl->GetNext())
        pkControl->Update(fTime);
}
//---------------------------------------------------------------------------
inline void NiAVObject::DoSelectedUpdate(NiUpdateProcess& kUpdate)
{
    if (GetSelectiveUpdate())
    {
        if (GetSelectiveUpdateRigid())
            UpdateRigidDownwardPass(kUpdate);
        else 
            UpdateSelectedDownwardPass(kUpdate);
    }
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetDisplayObject(bool bDisplay)
{
    SetBit(bDisplay, DISPLAY_OBJECT_MASK);
}
//---------------------------------------------------------------------------
inline bool NiAVObject::GetDisplayObject() const
{
    return GetBit(DISPLAY_OBJECT_MASK);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetCollisionObject(
    NiCollisionObject* pkCollisionObject)
{
    m_spCollisionObject = pkCollisionObject;

    if (m_spCollisionObject && 
        m_spCollisionObject->GetSceneGraphObject() != this)
        m_spCollisionObject->SetSceneGraphObject(this);
}
//---------------------------------------------------------------------------
inline NiCollisionObject* NiAVObject::GetCollisionObject() const
{
    return m_spCollisionObject;
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetSortObject(bool bSort)
{
    // Invert the meaning of the flag
    SetBit(!bSort, DISABLE_SORTING);
}
//---------------------------------------------------------------------------
inline bool NiAVObject::GetSortObject() const
{
    // Invert the meaning of the flag
    return !GetBit(DISABLE_SORTING);
}
//---------------------------------------------------------------------------
inline bool NiAVObject::IsNode()
{
    return GetBit(IS_NODE);
}
//---------------------------------------------------------------------------
inline bool NiAVObject::IsLeaf()
{
    return !GetBit(IS_NODE);
}
//---------------------------------------------------------------------------
inline void NiAVObject::SetNodeBit()
{
    SetBit(true, IS_NODE);
}
//---------------------------------------------------------------------------
