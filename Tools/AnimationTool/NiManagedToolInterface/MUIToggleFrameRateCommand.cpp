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
#include "MUIToggleFrameRateCommand.h"
#include "MFramework.h"
#include "MRenderer.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIToggleFrameRateCommand::MUIToggleFrameRateCommand()
{
}
//---------------------------------------------------------------------------
String* MUIToggleFrameRateCommand::GetName()
{
    return "Toggle Frame Rate";
}
//---------------------------------------------------------------------------
bool MUIToggleFrameRateCommand::Execute(MUIState*)
{
    MRenderer* pkRenderer = MFramework::Instance->Renderer;
    pkRenderer->ShowFrameRate = !pkRenderer->ShowFrameRate;
    pkRenderer->ShowLODStats = !pkRenderer->ShowLODStats;
    pkRenderer->ShowActiveMode = !pkRenderer->ShowActiveMode;
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIToggleFrameRateCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIToggleFrameRateCommand::RefreshData()
{
    DeleteContents();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIToggleFrameRateCommand::IsToggle()
{
    return true;
}
//---------------------------------------------------------------------------
bool MUIToggleFrameRateCommand::GetActive()
{
    MRenderer* pkRenderer = MFramework::Instance->Renderer;
    return pkRenderer->ShowFrameRate;
}
//---------------------------------------------------------------------------
bool MUIToggleFrameRateCommand::GetEnabled()
{
    return true;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIToggleFrameRateCommand::GetCommandType()
{
    return MUICommand::TOGGLE_FRAME_RATE;
}
//---------------------------------------------------------------------------
