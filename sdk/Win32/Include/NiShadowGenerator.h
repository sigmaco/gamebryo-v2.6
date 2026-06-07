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

#ifndef NISHADOWGENERATOR_H
#define NISHADOWGENERATOR_H

#include "NiShaderConstantMap.h"
#include "NiDynamicEffect.h"
#include "NiRenderObject.h"
#include "NiShadowTechnique.h"

class NiShadowMap;
class NiShadowCubeMap;
class NiRenderObject;

class NIMAIN_ENTRY NiShadowGenerator : public NiObject
{
    NiDeclareRTTI;
    NiDeclareClone(NiShadowGenerator);
    NiDeclareStream;
    NiDeclareFlags(unsigned short);

public:
    enum
    {
        AUTO_DETERMINE_SM_INDEX = 0xFF
    };

    // flags
    enum
    {
        DIRTY_SHADOWMAP  = 0x0001,
        DIRTY_RENDERVIEWS  = 0x0002,
        STATIC = 0x0004,
        ACTIVE = 0x0008,
        RENDER_BACKFACES = 0x0010,
        STRICTLY_OBSERVE_SIZE_HINT = 0x0020,

        AUTO_CALC_NEAR_DIST = 0x0040,
        AUTO_CALC_FAR_DIST = 0x0080,
        AUTO_CALC_DIR_FRUSTUM_WIDTH = 0x0100,
        AUTO_CALC_DIR_FRUSTUM_POSITION = 0x0200,

        AUTO_CALC_NEARFAR_MASK = 0x00C0,
        AUTO_CALC_FULL_MASK = 0x03C0
    };

    NiShadowGenerator();
    NiShadowGenerator(NiDynamicEffect* pkDynEffect);
    virtual ~NiShadowGenerator();

    inline NiDynamicEffect* GetAssignedDynamicEffect() const;

    inline void SetShadowTechnique(const NiFixedString& kTechniqueName, 
        bool bAssignDefaultDepthBias = true);
    inline void SetShadowTechnique(NiShadowTechnique* pkTechnique,
        bool bAssignDefaultDepthBias = true);
    inline NiShadowTechnique* GetShadowTechnique() const;

    bool AssignShadowMap(NiShadowMap* pkShadowMap, unsigned int uiSMIndex,
        NiTListIterator kSMIter = NULL, NiRenderObject* pkGeometry = NULL);
    inline NiShadowMap* RetrieveShadowMap(unsigned int uiSMIndex, 
        NiRenderObject* pkGeometry = NULL);

    inline NiShadowMap* GetOwnedShadowMap(unsigned int uiIndex);
    inline unsigned int GetShadowMapCount();

    inline bool SetShaderConstantData(
        NiRenderObject* pkGeometry, 
        unsigned int uiSMIndex, 
        NiShaderConstantMap::ObjectMappings eMapping, 
        void* pkData);

    inline bool GetShaderConstantData(
        void* pvData, 
        unsigned int uiDataSize, 
        NiRenderObject* pkGeometry, 
        unsigned int uiSMIndex, 
        NiShaderConstantMap::ObjectMappings eMapping, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, 
        const NiTransform& kWorld, 
        const NiBound& kWorldBound, 
        unsigned int uiPass);

    // This function set the material needs update flag to true for all of the 
    // shadow receiving geometry.
    void ForceMaterialNeedsUpdate();

    // Flag management
    void MarkShadowMapsDirty();
    bool AreShadowMapsDirty();

    inline void SetRenderViewsDirty(bool bDirty);
    inline bool GetRenderViewsDirty();

    inline void SetStatic(bool bStatic);
    inline bool GetStatic();

    inline void SetRenderBackfaces(bool bBackfaces);
    inline bool GetRenderBackfaces();

    bool SetActive(bool bActive, bool bForceMaterialUpdate = false,
        bool bRetainShadowMaps = false);
    inline bool GetActive();

    // These functions manually builds the NiGeometryLists when called.
    void GetCasterGeometryList(NiRenderObjectList& kCasterList);
    void GetReceiverGeometryList(NiRenderObjectList& kReceiverList);

    // Unaffected shadow caster list management
    inline const NiNodeList& GetUnaffectedCasterNodeList();
    inline bool AttachUnaffectedCasterNode(NiNode* pkUnaffectedCasterNode);
    inline bool DetachUnaffectedCasterNode(NiNode* pkUnaffectedCasterNode);
    inline void DetachAllUnaffectedCasterNodes();
    bool IsUnaffectedCasterNode(const NiNode* pkPossibleNode);
    bool IsUnaffectedCasterNode(const NiRenderObject* pkGeometry);

