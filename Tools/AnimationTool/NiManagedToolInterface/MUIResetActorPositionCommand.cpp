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
#include "MUIResetActorPositionCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
#include "MUIState.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIResetActorPositionCommand::MUIResetActorPositionCommand()
{

}
//---------------------------------------------------------------------------
String* MUIResetActorPositionCommand::GetName()
{
    return "Reset Actor Position";
}
//---------------------------------------------------------------------------
bool MUIResetActorPositionCommand::Execute(MUIState*)
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiActorManager* pkActor = pkSharedData->GetActorManager();
    if (!pkActor)
    {
        pkSharedData->Unlock();
        return false;
    }

    if (!pkActor->GetControllerManager())
    {
        pkSharedData->Unlock();
        return false;
    }
    
    pkActor->GetControllerManager()->ClearCumulativeAnimations();

    pkSharedData->Unlock();
    OnCommandExecuted(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIResetActorPositionCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIResetActorPositionCommand::RefreshData()
{
    DeleteContents();
}
//---------------------------------------------------------------------------
bool MUIResetActorPositionCommand::GetEnabled()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiActorManager* pkActor = pkSharedData->GetActorManager();
    if (!pkActor)
    {
        pkSharedData->Unlock();
        return false;
    }

    if (!pkActor->GetControllerManager())
    {
        pkSharedData->Unlock();
        return false;
    }
    
    pkSharedData->Unlock();
    return true;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIResetActorPositionCommand::GetCommandType()
{
    return MUICommand::RESET_ACTOR_POSITION;
}
//---------------------------------------------------------------------------
