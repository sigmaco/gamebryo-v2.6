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
inline NiShadowManager* NiShadowManager::GetShadowManager()
{
    return ms_pkShadowManager;
}
//---------------------------------------------------------------------------
inline bool NiShadowManager::AreShaderModelsValid()
{
    NIASSERT(ms_pkShadowManager);
    return ms_pkShadowManager->m_bValidShaderModels;
}
//---------------------------------------------------------------------------
inline NiMaterial* NiShadowManager::GetShadowWriteMaterial(
    NiStandardMaterial::LightType eType)
{
    NIASSERT(ms_pkShadowManager);
    NIASSERT(eType < NiStandardMaterial::LIGHT_MAX);
    return ms_pkShadowManager->m_apkShadowWriteMaterial[eType];
}
//---------------------------------------------------------------------------
inline NiMaterialSwapProcessorPtr NiShadowManager::GetMaterialSwapProcessor(
    NiStandardMaterial::LightType eType)
{
    NIASSERT(ms_pkShadowManager);
    NIASSERT(eType < NiStandardMaterial::LIGHT_MAX);
    return ms_pkShadowManager->m_aspMaterialSwapProcessor[eType];
}
//---------------------------------------------------------------------------
inline NiShadowClickValidatorPtr NiShadowManager::GetShadowClickValidator()
{
    NIASSERT(ms_pkShadowManager);
    return ms_pkShadowManager->m_spShadowClickValidator;
}
//---------------------------------------------------------------------------
inline NiShadowTechnique* NiShadowManager::GetActiveShadowTechnique(
    unsigned short usSlot)
{
    NIASSERT(ms_pkShadowManager);
    NIASSERT(usSlot < MAX_ACTIVE_SHADOWTECHNIQUES);
    return ms_pkShadowManager->m_pkActiveShadowTechniques[usSlot];
}
//---------------------------------------------------------------------------
inline NiShadowTechnique* NiShadowManager::GetKnownShadowTechnique(
    unsigned short usID)
{
    NIASSERT(ms_pkShadowManager);
    NIASSERT(usID < MAX_KNOWN_SHADOWTECHNIQUES);
    return ms_pkShadowManager->m_pkKnownShadowTechniques[usID];
}
//---------------------------------------------------------------------------
inline NiTListIterator NiShadowManager::RequestClosestShadowMap(
    unsigned int uiDesiredHeight, unsigned int uiDesiredWidth,
    NiTexture::FormatPrefs& kFormatPrefs, bool bExactSizeOnly)
{
    NiShadowMap* pkShadowMap = NULL;
    return RequestClosestShadowMap(uiDesiredHeight, uiDesiredWidth, 
        kFormatPrefs, pkShadowMap, bExactSizeOnly);
}
//---------------------------------------------------------------------------
inline NiTListIterator NiShadowManager::RequestClosestShadowCubeMap(
    unsigned int uiDesiredSize, NiTexture::FormatPrefs& kFormatPrefs, 
    bool bExactSizeOnly)
{
    NiShadowCubeMap* pkShadowCubeMap = NULL;
    return RequestClosestShadowCubeMap(uiDesiredSize, kFormatPrefs,
        pkShadowCubeMap, bExactSizeOnly);
}
//---------------------------------------------------------------------------
inline const NiTObjectPtrSet<NiShadowClickGeneratorPtr>&
    NiShadowManager::GetShadowClickGenerators()
{
    NIASSERT(ms_pkShadowManager);
    return ms_pkShadowManager->m_kShadowClickGenerators;
}
//---------------------------------------------------------------------------
inline NiCamera* NiShadowManager::GetSceneCamera()
{
    NIASSERT(ms_pkShadowManager);
    return ms_pkShadowManager->m_spSceneCamera;
}
//---------------------------------------------------------------------------
inline NiCullingProcess* NiShadowManager::GetCullingProcess()
{
    NIASSERT(ms_pkShadowManager);
    return ms_pkShadowManager->m_spCullingProcess;
}
//---------------------------------------------------------------------------
inline void NiShadowManager::MakeShadowMapUnavailable(NiShadowMap* pkShadowMap)
{
    NIASSERT(ms_pkShadowManager);
    ms_pkShadowManager->m_kAvailableShadowMaps.Remove(pkShadowMap);
}
//---------------------------------------------------------------------------
inline NiShadowMap* NiShadowManager::MakeShadowMapUnavailable(
    NiTListIterator kSMIter)
{
    NIASSERT(ms_pkShadowManager);
    NIASSERT(kSMIter);
    return ms_pkShadowManager->m_kAvailableShadowMaps.RemovePos(kSMIter);
}
//---------------------------------------------------------------------------
inline void NiShadowManager::MakeShadowCubeMapUnavailable(
    NiShadowCubeMap* pkShadowCubeMap)
{
    NIASSERT(ms_pkShadowManager);    
    ms_pkShadowManager->m_kAvailableShadowCubeMaps.Remove(pkShadowCubeMap);
}
//---------------------------------------------------------------------------
inline NiShadowCubeMap* NiShadowManager::MakeShadowCubeMapUnavailable(
    NiTListIterator kSMIter)
{
    NIASSERT(ms_pkShadowManager);
    NIASSERT(kSMIter);
    return ms_pkShadowManager->m_kAvailableShadowCubeMaps.RemovePos(kSMIter);
}
//---------------------------------------------------------------------------
inline void NiShadowManager::AppendRenderClick(NiRenderClick* pkClick)
{
    NIASSERT(ms_pkShadowManager);

    NIASSERT(pkClick);
    ms_pkShadowManager->m_kRenderClicks.AddTail(pkClick);
}
//---------------------------------------------------------------------------
inline unsigned int NiShadowManager::GetMaxActiveShadowGenerators()
{
    NIASSERT(ms_pkShadowManager);
    return ms_pkShadowManager->m_uiMaxActiveShadowGenerators;
}
//--------------------------------------------------------------------------
inline bool NiShadowManager::GetActive()
{
    NIASSERT(ms_pkShadowManager);
    return ms_pkShadowManager->m_bActive;
}
//--------------------------------------------------------------------------
inline const NiTPointerList<NiShadowGeneratorPtr>& 
    NiShadowManager::GetShadowGenerators()
{
    NIASSERT(ms_pkShadowManager);
    return ms_pkShadowManager->m_kGenerators;
}
//---------------------------------------------------------------------------
inline const NiTPointerList<NiShadowGeneratorPtr>& 
    NiShadowManager::GetActiveShadowGenerators()
{
    NIASSERT(ms_pkShadowManager);
    return ms_pkShadowManager->m_kActiveGenerators;
}
//---------------------------------------------------------------------------
inline void NiShadowManager::SetPurgeShadowMapsCallback(
    NiShadowManager::PurgeShadowMaps pfnCallback)
{
    NIASSERT(ms_pkShadowManager);
    NIASSERT(pfnCallback);
    ms_pkShadowManager->m_pfnPurgeShadowMapsCallback = pfnCallback;
}
//---------------------------------------------------------------------------
inline void NiShadowManager::SetMaxTextureMemoryAllocation(unsigned int uiMax)
{
    ms_pkShadowManager->m_uiMaxTexureMemoryAllocation = uiMax;

    if (ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation >
        ms_pkShadowManager->m_uiMaxTexureMemoryAllocation)
    {
        // The currently texture memory allocation exceeds the new maximum
        // allowed texture memory. Purge shadow maps until we no longer exceed
        // this limit.
        NIASSERT(ms_pkShadowManager->m_pfnPurgeShadowMapsCallback);
        ms_pkShadowManager->m_pfnPurgeShadowMapsCallback(
            0,
            ms_pkShadowManager->m_kActiveGenerators,
            ms_pkShadowManager->m_kAvailableShadowMaps,
            ms_pkShadowManager->m_kAvailableShadowCubeMaps,
            true);
        
        // If this assert is hit it means the Purge Shadow maps callback failed
        // to purge enough texture memory. This should never happen, it is the 
        // responsibility of that callback to ensure the maximum allowed 
        // texture memory allocation is not exceeded.
        NIASSERT(ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation <= 
            ms_pkShadowManager->m_uiMaxTexureMemoryAllocation);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiShadowManager::GetMaxTextureMemoryAllocation()
{
    return ms_pkShadowManager->m_uiMaxTexureMemoryAllocation;
}
//---------------------------------------------------------------------------
inline const unsigned int NiShadowManager::GetCurrentTextureMemoryAllocation()
{
    return ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation;
}
//---------------------------------------------------------------------------
