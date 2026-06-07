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

#include "NiRenameEntityCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiRenameEntityCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiRenameEntityCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiRenameEntityCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiRenameEntityCommand::DoCommand(NiEntityErrorInterface* pkErrors, bool)
{
    m_kOldName = m_spEntity->GetName();
    m_bOldDataValid = true;
    NiBool bSuccess = m_spEntity->SetName(m_kNewName);
    if (!bSuccess)
    {
        pkErrors->ReportError("Can't do command; the entity does not allow "
            "its name to be changed.", GetName(), m_spEntity->GetName(),
            NULL);
    }
}
//---------------------------------------------------------------------------
void NiRenameEntityCommand::UndoCommand(NiEntityErrorInterface* pkErrors)
{
    if (!m_bOldDataValid)
    {
        pkErrors->ReportError("Can't undo command; the command must be done "
            "before it can be undone.", GetName(), m_spEntity->GetName(),
            NULL);
        return;
    }

    m_spEntity->SetName(m_kOldName);
}
//---------------------------------------------------------------------------
