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

#include "NiSelectEntitiesCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiSelectEntitiesCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiSelectEntitiesCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiSelectEntitiesCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiSelectEntitiesCommand::DoCommand(NiEntityErrorInterface* pkErrors,
    bool)
{
    if (!m_bNewEntitiesValid)
    {
        pkErrors->ReportError("Can't do command; no entities specified.",
            GetName(), NULL, NULL);
        return;
    }

    // Store old entities.
    unsigned int uiNumOldEntities = m_spSelectionSet->GetEntityCount();
    m_kOldEntities.SetSize(uiNumOldEntities);
    m_kOldEntities.RemoveAll();
    for (unsigned int ui = 0; ui < uiNumOldEntities; ui++)
    {
        NiEntityInterface* pkEntity = m_spSelectionSet->GetEntityAt(ui);
        if (pkEntity)
        {
            m_kOldEntities.Add(pkEntity);
        }
    }
    m_bOldDataValid = true;

    // Perform command.
    for (unsigned int ui = 0; ui < m_kNewEntities.GetSize(); ui++)
    {
        NiEntityInterface* pkEntity = m_kNewEntities.GetAt(ui);
        if (pkEntity)
        {
            if (m_bAddToSelectionSet)
            {
                m_spSelectionSet->AddEntity(pkEntity);
            }
            else
            {
                m_spSelectionSet->RemoveEntity(pkEntity);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiSelectEntitiesCommand::UndoCommand(NiEntityErrorInterface* pkErrors)
{
    if (!m_bOldDataValid)
    {
        pkErrors->ReportError("Can't undo command; the command must be done "
            "before it can be undone.", GetName(), NULL, NULL);
        return;
    }

    m_spSelectionSet->RemoveAllEntities();
    for (unsigned int ui = 0; ui < m_kOldEntities.GetSize(); ui++)
    {
        NiEntityInterface* pkEntity = m_kOldEntities.GetAt(ui);
        if (pkEntity)
        {
            m_spSelectionSet->AddEntity(pkEntity);
        }
    }
}
//---------------------------------------------------------------------------
