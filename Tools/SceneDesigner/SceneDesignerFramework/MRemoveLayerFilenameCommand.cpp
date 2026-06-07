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

#include "MRemoveLayerFilenameCommand.h"
#include "MEventManager.h"
#include "MUtility.h"
#include "MLayerManager.h"
#include "MFramework.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MRemoveLayerFilenameCommand::MRemoveLayerFilenameCommand(String* filename,
    MLayer* pmLayer)
: m_filename(filename)
, m_pmLayer(pmLayer)
{
}
//---------------------------------------------------------------------------
void MRemoveLayerFilenameCommand::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MRemoveLayerFilenameCommand::get_Name()
{
    MVerifyValidInstance;

    return String::Format("Remove \"{0}\" from \"{1}\" layer", m_filename,
        m_pmLayer->Name);
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MRemoveLayerFilenameCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
void MRemoveLayerFilenameCommand::DoCommand(bool, bool)
{
    MVerifyValidInstance;

    MLayerManager* LayerManager = MLayerManager::Instance;

    // Execute command.
    MLayer* pmCondemned = LayerManager->FindLayerByName(m_filename);
    NIASSERT(pmCondemned != NULL);
    m_pmLayer->DoRemoveExplicitLayer(pmCondemned);
    m_pmLayer->Dirty = true;
}
//---------------------------------------------------------------------------
void MRemoveLayerFilenameCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    // Undo command.
    MLayerManager* LayerManager = MLayerManager::Instance;
    MLayer* pmResurected = LayerManager->FindLayerByName(m_filename);
    NIASSERT(pmResurected != NULL);
    m_pmLayer->DoAddExplicitLayer(pmResurected);
}
//---------------------------------------------------------------------------
