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

#ifndef NID3DCGVERTEXSHADER_H
#define NID3DCGVERTEXSHADER_H

#include "NiCgShaderLibType.h"
#include "NiCgHeaders.h"

#include "NiD3DVertexShader.h"

class NICGSHADERLIB_ENTRY NiD3DCgVertexShader : public NiD3DVertexShader
{
public:
    inline NiD3DCgVertexShader(NiD3DRenderer* pkRenderer);
    virtual ~NiD3DCgVertexShader();

    virtual void DestroyRendererData();
    virtual void RecreateRendererData();

    // Shader constants
    virtual bool SetShaderConstant(NiShaderConstantMapEntry* pkEntry,
        const void* pvDataSource = NULL, unsigned int uiRegisterCount = 0);
    virtual bool SetShaderConstantArray(
        NiShaderConstantMapEntry* pkEntry, const void* pvDataSource, 
        unsigned int uiNumEntries, unsigned int uiRegistersPerEntry,
        unsigned short* pusReorderArray = NULL);

    inline virtual const char* GetEntryPoint() const;
    inline virtual void SetEntryPoint(const char* pcEntryPoint);

    inline virtual const char* GetShaderTarget() const;
    inline virtual void SetShaderTarget(const char* pcShaderTarget);

    inline virtual CGprogram GetCgProgram() const;
    inline virtual void SetCgProgram(CGprogram kProgram);

    virtual unsigned int GetVariableCount();
    virtual const char* GetVariableName(unsigned int uiIndex);
protected:
    char* m_pcEntryPoint;
    char* m_pcShaderTarget;

    CGprogram m_kCgProgram;
};

typedef NiPointer<NiD3DCgVertexShader> NiD3DCgVertexShaderPtr;

#include "NiD3DCgVertexShader.inl"

#endif  //#ifndef NID3DCGVERTEXSHADER_H
