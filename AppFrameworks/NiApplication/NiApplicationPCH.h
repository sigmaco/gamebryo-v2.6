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

#ifndef NIAPPLICATIONPCH_H
#define NIAPPLICATIONPCH_H

#if defined(NI_USE_PCH)

#include <NiMainPCH.h>
#include <NiInput.h>

#include "NiAppWindow.h"
#include "NiApplication.h"
#include "NiApplicationMetrics.h"
#include "NiCommand.h"
#include "NiFrameRate.h"
#include "NiTurret.h"

#if defined(WIN32)
    #include "NiSettingsDialog.h"
    #include "NiRendererSettings.h"
#endif //#if defined(WIN32)

#if defined(_PS3)
    #include <NiPS3Renderer.h>
#endif //#if defined(_PS3)

#endif //#if defined(NI_USE_PCH)

#endif // #ifndef NIAPPLICATIONPCH_H
