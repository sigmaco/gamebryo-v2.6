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

#ifndef NID3D10SHADERLIBRARYVERSION_H
#define NID3D10SHADERLIBRARYVERSION_H


#include "NiD3D10ShaderFactory.h"

// NiD3D10ShaderLibraryVersionInfo
class NID3D10RENDERER_ENTRY NiD3D10ShaderLibraryVersion : public NiMemObject
{
public:
    NiD3D10ShaderLibraryVersion();

    // Requested version and platform
    inline void SetSystemVertexShaderVersion(unsigned int uiVersion);
    inline void SetMinVertexShaderVersion(unsigned int uiVersion);
    inline void SetVertexShaderVersionRequest(unsigned int uiVersion);
    inline void SetSystemVertexShaderVersion(unsigned int uiMaj, 
        unsigned int uiMin);
    inline void SetMinVertexShaderVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetVertexShaderVersionRequest(unsigned int uiMaj, 
        unsigned int uiMin);

    inline void SetSystemGeometryShaderVersion(unsigned int uiVersion);
    inline void SetMinGeometryShaderVersion(unsigned int uiVersion);
    inline void SetGeometryShaderVersionRequest(unsigned int uiVersion);
    inline void SetSystemGeometryShaderVersion(unsigned int uiMaj, 
        unsigned int uiMin);
    inline void SetMinGeometryShaderVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetGeometryShaderVersionRequest(unsigned int uiMaj, 
        unsigned int uiMin);

    inline void SetSystemPixelShaderVersion(unsigned int uiVersion);
    inline void SetMinPixelShaderVersion(unsigned int uiVersion);
    inline void SetPixelShaderVersionRequest(unsigned int uiVersion);
    inline void SetSystemPixelShaderVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetMinPixelShaderVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetPixelShaderVersionRequest(unsigned int uiMaj, 
        unsigned int uiMin);

    inline void SetSystemUserVersion(unsigned int uiVersion);
    inline void SetMinUserVersion(unsigned int uiVersion);
    inline void SetUserVersionRequest(unsigned int uiVersion);
    inline void SetSystemUserVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetMinUserVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetUserVersionRequest(unsigned int uiMaj, unsigned int uiMin);

    inline void SetPlatform(unsigned int uiPlatform);

    inline unsigned int GetSystemVertexShaderVersion() const;
    inline unsigned int GetMinVertexShaderVersion() const;
    inline unsigned int GetVertexShaderVersionRequest() const;

    inline unsigned int GetSystemGeometryShaderVersion() const;
    inline unsigned int GetMinGeometryShaderVersion() const;
    inline unsigned int GetGeometryShaderVersionRequest() const;

    inline unsigned int GetSystemPixelShaderVersion() const;
    inline unsigned int GetMinPixelShaderVersion() const;
    inline unsigned int GetPixelShaderVersionRequest() const;

    inline unsigned int GetSystemUserVersion() const;
    inline unsigned int GetMinUserVersion() const;
    inline unsigned int GetUserVersionRequest() const;
    inline unsigned int GetPlatform() const;

protected:
    unsigned int m_uiSystemVertexShaderVersion;
    unsigned int m_uiMinVertexShaderVersion;
    unsigned int m_uiRequestedVertexShaderVersion;

    unsigned int m_uiSystemGeometryShaderVersion;
    unsigned int m_uiMinGeometryShaderVersion;
    unsigned int m_uiRequestedGeometryShaderVersion;

    unsigned int m_uiSystemPixelShaderVersion;
    unsigned int m_uiMinPixelShaderVersion;
    unsigned int m_uiRequestedPixelShaderVersion;

    unsigned int m_uiSystemUserVersion;
    unsigned int m_uiMinUserVersion;
    unsigned int m_uiRequestedUserVersion;
    unsigned int m_uiPlatform;
};

#include "NiD3D10ShaderLibraryVersion.inl"

#endif  //#ifndef NID3D10SHADERLIBRARYVERSION_H
