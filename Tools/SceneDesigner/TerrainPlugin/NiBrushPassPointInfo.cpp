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

#include "TerrainPluginPCH.h"

#include "NiBrushPassPointInfo.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

bool Emergent::Gamebryo::SceneDesigner::TerrainPlugin::operator!=(
    const NiBrushPassPointInfo::FalloffData& s1, 
    const NiBrushPassPointInfo::FalloffData& s2
    )
{
    return s1.m_pkActual != s2.m_pkActual;
}

//---------------------------------------------------------------------------

bool Emergent::Gamebryo::SceneDesigner::TerrainPlugin::operator==(
    const NiBrushPassPointInfo::FalloffData& s1, 
    const NiBrushPassPointInfo::FalloffData& s2
    )
{
    return s1.m_pkActual == s2.m_pkActual;
}
