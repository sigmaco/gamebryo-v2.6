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

#ifndef NISHADOWMANAGER_H
#define NISHADOWMANAGER_H

#include "NiShadowCubeMap.h"
#include "NiShadowRenderClick.h"
#include "NiTPtrSet.h"
#include "NiTObjectList.h"
#include "NiTPointerList.h"
#include "NiShadowClickValidator.h"
#include "NiMaterialSwapProcessor.h"
#include "NiStandardMaterial.h"
#include "NiLight.h"
#include "NiPixelFormat.h"

class NiSpotShadowWriteMaterial;
class NiPointShadowWriteMaterial;
class NiDirectionalShadowWriteMaterial;
class NiShadowTechnique;
class NiCamera;
class NiCullingProcess;
class NiShadowClickGenerator;

NiSmartPointer(NiShadowClickGenerator);
NiSmartPointer(NiShadowGenerator);

class NIMAIN_ENTRY NiShadowManager : public NiMemObject
{
public:

    enum
    {
#if defined(_WII)
        MIN_VERTEX_SHADER_VERSION = 0,
        MIN_GEOMETRY_SHADER_VERSION = 0,
        MIN_PIXEL_SHADER_VERSION = 0,
#else
        MIN_VERTEX_SHADER_VERSION = 2,
        MIN_GEOMETRY_SHADER_VERSION = 0,
        MIN_PIXEL_SHADER_VERSION = 2,
#endif
        MAX_ACTIVE_SHADOWTECHNIQUES = 4,
        MAX_KNOWN_SHADOWTECHNIQUES = 16
    };

    static void Initialize();
    static void Shutdown();

    static NiShadowManager* GetShadowManager();
    static NiMaterial* GetShadowWriteMaterial(
        NiStandardMaterial::LightType eType);
    static NiMaterialSwapProcessorPtr GetMaterialSwapProcessor(
        NiStandardMaterial::LightType eType);
    static NiShadowClickValidatorPtr GetShadowClickValidator();

    static bool ValidateShaderVersions(
        unsigned short usVertexShaderVersionMajor, 
        unsigned short usVertexShaderVersionMinor,
        unsigned short usGeometryShaderVersionMajor,
        unsigned short usGeometryShaderVersionMinor,
        unsigned short usPixelShaderVersionMajor,
        unsigned short usPixelShaderVersionMinor);
    static bool AreShaderModelsValid();
    
    static void SetMaxActiveShadowGenerators(unsigned int uiMax);
    static unsigned int GetMaxActiveShadowGenerators();

    static void RendererRecreated();

    static void SetActive(bool bActive, bool bRetainShadowMaps = false);
    static bool GetActive();

    // Maximum texture memory management
    typedef void (*PurgeShadowMaps)
        (unsigned int uiBytesNeeded,
        NiTPointerList<NiShadowGeneratorPtr>& kActiveShadowGenerators,
        NiTPointerList<NiShadowMap*>& m_kAvailableShadowMaps,
        NiTPointerList<NiShadowCubeMap*>& m_kAvailableShadowCubeMaps,
        bool bMaxTexMemoryResized);
    
    static void DefaultPurgeCallback(
        unsigned int uiBytesNeeded,
        NiTPointerList<NiShadowGeneratorPtr>& kActiveShadowGenerators,
        NiTPointerList<NiShadowMap*>& m_kAvailableShadowMaps,
        NiTPointerList<NiShadowCubeMap*>& m_kAvailableShadowCubeMaps,
        bool bMaxTexMemoryResized);
    static void SetPurgeShadowMapsCallback(PurgeShadowMaps pfnCallback);
    static void SetMaxTextureMemoryAllocation(unsigned int uiMax);
    static unsigned int GetMaxTextureMemoryAllocation();
    static const unsigned int GetCurrentTextureMemoryAllocation();

    // NiShadowTechnique management
    static bool RegisterShadowTechnique(NiShadowTechnique* pkShadowTechnique,
        unsigned short usSlot, unsigned short usShaderID);

