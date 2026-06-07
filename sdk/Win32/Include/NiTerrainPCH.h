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

#ifndef NITERRAINPCH_H
#define NITERRAINPCH_H

#if defined(NI_USE_PCH)

#include <NiMainPCH.h>
#include <NiMeshPCH.h>
#include <NiFloodgatePCH.h>
#include <NiEntity.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
//#elif defined(_PS3)
//#pragma GCC system_header
#endif
#include <tinyxml.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

#endif //#if defined(NI_USE_PCH)

#include "NiTerrainLibType.h"

#endif // NITERRAINPCH_H
