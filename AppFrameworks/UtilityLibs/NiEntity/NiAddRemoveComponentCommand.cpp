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

#include "NiAddRemoveComponentCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiAddRemoveComponentCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiAddRemoveComponentCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiAddRemoveComponentCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiAddRemoveComponentCommand::DoCommand(NiEntityErrorInterface* pkErrors,
    bool)
{
    if (m_bAddComponent)
    {
        m_bSuccessfullyExecuted = NIBOOL_IS_TRUE(m_spEntity->AddComponent(
            m_spComponent, m_bPerformErrorChecking));

        if (!m_bSuccessfullyExecuted)
        {
            pkErrors->ReportError("Can't do command; an property with the "
                "same name as one contained in the component already exists "
                "in the entity.", GetName(),
                m_spComponent->GetName(), NULL);
        }
    }
    else
    {
        NiEntityComponentInterface* pkComponent = 
            m_spEntity->GetComponentByTemplateID(
            m_spComponent->GetTemplateID());

        if (!pkComponent)
        {
            m_bSuccessfullyExecuted = false;
            pkErrors->ReportError("Can't do command; the specified component "
                "does not exist in the entity and cannot be removed.",
                GetName(), m_spEntity->GetName(), NULL);
        }
        else
        {
            m_bSuccessfullyExecuted = NIBOOL_IS_TRUE(
                m_spEntity->RemoveComponent(pkComponent,
                m_bPerformErrorChecking));
        }
    }
}
//---------------------------------------------------------------------------
void NiAddRemoveComponentCommand::UndoCommand(NiEntityErrorInterface* pkErrors)
{
    if (!m_bSuccessfullyExecuted)
    {
        pkErrors->ReportError("Can't undo command; the command has not yet "
            "been executed successfully.", GetName(), m_spComponent->GetName(),
            NULL);
        return;
    }

    if (m_bAddComponent)
    {
        NIVERIFY(m_spEntity->RemoveComponent(m_spComponent,
            m_bPerformErrorChecking));
    }
    else
    {
        NIVERIFY(m_spEntity->AddComponent(m_spComponent,
            m_bPerformErrorChecking));
    }
}
//---------------------------------------------------------------------------
