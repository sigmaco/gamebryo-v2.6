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
#include "MUIManager.h"
#include "MSharedData.h"
#include "MUIAdjustBackgroundHeightState.h"
#include "MUICameraOrbitState.h"
#include "MUICameraFreeLookState.h"
#include "MUIZoomExtentsCommand.h"
#include "MUIResetFrustumCommand.h"
#include "MUIResetBackgroundHeightCommand.h"
#include "MUIToggleWireframeCommand.h"
#include "MUIToggleBackgroundVisibilityCommand.h"
#include "MUIToggleFrameRateCommand.h"
#include "MUIIncBoneLODCommand.h"
#include "MUIDecBoneLODCommand.h"
#include "MUIToggleSceneLightsCommand.h"
#include "MUIToggleCollisionABVCommand.h"
#include "MUIToggleTrajectoryCommand.h"
#include "MUIResetActorPositionCommand.h"
#include "MUIResetOrbitPointCommand.h"
#include "MUIToggleOrbitFollowCommand.h"
#include "MFramework.h"
#if defined(EE_PHYSX_BUILD)
#include "PhysX/MUIPhysicsForceState.h"
#endif

using namespace NiManagedToolInterface;
using namespace System::Threading;

//---------------------------------------------------------------------------
MUIManager::MUIManager()
{
    m_pkMCamera = NULL;
    m_pkMOrbitPoint = NULL;
    m_eUpAxis = MCamera::Z_AXIS;
    m_bInvertPan = false;
    m_fBackgroundHeight = 0.0f;
    m_uiLockCount = 0;
    m_eActiveUIType = MUIState::ORBIT;
    m_aUIState = NiExternalNew MUIState*[MUIState::MAX_UI_TYPES];
    MUICameraOrbitState* pkOrbitState = NiExternalNew MUICameraOrbitState; 
    m_aUIState[MUIState::ORBIT] = pkOrbitState;
    m_aUIState[MUIState::FREELOOK] = NiExternalNew MUICameraFreeLookState; 
    m_aUIState[MUIState::BACKGROUNDHEIGHT] = 
        NiExternalNew MUIAdjustBackgroundHeightState;
#if defined(EE_PHYSX_BUILD)
    m_aUIState[MUIState::PHYSICSFORCE] = NiExternalNew MUIPhysicsForceState;
#endif
    m_aCommands = NiExternalNew MUICommand*[MUICommand::MAX_COMMAND_TYPE];
    m_aCommands[MUICommand::ZOOM_EXTENTS] = NiExternalNew 
        MUIZoomExtentsCommand();
    m_aCommands[MUICommand::RESET_FRUSTUM] = NiExternalNew 
        MUIResetFrustumCommand();
    m_aCommands[MUICommand::BONELOD_INCREMENT]= NiExternalNew 
        MUIIncBoneLODCommand();
    m_aCommands[MUICommand::BONELOD_DECREMENT] = NiExternalNew 
        MUIDecBoneLODCommand();
    m_aCommands[MUICommand::TOGGLE_WIREFRAME] = 
        NiExternalNew MUIToggleWireframeCommand();
    m_aCommands[MUICommand::TOGGLE_SCENE_LIGHTS] = 
        NiExternalNew MUIToggleSceneLightsCommand();
    m_aCommands[MUICommand::TOGGLE_BACKGROUND_SCENE_VIS] = NiExternalNew 
        MUIToggleBackgroundVisibilityCommand();
    m_aCommands[MUICommand::TOGGLE_FRAME_RATE] = NiExternalNew 
        MUIToggleFrameRateCommand();
    m_aCommands[MUICommand::RESET_BACKGROUND_HEIGHT] = NiExternalNew 
        MUIResetBackgroundHeightCommand();
    m_aCommands[MUICommand::TOGGLE_COLLISION_ABVS] = NiExternalNew 
        MUIToggleCollisionABVCommand();
    m_aCommands[MUICommand::TOGGLE_TRAJECTORY] = NiExternalNew 
        MUIToggleTrajectoryCommand();
    m_aCommands[MUICommand::RESET_ACTOR_POSITION] = NiExternalNew
        MUIResetActorPositionCommand();
    m_aCommands[MUICommand::RESET_ORBIT_POINT] = NiExternalNew
        MUIResetOrbitPointCommand();
    m_aCommands[MUICommand::TOGGLE_ORBIT_FOLLOW] = NiExternalNew
        MUIToggleOrbitFollowCommand();
}
//---------------------------------------------------------------------------
void MUIManager::DeleteContents()
{
    for (unsigned int ui = 0; ui < MUIState::MAX_UI_TYPES; ui++)
    {
        if (m_aUIState[ui] != NULL)
            m_aUIState[ui]->DeleteContents();
    }

    for (unsigned int ui = 0; ui < MUICommand::MAX_COMMAND_TYPE; ui++)
    {
        if (m_aCommands[ui] != NULL)
            m_aCommands[ui]->DeleteContents();
    }

    if (m_pkMOrbitPoint != NULL)
    {
        m_pkMOrbitPoint->DeleteContents();
        m_pkMOrbitPoint = NULL;
    }

    m_pkMCamera = NULL;

}
//---------------------------------------------------------------------------
void MUIManager::Lock()
{
    if (m_uiLockCount == 0)
    {
        Monitor::Enter(this);
    }
    m_uiLockCount++;
}
//---------------------------------------------------------------------------
void MUIManager::Unlock()
{
    m_uiLockCount--;
    if (m_uiLockCount == 0)
    {
        Monitor::Exit(this);
    }
}
//---------------------------------------------------------------------------
void MUIManager::Shutdown()
{
    Lock();
    DeleteContents();
    Unlock();
}
//---------------------------------------------------------------------------
void MUIManager::Update()
{
    Lock();
    if (m_aUIState[m_eActiveUIType])
        m_aUIState[m_eActiveUIType]->Update();

    float fTime = MUtility::GetCurrentTimeInSec();
    for (unsigned int ui = 0; ui < MUICommand::MAX_COMMAND_TYPE; ui++)
    {
        if (m_aCommands[ui] != NULL)
            m_aCommands[ui]->Update(fTime);
    }

    Unlock();
}
//---------------------------------------------------------------------------
#pragma warning (disable:4312)
void MUIManager::InitWindowHandle(IntPtr pWindowHandle)
{
    Lock();
    HWND hWnd = reinterpret_cast<HWND> (pWindowHandle.ToInt32());
    for (unsigned int ui = 0; ui < MUIState::MAX_UI_TYPES; ui++)
    {
        if (m_aUIState[ui] != NULL)
        {
            m_aUIState[ui]->SetWindowHandle(hWnd);
        }
    }

    Unlock();
}
#pragma warning (default:4312)
//---------------------------------------------------------------------------
void MUIManager::Init()
{
    Lock();
    m_pkMOrbitPoint = NiExternalNew MOrbitPoint();
    MUIResetOrbitPointCommand * pkResetOrbitCommand = 
        dynamic_cast<MUIResetOrbitPointCommand*>(
        m_aCommands[MUICommand::RESET_ORBIT_POINT]);

    pkResetOrbitCommand->SetOrbitPoint(m_pkMOrbitPoint);

    MUIToggleOrbitFollowCommand * pkToggleOrbitFollowCommand = 
        dynamic_cast<MUIToggleOrbitFollowCommand*>(
        m_aCommands[MUICommand::TOGGLE_ORBIT_FOLLOW]);

    pkToggleOrbitFollowCommand->SetOrbitPoint(m_pkMOrbitPoint);

    MUIZoomExtentsCommand * pkZoomExtentsCommand = 
        dynamic_cast<MUIZoomExtentsCommand*>(
        m_aCommands[MUICommand::ZOOM_EXTENTS]);

    pkZoomExtentsCommand->SetOrbitPoint(m_pkMOrbitPoint);

    for (unsigned int ui = 0; ui < MUIState::MAX_UI_TYPES; ui++)
    {
        if (m_aUIState[ui] != NULL)
            m_aUIState[ui]->SetOrbitPoint(m_pkMOrbitPoint);
    }
    Unlock();
}
//---------------------------------------------------------------------------
void MUIManager::SetCamera(MCamera* pkMCamera)
{
    Lock();
    m_pkMCamera = pkMCamera;
    for (unsigned int ui = 0; ui < MUIState::MAX_UI_TYPES; ui++)
    {
        if (m_aUIState[ui] != NULL)
        {
            m_aUIState[ui]->SetCamera(pkMCamera);
        }
    }
    Unlock();
}
//---------------------------------------------------------------------------
void MUIManager::SubmitCommand(MUICommand::CommandType eType)
{
    Lock();
    if (m_aCommands[eType])
    {
        String* strCommand = String::Concat("Executing Command: ",
            m_aCommands[eType]->GetName());
        MLogger::LogEvent(strCommand);
        
        //bool bCommandExecuted = 
            m_aCommands[eType]->Execute(
            m_aUIState[m_eActiveUIType]);
        //NIASSERT(bCommandExecuted);
    }
    Unlock();
}
//---------------------------------------------------------------------------
MUICommand* MUIManager::GetCommand(MUICommand::CommandType eType)
{
    return m_aCommands[eType];
}
//---------------------------------------------------------------------------
void MUIManager::RefreshData()
{
    Lock();
    for (unsigned int ui = 0; ui < MUIState::MAX_UI_TYPES; ui++)
    {
        if (m_aUIState[ui] != NULL)
            m_aUIState[ui]->RefreshData();
    }

    for (unsigned int ui = 0; ui < MUICommand::MAX_COMMAND_TYPE; ui++)
    {
        if (m_aCommands[ui] != NULL)
            m_aCommands[ui]->RefreshData();
    }
    m_aUIState[m_eActiveUIType]->Deactivate();
    m_pkMCamera->Reset();
    if (m_pkMOrbitPoint)
        m_pkMOrbitPoint->ResetOrbitPoint();
    m_aUIState[m_eActiveUIType]->Activate(true);
    Unlock();
}

