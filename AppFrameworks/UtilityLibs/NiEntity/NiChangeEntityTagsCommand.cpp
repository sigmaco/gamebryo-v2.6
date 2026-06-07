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

#include "NiChangeEntityTagsCommand.h"

//---------------------------------------------------------------------------
NiChangeEntityTagsCommand::NiChangeEntityTagsCommand(
    NiEntityInterface* pkEntity,
    TagOperation eTagOp,
    const NiFixedString& kTag) :
    m_spEntity(pkEntity),
    m_kTag(kTag),
    m_eTagOp(eTagOp),
    m_bOldDataValid(false)
{
    NIASSERT(m_spEntity);

    const char* pcTagOp = NULL;
    const char* pcPrep = NULL;
    switch (m_eTagOp)
    {
        case TAGOP_SET:
            pcTagOp = "Set";
            pcPrep = "on";
            break;
        case TAGOP_ADD:
            pcTagOp = "Add";
            pcPrep = "to";
            break;
        case TAGOP_REMOVE:
            pcTagOp = "Remove";
            pcPrep = "from";
            break;
        default:
            NIASSERT(!"Unknown TagOperation!");
            break;
    }
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, 
        "%s \"%s\" tag %s \"%s\" entity.",
        pcTagOp, (const char*) m_kTag, pcPrep,
        (const char*) pkEntity->GetName());
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
void NiChangeEntityTagsCommand::AddReference()
{
    IncRefCount();
}
//---------------------------------------------------------------------------
void NiChangeEntityTagsCommand::RemoveReference()
{
    DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiChangeEntityTagsCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiChangeEntityTagsCommand::DoCommand(
    NiEntityErrorInterface*,
    bool)
{
    // Backup old data.
    m_kOldTags = m_spEntity->GetTags();
    m_bOldDataValid = true;

    // Set new data.
    switch (m_eTagOp)
    {
        case TAGOP_SET:
            m_spEntity->SetTags(m_kTag);
            break;
        case TAGOP_ADD:
            m_spEntity->AddTag(m_kTag);
            break;
        case TAGOP_REMOVE:
            m_spEntity->RemoveTag(m_kTag);
            break;
        default:
            NIASSERT(!"Unknown TagOperation!");
            break;
    }
}
//---------------------------------------------------------------------------
void NiChangeEntityTagsCommand::UndoCommand(NiEntityErrorInterface* pkErrors)
{
    if (!m_bOldDataValid)
    {
        pkErrors->ReportError("Can't undo command; the command must be done "
            "before it can be undone.", GetName(), m_spEntity->GetName(),
            NULL);
        return;
    }

    m_spEntity->SetTags(m_kOldTags);
}
//---------------------------------------------------------------------------
