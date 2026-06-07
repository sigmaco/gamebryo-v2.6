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
#include "MUIResetOrbitPointCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
#include "MUIState.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIResetOrbitPointCommand::MUIResetOrbitPointCommand()
{
    m_pkOrbitPt = NULL;
}
//---------------------------------------------------------------------------
void MUIResetOrbitPointCommand::SetOrbitPoint(MOrbitPoint* pkOrbitPt)
{
    m_pkOrbitPt = pkOrbitPt;
}
//---------------------------------------------------------------------------
String* MUIResetOrbitPointCommand::GetName()
{
    return "Reset Orbit Point";
}
//---------------------------------------------------------------------------
bool MUIResetOrbitPointCommand::Execute(MUIState*)
{
    if (m_pkOrbitPt)
        m_pkOrbitPt->ResetOrbitPoint();
    return true;
}
//---------------------------------------------------------------------------
void MUIResetOrbitPointCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIResetOrbitPointCommand::RefreshData()
{
    DeleteContents();
}
//---------------------------------------------------------------------------
bool MUIResetOrbitPointCommand::GetEnabled()
{
    return m_pkOrbitPt != NULL;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIResetOrbitPointCommand::GetCommandType()
{
    return MUICommand::RESET_ORBIT_POINT;
}
//---------------------------------------------------------------------------
