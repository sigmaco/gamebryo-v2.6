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

#ifndef NID3D10SHADERPROGRAM_H
#define NID3D10SHADERPROGRAM_H

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"

#include <NiFixedString.h>
#include <NiGPUProgram.h>

class NiD3D10ShaderProgramCreator;

class NID3D10RENDERER_ENTRY NiD3D10ShaderProgram : public NiGPUProgram
{
    NiDeclareRTTI;

public:
    NiD3D10ShaderProgram(NiGPUProgram::ProgramType eType, 
        ID3D10Blob* pkShaderByteCode);
    virtual ~NiD3D10ShaderProgram();

    inline const NiFixedString& GetName() const;
    inline void SetName(const NiFixedString& kName);

    inline ID3D10Blob* GetShaderByteCode() const;
    void SetShaderByteCode(ID3D10Blob* pkShaderByteCode);

    inline NiD3D10ShaderProgramCreator* GetCreator() const;
    inline void SetCreator(NiD3D10ShaderProgramCreator* pkCreator);

    virtual void DestroyRendererData() = 0;
    virtual void RecreateRendererData() = 0;

    // Shader reflection
    void SetShaderReflection(ID3D10ShaderReflection* pkReflection,
        bool bUpdateShaderConstantInfo = true);
    inline ID3D10ShaderReflection* GetShaderReflection() const;
    inline void ClearShaderReflection();

    inline unsigned int GetConstantBufferCount() const;
    const NiFixedString GetConstantBufferName(unsigned int uiIndex) const;

    void ClearShaderReflectionData();
    void UpdateShaderReflectionData();

protected:
    NiFixedString m_kName;

    ID3D10Blob* m_pkShaderByteCode;
    NiD3D10ShaderProgramCreator* m_pkCreator;

    ID3D10ShaderReflection* m_pkReflection;

    NiFixedString* m_pkConstantBufferNames;
    unsigned int m_uiConstantBufferCount;
};

NiSmartPointer(NiD3D10ShaderProgram);

#include "NiD3D10ShaderProgram.inl"

#endif  // NID3D10SHADERPROGRAM_H
