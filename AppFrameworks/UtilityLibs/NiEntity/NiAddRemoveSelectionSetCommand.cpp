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

#include "NiAddRemoveSelectionSetCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiAddRemoveSelectionSetCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiAddRemoveSelectionSetCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiAddRemoveSelectionSetCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiAddRemoveSelectionSetCommand::DoCommand(
    NiEntityErrorInterface* pkErrors, bool)
{
    if (m_bAddSelectionSet)
    {
        m_bSuccessfullyExecuted = NIBOOL_IS_TRUE(m_spScene->AddSelectionSet(
            m_spSelectionSet));
        if (!m_bSuccessfullyExecuted)
        {
            pkErrors->ReportError("Can't do command; a selection set with "
                "the same name already exists in the scene.", GetName(),
                m_spSelectionSet->GetName(), NULL);
        }
    }
    else
    {
        NiEntitySelectionSet* pkSelectionSet =
            m_spScene->GetSelectionSetByName(m_spSelectionSet->GetName());
        if (!pkSelectionSet)
        {
            m_bSuccessfullyExecuted = false;
            pkErrors->ReportError("Can't do command; the specified selection "
                "set does not exist in the scene and cannot be removed.",
                GetName(), m_spSelectionSet->GetName(), NULL);
        }
        else
        {
            m_bSuccessfullyExecuted = true;
            m_spScene->RemoveSelectionSet(m_spSelectionSet);
        }
    }
}
//---------------------------------------------------------------------------
void NiAddRemoveSelectionSetCommand::UndoCommand(
    NiEntityErrorInterface* pkErrors)
{
    if (!m_bSuccessfullyExecuted)
    {
        pkErrors->ReportError("Can't undo command; the command has not yet "
            "been executed successfully.", GetName(),
            m_spSelectionSet->GetName(), NULL);
        return;
    }

    if (m_bAddSelectionSet)
    {
        m_spScene->RemoveSelectionSet(m_spSelectionSet);
    }
    else
    {
        NIVERIFY(m_spScene->AddSelectionSet(m_spSelectionSet));
    }
}
//---------------------------------------------------------------------------
