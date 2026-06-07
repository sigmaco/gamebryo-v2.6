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

// Precompiled Header
#include "NiMainPCH.h"
#include "NiShadowManager.h"
#include "NiShadowTechnique.h"
#include "NiVSMShadowTechnique.h"
#include "NiShadowGenerator.h"
#include "NiShadowClickGenerator.h"
#include "NiRenderedCubeMap.h"
#include "NiTAbstractPoolAllocator.h"
#include "NiDefaultShadowClickGenerator.h"

#if defined(_WII)
#include "NiWiiShadowWriteMaterial.h"
#else
#include "NiDirectionalShadowWriteMaterial.h"
#include "NiPointShadowWriteMaterial.h"
#include "NiSpotShadowWriteMaterial.h"
#include "DirectionalShadowWriteMaterialNodeLibrary/\
NiDirectionalShadowWriteMaterialNodeLibrary.h"
#include "SpotShadowWriteMaterialNodeLibrary/\
NiSpotShadowWriteMaterialNodeLibrary.h"
#include "PointShadowWriteMaterialNodeLibrary/\
NiPointShadowWriteMaterialNodeLibrary.h"
#endif

NiAllocatorDeclareStatics(NiShadowMap, sizeof(NiShadowMap) * 16);
NiAllocatorDeclareStatics(NiShadowCubeMap, sizeof(NiShadowCubeMap) * 16);

NiShadowManager* NiShadowManager::ms_pkShadowManager = NULL;

#if defined(_WII) && defined(NIDEBUG)
bool NiShadowManager::ms_bWarnedAboutWiiCubeMaps = false;
#endif

//---------------------------------------------------------------------------
NiShadowManager::NiShadowManager() : 
    m_iActiveShadowClickGeneratorIndex(-1),
    m_uiCurrentTextureMemoryAllocation(0),
    m_pfnPurgeShadowMapsCallback(DefaultPurgeCallback),
    m_uiMaxActiveShadowGenerators(8), 
    m_uiMaxTexureMemoryAllocation(1024 * 1024 * 64), // 64Mb
    m_bActive(true),
    m_bValidShaderModels(false)
{
    // Create ShadowWriteMaterials 

#if defined(_WII)
    // The Wii does not use fragment materials or require different
    // shaders per light type, so a NiWiiShadowWriteMaterial
    // specific to the Wii is used for all light types.
    NiMaterial* pkShadowWriteMat = 
        NiMaterial::GetMaterial("NiWiiShadowWriteMat");
    if (!pkShadowWriteMat)
        pkShadowWriteMat = NiNew NiWiiShadowWriteMaterial();

    m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_DIR] = 
        pkShadowWriteMat;
    m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_SPOT] = 
        pkShadowWriteMat;
    m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_POINT] = 
        pkShadowWriteMat;
#else
    NiMaterialNodeLibraryPtr spDirectionalMaterialNodeLibrary = 
        NiDirectionalShadowWriteMaterialNodeLibrary::
        CreateMaterialNodeLibrary();
    NiMaterialNodeLibraryPtr spSpotMaterialNodeLibrary = 
        NiSpotShadowWriteMaterialNodeLibrary::CreateMaterialNodeLibrary();
    NiMaterialNodeLibraryPtr spPointMaterialNodeLibrary = 
        NiPointShadowWriteMaterialNodeLibrary::CreateMaterialNodeLibrary();

    m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_DIR] =
        NiDynamicCast(NiFragmentMaterial, NiMaterial::GetMaterial(
        "NiDirShadowWriteMat"));
    if (!m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_DIR])
    {
        m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_DIR] =
            NiNew NiDirectionalShadowWriteMaterial(
            spDirectionalMaterialNodeLibrary, false);
    }
    m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_SPOT] =
        NiDynamicCast(NiFragmentMaterial, NiMaterial::GetMaterial(
        "NiSpotShadowWriteMat"));
    if (!m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_SPOT])
    {
        m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_SPOT] =
            NiNew NiSpotShadowWriteMaterial(spSpotMaterialNodeLibrary, false);
    }
    m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_POINT] =
        NiDynamicCast(NiFragmentMaterial, NiMaterial::GetMaterial(
        "NiPointShadowWriteMat"));
    if (!m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_POINT])
    {
        m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_POINT] =
            NiNew NiPointShadowWriteMaterial(spPointMaterialNodeLibrary,
            false);
    }
#endif

    for (unsigned int ui = 0; ui < MAX_ACTIVE_SHADOWTECHNIQUES; ui++)
        m_pkActiveShadowTechniques[ui] = NULL;

    for (unsigned int ui = 0; ui < MAX_KNOWN_SHADOWTECHNIQUES; ui++)
        m_pkKnownShadowTechniques[ui] = NULL;

    m_aspMaterialSwapProcessor[NiStandardMaterial::LIGHT_DIR] = 
        NiNew NiMaterialSwapProcessor(
        m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_DIR]);
    m_aspMaterialSwapProcessor[NiStandardMaterial::LIGHT_SPOT] = 
        NiNew NiMaterialSwapProcessor(
        m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_SPOT]);
    m_aspMaterialSwapProcessor[NiStandardMaterial::LIGHT_POINT] = 
        NiNew NiMaterialSwapProcessor(
        m_apkShadowWriteMaterial[NiStandardMaterial::LIGHT_POINT]);

    m_spShadowClickValidator = NiNew NiShadowClickValidator;
}
//---------------------------------------------------------------------------
NiShadowManager::~NiShadowManager()
{
    // Remove all NiShadowGenerator objects
    m_kGenerators.RemoveAll();
    m_kActiveGenerators.RemoveAll();

    // Ensure that all shadow map objects have been released.
    DestroyAllShadowMaps();
    DestroyAllShadowCubeMaps();

    NIASSERT(m_kShadowMapPool.GetSize() == 0);
    NIASSERT(m_kShadowCubeMapPool.GetSize() == 0);

    for (unsigned int ui = 0; ui < MAX_KNOWN_SHADOWTECHNIQUES; ui++)
        NiDelete m_pkKnownShadowTechniques[ui];

    NiAllocatorShutdown(NiShadowMap);
    NiAllocatorShutdown(NiShadowCubeMap);
}
//---------------------------------------------------------------------------
void NiShadowManager::Initialize()
{
    NIASSERT(!ms_pkShadowManager);
    ms_pkShadowManager = NiNew NiShadowManager();
    NIASSERT(ms_pkShadowManager);

    // The Wii's shadow techniques are all platform specific
    // and are set up and registered by the NiWiiRenderer.
#if !defined(_WII)
    // Setup NiStandardShadowTechnique
    NiShadowTechnique* pkStandardTechnique = NiNew NiShadowTechnique(
      "NiStandardShadowTechnique", 
      "ShadowMap", "ShadowCubeMap", "SpotShadowMap",
      "WriteDepthToColor", "WriteDepthToColor", "WriteDepthToColor", 
      true, false);
    pkStandardTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_SPOT, false, 0.0001f);
    pkStandardTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_SPOT, true, 0.0f);
    pkStandardTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_DIR, false, 0.0001f);
    pkStandardTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_DIR, true, 0.0f);
    pkStandardTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_POINT, false, 0.96f);
    pkStandardTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_POINT, true, 0.98f);
    RegisterShadowTechnique(pkStandardTechnique, 0, 0);

    // Setup NiPCFShadowTechnique
    NiShadowTechnique* pkPCFTechnique = NiNew NiShadowTechnique(
        "NiPCFShadowTechnique", 
        "PCFShadowMap", "ShadowCubeMap", "SpotPCFShadowMap",
        "WriteDepthToColor", "WriteDepthToColor", "WriteDepthToColor", 
        true, false);
    pkPCFTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_SPOT, false, 0.0001f);
    pkPCFTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_SPOT, true, 0.0f);
    pkPCFTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_DIR, false, 0.0001f);
    pkPCFTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_DIR, true, 0.0f);
    pkPCFTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_POINT, false, 0.96f);
    pkPCFTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_POINT, true, 0.98f);
    RegisterShadowTechnique(pkPCFTechnique, 1, 1);


    // Setup NiVSMShadowTechnique
    NiVSMShadowTechnique* pkVSMTechnique = NiNew NiVSMShadowTechnique(
        "NiVSMShadowTechnique", 
        "VSMShadowMap", "ShadowCubeMap", "SpotVSMShadowMap",
        "WriteVSMDepthToColor", "WriteDepthToColor", "WriteVSMDepthToColor", 
        true, false);
    NiTexture::FormatPrefs kPrefs;
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::FLOAT_COLOR_64;
    pkVSMTechnique->SetTextureFormatPrefs(kPrefs);
    pkVSMTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_SPOT, false, 0.0001f);
    pkVSMTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_SPOT, true, 0.0f);
    pkVSMTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_DIR, false, 0.0001f);
    pkVSMTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_DIR, true, 0.0f);
    pkVSMTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_POINT, false, 0.96f);
    pkVSMTechnique->SetDefaultDepthBias(
        NiStandardMaterial::LIGHT_POINT, true, 0.98f);
    RegisterShadowTechnique(pkVSMTechnique, 2, 2);
