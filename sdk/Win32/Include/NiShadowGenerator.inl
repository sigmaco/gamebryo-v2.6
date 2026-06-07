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

#include "NiShadowManager.h"

//---------------------------------------------------------------------------
inline NiDynamicEffect* NiShadowGenerator::GetAssignedDynamicEffect() const
{
    return m_pkDynEffect;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetShadowTechnique(
    const NiFixedString& kTechniqueName, bool bAssignDefaultDepthBias)
{
    m_pkShadowTechnique = 
        NiShadowManager::GetActiveShadowTechnique(kTechniqueName);

    if (bAssignDefaultDepthBias)
        SetDepthBiasToDefault();

    // Release all the assigned shadow maps since the new technique may have
    // different shadow map needs.
    ReleaseAllShadowMaps();
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetShadowTechnique(
    NiShadowTechnique* pkTechnique, bool bAssignDefaultDepthBias)
{
    NIASSERT(pkTechnique);
    m_pkShadowTechnique = pkTechnique;

    if (bAssignDefaultDepthBias)
        SetDepthBiasToDefault();

    // Release all the assigned shadow maps since the new technique may have
    // different shadow map needs.
    ReleaseAllShadowMaps();
}
//---------------------------------------------------------------------------
inline NiShadowTechnique* NiShadowGenerator::GetShadowTechnique() const
{
    return m_pkShadowTechnique;
}
//---------------------------------------------------------------------------
inline void* NiShadowGenerator::GetExtraData() const
{
    return m_pvExtraData;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetExtraData(void* pvExtraData)
{
    m_pvExtraData = pvExtraData;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetSizeHint(unsigned short usSizeHint)
{
    m_usSizeHint = usSizeHint;
}
//---------------------------------------------------------------------------
inline unsigned short NiShadowGenerator::GetSizeHint() const
{
    return m_usSizeHint;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetStrictlyObserveSizeHint(
    bool bStrictlyObserveSizeHint)
{
    SetBit(bStrictlyObserveSizeHint, STRICTLY_OBSERVE_SIZE_HINT);
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::GetStrictlyObserveSizeHint() const
{
    return GetBit(STRICTLY_OBSERVE_SIZE_HINT);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetSMTextureAssignmentCallbackFunc(
    AssignSMTextureCallback pfnCallback)
{
    m_pfnSMTextureAssignmentCallback = pfnCallback;
}
//---------------------------------------------------------------------------
inline NiShadowGenerator::AssignSMTextureCallback NiShadowGenerator::
    GetSMTextureAssignmentCallbackFunc() const
{
    return m_pfnSMTextureAssignmentCallback;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetSMTextureRetrievalCallbackFunc(
    NiShadowGenerator::RetrieveSMTextureCallback pfnCallback)
{
    m_pfnSMTextureRetrievalCallback = pfnCallback;
}
//---------------------------------------------------------------------------
inline NiShadowGenerator::RetrieveSMTextureCallback NiShadowGenerator::
    GetSMTextureRetrievalCallbackFunc() const
{
    return m_pfnSMTextureRetrievalCallback;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetSMConstantRetrievalCallbackFunc(
    NiShadowGenerator::GetSMConstantCallback pfnCallback)
{
    m_pfnSMConstantDataRetrievalCallback = pfnCallback;
}
//---------------------------------------------------------------------------
inline NiShadowGenerator::GetSMConstantCallback NiShadowGenerator::
    GetSMConstantRetrievalCallbackFunc() const
{
    return m_pfnSMConstantDataRetrievalCallback;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetSMConstantAssignmentCallbackFunc(
    NiShadowGenerator::SetSMConstantCallback pfnCallback)
{
    m_pfnSMConstantDataAssignmentCallback = pfnCallback;
}
//---------------------------------------------------------------------------
inline NiShadowGenerator::SetSMConstantCallback NiShadowGenerator::
    GetSMConstantAssignmentCallbackFunc() const
{
    return m_pfnSMConstantDataAssignmentCallback;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetExtraDataCleanupCallbackFunc(
    CleanupExtraDataCallback pfnCallback)
{
    NIASSERT(pfnCallback);
    m_pfnExtraDataCleanupCallback = pfnCallback;
}
//---------------------------------------------------------------------------
inline NiShadowGenerator::CleanupExtraDataCallback NiShadowGenerator::
    GetExtraDataCleanupCallbackFunc() const
{
    return m_pfnExtraDataCleanupCallback;
}
//---------------------------------------------------------------------------
inline NiShadowMap* NiShadowGenerator::RetrieveShadowMap(
    unsigned int uiSMIndex, NiRenderObject* pkGeometry)
{
    NIASSERT(m_pfnSMTextureRetrievalCallback);
    return m_pfnSMTextureRetrievalCallback(this, pkGeometry, uiSMIndex);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetSMCountCallbackFunc(
    NiShadowGenerator::ShadowMapCountCallback pfnCallback)
{
    m_pfnSMCountCallback = pfnCallback;
}
//---------------------------------------------------------------------------
inline NiShadowGenerator::ShadowMapCountCallback NiShadowGenerator::
    GetSMCountCallbackFunc() const
{
    return m_pfnSMCountCallback;
}
//---------------------------------------------------------------------------
inline unsigned int NiShadowGenerator::GetShadowMapCount()
{
    if (m_pfnSMCountCallback)
        return m_pfnSMCountCallback(this);

    return 0;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetOwnedSMCallbackFunc(
    NiShadowGenerator::RetrieveOwnedShadowMap pfnCallback)
{
    NIASSERT(pfnCallback);
    m_pfnOwnedSMCallback = pfnCallback;
}
//---------------------------------------------------------------------------
inline NiShadowGenerator::RetrieveOwnedShadowMap NiShadowGenerator::
    GetOwnedSMCallbackFunc() const
{
    return m_pfnOwnedSMCallback;
}
//---------------------------------------------------------------------------
inline NiShadowMap* NiShadowGenerator::GetOwnedShadowMap(unsigned int uiIndex)
{
    if (m_pfnOwnedSMCallback)
        return m_pfnOwnedSMCallback(this, uiIndex);

    return NULL;
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::SetShaderConstantData(
    NiRenderObject* pkGeometry, 
    unsigned int uiSMIndex, 
    NiShaderConstantMap::ObjectMappings eMapping, 
    void* pkData)
{
    NIASSERT(m_pfnSMConstantDataAssignmentCallback);
    return m_pfnSMConstantDataAssignmentCallback(this, pkGeometry,
        uiSMIndex, eMapping, pkData);
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::GetShaderConstantData(
    void* pvData, 
    unsigned int uiDataSize, 
    NiRenderObject* pkGeometry, 
    unsigned int uiSMIndex,
    NiShaderConstantMap::ObjectMappings eMapping, 
    const NiPropertyState* pkState, 
    const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, 
    const NiBound& kWorldBound, 
    unsigned int uiPass)
{
    NIASSERT(m_pfnSMConstantDataRetrievalCallback);
    return m_pfnSMConstantDataRetrievalCallback(pvData, uiDataSize,
        this, pkGeometry, uiSMIndex, eMapping, pkState, pkEffects, kWorld, 
        kWorldBound, uiPass);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetShadowMapGenerator(NiShadowMap* pkShadowMap,
    NiShadowGenerator* pkGenerator)
{
    NIASSERT(pkShadowMap);

    NiShadowRenderClick* pkShadowClick = pkShadowMap->GetRenderClick();
    if (pkShadowClick)
    {
        pkShadowClick->SetGenerator(pkGenerator);
    }
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetShadowCubeMapGenerator(
    NiShadowCubeMap* pkShadowCubeMap, NiShadowGenerator* pkGenerator)
{
    NIASSERT(pkShadowCubeMap);

    for (unsigned char uc = 0; uc < pkShadowCubeMap->GetNumRenderClicks();
        uc++)
    {
        NiShadowRenderClick* pkShadowClick = pkShadowCubeMap->GetRenderClick(
            uc);
        if (pkShadowClick)
        {
            pkShadowClick->SetGenerator(pkGenerator);
        }
    }
}
//---------------------------------------------------------------------------
inline const NiNodeList& NiShadowGenerator::GetUnaffectedCasterNodeList()
{
    return m_kUnaffectedCasterList;
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::AttachUnaffectedCasterNode(NiNode* pkCasterNode)
{
    // Ensure the node is not already in the list
    if (m_kUnaffectedCasterList.FindPos(pkCasterNode))
        return false;

    m_kUnaffectedCasterList.AddHead(pkCasterNode);
    return true;
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::DetachUnaffectedCasterNode(NiNode* pkCasterNode)
{
    return m_kUnaffectedCasterList.Remove(pkCasterNode) != NULL;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::DetachAllUnaffectedCasterNodes()
{
    m_kUnaffectedCasterList.RemoveAll();
}
//---------------------------------------------------------------------------
inline const NiNodeList& NiShadowGenerator::GetUnaffectedReceiverNodeList()
{
    return m_kUnaffectedReceiverList;
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::AttachUnaffectedReceiverNode(
    NiNode* pkUnaffectedNode)
{
    // Ensure the node is not already in the list
    if (m_kUnaffectedReceiverList.FindPos(pkUnaffectedNode))
        return false;

    m_kUnaffectedReceiverList.AddHead(pkUnaffectedNode);
    return true;
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::DetachUnaffectedReceiverNode(
    NiNode* pkUnaffectedNode)
{
    return m_kUnaffectedReceiverList.Remove(pkUnaffectedNode) != NULL;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::DetachAllUnaffectedReceiverNodes()
{
    m_kUnaffectedReceiverList.RemoveAll();
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetDynamicEffect(NiDynamicEffect* pkEffect)
{
    m_pkDynEffect = pkEffect;

    // If the dynamic effect is not on ensure that the shadow generator is not
    // active.
    if (m_pkDynEffect && !m_pkDynEffect->GetSwitch())
        SetActive(false);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetStatic(bool bStatic)
{
    SetBit(bStatic, STATIC);
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::GetStatic()
{
    return GetBit(STATIC);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetRenderBackfaces(bool bBackfaces)
{
    SetBit(bBackfaces, RENDER_BACKFACES);
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::GetRenderBackfaces()
{
    return GetBit(RENDER_BACKFACES);
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::GetActive()
{
    return GetBit(ACTIVE);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetDepthBias(float fDepthBias)
{
    m_fDepthBias = fDepthBias;
}
//---------------------------------------------------------------------------
inline float NiShadowGenerator::GetDepthBias()
{
    return m_fDepthBias;
}
//---------------------------------------------------------------------------
inline NiStandardMaterial::LightType NiShadowGenerator::ConvertLightType(
    NiDynamicEffect::EffectType eEffectType)
{
    switch (eEffectType)
    {
        case NiDynamicEffect::SPOT_LIGHT:
        case NiDynamicEffect::SHADOWSPOT_LIGHT:
            return NiStandardMaterial::LIGHT_SPOT;
        case NiDynamicEffect::DIR_LIGHT:
        case NiDynamicEffect::SHADOWDIR_LIGHT:
            return NiStandardMaterial::LIGHT_DIR;
        case NiDynamicEffect::POINT_LIGHT:
        case NiDynamicEffect::SHADOWPOINT_LIGHT:
            return NiStandardMaterial::LIGHT_POINT;
        default:
            NIASSERT(!"Invalid NiDynamicEffect::EffectType!");
            return NiStandardMaterial::LIGHT_MAX;
    }
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::IsDepthBiasDefault()
{
    if (!m_pkShadowTechnique || !m_pkDynEffect ||
        m_pkDynEffect->GetEffectType() == NiDynamicEffect::AMBIENT_LIGHT)
    {
        return false;
    }

    NIASSERT(m_pkDynEffect);
    return (m_pkShadowTechnique->GetDefaultDepthBias(ConvertLightType(
        m_pkDynEffect->GetEffectType()), GetRenderBackfaces()) ==
        m_fDepthBias);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetDepthBiasToDefault()
{
    if (!m_pkShadowTechnique || !m_pkDynEffect ||
        m_pkDynEffect->GetEffectType() == NiDynamicEffect::AMBIENT_LIGHT)
    {
        return;
    }

    m_fDepthBias = m_pkShadowTechnique->GetDefaultDepthBias(ConvertLightType(
        m_pkDynEffect->GetEffectType()), GetRenderBackfaces());
}
//---------------------------------------------------------------------------
inline float NiShadowGenerator::GetNearClippingDist()
{
    return m_fNearClippingDist;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetNearClippingDist(float fNearClippingDist)
{
    m_fNearClippingDist = fNearClippingDist;
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::GetAutoCalcNearClippingDist()
{
    return GetBit(AUTO_CALC_NEAR_DIST);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetAutoCalcNearClippingDist(
    bool bAutoCalc)
{
    SetBit(bAutoCalc, AUTO_CALC_NEAR_DIST);
}
//---------------------------------------------------------------------------
inline float NiShadowGenerator::GetFarClippingDist()
{
    return m_fFarClippingDist;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetFarClippingDist(float fFarClippingDist)
{
    m_fFarClippingDist = fFarClippingDist;
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::GetAutoCalcFarClippingDist()
{
    return GetBit(AUTO_CALC_FAR_DIST);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetAutoCalcFarClippingDist(bool bAutoCalc)
{
    SetBit(bAutoCalc, AUTO_CALC_FAR_DIST);
}
//---------------------------------------------------------------------------
inline float NiShadowGenerator::GetDirLightFrusturmWidth()
{
    return m_fDirLightFrustumWidth;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetDirLightFrusturmWidth(
    float fDirLightFrustumWidth)
{
    m_fDirLightFrustumWidth = fDirLightFrustumWidth;
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::GetAutoCalcDirLightFrusturmWidth()
{
    return GetBit(AUTO_CALC_DIR_FRUSTUM_WIDTH);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetAutoCalcDirLightFrusturmWidth(bool bAutoCalc)
{
    SetBit(bAutoCalc, AUTO_CALC_DIR_FRUSTUM_WIDTH);
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::GetAutoCalcDirLightFrusturmPosition()
{
    return GetBit(AUTO_CALC_DIR_FRUSTUM_POSITION);
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetAutoCalcDirLightFrusturmPosition(
    bool bAutoCalc)
{
    SetBit(bAutoCalc, AUTO_CALC_DIR_FRUSTUM_POSITION);
}
//---------------------------------------------------------------------------
inline unsigned short NiShadowGenerator::GetFlags()
{
    return m_uFlags;
}
//---------------------------------------------------------------------------
inline void NiShadowGenerator::SetRenderViewsDirty(bool bDirty)
{
    SetBit(bDirty, DIRTY_RENDERVIEWS);

    // Do not mark shadow maps as dirty if the generator is static.
    if (bDirty && !GetBit(STATIC))
    {
        // If the render views are dirty this inherently means the associated
        // shadow maps must be dirty as well.
        MarkShadowMapsDirty();
    }
}
//---------------------------------------------------------------------------
inline bool NiShadowGenerator::GetRenderViewsDirty()
{
    return GetBit(DIRTY_RENDERVIEWS);
}
//---------------------------------------------------------------------------
