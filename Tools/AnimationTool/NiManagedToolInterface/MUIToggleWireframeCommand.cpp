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
#include "MUIToggleWireframeCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIToggleWireframeCommand::MUIToggleWireframeCommand()
{
    m_bOn = false;
    m_pkWireframe = NULL;
}
//---------------------------------------------------------------------------
String* MUIToggleWireframeCommand::GetName()
{
    return "Toggle Wireframe";
}
//---------------------------------------------------------------------------
bool MUIToggleWireframeCommand::Execute(MUIState*)
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    if (m_pkWireframe)
    {
        m_bOn = !m_bOn;
        m_pkWireframe->SetWireframe(m_bOn);
    }

    pkSharedData->Unlock();
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIToggleWireframeCommand::DeleteContents()
{
    if (m_pkWireframe)
    {
        MSharedData* pkSharedData = MSharedData::Instance;
        pkSharedData->Lock();

        NiAVObject* pkScene =
            pkSharedData->GetScene(MSharedData::CHARACTER_INDEX);
        if (pkScene)
        {
            pkScene->DetachProperty(m_pkWireframe);
            pkScene->UpdateProperties();
        }

        NiAVObject* pkBackground =
            pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX);

        if (pkBackground)
        {
            pkBackground->DetachProperty(m_pkWireframe);
            pkBackground->UpdateProperties();
        }

        
        pkSharedData->Unlock();

        m_pkWireframe->DecRefCount();
        m_pkWireframe = NULL;
    }
}
//---------------------------------------------------------------------------
void MUIToggleWireframeCommand::RefreshData()
{
    DeleteContents();
    m_pkWireframe = NiNew NiWireframeProperty();
    m_pkWireframe->IncRefCount();
    m_pkWireframe->SetWireframe(m_bOn);

    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiAVObject* pkScene = pkSharedData->GetScene(MSharedData::CHARACTER_INDEX);
    if (pkScene)
    {
        pkScene->AttachProperty(m_pkWireframe);
        pkScene->UpdateProperties();
    }

    NiAVObject* pkBackground =
        pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX);
    if (pkBackground)
    {
        pkBackground->AttachProperty(m_pkWireframe);
        pkBackground->UpdateProperties();
    }

    pkSharedData->Unlock();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIToggleWireframeCommand::GetActive()
{
    return m_bOn;
}
//---------------------------------------------------------------------------
bool MUIToggleWireframeCommand::GetEnabled()
{
    return true;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIToggleWireframeCommand::GetCommandType()
{
    return MUICommand::TOGGLE_WIREFRAME;
}
//---------------------------------------------------------------------------
