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

#include "MSceneFactory.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MSceneFactory::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MSceneFactory();
    }
}
//---------------------------------------------------------------------------
void MSceneFactory::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MSceneFactory::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MSceneFactory* MSceneFactory::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MSceneFactory::MSceneFactory()
{
    m_pmUnmanagedToManaged = new Hashtable();
}
//---------------------------------------------------------------------------
void MSceneFactory::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        Clear();
    }
}
//---------------------------------------------------------------------------
MScene* MSceneFactory::Get(NiScene* pkUnmanaged)
{
    MVerifyValidInstance;

    if (pkUnmanaged == NULL)
    {
        return NULL;
    }

    Object* pmKey = __box((unsigned int) PtrToUint(pkUnmanaged));

    MScene* pmManaged = dynamic_cast<MScene*>(
        m_pmUnmanagedToManaged->Item[pmKey]);
    if (pmManaged == NULL)
    {
        pmManaged = new MScene(pkUnmanaged);
        m_pmUnmanagedToManaged->Item[pmKey] = pmManaged;
    }

    return pmManaged;
}
//---------------------------------------------------------------------------
void MSceneFactory::Remove(NiScene* pkUnmanaged)
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
void MSceneFactory::Remove(MScene* pmManaged)
{
    MVerifyValidInstance;

    Remove(pmManaged->GetNiScene());
}
//---------------------------------------------------------------------------
void MSceneFactory::Clear()
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
