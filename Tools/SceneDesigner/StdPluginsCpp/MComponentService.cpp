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
#include "StdPluginsCppPCH.h"

#include "MComponentService.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MComponentService::MComponentService()
{
    m_pmNameToComponent = new Hashtable();
    m_pmIDToComponent = new Hashtable();
    m_pmIDToVisible = new Hashtable();
}
//---------------------------------------------------------------------------
void MComponentService::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        UnregisterAllComponents();
    }
}
//---------------------------------------------------------------------------
// IService members.
//---------------------------------------------------------------------------
String* MComponentService::get_Name()
{
    MVerifyValidInstance;

    return "Component Service";
}
//---------------------------------------------------------------------------
bool MComponentService::Initialize()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
bool MComponentService::Start()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
// IComponentService members.
//---------------------------------------------------------------------------
bool MComponentService::RegisterComponent(MComponent* pmComponent)
{
    MVerifyValidInstance;

    return RegisterComponent(pmComponent, true);
}
//---------------------------------------------------------------------------
bool MComponentService::RegisterComponent(MComponent* pmComponent,
    bool bVisibleToUser)
{
    MVerifyValidInstance;

    MAssert(pmComponent != NULL, "Null component provided to function!");

    if (m_pmIDToComponent->Contains(__box(pmComponent->TemplateID)))
    {
        return false;
    }

    m_pmNameToComponent->Item[pmComponent->Name] = pmComponent;
    m_pmIDToComponent->Item[__box(pmComponent->TemplateID)] = pmComponent;
    m_pmIDToVisible->Item[__box(pmComponent->TemplateID)] =
        __box(bVisibleToUser);

    MFramework::Instance->EventManager->RaiseComponentServiceChanged(
        pmComponent);

    return true;
}
//---------------------------------------------------------------------------
void MComponentService::UnregisterComponent(MComponent* pmComponent)
{
    MVerifyValidInstance;

    MAssert(pmComponent != NULL, "Null component provided to function!");

    m_pmNameToComponent->Remove(pmComponent->Name);
    m_pmIDToComponent->Remove(__box(pmComponent->TemplateID));
    m_pmIDToVisible->Remove(__box(pmComponent->TemplateID));
}
//---------------------------------------------------------------------------
void MComponentService::UnregisterAllComponents()
{
    MVerifyValidInstance;

    IDictionaryEnumerator* pmEnumerator =
        m_pmNameToComponent->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
            pmEnumerator->Value);
        if (pmDisposable != NULL)
        {
            pmDisposable->Dispose();
        }
    }
    m_pmNameToComponent->Clear();
    m_pmIDToComponent->Clear();
    m_pmIDToVisible->Clear();
}
//---------------------------------------------------------------------------
MComponent* MComponentService::GetAllComponents()[]
{
    MVerifyValidInstance;

    MComponent* amComponents[] = 
        new MComponent*[m_pmNameToComponent->Count];

    int iIndex = 0;

    IDictionaryEnumerator* pmEnumerator =
        m_pmNameToComponent->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        MComponent* pmComponent = 
            dynamic_cast<MComponent*>(pmEnumerator->Value);
        MAssert(pmComponent != NULL, "Bad hashtable key!");
        amComponents[iIndex++] = pmComponent;
    }
    return amComponents;
}
//---------------------------------------------------------------------------
String* MComponentService::GetComponentNames()[]
{
    MVerifyValidInstance;

    String* astrNames[] = new String*[m_pmNameToComponent->Count];
    int iIndex = 0;

    IDictionaryEnumerator* pmEnumerator =
        m_pmNameToComponent->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        String* strName = dynamic_cast<String*>(pmEnumerator->Key);
        MAssert(strName != NULL, "Bad hashtable key!");
        astrNames[iIndex++] = strName;
    }

    return astrNames;
}
//---------------------------------------------------------------------------
MComponent* MComponentService::CloneComponentByID(Guid mGuid)
{
    MVerifyValidInstance;

    MComponent* pmClone = NULL;

    MComponent* pmComponent = dynamic_cast<MComponent*>(
        m_pmIDToComponent->Item[__box(mGuid)]);
    if (pmComponent != NULL)
    {
        pmClone = pmComponent->Clone(false);
    }

    return pmClone;
}
//---------------------------------------------------------------------------
MComponent* MComponentService::GetComponentByID(Guid mGuid)
{
    MVerifyValidInstance;

    MComponent* pmComponent = dynamic_cast<MComponent*>(
        m_pmIDToComponent->Item[__box(mGuid)]);

    return pmComponent;
}
//---------------------------------------------------------------------------
bool MComponentService::IsComponentVisibleToUser(MComponent* pmComponent)
{
    MVerifyValidInstance;

    MAssert(m_pmIDToVisible->Contains(__box(pmComponent->TemplateID)),
        "Component has not been registered with service!");

    return *dynamic_cast<__box bool*>(m_pmIDToVisible->Item[
        __box(pmComponent->TemplateID)]);
}
//---------------------------------------------------------------------------