#endif

    RegisterShadowClickGenerator(NiNew NiDefaultShadowClickGenerator);
    SetActiveShadowClickGenerator("NiDefaultShadowClickGenerator");

    NiRegisterStream(NiShadowGenerator);

}
//---------------------------------------------------------------------------
void NiShadowManager::Shutdown()
{
    if (ms_pkShadowManager)
    {
        NiUnregisterStream(NiShadowGenerator);

        NiDelete ms_pkShadowManager;
        ms_pkShadowManager = NULL;
    }
}
//---------------------------------------------------------------------------
bool NiShadowManager::ValidateShaderVersions(
    unsigned short usVertexShaderVersionMajor, 
    unsigned short,
    unsigned short usGeometryShaderVersionMajor,
    unsigned short,
    unsigned short usPixelShaderVersionMajor,
    unsigned short)
{
    NIASSERT(ms_pkShadowManager);

    if (usVertexShaderVersionMajor < 
        (unsigned short)MIN_VERTEX_SHADER_VERSION ||
#if !defined(_PS3) && !defined(_XENON)
        usGeometryShaderVersionMajor < 
        (unsigned short)MIN_GEOMETRY_SHADER_VERSION ||
#endif
        usPixelShaderVersionMajor < 
        (unsigned short)MIN_PIXEL_SHADER_VERSION)
    {
        NiOutputDebugString("Warning: Current hardware does not support the "
            "required shader model for the shadowing system. The shadowing "
            "system will remain in-active for the lifetime of the "
            "application.\n");

        ms_pkShadowManager->m_bValidShaderModels = false;
        SetActive(false);
        return false;
    }

    ms_pkShadowManager->m_bValidShaderModels = true;
    return true;
}
//---------------------------------------------------------------------------
bool NiShadowManager::RegisterShadowTechnique(
    NiShadowTechnique* pkShadowTechnique, unsigned short usSlot, 
    unsigned short usShaderID)
{
    NIASSERT(ms_pkShadowManager);
    NIASSERT(pkShadowTechnique);
    NIASSERT(usSlot < MAX_ACTIVE_SHADOWTECHNIQUES);
    NIASSERT(usShaderID < MAX_KNOWN_SHADOWTECHNIQUES);

    if (ms_pkShadowManager->m_pkActiveShadowTechniques[usSlot] != NULL)
    {
        char cString[NI_MAX_PATH];
        NiSprintf(cString, NI_MAX_PATH, "Error: The ShadowTechnique [%s] "
            "attempted to register itself to an already used active "
            "ShadowTechnique slot.\n The slot [%d] is use by the "
            "ShadowTechnique [%s]\n", 
            (const char*)pkShadowTechnique->GetName(), usSlot,
            (const char*)
            ms_pkShadowManager->m_pkActiveShadowTechniques[usSlot]->GetName());

        NiOutputDebugString(cString);
        return false;
    }

    if (ms_pkShadowManager->m_pkKnownShadowTechniques[usShaderID] != NULL)
    {
        char cString[NI_MAX_PATH];
        NiSprintf(cString, NI_MAX_PATH, "Error: The ShadowTechnique [%s] "
            "attempted to register itself to an already used known "
            "ShadowTechnique ID.\n The ID [%d] is use by the "
            "ShadowTechnique [%s]\n", 
            (const char*)pkShadowTechnique->GetName(), usSlot,
            (const char*)
            ms_pkShadowManager->m_pkKnownShadowTechniques[usSlot]->GetName());

        NiOutputDebugString(cString);
        return false;
    }    

    ms_pkShadowManager->m_pkActiveShadowTechniques[usSlot] = 
        pkShadowTechnique;
    ms_pkShadowManager->m_pkKnownShadowTechniques[usShaderID] = 
        pkShadowTechnique;

    pkShadowTechnique->SetActiveTechniqueSlot(usSlot);
    pkShadowTechnique->SetTechniqueID(usShaderID);

    return true;
}
//---------------------------------------------------------------------------
bool NiShadowManager::AddShadowGenerator(NiShadowGenerator* pkGenerator)
{
    NIASSERT(pkGenerator);
    if (!ms_pkShadowManager)
    {
        NILOG("Shadow manager does not exist.\n");
        return false;
    }
   
    NiDynamicEffect* pkDynamicEffect = 
        pkGenerator->GetAssignedDynamicEffect();
    NIASSERT(pkDynamicEffect);

    // Ensure that no other shadow generator is already applied to the effect.
    NIASSERT(!pkDynamicEffect->GetShadowGenerator());
   
    pkDynamicEffect->AssignShadowGenerator(pkGenerator);

    ms_pkShadowManager->m_kGenerators.AddTail(pkGenerator);

    // Ensure we do not surpass the max allowed number of active shadow 
    // generators
    if (pkGenerator->GetActive())
    {
        if (ms_pkShadowManager->m_kActiveGenerators.GetSize() >= 
            ms_pkShadowManager->m_uiMaxActiveShadowGenerators ||
            !pkDynamicEffect->GetSwitch())
        {
            pkGenerator->SetActive(false);
        }
        else
        {
            ms_pkShadowManager->m_kActiveGenerators.AddTail(pkGenerator);
        }
    }

    // Get the active shadow click generator.
    if (ms_pkShadowManager->m_iActiveShadowClickGeneratorIndex >= 0)
    {
        NiShadowClickGenerator* pkShadowClickGenerator =
            ms_pkShadowManager->m_kShadowClickGenerators.GetAt(
            ms_pkShadowManager->m_iActiveShadowClickGeneratorIndex);
        NIASSERT(pkShadowClickGenerator);

        // Give the shadow click generator a chance to swap out the callback
        // functions for the new shadow generator.
        pkShadowClickGenerator->PrepareShadowGenerator(pkGenerator);
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiShadowManager::DeleteShadowGenerator(NiShadowGenerator* pkGenerator)
{
    // This will be called when a NiDynamicEffect object with a reference
    // to a NiShadowGenerator is deleted. It may also be called manually.
    NIASSERT(pkGenerator);

    pkGenerator->ReleaseAllShadowMaps();

    if (pkGenerator->GetActive())
    {
        ms_pkShadowManager->m_kActiveGenerators.Remove(pkGenerator);
    }

    ms_pkShadowManager->m_kGenerators.Remove(pkGenerator);

    return true;
}
//---------------------------------------------------------------------------
void NiShadowManager::ReorganizeActiveShadowGenerators()
{
    NIASSERT(ms_pkShadowManager);
    NiShadowClickGenerator* pkClickGenerator = 
        ms_pkShadowManager->m_kShadowClickGenerators.GetAt(
        ms_pkShadowManager->m_iActiveShadowClickGeneratorIndex);
        
    pkClickGenerator->ReorganizeActiveShadowGenerators(
        ms_pkShadowManager->m_kActiveGenerators,
        ms_pkShadowManager->m_kGenerators);
}
//---------------------------------------------------------------------------
NiShadowTechnique* NiShadowManager::GetActiveShadowTechnique(
    const NiFixedString& kTechniqueName)
{
    NIASSERT(ms_pkShadowManager);
    for (unsigned int ui = 0; ui < MAX_ACTIVE_SHADOWTECHNIQUES; ui++)
    {
        if (ms_pkShadowManager->m_pkActiveShadowTechniques[ui] && 
            ms_pkShadowManager->m_pkActiveShadowTechniques[ui]->GetName() == 
            kTechniqueName)
        {
            return ms_pkShadowManager->m_pkActiveShadowTechniques[ui];
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiShadowTechnique* NiShadowManager::GetKnownShadowTechnique(
    const NiFixedString& kTechniqueName)
{
    NIASSERT(ms_pkShadowManager);
    for (unsigned int ui = 0; ui < MAX_KNOWN_SHADOWTECHNIQUES; ui++)
    {
        if (ms_pkShadowManager->m_pkKnownShadowTechniques[ui] && 
            ms_pkShadowManager->m_pkKnownShadowTechniques[ui]->GetName() == 
            kTechniqueName)
        {
            return ms_pkShadowManager->m_pkKnownShadowTechniques[ui];
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiShadowMap* NiShadowManager::CreateShadowMap(
    unsigned int uiWidth, unsigned int uiHeight, 
    NiTexture::FormatPrefs& kPrefs, 
    const NiPixelFormat* pkDepthFormat,
    NiTexturingProperty::ClampMode eClampMode, 
    NiTexturingProperty::FilterMode eFilterMode)
{
    NIASSERT(ms_pkShadowManager);

    // Clamp the maximum allowed dimensions for a shadow map to 2048
    uiWidth = NiMin(uiWidth, 2048);
    uiHeight = NiMin(uiHeight, 2048);

    unsigned int uiEstimatedTexMemUsage = 
        ms_pkShadowManager->GetApproximateTexMemUsage(
        uiWidth, uiHeight, kPrefs, false);

    if (ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation +
        uiEstimatedTexMemUsage >
        ms_pkShadowManager->m_uiMaxTexureMemoryAllocation)
    {
#ifdef NIDEBUG
        unsigned uiMemoryOverflow = (uiEstimatedTexMemUsage +
            ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation) - 
            ms_pkShadowManager->m_uiMaxTexureMemoryAllocation;

        char acBuff[NI_MAX_PATH];
        NiSprintf(acBuff, NI_MAX_PATH, "Warning: Requested shadow map exceeds"
            " the maximum allowed shadow map texture memory by %d bytes "
            "(%d MB)\n", uiMemoryOverflow, uiMemoryOverflow / (1024 * 1024));
        NiOutputDebugString(acBuff);
#endif

        // Try and purge unused shadow maps before we fail out.
        NIASSERT(ms_pkShadowManager->m_pfnPurgeShadowMapsCallback);
        ms_pkShadowManager->m_pfnPurgeShadowMapsCallback(
            uiEstimatedTexMemUsage,
            ms_pkShadowManager->m_kActiveGenerators,
            ms_pkShadowManager->m_kAvailableShadowMaps,
            ms_pkShadowManager->m_kAvailableShadowCubeMaps,
            false);

        // Unable to purge enough texture memory so fail out.
        if (ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation +
            uiEstimatedTexMemUsage >
            ms_pkShadowManager->m_uiMaxTexureMemoryAllocation)
        {
            return NULL;
        }
    }

    NiShadowMap* pkShadowMap = 
        ms_pkShadowManager->m_kShadowMapPool.AddNewTail();

    if (!NiShadowMap::Initialize(pkShadowMap, uiWidth, uiHeight, kPrefs, 
        pkDepthFormat, eClampMode, eFilterMode))
    {
        // Failed to initialize the shadow map. This is most likely because the
        // requested texture format is not supported by the current hardware.
        ms_pkShadowManager->m_kShadowMapPool.RemoveTail();
        return NULL;
    }

    ms_pkShadowManager->m_kAvailableShadowMaps.AddTail(pkShadowMap);

    ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation += 
        uiEstimatedTexMemUsage;

    return pkShadowMap;
}
//---------------------------------------------------------------------------
void NiShadowManager::DestroyShadowMap(NiShadowMap* pkShadowMap)
{
    NIASSERT(ms_pkShadowManager);
    
    ms_pkShadowManager->m_kAvailableShadowMaps.Remove(pkShadowMap);
    ms_pkShadowManager->m_kShadowMapPool.Remove(pkShadowMap);

    unsigned int uiEstimatedMemUsage = 
        ms_pkShadowManager->GetApproximateTexMemUsage(
        pkShadowMap->GetTexture()->GetWidth(),
        pkShadowMap->GetTexture()->GetHeight(),
        pkShadowMap->GetTexture()->GetFormatPreferences(), false);

    ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation -=
        uiEstimatedMemUsage;

    NiShadowMap::Destroy(pkShadowMap);
}
//---------------------------------------------------------------------------
void NiShadowManager::DestroyAllShadowMaps()
{
    NIASSERT(ms_pkShadowManager);

    ms_pkShadowManager->m_kAvailableShadowMaps.RemoveAll();

    NiShadowMap* pkShadowMap = ms_pkShadowManager->m_kShadowMapPool.GetHead();
    while (pkShadowMap)
    {
        unsigned int uiEstimatedMemUsage = 
            ms_pkShadowManager->GetApproximateTexMemUsage(
            pkShadowMap->GetTexture()->GetWidth(),
            pkShadowMap->GetTexture()->GetHeight(),
            pkShadowMap->GetTexture()->GetFormatPreferences(), false);

        ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation -=
            uiEstimatedMemUsage;

        NiShadowMap::Destroy(pkShadowMap);

        ms_pkShadowManager->m_kShadowMapPool.RemoveHead();
        pkShadowMap = ms_pkShadowManager->m_kShadowMapPool.GetHead();
    }

    ms_pkShadowManager->m_kShadowMapPool.RemoveAll();
    ms_pkShadowManager->m_kAvailableShadowMaps.RemoveAll();
}
//---------------------------------------------------------------------------
NiShadowCubeMap* NiShadowManager::CreateShadowCubeMap(
    unsigned int uiSize,
    NiTexture::FormatPrefs& kPrefs, 
    const NiPixelFormat* pkDepthFormat,
    NiTexturingProperty::ClampMode eClampMode,
    NiTexturingProperty::FilterMode eFilterMode)
{
#if defined(_WII)
#if defined(NIDEBUG)
    if (!ms_bWarnedAboutWiiCubeMaps)
    {
        NiOutputDebugString("NiShadowManager: Cube map shadow maps are "
            "not supported on the Wii.  No shadows will be displayed when "
            "NiPointLight shadow generators use the "
            "NiDefaultShadowClickGenerator.\n");
        ms_bWarnedAboutWiiCubeMaps = true;
    }
#endif
    // Return false in all configurations on the Wii.
    return false;
#endif

    NIASSERT(ms_pkShadowManager);

    // Clamp the maximum allowed dimensions for a shadow map to 2048
    uiSize = NiMin(uiSize, 2048);

    unsigned int uiEstimatedTexMemUsage = 
        ms_pkShadowManager->GetApproximateTexMemUsage(
        uiSize, uiSize, kPrefs, true);

    if (ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation +
        uiEstimatedTexMemUsage >
        ms_pkShadowManager->m_uiMaxTexureMemoryAllocation)
    {
#ifdef NIDEBUG
        unsigned uiMemoryOverflow = (uiEstimatedTexMemUsage +
            ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation) - 
            ms_pkShadowManager->m_uiMaxTexureMemoryAllocation;

        char acBuff[NI_MAX_PATH];
        NiSprintf(acBuff, NI_MAX_PATH, "Warning: Requested shadow map exceeds"
            " the maximum allowed shadow map texture memory by %d bytes "
            "(%d MB)\n", uiMemoryOverflow, uiMemoryOverflow / (1024 * 1024));
        NiOutputDebugString(acBuff);
#endif

        // Try and purge unused shadow maps before we fail out.
        NIASSERT(ms_pkShadowManager->m_pfnPurgeShadowMapsCallback);
        ms_pkShadowManager->m_pfnPurgeShadowMapsCallback(
            uiEstimatedTexMemUsage,
            ms_pkShadowManager->m_kActiveGenerators,
            ms_pkShadowManager->m_kAvailableShadowMaps,
            ms_pkShadowManager->m_kAvailableShadowCubeMaps,
            false);

        // Unable to purge enough texture memory so fail out.
        if (ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation +
            uiEstimatedTexMemUsage >
            ms_pkShadowManager->m_uiMaxTexureMemoryAllocation)
        {
            return NULL;
        }
    }
    
    NiShadowCubeMap* pkShadowCubeMap =
        ms_pkShadowManager->m_kShadowCubeMapPool.AddNewTail();

    if (!NiShadowCubeMap::Initialize(pkShadowCubeMap, uiSize, kPrefs, 
        pkDepthFormat, eClampMode,eFilterMode))
    {
        // Failed to initialize the shadow map. This is most likely because the
        // requested texture format is not supported by the current hardware.
        ms_pkShadowManager->m_kShadowCubeMapPool.RemoveTail();
        return NULL;
    }


    ms_pkShadowManager->m_kAvailableShadowCubeMaps.AddTail(pkShadowCubeMap);
        
    ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation += 
        uiEstimatedTexMemUsage;

    return pkShadowCubeMap;
}
//---------------------------------------------------------------------------
void NiShadowManager::DestroyShadowCubeMap(NiShadowCubeMap* pkShadowCubeMap)
{
    NIASSERT(ms_pkShadowManager);

    ms_pkShadowManager->m_kAvailableShadowCubeMaps.Remove(pkShadowCubeMap);
    ms_pkShadowManager->m_kShadowCubeMapPool.Remove(pkShadowCubeMap);

    unsigned int uiEstimatedMemUsage = 
        ms_pkShadowManager->GetApproximateTexMemUsage(
        pkShadowCubeMap->GetTexture()->GetWidth(),
        pkShadowCubeMap->GetTexture()->GetHeight(),
        pkShadowCubeMap->GetTexture()->GetFormatPreferences(), true);

    NiShadowCubeMap::Destroy(pkShadowCubeMap);

    ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation -=
        uiEstimatedMemUsage;
}
//---------------------------------------------------------------------------
void NiShadowManager::DestroyAllShadowCubeMaps()
{
    NIASSERT(ms_pkShadowManager);

    ms_pkShadowManager->m_kAvailableShadowCubeMaps.RemoveAll();

    NiShadowCubeMap* pkShadowCubeMap =
        ms_pkShadowManager->m_kShadowCubeMapPool.GetHead();
    while (pkShadowCubeMap)
    {
        unsigned int uiEstimatedMemUsage = 
            ms_pkShadowManager->GetApproximateTexMemUsage(
            pkShadowCubeMap->GetTexture()->GetWidth(),
            pkShadowCubeMap->GetTexture()->GetHeight(),
            pkShadowCubeMap->GetTexture()->GetFormatPreferences(), true);

        ms_pkShadowManager->m_uiCurrentTextureMemoryAllocation -=
            uiEstimatedMemUsage;

        NiShadowCubeMap::Destroy(pkShadowCubeMap);

        ms_pkShadowManager->m_kShadowCubeMapPool.RemoveHead();
        pkShadowCubeMap = ms_pkShadowManager->m_kShadowCubeMapPool.GetHead();
    }

    ms_pkShadowManager->m_kShadowCubeMapPool.RemoveAll();
    ms_pkShadowManager->m_kAvailableShadowCubeMaps.RemoveAll();
}
//---------------------------------------------------------------------------
NiTListIterator NiShadowManager::RequestClosestShadowMap(
    unsigned int uiDesiredHeight, unsigned int uiDesiredWidth,
    NiTexture::FormatPrefs& kFormatPrefs, NiShadowMap*& pkClosestShadowMap, 
    bool bExactSizeOnly)
{
    NIASSERT(ms_pkShadowManager);

    int iDesiredSize = uiDesiredHeight * uiDesiredWidth;
    int iClosestSizeDiff = INT_MAX;
    NiTListIterator kClosestIter = NULL;
    NiTListIterator kPrevIter = NULL;
    pkClosestShadowMap = NULL;
    
    // Search through all the available shadow maps and find the shadow map
    // that size closest to the desired size.
    NiTListIterator kSMIter = 
        ms_pkShadowManager->m_kAvailableShadowMaps.GetHeadPos();
    while (kSMIter)
    {
        kPrevIter = kSMIter;
        NiShadowMap* pkShadowMap = 
            ms_pkShadowManager->m_kAvailableShadowMaps.GetNext(kSMIter);

        // Ensure we use a compatible texture format.
        NiTexture::FormatPrefs kSMFormat = 
            pkShadowMap->GetTexture()->GetFormatPreferences();
        if ( (kSMFormat.m_ePixelLayout != kFormatPrefs.m_ePixelLayout) ||
             (kSMFormat.m_eAlphaFmt != kFormatPrefs.m_eAlphaFmt) ||
             (kSMFormat.m_eMipMapped != kFormatPrefs.m_eMipMapped))
            continue;

        int iSizeDiff = pkShadowMap->GetTexture()->GetWidth() * 
            pkShadowMap->GetTexture()->GetHeight();

        iSizeDiff = abs(iSizeDiff - iDesiredSize);

        if (iSizeDiff == 0)
        {
            kClosestIter = kPrevIter;
            pkClosestShadowMap = pkShadowMap;
            break;
        }
        else if (!bExactSizeOnly && iSizeDiff < iClosestSizeDiff)
        {
            kClosestIter = kPrevIter;
            pkClosestShadowMap = pkShadowMap;
            iClosestSizeDiff = iSizeDiff;
        }
        
    }

    return kClosestIter;
}
//---------------------------------------------------------------------------
NiTListIterator NiShadowManager::RequestClosestShadowCubeMap(
    unsigned int uiDesiredSize, 
    NiTexture::FormatPrefs& kFormatPrefs,
    NiShadowCubeMap*& pkClosestShadowCubeMap,
    bool bExactSizeOnly)
{
    NIASSERT(ms_pkShadowManager);

    int iClosestSizeDiff = INT_MAX;
    NiTListIterator kClosestIter = NULL;
    NiTListIterator kPrevIter = NULL;
    pkClosestShadowCubeMap = NULL;
    
    // Search through all the available shadow maps and find the shadow map
    // that size closest to the desired size.
    NiTListIterator kSMIter = 
        ms_pkShadowManager->m_kAvailableShadowCubeMaps.GetHeadPos();
    while (kSMIter)
    {
        kPrevIter = kSMIter;
        NiShadowCubeMap* pkShadowCubeMap = 
            ms_pkShadowManager->m_kAvailableShadowCubeMaps.GetNext(kSMIter);

        // Ensure we use a compatible texture format.
        NiTexture::FormatPrefs kSMFormat = 
            pkShadowCubeMap->GetTexture()->GetFormatPreferences();
        if ( (kSMFormat.m_ePixelLayout != kFormatPrefs.m_ePixelLayout) ||
            (kSMFormat.m_eAlphaFmt != kFormatPrefs.m_eAlphaFmt) ||
            (kSMFormat.m_eMipMapped != kFormatPrefs.m_eMipMapped))
            continue;

        int iSizeDiff = pkShadowCubeMap->GetCubeMapTexture()->GetWidth();

        iSizeDiff = abs(iSizeDiff - (int) uiDesiredSize);

        if (iSizeDiff == 0)
        {
            kClosestIter = kPrevIter;
            pkClosestShadowCubeMap = pkShadowCubeMap;
            break;
        }
        else if (!bExactSizeOnly && iSizeDiff < iClosestSizeDiff)
        {
            kClosestIter = kPrevIter;
            pkClosestShadowCubeMap = pkShadowCubeMap;
            iClosestSizeDiff = iSizeDiff;
        }
    }

    return kClosestIter;
}
//---------------------------------------------------------------------------
bool NiShadowManager::RegisterShadowClickGenerator(
    NiShadowClickGenerator* pkShadowClickGenerator)
{
    NIASSERT(ms_pkShadowManager);
    NIASSERT(pkShadowClickGenerator);

    if (ms_pkShadowManager->GetShadowClickGenerator(
        pkShadowClickGenerator->GetName()))
    {
        return false;
    }

    ms_pkShadowManager->m_kShadowClickGenerators.Add(pkShadowClickGenerator);
    return true;
}
//---------------------------------------------------------------------------
NiShadowClickGenerator* NiShadowManager::GetShadowClickGenerator(
    const NiFixedString& kName)
{
    NIASSERT(ms_pkShadowManager);

    for (unsigned int ui = 0; ui < ms_pkShadowManager
        ->m_kShadowClickGenerators.GetSize(); ui++)
    {
        NiShadowClickGenerator* pkGenerator = ms_pkShadowManager
            ->m_kShadowClickGenerators.GetAt(ui);
        if (pkGenerator->GetName() == kName)
        {
            return pkGenerator;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool NiShadowManager::SetActiveShadowClickGenerator(
    NiShadowClickGenerator* pkActiveShadowClickGenerator)
{
    NIASSERT(ms_pkShadowManager);
    NIASSERT(pkActiveShadowClickGenerator);

    for (unsigned int ui = 0; ui < ms_pkShadowManager
        ->m_kShadowClickGenerators.GetSize(); ui++)
    {
        if (ms_pkShadowManager->m_kShadowClickGenerators.GetAt(ui) ==
            pkActiveShadowClickGenerator)
        {
            ms_pkShadowManager->m_iActiveShadowClickGeneratorIndex = ui;
            ms_pkShadowManager->OnActiveShadowClickGeneratorChanged();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiShadowManager::SetActiveShadowClickGenerator(
    const NiFixedString& kName)
{
    NIASSERT(ms_pkShadowManager);

    for (unsigned int ui = 0; ui < ms_pkShadowManager
        ->m_kShadowClickGenerators.GetSize(); ui++)
    {
        if (ms_pkShadowManager->m_kShadowClickGenerators.GetAt(ui)->GetName()
            == kName)
        {
            ms_pkShadowManager->m_iActiveShadowClickGeneratorIndex = ui;
            ms_pkShadowManager->OnActiveShadowClickGeneratorChanged();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiShadowManager::OnActiveShadowClickGeneratorChanged()
{
    // Get the active shadow click generator.
    NIASSERT(m_iActiveShadowClickGeneratorIndex >= 0);
    NiShadowClickGenerator* pkShadowClickGenerator = m_kShadowClickGenerators
        .GetAt(m_iActiveShadowClickGeneratorIndex);
    NIASSERT(pkShadowClickGenerator);

    // Tell each shadow generator to clean up its extra data, since its
    // callback functions will likely be changed by the new shadow click
    // generator.
    NiTListIterator kIter = m_kGenerators.GetHeadPos();
    while (kIter)
    {
        NiShadowGenerator* pkShadowGenerator = m_kGenerators.GetNext(kIter);
        NiShadowGenerator::CleanupExtraDataCallback pfnCallback =
            pkShadowGenerator->GetExtraDataCleanupCallbackFunc();
        if (pfnCallback)
        {
            pfnCallback(pkShadowGenerator);
        }
    }

    // De-allocate all shadow maps in the m_kAvailableShadowCubeMaps pool.
    DestroyAllShadowMaps();

    // Give the shadow click generator a chance to swap out the callback
    // functions for each generator.
    pkShadowClickGenerator->PrepareShadowGenerators(m_kGenerators);
}
//---------------------------------------------------------------------------
void NiShadowManager::SetSceneCamera(NiCamera* pkSceneCamera)
{
    NIASSERT(ms_pkShadowManager);

    NIASSERT(pkSceneCamera);
    ms_pkShadowManager->m_spSceneCamera = pkSceneCamera;

    // Update the camera in the shadow click validator.
    ms_pkShadowManager->m_spShadowClickValidator->SetCamera(
        ms_pkShadowManager->m_spSceneCamera);
}
//---------------------------------------------------------------------------
void NiShadowManager::SetCullingProcess(NiCullingProcess* pkCullingProcess)
{
    NIASSERT(ms_pkShadowManager);

    NIASSERT(pkCullingProcess);
    ms_pkShadowManager->m_spCullingProcess = pkCullingProcess;

    // Update the culling process in the shadow click validator.
    ms_pkShadowManager->m_spShadowClickValidator->SetCullingProcess(
        ms_pkShadowManager->m_spCullingProcess);

    // Update the culling process for all shadow maps.
    NiTListIterator kIter = ms_pkShadowManager->m_kShadowMapPool.GetHeadPos();
    while (kIter)
    {
        NiShadowMap* pkShadowMap =
            ms_pkShadowManager->m_kShadowMapPool.GetNext(kIter);

        NiShadowRenderClick* pkShadowClick = pkShadowMap->GetRenderClick();
        NIASSERT(pkShadowClick);

        NIASSERT(pkShadowClick->GetRenderViews().GetSize() > 0 && NiIsKindOf(
            Ni3DRenderView, pkShadowClick->GetRenderViews().GetHead()));
        Ni3DRenderView* pkShadowView = NiSmartPointerCast(Ni3DRenderView,
            pkShadowClick->GetRenderViews().GetHead());

        pkShadowView->SetCullingProcess(
            ms_pkShadowManager->m_spCullingProcess);
    }

    // Update the culling process for all shadow cube maps.
    kIter = ms_pkShadowManager->m_kShadowCubeMapPool.GetHeadPos();
    while (kIter)
    {
        NiShadowCubeMap* pkShadowCubeMap = ms_pkShadowManager
            ->m_kShadowCubeMapPool.GetNext(kIter);

        for (unsigned char uc = 0; uc < pkShadowCubeMap->GetNumRenderClicks();
            uc++)
        {
            NiShadowRenderClick* pkShadowClick =
                pkShadowCubeMap->GetRenderClick(uc);
            NIASSERT(pkShadowClick);

            NIASSERT(pkShadowClick->GetRenderViews().GetSize() > 0);
            NIASSERT(NiIsKindOf(Ni3DRenderView,
                pkShadowClick->GetRenderViews().GetHead()));
            Ni3DRenderView* pkShadowView = NiSmartPointerCast(Ni3DRenderView,
                pkShadowClick->GetRenderViews().GetHead());

            pkShadowView->SetCullingProcess(
                ms_pkShadowManager->m_spCullingProcess);
        }
    }
}
//---------------------------------------------------------------------------
const NiTPointerList<NiRenderClick*>&
    NiShadowManager::GenerateRenderClicks()
{
    NIASSERT(ms_pkShadowManager);

    NIASSERT(ms_pkShadowManager->m_iActiveShadowClickGeneratorIndex >= 0);
    NIASSERT(ms_pkShadowManager->m_kShadowClickGenerators.GetAt(
        ms_pkShadowManager->m_iActiveShadowClickGeneratorIndex));
    NIASSERT(ms_pkShadowManager->m_spSceneCamera);
    NIASSERT(ms_pkShadowManager->m_spCullingProcess);

    ms_pkShadowManager->m_kRenderClicks.RemoveAll();

    if (ms_pkShadowManager->m_bActive)
    {
        NIVERIFY(ms_pkShadowManager->m_kShadowClickGenerators.GetAt(
            ms_pkShadowManager->m_iActiveShadowClickGeneratorIndex)
            ->GenerateRenderClicks(ms_pkShadowManager->m_kActiveGenerators));
    }

    return ms_pkShadowManager->m_kRenderClicks;
}
//---------------------------------------------------------------------------
void NiShadowManager::MakeShadowMapAvailable(NiShadowMap* pkShadowMap)
{
    NIASSERT(ms_pkShadowManager);

    NiShadowRenderClick* pkClick = pkShadowMap->GetRenderClick();

    if (pkClick)
    {
        NIASSERT(pkClick->GetRenderViews().GetSize() > 0);
        Ni3DRenderView* pkShadowView = NiDynamicCast(Ni3DRenderView,
            pkClick->GetRenderViews().GetHead());

        // Clear all scenes from the render view.
        NIASSERT(pkShadowView);
        pkShadowView->GetScenes().RemoveAll();
    }

    ms_pkShadowManager->m_kAvailableShadowMaps.AddTail(pkShadowMap);
}
//---------------------------------------------------------------------------
void NiShadowManager::MakeShadowCubeMapAvailable(
    NiShadowCubeMap* pkShadowCubeMap)
{
    NIASSERT(ms_pkShadowManager);

    unsigned int uiClickCount = pkShadowCubeMap->GetNumRenderClicks();
    for (unsigned int ui = 0; ui < uiClickCount; ui++)
    {
        NiShadowRenderClick* pkClick =
            pkShadowCubeMap->GetRenderClick((unsigned char)ui);

        if (pkClick)
        {
            NIASSERT(pkClick->GetRenderViews().GetSize() > 0);
            Ni3DRenderView* pkShadowView = NiDynamicCast(Ni3DRenderView,
                pkClick->GetRenderViews().GetHead());

            // Clear all scenes from the render view.
            NIASSERT(pkShadowView);
            pkShadowView->GetScenes().RemoveAll();
        }
    }

    ms_pkShadowManager->m_kAvailableShadowCubeMaps.AddTail(pkShadowCubeMap);
}
//---------------------------------------------------------------------------
bool NiShadowManager::SetShadowGeneratorActiveState(
    NiShadowGenerator* pkGenerator, bool bActive)
{
    NIASSERT(ms_pkShadowManager);

    // Work with shadow generator as a smart pointer so it does not get
    // DecRef() deleted when working with the generator lists.
    NiShadowGeneratorPtr spGenerator = pkGenerator;

    if (bActive)
    {
        // Ensure we have are allowed to add more active shadow generators
        if (ms_pkShadowManager->m_kActiveGenerators.GetSize() >= 
            ms_pkShadowManager->m_uiMaxActiveShadowGenerators)
            return false;

        // Do not add the shadow generator to the active list if it is already
        // in the list.
        if (ms_pkShadowManager->m_kActiveGenerators.FindPos(spGenerator))
            return true;

        ms_pkShadowManager->m_kActiveGenerators.AddTail(spGenerator);
    }
    else
    {
        ms_pkShadowManager->m_kActiveGenerators.Remove(spGenerator);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiShadowManager::SetMaxActiveShadowGenerators(unsigned int uiMax)
{
    NIASSERT(ms_pkShadowManager);
    
    if (ms_pkShadowManager->m_kActiveGenerators.GetSize() > uiMax)
    {
        // Remove the last m_kActiveGenerators.GetSize() - uiMax generators
        // from the list of active shadow generators.
        // Note that this will simply chop off the extra NiShadowGenerators
        // from the end of the list. If this is not the desired functionality, 
        // it is up the the NiShadowClickGenrator to choose the most 
        // appropriate NiShadowGenerators to keep active.

        unsigned int uiOverflow = 
            ms_pkShadowManager->m_kActiveGenerators.GetSize() - uiMax;

        for (unsigned int ui = 0; ui < uiOverflow; ui++)
        {
            NiShadowGenerator* pkGenerator = 
                ms_pkShadowManager->m_kActiveGenerators.GetTail();
            pkGenerator->SetActive(false);
            pkGenerator->ForceMaterialNeedsUpdate();
        }
    }

    ms_pkShadowManager->m_uiMaxActiveShadowGenerators = uiMax;

}
//---------------------------------------------------------------------------
void NiShadowManager::SetActive(bool bActive, bool bRetainShadowMaps)
{
    NIASSERT(ms_pkShadowManager);

    if (bActive == ms_pkShadowManager->m_bActive)
        return;

    // Do not allow the shadow manager to activated if the current hardware 
    // does not support the shader models required by the shadowing system.
    if (bActive == true && !ms_pkShadowManager->m_bValidShaderModels)
        return;

    // Change active state and inform all shadow receiving geometry that
    // they need to update their materials
    ms_pkShadowManager->m_bActive = bActive;

    NiTListIterator kIter =  ms_pkShadowManager->m_kGenerators.GetHeadPos();
    while (kIter)
    {
        NiShadowGenerator* pkGenerator = 
            ms_pkShadowManager->m_kGenerators.GetNext(kIter);
        pkGenerator->SetActive(bActive, true, bRetainShadowMaps);
        pkGenerator->ForceMaterialNeedsUpdate();
    }

}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiShadowManager::GetCompatibleDepthStencil(
    Ni2DBuffer* pkBuffer, const NiPixelFormat* pkDepthFormat)
{   
    NIASSERT(ms_pkShadowManager);
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();

    // On the Wii, depth data is copied out to the depth/stencil
    // object's renderer data, so two shadow maps cannot reuse
    // the same depth/stencil buffer.  Therefore, on the Wii,
    // we must always create a new depth/stencil buffer when
    // one is required by a new shadow map.
#if !defined(_WII)
    // Try and find a compatible depth stencil buffer.
    NiTListIterator kIter = ms_pkShadowManager->m_kShadowMapPool.GetHeadPos();

    while (kIter)
    {
        NiShadowMap* pkShadowMap = 
            ms_pkShadowManager->m_kShadowMapPool.GetNext(kIter);
        
        NiRenderClick* pkRenderClick = pkShadowMap->GetRenderClick();

        if (!pkRenderClick)
            continue;

        NiRenderTargetGroup* pkRTG = pkRenderClick->GetRenderTargetGroup();

        if (!pkRTG)
            continue;

        NiDepthStencilBuffer* pkDSBuffer = pkRTG->GetDepthStencilBuffer();

        if (!pkDSBuffer)
            continue;

        if (pkDepthFormat)
        {
            // If a specific depth format is required, only reuse a buffer
            // with that format.
            if (pkDepthFormat->FunctionallyIdentical(
                    *(pkDSBuffer->GetPixelFormat()), true))
                return pkDSBuffer;
        }
        else
        {
            // If no specific depth format is required, reuse any compatible
            // depth buffer.
            if (pkRenderer->IsDepthBufferCompatible(pkBuffer, pkDSBuffer))
                return pkDSBuffer;
        }
    }

    kIter = ms_pkShadowManager->m_kShadowCubeMapPool.GetHeadPos();
    while (kIter)
    {
        NiShadowCubeMap* pkShadowCubeMap = 
            ms_pkShadowManager->m_kShadowCubeMapPool.GetNext(kIter);
        unsigned int ui;

        for (ui = 0; ui < pkShadowCubeMap->GetNumRenderClicks(); ui++)
        {
            NiRenderClick* pkRenderClick =
                pkShadowCubeMap->GetRenderClick((unsigned char)ui);

            if (!pkRenderClick)
                continue;

            NiRenderTargetGroup* pkRTG = pkRenderClick->GetRenderTargetGroup();

            if (!pkRTG)
                continue;

            NiDepthStencilBuffer* pkDSBuffer = pkRTG->GetDepthStencilBuffer();

            if (!pkDSBuffer)
                continue;

            if (pkDepthFormat)
            {
                // If a specific depth format is required, only reuse a buffer
                // with that format.
                if (pkDepthFormat->FunctionallyIdentical(
                        *(pkDSBuffer->GetPixelFormat()), true))
                    return pkDSBuffer;
            }
            else
            {
                // If no specific depth format is required, reuse any compatible
                // depth buffer.
                if (pkRenderer->IsDepthBufferCompatible(pkBuffer, pkDSBuffer))
                    return pkDSBuffer;
            }
        }
    }
#endif

    // A reusable depth stencil buffer was not found.  If a specific depth 
    // format is required, create a new one with that format.  Otherwise,
    // create a new one compatible with the color buffer pixel format.
    if (pkDepthFormat)
    {
        return NiDepthStencilBuffer::Create(
            pkBuffer->GetWidth(), 
            pkBuffer->GetHeight(),
            pkRenderer, *pkDepthFormat, 
            pkBuffer->GetMSAAPref());
    }
    else
    {
        return NiDepthStencilBuffer::CreateCompatible(pkBuffer, pkRenderer);
    }
}
//---------------------------------------------------------------------------
unsigned int NiShadowManager::GetApproximateTexMemUsage(
    const unsigned int uiWidth, const unsigned int uiHeight, 
    const NiTexture::FormatPrefs& kPrefs, const bool bCubeMap)
{
    // Estimate size of texture map.
    unsigned int uiTexMemSize = uiHeight * uiWidth;
    
    if (bCubeMap)
        uiTexMemSize *= 6;

    switch (kPrefs.m_ePixelLayout)
    {
    case NiTexture::FormatPrefs::PALETTIZED_4:
        uiTexMemSize /= 2;
        break;

    case NiTexture::FormatPrefs::PALETTIZED_8:
    case NiTexture::FormatPrefs::SINGLE_COLOR_8:
        // uiSize *= 1;
        break;

    case NiTexture::FormatPrefs::HIGH_COLOR_16:
    case NiTexture::FormatPrefs::SINGLE_COLOR_16:
        uiTexMemSize *= 2;
        break;

    case NiTexture::FormatPrefs::TRUE_COLOR_32:
    case NiTexture::FormatPrefs::SINGLE_COLOR_32:
    case NiTexture::FormatPrefs::DOUBLE_COLOR_32:
    case NiTexture::FormatPrefs::FLOAT_COLOR_32:
    case NiTexture::FormatPrefs::PIX_DEFAULT:
        uiTexMemSize *= 4;
        break;

    case NiTexture::FormatPrefs::DOUBLE_COLOR_64:
    case NiTexture::FormatPrefs::FLOAT_COLOR_64:
        uiTexMemSize *= 8;
        break;

    case NiTexture::FormatPrefs::FLOAT_COLOR_128:
        uiTexMemSize *= 16;
        break;       
    default:
        // Assume 32 bpp
        uiTexMemSize *= 4;
        break;
    };

    if (kPrefs.m_eMipMapped)
    {
        // Estimate mip-map chain with an additional 1.33 
        uiTexMemSize *= 4;
        uiTexMemSize /= 3;
    }

    return uiTexMemSize;
}
//---------------------------------------------------------------------------
void NiShadowManager::DefaultPurgeCallback(
    unsigned int uiBytesNeeded,
    NiTPointerList<NiShadowGeneratorPtr>& kActiveShadowGenerators,
    NiTPointerList<NiShadowMap*>& kAvailableShadowMaps,
    NiTPointerList<NiShadowCubeMap*>& kAvailableShadowCubeMaps,
    bool bMaxTexMemoryResized)
{   
#ifdef NIDEBUG
    NiOutputDebugString("Warning: Purging unused shadow maps to conserve "
        "texture memory.\n");
#endif    
    unsigned int uiOldTexMemoryUsed = 
        NiShadowManager::GetCurrentTextureMemoryAllocation();

    // First purge unused shadow cube maps.
    while (kAvailableShadowCubeMaps.GetSize() > 0)
    {  
        NiShadowCubeMap* pkShadowCubeMap = kAvailableShadowCubeMaps.GetHead();
        NiShadowManager::DestroyShadowCubeMap(pkShadowCubeMap);

        // Check to see if we are under the allowed texture memory limit and 
        // that we have freed as many bytes as needed.
        unsigned int uiFreedBytes = uiOldTexMemoryUsed -
            NiShadowManager::GetCurrentTextureMemoryAllocation();

        if (NiShadowManager::GetMaxTextureMemoryAllocation() >=
            NiShadowManager::GetCurrentTextureMemoryAllocation() &&
            uiFreedBytes >= uiBytesNeeded)
        {
            return;
        }
    }

    // Now purge unused shadow maps.
    while (kAvailableShadowMaps.GetSize() > 0)
    {  
        NiShadowMap* pkShadowMap = kAvailableShadowMaps.GetHead();
        NiShadowManager::DestroyShadowMap(pkShadowMap);

        // Check to see if we are under the allowed texture memory limit and 
        // that we have freed as many bytes as needed.
        unsigned int uiFreedBytes = uiOldTexMemoryUsed -
            NiShadowManager::GetCurrentTextureMemoryAllocation();

        if (NiShadowManager::GetMaxTextureMemoryAllocation() >=
            NiShadowManager::GetCurrentTextureMemoryAllocation() &&
            uiFreedBytes >= uiBytesNeeded)
        {
            return;
        }
    }

    if (!bMaxTexMemoryResized)
        return; 

#ifdef NIDEBUG
    NiOutputDebugString("Warning: Deactivating NiShadowGenerator objects to "
        "conserve texture memory.\n");
#endif    

    // Iterate over all the active shadow generators and purging the shadow 
    // maps by deactivating the shadow maps until the active memory usage 
    // complies with the maximum allowed texture memory usage.
    while (kActiveShadowGenerators.GetSize() > 0)
    {
        NiShadowGenerator* pkGenerator = kActiveShadowGenerators.GetHead();

        // Deactivate the shadow generator so it can release the shadow maps
        // it currently owns.
        pkGenerator->SetActive(false);

        // Purge all shadow cube maps that have been made available by 
        // deactivating the shadow generator.
        while (kAvailableShadowCubeMaps.GetSize() > 0)
        {  
            NiShadowCubeMap* pkShadowCubeMap = 
                kAvailableShadowCubeMaps.GetHead();
            NiShadowManager::DestroyShadowCubeMap(pkShadowCubeMap);
        }

        // Purge all shadow maps that have been made available by deactivating
        // the shadow generator.
        while (kAvailableShadowMaps.GetSize() > 0)
        {  
            NiShadowMap* pkShadowMap = kAvailableShadowMaps.GetHead();
            NiShadowManager::DestroyShadowMap(pkShadowMap);
        }

        // Check to see if we are under the allowed texture memory limit and 
        // that we have freed as many bytes as needed.
        unsigned int uiFreedBytes = uiOldTexMemoryUsed -
            NiShadowManager::GetCurrentTextureMemoryAllocation();

        if (NiShadowManager::GetMaxTextureMemoryAllocation() >=
            NiShadowManager::GetCurrentTextureMemoryAllocation() &&
            uiFreedBytes >= uiBytesNeeded)
        {
            return;
        }
    }   

}
//---------------------------------------------------------------------------
void NiShadowManager::RendererRecreated()
{
    NIASSERT(ms_pkShadowManager);

    // The renderer has been re-created. This means we can not trust the data
    // stored in the shadow maps. So iterate over each shadow generator
    // and mark its shadow maps as dirty.
    NiTListIterator kGeneratorIter = 
        ms_pkShadowManager->m_kActiveGenerators.GetHeadPos();

    while (kGeneratorIter)
    {
        NiShadowGenerator* pkGenerator = 
            ms_pkShadowManager->m_kActiveGenerators.GetNext(kGeneratorIter);

        if (pkGenerator->GetActive())
            pkGenerator->MarkShadowMapsDirty();
    }
}
//---------------------------------------------------------------------------
