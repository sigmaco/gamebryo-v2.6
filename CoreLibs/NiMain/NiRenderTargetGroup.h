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

#ifndef NIRENDERTARGETGROUP_H
#define NIRENDERTARGETGROUP_H

#include "NiDepthStencilBuffer.h"
#include "NiRenderedCubeMap.h"

NiSmartPointer(NiRenderTargetGroup);

class NIMAIN_ENTRY NiRenderTargetGroup : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;
public:
    // Create an NiRenderTargetGroup with a fixed number of buffers.
    static NiRenderTargetGroup* Create(unsigned int uiNumBuffers, 
        NiRenderer* pkRenderer);
    // Create an NiRenderTargetGroup from an Ni2DBuffer and optionally
    // use the default depth/stencil buffer or create a new depth/stencil
    // buffer compatible with the primary 2D buffer (if possible)
    static NiRenderTargetGroup* Create(Ni2DBuffer* pkBuffer, 
        NiRenderer* pkRenderer, bool bUseDefaultDepthStencil, 
        bool bCreateCompatibleDepthStencil = false);
    // Create an NiRenderTargetGroup from an Ni2DBuffer and the
    // specified NiDepthStencilBuffer.
    static NiRenderTargetGroup* Create(Ni2DBuffer* pkBuffer, 
        NiRenderer* pkRenderer, NiDepthStencilBuffer* pkDSBuffer);

    virtual ~NiRenderTargetGroup();

    enum
    {
#if defined(WIN32)
        MAX_RENDER_BUFFERS  = 8
#else //#if defined(WIN32)
        MAX_RENDER_BUFFERS  = 4
#endif//#if defined(WIN32)
    };

    virtual unsigned int GetWidth(unsigned int uiIndex) const;
    virtual unsigned int GetHeight(unsigned int uiIndex) const;

    virtual unsigned int GetDepthStencilWidth() const;
    virtual unsigned int GetDepthStencilHeight() const;

    bool HasDepthStencil() const;

    bool IsValid() const;

    bool CheckMSAAPrefConsistency() const;

    virtual const NiPixelFormat* GetPixelFormat(unsigned int uiIndex) const;
    virtual const NiPixelFormat* GetDepthStencilPixelFormat() const;

    virtual unsigned int GetBufferCount() const;
    
    virtual bool AttachBuffer(Ni2DBuffer* pkBuffer, unsigned int uiIndex);
    virtual bool AttachDepthStencilBuffer(NiDepthStencilBuffer* 
        pkDepthBuffer); 

    virtual Ni2DBuffer* GetBuffer(unsigned int uiIndex) const;
    virtual NiDepthStencilBuffer* GetDepthStencilBuffer() const;

    class NIMAIN_ENTRY CubeMapSetupHelper : public NiMemObject
    {
    public:
        CubeMapSetupHelper(NiRenderedCubeMap* pkCubeMap);

        NiRenderTargetGroupPtr 
            m_aspRenderTargetGroups[NiRenderedCubeMap::FACE_NUM];
    protected:
        void InitializeFromCubeMap(NiRenderedCubeMap* pkCubeMap);
    };

    /*** begin Emergent internal use only ***/
    // Renderer specific data class
    class NIMAIN_ENTRY RendererData : public NiMemObject
    {
    public:
        inline virtual ~RendererData() {};
    };

    virtual const RendererData* GetRendererData() const;
    virtual void SetRendererData(RendererData* pkRendererData);

    virtual Ni2DBuffer::RendererData* GetBufferRendererData(
        unsigned int uiIndex) const;
    virtual Ni2DBuffer::RendererData* GetDepthStencilBufferRendererData() 
        const;
    /*** end Emergent internal use only ***/

protected:
    NiRenderTargetGroup();
    Ni2DBufferPtr m_aspBuffers[MAX_RENDER_BUFFERS];
    unsigned int m_uiNumBuffers;
    NiDepthStencilBufferPtr m_spDepthStencilBuffer;
    RendererData* m_pkRendererData;
};

#endif

