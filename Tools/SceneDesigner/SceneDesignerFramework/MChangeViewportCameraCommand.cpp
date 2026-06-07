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

#include "MChangeViewportCameraCommand.h"
#include "MCameraManager.h"
#include "MEventManager.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MChangeViewportCameraCommand::MChangeViewportCameraCommand(
    MViewport* pmViewport, MEntity* pmCamera) : m_pmViewport(pmViewport),
    m_pmNewCamera(pmCamera), m_bOldDataValid(false)
{
    MAssert(m_pmViewport != NULL, "Null viewport provided to constructor!");
    MAssert(m_pmNewCamera != NULL, "Null camera provided to constructor!");
    MAssert(MCameraManager::EntityIsCamera(m_pmNewCamera), "Invalid camera "
        "provided to constructor!");
}
//---------------------------------------------------------------------------
void MChangeViewportCameraCommand::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MChangeViewportCameraCommand::get_Name()
{
    MVerifyValidInstance;

    return String::Format("Change viewport camera to \"{0}\"",
        m_pmNewCamera->Name);
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MChangeViewportCameraCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
void MChangeViewportCameraCommand::DoCommand(bool, bool)
{
    MVerifyValidInstance;

    // Execute command.
    m_pmOldCamera = m_pmViewport->CameraEntity;
    m_bOldDataValid = true;
    m_pmViewport->InternalSetCamera(m_pmNewCamera);

    // Raise event.
    MEventManager::Instance->RaiseViewportCameraChanged(m_pmViewport,
        m_pmNewCamera);
}
//---------------------------------------------------------------------------
void MChangeViewportCameraCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    // Report errors.
    if (!m_bOldDataValid)
    {
        const char* pcCommandName = MStringToCharPointer(this->Name);
        const char* pcCameraName = MStringToCharPointer(m_pmNewCamera->Name);

        NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler(1);
        spErrors->ReportError("Can't undo command; the command must be done "
            "before it can be undone.", pcCommandName, pcCameraName, NULL);

        MFreeCharPointer(pcCameraName);
        MFreeCharPointer(pcCommandName);

        MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
            spErrors);
    }

    // Undo command.
    m_pmViewport->InternalSetCamera(m_pmOldCamera);

    // Raise event.
    MEventManager::Instance->RaiseViewportCameraChanged(m_pmViewport,
        m_pmOldCamera);
}
//---------------------------------------------------------------------------