    // Unaffected shadow receiver list management
    inline const NiNodeList& GetUnaffectedReceiverNodeList();
    inline bool AttachUnaffectedReceiverNode(NiNode* pkUnaffectedNode);
    inline bool DetachUnaffectedReceiverNode(NiNode* pkUnaffectedNode);
    inline void DetachAllUnaffectedReceiverNodes();
    bool IsUnaffectedReceiverNode(const NiNode* pkPossibleNode);
    bool IsUnaffectedReceiverNode(const NiRenderObject* pkGeometry);
    
    // Callback bookkeeping

    // Typedefs to define the parameters for the NiShadowMap and shader
    // constants callbacks.
    typedef bool (*AssignSMTextureCallback) 
        (NiShadowGenerator* pkGenerator, NiRenderObject* pkGeometry, 
        NiShadowMap* pkShadowMap, unsigned int uiSMIndex);

    typedef NiShadowMap* (*RetrieveSMTextureCallback)
        (NiShadowGenerator* pkGenerator, NiRenderObject* pkGeometry, 
        unsigned int uiSMIndex);

    typedef NiShadowMap* (*RetrieveOwnedShadowMap)
        (NiShadowGenerator* pkGenerator, unsigned int uiIndex);

    typedef unsigned int (*ShadowMapCountCallback)
        (NiShadowGenerator* pkGenerator);

    typedef bool (*SetSMConstantCallback)
        (NiShadowGenerator* pkGenerator, NiRenderObject* pkGeometry, 
        unsigned int uiSMIndex, NiShaderConstantMap::ObjectMappings eMapping, 
        void* pkData);

    typedef bool (*GetSMConstantCallback)
        (void* pvData, unsigned int uiDataSize, 
        NiShadowGenerator* pkGenerator,
        NiRenderObject* pkGeometry, unsigned int uiSMIndex, 
        NiShaderConstantMap::ObjectMappings eMapping,
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
        const NiTransform& kWorld, const NiBound& kWorldBound, 
        unsigned int uiPass);

    typedef void (*CleanupExtraDataCallback)
        (NiShadowGenerator* pkGenerator);

    // ShadowMap Texture Assignment callback.
    inline void SetSMTextureAssignmentCallbackFunc(
        AssignSMTextureCallback pfnCallback);
    AssignSMTextureCallback GetSMTextureAssignmentCallbackFunc() const;

    // ShadowMap Texture Retrieval callback.
    inline void SetSMTextureRetrievalCallbackFunc(
        RetrieveSMTextureCallback pfnCallback);
    RetrieveSMTextureCallback GetSMTextureRetrievalCallbackFunc() const;

    // Owned Shadow Map callback.
    inline void SetOwnedSMCallbackFunc(RetrieveOwnedShadowMap pfnCallback);
    RetrieveOwnedShadowMap GetOwnedSMCallbackFunc() const;

    // ShadowMap Count Callback
    inline void SetSMCountCallbackFunc(ShadowMapCountCallback pfnCallback);
    ShadowMapCountCallback GetSMCountCallbackFunc() const;

    // ShadowMap Constant Retrieval callback.
    inline void SetSMConstantRetrievalCallbackFunc(
        GetSMConstantCallback pfnCallback);
    GetSMConstantCallback GetSMConstantRetrievalCallbackFunc() const;

    // ShadowMap Constant Assignment callback.
    inline void SetSMConstantAssignmentCallbackFunc(
        SetSMConstantCallback pfnCallback);
    SetSMConstantCallback GetSMConstantAssignmentCallbackFunc() const;

    // Extra data cleanup callback (used to free any allocations made during
    // other callbacks when swapping out the set of callback functions).
    inline void SetExtraDataCleanupCallbackFunc(
        CleanupExtraDataCallback pfnCallback);
    CleanupExtraDataCallback GetExtraDataCleanupCallbackFunc() const;

    inline void* GetExtraData() const;
    inline void SetExtraData(void* pvExtraData);

    inline void SetSizeHint(unsigned short usSizeHint);
    inline unsigned short GetSizeHint() const;

    inline void SetStrictlyObserveSizeHint(bool bStrictlyObserveSizeHint);
    inline bool GetStrictlyObserveSizeHint() const;

    inline void SetDepthBias(float fDepthBias);
    inline float GetDepthBias();
    inline bool IsDepthBiasDefault();
    inline void SetDepthBiasToDefault();

    inline float GetNearClippingDist();
    inline void SetNearClippingDist(float fNearClippingDist);
    inline bool GetAutoCalcNearClippingDist();
    inline void SetAutoCalcNearClippingDist(bool bAutoDetermine);

    inline float GetFarClippingDist();
    inline void SetFarClippingDist(float fFarClippingDist);
    inline bool GetAutoCalcFarClippingDist();
    inline void SetAutoCalcFarClippingDist(bool bAutoDetermine);

