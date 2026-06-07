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
#include "MUIDecBoneLODCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIDecBoneLODCommand::MUIDecBoneLODCommand()
{
}
//---------------------------------------------------------------------------
String* MUIDecBoneLODCommand::GetName()
{
    return "Decrement Bone LOD";
}
//---------------------------------------------------------------------------
bool MUIDecBoneLODCommand::Execute(MUIState*)
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiActorManager* pkActorManager = pkSharedData->GetActorManager();
    if (!pkActorManager)
    {
        pkSharedData->Unlock();
        return false;
    }

    NiSkinningLODController* pkBoneLOD = 
        pkActorManager->GetSkinningLODController();

    if (pkBoneLOD)
    {
        unsigned int uiBoneLOD = pkBoneLOD->GetBoneLOD();
        if (uiBoneLOD > 0)
            pkBoneLOD->SetBoneLOD(uiBoneLOD - 1);
    }

    pkSharedData->Unlock();    
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIDecBoneLODCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIDecBoneLODCommand::RefreshData()
{
    DeleteContents();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIDecBoneLODCommand::GetEnabled()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiActorManager* pkActorManager = pkSharedData->GetActorManager();
    if (!pkActorManager)
    {
        pkSharedData->Unlock();
        return false;
    }

    NiSkinningLODController* pkBoneLOD = 
        pkActorManager->GetSkinningLODController();

    if (!pkBoneLOD)
    {
        pkSharedData->Unlock();
        return false;
    }

    // The toolbar may be refreshed prior to the controller being updated,
    // so use the next LOD level rather than the current LOD level.
    unsigned int uiBoneLOD = pkBoneLOD->GetNextBoneLOD();
    pkSharedData->Unlock();

    return (uiBoneLOD > 0);
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIDecBoneLODCommand::GetCommandType()
{
    return MUICommand::BONELOD_DECREMENT;
}
//---------------------------------------------------------------------------
