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
#include "MUICommand.h"


using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUICommand::MUICommand()
{
}
//---------------------------------------------------------------------------
String* MUICommand::GetName()
{
    return NULL;
}
//---------------------------------------------------------------------------
bool MUICommand::Execute(MUIState*)
{
    return false;
}
//---------------------------------------------------------------------------
void MUICommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUICommand::RefreshData()
{
    DeleteContents();
}
//---------------------------------------------------------------------------
bool MUICommand::IsToggle()
{
    return false;
}
//---------------------------------------------------------------------------
bool MUICommand::GetEnabled()
{
    return false;
}
//---------------------------------------------------------------------------
bool MUICommand::GetActive()
{
    return true;
}
//---------------------------------------------------------------------------
void MUICommand::Update(float)
{
}
//---------------------------------------------------------------------------
