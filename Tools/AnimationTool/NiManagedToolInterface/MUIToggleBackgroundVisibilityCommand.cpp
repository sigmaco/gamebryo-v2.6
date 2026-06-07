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
#include "MUIToggleBackgroundVisibilityCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIToggleBackgroundVisibilityCommand::MUIToggleBackgroundVisibilityCommand()
{
    m_bOn = true;
}
//---------------------------------------------------------------------------
String* MUIToggleBackgroundVisibilityCommand::GetName()
{
    return "Toggle Background Visibility";
}
//---------------------------------------------------------------------------
bool MUIToggleBackgroundVisibilityCommand::Execute(MUIState*)
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiAVObject* pkBackground =
        pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX);
    if (pkBackground)
    {
        m_bOn = !m_bOn;
        pkBackground->SetAppCulled(!m_bOn);
    }

    pkSharedData->Unlock();
    
#if defined(EE_PHYSX_BUILD)
    if (m_bOn)
    {
        MFramework::Instance->Physics->AddGroundPlane();
    }
    else
    {
        MFramework::Instance->Physics->RemoveGroundPlane();
    }
#endif
    
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIToggleBackgroundVisibilityCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIToggleBackgroundVisibilityCommand::RefreshData()
{
    DeleteContents();
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiAVObject* pkBackground =
        pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX);
    
    if (pkBackground)
    {
        pkBackground->SetAppCulled(!m_bOn);
    }
    pkSharedData->Unlock();

#if defined(EE_PHYSX_BUILD)
    if (m_bOn)
    {
        MFramework::Instance->Physics->AddGroundPlane();
    }
    else
    {
        MFramework::Instance->Physics->RemoveGroundPlane();
    }
#endif

    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIToggleBackgroundVisibilityCommand::GetActive()
{
    return m_bOn;
}
//---------------------------------------------------------------------------
bool MUIToggleBackgroundVisibilityCommand::GetEnabled()
{
    return true;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIToggleBackgroundVisibilityCommand::GetCommandType()
{
    return MUICommand::TOGGLE_BACKGROUND_SCENE_VIS;
}
//---------------------------------------------------------------------------
