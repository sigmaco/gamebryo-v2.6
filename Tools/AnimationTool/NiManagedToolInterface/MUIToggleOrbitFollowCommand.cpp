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

#include "stdafx.h"
#include "MUIToggleOrbitFollowCommand.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIToggleOrbitFollowCommand::MUIToggleOrbitFollowCommand()
{
    m_pkOrbitPt = NULL;
}
//---------------------------------------------------------------------------
String* MUIToggleOrbitFollowCommand::GetName()
{
    return "Toggle Orbit Follow";
}
//---------------------------------------------------------------------------
bool MUIToggleOrbitFollowCommand::Execute(MUIState*)
{
    if (m_pkOrbitPt != NULL)
        m_pkOrbitPt->Follow = !m_pkOrbitPt->Follow;
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIToggleOrbitFollowCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIToggleOrbitFollowCommand::RefreshData()
{
    DeleteContents();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIToggleOrbitFollowCommand::IsToggle()
{
    return true;
}
//---------------------------------------------------------------------------
bool MUIToggleOrbitFollowCommand::GetActive()
{
    if (m_pkOrbitPt != NULL)
        return m_pkOrbitPt->Follow;
    else
        return false;
}
//---------------------------------------------------------------------------
bool MUIToggleOrbitFollowCommand::GetEnabled()
{
    return true;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIToggleOrbitFollowCommand::GetCommandType()
{
    return MUICommand::TOGGLE_ORBIT_FOLLOW;
}
//---------------------------------------------------------------------------
void MUIToggleOrbitFollowCommand::SetOrbitPoint(MOrbitPoint* pkOrbitPt)
{
    m_pkOrbitPt = pkOrbitPt;
}
//---------------------------------------------------------------------------
