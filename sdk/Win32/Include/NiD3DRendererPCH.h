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

#ifndef NID3DRENDERERPCH_H
#define NID3DRENDERERPCH_H

#if defined(NI_USE_PCH)

#include <NiMainPCH.h>

#if defined(WIN32)
    #include "NiDX9RendererPCH.h"
#elif defined(_XENON)
    #include "NiXenonRendererPCH.h"
#endif

#include "NiD3DDefaultShader.h"
#include "NiD3DDefines.h"
#include "NiD3DError.h"
#include "NiD3DGPUProgramCache.h"
#include "NiD3DMacros.h"
#include "NiD3DPass.h"
#include "NiD3DPixelShader.h"
#include "NiD3DRendererHeaders.h"
#include "NiD3DRenderStateGroup.h"
#include "NiD3DShader.h"
#include "NiD3DShaderConstantMap.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DShaderInterface.h"
#include "NiD3DShaderLibrary.h"
#include "NiD3DShaderLibraryInterface.h"
#include "NiD3DShaderLibraryVersion.h"
#include "NiD3DShaderProgram.h"
#include "NiD3DShaderProgramCreator.h"
#include "NiD3DShaderProgramCreatorAsm.h"
#include "NiD3DShaderProgramCreatorObj.h"
#include "NiD3DShaderProgramFactory.h"
#include "NiD3DTextureStage.h"
#include "NiD3DTextureStageGroup.h"
#include "NiD3DUtility.h"
#include "NiD3DUtils.h"
#include "NiD3DVertexShader.h"

#endif //#if defined(NI_USE_PCH)

#endif // #ifndef NID3DRENDERERPCH_H
