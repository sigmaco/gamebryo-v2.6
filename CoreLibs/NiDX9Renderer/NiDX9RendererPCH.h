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

#ifndef NIDX9RENDERERPCH_H
#define NIDX9RENDERERPCH_H

#if defined(NI_USE_PCH)

#if !defined(WIN32)
#error NiDX9RendererPCH.h should only be included in Win32 builds!
#endif  //#if !defined(WIN32)

#include "NiDX92DBufferData.h"
#include "NiDX9Defines.h"
#include "NiDX9RendererHeaders.h"
#include "NiDX9DynamicTextureData.h"
#include "NiDX9Error.h"
#include "NiDX9FragmentShader.h"
#include "NiDX9Headers.h"
#include "NiDX9LightManager.h"
#include "NiDX9MaterialManager.h"
#include "NiDX9PersistentSrcTextureRendererData.h"
#include "NiDX9PixelFormat.h"
#include "NiDX9RenderedCubeMapData.h"
#include "NiDX9RenderedTextureData.h"
#include "NiDX9Renderer.h"
#include "NiDX9RendererLibType.h"
#include "NiDX9RendererMetrics.h"
#include "NiDX9RendererSDM.h"
#include "NiDX9RenderState.h"
#include "NiDX9ShaderConstantManager.h"
#include "NiDX9SourceCubeMapData.h"
#include "NiDX9SourceTextureData.h"
#include "NiDX9StringMacros.h"
#include "NiDX9SystemDesc.h"
#include "NiDX9TextureData.h"
#include "NiDX9TextureManager.h"

#include "NiD3DHLSLPixelShader.h"
#include "NiD3DHLSLVertexShader.h"
#include "NiD3DShaderProgramCreatorHLSL.h"

#endif //#if defined(NI_USE_PCH)

#endif // #ifndef NIDX9RENDERERPCH_H
