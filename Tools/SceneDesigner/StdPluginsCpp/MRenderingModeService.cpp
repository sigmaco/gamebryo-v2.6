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

#include "MRenderingModeService.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MRenderingModeService::MRenderingModeService()
{
    m_pmModes = new ArrayList();
}
//---------------------------------------------------------------------------
void MRenderingModeService::Do_Dispose(bool bDisposing)
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
String* MRenderingModeService::get_Name()
{
    MVerifyValidInstance;

    return "Rendering Mode Service";
}
//---------------------------------------------------------------------------
bool MRenderingModeService::Initialize()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
bool MRenderingModeService::Start()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
// IRenderingModeService members.
//---------------------------------------------------------------------------
void MRenderingModeService::AddRenderingMode(IRenderingMode* pmMode)
{
    MVerifyValidInstance;

    MAssert(GetRenderingModeByName(pmMode->Name) == NULL, "Rendering mode "
        "with same name already exists!");

    m_pmModes->Add(pmMode);
}
//---------------------------------------------------------------------------
IRenderingMode* MRenderingModeService::GetRenderingModeByName(String* strName)
{
    MVerifyValidInstance;

    for (int i = 0; i < m_pmModes->Count; i++)
    {
        IRenderingMode* pmMode = dynamic_cast<IRenderingMode*>(
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
IRenderingMode* MRenderingModeService::GetRenderingModes()[]
{
    MVerifyValidInstance;

    return dynamic_cast<IRenderingMode*[]>(m_pmModes->ToArray(
        __typeof(IRenderingMode)));
}
//---------------------------------------------------------------------------
