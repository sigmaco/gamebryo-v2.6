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

#include "MComponentFactory.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MComponentFactory::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MComponentFactory();
    }
}
//---------------------------------------------------------------------------
void MComponentFactory::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MComponentFactory::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MComponentFactory* MComponentFactory::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MComponentFactory::MComponentFactory()
{
    m_pmUnmanagedToManaged = new Hashtable();
}
//---------------------------------------------------------------------------
void MComponentFactory::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        Clear();
    }
}
//---------------------------------------------------------------------------
MComponent* MComponentFactory::Get(NiEntityComponentInterface* pkUnmanaged)
{
    MVerifyValidInstance;

    if (pkUnmanaged == NULL)
    {
        return NULL;
    }

    Object* pmKey = __box((unsigned int) PtrToUint(pkUnmanaged));

    MComponent* pmManaged = dynamic_cast<MComponent*>(
        m_pmUnmanagedToManaged->Item[pmKey]);
    if (pmManaged == NULL)
    {
        pmManaged = new MComponent(pkUnmanaged);
        m_pmUnmanagedToManaged->Item[pmKey] = pmManaged;
    }

    return pmManaged;
}
//---------------------------------------------------------------------------
void MComponentFactory::Remove(NiEntityComponentInterface* pkUnmanaged)
{
    MVerifyValidInstance;

    Object* pmKey = __box((unsigned int) PtrToUint(pkUnmanaged));

    IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
        m_pmUnmanagedToManaged->Item[pmKey]);

    m_pmUnmanagedToManaged->Remove(pmKey);

    if (pmDisposable != NULL)
    {
        pmDisposable->Dispose();
    }
}
//---------------------------------------------------------------------------
void MComponentFactory::Remove(MComponent* pmManaged)
{
    MVerifyValidInstance;

    Remove(pmManaged->GetNiEntityComponentInterface());
}
//---------------------------------------------------------------------------
void MComponentFactory::Clear()
{
    MVerifyValidInstance;

    IDictionaryEnumerator* pmEnumerator =
        m_pmUnmanagedToManaged->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
            pmEnumerator->Value);
        if (pmDisposable != NULL)
        {
            pmDisposable->Dispose();
        }
    }
    m_pmUnmanagedToManaged->Clear();
}
//---------------------------------------------------------------------------
