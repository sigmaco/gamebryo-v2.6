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

#include "NiCloningProcess.h"
#include "NiNode.h"
#include "NiRenderObject.h"
#include "NiShadowMap.h"
#include "NiShadowTechnique.h"
#include "NiShadowGenerator.h"
#include "NiShaderConstantMap.h"
#include "NiStencilProperty.h"

NiImplementRTTI(NiShadowGenerator, NiObject);

//---------------------------------------------------------------------------
NiShadowGenerator::NiShadowGenerator() :
    m_pkShadowTechnique(NULL), 
    m_pfnSMTextureRetrievalCallback(NULL),
    m_pfnSMTextureAssignmentCallback(NULL),
    m_pfnOwnedSMCallback(NULL),
    m_pfnSMCountCallback(NULL),
    m_pfnSMConstantDataAssignmentCallback(NULL),
    m_pfnSMConstantDataRetrievalCallback(NULL),
    m_pfnExtraDataCleanupCallback(NULL),
    m_pkDynEffect(NULL),
    m_pvExtraData(NULL),
    m_fDepthBias(0.0f),
    m_fNearClippingDist(0.0f),
    m_fFarClippingDist(0.0f),
    m_fDirLightFrustumWidth(0.0f),
    m_usSizeHint(1024)
{
    SetRenderViewsDirty(true);
    SetStatic(false);
    SetAutoCalcNearClippingDist(true);
    SetAutoCalcFarClippingDist(true);
    SetAutoCalcDirLightFrusturmWidth(true);
    SetAutoCalcDirLightFrusturmPosition(true);
    SetBit(true, ACTIVE);
    SetRenderBackfaces(true);
    SetStrictlyObserveSizeHint(false);

    if (NiShadowManager::GetShadowManager() == NULL)
    {
        NiOutputDebugString("Warning: An NiShadowGenerator object has been "
            "created while there is no active shadow manager. The "
            "NiShadowGenerator object will go unused and will need to be "
            "manually deleted by the application.\n");
        return;
    }
    else
    {
        // Assign the registered default shadow technique.
        m_pkShadowTechnique = NiShadowManager::GetActiveShadowTechnique(0);
    }
}
//---------------------------------------------------------------------------
NiShadowGenerator::NiShadowGenerator(NiDynamicEffect* pkDynEffect) :
    m_pkShadowTechnique(NULL), 
    m_pfnSMTextureRetrievalCallback(NULL),
    m_pfnSMTextureAssignmentCallback(NULL),
    m_pfnOwnedSMCallback(NULL),
    m_pfnSMCountCallback(NULL),
    m_pfnSMConstantDataAssignmentCallback(NULL),
    m_pfnSMConstantDataRetrievalCallback(NULL),
    m_pfnExtraDataCleanupCallback(NULL),
    m_pkDynEffect(pkDynEffect),
    m_pvExtraData(NULL),
    m_fDepthBias(0.0f),
    m_fNearClippingDist(0.0f),
    m_fFarClippingDist(0.0f),
    m_fDirLightFrustumWidth(0.0f),
    m_usSizeHint(1024)
{
    SetRenderViewsDirty(true);
    SetStatic(false);
    SetAutoCalcNearClippingDist(true);
    SetAutoCalcFarClippingDist(true);
    SetAutoCalcDirLightFrusturmWidth(true);
    SetAutoCalcDirLightFrusturmPosition(true);
    SetRenderBackfaces(true);
    SetStrictlyObserveSizeHint(false);

    NIASSERT(m_pkDynEffect);

    // NiShadowGenerator objects currently only have support for NiLights.
    NIASSERT(NiIsKindOf(NiLight, m_pkDynEffect));

    SetBit(m_pkDynEffect->GetSwitch(), ACTIVE);

    if (NiShadowManager::GetShadowManager() == NULL)
    {
        NiOutputDebugString("Warning: An NiShadowGenerator object has been "
            "created while there is no active shadow manager. The "
            "NiShadowGenerator object will go unused and will need to be "
            "manually deleted by the application.\n");
        return;
    }
    else
    {
        // Assign the registered default shadow technique.
        m_pkShadowTechnique = NiShadowManager::GetActiveShadowTechnique(0);
    }

}
//---------------------------------------------------------------------------
NiShadowGenerator::~NiShadowGenerator()
{
    ReleaseAllShadowMaps();
    
    // Ensure the associated NiDynamicEffect no longer references this
    // NiShadowGenerator object.
    if (m_pkDynEffect)
        m_pkDynEffect->AssignShadowGenerator(NULL);
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::AssignShadowMap(
    NiShadowMap* pkShadowMap, 
    unsigned int uiSMIndex, 
    NiTListIterator kSMIter, 
    NiRenderObject* pkGeometry)
{
    if (!GetActive())
        return false;

    NIASSERT(pkShadowMap);
    
    SetBit(true, DIRTY_SHADOWMAP);

    switch (pkShadowMap->GetTextureType())
    {
        case NiShadowMap::TT_SINGLE:
        {
            SetShadowMapGenerator(pkShadowMap, this);
            if (kSMIter)
            {
                NIVERIFYEQUALS(
                    NiShadowManager::MakeShadowMapUnavailable(kSMIter), pkShadowMap);
            }
            else
            {
                NiShadowManager::MakeShadowMapUnavailable(pkShadowMap);
            }

            // Ensure the shadow map we are assigning is marked as dirty.
            NiShadowRenderClick* pkRenderClick = pkShadowMap->GetRenderClick();
            if (pkRenderClick)
                pkRenderClick->MarkAsDirty();
        }
        break;
        case NiShadowMap::TT_CUBE:
        {
            SetShadowCubeMapGenerator((NiShadowCubeMap*) pkShadowMap, this);
            if (kSMIter)
            {
                NIVERIFYEQUALS(
                    NiShadowManager::MakeShadowCubeMapUnavailable(kSMIter), 
                    pkShadowMap);
            }
            else
            {
                NiShadowManager::MakeShadowCubeMapUnavailable(
                    (NiShadowCubeMap*) pkShadowMap);
            }

            // Ensure the shadow map we are assigning is marked as dirty.
            NiShadowCubeMap* pkShadowCubeMap = 
                NiDynamicCast(NiShadowCubeMap, pkShadowMap);
            for (unsigned int ui = 0; 
                ui < pkShadowCubeMap->GetNumRenderClicks(); ui++)
            {
                NiShadowRenderClick* pkRenderClick = 
                    pkShadowCubeMap->GetRenderClick((unsigned char)ui);

                if (pkRenderClick)
                    pkRenderClick->MarkAsDirty();
            }
        }
        break;
        default:
            NIASSERT(!"Unknown shadow map texture type!");
            break;
    }

    NIASSERT(m_pfnSMTextureAssignmentCallback);
    bool bResults = m_pfnSMTextureAssignmentCallback(this, pkGeometry, 
          pkShadowMap, uiSMIndex);

    ForceMaterialNeedsUpdate();

    return bResults;
}
//---------------------------------------------------------------------------
void NiShadowGenerator::ReleaseAllShadowMaps()
{
    SetRenderViewsDirty(true);

    unsigned int uiSMCount = GetShadowMapCount();  
    for (unsigned int ui = 0; ui < uiSMCount; ui++)
    {
        NiShadowMap* pkShadowMap = GetOwnedShadowMap(ui);

        switch (pkShadowMap->GetTextureType())
        {
            case NiShadowMap::TT_SINGLE:
                SetShadowMapGenerator(pkShadowMap, NULL);
                NiShadowManager::MakeShadowMapAvailable(pkShadowMap);
                break;
            case NiShadowMap::TT_CUBE:
                SetShadowCubeMapGenerator((NiShadowCubeMap*) pkShadowMap,
                    NULL);
                NiShadowManager::MakeShadowCubeMapAvailable((NiShadowCubeMap*)
                    pkShadowMap);
                break;
            default:
                NIASSERT(!"Unknown shadow map texture type!");
                break;
        }
    }

    // Clean up any extra data used by the callbacks
    if (m_pfnExtraDataCleanupCallback)
        m_pfnExtraDataCleanupCallback(this);

    ForceMaterialNeedsUpdate();
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::IsUnaffectedCasterNode(const NiNode* pkPossibleNode)
{
    if (!pkPossibleNode)
        return false;

    NiTListIterator kPos = m_kUnaffectedCasterList.GetHeadPos();
    while ( kPos )
    {
        NiNode* pkNode = m_kUnaffectedCasterList.GetNext(kPos);
        if (IsUnaffectedCasterNodeHelper(pkNode, pkPossibleNode))
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::IsUnaffectedCasterNode(
    const NiRenderObject* pkGeometry)
{
    if (!pkGeometry)
        return false;

    NiTListIterator kPos = m_kUnaffectedCasterList.GetHeadPos();
    while ( kPos )
    {
        NiNode* pkNode = m_kUnaffectedCasterList.GetNext(kPos);
        if (IsUnaffectedCasterNodeHelper(pkNode, pkGeometry))
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::IsUnaffectedCasterNodeHelper(NiNode* pkCurrNode, 
    const NiNode* pkPossibleNode)
{
    if (pkCurrNode == NULL)
        return false;

    if (pkCurrNode == pkPossibleNode)
        return true;

    // Recurse the children
    unsigned int uiChildCount = pkCurrNode->GetArrayCount();
    for (unsigned int i = 0; i < uiChildCount; i++)
    {
        NiNode* pkChild = NiDynamicCast(NiNode, pkCurrNode->GetAt(i));
        if (pkChild)
        {
            if (pkChild == pkPossibleNode)
                return true;

            if (IsUnaffectedCasterNodeHelper(pkChild, pkPossibleNode))
                return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::IsUnaffectedCasterNodeHelper(NiNode* pkCurrNode, 
    const NiRenderObject* pkPossibleGeometry)
{
    if (pkCurrNode == NULL)
        return false;

    // Recurse the children
    unsigned int uiChildCount = pkCurrNode->GetArrayCount();
    for (unsigned int i = 0; i < uiChildCount; i++)
    {
        NiRenderObject* pkGeom = NiDynamicCast(NiRenderObject, 
            pkCurrNode->GetAt(i));
        if (pkGeom == pkPossibleGeometry)
            return true;

        NiNode* pkChild = NiDynamicCast(NiNode, pkCurrNode->GetAt(i));
        if (pkChild && 
            IsUnaffectedCasterNodeHelper(pkChild, pkPossibleGeometry))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::IsUnaffectedReceiverNode(const NiNode* pkPossibleNode)
{
    if (!pkPossibleNode)
        return false;

    NiTListIterator kPos = m_kUnaffectedReceiverList.GetHeadPos();
    while ( kPos )
    {
        NiNode* pkNode = m_kUnaffectedReceiverList.GetNext(kPos);
        if (IsUnaffectedReceiverNodeHelper(pkNode, pkPossibleNode))
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::IsUnaffectedReceiverNode(
    const NiRenderObject* pkGeometry)
{
    if (!pkGeometry)
        return false;

    NiTListIterator kPos = m_kUnaffectedReceiverList.GetHeadPos();
    while ( kPos )
    {
        NiNode* pkNode = m_kUnaffectedReceiverList.GetNext(kPos);
        if (IsUnaffectedReceiverNodeHelper(pkNode, pkGeometry))
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::IsUnaffectedReceiverNodeHelper(NiNode* pkCurrNode, 
    const NiNode* pkPossibleNode)
{
    if (pkCurrNode == NULL)
        return false;

    if (pkCurrNode == pkPossibleNode)
        return true;

    // Recurse the children
    unsigned int uiChildCount = pkCurrNode->GetArrayCount();
    for (unsigned int i = 0; i < uiChildCount; i++)
    {
        NiNode* pkChild = NiDynamicCast(NiNode, pkCurrNode->GetAt(i));
        if (pkChild)
        {
            if (pkChild == pkPossibleNode)
                return true;

            if (IsUnaffectedReceiverNodeHelper(pkChild, pkPossibleNode))
                return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::IsUnaffectedReceiverNodeHelper(NiNode* pkCurrNode, 
    const NiRenderObject* pkPossibleGeometry)
{
    if (pkCurrNode == NULL)
        return false;

    // Recurse the children
    unsigned int uiChildCount = pkCurrNode->GetArrayCount();
    for (unsigned int i = 0; i < uiChildCount; i++)
    {
        NiRenderObject* pkGeom = NiDynamicCast(NiRenderObject, 
            pkCurrNode->GetAt(i));
        if (pkGeom == pkPossibleGeometry)
            return true;

        NiNode* pkChild = NiDynamicCast(NiNode, pkCurrNode->GetAt(i));
        if (pkChild && 
            IsUnaffectedReceiverNodeHelper(pkChild, pkPossibleGeometry))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiShadowGenerator::GetCasterGeometryList(NiRenderObjectList& kCasterList)
{
    kCasterList.RemoveAll();

    if (!m_pkDynEffect)
    {
        return;
    }

    // Flatten the affected node list of the shadow generator's light into a 
    // set of only NiRenderObject objects.
    const NiNodeList& kAffectedNodeList = m_pkDynEffect->GetAffectedNodeList();
    NiTListIterator kIter = kAffectedNodeList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kAffectedNodeList.GetNext(kIter);
        AdditiveListHelper(pkNode, &kCasterList);
    }

    // Now remove all NiRenderObject objects from the casters list that are 
    // included in the light's unaffected node list.
    const NiNodeList& kUnaffectedNodeList = 
        m_pkDynEffect->GetUnaffectedNodeList();
    kIter = kUnaffectedNodeList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kUnaffectedNodeList.GetNext(kIter);
        SubtractiveListHelper(pkNode, &kCasterList);
    }

    // Now remove all NiRenderObject objects from the casters list that are 
    // included in the generators unaffected caster list.
    kIter = m_kUnaffectedCasterList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = m_kUnaffectedCasterList.GetNext(kIter);
        SubtractiveListHelper(pkNode, &kCasterList);
    }

}
//---------------------------------------------------------------------------
void NiShadowGenerator::GetReceiverGeometryList(
    NiRenderObjectList& kReceiverList)
{
    kReceiverList.RemoveAll();

    if (!m_pkDynEffect)
    {
        return;
    }

    // Flatten the affected node list of the shadow generator's light into a 
    // set of only NiRenderObject objects.
    const NiNodeList& kAffectedNodeList = m_pkDynEffect->GetAffectedNodeList();
    NiTListIterator kIter = kAffectedNodeList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kAffectedNodeList.GetNext(kIter);
        AdditiveListHelper(pkNode, &kReceiverList);
    }

    // Now remove all NiRenderObject objects from the receiver list that are 
    // included in the light's unaffected node list.
    const NiNodeList& kUnaffectedNodeList = 
        m_pkDynEffect->GetUnaffectedNodeList();
    kIter = kUnaffectedNodeList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kUnaffectedNodeList.GetNext(kIter);
        SubtractiveListHelper(pkNode, &kReceiverList);
    }

    // Now remove all NiRenderObject objects from the receiver list that are 
    // included in the generators unaffected receiver list.
    kIter = m_kUnaffectedReceiverList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = m_kUnaffectedReceiverList.GetNext(kIter);
        SubtractiveListHelper(pkNode, &kReceiverList);
    }

}
//---------------------------------------------------------------------------
void NiShadowGenerator::AdditiveListHelper(NiNode* pkNode, 
    NiRenderObjectList* pkGeomList)
{
    if (!pkNode)
        return;

    // Recurse to children
    unsigned int uiChildCount = pkNode->GetArrayCount();
    for (unsigned int i = 0; i < uiChildCount; i++)
    {
        NiAVObject* pkChild = pkNode->GetAt(i);
        if (pkChild)
        {
            if (NiIsKindOf(NiRenderObject, pkChild))
            {
                pkGeomList->AddTail((NiRenderObject*)pkChild);
            }
            else if (pkChild->IsNode())
            {
                AdditiveListHelper((NiNode*)pkChild, pkGeomList);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiShadowGenerator::SubtractiveListHelper(NiNode* pkNode, 
    NiRenderObjectList* pkGeomList)
{
    if (!pkNode)
        return;

    // Recurse to children
    unsigned int uiChildCount = pkNode->GetArrayCount();
    for (unsigned int i = 0; i < uiChildCount; i++)
    {
        NiAVObject* pkChild = pkNode->GetAt(i);
        if (pkChild)
        {
            if (NiIsKindOf(NiRenderObject, pkChild))
                pkGeomList->Remove((NiRenderObject*)pkChild);
            else if (pkChild->IsNode())
                SubtractiveListHelper((NiNode*)pkChild, pkGeomList);
        }
    }

}
//---------------------------------------------------------------------------
bool NiShadowGenerator::SetActive(bool bActive, bool bForceMaterialUpdate,
    bool bRetainShadowMaps)
{
    if (!m_pkDynEffect)
        return false;
    
    bool bRetVal = true;

    if (bActive != GetBit(ACTIVE))
    {
        if (!m_pkDynEffect->GetSwitch() && bActive)
            bRetVal = false;

        if (bRetVal && (!NiShadowManager::GetShadowManager() || 
            !NiShadowManager::SetShadowGeneratorActiveState(this, bActive)))
            bRetVal = false;

        if (bRetVal)
        {
            SetBit(bActive, ACTIVE);

            if (bActive == false && bRetainShadowMaps == false)
                ReleaseAllShadowMaps();

            if (bForceMaterialUpdate)
                ForceMaterialNeedsUpdate();
        }
    }

    // This ensures that the light type for the NiDynamicEffect is up-to-date.
    // This code requires the EffectType enum in NiDynamicEffect to be proved
    // the indexing diff between shadow casting and non-shadow casting lights
    if (GetBit(ACTIVE) &&
        m_pkDynEffect->GetEffectType() < NiDynamicEffect::SHADOWLIGHT_INDEX)
    {
        m_pkDynEffect->SetEffectType(static_cast<unsigned char>(
            m_pkDynEffect->GetEffectType() + 
            NiDynamicEffect::SHADOW_INDEX_DIFF));
    }
    else if (!GetBit(ACTIVE) &&
        m_pkDynEffect->GetEffectType() >= NiDynamicEffect::SHADOWLIGHT_INDEX)

    {
        m_pkDynEffect->SetEffectType(static_cast<unsigned char>(
            m_pkDynEffect->GetEffectType() -
            NiDynamicEffect::SHADOW_INDEX_DIFF));
    }

    return bRetVal;
}
//---------------------------------------------------------------------------
void NiShadowGenerator::ForceMaterialNeedsUpdate()
{
    NiRenderObjectList kReceiverList;
    GetReceiverGeometryList(kReceiverList);
    NiTListIterator kIter = kReceiverList.GetHeadPos();
    
    while (kIter)
    {
        NiRenderObject* pkGeom = kReceiverList.GetNext(kIter);
        pkGeom->SetMaterialNeedsUpdate(true);
    }
}
//---------------------------------------------------------------------------
void NiShadowGenerator::MarkShadowMapsDirty()
{
    SetBit(true, DIRTY_SHADOWMAP);

    // Cycle through all the render clicks of all the assigned shadow maps
    // and mark them dirty.
    unsigned int uiSMCount = GetShadowMapCount();  
    for (unsigned int ui = 0; ui < uiSMCount; ui++)
    {
        NiShadowMap* pkShadowMap = GetOwnedShadowMap(ui);

        if (pkShadowMap->GetTextureType() == NiShadowMap::TT_SINGLE)
        {
            NiShadowRenderClick* pkRenderClick = pkShadowMap->GetRenderClick();

            if (pkRenderClick)
                pkRenderClick->MarkAsDirty();
        }
        else
        {
            NiShadowCubeMap* pkShadowCubeMap = 
                NiDynamicCast(NiShadowCubeMap, pkShadowMap);
            for (unsigned int uiIdx = 0; 
                uiIdx < pkShadowCubeMap->GetNumRenderClicks(); uiIdx++)
            {
                NiShadowRenderClick* pkRenderClick = 
                    pkShadowCubeMap->GetRenderClick((unsigned char)uiIdx);

                if (pkRenderClick)
                    pkRenderClick->MarkAsDirty();
            }
        }
    }

}
//---------------------------------------------------------------------------
bool NiShadowGenerator::AreShadowMapsDirty()
{
    if (!GetBit(DIRTY_SHADOWMAP))
        return false;

    // Check the render clicks of all the shadow maps to see if all the clicks 
    // are clean. If this is true then the shadow generator is clean.
    unsigned int uiSMCount = GetShadowMapCount();

    if (!uiSMCount)
        return true;

    for (unsigned int ui = 0; ui < uiSMCount; ui++)
    {
        NiShadowMap* pkShadowMap = GetOwnedShadowMap(ui);

        if (pkShadowMap->GetTextureType() == NiShadowMap::TT_SINGLE)
        {
            NiShadowRenderClick* pkRenderClick = pkShadowMap->GetRenderClick();
            if (pkRenderClick && pkRenderClick->IsDirty())
                return true;
        }
        else
        {
            NiShadowCubeMap* pkShadowCubeMap = 
                NiDynamicCast(NiShadowCubeMap, pkShadowMap);
            for (unsigned int uiIdx = 0; 
                uiIdx < pkShadowCubeMap->GetNumRenderClicks(); uiIdx++)
            {
                NiShadowRenderClick* pkRenderClick = 
                    pkShadowCubeMap->GetRenderClick((unsigned char)uiIdx);
                if (pkRenderClick && pkRenderClick->IsDirty())
                    return true;
            }
        }
    }

    SetBit(false, DIRTY_SHADOWMAP);
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Callbacks:
//---------------------------------------------------------------------------
bool NiShadowGenerator::SingleShadowMapAssignmentCallback(
    NiShadowGenerator* pkGenerator, NiRenderObject*, 
    NiShadowMap* pkShadowMap, unsigned int uiSMIndex)
{
    NIASSERT(pkGenerator);
    if (uiSMIndex > 0)
        return false;

    // Directly assign the NiShadowMap to extra data since we know this is the
    // only NiShadowMap that will ever be used for the generator.
    pkGenerator->SetExtraData((void*)pkShadowMap);
    return true;
}
//---------------------------------------------------------------------------
NiShadowMap* NiShadowGenerator::SingleShadowMapRetrievalCallback(
    NiShadowGenerator* pkGenerator, NiRenderObject*, 
    unsigned int uiSMIndex)
{
    NIASSERT(pkGenerator);

    if (uiSMIndex != AUTO_DETERMINE_SM_INDEX && uiSMIndex > 0)
        return NULL;

    // The NiShadowMap is directly stored in the extra data since we know that
    // the generator will only have one NiShadowMap.
    return (NiShadowMap*)pkGenerator->GetExtraData();
}
//---------------------------------------------------------------------------
NiShadowMap* NiShadowGenerator::SingleShadowMapGetOwnedCallback(
    NiShadowGenerator* pkGenerator, unsigned int uiIndex)
{
    NIASSERT(pkGenerator);

    if (uiIndex > 0)
        return NULL;
    
    return (NiShadowMap*)pkGenerator->m_pvExtraData;
}
//---------------------------------------------------------------------------
unsigned int NiShadowGenerator::SingleShadowMapCountCallback(
    NiShadowGenerator* pkGenerator)
{
    NIASSERT(pkGenerator);

    if (pkGenerator->m_pvExtraData)
        return 1;
    
    return 0;
}
//---------------------------------------------------------------------------
void NiShadowGenerator::SingleShadowMapCleanupCallback(
    NiShadowGenerator* pkGenerator)
{
    NIASSERT(pkGenerator);

    pkGenerator->SetExtraData(NULL);
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::DefaultGetShaderConstantCallback(void* pvData, 
    unsigned int uiDataSize, 
    NiShadowGenerator* pkGenerator, 
    NiRenderObject*, unsigned int, 
    NiShaderConstantMap::ObjectMappings eMapping, 
    const NiPropertyState* pkState, const NiDynamicEffectState*, 
    const NiTransform&, const NiBound&, unsigned int)
{
    NI_UNUSED_ARG(uiDataSize);
    // Only single register shader constants supported by callback.
    NIASSERT(uiDataSize >= 4 * sizeof(float));

    float* pfData = (float*) pvData;

    switch (eMapping)    
    {
        case NiShaderConstantMap::SCM_OBJ_SHADOWBIAS:
        {
            float fDepthBias = pkGenerator->GetDepthBias();

            // If we are rendering backfaces and the stencil draw mode is set
            // to DRAW_BOTH, the default depth bias should be used as if we
            // were not rendering backfaces. If the depth bias is not the
            // default, don't change it.
            if (pkState->GetStencil()->GetDrawMode() ==
                    NiStencilProperty::DRAW_BOTH &&
                pkGenerator->GetRenderBackfaces() &&
                pkGenerator->IsDepthBiasDefault())
            {
                fDepthBias = pkGenerator->GetShadowTechnique()
                    ->GetDefaultDepthBias(ConvertLightType(
                    pkGenerator->GetAssignedDynamicEffect()->GetEffectType()),
                    false);
            }

            pfData[0] = fDepthBias;
            pfData[1] = 0.0f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return true;
        }
        case NiShaderConstantMap::SCM_OBJ_SHADOW_VSM_POWER_EPSILON:
        {
            pfData[0] = 10.0f;
            pfData[1] = 0.001f;
            pfData[2] = 0.0f;
            pfData[3] = 0.0f;
            return true;
        }

        default:
#ifdef NIDEBUG
        {
            char acString[256];
            NiSprintf(acString, 256, "Constant \"%d\" is not supported.\n",
                eMapping);
            NiOutputDebugString(acString);
        }
#endif
        return false;
        break;

    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiShadowGenerator);
//---------------------------------------------------------------------------
void NiShadowGenerator::CopyMembers(NiShadowGenerator* pkDest,
                               NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);
    
    pkDest->m_pkShadowTechnique = m_pkShadowTechnique;

    pkDest->m_uFlags = m_uFlags;

    pkDest->m_fDepthBias = m_fDepthBias;

    pkDest->m_fNearClippingDist = m_fNearClippingDist;
    pkDest->m_fFarClippingDist = m_fFarClippingDist;
    pkDest->m_fDirLightFrustumWidth = m_fDirLightFrustumWidth;

    pkDest->m_usSizeHint = m_usSizeHint;

    // m_kUnaffectedCasterList and m_kUnaffectedReceiverList are processed in
    // ProcessClone. Also the assigned NiDynamicEffect object will not be
    // copied here. It is the responsibility of NiDynamicEffect::CopyMembers
    // to properly assign the dynamic effect.
}
//---------------------------------------------------------------------------
void NiShadowGenerator::ProcessClone(NiCloningProcess& kCloning)
{
    // Get clone from the clone map.
    NiObject* pkClone = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkClone));
    NiShadowGenerator* pkDest = (NiShadowGenerator*) pkClone;

    // Only process caster and unaffected receiver lists if the cloning
    // behavior for dynamic effects is not set to CLONE_DYNEFF_NONE.
    if (kCloning.m_eDynamicEffectRelationBehavior !=
        NiCloningProcess::CLONE_RELATION_NONE)
    {
        // Iterate in reverse order so cloned lists will be in the same order.

        // Process unaffected casters.
        NiTListIterator kIter = m_kUnaffectedCasterList.GetTailPos();
        while (kIter)
        {
            NiNode* pkUnaffectedCaster =
                m_kUnaffectedCasterList.GetPrev(kIter);
            if (kCloning.m_pkCloneMap->GetAt(pkUnaffectedCaster, pkClone))
            {
                pkUnaffectedCaster = (NiNode*) pkClone;
            }
            else if (kCloning.m_eDynamicEffectRelationBehavior ==
                NiCloningProcess::CLONE_RELATION_CLONEDONLY)
            {
                pkUnaffectedCaster = NULL;
            }

            if (pkUnaffectedCaster)
            {
                pkDest->AttachUnaffectedCasterNode(pkUnaffectedCaster);
            }
        }
        
        // Process unaffected receivers.
        kIter = m_kUnaffectedReceiverList.GetTailPos();
        while (kIter)
        {
            NiNode* pkUnaffectedReceiver = m_kUnaffectedReceiverList.GetPrev(
                kIter);
            if (kCloning.m_pkCloneMap->GetAt(pkUnaffectedReceiver, pkClone))
            {
                pkUnaffectedReceiver = (NiNode*) pkClone;
            }
            else if (kCloning.m_eDynamicEffectRelationBehavior ==
                NiCloningProcess::CLONE_RELATION_CLONEDONLY)
            {
                pkUnaffectedReceiver = NULL;
            }

            if (pkUnaffectedReceiver)
            {
                pkDest->AttachUnaffectedReceiverNode(pkUnaffectedReceiver);
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiShadowGenerator);
//---------------------------------------------------------------------------
void NiShadowGenerator::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiFixedString kFixedString;
    kStream.LoadFixedString(kFixedString);

    // Save Flags
    NiStreamLoadBinary(kStream, m_uFlags);

    // This assert should never be hit. NiShadowGenerator objects are only 
    // registered as streamable when the NiShadowManager is instantiated. 
    NIASSERT(NiShadowManager::GetShadowManager());

    // Obtain the correct NiShadowTechnique based on the technique name.
    m_pkShadowTechnique = 
        NiShadowManager::GetActiveShadowTechnique(kFixedString);

    if (!m_pkShadowTechnique)
    {
#ifdef NIDEBUG
        char acString[NI_MAX_PATH];
        if (NiShadowManager::GetKnownShadowTechnique(kFixedString))
        {
            NiSprintf(acString, NI_MAX_PATH, "Warning: A NiShadowGenerator "
                "object has been streamed in with the an assigned "
                "NiShadowTechnique of [%s]. While this NiShadowTechnique is "
                "known, it is not currently active. The NiShadowGenerator "
                "object will instead be assigned the first available active "
                "NiShadowTechnique.", (const char*)kFixedString);
        }
        else
        {
            NiSprintf(acString, NI_MAX_PATH, "Warning: A NiShadowGenerator "
                "object has been streamed in with the an assigned "
                "NiShadowTechnique of [%s]. This NiShadowTechnique is not "
                "known by the NiShadowManager. The NiShadowGenerator "
                "object will instead be assigned the first available active "
                "NiShadowTechnique.", (const char*)kFixedString);
        }
        NiOutputDebugString(acString);
#endif
        
        // Assign the the NiShadowGenerator the first active NiShadowTechnique,
        // since the requested NiShadowTechnique is not active.
        for (unsigned int ui = 0; 
            ui < NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES; ui++)
        {
            if (NiShadowManager::GetActiveShadowTechnique((unsigned short)ui))
            {
                m_pkShadowTechnique = 
                    NiShadowManager::GetActiveShadowTechnique(
                    (unsigned short)ui);
                break;
            }
        }

        // Ensure that a NiShadowTechnique was assigned. If this assert is hit
        // it means the NiShadowManager has no active NiShadowTechniques. This
        // should never be the case.
        NIASSERT(m_pkShadowTechnique);
    }

    // Read shadow caster list
    kStream.ReadMultipleLinkIDs(); 

    // Read unaffected shadow receivers list
    kStream.ReadMultipleLinkIDs();

    // Read dynamic effect.
    kStream.ReadLinkID();

    // Read depth bias.
    NiStreamLoadBinary(kStream, m_fDepthBias);

    // Read size hint.
    NiStreamLoadBinary(kStream, m_usSizeHint);

    if (kStream.GetFileVersion() >=  NiStream::GetVersion(20, 3, 0, 7))
    {
        // Read tweakable frustum settings
        NiStreamLoadBinary(kStream, m_fNearClippingDist);
        NiStreamLoadBinary(kStream, m_fFarClippingDist);
        NiStreamLoadBinary(kStream, m_fDirLightFrustumWidth);
    }

}
//---------------------------------------------------------------------------
void NiShadowGenerator::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    // Link unaffected shadow caster list
    unsigned int uiSize = kStream.GetNumberOfLinkIDs();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiNode* pkNode = (NiNode*)kStream.GetObjectFromLinkID();
        if (pkNode)
            m_kUnaffectedCasterList.AddTail(pkNode);
    }

    // Link unaffected shadow receiver list
    uiSize = kStream.GetNumberOfLinkIDs();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiNode* pkNode = (NiNode*)kStream.GetObjectFromLinkID();
        if (pkNode)
            m_kUnaffectedReceiverList.AddTail(pkNode);
    }
    
    m_pkDynEffect = (NiDynamicEffect*)kStream.GetObjectFromLinkID();

    NiShadowManager::AddShadowGenerator(this);
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    return kStream.RegisterFixedString(m_pkShadowTechnique->GetName());
}
//---------------------------------------------------------------------------
void NiShadowGenerator::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    // Save Name
    kStream.SaveFixedString(m_pkShadowTechnique->GetName());

    // Save Flags
    NiStreamSaveBinary(kStream, m_uFlags);
    
    // Save unaffected shadow caster list
    unsigned int uiCasterCount = m_kUnaffectedCasterList.GetSize();
    NiStreamSaveBinary(kStream, uiCasterCount);
    NiTListIterator kIter = m_kUnaffectedCasterList.GetHeadPos();
    while (kIter)
    {
        kStream.SaveLinkID(m_kUnaffectedCasterList.GetNext(kIter));
    }

    // Save unaffected shadow receivers list
    unsigned int uiUnaffectedCount = m_kUnaffectedReceiverList.GetSize();
    NiStreamSaveBinary(kStream, uiUnaffectedCount);
    kIter = m_kUnaffectedReceiverList.GetHeadPos();
    while (kIter)
    {
        kStream.SaveLinkID(m_kUnaffectedReceiverList.GetNext(kIter));
    }

    // Save dynamic effect.
    kStream.SaveLinkID(m_pkDynEffect);

    // Save depth bias.
    NiStreamSaveBinary(kStream, m_fDepthBias);

    // Save size hint.
    NiStreamSaveBinary(kStream, m_usSizeHint);

    // Save tweakable frustum settings
    NiStreamSaveBinary(kStream, m_fNearClippingDist);
    NiStreamSaveBinary(kStream, m_fFarClippingDist);
    NiStreamSaveBinary(kStream, m_fDirLightFrustumWidth);
}
//---------------------------------------------------------------------------
bool NiShadowGenerator::IsEqual(NiObject* pkObject)
{
    NIASSERT( NiIsKindOf(NiShadowGenerator, pkObject));

    if(!NiObject::IsEqual(pkObject))
        return false;

    NiShadowGenerator* pkTestObject = (NiShadowGenerator*)pkObject;

    if (pkTestObject->m_pkDynEffect != m_pkDynEffect)
        return false;

    if (pkTestObject->m_pkShadowTechnique != m_pkShadowTechnique)
        return false;

    if (pkTestObject->m_kUnaffectedCasterList.GetSize() != 
        m_kUnaffectedCasterList.GetSize())
        return false;

    if (pkTestObject->m_kUnaffectedReceiverList.GetSize() != 
            m_kUnaffectedReceiverList.GetSize())
        return false;

    // Test to see if the unaffected caster lists are the same.
    NiTListIterator kBaseIter = m_kUnaffectedCasterList.GetHeadPos();
    NiTListIterator kTestIter = 
        pkTestObject->m_kUnaffectedCasterList.GetHeadPos();
    while (kBaseIter && kTestIter)
    {
        if ( m_kUnaffectedCasterList.GetNext(kBaseIter) != 
                pkTestObject->m_kUnaffectedCasterList.GetNext(kTestIter))
            return false;
    }

    // Test to see if the unaffected receiver lists are the same.
    kBaseIter = m_kUnaffectedReceiverList.GetHeadPos();
    kTestIter = pkTestObject->m_kUnaffectedReceiverList.GetHeadPos();
    while (kBaseIter && kTestIter)
    {
        if ( m_kUnaffectedReceiverList.GetNext(kBaseIter) != 
                pkTestObject->m_kUnaffectedReceiverList.GetNext(kTestIter))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
