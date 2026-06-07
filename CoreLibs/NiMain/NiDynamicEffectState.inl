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
//  NiDynamicEffectState inline functions

//---------------------------------------------------------------------------
inline NiDynamicEffectState::NiDynamicEffectState() 
{ 
    m_bMultiTexture = false;
    m_pLightList = NULL; 
    m_pProjLightList = NULL; 
    m_pProjShadowList = NULL; 

    m_pEnvMap = 0;
    m_pFogMap = 0;
}
//---------------------------------------------------------------------------
inline NiDynEffectStateIter 
    NiDynamicEffectState::GetLightHeadPos() const
{
    return (NiDynEffectStateIter)m_pLightList;
}
//---------------------------------------------------------------------------
inline NiLight* NiDynamicEffectState::GetNextLight(
    NiDynEffectStateIter& iter) const
{
    if(!iter)
        return NULL;

    NiDynamicEffect* pItem = ((ListItem*)iter)->m_pItem;

    iter = (NiDynEffectStateIter)(((ListItem*)iter)->m_pNext);
    
    return (NiLight*)pItem;
}
//---------------------------------------------------------------------------
inline NiDynEffectStateIter 
    NiDynamicEffectState::GetProjLightHeadPos() const
{
    return (NiDynEffectStateIter)m_pProjLightList;
}
//---------------------------------------------------------------------------
inline NiTextureEffect* NiDynamicEffectState::GetNextProjLight(
    NiDynEffectStateIter& iter) const
{
    if(!iter)
        return NULL;

    NiDynamicEffect* pItem = ((ListItem*)iter)->m_pItem;

    iter = (NiDynEffectStateIter)(((ListItem*)iter)->m_pNext);
    
    return (NiTextureEffect*)pItem;
}
//---------------------------------------------------------------------------
inline NiDynEffectStateIter 
    NiDynamicEffectState::GetProjShadowHeadPos() const
{
    return (NiDynEffectStateIter)m_pProjShadowList;
}
//---------------------------------------------------------------------------
inline NiTextureEffect* NiDynamicEffectState::GetNextProjShadow(
    NiDynEffectStateIter& iter) const
{
    if(!iter)
        return NULL;

    NiDynamicEffect* pItem = ((ListItem*)iter)->m_pItem;

    iter = (NiDynEffectStateIter)(((ListItem*)iter)->m_pNext);
    
    return (NiTextureEffect*)pItem;
}
//---------------------------------------------------------------------------
inline NiTextureEffect* 
    NiDynamicEffectState::GetEnvironmentMap() const
{
    return m_pEnvMap;
}
//---------------------------------------------------------------------------
inline NiTextureEffect* 
    NiDynamicEffectState::GetFogMap() const
{
    return m_pFogMap;
}
//---------------------------------------------------------------------------
inline bool NiDynamicEffectState::ListsEqual(const ListItem* pL1, 
    const ListItem* pL2)
{
    while(pL1)
    {
        if(!pL2)
            return false;

        if(pL1->m_pItem != pL2->m_pItem)
            return false;

        pL1 = pL1->m_pNext;
        pL2 = pL2->m_pNext;
    }

    if(pL2)
        return false;

    return true;
}
//---------------------------------------------------------------------------
inline void NiDynamicEffectState::ListDelete(ListItem* &pL)
{
    // remove all items
    while(pL)
    {
        ListItem* pKill = pL;

        pL = pL->m_pNext;

        NiDelete pKill;
    }
}
#ifndef __SPU__
//---------------------------------------------------------------------------
inline void NiDynamicEffectState::ListCopy(const ListItem* pSrc, 
    ListItem* &pDest)
{
    if(pSrc)
    {
        pDest = NiNew ListItem;
        pDest->m_pItem = pSrc->m_pItem;

        const ListItem* pSrcItem = pSrc;
        ListItem* pDestItem = pDest;
        while(pSrcItem->m_pNext)
        {
            pDestItem->m_pNext = NiNew ListItem;

            pDestItem = pDestItem->m_pNext;
            pSrcItem = pSrcItem->m_pNext;

            pDestItem->m_pItem = pSrcItem->m_pItem;
        }

        pDestItem->m_pNext = NULL;
    }
}
#endif
//---------------------------------------------------------------------------
inline bool NiDynamicEffectState::IsMultiTexture() const
{
    return m_bMultiTexture;
}
