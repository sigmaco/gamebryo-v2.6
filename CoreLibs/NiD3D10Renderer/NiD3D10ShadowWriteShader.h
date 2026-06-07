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


#ifndef NID3D10SHADOWWRITESHADER_H
#define NID3D10SHADOWWRITESHADER_H

#include <NiShaderDesc.h>
#include <NiMaterialDescriptor.h>
#include <NiFragmentShaderInstanceDescriptor.h>
#include <NiStencilProperty.h>
#include "NiD3D10FragmentShader.h"

class NID3D10RENDERER_ENTRY NiD3D10ShadowWriteShader : 
    public NiD3D10FragmentShader
{
    NiDeclareRTTI;
public:
    NiD3D10ShadowWriteShader(NiMaterialDescriptor* pkDesc);
    virtual ~NiD3D10ShadowWriteShader();

    virtual unsigned int PreProcessPipeline(const NiRenderCallContext& kRCC);

    static void SetRenderBackfaces(bool bRenderBackfaces);

protected:
    static bool ms_bRenderBackfaces;
};

#include "NiD3D10ShadowWriteShader.inl"

typedef NiPointer<NiD3D10ShadowWriteShader> NiD3D10ShadowWriteShaderPtr;

#endif  //#ifndef NID3D10SHADOWWRITESHADER_H

