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

#ifndef NID3D10SHADERINTERFACE_H
#define NID3D10SHADERINTERFACE_H

#include <NiShader.h>

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"

class NID3D10RENDERER_ENTRY NiD3D10ShaderInterface : public NiShader
{
    NiDeclareRTTI;

public:
    NiD3D10ShaderInterface();
    virtual ~NiD3D10ShaderInterface();

    virtual bool Initialize();

    // Allow for complete shader reconstruction
    virtual void DestroyRendererData();
    virtual void RecreateRendererData();

    // 
    virtual const D3D10_STATE_BLOCK_MASK* GetStateBlockMask(
        const NiRenderCallContext& kRCC) const;

    // *** begin Emergent internal use only

    virtual void ReferenceVertexDeclarationCache(
        NiVertexDeclarationCache kCache) const;
    virtual void ReleaseVertexDeclarationCache(
        NiVertexDeclarationCache kCache) const;

    // *** end Emergent internal use only

protected:
};

NiSmartPointer(NiD3D10ShaderInterface);

//#include "NiD3D10ShaderInterface.inl"

#endif  //#ifndef NID3D10SHADERINTERFACE_H
