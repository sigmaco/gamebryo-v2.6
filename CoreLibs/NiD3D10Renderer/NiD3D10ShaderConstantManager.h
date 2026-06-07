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

#ifndef NID3D10SHADERCONSTANTMANAGER_H
#define NID3D10SHADERCONSTANTMANAGER_H

#include <NiMemObject.h>

#include "NiD3D10RendererLibType.h"
#include "NiD3D10DeviceState.h"
#include "NiD3D10Headers.h"

class NiD3D10ShaderConstantMap;
class NiD3D10ShaderProgram;

class NID3D10RENDERER_ENTRY NiD3D10ShaderConstantManager : public NiMemObject
{
public:
    NiD3D10ShaderConstantManager(NiD3D10DeviceState* pkDeviceState);
    virtual ~NiD3D10ShaderConstantManager();
    inline void ResetConstantBuffers();
    // Accumulate constant buffers
    void SetShaderConstantMap(NiD3D10ShaderProgram* pkShaderProgram, 
        NiD3D10ShaderConstantMap* pkConstantMap);
    // Set/Get accumulated constant buffers
    void SetVertexShaderConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, 
        ID3D10Buffer*const* ppkConstantBuffers);
    void GetVertexShaderConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, 
        ID3D10Buffer** ppkConstantBuffers) const;
    void ClearVertexShaderConstantBuffers();
    void SetGeometryShaderConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, 
        ID3D10Buffer*const* ppkConstantBuffers);
    void GetGeometryShaderConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, 
        ID3D10Buffer** ppkConstantBuffers) const;
    void ClearGeometryShaderConstantBuffers();
    void SetPixelShaderConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, 
        ID3D10Buffer*const* ppkConstantBuffers);
    void GetPixelShaderConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, 
        ID3D10Buffer** ppkConstantBuffers) const;
    void ClearPixelShaderConstantBuffers();
    void ApplyShaderConstants();

protected:
    NiD3D10DeviceState* m_pkDeviceState;
    ID3D10Buffer* m_apkVertexConstantBuffers
        [D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT];
    ID3D10Buffer* m_apkGeometryConstantBuffers
        [D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT];
    ID3D10Buffer* m_apkPixelConstantBuffers
        [D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT];
};

#include "NiD3D10ShaderConstantManager.inl"

#endif // NID3D10SHADERCONSTANTMANAGER_H