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

#ifndef NID3D10GEOMETRYSHADER_H
#define NID3D10GEOMETRYSHADER_H

#include "NiD3D10ShaderProgram.h"
#include "NiD3D10Headers.h"

class NID3D10RENDERER_ENTRY NiD3D10GeometryShader : 
    public NiD3D10ShaderProgram
{
    NiDeclareRTTI;

public:
    NiD3D10GeometryShader();
    NiD3D10GeometryShader(ID3D10GeometryShader* pkGeometryShader, 
        ID3D10Blob* pkShaderByteCode);
    virtual ~NiD3D10GeometryShader();

    inline ID3D10GeometryShader* GetGeometryShader() const;
    void SetGeometryShader(ID3D10GeometryShader* pkGeometryShader);

    virtual void DestroyRendererData();
    virtual void RecreateRendererData();

    // Stream output
    void SetStreamOutputDeclaration(
        const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
        unsigned int uiNumEntries, unsigned int uiOutputStreamStride);

    inline const D3D10_SO_DECLARATION_ENTRY* GetStreamOutputDeclaration() const;
    inline unsigned int GetNumStreamOutputEntries() const;
    inline unsigned int GetOutputStreamStride() const;

protected:
    ID3D10GeometryShader* m_pkGeometryShader;

    D3D10_SO_DECLARATION_ENTRY* m_pkSODeclaration;
    unsigned int m_uiNumSOEntries;
    unsigned int m_uiOutputStreamStride;
};

NiSmartPointer(NiD3D10GeometryShader);

#include "NiD3D10GeometryShader.inl"

#endif // NID3D10GEOMETRYSHADER_H