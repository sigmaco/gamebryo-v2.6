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
inline NiShadowGeneratorComponent::NiShadowGeneratorComponent(
    unsigned int uiCasterArraySize, unsigned int uiCasterArrayGrowBy,
    unsigned int uiUnaffectedReceiverArraySize,
    unsigned int uiUnaffectedReceiverArrayGrowBy) : m_uFlags(0),
    m_kUnaffectedCasters(uiCasterArraySize, uiCasterArrayGrowBy),
    m_kUnaffectedReceivers(uiUnaffectedReceiverArraySize,
    uiUnaffectedReceiverArrayGrowBy),
    m_kShadowTechnique("NiStandardShadowTechnique"), m_fDepthBias(0.0f),
    m_usSizeHint(1024), m_bCastShadows(true), m_bStaticShadows(false),
    m_bStrictlyObserveSizeHint(false), m_bRenderBackfaces(true),
    m_bUseDefaultDepthBias(true)
{
}
//---------------------------------------------------------------------------
inline NiShadowGeneratorComponent::NiShadowGeneratorComponent(
    bool bCastShadows, bool bStaticShadows, unsigned short usSizeHint,
    bool bStrictlyObserveSizeHint, const NiFixedString& kShadowTechnique,
    bool bRenderBackfaces, float fDepthBias, bool bUseDefaultDepthBias,
    unsigned int uiCasterArraySize, unsigned int uiCasterArrayGrowBy,
    unsigned int uiUnaffectedReceiverArraySize,
    unsigned int uiUnaffectedReceiverArrayGrowBy) : m_uFlags(0),
    m_kUnaffectedCasters(uiCasterArraySize, uiCasterArrayGrowBy),
    m_kUnaffectedReceivers(uiUnaffectedReceiverArraySize,
    uiUnaffectedReceiverArrayGrowBy), m_kShadowTechnique(kShadowTechnique),
    m_fDepthBias(fDepthBias), m_usSizeHint(usSizeHint),
    m_bCastShadows(bCastShadows), m_bStaticShadows(bStaticShadows),
    m_bStrictlyObserveSizeHint(bStrictlyObserveSizeHint),
    m_bRenderBackfaces(bRenderBackfaces),
    m_bUseDefaultDepthBias(bUseDefaultDepthBias)
{
}
//---------------------------------------------------------------------------
inline NiShadowGeneratorComponent::NiShadowGeneratorComponent(
    unsigned int uiCasterArraySize, unsigned int uiCasterArrayGrowBy,
    unsigned int uiUnaffectedReceiverArraySize,
    unsigned int uiUnaffectedReceiverArrayGrowBy,
    NiShadowGeneratorComponent* pkMasterComponent) : m_uFlags(0),
    m_spMasterComponent(pkMasterComponent),
    m_kUnaffectedCasters(uiCasterArraySize, uiCasterArrayGrowBy),
    m_kUnaffectedReceivers(uiUnaffectedReceiverArraySize,
    uiUnaffectedReceiverArrayGrowBy),
    m_kShadowTechnique("NiStandardShadowTechnique"), m_fDepthBias(0.0f),
    m_usSizeHint(1024), m_bCastShadows(true), m_bStaticShadows(false),
    m_bStrictlyObserveSizeHint(false), m_bRenderBackfaces(true),
    m_bUseDefaultDepthBias(true)
{
}
//---------------------------------------------------------------------------
inline NiShadowGeneratorComponent::~NiShadowGeneratorComponent()
{
    ClearShadowGenerator();
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetCastShadows(bool bCastShadows)
{
    if (GetCastShadows() != bCastShadows)
    {
        m_bCastShadows = bCastShadows;
        SetCastShadowsUnique(true);

        if (!m_bCastShadows)
        {
            ClearShadowGenerator();
        }
    }
}
//---------------------------------------------------------------------------
inline bool NiShadowGeneratorComponent::GetCastShadows() const
{
    if (!GetCastShadowsUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetCastShadows();
    }
    else
    {
        return m_bCastShadows;
    }
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetStaticShadows(bool bStaticShadows)
{
    if (GetStaticShadows() != bStaticShadows)
    {
        m_bStaticShadows = bStaticShadows;
        SetStaticShadowsUnique(true);
        SetShadowGeneratorPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline bool NiShadowGeneratorComponent::GetStaticShadows() const
{
    if (!GetStaticShadowsUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetStaticShadows();
    }
    else
    {
        return m_bStaticShadows;
    }
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetSizeHint(unsigned short usSizeHint)
{
    if (GetSizeHint() != usSizeHint)
    {
        m_usSizeHint = usSizeHint;
        SetSizeHintUnique(true);
        ClearShadowGenerator();
    }
}
//---------------------------------------------------------------------------
inline unsigned short NiShadowGeneratorComponent::GetSizeHint() const
{
    if (!GetSizeHintUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetSizeHint();
    }
    else
    {
        return m_usSizeHint;
    }
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetStrictlyObserveSizeHint(
    bool bStrictlyObserveSizeHint)
{
    if (GetStrictlyObserveSizeHint() != bStrictlyObserveSizeHint)
    {
        m_bStrictlyObserveSizeHint = bStrictlyObserveSizeHint;
        SetStrictlyObserveSizeHintUnique(true);
        ClearShadowGenerator();
    }
}
//---------------------------------------------------------------------------
inline bool NiShadowGeneratorComponent::GetStrictlyObserveSizeHint() const
{
    if (!GetStrictlyObserveSizeHintUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetStrictlyObserveSizeHint();
    }
    else
    {
        return m_bStrictlyObserveSizeHint;
    }
}
//---------------------------------------------------------------------------
inline unsigned int 
    NiShadowGeneratorComponent::GetUnaffectedCasterCount() const
{
    return m_kUnaffectedCasters.GetSize();
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiShadowGeneratorComponent::GetUnaffectedCasterAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kUnaffectedCasters.GetSize());
    return m_kUnaffectedCasters.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::AddUnaffectedCaster(
    NiEntityInterface* pkEntity)
{
    NIASSERT(pkEntity);
    bool bFound = false;
    for (unsigned int ui = 0; ui < m_kUnaffectedCasters.GetSize(); ui++)
    {
        if (m_kUnaffectedCasters.GetAt(ui) == pkEntity)
        {
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        m_kUnaffectedCasters.AddFirstEmpty(pkEntity);
        SetUnaffectedCastersChanged(true);
    }
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::RemoveUnaffectedCaster(
    NiEntityInterface* pkEntity)
{
    NIASSERT(pkEntity);
    m_kUnaffectedCasters.Remove(pkEntity);
    SetUnaffectedCastersChanged(true);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::RemoveUnaffectedCasterAt(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kUnaffectedCasters.GetSize());
    m_kUnaffectedCasters.RemoveAt(uiIndex);
    SetUnaffectedCastersChanged(true);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::RemoveAllUnaffectedCasters()
{
    m_kUnaffectedCasters.RemoveAll();
    SetUnaffectedCastersChanged(true);
}
//---------------------------------------------------------------------------
inline unsigned int NiShadowGeneratorComponent::GetUnaffectedReceiverCount()
    const
{
    return m_kUnaffectedReceivers.GetSize();
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiShadowGeneratorComponent::GetUnaffectedReceiverAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kUnaffectedReceivers.GetSize());
    return m_kUnaffectedReceivers.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::AddUnaffectedReceiver(
    NiEntityInterface* pkEntity)
{
    NIASSERT(pkEntity);
    bool bFound = false;
    for (unsigned int ui = 0; ui < m_kUnaffectedReceivers.GetSize(); ui++)
    {
        if (m_kUnaffectedReceivers.GetAt(ui) == pkEntity)
        {
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        m_kUnaffectedReceivers.AddFirstEmpty(pkEntity);
        SetUnaffectedReceiversChanged(true);
    }
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::RemoveUnaffectedReceiver(
    NiEntityInterface* pkEntity)
{
    NIASSERT(pkEntity);
    m_kUnaffectedReceivers.Remove(pkEntity);
    SetUnaffectedReceiversChanged(true);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::RemoveUnaffectedReceiverAt(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kUnaffectedReceivers.GetSize());
    m_kUnaffectedReceivers.RemoveAt(uiIndex);
    SetUnaffectedReceiversChanged(true);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::RemoveAllUnaffectedReceivers()
{
    m_kUnaffectedReceivers.RemoveAll();
    SetUnaffectedReceiversChanged(true);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetShadowTechnique(
    const NiFixedString& kShadowTechnique)
{
    if (GetShadowTechnique() != kShadowTechnique)
    {
        m_kShadowTechnique = kShadowTechnique;
        SetShadowTechniqueUnique(true);
        SetShadowGeneratorPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiShadowGeneratorComponent::GetShadowTechnique()
    const
{
    if (!GetShadowTechniqueUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetShadowTechnique();
    }
    else
    {
        return m_kShadowTechnique;
    }
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetRenderBackfaces(
    bool bRenderBackfaces)
{
    if (GetRenderBackfaces() != bRenderBackfaces)
    {
        m_bRenderBackfaces = bRenderBackfaces;
        SetRenderBackfacesUnique(true);
        SetShadowGeneratorPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline bool NiShadowGeneratorComponent::GetRenderBackfaces() const
{
    if (!GetRenderBackfacesUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetRenderBackfaces();
    }
    else
    {
        return m_bRenderBackfaces;
    }
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetDepthBias(float fDepthBias)
{
    if (GetDepthBias() != fDepthBias)
    {
        m_fDepthBias = fDepthBias;
        SetDepthBiasUnique(true);
        SetShadowGeneratorPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline float NiShadowGeneratorComponent::GetDepthBias() const
{
    if (!GetDepthBiasUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetDepthBias();
    }
    else
    {
        return m_fDepthBias;
    }
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetUseDefaultDepthBias(
    bool bUseDefaultDepthBias)
{
    if (GetUseDefaultDepthBias() != bUseDefaultDepthBias)
    {
        m_bUseDefaultDepthBias = bUseDefaultDepthBias;
        SetUseDefaultDepthBiasUnique(true);
        SetShadowGeneratorPropertiesChanged(true);
    }
}
//---------------------------------------------------------------------------
inline bool NiShadowGeneratorComponent::GetUseDefaultDepthBias() const
{
    if (!GetUseDefaultDepthBiasUnique() && m_spMasterComponent)
    {
        return m_spMasterComponent->GetUseDefaultDepthBias();
    }
    else
    {
        return m_bUseDefaultDepthBias;
    }
}
//---------------------------------------------------------------------------
inline NiShadowGenerator* NiShadowGeneratorComponent::GetShadowGenerator()
    const
{
    return m_spShadowGenerator;
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetShadowGeneratorPropertiesChanged()
    const
{
    return GetBit(SHADOW_GENERATOR_PROPERTIES_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetShadowGeneratorPropertiesChanged(
    bool bShadowGeneratorPropertiesChanged)
{
    SetBit(bShadowGeneratorPropertiesChanged,
        SHADOW_GENERATOR_PROPERTIES_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetUnaffectedCastersChanged() const
{
    return GetBit(UNAFFECTED_CASTERS_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetUnaffectedCastersChanged(
    bool bCastersChanged)
{
    SetBit(bCastersChanged, UNAFFECTED_CASTERS_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetUnaffectedReceiversChanged() const
{
    return GetBit(UNAFFECTED_RECEIVERS_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetUnaffectedReceiversChanged(
    bool bUnaffectedReceiversChanged)
{
    SetBit(bUnaffectedReceiversChanged, UNAFFECTED_RECEIVERS_CHANGED_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetCastShadowsUnique() const
{
    return GetBit(CAST_SHADOWS_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetCastShadowsUnique(
    bool bCastShadowsUnique)
{
    SetBit(bCastShadowsUnique, CAST_SHADOWS_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetStaticShadowsUnique() const
{
    return GetBit(STATIC_SHADOWS_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetStaticShadowsUnique(
    bool bStaticShadowsUnique)
{
    SetBit(bStaticShadowsUnique, STATIC_SHADOWS_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetSizeHintUnique() const
{
    return GetBit(SIZE_HINT_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetSizeHintUnique(bool bSizeHintUnique)
{
    SetBit(bSizeHintUnique, SIZE_HINT_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetStrictlyObserveSizeHintUnique()
    const
{
    return GetBit(STRICTLY_OBSERVE_SIZE_HINT_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetStrictlyObserveSizeHintUnique(
    bool bStrictlyObserveSizeHintUnique)
{
    SetBit(bStrictlyObserveSizeHintUnique,
        STRICTLY_OBSERVE_SIZE_HINT_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetShadowTechniqueUnique() const
{
    return GetBit(SHADOW_TECHNIQUE_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetShadowTechniqueUnique(
    bool bShadowTechniqueUnique)
{
    SetBit(bShadowTechniqueUnique, SHADOW_TECHNIQUE_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetRenderBackfacesUnique() const
{
    return GetBit(RENDER_BACKFACES_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetRenderBackfacesUnique(
    bool bRenderBackfacesUnique)
{
    SetBit(bRenderBackfacesUnique, RENDER_BACKFACES_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetDepthBiasUnique() const
{
    return GetBit(DEPTH_BIAS_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetDepthBiasUnique(
    bool bDepthBiasUnique)
{
    SetBit(bDepthBiasUnique, DEPTH_BIAS_UNIQUE_MASK);
}
//---------------------------------------------------------------------------
inline NiBool NiShadowGeneratorComponent::GetUseDefaultDepthBiasUnique() const
{
    return GetBit(USE_DEFAULT_DEPTH_BIAS_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::SetUseDefaultDepthBiasUnique(
    bool bUseDefaultDepthBiasUnique)
{
    SetBit(bUseDefaultDepthBiasUnique, USE_DEFAULT_DEPTH_BIAS_MASK);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::CreateShadowGenerator(
    NiDynamicEffect* pkDynamicEffect)
{
    if (m_spShadowGenerator)
    {
        ClearShadowGenerator();
    }

    // Create the shadow generator.
    m_spShadowGenerator = NiNew NiShadowGenerator(pkDynamicEffect);

    // Add the shadow generator to the shadow manager.
    NiShadowManager::AddShadowGenerator(m_spShadowGenerator);
}
//---------------------------------------------------------------------------
inline void NiShadowGeneratorComponent::ClearShadowGenerator()
{
    if (m_spShadowGenerator)
    {
        // Detach all unaffected casters and receivers before removing from
        // the shadow manager to avoid a problem upon scene destruction where
        // these nodes may have already been deleted. Since we are removing
        // the shadow generator, it does not matter that these nodes are not
        // set at this point.
        m_spShadowGenerator->DetachAllUnaffectedCasterNodes();
        m_spShadowGenerator->DetachAllUnaffectedReceiverNodes();

        // Remove the shadow generator from the shadow manager.
        NiShadowManager::DeleteShadowGenerator(m_spShadowGenerator);

        // Clear the shadow generator pointer.
        m_spShadowGenerator = NULL;
    }
}
//---------------------------------------------------------------------------
inline bool NiShadowGeneratorComponent::ShouldUpdateShadowGeneratorProperties()
    const
{
    return (GetShadowGeneratorPropertiesChanged() ||
        GetCastShadows() != m_bCastShadows ||
        GetStaticShadows() != m_bStaticShadows ||
        GetSizeHint() != m_usSizeHint ||
        GetStrictlyObserveSizeHint() != m_bStrictlyObserveSizeHint ||
        GetShadowTechnique() != m_kShadowTechnique ||
        GetRenderBackfaces() != m_bRenderBackfaces ||
        GetDepthBias() != m_fDepthBias ||
        GetUseDefaultDepthBias() != m_bUseDefaultDepthBias);
}
//---------------------------------------------------------------------------
inline void
    NiShadowGeneratorComponent::RefreshCachedShadowGeneratorProperties()
{
    m_bCastShadows = GetCastShadows();
    m_bStaticShadows = GetStaticShadows();
    m_usSizeHint = GetSizeHint();
    m_bStrictlyObserveSizeHint = GetStrictlyObserveSizeHint();
    m_kShadowTechnique = GetShadowTechnique();
    m_bRenderBackfaces = GetRenderBackfaces();
    m_fDepthBias = GetDepthBias();
    m_bUseDefaultDepthBias = GetUseDefaultDepthBias();
}
//---------------------------------------------------------------------------