    static NiShadowTechnique* GetActiveShadowTechnique(unsigned short usSlot);
    static NiShadowTechnique* GetActiveShadowTechnique(
        const NiFixedString& kTechniqueName);

    static NiShadowTechnique* GetKnownShadowTechnique(unsigned short usID);
    static NiShadowTechnique* GetKnownShadowTechnique(
        const NiFixedString& kTechniqueName);

    // NiShadowGenerator management
    static bool AddShadowGenerator(NiShadowGenerator* pkGenerator);
    static bool DeleteShadowGenerator(NiShadowGenerator* pkGenerator);
    static void ReorganizeActiveShadowGenerators();

    static const NiTPointerList<NiShadowGeneratorPtr>& GetShadowGenerators();
    static const NiTPointerList<NiShadowGeneratorPtr>& 
        GetActiveShadowGenerators();

    // NiShadowMap management
    static NiShadowMap* CreateShadowMap(
        unsigned int uiWidth, unsigned int uiHeight, 
        NiTexture::FormatPrefs& kPrefs, 
        const NiPixelFormat* pkDepthFormat, 
        NiTexturingProperty::ClampMode eClampMode = 
            NiTexturingProperty::CLAMP_S_CLAMP_T, 
        NiTexturingProperty::FilterMode eFilterMode = 
            NiTexturingProperty::FILTER_NEAREST_MIPNEAREST);
    static void DestroyShadowMap(NiShadowMap* pkShadowMap);
    static void DestroyAllShadowMaps();

    // NiShadowCubeMap management
    static NiShadowCubeMap* CreateShadowCubeMap(
        unsigned int uiSize, NiTexture::FormatPrefs& kPrefs,
        const NiPixelFormat* pkDepthFormat, 
        NiTexturingProperty::ClampMode eClampMode =
            NiTexturingProperty::CLAMP_S_CLAMP_T,
        NiTexturingProperty::FilterMode eFilterMode =
            NiTexturingProperty::FILTER_NEAREST_MIPNEAREST);
    static void DestroyShadowCubeMap(NiShadowCubeMap* pkShadowCubeMap);
    static void DestroyAllShadowCubeMaps();

    static NiTListIterator RequestClosestShadowMap(
        unsigned int uiDesiredHeight, unsigned int uiDesiredWidth,
        NiTexture::FormatPrefs& kFormatPrefs, bool bExactSizeOnly);
    static NiTListIterator RequestClosestShadowMap(
        unsigned int uiDesiredHeight, unsigned int uiDesiredWidth,
        NiTexture::FormatPrefs& kFormatPrefs, NiShadowMap*& pkClosestShadowMap,
        bool bExactSizeOnly);
    static NiTListIterator RequestClosestShadowCubeMap(
        unsigned int uiDesiredSize, NiTexture::FormatPrefs& kFormatPrefs, 
        bool bExactSizeOnly);
    static NiTListIterator RequestClosestShadowCubeMap(
        unsigned int uiDesiredSize, 
        NiTexture::FormatPrefs& kFormatPrefs, 
        NiShadowCubeMap*& pkClosestShadowCubeMap,
        bool bExactSizeOnly);

    // Shadow click generator functions.
    static bool RegisterShadowClickGenerator(
        NiShadowClickGenerator* pkShadowClickGenerator);
    static NiShadowClickGenerator* GetShadowClickGenerator(
        const NiFixedString& kName);
    static bool SetActiveShadowClickGenerator(
        NiShadowClickGenerator* pkActiveShadowClickGenerator);
    static bool SetActiveShadowClickGenerator(const NiFixedString& kName);
    static const NiTObjectPtrSet<NiShadowClickGeneratorPtr>&
        GetShadowClickGenerators();

    static void SetSceneCamera(NiCamera* pkSceneCamera);
    static NiCamera* GetSceneCamera();

    static void SetCullingProcess(NiCullingProcess* pkCullingProcess);
    static NiCullingProcess* GetCullingProcess();

