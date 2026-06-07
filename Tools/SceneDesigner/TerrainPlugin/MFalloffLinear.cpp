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

#include "MFalloffLinear.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

// fDist: the reverse normalised distance from the inner radius to the outer
// radius, where 0 is at the outer radius and 1 is at the inner radius
inline float MFalloffLinear::CalcFalloff(float fDist)
{
    if (fDist > 1)
        return 1.0f;
    else if (fDist < 0)
        return 0.0f;
    else
        return fDist;
}