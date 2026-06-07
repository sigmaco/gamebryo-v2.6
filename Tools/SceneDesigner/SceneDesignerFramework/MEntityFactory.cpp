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

#include "MEntityFactory.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MEntityFactory::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MEntityFactory();
    }
}
//---------------------------------------------------------------------------
void MEntityFactory::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MEntityFactory::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MEntityFactory* MEntityFactory::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MEntityFactory::MEntityFactory()
{
    m_pmUnmanagedToManaged = new Hashtable();
}
//---------------------------------------------------------------------------
void MEntityFactory::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        Clear();
    }
}
//---------------------------------------------------------------------------
MEntity* MEntityFactory::Get(NiEntityInterface* pkUnmanaged)
{
    MVerifyValidInstance;

    if (pkUnmanaged == NULL)
    {
        return NULL;
    }

    Object* pmKey = __box((unsigned int) PtrToUint(pkUnmanaged));

    MEntity* pmManaged = dynamic_cast<MEntity*>(
        m_pmUnmanagedToManaged->Item[pmKey]);
    if (pmManaged == NULL)
    {
        pmManaged = new MEntity(pkUnmanaged);
        m_pmUnmanagedToManaged->Item[pmKey] = pmManaged;
    }

    return pmManaged;
}
//---------------------------------------------------------------------------
void MEntityFactory::Remove(NiEntityInterface* pkUnmanaged)
{
    MVerifyValidInstance;

    Object* pmKey = __box((unsigned int) PtrToUint(pkUnmanaged));

    if (m_pmUnmanagedToManaged->Contains(pmKey))
    {
        IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
            m_pmUnmanagedToManaged->Item[pmKey]);
        if (pmDisposable != NULL)
        {
            pmDisposable->Dispose();
        }
        m_pmUnmanagedToManaged->Remove(pmKey);
    }
}
//---------------------------------------------------------------------------
void MEntityFactory::Remove(MEntity* pmManaged)
{
    MVerifyValidInstance;

    Remove(pmManaged->GetNiEntityInterface());
}
//---------------------------------------------------------------------------
void MEntityFactory::Clear()
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
