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

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
// Disable warning for generating native code, since AnimationTool is
// a Win32-specific application.
#pragma warning( disable : 4793 )

// #pragma unmanaged
// Push managed state on to stack and set unmanaged state.
#pragma managed(push, off)

// Include Gamebryo header files.
#include <NiSystem.h>
#include <NiMain.h>
#include <NiAnimation.h>
#include <NiCollision.h>
#include <NiPortal.h>
#include <NiParticle.h>
#include <NiMesh.h>
#include <NiDX9Renderer.h>
#include <NiD3D10Renderer.h>

#include <NiD3DShaderFactory.h>
#include <NiD3DUtility.h>
#include <NiMaterialToolkit.h>
#include <NiMaterialHelpers.h>
#include <NiMeshCullingProcess.h>
#include <NiVersion.h>
#include <NiMesh2DRenderView.h>
#include <NiMemTracker.h>
#include <NiStandardAllocator.h>

#if defined(EE_PHYSX_BUILD)
#include <NiPhysX.h>
#include <NiPhysXParticle.h>
#include <NiPhysXFluid.h>
#endif

// #pragma managed
#pragma managed(pop)

using namespace System;

#include "NiVirtualBoolBugWrapper.h"
#include "ManagedMacros.h"
#include "MLogger.h"
#include "MUtility.h"