//---------------------------------------------------------------------------
MOrbitPoint* MUIManager::get_OrbitPoint()
{
    return m_pkMOrbitPoint;
}
//---------------------------------------------------------------------------
MUIState* MUIManager::get_ActiveUIState()
{
    return m_aUIState[m_eActiveUIType];
}
//---------------------------------------------------------------------------
MUIState::UIType MUIManager::get_ActiveUIType()
{
    return m_eActiveUIType;
}
//---------------------------------------------------------------------------
void MUIManager::set_ActiveUIType(MUIState::UIType eActiveType)
{
    if (eActiveType != m_eActiveUIType)
    {
        m_aUIState[m_eActiveUIType]->Deactivate();
        m_eActiveUIType = eActiveType;
        m_aUIState[m_eActiveUIType]->Activate(false);
        OnActiveUITypeChanged(m_eActiveUIType);
    }
}
//---------------------------------------------------------------------------
bool MUIManager::get_InvertPan()
{
    return m_bInvertPan;
}
//---------------------------------------------------------------------------
void MUIManager::set_InvertPan(bool bInvert)
{
    NIASSERT(m_pkMCamera != NULL);
    if (bInvert != m_bInvertPan)
    {
        m_bInvertPan = bInvert;
        m_pkMCamera->InvertPan = bInvert;
    }
}
//---------------------------------------------------------------------------
MCamera::UpAxisType MUIManager::get_UpAxis()
{
    return m_eUpAxis;
}
//---------------------------------------------------------------------------
void MUIManager::set_UpAxis(MCamera::UpAxisType eType)
{
    NIASSERT(m_pkMCamera != NULL);
    if (eType != m_eUpAxis)
    {
        m_eUpAxis = eType;
        m_pkMCamera->UpAxis = eType;
        m_aUIState[m_eActiveUIType]->Deactivate();
        m_pkMCamera->Reset();
        m_aUIState[m_eActiveUIType]->Activate(true);
    }
    
}
//---------------------------------------------------------------------------
float MUIManager::get_BackgroundHeight()
{
    return m_fBackgroundHeight;
}
//---------------------------------------------------------------------------
void MUIManager::set_BackgroundHeight(float fHeight)
{
    m_fBackgroundHeight = fHeight;
}
//---------------------------------------------------------------------------
