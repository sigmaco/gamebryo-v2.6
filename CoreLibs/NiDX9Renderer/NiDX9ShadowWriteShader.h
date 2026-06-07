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


#ifndef NIDX9SHADOWWRITESHADER_H
#define NIDX9SHADOWWRITESHADER_H

#include <NiShaderDesc.h>
#include <NiMaterialDescriptor.h>
#include <NiFragmentShaderInstanceDescriptor.h>
#include "NiDX9FragmentShader.h"

class NIDX9RENDERER_ENTRY NiDX9ShadowWriteShader : 
    public NiDX9FragmentShader
{
    NiDeclareRTTI;
public:
    NiDX9ShadowWriteShader(NiMaterialDescriptor* pkDesc);
    virtual ~NiDX9ShadowWriteShader();

    virtual unsigned int PreProcessPipeline(const NiRenderCallContext& kRCC);

    static void SetRenderBackfaces(bool bRenderBackfaces);

protected:
    static bool ms_bRenderBackfaces;
    static unsigned int 
        ms_auiBackfaceCullModeMapping[NiStencilProperty::DRAW_MAX][2];

};

#include "NiDX9ShadowWriteShader.inl"

typedef NiPointer<NiDX9ShadowWriteShader> NiDX9ShadowWriteShaderPtr;

#endif  //#ifndef NIDX9SHADOWWRITESHADER_H

