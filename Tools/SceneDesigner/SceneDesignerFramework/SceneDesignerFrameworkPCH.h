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

#pragma once

#pragma unmanaged

#include <NiSystem.h>
#include <NiMemTracker.h>
#include <NiStandardAllocator.h>
#include <NiMain.h>
#include <NiMeshLib.h>
#include <NiAnimation.h>
#include <NiParticle.h>
#include <NiCollision.h>
#include <NiDX9Renderer.h>
#include <NiD3D10Renderer.h>
#include <NiEntity.h>
#include <NiViewMath.h>
#include <NiMaterialToolkit.h>
#include <NiMeshScreenElements.h>
#include <NiMeshCullingProcess.h>

#pragma managed

#include "ManagedMacros.h"

#if !(_MSC_VER < 1400) 
    // Make unmanaged types that are used in public managed classes
    // accessible outside this assembly.
    #pragma make_public(NiAddRemoveComponentCommand)
    #pragma make_public(NiAddRemoveEntityCommand)
    #pragma make_public(NiAddRemovePropertyCommand)
    #pragma make_public(NiAddRemoveSelectionSetCommand)
    #pragma make_public(NiAVObject)
    #pragma make_public(NiBound)
    #pragma make_public(NiCamera)
    #pragma make_public(NiChangeHiddenStateCommand)
    #pragma make_public(NiChangePropertyCommand)
    #pragma make_public(NiEntityCommandInterface)
    #pragma make_public(NiEntityComponentInterface)
    #pragma make_public(NiEntityErrorInterface)
    #pragma make_public(NiEntityInterface)
    #pragma make_public(NiEntityPropertyInterface)
    #pragma make_public(NiEntityRenderingContext)
    #pragma make_public(NiEntitySelectionSet)
    #pragma make_public(NiExternalAssetManager)
    #pragma make_public(NiFrustum)
    #pragma make_public(NiMakePropertyUniqueCommand)
    #pragma make_public(NiMatrix3)
    #pragma make_public(NiPick)
    #pragma make_public(NiPoint2)
    #pragma make_public(NiPoint3)
    #pragma make_public(NiQuaternion)
    #pragma make_public(NiRenameEntityCommand)
    #pragma make_public(NiRenameSelectionSetCommand)
    #pragma make_public(NiScene)
    #pragma make_public(NiSelectEntitiesCommand)
    #pragma make_public(NiUniqueID)
#endif  // !(_MSC_VER < 1400)

using namespace System;
