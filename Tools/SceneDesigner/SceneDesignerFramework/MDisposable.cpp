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

#include "MDisposable.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MDisposable::MDisposable() : m_bDisposed(false)
{
}
//---------------------------------------------------------------------------
MDisposable::~MDisposable()
{
    Dispose(true);
}
//---------------------------------------------------------------------------
void MDisposable::Dispose()
{
    Dispose(false);
}
//---------------------------------------------------------------------------
void MDisposable::Dispose(bool bFromDestructor)
{
     __try
    {
        Threading::Monitor::Enter(this);
   if (!m_bDisposed)
    {
        Do_Dispose(!bFromDestructor);
        if (!bFromDestructor)
        {
            GC::SuppressFinalize(this);
        }
        m_bDisposed = true;
    }
    }
    __finally
    {
        Threading::Monitor::Exit(this);
    }
}
//---------------------------------------------------------------------------
bool MDisposable::HasBeenDisposed()
{
    return m_bDisposed;
}
//---------------------------------------------------------------------------