    // Function that generates a list of render click objects that can be used
    // to render the shadow maps.
    static const NiTPointerList<NiRenderClick*>& GenerateRenderClicks();

    // Helper functions
    static unsigned int GetApproximateTexMemUsage(
        const unsigned int uiWidth, const unsigned int uiHeight, 
        const NiTexture::FormatPrefs& kPrefs, const bool bCubeMap);

    // *** begin Emergent internal use only ***
    static void MakeShadowMapAvailable(NiShadowMap* pkShadowMap);
    static void MakeShadowMapUnavailable(NiShadowMap* pkShadowMap);
    static NiShadowMap* MakeShadowMapUnavailable(NiTListIterator kSMIter);

    static void MakeShadowCubeMapAvailable(NiShadowCubeMap* pkShadowCubeMap);
    static void MakeShadowCubeMapUnavailable(NiShadowCubeMap* pkShadowCubeMap);
    static NiShadowCubeMap* MakeShadowCubeMapUnavailable(
        NiTListIterator kSMIter);

    static void AppendRenderClick(NiRenderClick* pkClick);
    static bool SetShadowGeneratorActiveState(NiShadowGenerator* pkGenerator,
        bool bActive);

    static NiDepthStencilBuffer* GetCompatibleDepthStencil(
        Ni2DBuffer* pkBuffer, const NiPixelFormat* pkDepthFormat);
    // *** end Emergent internal use only ***

private:
    NiShadowManager();
    ~NiShadowManager();

    void OnActiveShadowClickGeneratorChanged();

    // Masks used to encode light data
    enum
    {
        LIGHT_TYPE_MASK = 0x00FF,
        LIGHT_INDEX_MASK = 0xFF00,
        LIGHT_INDEX_SHIFT = 8
    };

    NiShadowTechnique* m_pkActiveShadowTechniques[MAX_ACTIVE_SHADOWTECHNIQUES];
    NiShadowTechnique* m_pkKnownShadowTechniques[MAX_KNOWN_SHADOWTECHNIQUES];

    NiMaterial* m_apkShadowWriteMaterial[
        NiStandardMaterial::LIGHT_MAX];

    NiTObjectList<NiShadowMap> m_kShadowMapPool;
    NiTPointerList<NiShadowMap*> m_kAvailableShadowMaps;

    NiTObjectList<NiShadowCubeMap> m_kShadowCubeMapPool;
    NiTPointerList<NiShadowCubeMap*> m_kAvailableShadowCubeMaps;

    NiTObjectPtrSet<NiShadowClickGeneratorPtr> m_kShadowClickGenerators;

    int m_iActiveShadowClickGeneratorIndex;
    unsigned int m_uiCurrentTextureMemoryAllocation;
    
    PurgeShadowMaps m_pfnPurgeShadowMapsCallback;
    unsigned int m_uiMaxActiveShadowGenerators;
    unsigned int m_uiMaxTexureMemoryAllocation;

    bool m_bActive;
    bool m_bValidShaderModels;

    NiCameraPtr m_spSceneCamera;
    NiCullingProcessPtr m_spCullingProcess;

    NiMaterialSwapProcessorPtr 
        m_aspMaterialSwapProcessor[NiStandardMaterial::LIGHT_MAX];

    NiShadowClickValidatorPtr m_spShadowClickValidator;

    NiTPointerList<NiShadowGeneratorPtr> m_kGenerators;
    NiTPointerList<NiShadowGeneratorPtr> m_kActiveGenerators;

    NiTPointerList<NiRenderClick*> m_kRenderClicks;

    static NiShadowManager* ms_pkShadowManager;

#if defined(_WII) && defined(NIDEBUG)
    // This data is used so that users don't get an endless
    // stream of "no shadow cube map" error messages every 
    // frame on the Wii.
    static bool ms_bWarnedAboutWiiCubeMaps;
#endif
};

#include "NiShadowManager.inl"

#endif // NISHADOWMANAGER_H
