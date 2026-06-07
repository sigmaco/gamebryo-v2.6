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
// NSBD3D10Shader inline functions
//---------------------------------------------------------------------------
inline NSBD3D10Shader::CREATENID3D10BINARYSHADER 
NSBD3D10Shader::SetCreateNiD3D10BinaryShaderCallback(
    NSBD3D10Shader::CREATENID3D10BINARYSHADER pfnCallback)
{
    CREATENID3D10BINARYSHADER pfnOld = ms_pfnCreateNiD3D10BinaryShader;

    if (pfnCallback)
        ms_pfnCreateNiD3D10BinaryShader = pfnCallback;
    else
        ms_pfnCreateNiD3D10BinaryShader = DefaultCreateNiD3D10BinaryShader;

    return pfnOld;
}
//---------------------------------------------------------------------------
inline NSBD3D10Shader::CREATENID3D10BINARYSHADER 
NSBD3D10Shader::GetCreateNiD3D10BinaryShaderCallback()
{
    return ms_pfnCreateNiD3D10BinaryShader;
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10Shader::GetName()
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10Shader::GetDescription()
{
    return m_pcDescription;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Shader::GetMinPixelShaderVersion()
{
    return m_uiMinPixelShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Shader::GetMaxPixelShaderVersion()
{
    return m_uiMaxPixelShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Shader::GetMinVertexShaderVersion()
{
    return m_uiMinVertexShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Shader::GetMaxVertexShaderVersion()
{
    return m_uiMaxVertexShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Shader::GetMinUserVersion()
{
    return m_uiMinUserVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Shader::GetMaxUserVersion()
{
    return m_uiMaxUserVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Shader::GetPlatform()
{
    return m_uiPlatform;
}
//---------------------------------------------------------------------------
inline NSBD3D10AttributeTable* NSBD3D10Shader::GetGlobalAttributeTable()
{
    return &m_kGlobalAttribTable;
}
//---------------------------------------------------------------------------
inline NSBD3D10AttributeTable* NSBD3D10Shader::GetAttributeTable()
{
    return &m_kAttribTable;
}
//---------------------------------------------------------------------------
inline NSBD3D10ObjectTable* NSBD3D10Shader::GetObjectTable()
{
    return &m_kObjectTable;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::SetMinPixelShaderVersionRequest(
    unsigned int uiVersion)
{
    m_uiMinPixelShaderVersionRequest = uiVersion;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::SetMaxPixelShaderVersionRequest(
    unsigned int uiVersion)
{
    m_uiMaxPixelShaderVersionRequest = uiVersion;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::SetMinVertexShaderVersionRequest(
    unsigned int uiVersion)
{
    m_uiMinVertexShaderVersionRequest = uiVersion;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::SetMaxVertexShaderVersionRequest(
    unsigned int uiVersion)
{
    m_uiMaxVertexShaderVersionRequest = uiVersion;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::SetMinUserVersionRequest(unsigned int uiVersion)
{
    m_uiMinUserVersionRequest = uiVersion;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::SetMaxUserVersionRequest(unsigned int uiVersion)
{
    m_uiMaxUserVersionRequest = uiVersion;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::AddPlatform(unsigned int uiPlatformFlag)
{
    m_uiPlatform |= uiPlatformFlag;
}
//---------------------------------------------------------------------------
/***
inline void NSBD3D10Shader::RemovePlatform(unsigned int uiPlatformFlag)
{
    m_uiPlatform &= ~uiPlatformFlag;
}
***/
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::SetPlatformRequest(unsigned int uiPlatform)
{
    m_uiPlatformRequest = uiPlatform;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::AddVertexShaderVersion(unsigned int uiVersion)
{
    if (m_uiMinVertexShaderVersion > uiVersion)
        m_uiMinVertexShaderVersion = uiVersion;
    if (m_uiMaxVertexShaderVersion < uiVersion)
        m_uiMaxVertexShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::AddGeometryShaderVersion(unsigned int uiVersion)
{
    if (m_uiMinGeometryShaderVersion > uiVersion)
        m_uiMinGeometryShaderVersion = uiVersion;
    if (m_uiMaxGeometryShaderVersion < uiVersion)
        m_uiMaxGeometryShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::AddPixelShaderVersion(unsigned int uiVersion)
{
    if (m_uiMinPixelShaderVersion > uiVersion)
        m_uiMinPixelShaderVersion = uiVersion;
    if (m_uiMaxPixelShaderVersion < uiVersion)
        m_uiMaxPixelShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::AddUserVersion(unsigned int uiVersion)
{
    if (m_uiMinUserVersion > uiVersion)
        m_uiMinUserVersion = uiVersion;
    if (m_uiMaxUserVersion < uiVersion)
        m_uiMaxUserVersion = uiVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Shader::GetReadVersion()
{
    return ms_uiReadVersion;
}
//---------------------------------------------------------------------------
inline NSBD3D10UserDefinedDataSet* NSBD3D10Shader::GetUserDefinedDataSet()
{
    return m_spUserDefinedDataSet;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::SetUserDefinedDataSet(
    NSBD3D10UserDefinedDataSet* pkUDDSet)
{
    m_spUserDefinedDataSet = pkUDDSet;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::AddOutputStreamDescriptor(
    const NiOutputStreamDescriptor& kDescriptor)
{
    m_kOutputStreamDescriptors.Add(kDescriptor);
}
//---------------------------------------------------------------------------
inline void NSBD3D10Shader::ClearOutputStreamDescriptors()
{
    m_kOutputStreamDescriptors.RemoveAll();
}
//---------------------------------------------------------------------------
inline const NiOutputStreamDescriptorArray& 
NSBD3D10Shader::GetOutputStreamDescriptors()
{
    return m_kOutputStreamDescriptors;
}
//---------------------------------------------------------------------------
