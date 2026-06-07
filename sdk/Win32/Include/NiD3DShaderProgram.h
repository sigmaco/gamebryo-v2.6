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

#ifndef NID3DSHADERPROGRAM_H
#define NID3DSHADERPROGRAM_H

#include <NiSmartPointer.h>
#include <NiRefObject.h>
#include <NiGPUProgram.h>
#include "NiD3DDefines.h"

class NiShaderConstantMapEntry;
class NiD3DShaderProgramCreator;

// 
class NID3D_ENTRY NiD3DShaderProgram : public NiGPUProgram
{
    NiDeclareRTTI;

    // *** begin Emergent internal use only ***
protected:
    void SetD3DDevice(D3DDevicePtr pkD3DDevice);
    void SetD3DRenderState(NiD3DRenderState* pkRS);
public:
    inline void SetD3DRenderer(NiD3DRenderer* pkD3DRenderer);
    // *** end Emergent internal use only ***

public:
    NiD3DShaderProgram(NiD3DRenderer* pkRenderer,
        NiGPUProgram::ProgramType eType);
    virtual ~NiD3DShaderProgram();

    // Access functions
    virtual const char* GetName() const;
    virtual void SetName(const char* pszName);

    virtual const char* GetShaderProgramName() const;
    virtual void SetShaderProgramName(const char* pszName);

    virtual unsigned int GetCodeSize() const;
    virtual void* GetCode() const;
    virtual void SetCode(unsigned int uiSize, void* pvCode);

    virtual NiD3DShaderProgramCreator* GetCreator() const;
    virtual void SetCreator(NiD3DShaderProgramCreator* pkCreator);

    //*** Helper functions
    virtual bool SetShaderConstant(
        NiShaderConstantMapEntry* pkEntry,
        const void* pvDataSource = NULL, 
        unsigned int uiRegisterCount = 0);
    virtual bool SetShaderConstantArray(
        NiShaderConstantMapEntry* pkEntry, 
        const void* pvDataSource, 
        unsigned int uiNumEntries, 
        unsigned int uiRegistersPerEntry,
        const unsigned short* pusReorderArray = NULL);
protected:
    char* m_pszName;

    char* m_pszShaderProgramName;
    unsigned int m_uiCodeSize;
    void* m_pvCode;
    NiD3DShaderProgramCreator* m_pkCreator;

    D3DDevicePtr m_pkD3DDevice;
    NiD3DRenderer* m_pkD3DRenderer;
    NiD3DRenderState* m_pkD3DRenderState;
};

typedef NiPointer<NiD3DShaderProgram> NiD3DShaderProgramPtr;

#include "NiD3DShaderProgram.inl"

#endif  //#ifndef NID3DSHADERPROGRAM_H
