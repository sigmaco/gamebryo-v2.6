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

#include "MSelectionSetFactory.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MSelectionSetFactory::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MSelectionSetFactory();
    }
}
//---------------------------------------------------------------------------
void MSelectionSetFactory::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MSelectionSetFactory::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MSelectionSetFactory* MSelectionSetFactory::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MSelectionSetFactory::MSelectionSetFactory()
{
    m_pmUnmanagedToManaged = new Hashtable();
}
//---------------------------------------------------------------------------
void MSelectionSetFactory::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        Clear();
    }
}
//---------------------------------------------------------------------------
MSelectionSet* MSelectionSetFactory::Get(NiEntitySelectionSet* pkUnmanaged)
{
    MVerifyValidInstance;

    if (pkUnmanaged == NULL)
    {
        return NULL;
    }

    Object* pmKey = __box((unsigned int) PtrToUint(pkUnmanaged));

    MSelectionSet* pmManaged = dynamic_cast<MSelectionSet*>(
        m_pmUnmanagedToManaged->Item[pmKey]);
    if (pmManaged == NULL)
    {
        pmManaged = new MSelectionSet(pkUnmanaged);
        m_pmUnmanagedToManaged->Item[pmKey] = pmManaged;
    }

    return pmManaged;
}
//---------------------------------------------------------------------------
void MSelectionSetFactory::Remove(NiEntitySelectionSet* pkUnmanaged)
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
void MSelectionSetFactory::Remove(MSelectionSet* pmManaged)
{
    MVerifyValidInstance;

    Remove(pmManaged->GetNiEntitySelectionSet());
}
//---------------------------------------------------------------------------
void MSelectionSetFactory::Clear()
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
