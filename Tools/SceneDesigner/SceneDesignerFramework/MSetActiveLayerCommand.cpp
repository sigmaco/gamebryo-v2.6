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
#include "SceneDesignerFrameworkPCH.h"

#include "MSetActiveLayerCommand.h"
#include "MEventManager.h"
#include "MUtility.h"
#include "MLayerManager.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MSetActiveLayerCommand::MSetActiveLayerCommand(MLayer* oldActive, 
                                               MLayer* newActive)
: m_pmOldActive(oldActive)
, m_pmNewActive(newActive)
{
}
//---------------------------------------------------------------------------
void MSetActiveLayerCommand::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MSetActiveLayerCommand::get_Name()
{
    MVerifyValidInstance;

    return String::Format("Set active layer to \"{0}\"", 
        m_pmNewActive != NULL ? m_pmNewActive->Name : "");
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MSetActiveLayerCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
void MSetActiveLayerCommand::DoCommand(bool, bool)
{
    MVerifyValidInstance;

    // Execute command.
    MLayerManager::Instance->DoSetActiveLayer(m_pmNewActive, true);
}
//---------------------------------------------------------------------------
void MSetActiveLayerCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    // Report errors.
    if (MLayerManager::Instance->ActiveLayer == m_pmOldActive)
    {
        const char* pcCommandName = MStringToCharPointer(this->Name);

        NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler(1);
        spErrors->ReportError("Can't undo command; the command must be done "
            "before it can be undone.", pcCommandName, NULL, NULL);

        MFreeCharPointer(pcCommandName);

        MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
            spErrors);
    }

    // Undo command.
    MLayerManager::Instance->DoSetActiveLayer(m_pmOldActive, true);
}
//---------------------------------------------------------------------------
