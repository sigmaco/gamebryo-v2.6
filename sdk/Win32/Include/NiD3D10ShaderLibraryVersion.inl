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
//---------------------------------------------------------------------------
// NiD3D10ShaderLibraryVersion inline functions
//---------------------------------------------------------------------------
inline NiD3D10ShaderLibraryVersion::NiD3D10ShaderLibraryVersion() :
    m_uiSystemVertexShaderVersion(0), 
    m_uiMinVertexShaderVersion(0), 
    m_uiRequestedVertexShaderVersion(0), 
    m_uiSystemGeometryShaderVersion(0), 
    m_uiMinGeometryShaderVersion(0), 
    m_uiRequestedGeometryShaderVersion(0), 
    m_uiSystemPixelShaderVersion(0), 
    m_uiMinPixelShaderVersion(0), 
    m_uiRequestedPixelShaderVersion(0), 
    m_uiSystemUserVersion(0), 
    m_uiMinUserVersion(0), 
    m_uiRequestedUserVersion(0), 
    m_uiPlatform(0)
{
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetSystemVertexShaderVersion(
    unsigned int uiVersion)
{
    m_uiSystemVertexShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetMinVertexShaderVersion(
    unsigned int uiVersion)
{
    m_uiMinVertexShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetVertexShaderVersionRequest(
    unsigned int uiVersion)
{
    m_uiRequestedVertexShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetSystemVertexShaderVersion(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiSystemVertexShaderVersion = NiD3D10ShaderFactory::GetInstance()->
        CreateVertexShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetMinVertexShaderVersion(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiMinVertexShaderVersion = NiD3D10ShaderFactory::GetInstance()->
        CreateVertexShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetVertexShaderVersionRequest(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiRequestedVertexShaderVersion = NiD3D10ShaderFactory::GetInstance()->
        CreateVertexShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetSystemGeometryShaderVersion(
    unsigned int uiVersion)
{
    m_uiSystemGeometryShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetMinGeometryShaderVersion(
    unsigned int uiVersion)
{
    m_uiMinGeometryShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetGeometryShaderVersionRequest(
    unsigned int uiVersion)
{
    m_uiRequestedGeometryShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetSystemGeometryShaderVersion(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiSystemGeometryShaderVersion = NiD3D10ShaderFactory::GetInstance()->
        CreateGeometryShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetMinGeometryShaderVersion(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiMinGeometryShaderVersion = NiD3D10ShaderFactory::GetInstance()->
        CreateGeometryShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetGeometryShaderVersionRequest(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiRequestedGeometryShaderVersion = NiD3D10ShaderFactory::GetInstance()->
        CreateGeometryShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetSystemPixelShaderVersion(
    unsigned int uiVersion)
{
    m_uiSystemPixelShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetMinPixelShaderVersion(
    unsigned int uiVersion)
{
    m_uiMinPixelShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetPixelShaderVersionRequest(
    unsigned int uiVersion)
{
    m_uiRequestedPixelShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetSystemPixelShaderVersion(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiSystemPixelShaderVersion = NiD3D10ShaderFactory::GetInstance()->
        CreatePixelShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetMinPixelShaderVersion(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiMinPixelShaderVersion = NiD3D10ShaderFactory::GetInstance()->
        CreatePixelShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetPixelShaderVersionRequest(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiRequestedPixelShaderVersion = NiD3D10ShaderFactory::GetInstance()->
        CreatePixelShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetSystemUserVersion(
    unsigned int uiVersion)
{
    m_uiSystemUserVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetMinUserVersion(
    unsigned int uiVersion)
{
    m_uiMinUserVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetUserVersionRequest(
    unsigned int uiVersion)
{
    m_uiRequestedUserVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetSystemUserVersion(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiSystemUserVersion = NiD3D10ShaderFactory::GetInstance()->
        CreateVertexShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetMinUserVersion(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiMinUserVersion = NiD3D10ShaderFactory::GetInstance()->
        CreateVertexShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetUserVersionRequest(
    unsigned int uiMaj, unsigned int uiMin)
{
    m_uiRequestedUserVersion = NiD3D10ShaderFactory::GetInstance()->
        CreateVertexShaderVersion(uiMaj, uiMin);
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderLibraryVersion::SetPlatform(unsigned int uiPlatform)
{
    m_uiPlatform = uiPlatform;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10ShaderLibraryVersion::GetSystemVertexShaderVersion()
    const
{
    return m_uiSystemVertexShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10ShaderLibraryVersion::GetMinVertexShaderVersion() 
    const
{
    return m_uiMinVertexShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int 
    NiD3D10ShaderLibraryVersion::GetVertexShaderVersionRequest() const
{
    return m_uiRequestedVertexShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int 
    NiD3D10ShaderLibraryVersion::GetSystemGeometryShaderVersion() const
{
    return m_uiSystemGeometryShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int 
    NiD3D10ShaderLibraryVersion::GetMinGeometryShaderVersion() const
{
    return m_uiMinGeometryShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int 
    NiD3D10ShaderLibraryVersion::GetGeometryShaderVersionRequest() const
{
    return m_uiRequestedGeometryShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10ShaderLibraryVersion::GetSystemPixelShaderVersion() 
    const
{
    return m_uiSystemPixelShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10ShaderLibraryVersion::GetMinPixelShaderVersion() 
    const
{
    return m_uiMinPixelShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10ShaderLibraryVersion::GetPixelShaderVersionRequest()
    const
{
    return m_uiRequestedPixelShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10ShaderLibraryVersion::GetSystemUserVersion() const
{
    return m_uiSystemUserVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10ShaderLibraryVersion::GetMinUserVersion() const
{
    return m_uiMinUserVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10ShaderLibraryVersion::GetUserVersionRequest() const
{
    return m_uiRequestedUserVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10ShaderLibraryVersion::GetPlatform() const
{
    return m_uiPlatform;
}
//---------------------------------------------------------------------------

