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

#include "NiBatchCommand.h"

//---------------------------------------------------------------------------
// NiEntityCommandInterface overrides.
//---------------------------------------------------------------------------
void NiBatchCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiBatchCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiBatchCommand::GetName()
{
    return m_kName;
}
//---------------------------------------------------------------------------
void NiBatchCommand::DoCommand(NiEntityErrorInterface* pkErrors,
    bool bUndoable)
{
    unsigned int uiSize = m_kCommands.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        m_kCommands.GetAt(ui)->DoCommand(pkErrors, bUndoable);
    }
}
//---------------------------------------------------------------------------
void NiBatchCommand::UndoCommand(NiEntityErrorInterface* pkErrors)
{
    for (unsigned int ui = m_kCommands.GetSize(); ui > 0; ui--)
    {
        m_kCommands.GetAt(ui - 1)->UndoCommand(pkErrors);
    }
}
//---------------------------------------------------------------------------
