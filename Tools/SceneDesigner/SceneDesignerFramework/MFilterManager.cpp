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
#include "SceneDesignerFrameworkPCH.h"

#include "MFilterManager.h"
#include "MEventManager.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MFilterManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MFilterManager();
    }
}
//---------------------------------------------------------------------------
void MFilterManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MFilterManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MFilterManager* MFilterManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MFilterManager::MFilterManager() : m_bFilterEnabled(false)
{
    m_pmFilters = new List<IFilter*>();

    __hook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
        &MFilterManager::OnNewSceneLoaded);
}
//---------------------------------------------------------------------------
void MFilterManager::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        __unhook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
            &MFilterManager::OnNewSceneLoaded);
    }
}
//---------------------------------------------------------------------------
bool MFilterManager::get_FilterEnabled()
{
    MVerifyValidInstance;

    return m_bFilterEnabled;
}
//---------------------------------------------------------------------------
void MFilterManager::set_FilterEnabled(bool bFilterEnabled)
{
    MVerifyValidInstance;

    if (m_bFilterEnabled != bFilterEnabled)
    {
        m_bFilterEnabled = bFilterEnabled;
        MEventManager::Instance->RaiseFilterEnabledChanged();
    }
}
//---------------------------------------------------------------------------
void MFilterManager::AddFilter(IFilter* pmFilter)
{
    MVerifyValidInstance;

    if (!m_pmFilters->Contains(pmFilter))
    {
        m_pmFilters->Add(pmFilter);
        if (m_bFilterEnabled)
        {
            MEventManager::Instance->RaiseFilterOptionsChanged();
        }
    }
}
//---------------------------------------------------------------------------
void MFilterManager::RemoveFilter(IFilter* pmFilter)
{
    MVerifyValidInstance;

    bool bRemoved = m_pmFilters->Remove(pmFilter);
    if (bRemoved && m_bFilterEnabled)
    {
        MEventManager::Instance->RaiseFilterOptionsChanged();
    }
}
//---------------------------------------------------------------------------
void MFilterManager::ClearFilters()
{
    MVerifyValidInstance;

    m_pmFilters->Clear();
    if (m_bFilterEnabled)
    {
        MEventManager::Instance->RaiseFilterOptionsChanged();
    }
}
//---------------------------------------------------------------------------
void MFilterManager::UpdateFilter(IFilter* pmFilter)
{
    MVerifyValidInstance;

    if (!m_pmFilters->Contains(pmFilter))
    {
        AddFilter(pmFilter);
    }
    else if (m_bFilterEnabled)
    {
        MEventManager::Instance->RaiseFilterOptionsChanged();
    }
}
//---------------------------------------------------------------------------
unsigned int MFilterManager::get_FilterCount()
{
    MVerifyValidInstance;

    return m_pmFilters->Count;
}
//---------------------------------------------------------------------------
IFilter* MFilterManager::GetFilterAt(unsigned int uiIndex)
{
    MVerifyValidInstance;

    MAssert(uiIndex < FilterCount, "Invalid index!");

    unsigned int uiCurIndex = 0;
    List<IFilter*>::Enumerator mEnum = m_pmFilters->GetEnumerator();
    while (mEnum.MoveNext())
    {
        if (uiCurIndex++ == uiIndex)
        {
            return mEnum.Current;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
MEntity* MFilterManager::FilterEntities(MEntity* amInputEntities[])[]
{
    MVerifyValidInstance;

    MEntity* amOutputEntities[] = amInputEntities;

    if (FilterEnabled)
    {
        List<IFilter*>::Enumerator mEnum = m_pmFilters->GetEnumerator();
        while (mEnum.MoveNext())
        {
            amOutputEntities = mEnum.Current->FilterEntities(amOutputEntities);
        }
    }

    return amOutputEntities;
}
//---------------------------------------------------------------------------
bool MFilterManager::EntityMatchesFilter(MEntity* pmEntity)
{
    MVerifyValidInstance;

    return (FilterEntities(new MEntity*[] { pmEntity })->Length > 0);
}
//---------------------------------------------------------------------------
void MFilterManager::OnNewSceneLoaded(MScene*)
{
    MVerifyValidInstance;

    FilterEnabled = false;
}
//---------------------------------------------------------------------------
