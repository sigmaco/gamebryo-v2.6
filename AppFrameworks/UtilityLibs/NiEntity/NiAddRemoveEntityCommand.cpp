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

#include "NiAddRemoveEntityCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiAddRemoveEntityCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiAddRemoveEntityCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiAddRemoveEntityCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiAddRemoveEntityCommand::DoCommand(NiEntityErrorInterface* pkErrors,
    bool)
{
    if (m_bAddEntity)
    {
        m_bSuccessfullyExecuted = NIBOOL_IS_TRUE(m_spScene->AddEntity(
            m_spEntity));
        if (!m_bSuccessfullyExecuted)
        {
            pkErrors->ReportError("Can't do command; an entity with the same "
                "name already exists in the scene.", GetName(),
                m_spEntity->GetName(), NULL);
        }
    }
    else
    {
        if (!m_spScene->IsEntityInScene(m_spEntity))
        {
            m_bSuccessfullyExecuted = false;
            pkErrors->ReportError("Can't do command; the specified entity "
                "does not exist in the scene and cannot be removed.",
                GetName(), m_spEntity->GetName(), NULL);
        }
        else
        {
            m_bSuccessfullyExecuted = true;
            m_spScene->RemoveEntity(m_spEntity);
        }
    }
}
//---------------------------------------------------------------------------
void NiAddRemoveEntityCommand::UndoCommand(NiEntityErrorInterface* pkErrors)
{
    if (!m_bSuccessfullyExecuted)
    {
        pkErrors->ReportError("Can't undo command; the command has not yet "
            "been executed successfully.", GetName(), m_spEntity->GetName(),
            NULL);
        return;
    }

    if (m_bAddEntity)
    {
        m_spScene->RemoveEntity(m_spEntity);
    }
    else
    {
        NIVERIFY(m_spScene->AddEntity(m_spEntity));
    }
}
//---------------------------------------------------------------------------
