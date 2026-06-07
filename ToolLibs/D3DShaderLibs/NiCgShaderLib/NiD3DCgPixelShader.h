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

#ifndef NID3DCGPIXELSHADER_H
#define NID3DCGPIXELSHADER_H

#include "NiCgShaderLibType.h"
#include "NiCgHeaders.h"

#include "NiD3DPixelShader.h"

class NICGSHADERLIB_ENTRY NiD3DCgPixelShader : public NiD3DPixelShader
{
public:
    inline NiD3DCgPixelShader(NiD3DRenderer* pkRenderer);
    virtual ~NiD3DCgPixelShader();

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

typedef NiPointer<NiD3DCgPixelShader> NiD3DCgPixelShaderPtr;

#include "NiD3DCgPixelShader.inl"

#endif  //#ifndef NID3DCGPIXELSHADER_H
