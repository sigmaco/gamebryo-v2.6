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

#include "SettingChangedEventArgs.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
SettingChangedEventArgs::SettingChangedEventArgs(String* strName,
    SettingsCategory eCategory) : m_strName(strName), m_eCategory(eCategory)
{
}
//---------------------------------------------------------------------------
String* SettingChangedEventArgs::get_Name()
{
    return m_strName;
}
//---------------------------------------------------------------------------
SettingsCategory SettingChangedEventArgs::get_Category()
{
    return m_eCategory;
}
//---------------------------------------------------------------------------
