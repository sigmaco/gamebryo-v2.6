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

#include "MSetMasterEntityCommand.h"
#include "MEventManager.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MSetMasterEntityCommand::MSetMasterEntityCommand()
: m_pmEntries(new List<Entry>())
{
}
//---------------------------------------------------------------------------
void MSetMasterEntityCommand::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
void MSetMasterEntityCommand::Add(MEntity* pmDest, MEntity* pmMaster)
{
    Entry entry;
    entry.m_pmDest = pmDest;
    entry.m_pmMaster = pmMaster;
    m_pmEntries->Add(entry);
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MSetMasterEntityCommand::get_Name()
{
    MVerifyValidInstance;
    return String::Format("Set master entities for {0} entities", 
        __box(m_pmEntries->Count));
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MSetMasterEntityCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
void MSetMasterEntityCommand::DoCommand(bool, bool)
{
    MVerifyValidInstance;

    // Execute command.
    List<Entry>::Enumerator en = m_pmEntries->GetEnumerator();
    while (en.MoveNext())
    {
        MEntity* pmDest = en.Current.m_pmDest;
        MEntity* pmOldMaster = pmDest->MasterEntity;
        pmDest->MasterEntity = en.Current.m_pmMaster;
        en.Current.m_pmMaster = pmOldMaster;
    }
}
//---------------------------------------------------------------------------
void MSetMasterEntityCommand::UndoCommand(bool bInBatch)
{
    MVerifyValidInstance;

    // Undo command.
    // because we saved the info in the same struct we just need
    // to redo the command
    DoCommand(bInBatch, false);
}
//---------------------------------------------------------------------------
