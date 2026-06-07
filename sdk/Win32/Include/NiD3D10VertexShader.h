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

#ifndef NID3D10VERTEXSHADER_H
#define NID3D10VERTEXSHADER_H

#include "NiD3D10ShaderProgram.h"
#include "NiD3D10Headers.h"

class NID3D10RENDERER_ENTRY NiD3D10VertexShader : public NiD3D10ShaderProgram
{
    NiDeclareRTTI;

public:
    NiD3D10VertexShader();
    NiD3D10VertexShader(ID3D10VertexShader* pkVertexShader, 
        ID3D10Blob* pkShaderByteCode);
    virtual ~NiD3D10VertexShader();

    ID3D10VertexShader* GetVertexShader() const;
    void SetVertexShader(ID3D10VertexShader* pkPixelShader);

    virtual void DestroyRendererData();
    virtual void RecreateRendererData();

protected:
    ID3D10VertexShader* m_pkVertexShader;
};

NiSmartPointer(NiD3D10VertexShader);

//#include "NiD3D10VertexShader.inl"

#endif // NID3D10VERTEXSHADER_H