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

#include "MChangeLayerStateCommand.h"
#include "MLayer.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MChangeLayerStateCommand::MChangeLayerStateCommand(MLayer* pmLayer, 
                                                   StateType eState,
                                                   bool bVisibleOrLocked)
: m_pmLayer(pmLayer)
, m_eState(eState)
, m_bVisibleOrLocked(bVisibleOrLocked)
{
    NIASSERT(pmLayer != NULL);
}
//---------------------------------------------------------------------------
void MChangeLayerStateCommand::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MChangeLayerStateCommand::get_Name()
{
    MVerifyValidInstance;

    String* strState = m_eState == Visible ? "visible" : "locked";
    return String::Format("Change {0} state of \"{1}\" layer", 
        strState, m_pmLayer->Name);
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MChangeLayerStateCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
void MChangeLayerStateCommand::DoCommand(bool, bool)
{
    MVerifyValidInstance;

    NIASSERT(m_pmLayer != NULL);

    // Execute command.
    if (m_eState == Visible)
    {
        m_pmLayer->DoChangeVisibleStatus(m_bVisibleOrLocked);
    } else
    {
        m_pmLayer->DoChangeLockedStatus(m_bVisibleOrLocked);
    }
}
//---------------------------------------------------------------------------
void MChangeLayerStateCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    NIASSERT(m_pmLayer != NULL);

    // Execute command.
    if (m_eState == Visible)
    {
        m_pmLayer->DoChangeVisibleStatus(!m_bVisibleOrLocked);
    } else
    {
        m_pmLayer->DoChangeLockedStatus(!m_bVisibleOrLocked);
    }
}
//---------------------------------------------------------------------------
