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

#ifndef NID3DSHADERLIBRARYVERSION_H
#define NID3DSHADERLIBRARYVERSION_H

#include "NiD3DDefines.h"

// NiD3DShaderLibraryVersionInfo
class NID3D_ENTRY NiD3DShaderLibraryVersion : public NiMemObject
{
public:
    NiD3DShaderLibraryVersion();

    // Requested version and platform
    inline void SetSystemPixelShaderVersion(unsigned int uiVersion);
    inline void SetMinPixelShaderVersion(unsigned int uiVersion);
    inline void SetPixelShaderVersionRequest(unsigned int uiVersion);
    inline void SetSystemPixelShaderVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetMinPixelShaderVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetPixelShaderVersionRequest(unsigned int uiMaj, 
        unsigned int uiMin);
    inline void SetSystemVertexShaderVersion(unsigned int uiVersion);
    inline void SetMinVertexShaderVersion(unsigned int uiVersion);
    inline void SetVertexShaderVersionRequest(unsigned int uiVersion);
    inline void SetSystemVertexShaderVersion(unsigned int uiMaj, 
        unsigned int uiMin);
    inline void SetMinVertexShaderVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetVertexShaderVersionRequest(unsigned int uiMaj, 
        unsigned int uiMin);
    inline void SetSystemUserVersion(unsigned int uiVersion);
    inline void SetMinUserVersion(unsigned int uiVersion);
    inline void SetUserVersionRequest(unsigned int uiVersion);
    inline void SetSystemUserVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetMinUserVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetUserVersionRequest(unsigned int uiMaj, unsigned int uiMin);
    inline void SetPlatform(unsigned int uiPlatform);

    inline unsigned int GetSystemPixelShaderVersion() const;
    inline unsigned int GetMinPixelShaderVersion() const;
    inline unsigned int GetPixelShaderVersionRequest() const;
    inline unsigned int GetSystemVertexShaderVersion() const;
    inline unsigned int GetMinVertexShaderVersion() const;
    inline unsigned int GetVertexShaderVersionRequest() const;
    inline unsigned int GetSystemUserVersion() const;
    inline unsigned int GetMinUserVersion() const;
    inline unsigned int GetUserVersionRequest() const;
    inline unsigned int GetPlatform() const;

protected:
    unsigned int m_uiSystemPixelShaderVersion;
    unsigned int m_uiMinPixelShaderVersion;
    unsigned int m_uiRequestedPixelShaderVersion;
    unsigned int m_uiSystemVertexShaderVersion;
    unsigned int m_uiMinVertexShaderVersion;
    unsigned int m_uiRequestedVertexShaderVersion;
    unsigned int m_uiSystemUserVersion;
    unsigned int m_uiMinUserVersion;
    unsigned int m_uiRequestedUserVersion;
    unsigned int m_uiPlatform;
};

#include "NiD3DShaderLibraryVersion.inl"

#endif  //#ifndef NID3DSHADERLIBRARYVERSION_H
