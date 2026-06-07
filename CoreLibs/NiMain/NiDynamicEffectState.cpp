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

#include "NiDynamicEffectState.h"
#include "NiDynamicEffect.h"
#include "NiTextureEffect.h"

//---------------------------------------------------------------------------
NiDynamicEffectState::~NiDynamicEffectState()
{
    // remove all items
    ListDelete(m_pLightList);
    ListDelete(m_pProjLightList);
    ListDelete(m_pProjShadowList);
}
//---------------------------------------------------------------------------
NiDynamicEffectState* NiDynamicEffectState::Copy() const
{
    NiDynamicEffectState* pNew = NiNew NiDynamicEffectState;

    pNew->m_bMultiTexture = m_bMultiTexture;
    ListCopy(m_pLightList, pNew->m_pLightList);
    ListCopy(m_pProjLightList, pNew->m_pProjLightList);
    ListCopy(m_pProjShadowList, pNew->m_pProjShadowList);

    pNew->m_pEnvMap = m_pEnvMap;
    pNew->m_pFogMap = m_pFogMap;

    return pNew;
}
//---------------------------------------------------------------------------
void NiDynamicEffectState::AddEffect(NiDynamicEffect* pEffect)
{
    if(!pEffect)
        return;

    if(pEffect->GetEffectType() <= NiDynamicEffect::MAX_LIGHT_TYPE)
        ListInsertSorted(m_pLightList, pEffect);
    else
    {
        const NiTextureEffect::TextureType eTex = 
            ((NiTextureEffect*)pEffect)->GetTextureType();

        switch(eTex)
        {
            case NiTextureEffect::PROJECTED_LIGHT:
                ListInsertSorted(m_pProjLightList, pEffect);
                break;
            case NiTextureEffect::PROJECTED_SHADOW:
                ListInsertSorted(m_pProjShadowList, pEffect);
                break;
            case NiTextureEffect::ENVIRONMENT_MAP:
                m_pEnvMap = (NiTextureEffect*)pEffect;
                break;
            case NiTextureEffect::FOG_MAP:
                m_pFogMap = (NiTextureEffect*)pEffect;
                break;
            default:
                NIASSERT(!"Unknown effect type");
                break;
        }

        m_bMultiTexture = true;
    }
}
//---------------------------------------------------------------------------
void NiDynamicEffectState::RemoveEffect(NiDynamicEffect* pEffect)
{
    if(!pEffect)
        return;

    if(pEffect->GetEffectType() <= NiDynamicEffect::MAX_LIGHT_TYPE)
        ListRemove(m_pLightList, pEffect);
    else
    {
        const NiTextureEffect::TextureType eTex = 
            ((NiTextureEffect*)pEffect)->GetTextureType();

        switch(eTex)
        {
            case NiTextureEffect::PROJECTED_LIGHT:
                ListRemove(m_pProjLightList, pEffect);
                break;
            case NiTextureEffect::PROJECTED_SHADOW:
                ListRemove(m_pProjShadowList, pEffect);
                break;
            case NiTextureEffect::ENVIRONMENT_MAP:
                if(pEffect == m_pEnvMap)
                    m_pEnvMap = 0;
                break;
            case NiTextureEffect::FOG_MAP:
                if(pEffect == m_pFogMap)
                    m_pFogMap = 0;
                break;
            default:
                NIASSERT(!"Unknown effect type");
                break;

        }

        SetMultiTexture();
    }
}
//---------------------------------------------------------------------------
void NiDynamicEffectState::SetMultiTexture()
{
    m_bMultiTexture =
        GetProjLightHeadPos() != NULL ||
        GetProjShadowHeadPos() != NULL ||
        GetEnvironmentMap() != NULL || 
        GetFogMap() != NULL;
}
//---------------------------------------------------------------------------
NiBool NiDynamicEffectState::Equal(const NiDynamicEffectState* pState) const
{
    if(!pState)
        return false;

    if(m_pEnvMap != pState->m_pEnvMap)
        return false;
    if(m_pFogMap != pState->m_pFogMap)
        return false;

    if(!ListsEqual(m_pLightList, pState->m_pLightList))
        return false;
    if(!ListsEqual(m_pProjLightList, pState->m_pProjLightList))
        return false;
    if(!ListsEqual(m_pProjShadowList, pState->m_pProjShadowList))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiDynamicEffectState::ListInsertSorted(ListItem* &pL, 
    NiDynamicEffect* pItem)
{
    // insert the effect in ascending order of index...
    int iIndex = pItem->GetIndex();

    if(!pL || (iIndex < pL->m_pItem->GetIndex()))
    {
        ListItem* pNew = NiNew ListItem;
        pNew->m_pItem = pItem;
        pNew->m_pNext = pL;
        pL = pNew;
    }
    else if(iIndex == pL->m_pItem->GetIndex())
        return;
    else
    {
        ListItem* pIter = pL;
        ListItem* pNext = pL->m_pNext;

        NIASSERT(pL->m_pItem && (iIndex >= pL->m_pItem->GetIndex()));

        // While the current node has a lower or equal index item
        while(pNext && (iIndex > pNext->m_pItem->GetIndex()))
        {
            pIter = pNext;
            pNext = pIter->m_pNext;
        }

        // item is in list - return
        if(pNext && (iIndex == pNext->m_pItem->GetIndex()))
            return;

        // either we are pointing to the last item and our new item has a
        // higher index, or the current item has a lower index than the new
        // item, and the next item has a higher index than the new item.
        // In either case, the place for the new item is after pIter

        // add after current item
        ListItem* pNew = NiNew ListItem;
        pNew->m_pItem = pItem;
        pNew->m_pNext = pNext;
        pIter->m_pNext = pNew;
    }
}
//---------------------------------------------------------------------------
void NiDynamicEffectState::ListRemove(ListItem* &pL, NiDynamicEffect* pItem)
{
    int iIndex = pItem->GetIndex();

    if(!pL || (iIndex < pL->m_pItem->GetIndex()))
        return;
    else if(iIndex == pL->m_pItem->GetIndex())
    {
        ListItem* pKill = pL;
        pL = pL->m_pNext;
        NiDelete pKill;
    }
    else
    {
        ListItem* pIter = pL;
        ListItem* pNext = pL->m_pNext;

        NIASSERT(pL->m_pItem && (iIndex >= pL->m_pItem->GetIndex()));

        // While the current node has a lower or equal index item
        while(pNext && (iIndex > pNext->m_pItem->GetIndex()))
        {
            pIter = pNext;
            pNext = pIter->m_pNext;
        }

        // item is in list - return
        if(pNext && (iIndex == pNext->m_pItem->GetIndex()))
        {
            pIter->m_pNext = pNext->m_pNext;
            NiDelete pNext;
        }
    }
}
//---------------------------------------------------------------------------
