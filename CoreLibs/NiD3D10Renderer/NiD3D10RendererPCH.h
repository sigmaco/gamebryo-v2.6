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

#ifndef NID3D10RENDERERPCH_H
#define NID3D10RENDERERPCH_H

#if defined(NI_USE_PCH)

#include <NiMainPCH.h>

#include "NiCubeMapDepthStencilBuffer.h"
#include "NiD3D102DBufferData.h"
#include "NiD3D10DataStream.h"
#include "NiD3D10DataStreamFactory.h"
#include "NiD3D10DeviceState.h"
#include "NiD3D10Direct3DTexture.h"
#include "NiD3D10Direct3DTextureData.h"
#include "NiD3D10DynamicTextureData.h"
#include "NiD3D10Error.h"
#include "NiD3D10ErrorShader.h"
#include "NiD3D10FragmentShader.h"
#include "NiD3D10GPUProgramCache.h"
#include "NiD3D10GeometryShader.h"
#include "NiD3D10Headers.h"
#include "NiD3D10MeshMaterialBinding.h"
#include "NiD3D10Pass.h"
#include "NiD3D10PersistentSrcTextureRendererData.h"
#include "NiD3D10PixelFormat.h"
#include "NiD3D10PixelShader.h"
#include "NiD3D10RenderStateGroup.h"
#include "NiD3D10RenderStateManager.h"
#include "NiD3D10RenderedTextureData.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10RendererLibType.h"
#include "NiD3D10RendererSDM.h"
#include "NiD3D10ResourceManager.h"
#include "NiD3D10Shader.h"
#include "NiD3D10ShaderConstantManager.h"
#include "NiD3D10ShaderConstantMap.h"
#include "NiD3D10ShaderInterface.h"
#include "NiD3D10ShaderLibrary.h"
#include "NiD3D10ShaderLibraryInterface.h"
#include "NiD3D10ShaderLibraryVersion.h"
#include "NiD3D10ShaderProgram.h"
#include "NiD3D10ShaderProgramCreator.h"
#include "NiD3D10ShaderProgramCreatorHLSL.h"
#include "NiD3D10ShaderProgramFactory.h"
#include "NiD3D10SourceTextureData.h"
#include "NiD3D10SystemDesc.h"
#include "NiD3D10TextureData.h"
#include "NiD3D10Utility.h"
#include "NiD3D10VertexShader.h"

#endif //#if defined(NI_USE_PCH)

#endif // #ifndef NID3D10RENDERERPCH_H
