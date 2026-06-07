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

#ifndef NIDX9ERRORSHADER_H
#define NIDX9ERRORSHADER_H

#include "NiD3DShader.h"

class NID3D_ENTRY NiDX9ErrorShader : public NiD3DShader
{
    NiDeclareRTTI;
public:
    NiDX9ErrorShader();
    virtual ~NiDX9ErrorShader();

    virtual bool Initialize();

    // Override these functions to implement a custom pipeline...
    virtual unsigned int UpdatePipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int SetupTransformations(const NiRenderCallContext& kRCC);

protected:
    bool CreateStagesAndPasses();
    bool CreateShaderDeclaration();

    NiD3DPass* m_pkPass;
};

typedef NiPointer<NiDX9ErrorShader> NiDX9ErrorShaderPtr;

#endif  //#ifndef NIDX9ERRORSHADER_H
