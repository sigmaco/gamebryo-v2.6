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
#include "SceneDesignerFrameworkPCH.h"

#include "UIState.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
UIState::UIState() : m_bEnabled(true), m_bChecked(false)
{
}
//---------------------------------------------------------------------------
bool UIState::get_Enabled()
{
    return m_bEnabled;
}
//---------------------------------------------------------------------------
void UIState::set_Enabled(bool bEnabled)
{
    m_bEnabled = bEnabled;
}
//---------------------------------------------------------------------------
bool UIState::get_Checked()
{
    return m_bChecked;
}
//---------------------------------------------------------------------------
void UIState::set_Checked(bool bChecked)
{
    m_bChecked = bChecked;
}
//---------------------------------------------------------------------------
String* UIState::get_Text()
{
    return m_strText;
}
//---------------------------------------------------------------------------
void UIState::set_Text(String* strText)
{
    m_strText = strText;
}
//---------------------------------------------------------------------------
