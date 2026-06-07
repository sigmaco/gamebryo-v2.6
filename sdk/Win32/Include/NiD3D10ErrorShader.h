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

#ifndef NID3D10ERRORSHADER_H
#define NID3D10ERRORSHADER_H

#include "NiD3D10Shader.h"

class NID3D10RENDERER_ENTRY NiD3D10ErrorShader : public NiD3D10Shader
{
    NiDeclareRTTI;
public:
    NiD3D10ErrorShader();
    virtual ~NiD3D10ErrorShader();

    virtual bool Initialize();

    // Override these functions to implement a custom pipeline...
    virtual unsigned int SetupTransformations(const NiRenderCallContext& kRCC);

protected:
    bool CreateStagesAndPasses();
    bool CreateShaderDeclaration();
    bool CreateShaders();

    NiD3D10Pass* m_pkPass;

    D3DXVECTOR4 m_kMaterialColor;
};

typedef NiPointer<NiD3D10ErrorShader> NiD3D10ErrorShaderPtr;

#endif  //#ifndef NID3D10ERRORSHADER_H
