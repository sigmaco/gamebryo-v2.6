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

#ifndef NIDX9RENDERERMETRICS_H
#define NIDX9RENDERERMETRICS_H

#include "NiDX9RendererLibType.h"
#include <NiMetricsLayer.h>
#include <NiMetricsTimer.h>

#define NIMETRICS_DX9RENDERER_PREFIX NIMETRICS_PREFIX "dx9."

class NIDX9RENDERER_ENTRY NiDx9RendererMetrics
{
public:
    enum NiMetric
    {
        // ---- CUMULATIVE METRICS POLLED ONCE PER FRAME ----
        // This ordering is important to the indexing of the two static
        // member arrays.  Be careful about modifying it.

        // Number of meshes drawn...
        DRAW_SUBMESH_COUNT,

        // Per frame, number of primitives and verts that were drawn
        DRAW_PRIMITIVES,
        DRAW_VERTS,

        // Calls to SetRenderState
        RENDER_STATE_CHANGES,

        // Calls to SetRenderTarget
        RENDER_TARGET_CHANGES,

        // Calls to SetTexture
        TEXTURE_CHANGES,

        // Calls to SetStreamSource
        VERTEX_BUFFER_CHANGES,

        // Calls to SetPixelShader
        PIXEL_SHADER_CHANGES,

        // Calls to SetVertexShader
        VERTEX_SHADER_CHANGES,

        // Number of vertices in vertex buffers.  Updated after
        // CreateVertexBuffer calls.
        VERTEX_BUFFER_SIZE,

        // Number of indices in index buffers.  Updated after
        // CreateIndexBuffer calls.
        INDEX_BUFFER_SIZE,

        // Do_Render* time in seconds
        DRAW_TIME_MESH,

        NUM_METRICS
    };

    static void RecordMetrics();
    static void IncrementMetric(NiMetric eMetric, int iInc = 1);
    static void RecordTime(NiMetric eMetric, float fTime);

    const static char ms_acNames[NUM_METRICS][NIMETRICS_NAMELENGTH];

protected:
    // For performance reasons, some values are tracked and recorded per
    // frame.
    static int ms_aiPerFrameMetrics[INDEX_BUFFER_SIZE + 1];
    static float ms_afPerFrameTimes[NUM_METRICS - DRAW_TIME_MESH];
};

// Utility class.  Instead of sending time to the metrics system,
// send elapsed time to NiDx9RendererMetrics to aggregate internally
// per frame.
class NIDX9RENDERER_ENTRY NiDx9ScopeTimer : public NiPerformanceCounterTimer
{
public:
    NiDx9ScopeTimer(NiDx9RendererMetrics::NiMetric eMetric);
    ~NiDx9ScopeTimer();
protected:
    NiDx9RendererMetrics::NiMetric m_eMetric;
};

#if NIMETRICS
    #define NIMETRICS_DX9RENDERER_ADDVALUE(eMetric, fValue) \
        NIMETRICS_ADDVALUE(NiDx9RendererMetrics::ms_acNames[ \
            NiDx9RendererMetrics::eMetric], fValue);
    #define NIMETRICS_DX9RENDERER_SCOPETIMER(eMetric) \
        NiDx9ScopeTimer kMetricsTimer(NiDx9RendererMetrics::eMetric);
    #define NIMETRICS_DX9RENDERER_RECORDMETRICS() \
        NiDx9RendererMetrics::RecordMetrics();
    #define NIMETRICS_DX9RENDERER_AGGREGATEVALUE(eMetric, iSize) \
        NiDx9RendererMetrics::IncrementMetric(NiDx9RendererMetrics::eMetric, \
        (int)iSize);
#else //#if NIMETRICS
    #define NIMETRICS_DX9RENDERER_ADDVALUE(eMetric, fValue)
    #define NIMETRICS_DX9RENDERER_SCOPETIMER(eMetric)
    #define NIMETRICS_DX9RENDERER_RECORDMETRICS()
    #define NIMETRICS_DX9RENDERER_AGGREGATEVALUE(eMetric, iSize)
#endif //#if NIMETRICS

#include "NiDX9RendererMetrics.inl"

#endif //#ifndef NIDX9RENDERERMETRICS_H
