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

#include "NiChangePropertyCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiChangePropertyCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiChangePropertyCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiChangePropertyCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiChangePropertyCommand::DoCommand(NiEntityErrorInterface* pkErrors, 
    bool bUndoable)
{
    // Detect whether or not the index is out of range. This will be used to
    // determine what to do when undoing the command.
    unsigned int uiCount;
    if (!m_spEntityPropertyInterface->GetElementCount(m_kPropertyName,
        uiCount))
    {
        pkErrors->ReportError("Command execution failed; GetElementCount "
            "failed.", GetName(), m_spEntityPropertyInterface->GetName(),
            m_kPropertyName);
        return;
    }
    m_bIndexInRange = (m_uiPropertyIndex < uiCount);

    // Detect whether or not this command will cause the property to become
    // unique. This will be used to determine what to do when undoing the
    // command.
    bool bIsUnique;
    if (!m_spEntityPropertyInterface->IsPropertyUnique(m_kPropertyName,
        bIsUnique))
    {
        pkErrors->ReportError("Command execution failed; IsPropertyUnique "
            "failed.", GetName(), m_spEntityPropertyInterface->GetName(),
            m_kPropertyName);
        return;
    }
    m_bShouldBeReset = !bIsUnique;

    // Store the old data.
    m_bOldDataValid = bUndoable && NIBOOL_IS_TRUE(StoreOldData());

    // Set the new data.
    if (!SetNewData())
    {
        pkErrors->ReportError("Command execution failed; the new data could "
            "not be set.", GetName(), m_spEntityPropertyInterface->GetName(),
            m_kPropertyName);
        return;
    }
}
//---------------------------------------------------------------------------
void NiChangePropertyCommand::UndoCommand(NiEntityErrorInterface* pkErrors)
{
    // If the old data was not properly stored, report an error and abort.
    if (!m_bOldDataValid && m_bIndexInRange)
    {
        pkErrors->ReportError("Command undo failed; the old data was not "
            "properly stored.", GetName(),
            m_spEntityPropertyInterface->GetName(), m_kPropertyName);
        return;
    }

    // If the old data is valid, restore it.
    if (m_bOldDataValid)
    {
        if (!SetOldData())
        {
            pkErrors->ReportError("Command undo failed; the old data could "
                "not be set.", GetName(), 
                m_spEntityPropertyInterface->GetName(), m_kPropertyName);
        }
        if (m_bShouldBeReset)
        {
            m_spEntityPropertyInterface->ResetProperty(m_kPropertyName);
        }
    }
}
//---------------------------------------------------------------------------
