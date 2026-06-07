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

#include "DockPositionAttribute.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
DockPositionAttribute::DockPositionAttribute() :
    m_ePosition(DefaultDock::DockTop), m_bAllowFloat(true)
{
}
//---------------------------------------------------------------------------
DefaultDock DockPositionAttribute::get_Position()
{
    return m_ePosition;
}
//---------------------------------------------------------------------------
void DockPositionAttribute::set_Position(DefaultDock ePosition)
{
    m_ePosition = ePosition;
}
//---------------------------------------------------------------------------
bool DockPositionAttribute::get_AllowFloat()
{
    return m_bAllowFloat;
}
//---------------------------------------------------------------------------
void DockPositionAttribute::set_AllowFloat(bool value)
{
    m_bAllowFloat = value;
}
//---------------------------------------------------------------------------
