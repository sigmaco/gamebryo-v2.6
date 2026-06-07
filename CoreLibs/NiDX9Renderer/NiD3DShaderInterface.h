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

#ifndef NID3DSHADERINTERFACE_H
#define NID3DSHADERINTERFACE_H

#include <NiShader.h>
#include <NiRenderObject.h>
#include <NiSmartPointer.h>

#include "NiD3DDefines.h"
#include "NiD3DError.h"

class NiDX9RenderState;
NiSmartPointer(NiDX9RenderState);

// NiD3DShaderInterface
// Shader derived for D3D-based renderers. (DX9/Xenon)

class NID3D_ENTRY NiD3DShaderInterface : public NiShader
{
    NiDeclareRTTI;

    // *** begin Emergent internal use only ***
protected:
    void SetD3DDevice(D3DDevicePtr pkD3DDevice);
    inline void SetD3DRenderState(NiDX9RenderState* pkRS);
public:
    void SetD3DRenderer(NiD3DRenderer* pkD3DRenderer);
    
    // Called by the renderer upon creation.
    // This can not go into the SDM initialization scheme, as it requires
    // an intialized renderer!
    static NiD3DError InitializeShaderSystem(NiD3DRenderer* pkD3DRenderer);
    static NiD3DError ShutdownShaderSystem();
    // *** end Emergent internal use only ***

public:
    NiD3DShaderInterface();
    virtual ~NiD3DShaderInterface();

    virtual bool Initialize();

    // Allow for device resets
    virtual void HandleLostDevice();
    virtual void HandleResetDevice();

    // Allow for complete shader reconstruction
    virtual void DestroyRendererData();
    virtual void RecreateRendererData();

    // Query shader for presence of shader programs
    virtual bool GetVSPresentAllPasses() const;
    virtual bool GetVSPresentAnyPass() const;
    virtual bool GetPSPresentAllPasses() const;
    virtual bool GetPSPresentAnyPass() const;

    // *** begin Emergent internal use only

    virtual void ReferenceVertexDeclarationCache(
        NiVertexDeclarationCache kCache) const;
    virtual void ReleaseVertexDeclarationCache(
        NiVertexDeclarationCache kCache) const;

    // *** end Emergent internal use only

protected:
    // D3DDevice, Renderer, and RenderState access members
    D3DDevicePtr m_pkD3DDevice;
    // Use standard pointers to prevent circular dependencies
    NiD3DRenderer* m_pkD3DRenderer;
    NiDX9RenderState* m_pkD3DRenderState;
};

typedef NiPointer<NiD3DShaderInterface> NiD3DShaderInterfacePtr;

#include "NiD3DShaderInterface.inl"

#endif  //#ifndef NID3DSHADERINTERFACE_H
