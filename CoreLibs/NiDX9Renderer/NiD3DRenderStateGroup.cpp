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
#include "NiD3DRendererPCH.h"

#include "NiD3DRenderStateGroup.h"
#include "NiD3DRendererHeaders.h"

//---------------------------------------------------------------------------
NiD3DRenderer* NiD3DRenderStateGroup::ms_pkD3DRenderer = 0;
NiD3DRenderState* NiD3DRenderStateGroup::ms_pkD3DRenderState = 0;
NiTObjectPool<NiD3DRenderStateGroup>* 
    NiD3DRenderStateGroup::ms_pkRenderStateGroupPool = NULL;
NiTObjectPool<NiD3DRenderStateGroup::NiD3DRSEntry>* 
    NiD3DRenderStateGroup::ms_pkRenderStateEntryPool = NULL;

//---------------------------------------------------------------------------
void NiD3DRenderStateGroup::SetD3DRenderer(NiD3DRenderer* pkD3DRenderer)
{
    ms_pkD3DRenderer = pkD3DRenderer;
    if (ms_pkD3DRenderer)
        SetD3DRenderState(ms_pkD3DRenderer->GetRenderState());
    else
        SetD3DRenderState(0);
}
//---------------------------------------------------------------------------
void NiD3DRenderStateGroup::SetD3DRenderState(NiD3DRenderState* pkRS)
{
    ms_pkD3DRenderState = pkRS;
}
//---------------------------------------------------------------------------
void NiD3DRenderStateGroup::InitializePools()
{
    ms_pkRenderStateGroupPool = NiNew NiTObjectPool<NiD3DRenderStateGroup>;
    ms_pkRenderStateEntryPool = 
        NiNew NiTObjectPool<NiD3DRenderStateGroup::NiD3DRSEntry>(16);
}
//---------------------------------------------------------------------------
void NiD3DRenderStateGroup::ShutdownPools()
{
    NiDelete ms_pkRenderStateGroupPool;
    NiDelete ms_pkRenderStateEntryPool;
}
//---------------------------------------------------------------------------
NiD3DRenderStateGroup::~NiD3DRenderStateGroup()
{
    // Release the entries
    NiD3DRSEntry* pkCurr;
    NiD3DRSEntry* pkNext;

    pkCurr = m_pkRenderStates;
    while (pkCurr)
    {
        pkNext = pkCurr->GetNext();
        ms_pkRenderStateEntryPool->ReleaseObject(pkCurr);
        pkCurr = pkNext;
    }

    pkCurr = m_pkSavedRenderStates;
    while (pkCurr)
    {
        pkNext = pkCurr->GetNext();
        ms_pkRenderStateEntryPool->ReleaseObject(pkCurr);
        pkCurr = pkNext;
    }

    m_pkRenderStates = 0;
    m_pkSavedRenderStates = 0;
}
//---------------------------------------------------------------------------
void NiD3DRenderStateGroup::SetRenderState(unsigned int uiRenderState, 
    unsigned int uiValue, bool bSave)
{
    NiD3DRSEntry* pkRSEntry;
    bool bInSave = false;
    pkRSEntry = FindRenderStateEntry(uiRenderState, bInSave);
    if (pkRSEntry)
    {
        pkRSEntry->SetValue(uiValue);
        if (bInSave != bSave)
        {
            // Flip it to the other list
            if (bInSave)
            {
                RemoveRSEntryFromSavedList(pkRSEntry);
                InsertRSEntryIntoNoSaveList(pkRSEntry);
            }
            else
            {
                RemoveRSEntryFromNoSaveList(pkRSEntry);
                InsertRSEntryIntoSavedList(pkRSEntry);
            }
        }

        return;
    }

    // Wasn't in either... get an entry and add it
    pkRSEntry = ms_pkRenderStateEntryPool->GetFreeObject();
    NIASSERT(pkRSEntry);
    pkRSEntry->SetNext(NULL);
    pkRSEntry->SetPrev(NULL);
    pkRSEntry->SetRenderState(uiRenderState);
    pkRSEntry->SetValue(uiValue);
    if (bSave)
        InsertRSEntryIntoSavedList(pkRSEntry);
    else
        InsertRSEntryIntoNoSaveList(pkRSEntry);
}
//---------------------------------------------------------------------------
void NiD3DRenderStateGroup::RemoveRenderState(unsigned int uiRenderState)
{
    NiD3DRSEntry* pkRSEntry;

    bool bInSave = false;
    pkRSEntry = FindRenderStateEntry(uiRenderState, bInSave);
    if (pkRSEntry)
    {
        if (bInSave)
            RemoveRSEntryFromSavedList(pkRSEntry);
        else
            RemoveRSEntryFromNoSaveList(pkRSEntry);

        ms_pkRenderStateEntryPool->ReleaseObject(pkRSEntry);
    }

    // Wasn't found.
}
//---------------------------------------------------------------------------
bool NiD3DRenderStateGroup::GetRenderState(unsigned int uiRenderState, 
    unsigned int& uiValue, bool& bSave)
{
    NiD3DRSEntry* pkRSEntry;

    pkRSEntry = FindRenderStateEntry(uiRenderState, bSave);
    if (pkRSEntry)
    {
        uiValue = pkRSEntry->GetValue();
        return true;
    }

    // Wasn't found - return failure.
    return false;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DRenderStateGroup::SetRenderStates()
{
    NIASSERT(ms_pkD3DRenderState);

    NiD3DRSEntry* pkRSEntry;

    pkRSEntry = m_pkRenderStates;
    while (pkRSEntry)
    {
        ms_pkD3DRenderState->SetRenderState(
            (D3DRENDERSTATETYPE)pkRSEntry->GetRenderState(), 
            pkRSEntry->GetValue());

        pkRSEntry = pkRSEntry->GetNext();
    }

    pkRSEntry = m_pkSavedRenderStates;
    while (pkRSEntry)
    {
        ms_pkD3DRenderState->SetRenderState(
            (D3DRENDERSTATETYPE)pkRSEntry->GetRenderState(), 
            pkRSEntry->GetValue(), true);

        pkRSEntry = pkRSEntry->GetNext();
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DRenderStateGroup::RestoreRenderStates()
{
    NIASSERT(ms_pkD3DRenderState);

    NiD3DRSEntry* pkRSEntry;

    pkRSEntry = m_pkSavedRenderStates;
    while (pkRSEntry)
    {
        ms_pkD3DRenderState->RestoreRenderState(
            (D3DRENDERSTATETYPE)pkRSEntry->GetRenderState());

        pkRSEntry = pkRSEntry->GetNext();
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiD3DRenderStateGroup* NiD3DRenderStateGroup::GetFreeRenderStateGroup()
{
    NiD3DRenderStateGroup* pkGroup = 
        ms_pkRenderStateGroupPool->GetFreeObject();

    if (!pkGroup->m_bRendererOwned)
    {
        // If the renderer owns this pass, it is part of the NiD3DPass pool
        pkGroup->m_bRendererOwned = true;
    }

    return pkGroup;
}
//---------------------------------------------------------------------------
void NiD3DRenderStateGroup::ReleaseRenderStateGroup(
    NiD3DRenderStateGroup* pkGroup)
{
    if (!pkGroup || !pkGroup->m_bRendererOwned)
    {
        NiDelete pkGroup;
        return;
    }

    // Release any entries
    NiD3DRSEntry* pkEntry;
    NiD3DRSEntry* pkNext;

    pkEntry = pkGroup->m_pkRenderStates;
    while (pkEntry)
    {
        pkNext = pkEntry->GetNext();
        ms_pkRenderStateEntryPool->ReleaseObject(pkEntry);
        pkEntry = pkNext;
    }
    pkGroup->m_pkRenderStates = 0;
    pkGroup->m_uiStateCount = 0;

    pkEntry = pkGroup->m_pkSavedRenderStates;
    while (pkEntry)
    {
        pkNext = pkEntry->GetNext();
        ms_pkRenderStateEntryPool->ReleaseObject(pkEntry);
        pkEntry = pkNext;
    }
    pkGroup->m_pkSavedRenderStates = 0;
    pkGroup->m_uiSavedStateCount = 0;

    ms_pkRenderStateGroupPool->ReleaseObject(pkGroup);
}
//---------------------------------------------------------------------------
unsigned int NiD3DRenderStateGroup::GetTotalRenderStateCount()
{
    return m_uiStateCount + m_uiSavedStateCount;
}
//---------------------------------------------------------------------------
unsigned int NiD3DRenderStateGroup::GetNoSaveRenderStateCount()
{
    return m_uiStateCount;
}
//---------------------------------------------------------------------------
unsigned int NiD3DRenderStateGroup::GetSavedRenderStateCount()
{
    return m_uiSavedStateCount;
}
//---------------------------------------------------------------------------
void NiD3DRenderStateGroup::RemoveRSEntryFromNoSaveList(
    NiD3DRenderStateGroup::NiD3DRSEntry* pkEntry)
{
    // The assumption is that the entry has already been determined
    // to be in the no save list!
    NiD3DRSEntry* pkNext = pkEntry->GetNext();
    NiD3DRSEntry* pkPrev = pkEntry->GetPrev();

    if (pkNext)
        pkNext->SetPrev(pkPrev);
    if (pkPrev)
        pkPrev->SetNext(pkNext);

    if (pkEntry == m_pkRenderStates)
    {
        NIASSERT(!pkPrev);
        m_pkRenderStates = pkNext;
    }

    m_uiStateCount--;

    // Clear the entry just for safety?
    pkEntry->SetNext(0);
    pkEntry->SetPrev(0);
}
//---------------------------------------------------------------------------
void NiD3DRenderStateGroup::RemoveRSEntryFromSavedList(
    NiD3DRenderStateGroup::NiD3DRSEntry* pkEntry)
{
    // The assumption is that the entry has already been determined
    // to be in the saved list!
    NiD3DRSEntry* pkNext = pkEntry->GetNext();
    NiD3DRSEntry* pkPrev = pkEntry->GetPrev();

    if (pkNext)
        pkNext->SetPrev(pkPrev);
    if (pkPrev)
        pkPrev->SetNext(pkNext);

    if (pkEntry == m_pkSavedRenderStates)
    {
        NIASSERT(!pkPrev);
        m_pkSavedRenderStates = pkNext;
    }

    m_uiSavedStateCount--;

    // Clear the entry just for safety?
    pkEntry->SetNext(0);
    pkEntry->SetPrev(0);
}
//---------------------------------------------------------------------------
NiD3DRenderStateGroup::NiD3DRSEntry* 
NiD3DRenderStateGroup::FindRenderStateEntry(unsigned int uiState, 
    bool& bInSaveList)
{
    NiD3DRSEntry* pkCheckEntry;
    
    // Check the no-save list first.
    pkCheckEntry = m_pkRenderStates;
    while (pkCheckEntry)
    {
        if (pkCheckEntry->GetRenderState() == uiState)
        {
            bInSaveList = false;
            return pkCheckEntry;
        }
        pkCheckEntry = pkCheckEntry->GetNext();
    }

    // Check the saved states
    pkCheckEntry = m_pkSavedRenderStates;
    while (pkCheckEntry)
    {
        if (pkCheckEntry->GetRenderState() == uiState)
        {
            bInSaveList = true;
            return pkCheckEntry;
        }
        pkCheckEntry = pkCheckEntry->GetNext();
    }

    // Not found!
    return 0;
}
//---------------------------------------------------------------------------
