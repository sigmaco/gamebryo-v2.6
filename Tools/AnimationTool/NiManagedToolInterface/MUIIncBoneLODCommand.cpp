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
#include "MUIIncBoneLODCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIIncBoneLODCommand::MUIIncBoneLODCommand()
{
}
//---------------------------------------------------------------------------
String* MUIIncBoneLODCommand::GetName()
{
    return "Increment Bone LOD";
}
//---------------------------------------------------------------------------
bool MUIIncBoneLODCommand::Execute(MUIState*)
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
        if (uiBoneLOD + 1 < pkBoneLOD->GetNumberOfBoneLODs())
            pkBoneLOD->SetBoneLOD(uiBoneLOD + 1);
    }

    pkSharedData->Unlock();
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIIncBoneLODCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIIncBoneLODCommand::RefreshData()
{
    DeleteContents();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIIncBoneLODCommand::GetEnabled()
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
    unsigned int uiNumBoneLODs = pkBoneLOD->GetNumberOfBoneLODs();

    pkSharedData->Unlock();
    return (uiBoneLOD + 1 < uiNumBoneLODs);
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIIncBoneLODCommand::GetCommandType()
{
    return MUICommand::BONELOD_INCREMENT;
}
//---------------------------------------------------------------------------
