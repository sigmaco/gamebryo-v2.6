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

// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MInteractionModeService.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MInteractionModeService::MInteractionModeService()
{
    m_pmModes = new ArrayList();
}
//---------------------------------------------------------------------------
void MInteractionModeService::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        for (int i = 0; i < m_pmModes->Count; i++)
        {
            IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
                m_pmModes->Item[i]);
            if (pmDisposable != NULL)
            {
                pmDisposable->Dispose();
            }
        }
    }
}
//---------------------------------------------------------------------------
// IService members.
//---------------------------------------------------------------------------
String* MInteractionModeService::get_Name()
{
    MVerifyValidInstance;

    return "Interaction Mode Service";
}
//---------------------------------------------------------------------------
bool MInteractionModeService::Initialize()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
bool MInteractionModeService::Start()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
// IInteractionModeService members.
//---------------------------------------------------------------------------
void MInteractionModeService::AddInteractionMode(IInteractionMode* pmMode)
{
    MVerifyValidInstance;

    MAssert(GetInteractionModeByName(pmMode->Name) == NULL, "Interaction "
        "mode with same name already exists!");

    if (pmMode->Initialize())
    {
        m_pmModes->Add(pmMode);
        if (m_pmActiveMode == NULL)
        {
            m_pmActiveMode = pmMode;
        }
    }
    else
    {
        String* strError = "Failed to load interaction mode: ";
        strError = String::Concat(strError, pmMode->Name);
        IMessageService* pmMessageService = MGetService(IMessageService);
        MAssert(pmMessageService != NULL, "Service not found!");
        pmMessageService->AddMessage(MessageChannelType::Errors, strError);
    }
}
//---------------------------------------------------------------------------
IInteractionMode* MInteractionModeService::GetInteractionModeByName(
    String* strName)
{
    MVerifyValidInstance;

    for (int i = 0; i < m_pmModes->Count; i++)
    {
        IInteractionMode* pmMode = dynamic_cast<IInteractionMode*>(
            m_pmModes->Item[i]);
        MAssert(pmMode != NULL);
        if (String::Equals(pmMode->Name, strName))
        {
            return pmMode;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
IInteractionMode* MInteractionModeService::GetInteractionModes()[]
{
    MVerifyValidInstance;

    return dynamic_cast<IInteractionMode*[]>(m_pmModes->ToArray(
        __typeof(IInteractionMode)));
}
//---------------------------------------------------------------------------
IInteractionMode* MInteractionModeService::get_ActiveMode()
{
    MVerifyValidInstance;

    return m_pmActiveMode;
}
//---------------------------------------------------------------------------
void MInteractionModeService::set_ActiveMode(IInteractionMode* pmActiveMode)
{
    MVerifyValidInstance;

    MAssert(GetInteractionModeByName(pmActiveMode->Name) != NULL, "Active "
        "mode is not owned by the manager.");

    m_pmActiveMode = pmActiveMode;
}
//---------------------------------------------------------------------------
void MInteractionModeService::Update(float fTime)
{
    MVerifyValidInstance;

    if (m_pmActiveMode != NULL)
    {
        m_pmActiveMode->Update(fTime);
    }
}
//---------------------------------------------------------------------------
