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
inline const NiFixedString& NiTransformationComponent::PROP_TRANSLATION()
{
    return ms_kPropTranslation;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::PROP_ROTATION()
{
    return ms_kPropRotation;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::PROP_SCALE()
{
    return ms_kPropScale;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::PROP_SOURCE_ENTITY()
{
    return ms_kPropSourceEntity;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::
    PROP_ATTACHMENT_POINT_NAME()
{
    return ms_kPropAttachmentPointName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::
    PROP_INHERIT_TRANSLATION()
{
    return ms_kPropInheritTranslation;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::PROP_INHERIT_ROTATION()
{
    return ms_kPropInheritRotation;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::PROP_INHERIT_SCALE()
{
    return ms_kPropInheritScale;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::PROP_LOCAL_TRANSLATION()
{
    return ms_kPropLocalTranslation;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::PROP_LOCAL_ROTATION()
{
    return ms_kPropLocalRotation;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::PROP_LOCAL_SCALE()
{
    return ms_kPropLocalScale;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::
    ERR_ATTACHMENT_POINT_NOT_FOUND()
{
    return ms_kErrAttachmentPointNotFound;
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::GetTranslation(NiPoint3& kTranslation)
    const
{
    if (!GetTranslationUnique() && m_spMasterComponent)
    {
        m_spMasterComponent->GetTranslation(kTranslation);
    }
    else
    {
        kTranslation = m_kTranslation;
    }
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetTranslation(
    const NiPoint3& kTranslation)
{
    NiPoint3 kCurrentTranslation;
    GetTranslation(kCurrentTranslation);
    if (kCurrentTranslation != kTranslation)
    {
        m_kTranslation = kTranslation;
        SetTranslationUnique(true);
    }
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::GetRotation(NiMatrix3& kRotation) const
{
    if (!GetRotationUnique() && m_spMasterComponent)
    {
        m_spMasterComponent->GetRotation(kRotation);
    }
    else
    {
        kRotation = m_kRotation;
    }
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetRotation(const NiMatrix3& kRotation)
{
    NiMatrix3 kCurrentRotation;
    GetRotation(kCurrentRotation);
    if (kCurrentRotation != kRotation)
    {
        m_kRotation = kRotation;
        SetRotationUnique(true);
    }
}
//---------------------------------------------------------------------------
inline float NiTransformationComponent::GetScale() const
{
    float fScale;
    if (!GetScaleUnique() && m_spMasterComponent)
    {
        fScale = m_spMasterComponent->GetScale();
    }
    else
    {
        fScale = m_fScale;
    }

    return fScale;
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetScale(float fScale)
{
    if (GetScale() != fScale)
    {
        m_fScale = fScale;
        SetScaleUnique(true);
    }
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiTransformationComponent::GetSourceEntity() const
{
    return m_pkSourceEntity;
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetSourceEntity(
    NiEntityInterface* pkSourceEntity)
{
    if (GetSourceEntity() != pkSourceEntity)
    {
        m_pkSourceEntity = pkSourceEntity;
        ClearAttachmentPoint();
    }
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTransformationComponent::GetAttachmentPointName()
    const
{
    if (!GetAttachmentPointNameUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetAttachmentPointName();
    }
    else
    {
        return m_kAttachmentPointName;
    }
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetAttachmentPointName(
    const NiFixedString& kAttachmentPointName)
{
    if (GetAttachmentPointName() != kAttachmentPointName)
    {
        m_kAttachmentPointName = kAttachmentPointName;
        ClearAttachmentPoint();
        SetAttachmentPointNameUnique(true);
    }
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetInheritTranslation() const
{
    if (!GetInheritTranslationUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetInheritTranslation();
    }
    else
    {
        return GetBit(INHERIT_TRANSLATION_MASK);
    }
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetInheritTranslation(
    bool bInheritTranslation)
{
    if (NIBOOL_IS_TRUE(GetInheritTranslation()) != bInheritTranslation)
    {
        SetBit(bInheritTranslation, INHERIT_TRANSLATION_MASK);
        SetInheritTranslationUnique(true);
    }
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetInheritRotation() const
{
    if (!GetInheritRotationUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetInheritRotation();
    }
    else
    {
        return GetBit(INHERIT_ROTATION_MASK);
    }
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetInheritRotation(
    bool bInheritRotation)
{
    if (NIBOOL_IS_TRUE(GetInheritRotation()) != bInheritRotation)
    {
        SetBit(bInheritRotation, INHERIT_ROTATION_MASK);
        SetInheritRotationUnique(true);
    }
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetInheritScale() const
{
    if (!GetInheritScaleUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetInheritScale();
    }
    else
    {
        return GetBit(INHERIT_SCALE_MASK);
    }
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetInheritScale(bool bInheritScale)
{
    if (NIBOOL_IS_TRUE(GetInheritScale()) != bInheritScale)
    {
        SetBit(bInheritScale, INHERIT_SCALE_MASK);
        SetInheritScaleUnique(true);
    }
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::ClearAttachmentPoint()
{
    m_pkAttachmentPoint = NULL;
    SetErrorResolvingAttachmentPoint(false);
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::ShouldResolveAttachmentPoint()
{
    // If the attachment point name has changed on the master component,
    // reset it and clear the attachment point.
    if (GetAttachmentPointName() != m_kAttachmentPointName)
    {
        m_kAttachmentPointName = GetAttachmentPointName();
        ClearAttachmentPoint();
    }

    return (m_pkSourceEntity && !m_pkAttachmentPoint &&
        m_kAttachmentPointName.Exists() &&
        !GetErrorResolvingAttachmentPoint());
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetErrorResolvingAttachmentPoint()
    const
{
    return GetBit(ERROR_RESOLVING_ATTACHMENT_POINT_MASK);
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetErrorResolvingAttachmentPoint(
    bool bErrorResolvingAttachmentPoint)
{
    SetBit(bErrorResolvingAttachmentPoint,
        ERROR_RESOLVING_ATTACHMENT_POINT_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetTranslationUnique() const
{
    return GetBit(TRANSLATION_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetTranslationUnique(
    bool bTranslationUnique)
{
    SetBit(bTranslationUnique, TRANSLATION_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetRotationUnique() const
{
    return GetBit(ROTATION_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetRotationUnique(bool bRotationUnique)
{
    SetBit(bRotationUnique, ROTATION_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetScaleUnique() const
{
    return GetBit(SCALE_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetScaleUnique(bool bScaleUnique)
{
    SetBit(bScaleUnique, SCALE_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetAttachmentPointNameUnique() const
{
    return GetBit(ATTACHMENT_POINT_NAME_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetAttachmentPointNameUnique(
    bool bAttachementPointNameUnique)
{
    SetBit(bAttachementPointNameUnique, ATTACHMENT_POINT_NAME_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetInheritTranslationUnique() const
{
    return GetBit(INHERIT_TRANSLATION_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetInheritTranslationUnique(
    bool bInheritTranslationUnique)
{
    SetBit(bInheritTranslationUnique, INHERIT_TRANSLATION_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetInheritRotationUnique() const
{
    return GetBit(INHERIT_ROTATION_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetInheritRotationUnique(
    bool bInheritRotationUnique)
{
    SetBit(bInheritRotationUnique, INHERIT_ROTATION_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiTransformationComponent::GetInheritScaleUnique() const
{
    return GetBit(INHERIT_SCALE_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiTransformationComponent::SetInheritScaleUnique(
    bool bInheritScaleUnique)
{
    SetBit(bInheritScaleUnique, INHERIT_SCALE_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
