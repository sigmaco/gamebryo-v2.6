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
#include "NiEntityPCH.h"

#include "NiRenameSelectionSetCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiRenameSelectionSetCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiRenameSelectionSetCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiRenameSelectionSetCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiRenameSelectionSetCommand::DoCommand(NiEntityErrorInterface*, bool)
{
    m_kOldName = m_spSelectionSet->GetName();
    m_bOldDataValid = true;
    m_spSelectionSet->SetName(m_kNewName);
}
//---------------------------------------------------------------------------
void NiRenameSelectionSetCommand::UndoCommand(
    NiEntityErrorInterface* pkErrors)
{
    if (!m_bOldDataValid)
    {
        pkErrors->ReportError("Can't undo command; the command must be done "
            "before it can be undone.", GetName(),
            m_spSelectionSet->GetName(), NULL);
        return;
    }

    m_spSelectionSet->SetName(m_kOldName);
}
//---------------------------------------------------------------------------
