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

#include "NiD3DRendererPCH.h"
#include "NiDX9RendererMetrics.h"

const char NiDx9RendererMetrics::ms_acNames
    [NiDx9RendererMetrics::NUM_METRICS][NIMETRICS_NAMELENGTH] =
{
    NIMETRICS_DX9RENDERER_PREFIX "DrawnSubmeshCount",
    NIMETRICS_DX9RENDERER_PREFIX "DrawnPrimitives",
    NIMETRICS_DX9RENDERER_PREFIX "DrawnVerts",
    NIMETRICS_DX9RENDERER_PREFIX "RenderStateChanges",
    NIMETRICS_DX9RENDERER_PREFIX "RenderTargetChanges",
    NIMETRICS_DX9RENDERER_PREFIX "TextureChanges",
    NIMETRICS_DX9RENDERER_PREFIX "VertexBufferChanges",
    NIMETRICS_DX9RENDERER_PREFIX "PixelShaderChanges",
    NIMETRICS_DX9RENDERER_PREFIX "VertexShaderChanges",
    NIMETRICS_DX9RENDERER_PREFIX "VertexBufferUsage",
    NIMETRICS_DX9RENDERER_PREFIX "IndexBufferUsage",
    NIMETRICS_DX9RENDERER_PREFIX "drawtime.Mesh"    
};

int NiDx9RendererMetrics::ms_aiPerFrameMetrics[
    NiDx9RendererMetrics::INDEX_BUFFER_SIZE + 1];
float NiDx9RendererMetrics::ms_afPerFrameTimes[
    NiDx9RendererMetrics::NUM_METRICS - NiDx9RendererMetrics::DRAW_TIME_MESH];
