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
#include "NiSceneEventHandler.h"

//---------------------------------------------------------------------------
NiSceneEventHandler::NiSceneEventHandler()
{ 
    m_bUseKeyboard = false;
    m_bUseMouse = false;
    m_pkViewer = NULL;
    m_pkUIMap = NULL;
    m_pkKeyboard = NULL;
    m_pkMouse = NULL;
}
//---------------------------------------------------------------------------
NiSceneEventHandler::~NiSceneEventHandler()
{ 
    m_pkViewer = NULL;
    m_pkKeyboard = NULL;
    m_pkMouse = NULL;
    m_pkUIMap = NULL;

}
//---------------------------------------------------------------------------
void NiSceneEventHandler::SetSceneViewer(NiSceneViewer* pkViewer)
{
    m_pkViewer = pkViewer;
}
//---------------------------------------------------------------------------
NiSceneViewer* NiSceneEventHandler::GetSceneViewer()
{
    return m_pkViewer;
}
//---------------------------------------------------------------------------
void NiSceneEventHandler::InvalidateUI()
{
    NiDelete m_pkUIMap;
}
//---------------------------------------------------------------------------
bool NiSceneEventHandler::IsValid()
{
    return HasValidUIMap();
}
//---------------------------------------------------------------------------
bool NiSceneEventHandler::HasValidUIMap()
{ 
    if (m_pkUIMap != NULL)
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
void NiSceneEventHandler::SetupUI(NiUIMap* pkMap)
{ 
    NIASSERT(pkMap != NULL);

    m_pkUIMap = pkMap;
    if (m_bUseKeyboard)
        m_pkUIMap->SetKeyboard(m_pkKeyboard);
    if (m_bUseMouse)
        m_pkUIMap->SetMouse(m_pkMouse);

    m_pkUIMap->Initialize();
}
//---------------------------------------------------------------------------
NiUIMap* NiSceneEventHandler::GetUI()
{ 
    return m_pkUIMap;
}
//---------------------------------------------------------------------------
void NiSceneEventHandler::SetKeyboard(NiSceneKeyboard* pkKeyboard)
{
    if (pkKeyboard == NULL)
        return;
    m_pkKeyboard = pkKeyboard;
    m_bUseKeyboard = true;
    if (HasValidUIMap())
        m_pkUIMap->SetKeyboard(pkKeyboard);
    m_lLastKeyboardModifiers = 0;
}
//---------------------------------------------------------------------------
void NiSceneEventHandler::SetMouse(NiSceneMouse* pkMouse)
{
    if (pkMouse == NULL)
        return;
    //return;
    m_pkMouse = pkMouse;
    m_bUseMouse = true;
    if (HasValidUIMap())
        m_pkUIMap->SetMouse(pkMouse);
    m_lLastMouseModifiers = 0;
}
//---------------------------------------------------------------------------
void NiSceneEventHandler::GenerateIdleCommands()
{
    if (HasValidUIMap() && m_pkViewer)
    {
        if (m_bUseKeyboard)
            m_pkViewer->SubmitCommands(m_pkUIMap->CreateKeyboardCommands());

        if (m_bUseMouse)
            m_pkViewer->SubmitCommands(m_pkUIMap->CreateMouseCommands());
        
        m_pkViewer->SubmitCommands(m_pkUIMap->CreateExternalCommands());
    }
}
//---------------------------------------------------------------------------
bool NiSceneEventHandler::IsKeyboardActive()
{
    return m_bUseKeyboard;
}
//---------------------------------------------------------------------------
bool NiSceneEventHandler::IsMouseActive()
{
    return m_bUseMouse;
}
//---------------------------------------------------------------------------
