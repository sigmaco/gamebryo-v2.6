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

#include "NiAddRemovePropertyCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiAddRemovePropertyCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiAddRemovePropertyCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiAddRemovePropertyCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiAddRemovePropertyCommand::DoCommand(NiEntityErrorInterface* pkErrors,
    bool)
{
    NiFixedString kDisplayName;
    NiBool bPropertyExists = m_spPropertyInterface->GetDisplayName(
        m_kPropertyName, kDisplayName);

    if (m_bAddProperty && !bPropertyExists)
    {
        // If the property does not already exist, add it.
        m_bSuccessfullyExecuted = NIBOOL_IS_TRUE(m_spPropertyInterface->
            AddProperty(m_kPropertyName, m_kDisplayName, m_kPrimitiveType,
            m_kSemanticType, m_kDescription));

        if (m_bSuccessfullyExecuted)
        {
            NIVERIFY(m_spPropertyInterface->MakeCollection(m_kPropertyName,
                m_bCollection));
        }
        else
        {
            pkErrors->ReportError("Can't do command; error adding "
                "property.", GetName(), m_spPropertyInterface->GetName(),
                NULL);
        }
        

        m_bShouldUndo = true;
    }
    else if (!m_bAddProperty && bPropertyExists)
    {
        // If the property exists, remove it.
        NIVERIFY(m_spPropertyInterface->IsCollection(m_kPropertyName,
            m_bCollection));

        m_bSuccessfullyExecuted = NIBOOL_IS_TRUE(m_spPropertyInterface->
            RemoveProperty(m_kPropertyName));

        if (!m_bSuccessfullyExecuted)
        {
            pkErrors->ReportError("Can't do command; the specified property "
                "does not exist in the entity/component and cannot be "
                "removed or the property is inherited from a master.",
                GetName(), m_spPropertyInterface->GetName(), NULL);
        }

        m_bShouldUndo = true;
    }
}
//---------------------------------------------------------------------------
void NiAddRemovePropertyCommand::UndoCommand(NiEntityErrorInterface* pkErrors)
{
    if (m_bShouldUndo)
    {
        if (!m_bSuccessfullyExecuted)
        {
            pkErrors->ReportError("Can't undo command; the command has not "
                "yet been executed successfully.", GetName(), m_kPropertyName,
                NULL);
            return;
        }

        if (m_bAddProperty)
        {
            NIVERIFY(m_spPropertyInterface->RemoveProperty(
                m_kPropertyName));
        }
        else
        {
            NIVERIFY(m_spPropertyInterface->AddProperty(
                m_kPropertyName, m_kDisplayName, m_kPrimitiveType,
                m_kSemanticType, m_kDescription));
            NIVERIFY(m_spPropertyInterface->MakeCollection(m_kPropertyName,
                m_bCollection));
        }
    }
}
//---------------------------------------------------------------------------
