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

#include "NiChangeHiddenStateCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiChangeHiddenStateCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiChangeHiddenStateCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiChangeHiddenStateCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiChangeHiddenStateCommand::DoCommand(NiEntityErrorInterface*, bool)
{
    m_bOldHiddenState = NIBOOL_IS_TRUE(m_spEntity->GetHidden());
    m_bOldDataValid = true;
    m_spEntity->SetHidden(m_bNewHiddenState);
}
//---------------------------------------------------------------------------
void NiChangeHiddenStateCommand::UndoCommand(NiEntityErrorInterface* pkErrors)
{
    if (!m_bOldDataValid)
    {
        pkErrors->ReportError("Can't undo command; the command must be done "
            "before it can be undone.", GetName(), m_spEntity->GetName(),
            NULL);
        return;
    }

    m_spEntity->SetHidden(m_bOldHiddenState);
}
//---------------------------------------------------------------------------
