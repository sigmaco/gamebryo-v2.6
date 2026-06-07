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

#ifndef NID3DXEFFECTSHADERLIBPCH_H
#define NID3DXEFFECTSHADERLIBPCH_H

#if defined(NI_USE_PCH)

#include <NiMainPCH.h>

// Manually include DX-agnostic D3D headers
#include "NiD3DDefines.h"
#include "NiD3DError.h"
#include "NiGlobalConstantEntry.h"
#include "NiD3DMacros.h"
#include "NiD3DPass.h"
#include "NiD3DPixelShader.h"
#include "NiD3DRenderStateGroup.h"
#include "NiD3DRendererHeaders.h"
#include "NiD3DShader.h"
#include "NiD3DShaderConstantMap.h"
#include "NiShaderConstantMapEntry.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DShaderInterface.h"
#include "NiD3DShaderLibrary.h"
#include "NiD3DShaderLibraryInterface.h"
#include "NiD3DShaderLibraryVersion.h"
#include "NiD3DShaderProgram.h"
#include "NiD3DShaderProgramFactory.h"
#include "NiD3DTextureStage.h"
#include "NiD3DTextureStageGroup.h"
#include "NiD3DUtility.h"
#include "NiD3DUtils.h"
#include "NiD3DVertexShader.h"
#include "NiMemoryDefines.h"
#include "NiTPool.h"

#include "NiD3DXEffectShaderLibType.h"
#include "NiD3DXEffectAttributeTable.h"
#include "NiD3DXEffectFactory.h"
#include "NiD3DXEffectFile.h"
#include "NiD3DXEffectHeaders.h"
#include "NiD3DXEffectLoader.h"
#include "NiD3DXEffectParameter.h"
#include "NiD3DXEffectParameterDesc.h"
#include "NiD3DXEffectParser.h"
#include "NiD3DXEffectShader.h"
#include "NiD3DXEffectShaderLib.h"
#include "NiD3DXEffectShaderLibrary.h"
#include "NiD3DXEffectShaderSDM.h"
#include "NiD3DXEffectStateManager.h"
#include "NiD3DXEffectTechnique.h"
#include "NiD3DXEffectTechniqueDesc.h"
#include "NiD3DXEffectTechniqueSet.h"
#include "NiD3DXInclude.h"

#endif //#if defined(NI_USE_PCH)

#endif // #ifndef NID3DXEFFECTSHADERLIBPCH_H
