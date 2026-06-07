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
#include "MUIResetFrustumCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIResetFrustumCommand::MUIResetFrustumCommand()
{
}
//---------------------------------------------------------------------------
String* MUIResetFrustumCommand::GetName()
{
    return "Reset Frustum";
}
//---------------------------------------------------------------------------
bool MUIResetFrustumCommand::Execute(MUIState*)
{
    MCamera* pkMCamera = MFramework::Instance->Renderer->ActiveCamera;
    NIASSERT(pkMCamera);

    NiCamera* pkCamera = pkMCamera->GetCamera();
    if (!pkCamera)
        return false;

    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiAVObject* pkScene = pkSharedData->GetScene(MSharedData::CHARACTER_INDEX);

    if (!pkScene)
        pkScene = pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX);

    if (!pkScene)
    {
        pkSharedData->Unlock();
        return false;
    }

    pkMCamera->CalcCameraFrustum(pkScene);
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIResetFrustumCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIResetFrustumCommand::RefreshData()
{
    DeleteContents();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIResetFrustumCommand::GetEnabled()
{
    return true;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIResetFrustumCommand::GetCommandType()
{
    return MUICommand::RESET_FRUSTUM;
}
//---------------------------------------------------------------------------
