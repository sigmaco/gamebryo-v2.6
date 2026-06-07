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

#include "MSetPrefabRootCommand.h"
#include "MEventManager.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MSetPrefabRootCommand::MSetPrefabRootCommand()
: m_pmEntries(new List<Entry>())
{
}
//---------------------------------------------------------------------------
void MSetPrefabRootCommand::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
void MSetPrefabRootCommand::Add(MEntity* pmDest, MEntity* pmPrefabRoot)
{
    Entry entry;
    entry.m_pmDest = pmDest;
    entry.m_pmPrefabRoot = pmPrefabRoot;
    m_pmEntries->Add(entry);
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MSetPrefabRootCommand::get_Name()
{
    MVerifyValidInstance;
    return String::Format("Set prefab root for {0} entities", 
        __box(m_pmEntries->Count));
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MSetPrefabRootCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
void MSetPrefabRootCommand::DoCommand(bool, bool)
{
    MVerifyValidInstance;

    // Execute command.
    List<Entry>* pmNewList = new List<Entry>();
    List<Entry>::Enumerator en = m_pmEntries->GetEnumerator();
    while (en.MoveNext())
    {
        Entry entry = en.Current;
        MEntity* pmDest = entry.m_pmDest;
        MEntity* pmOldRoot = pmDest->PrefabRoot;
        pmDest->PrefabRoot = entry.m_pmPrefabRoot;
        entry.m_pmPrefabRoot = pmOldRoot;
        pmNewList->Add(entry);
    }
    m_pmEntries = pmNewList;
}
//---------------------------------------------------------------------------
void MSetPrefabRootCommand::UndoCommand(bool bInBatch)
{
    MVerifyValidInstance;

    // Undo command.
    // because we saved the info in the same struct we just need
    // to redo the command
    DoCommand(bInBatch, false);
}
//---------------------------------------------------------------------------
