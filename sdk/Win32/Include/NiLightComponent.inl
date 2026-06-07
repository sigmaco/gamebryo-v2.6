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
inline NiLightComponent::NiLightComponent(
    unsigned int uiAffectedEntitiesArraySize,
    unsigned int uiAffectedEntitiesArrayGrowBy) : m_uFlags(0),
    m_kLightType(LT_SPOT), m_fDimmer(1.0f), m_kAmbientColor(NiColor::BLACK),
    m_kDiffuseColor(NiColor::WHITE), m_kSpecularColor(NiColor::WHITE),
    m_fConstantAttenuation(1.0f), m_fLinearAttenuation(0.0f),
    m_fQuadraticAttenuation(0.0f), m_fOuterSpotAngle(22.5f),
    m_fInnerSpotAngle(21.5f), m_fSpotExponent(1.0f),
    m_kAffectedEntities(uiAffectedEntitiesArraySize,
    uiAffectedEntitiesArrayGrowBy)
{
}
//---------------------------------------------------------------------------
inline NiLightComponent::NiLightComponent(const NiFixedString& kLightType,
    float fDimmer, const NiColor& kAmbientColor, const NiColor& kDiffuseColor,
    const NiColor& kSpecularColor, float fConstantAttenuation,
    float fLinearAttenuation, float fQuadraticAttenuation,
    float fOuterSpotAngle, float fInnerSpotAngle, float fSpotExponent,
    unsigned int uiAffectedEntitiesArraySize,
    unsigned int uiAffectedEntitiesArrayGrowBy) : m_uFlags(0),
    m_kLightType(kLightType), m_fDimmer(fDimmer),
    m_kAmbientColor(kAmbientColor), m_kDiffuseColor(kDiffuseColor),
    m_kSpecularColor(kSpecularColor),
    m_fConstantAttenuation(fConstantAttenuation),
    m_fLinearAttenuation(fLinearAttenuation),
    m_fQuadraticAttenuation(fQuadraticAttenuation),
    m_fOuterSpotAngle(fOuterSpotAngle), m_fInnerSpotAngle(fInnerSpotAngle),
    m_fSpotExponent(fSpotExponent),
    m_kAffectedEntities(uiAffectedEntitiesArraySize,
    uiAffectedEntitiesArrayGrowBy)
{
    NIASSERT(IsLightTypeValid(m_kLightType));
}
//---------------------------------------------------------------------------
inline NiBool NiLightComponent::IsLightTypeValid(
    const NiFixedString& kLightType) const
{
    if (kLightType == LT_AMBIENT ||
        kLightType == LT_DIRECTIONAL ||
        kLightType == LT_POINT ||
        kLightType == LT_SPOT)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline void NiLightComponent::DeleteLight()
{
    if (m_spLight)
    {
        // Clear all old affected entities.
        const NiNodeList& kAffectedNodesList =
            m_spLight->GetAffectedNodeList();
        NiTPrimitiveSet<NiNode*> kAffectedNodesSet(
            kAffectedNodesList.GetSize());
        NiTListIterator kIter = kAffectedNodesList.GetHeadPos();
        while (kIter)
        {
            kAffectedNodesSet.Add(kAffectedNodesList.GetNext(kIter));
        }
        m_spLight->DetachAllAffectedNodes();
        for (unsigned int ui = 0; ui < kAffectedNodesSet.GetSize(); ui++)
        {
            kAffectedNodesSet.GetAt(ui)->UpdateEffects();
        }

        // Clear out existing light.
        m_spLight = NULL;
        SetLightCreationErrorHit(false);
    }
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiLightComponent::GetLightType() const
{
    return m_kLightType;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetLightType(const NiFixedString& kLightType)
{
    if (GetLightType() != kLightType)
    {
        NIASSERT(IsLightTypeValid(kLightType));
        m_kLightType = kLightType;
        DeleteLight();
    }
}
//---------------------------------------------------------------------------
inline float NiLightComponent::GetDimmer() const
{
    return m_fDimmer;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetDimmer(float fDimmer)
{
    if (GetDimmer() != fDimmer)
    {
        m_fDimmer = fDimmer;
        SetLightPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline const NiColor& NiLightComponent::GetAmbientColor() const
{
    return m_kAmbientColor;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetAmbientColor(const NiColor& kAmbientColor)
{
    if (GetAmbientColor() != kAmbientColor)
    {
        m_kAmbientColor = kAmbientColor;
        SetLightPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline const NiColor& NiLightComponent::GetDiffuseColor() const
{
    return m_kDiffuseColor;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetDiffuseColor(const NiColor& kDiffuseColor)
{
    if (GetDiffuseColor() != kDiffuseColor)
    {
        m_kDiffuseColor = kDiffuseColor;
        SetLightPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline const NiColor& NiLightComponent::GetSpecularColor() const
{
    return m_kSpecularColor;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetSpecularColor(const NiColor& kSpecularColor)
{
    if (GetSpecularColor() != kSpecularColor)
    {
        m_kSpecularColor = kSpecularColor;
        SetLightPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline float NiLightComponent::GetConstantAttenuation() const
{
    return m_fConstantAttenuation;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetConstantAttenuation(
    float fConstantAttenuation)
{
    if (GetConstantAttenuation() != fConstantAttenuation)
    {
        m_fConstantAttenuation = fConstantAttenuation;
        SetLightPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline float NiLightComponent::GetLinearAttenuation() const
{
    return m_fLinearAttenuation;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetLinearAttenuation(float fLinearAttenuation)
{
    if (GetLinearAttenuation() != fLinearAttenuation)
    {
        m_fLinearAttenuation = fLinearAttenuation;
        SetLightPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline float NiLightComponent::GetQuadraticAttenuation() const
{
    return m_fQuadraticAttenuation;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetQuadraticAttenuation(
    float fQuadraticAttenuation)
{
    if (GetQuadraticAttenuation() != fQuadraticAttenuation)
    {
        m_fQuadraticAttenuation = fQuadraticAttenuation;
        SetLightPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline float NiLightComponent::GetOuterSpotAngle() const
{
    return m_fOuterSpotAngle;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetOuterSpotAngle(float fOuterSpotAngle)
{
    if (GetOuterSpotAngle() != fOuterSpotAngle)
    {
        m_fOuterSpotAngle = fOuterSpotAngle;
        SetLightPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline float NiLightComponent::GetInnerSpotAngle() const
{
    return m_fInnerSpotAngle;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetInnerSpotAngle(float fInnerSpotAngle)
{
    if (GetInnerSpotAngle() != fInnerSpotAngle)
    {
        m_fInnerSpotAngle = fInnerSpotAngle;
        SetLightPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline float NiLightComponent::GetSpotExponent() const
{
    return m_fSpotExponent;
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetSpotExponent(float fSpotExponent)
{
    if (GetSpotExponent() != fSpotExponent)
    {
        m_fSpotExponent = fSpotExponent;
        SetLightPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiLightComponent::GetAffectedEntitiesCount() const
{
    return m_kAffectedEntities.GetSize();
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiLightComponent::GetAffectedEntityAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kAffectedEntities.GetSize());

    return m_kAffectedEntities.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiLightComponent::AddAffectedEntity(NiEntityInterface* pkEntity)
{
    NIASSERT(pkEntity);

    NiBool bFound = false;
    for (unsigned int ui = 0; ui < m_kAffectedEntities.GetSize(); ui++)
    {
        if (m_kAffectedEntities.GetAt(ui) == pkEntity)
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        m_kAffectedEntities.AddFirstEmpty(pkEntity);
        SetAffectedEntitiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline void NiLightComponent::RemoveAffectedEntity(
    NiEntityInterface* pkEntity)
{
    NIASSERT(pkEntity);

    m_kAffectedEntities.Remove(pkEntity);
    SetAffectedEntitiesChanged(true);
}
//---------------------------------------------------------------------------
inline void NiLightComponent::RemoveAffectedEntityAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kAffectedEntities.GetSize());

    m_kAffectedEntities.RemoveAt(uiIndex);
    SetAffectedEntitiesChanged(true);
}
//---------------------------------------------------------------------------
inline void NiLightComponent::RemoveAllAffectedEntities()
{
    m_kAffectedEntities.RemoveAll();
    SetAffectedEntitiesChanged(true);
}
//---------------------------------------------------------------------------
inline NiBool NiLightComponent::GetLightCreationErrorHit() const
{
    return GetBit(LIGHT_CREATION_ERROR_HIT_MASK);
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetLightCreationErrorHit(
    bool bLightCreationErrorHit)
{
    SetBit(bLightCreationErrorHit, LIGHT_CREATION_ERROR_HIT_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiLightComponent::GetLightPropertiesChanged() const
{
    return GetBit(LIGHT_PROPERTIES_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetLightPropertiesChanged(
    bool bLightPropertiesChanged)
{
    SetBit(bLightPropertiesChanged, LIGHT_PROPERTIES_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiLightComponent::GetAffectedEntitiesChanged() const
{
    return GetBit(AFFECTED_ENTITIES_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline void NiLightComponent::SetAffectedEntitiesChanged(
    bool bAffectedEntitiesChanged)
{
    SetBit(bAffectedEntitiesChanged, AFFECTED_ENTITIES_CHANGED_MASK);
}
//---------------------------------------------------------------------------
