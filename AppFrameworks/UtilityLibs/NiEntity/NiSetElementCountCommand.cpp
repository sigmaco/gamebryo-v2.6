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

#include "NiSetElementCountCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiSetElementCountCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiSetElementCountCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiSetElementCountCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiSetElementCountCommand::DoCommand(NiEntityErrorInterface* pkErrors,
    bool bUndoable)
{
    if (bUndoable)
    {
        if (m_spPropertyInterface->GetElementCount(m_kPropertyName,
            m_uiOldCount))
        {
            m_bOldCountValid = true;
        }
        else
        {
            pkErrors->ReportError("Unable to get element count.", GetName(),
                m_spPropertyInterface->GetName(), m_kPropertyName);
        }
    }

    bool bCountSet = false;
    if (!m_spPropertyInterface->SetElementCount(m_kPropertyName, m_uiNewCount,
        bCountSet))
    {
        pkErrors->ReportError("Unable to set element count.", GetName(),
            m_spPropertyInterface->GetName(), m_kPropertyName);
    }
    m_bAttemptUndo = bUndoable && bCountSet;
}
//---------------------------------------------------------------------------
void NiSetElementCountCommand::UndoCommand(NiEntityErrorInterface* pkErrors)
{
    if (!m_bAttemptUndo)
    {
        return;
    }

    if (!m_bOldCountValid)
    {
        pkErrors->ReportError("Can't undo command; no old element count "
            "stored.", GetName(), m_spPropertyInterface->GetName(),
            m_kPropertyName);
        return;
    }

    bool bCountSet;
    if (!m_spPropertyInterface->SetElementCount(m_kPropertyName, m_uiOldCount,
        bCountSet))
    {
        pkErrors->ReportError("Can't undo command; unable to set old element "
            "count.", GetName(), m_spPropertyInterface->GetName(),
            m_kPropertyName);
        return;
    }
    if (!bCountSet)
    {
        pkErrors->ReportError("Error undoing command; old element could not "
            "be set.", GetName(), m_spPropertyInterface->GetName(),
            m_kPropertyName);
    }
}
//---------------------------------------------------------------------------
