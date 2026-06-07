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

#ifndef TERRAINPLUGINPCH_H
#define TERRAINPLUGINPCH_H

#pragma unmanaged
#include <NiSystem.h>
#include <NiMain.h>
#include <NiDX9Renderer.h>
#include <NiEntity.h>
#include <NiTerrainLib.h>
#include <NiViewMath.h>
#include <NiPick.h>

#pragma managed

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

#include "ManagedMacros.h"
#include "resource.h"

#endif // TERRAINPLUGINPCH_H