    inline float GetDirLightFrusturmWidth();
    inline void SetDirLightFrusturmWidth(float fDirLightFrustumWidth);
    inline bool GetAutoCalcDirLightFrusturmWidth();
    inline void SetAutoCalcDirLightFrusturmWidth(bool bAutoDetermine);

    inline bool GetAutoCalcDirLightFrusturmPosition();
    inline void SetAutoCalcDirLightFrusturmPosition(bool bAutoDetermine);

    inline unsigned short GetFlags();
    void ReleaseAllShadowMaps();

    // Callback functions:
    static bool SingleShadowMapAssignmentCallback(
        NiShadowGenerator* pkGenerator, NiRenderObject* pkGeometry, 
        NiShadowMap* pkShadowMap, unsigned int uiSMIndex);
    static NiShadowMap* SingleShadowMapRetrievalCallback(
        NiShadowGenerator* pkGenerator, NiRenderObject* pkGeometry, 
        unsigned int uiSMIndex);
    static NiShadowMap* SingleShadowMapGetOwnedCallback(
        NiShadowGenerator* pkGenerator, unsigned int uiIndex);
    static unsigned int SingleShadowMapCountCallback(
        NiShadowGenerator* pkGenerator);
    static void SingleShadowMapCleanupCallback(NiShadowGenerator* pkGenerator);
    
    static bool DefaultGetShaderConstantCallback(void* pvData, 
        unsigned int uiDataSize, NiShadowGenerator* pkGenerator, 
        NiRenderObject* pkGeometry, unsigned int uiSMIndex, 
        NiShaderConstantMap::ObjectMappings eMapping,
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
        const NiTransform& kWorld, const NiBound& kWorldBound, 
        unsigned int uiPass);

    // *** begin Emergent internal use only ***
    virtual bool StreamCanSkip();
    inline void SetDynamicEffect(NiDynamicEffect* pkEffect);
    virtual void ProcessClone(NiCloningProcess& kCloning);
    // *** end Emergent internal use only ***

protected:

    static void SetShadowMapGenerator(NiShadowMap* pkShadowMap,
        NiShadowGenerator* pkGenerator);
    static void SetShadowCubeMapGenerator(NiShadowCubeMap* pkShadowCubeMap,
        NiShadowGenerator* pkGenerator);
    
    bool IsUnaffectedCasterNodeHelper(NiNode* pkCurrNode, 
        const NiNode* pkPossibleNode);
    bool IsUnaffectedCasterNodeHelper(NiNode* pkCurrNode, 
        const NiRenderObject* pkPossibleGeometry);

    bool IsUnaffectedReceiverNodeHelper(NiNode* pkCurrNode, 
        const NiNode* pkPossibleNode);
    bool IsUnaffectedReceiverNodeHelper(NiNode* pkCurrNode, 
        const NiRenderObject* pkPossibleGeometry);

    void AdditiveListHelper(NiNode* pkNode, NiRenderObjectList* pkGeomList);
    void SubtractiveListHelper(NiNode* pkNode, NiRenderObjectList* pkGeomList);

    static NiStandardMaterial::LightType ConvertLightType(
        NiDynamicEffect::EffectType eEffectType);

    NiShadowTechnique* m_pkShadowTechnique;
   
    NiNodeList m_kUnaffectedCasterList;   
    NiNodeList m_kUnaffectedReceiverList;       
                           
    // Call back functions used to handle assignment and retrieval
    // of shadow maps.
    RetrieveSMTextureCallback m_pfnSMTextureRetrievalCallback;
    AssignSMTextureCallback m_pfnSMTextureAssignmentCallback;
    RetrieveOwnedShadowMap m_pfnOwnedSMCallback;
    ShadowMapCountCallback m_pfnSMCountCallback;

    // Call back functions used to handle assignment and retrieval
    // of shader constant maps.
    SetSMConstantCallback m_pfnSMConstantDataAssignmentCallback;
    GetSMConstantCallback m_pfnSMConstantDataRetrievalCallback;

    CleanupExtraDataCallback m_pfnExtraDataCleanupCallback;

    // Reference to the shadow creating source object. Currently only
    // lights are supported. A normal pointer is used to avoid cyclical 
    // dependencies.
    NiDynamicEffect* m_pkDynEffect;

    // This is intended to be used by custom callbacks.
    void* m_pvExtraData;

    // The depth bias applied by the DefaultGetShaderConstantCallback
    float m_fDepthBias;

    // Tweakable values to allow artist to explicitly override the
    // view frustum settings that are used.
    float m_fNearClippingDist;
    float m_fFarClippingDist;
    float m_fDirLightFrustumWidth;

    // The requested shadow map size for this shadow generator. Shadow click
    // generators may or may not observe this size hint.
    unsigned short m_usSizeHint;
};

#include "NiShadowGenerator.inl"

#endif // NISHADOWGENERATOR_H
